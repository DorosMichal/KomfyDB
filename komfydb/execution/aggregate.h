#ifndef __AGGREGATE_H__
#define __AGGREGATE_H__

#include "komfydb/execution/aggregator.h"
#include "komfydb/execution/op_iterator.h"

namespace komfydb::execution {

class Aggregate : public OpIterator {
 public:
  static absl::StatusOr<std::unique_ptr<Aggregate>> Create(
      std::unique_ptr<OpIterator> child, Aggregator aggregator);

  absl::Status Open() override;

  void Close() override;

 private:
  std::unique_ptr<OpIterator> child;
  Aggregator aggregator;

  Aggregate(std::unique_ptr<OpIterator> child, Aggregator aggregator);

  absl::Status FetchNext() override;
};

}  // namespace komfydb::execution

#endif  // __AGGREGATE_H__