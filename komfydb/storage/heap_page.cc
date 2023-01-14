#include "komfydb/storage/heap_page.h"

#include <arpa/inet.h>

#include "komfydb/common/int_field.h"
#include "komfydb/common/string_field.h"
#include "komfydb/config.h"
#include "komfydb/utils/status_macros.h"

#include "absl/strings/str_cat.h"

namespace {

using namespace komfydb::common;

absl::StatusOr<bool> TuplePresent(std::vector<uint8_t>& header, int i) {
  if (i / 8 >= header.size() || i < 0)
    return absl::InvalidArgumentError("Index out of range");
  return header[i / 8] & (1 << (i % 8));
}

absl::Status MarkOccupied(std::vector<uint8_t>& header, int i) {
  if (i / 8 >= header.size() || i < 0)
    return absl::InvalidArgumentError("Index out of range");
  header[i / 8] |= (1 << (i % 8));
  return absl::OkStatus();
}

absl::Status MarkFree(std::vector<uint8_t>& header, int i) {
  if (i / 8 >= header.size() || i < 0)
    return absl::InvalidArgumentError("Index out of range");
  header[i / 8] &= ~(1 << (i % 8));
  return absl::OkStatus();
}

int ParseInt(uint8_t* data) {
  int result = *reinterpret_cast<int*>(data);
  return ntohl(result);
}

std::unique_ptr<IntField> ParseInt(std::vector<uint8_t>& data, int& data_idx) {
  std::unique_ptr<IntField> field =
      std::unique_ptr<IntField>(new IntField(ParseInt(data.data() + data_idx)));
  data_idx += Type::INT_LEN;
  return field;
}

std::unique_ptr<StringField> ParseString(std::vector<uint8_t>& data,
                                         int& data_idx) {
  int string_len = ParseInt(&data[data_idx]);
  data_idx += 4;
  std::string s((char*)&data[data_idx]);
  s = s.substr(0, string_len);
  std::unique_ptr<StringField> value = std::make_unique<StringField>(s);
  data_idx += Type::STR_LEN;
  return value;
}

void DumpInt(int data, std::vector<uint8_t>& result) {
  // We want data to be stored in big-endian.
  data = htonl(data);
  uint8_t* bytes = reinterpret_cast<uint8_t*>(&data);
  result.insert(result.end(), bytes, bytes + sizeof(data));
}

void DumpString(Field* field, std::vector<uint8_t>& result) {
  std::string data;
  field->GetValue(data);
  int padding = Type::STR_LEN - data.size();
  DumpInt(data.size(), result);
  result.insert(result.end(), data.begin(), data.end());
  result.insert(result.end(), padding, '\0');
}

void DumpInt(Field* field, std::vector<uint8_t>& result) {
  int data;
  field->GetValue(data);
  DumpInt(data, result);
}

}  // namespace

namespace komfydb::storage {

HeapPage::HeapPage(PageId pid, TupleDesc* tuple_desc,
                   std::vector<uint8_t> header, std::vector<Record> records,
                   int num_slots)
    : header(header),
      records(std::move(records)),
      last_transaction(transaction::NO_TID),
      tuple_desc(tuple_desc),
      pid(pid),
      num_slots(num_slots),
      is_dirty(false) {}

absl::StatusOr<std::unique_ptr<HeapPage>> HeapPage::Create(
    PageId pid, TupleDesc* tuple_desc, std::vector<uint8_t>& data) {
  int n_slots = (CONFIG_PAGE_SIZE * 8) / (tuple_desc->GetSize() * 8 + 1);
  int header_len = (n_slots + 7) / 8;
  int n_fields = tuple_desc->Length();
  int data_idx = header_len;
  std::vector<uint8_t> header;
  std::vector<Record> records;

  header.insert(header.end(), data.begin(), data.begin() + header_len);
  for (int i = 0; i < n_slots; i++) {
    Record record(tuple_desc, pid, i);

    if (!TuplePresent(header, i).value()) {
      data_idx += tuple_desc->GetSize();
    } else {
      for (int j = 0; j < n_fields; j++) {
        ASSIGN_OR_RETURN(Type field_type, tuple_desc->GetFieldType(j));

        if (field_type.GetValue() == Type::INT) {
          RETURN_IF_ERROR(record.SetField(j, ParseInt(data, data_idx)));
        } else if (field_type.GetValue() == Type::STRING) {
          RETURN_IF_ERROR(record.SetField(j, ParseString(data, data_idx)));
        }
      }
    }

    records.push_back(std::move(record));
  }

  return std::unique_ptr<HeapPage>(
      new HeapPage(pid, tuple_desc, header, std::move(records), n_slots));
}

PageId HeapPage::GetId() {
  return pid;
}

TransactionId HeapPage::GetLastTransaction() {
  return last_transaction;
}

bool HeapPage::IsDirty() {
  return is_dirty;
}

void HeapPage::SetDirty(bool dirty, TransactionId tid) {
  last_transaction = tid;
  is_dirty = true;
}

absl::StatusOr<std::vector<uint8_t>> HeapPage::GetPageData() {
  // TODO(transactions) Not necessary for now, also requires prior call to
  // `SetBeforeImage`.
  // if (!is_dirty) {
  //   return old_data;
  // }

  std::vector<uint8_t> result = header;
  int tuple_len = tuple_desc->Length();

  for (int i = 0; i < num_slots; i++) {
    ASSIGN_OR_RETURN(bool tuple_present, TuplePresent(header, i));
    if (!tuple_present) {
      result.insert(result.end(), tuple_desc->GetSize(), '\0');
      continue;
    }

    Record& record = records[i];

    for (int j = 0; j < tuple_len; j++) {
      ASSIGN_OR_RETURN(Type field_type, tuple_desc->GetFieldType(j));
      ASSIGN_OR_RETURN(Field * field, record.GetField(j));

      if (field_type.GetValue() == Type::STRING) {
        DumpString(field, result);
      } else if (field_type.GetValue() == Type::INT) {
        DumpInt(field, result);
      }
    }
  }
  result.insert(result.end(), CONFIG_PAGE_SIZE - result.size(), '\0');
  return result;
}

absl::StatusOr<std::unique_ptr<Page>> HeapPage::GetBeforeImage() {
  absl::MutexLock l(&old_data_lock);
  ASSIGN_OR_RETURN(std::unique_ptr<HeapPage> result,
                   Create(pid, tuple_desc, old_data));
  return result;
}

absl::Status HeapPage::SetBeforeImage() {
  absl::MutexLock l(&old_data_lock);
  ASSIGN_OR_RETURN(old_data, GetPageData());
  return absl::OkStatus();
}

std::vector<Record> HeapPage::GetRecords() {
  std::vector<Record> result;
  for (int i = 0; i < records.size(); i++) {
    if (TuplePresent(header, i).value()) {
      result.push_back(Record(records[i]));
    }
  }

  return result;
}

absl::Status HeapPage::AddTuple(Tuple& t) {
  for (int i = 0; i < num_slots; i++) {
    // TODO: find first 0 bit faster
    ASSIGN_OR_RETURN(bool occupied, TuplePresent(header, i));
    if (!occupied) {
      RETURN_IF_ERROR(MarkOccupied(header, i));
      records[i] = Record(t, RecordId(pid, i));
      return absl::OkStatus();
    }
  }
  return absl::FailedPreconditionError(
      absl::StrCat(std::string(pid), " has no space to add a new record."));
}

absl::Status HeapPage::RemoveRecord(RecordId& rid) {
  if (rid.GetTupleNumber() < 0 || rid.GetTupleNumber() >= num_slots) {
    return absl::OutOfRangeError(absl::StrCat(
        std::string(rid),
        " out of range [num_slots=", std::to_string(num_slots), "]"));
  }
  ASSIGN_OR_RETURN(bool occupied, TuplePresent(header, rid.GetTupleNumber()));
  if (rid.GetPageId() != pid || !occupied) {
    return absl::InvalidArgumentError(
        absl::StrCat(std::string(pid), " does not contain record with rid ",
                     std::string(rid)));
  }
  RETURN_IF_ERROR(MarkFree(header, rid.GetTupleNumber()));
  return absl::OkStatus();
}

};  // namespace komfydb::storage
