#ifndef __AGGREGATE_H__
#define __AGGREGATE_H__

#include "komfydb/execution/aggregator.h"
#include "komfydb/execution/op_iterator.h"

#include "absl/status/status.h"
#include "absl/status/statusor.h"

namespace {}  // namespace

namespace komfydb::execution {

class Aggregate : public OpIterator {
 public:
  static absl::StatusOr<std::unique_ptr<Aggregate>> Create(
      std::unique_ptr<OpIterator> child,
      Aggregator::AggregateType aggregate_type, int aggregate_field,
      int groupby_field);

  absl::Status Open() override;

  void Close() override;

 private:
  std::unique_ptr<OpIterator> child;
  Aggregator::AggregateType aggregate_type;
  int aggregate_field, groupby_field;

  Aggregate(std::unique_ptr<OpIterator> child,
            Aggregator::AggregateType aggregate_type, int aggregate_field,
            int groupby_field, TupleDesc& tuple_desc);

  absl::Status FetchNext() override;
};

}  // namespace komfydb::execution

#endif  // __AGGREGATE_H__