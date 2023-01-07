#include "komfydb/execution/aggregate.h"

namespace komfydb::execution {

Aggregate::Aggregate(std::unique_ptr<OpIterator> child, Aggregator aggregator)
    : OpIterator(*child->GetTupleDesc()),
      child(std::move(child)),
      aggregator(aggregator) {}

absl::StatusOr<std::unique_ptr<Aggregate>> Create(
    std::unique_ptr<OpIterator> child, Aggregator aggregator) {}

absl::Status Aggregate::Open() {
  child->Open();
}

}  // namespace komfydb::execution