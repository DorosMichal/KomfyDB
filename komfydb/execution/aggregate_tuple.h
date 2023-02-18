#ifndef __AGGREGATE_TUPLE_H__
#define __AGGREGATE_TUPLE_H__

#include "komfydb/common/field.h"
#include "komfydb/common/tuple.h"
#include "komfydb/common/tuple_desc.h"
#include "komfydb/execution/aggregator.h"

namespace {

typedef komfydb::execution::Aggregator::AggregateType AggregateType;
using komfydb::common::Field;
using komfydb::common::Tuple;
using komfydb::common::TupleDesc;

}  // namespace

namespace komfydb::execution {

class AggregateTuple : public Tuple {
 private:
  int group_size;

 public:
  AggregateTuple(int size);

  AggregateTuple(const AggregateTuple& t);

  int GetGroupSize();

  void IncremetGroupSize();

  absl::Status ApplyAggregate(AggregateType aggregate_type, int i,
                              Field* new_field);

  absl::Status FinalizeAggregates(std::vector<AggregateType> aggregate_types);
};

}  // namespace komfydb::execution

#endif  // __AGGREGATE_TUPLE_H__
