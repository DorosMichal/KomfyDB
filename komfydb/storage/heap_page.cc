#include "komfydb/storage/heap_page.h"
#include "komfydb/common/int_field.h"
#include "komfydb/common/string_field.h"
#include "komfydb/config.h"
#include "komfydb/utils/status_macros.h"

namespace {

using namespace komfydb::common;

absl::StatusOr<bool> TuplePresent(std::vector<uint8_t>& header, int i) {
  if (i / 8 >= header.size() || i < 0)
    return absl::InvalidArgumentError("Index out of range");
  return header[i / 8] & (1 << (i % 8));
}

std::unique_ptr<IntField> ParseInt(std::vector<uint8_t>& data, int& data_idx) {
  std::unique_ptr<IntField> field =
      std::make_unique<IntField>(*((int*)&data[data_idx]));
  data_idx += Type::INT_LEN;
  return field;
}

std::unique_ptr<StringField> ParseString(std::vector<uint8_t>& data,
                                         int& data_idx) {
  std::unique_ptr<StringField> value =
      std::make_unique<StringField>((char*)&data[data_idx]);
  data_idx += Type::STR_LEN;
  return value;
}

void DumpString(Field* field, std::vector<uint8_t>& result) {
  std::string data;
  field->GetValue(data);
  int padding = Type::STR_LEN - data.size();
  result.insert(result.end(), data.begin(), data.end());
  result.insert(result.end(), padding, '\0');
}

void DumpInt(Field* field, std::vector<uint8_t>& result) {
  int data;
  field->GetValue(data);
  uint8_t* bytes = reinterpret_cast<uint8_t*>(&data);
  result.insert(result.end(), bytes, bytes + sizeof(data));
}

}  // namespace

namespace komfydb::storage {

absl::StatusOr<std::unique_ptr<HeapPage>> HeapPage::Create(
    PageId id, TupleDesc* td, std::vector<uint8_t>& data) {
  int n_slots = (CONFIG_PAGE_SIZE * 8) / (td->GetSize() * 8 + 1);
  int header_len = (n_slots + 7) / 8;
  int n_fields = td->Length();
  int data_idx = header_len;
  std::vector<uint8_t> header;
  std::vector<Tuple> tuples;

  header.insert(header.end(), data.begin(), data.begin() + header_len);
  for (int i = 0; i < n_slots; i++) {
    Tuple tuple(td);

    for (int j = 0; j < n_fields; j++) {
      ASSIGN_OR_RETURN(Type field_type, td->GetFieldType(j));

      if (field_type.GetValue() == Type::INT) {
        RETURN_IF_ERROR(tuple.SetField(j, ParseInt(data, data_idx)));
      } else if (field_type.GetValue() == Type::STRING) {
        RETURN_IF_ERROR(tuple.SetField(j, ParseString(data, data_idx)));
      }
    }
    tuples.push_back(std::move(tuple));
  }

  return std::unique_ptr<HeapPage>(
      new HeapPage(id, td, header, std::move(tuples), n_slots));
}

PageId HeapPage::GetId() {
  return pid;
}

// std::optional<TransactionId> HeapPage::DirtiedBy() {
// }

// void HeapPage::MarkDirty(bool dirty, TransactionId tid) {
// }

absl::StatusOr<std::vector<uint8_t>> HeapPage::GetPageData() {
  // TODO: uncomment when DirtiedBy is implemented
  // if (!DirtiedBy())
  //   return old_data;

  std::vector<uint8_t> result = header;
  int n_tuples = tuples.size();
  int tuple_len = td->Length();

  for (int i = 0; i < n_tuples; i++) {
    ASSIGN_OR_RETURN(bool tuple_present, TuplePresent(header, i));
    if (!tuple_present) {
      result.insert(result.end(), td->GetSize(), '\0');
      continue;
    }
    Tuple& tuple = tuples[i];
    for (int j = 0; j < tuple_len; j++) {
      ASSIGN_OR_RETURN(Type field_type, td->GetFieldType(j));
      ASSIGN_OR_RETURN(Field * field, tuple.GetField(j));

      if (field_type.GetValue() == Type::STRING)
        DumpString(field, result);
      else if (field_type.GetValue() == Type::INT)
        DumpInt(field, result);
    }
  }
  result.insert(result.end(), CONFIG_PAGE_SIZE - result.size(), '\0');
  return result;
}

absl::StatusOr<std::unique_ptr<Page>> HeapPage::GetBeforeImage() {
  absl::MutexLock l(&old_data_lock);
  ASSIGN_OR_RETURN(std::unique_ptr<HeapPage> result, Create(pid, td, old_data));
  return result;
}

absl::Status HeapPage::SetBeforeImage() {
  absl::MutexLock l(&old_data_lock);
  ASSIGN_OR_RETURN(old_data, GetPageData());
  return absl::OkStatus();
}

};  // namespace komfydb::storage
