#ifndef __MOCK_ITERATOR_H__
#define __MOCK_ITERATOR_H__

#include <memory>

#include "absl/status/statusor.h"

#include "komfydb/execution/op_iterator.h"
#include "komfydb/storage/record.h"

namespace komfydb::execution {

// Mock class for testing purposes only.
class MockIterator : public OpIterator {
 public:
  static absl::StatusOr<std::unique_ptr<MockIterator>> Create(
      std::vector<Record> records, TupleDesc& tuple_desc);

  absl::Status Open() override;

  void Close() override;

  absl::Status Rewind() override;

 private:
  MockIterator(std::vector<Record> records, TupleDesc& td);

  std::vector<Record> records;
  std::vector<Record>::iterator records_it;

  absl::Status FetchNext() override;
};

};  // namespace komfydb::execution

#endif
