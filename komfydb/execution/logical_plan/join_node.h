#ifndef __JOIN_NODE_H__
#define __JOIN_NODE_H__

#include <memory>

#include "komfydb/common/column_ref.h"
#include "komfydb/execution/op.h"

namespace {

using komfydb::common::ColumnRef;

};

namespace komfydb::execution::logical_plan {

class LogicalPlan;

class JoinNode {
 public:
  enum Type {
    COL_COL,
    COL_SUB,
  };

  ColumnRef lref;
  ColumnRef rref;
  std::unique_ptr<LogicalPlan> subplan;
  Op op;
  Type type;

  JoinNode(ColumnRef lref, Op op, ColumnRef rref);

  JoinNode(ColumnRef lref, Op op, std::unique_ptr<LogicalPlan> subplan);
};

};  // namespace komfydb::execution::logical_plan

#endif  // __JOIN_NODE_H__
