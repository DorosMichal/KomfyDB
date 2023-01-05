#include "komfydb/execution/op_iterator.h"

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "komfydb/utils/status_macros.h"

namespace komfydb::execution {

OpIterator::OpIterator(TupleDesc& tuple_desc, std::string_view alias)
    : tuple_desc(tuple_desc),
      fields_table_aliases(tuple_desc.Length(), std::string(alias)),
      next_record(nullptr) {}

OpIterator::OpIterator(TupleDesc& tuple_desc,
                       std::vector<std::string> fields_table_ids)
    : tuple_desc(tuple_desc),
      fields_table_aliases(std::move(fields_table_ids)),
      next_record(nullptr) {}

TupleDesc* OpIterator::GetTupleDesc() {
  return &tuple_desc;
}

std::vector<std::string>* OpIterator::GetFieldsTableAliases() {
  return &fields_table_aliases;
}

absl::StatusOr<int> OpIterator::GetIndexForColumnRef(ColumnRef col) {
  std::vector<common::TDItem> items = tuple_desc.GetItems();
  for (int i = 0; i < items.size(); i++) {
    if (col.table == fields_table_aliases[i] &&
        items[i].field_name == col.column) {
      return i;
    }
  }
  return absl::InvalidArgumentError(absl::StrCat(
      "This OpIterator has no field corresponding to ", std::string(col)));
}

absl::StatusOr<std::unique_ptr<Record>> OpIterator::Next() {
  if (next_record.get() == nullptr) {
    RETURN_IF_ERROR(FetchNext());
  }
  return std::move(next_record);
}

absl::Status OpIterator::HasNext() {
  if (next_record.get() == nullptr) {
    return FetchNext();
  }
  return absl::OkStatus();
}

std::string OpIterator::Indent(int indent) {
  return std::string(indent, ' ');
}

};  // namespace komfydb::execution
