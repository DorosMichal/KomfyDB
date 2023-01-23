#include "komfydb/execution/fixed_iterator.h"

namespace komfydb::execution {

FixedIterator::FixedIterator(TupleDesc tuple_desc, std::vector<Record> records)
    : OpIterator(tuple_desc, "__fixed__"), records(std::move(records)) {}

absl::StatusOr<std::unique_ptr<FixedIterator>> FixedIterator::Create(
    TupleDesc tuple_desc, std::vector<Record>&& records) {
  return std::unique_ptr<FixedIterator>(
      new FixedIterator(tuple_desc, std::move(records)));
}

absl::Status FixedIterator::Open() {
  it = records.begin();
  return absl::OkStatus();
}

void FixedIterator::Close() {}

absl::Status FixedIterator::Rewind() {
  it = records.begin();
  return absl::OkStatus();
}

void FixedIterator::Explain(std::ostream& os, int indent) {
  os << Indent(indent) << "-> Passing " << records.size() << " tuples.\n";
}

absl::Status FixedIterator::FetchNext() {
  if (it == records.end()) {
    return absl::OutOfRangeError("No more records in this OpIterator");
  }
  next_record = std::make_unique<Record>(*it++);
  return absl::OkStatus();
}

}  // namespace komfydb::execution
