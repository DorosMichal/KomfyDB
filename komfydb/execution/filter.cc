#include "komfydb/execution/filter.h"

#include "absl/status/status.h"
#include "absl/status/statusor.h"

#include "komfydb/execution/op.h"
#include "komfydb/utils/status_macros.h"

namespace {

using namespace komfydb::common;

absl::StatusOr<bool> filter(Record record, Predicate* predicate) {
  ASSIGN_OR_RETURN(Field * record_field,
                   record.GetField(predicate->GetFieldIdx()));
  return record_field->Compare(predicate->GetOp(), predicate->GetOperand());
}

}  // namespace

namespace komfydb::execution {

Filter::Filter(std::unique_ptr<OpIterator> child, Predicate predicate,
               TupleDesc& td)
    : child(std::move(child)),
      td(td),
      predicate(std::move(predicate)),
      next_record(nullptr) {}

absl::StatusOr<std::unique_ptr<Filter>> Filter::Create(
    std::unique_ptr<OpIterator> child, Predicate predicate) {
  TupleDesc* td = child->GetTupleDesc();
  /* Check if td has field specified in predicate */
  RETURN_IF_ERROR(td->GetFieldType(predicate.GetFieldIdx()).status());

  return std::unique_ptr<Filter>(
      new Filter(std::move(child), std::move(predicate), *td));
}

Predicate* Filter::GetPredicate() {
  return &predicate;
}

absl::Status Filter::Open() {
  RETURN_IF_ERROR(child->Open());
  while (child->HasNext()) {
    ASSIGN_OR_RETURN(Record record, child->Next());
    ASSIGN_OR_RETURN(bool good_record, filter(record, &predicate));
    if (good_record) {
      next_record = std::make_unique<Record>(record);
      break;
    }
  }
  return absl::OkStatus();
}

bool Filter::HasNext() {
  return next_record.get() != nullptr;
}

void Filter::Close() {
  child->Close();
}

absl::StatusOr<Record> Filter::Next() {
  if (!HasNext()) {
    return absl::OutOfRangeError("No more records in this OpIterator.");
  }
  Record result = std::move(*next_record);
  next_record = nullptr;

  while (child->HasNext()) {
    ASSIGN_OR_RETURN(Record record, child->Next());
    ASSIGN_OR_RETURN(bool good_record, filter(record, &predicate));
    if (good_record) {
      next_record = std::make_unique<Record>(record);
      break;
    }
  }
  return result;
}

TupleDesc* Filter::GetTupleDesc() {
  return &td;
}

}  // namespace komfydb::execution