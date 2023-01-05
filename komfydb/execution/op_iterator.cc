#include "komfydb/execution/op_iterator.h"

namespace komfydb::execution {

OpIterator::OpIterator(TupleDesc& td) : td(td), next_record(nullptr) {}

TupleDesc* OpIterator::GetTupleDesc() {
  return &td;
}

absl::StatusOr<std::unique_ptr<Record>> OpIterator::Next() {
  if (next_record.get() == nullptr) {
    auto record = FetchNext();
    if (!record.ok()) {
      return record.status();
    }
    next_record = std::move(record.value());
  }
  return std::move(next_record);
}

bool OpIterator::HasNext() {
  if (next_record.get() == nullptr) {
    auto record = FetchNext();
    if (record.ok()) {
      next_record = std::move(record.value());
      return true;
    }
    assert(absl::IsOutOfRange(record.status()));
    return false;
  }
}

};  // namespace komfydb::execution