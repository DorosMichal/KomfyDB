#include "komfydb/execution/limit.h"

#include "komfydb/utils/status_macros.h"

namespace komfydb::execution {

absl::StatusOr<std::unique_ptr<Limit>> Limit::Create(
    std::unique_ptr<OpIterator> child, limit_t limit) {
  return std::unique_ptr<Limit>(new Limit(std::move(child), limit));
}

Limit::Limit(std::unique_ptr<OpIterator> child, limit_t limit)
    : OpIterator(*child->GetTupleDesc(), *child->GetFieldsTableAliases()),
      child(std::move(child)),
      limit(limit) {}

absl::Status Limit::Open() {
  RETURN_IF_ERROR(child->Open());
  fetched = 0;
  return absl::OkStatus();
}

void Limit::Close() {
  child->Close();
}

absl::Status Limit::Rewind() {
  RETURN_IF_ERROR(child->Rewind());
  fetched = 0;
  return absl::OkStatus();
}

void Limit::Explain(std::ostream& os, int indent) {
  os << Indent(indent) << "-> Limit(" << limit << ")\n";
  child->Explain(os, indent + child_indent);
}

absl::Status Limit::FetchNext() {
  if (fetched == limit) {
    return absl::OutOfRangeError(
        absl::StrCat("Tuple limit ", limit, " reached."));
  }
  fetched++;
  ASSIGN_OR_RETURN(next_record, child->Next());
  return absl::OkStatus();
}

};  // namespace komfydb::execution
