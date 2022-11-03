#include "komfydb/storage/heap_page.h"
#include "komfydb/common/int_field.h"
#include "komfydb/common/string_field.h"
#include "komfydb/config.h"
#include "komfydb/utils/status_macros.h"

namespace {
using namespace komfydb::common;
}

namespace komfydb::storage {

absl::StatusOr<std::unique_ptr<HeapPage>> HeapPage::Create(
    PageId id, TupleDesc td, std::vector<uint8_t> data) {
  int n_slots = (CONFIG_PAGE_SIZE * 8) / (td.GetSize() * 8 + 1);
  int header_len = (n_slots + 7) / 8;
  std::vector<uint8_t> header;
  for (int i = 0; i < header_len; i++)
    header.push_back(data[i]);

  std::vector<Tuple> tuples;
  int n_fields = td.Length();
  int data_idx = header_len;

  for (int i = 0; i < n_slots; i++) {
    Tuple tuple = Tuple(&td);
    for (int j = 0; j < n_fields; j++) {
      ASSIGN_OR_RETURN(Type field_type, td.GetFieldType(j));
      if (field_type.GetValue() == Type::INT) {
        uint8_t bytes[Type::INT_LEN];
        for (int byte = 0; byte < Type::INT_LEN; byte++)
          bytes[byte] = data[data_idx + byte];
        data_idx += Type::INT_LEN;
        IntField* field = new IntField(*((int*)bytes));
        tuple.SetField(j, field);
      } else if (field_type.GetValue() == Type::STRING) {
        std::string value = "";
        for (int byte = 0; byte < Type::STR_LEN; byte++)
          value.push_back((char)data[data_idx + byte]);
        data_idx += Type::STR_LEN;
        StringField* field = new StringField(value);
        tuple.SetField(j, field);
      }
    }
    tuples.push_back(tuple);
  }
  std::unique_ptr<HeapPage> result = std::make_unique<HeapPage>();
  result->pid = id;
  result->td = td;
  result->header = header;
  result->tuples = tuples;
  result->num_slots = n_slots;
  return result;
}

PageId* HeapPage::GetId() {
  return (PageId*)&pid;
}

TransactionId* HeapPage::IsDirty() {
  // TODO
}

void HeapPage::MarkDirty(bool dirty, TransactionId tid) {
  // TODO
}

absl::StatusOr<bool> HeapPage::TuplePresent(int i) {
  if (i / 8 >= header.size() || i < 0)
    return absl::InvalidArgumentError("Index out of range");
  return header[i / 8] & (1 << (i % 8));
}

absl::StatusOr<std::vector<uint8_t>> HeapPage::GetPageData() {
  if (!IsDirty())
    return old_data;

  std::vector<uint8_t> result = header;
  int n_tuples = tuples.size();
  int tuple_len = td.Length();

  for (int i = 0; i < n_tuples; i++) {
    ASSIGN_OR_RETURN(bool tuple_present, TuplePresent(i));
    if (!tuple_present) {
      for (int j = 0; j < td.GetSize(); j++)
        result.push_back('\0');
      continue;
    }
    Tuple tuple = tuples[i];
    for (int j = 0; j < tuple_len; j++) {
      ASSIGN_OR_RETURN(Type field_type, td.GetFieldType(j));
      ASSIGN_OR_RETURN(Field * field, tuple.GetField(j));

      if (field_type.GetValue() == Type::STRING) {
        std::string data;
        field->GetValue(data);
        int padding = Type::STR_LEN - data.size();
        for (char character : data)
          result.push_back((uint8_t)character);
        for (int i = 0; i < padding; i++)
          result.push_back('\0');
      } else if (field_type.GetValue() == Type::INT) {
        int data;
        field->GetValue(data);
        for (int byte = 0; byte < sizeof(data); byte++)
          result.push_back(*((uint8_t*)&data + byte));
      }
    }
  }
  for (int i = 0; i < CONFIG_PAGE_SIZE - result.size(); i++)
    result.push_back('\0');
  return result;
}

std::unique_ptr<Page> HeapPage::GetBeforeImage() {
  absl::MutexLock l(&old_data_lock);
  ASSIGN_OR_RETURN(std::unique_ptr<HeapPage> result,
                   Create(pid, td, old_data));  // strange error here
  return result;
}

};  // namespace komfydb::storage