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
      std::vector<Aggregator::AggregateType>& aggregate_types,
      std::vector<int>& aggregate_fields, std::vector<int>& groupby_fields);

  absl::Status Open() override;

  void Close() override;

  absl::Status Rewind() override;

  void Explain(std::ostream& os, int indent = 0) override;

 private:
  std::unique_ptr<OpIterator> child;
  std::vector<Aggregator::AggregateType> aggregate_types;
  std::vector<int> aggregate_fields, groupby_fields;
  std::vector<Tuple> out_tuples;
  std::vector<Tuple>::iterator out_tuples_it;

  Aggregate(std::unique_ptr<OpIterator> child,
            std::vector<Aggregator::AggregateType>& aggregate_types,
            std::vector<int>& aggregate_fields,
            std::vector<int>& groupby_fields, TupleDesc& tuple_desc);

  absl::Status FetchNext() override;

  absl::Status Prepare();

  absl::Status PrepareWithGrouping();

  absl::Status PrepareNoGrouping();
};

}  // namespace komfydb::execution

#endif  // __AGGREGATE_H__
