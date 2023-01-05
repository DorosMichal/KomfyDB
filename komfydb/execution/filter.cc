#include "komfydb/execution/filter.h"

#include "absl/status/status.h"
#include "absl/status/statusor.h"

#include "komfydb/execution/op.h"
#include "komfydb/utils/status_macros.h"

namespace {

using namespace komfydb::common;

}  // namespace

namespace komfydb::execution {

Filter::Filter(std::unique_ptr<OpIterator> child, Predicate predicate,
               TupleDesc& td)
    : OpIterator(td, *child->GetFieldsTableAliases()),
      child(std::move(child)),
      predicate(std::move(predicate)) {}

absl::StatusOr<std::unique_ptr<Filter>> Filter::Create(
    std::unique_ptr<OpIterator> child, Predicate predicate) {
  TupleDesc* td = child->GetTupleDesc();
  /* Check if td has field specified in predicate */
  RETURN_IF_ERROR(td->GetFieldType(predicate.GetLField()).status());
  if (predicate.GetType() == Predicate::Type::COL_COL) {
    RETURN_IF_ERROR(td->GetFieldType(predicate.GetRField()).status());
  }

  return std::unique_ptr<Filter>(
      new Filter(std::move(child), std::move(predicate), *td));
}

Predicate* Filter::GetPredicate() {
  return &predicate;
}

absl::Status Filter::Open() {
  return child->Open();
}

void Filter::Close() {
  child->Close();
}

absl::Status Filter::Rewind() {
  return child->Rewind();
}

absl::Status Filter::FetchNext() {
  ITERATE_RECORDS(child, record) {
    if (predicate.Evaluate(*record.value())) {
      next_record = std::move(record.value());
      return absl::OkStatus();
    }
  }
  return record.status();
}

void Filter::Explain(std::ostream& os, int indent) {
  os << Indent(indent) << "-> Filter with " << predicate << "\n";
  child->Explain(os, indent + child_indent);
}

};  // namespace komfydb::execution
