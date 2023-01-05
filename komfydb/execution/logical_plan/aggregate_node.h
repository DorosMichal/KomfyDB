#ifndef __AGGREGATE_NODE_H__
#define __AGGREGATE_NODE_H__

#include "komfydb/common/column_ref.h"
#include "komfydb/execution/aggregator.h"

namespace {

using komfydb::common::COLUMN_REF_STAR;
using komfydb::common::ColumnRef;

};  // namespace

namespace komfydb::execution::logical_plan {

class AggregateNode {
 public:
  ColumnRef col;
  Aggregator::AggregateType type;

  AggregateNode(Aggregator::AggregateType type, ColumnRef col)
      : col(col), type(type) {}

  AggregateNode(Aggregator::AggregateType type)
      : col(COLUMN_REF_STAR), type(type) {}

  operator std::string() const {
    return Aggregator::AggregateTypeToString(type) + "(" +
           static_cast<std::string>(col) + ")";
  }
};

};  // namespace komfydb::execution::logical_plan

#endif
