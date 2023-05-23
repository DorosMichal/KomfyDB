#include "komfydb/storage/heap_page.h"

#include <arpa/inet.h>

#include "komfydb/common/int_field.h"
#include "komfydb/common/string_field.h"
#include "komfydb/config.h"
#include "komfydb/utils/status_macros.h"

#include "absl/strings/str_cat.h"

namespace {

using namespace komfydb::common;

bool TuplePresent(std::vector<uint8_t>& header, int i) {
  return header[i / 8] & (1 << (i % 8));
}

void MarkOccupied(std::vector<uint8_t>& header, int i) {
  header[i / 8] |= (1 << (i % 8));
}

void MarkFree(std::vector<uint8_t>& header, int i) {
  header[i / 8] &= ~(1 << (i % 8));
}

inline uint32_t load32_le(uint8_t const* V) {
  uint32_t ret = 0;
  ret |= (uint32_t)V[0];
  ret |= ((uint32_t)V[1]) << 8;
  ret |= ((uint32_t)V[2]) << 16;
  ret |= ((uint32_t)V[3]) << 24;
  return ret;
}

int ParseInt(uint8_t* data) {
  // We need to use this custom function as the data in a heap page can be
  // unaligned! This is due to how we compute the header length. It can be
  // arbitrary, and the data starts right after it, so it can be that the first
  // integer starts at byte 7th for example. Thus, we cannot simply cast the
  // uint8_t* data to uint32_t*
  //
  // Fortunatelly, it seems that x86_64 and arm64 supports reading from
  // unaligned memory and that the clang compiler optimizes the function
  // to simply do a move on these architectures.
  int result = load32_le(data);
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

void DumpString(StringField* field, std::vector<uint8_t>& result) {
  std::string data = field->GetValue();
  int padding = Type::STR_LEN - data.size();
  DumpInt(data.size(), result);
  result.insert(result.end(), data.begin(), data.end());
  result.insert(result.end(), padding, '\0');
}

void DumpInt(IntField* field, std::vector<uint8_t>& result) {
  int data = field->GetValue();
  DumpInt(data, result);
}

}  // namespace

namespace komfydb::storage {

HeapPage::HeapPage(PageId pid, TupleDesc* tuple_desc,
                   std::vector<uint8_t> header, std::vector<Record> records,
                   int num_slots, int free_space)
    : header(header),
      records(std::move(records)),
      last_transaction(transaction::NO_TID),
      tuple_desc(tuple_desc),
      pid(pid),
      num_slots(num_slots),
      free_space(free_space),
      is_dirty(false) {}

absl::StatusOr<std::unique_ptr<HeapPage>> HeapPage::Create(
    PageId pid, TupleDesc* tuple_desc, std::vector<uint8_t>& data) {
  int n_slots = (CONFIG_PAGE_SIZE * 8) / (tuple_desc->GetSize() * 8 + 1);
  int free_space = 0;
  int header_len = (n_slots + 7) / 8;
  int n_fields = tuple_desc->Length();
  int data_idx = header_len;
  std::vector<uint8_t> header;
  std::vector<Record> records;

  header.insert(header.end(), data.begin(), data.begin() + header_len);
  for (int i = 0; i < n_slots; i++) {
    Record record(tuple_desc->Length(), pid, i);

    if (!TuplePresent(header, i)) {
      data_idx += tuple_desc->GetSize();
      free_space++;
    } else {
      for (int j = 0; j < n_fields; j++) {
        Type field_type = tuple_desc->GetFieldType(j);
        std::unique_ptr<Field> parsed;
        switch (field_type.GetValue()) {
          case Type::INT: {
            parsed = ParseInt(data, data_idx);
            break;
          }
          case Type::STRING: {
            parsed = ParseString(data, data_idx);
            break;
          }
        }
        record.SetField(j, std::move(parsed));
      }
    }

    records.push_back(std::move(record));
  }

  return std::unique_ptr<HeapPage>(new HeapPage(
      pid, tuple_desc, header, std::move(records), n_slots, free_space));
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

std::vector<uint8_t> HeapPage::GetPageData() {
  // TODO(transactions) Not necessary for now, also requires prior call to
  // `SetBeforeImage`.
  // if (!is_dirty) {
  //   return old_data;
  // }

  std::vector<uint8_t> result = header;
  int tuple_len = tuple_desc->Length();

  for (int i = 0; i < num_slots; i++) {
    if (!TuplePresent(header, i)) {
      result.insert(result.end(), tuple_desc->GetSize(), '\0');
      continue;
    }

    Record& record = records[i];

    for (int j = 0; j < tuple_len; j++) {
      Type field_type = tuple_desc->GetFieldType(j);
      Field* field = record.GetField(j);

      switch (field_type.GetValue()) {
        case Type::STRING: {
          DumpString(static_cast<StringField*>(field), result);
          break;
        }
        case Type::INT: {
          DumpInt(static_cast<IntField*>(field), result);
          break;
        }
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

void HeapPage::SetBeforeImage() {
  absl::MutexLock l(&old_data_lock);
  old_data = GetPageData();
}

std::vector<Record> HeapPage::GetRecords() {
  std::vector<Record> result;
  for (int i = 0; i < records.size(); i++) {
    if (TuplePresent(header, i)) {
      result.push_back(Record(records[i]));
    }
  }

  return result;
}

absl::Status HeapPage::AddTuples(std::unique_ptr<Tuple> tuples[], int num) {
  if (num > free_space) {
    return absl::FailedPreconditionError(
        absl::StrCat(std::string(pid), " has no space to add ",
                     std::to_string(num), " tuples."));
  }
  for (int i = 0, j = 0; i < num_slots && j < num; i++) {
    // TODO: find first 0 bit faster
    // TODO(Transactions): invalidate transaction on error
    if (!TuplePresent(header, i)) {
      MarkOccupied(header, i);
      records[i] = Record(*tuples[j], RecordId(pid, i));
      j++;
    }
  }
  free_space -= num;
  return absl::OkStatus();
}

absl::Status HeapPage::RemoveRecord(RecordId& rid) {
  if (rid.GetTupleNumber() < 0 || rid.GetTupleNumber() >= num_slots) {
    return absl::OutOfRangeError(absl::StrCat(
        std::string(rid),
        " out of range [num_slots=", std::to_string(num_slots), "]"));
  }
  if (rid.GetPageId() != pid || !TuplePresent(header, rid.GetTupleNumber())) {
    return absl::InvalidArgumentError(
        absl::StrCat(std::string(pid), " does not contain record with rid ",
                     std::string(rid)));
  }
  MarkFree(header, rid.GetTupleNumber());
  free_space++;
  return absl::OkStatus();
}

int HeapPage::GetFreeSpace() {
  return free_space;
}

};  // namespace komfydb::storage
