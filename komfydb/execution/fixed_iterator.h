#ifndef __FIXED_ITERATOR_H__
#define __FIXED_ITERATOR_H__

#include "komfydb/common/tuple_desc.h"
#include "komfydb/execution/op_iterator.h"

#include "absl/status/status.h"
#include "absl/status/statusor.h"

namespace komfydb::execution {

class FixedIterator : public OpIterator {
 public:
  static absl::StatusOr<std::unique_ptr<FixedIterator>> Create(
      TupleDesc tuple_desc, std::vector<Record>&& records);

  absl::Status Open() override;

  void Close() override;

  absl::Status Rewind() override;

  void Explain(std::ostream& os, int indent = 0) override;

 private:
  std::vector<Record> records;
  std::vector<Record>::iterator it;

  FixedIterator(TupleDesc tuple_desc, std::vector<Record> records);

  absl::Status FetchNext() override;
};

}  // namespace komfydb::execution

#endif  // __FIXED_ITERATOR_H__