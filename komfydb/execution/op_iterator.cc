#include "komfydb/execution/op_iterator.h"

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "komfydb/utils/status_macros.h"

namespace komfydb::execution {

OpIterator::OpIterator(TupleDesc& tuple_desc)
    : tuple_desc(tuple_desc), next_record(nullptr) {}

TupleDesc* OpIterator::GetTupleDesc() {
  return &tuple_desc;
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
