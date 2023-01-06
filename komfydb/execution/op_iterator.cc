#include "komfydb/execution/op_iterator.h"

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "komfydb/utils/status_macros.h"

namespace komfydb::execution {

OpIterator::OpIterator(TupleDesc& td) : td(td), next_record(nullptr) {}

TupleDesc* OpIterator::GetTupleDesc() {
  return &td;
}

absl::StatusOr<std::unique_ptr<Record>> OpIterator::Next() {
  if (next_record.get() == nullptr) {
    RETURN_IF_ERROR(FetchNext());
  }
  return std::move(next_record);
}

bool OpIterator::HasNext() {
  if (next_record.get() == nullptr) {
    absl::Status fetch = FetchNext();
    if (fetch.ok()) {
      return true;
    }
    assert(absl::IsOutOfRange(fetch));
    return false;
  }
  return true;
}

};  // namespace komfydb::execution