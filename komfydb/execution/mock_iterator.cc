#include "komfydb/execution/mock_iterator.h"
#include "op_iterator.h"

namespace komfydb::execution {

absl::StatusOr<std::unique_ptr<MockIterator>> MockIterator::Create(
    std::vector<Record> records, TupleDesc& tuple_desc) {
  return std::unique_ptr<MockIterator>(
      new MockIterator(std::move(records), tuple_desc));
}

absl::Status MockIterator::Open() {
  records_it = records.begin();
  return absl::OkStatus();
}

void MockIterator::Close() {
  records_it = records.end();
}

absl::Status MockIterator::Rewind() {
  records_it = records.begin();
  return absl::OkStatus();
}

MockIterator::MockIterator(std::vector<Record> records, TupleDesc& td)
    : OpIterator(td), records(std::move(records)) {}

absl::Status MockIterator::FetchNext() {
  if (records_it == records.end()) {
    return absl::OutOfRangeError("Mock iterator out of range.");
  }
  next_record = std::make_unique<Record>(*records_it++);
  return absl::OkStatus();
}

};  // namespace komfydb::execution
