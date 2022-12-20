#ifndef __JOIN_NODE_H__
#define __JOIN_NODE_H__

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

  JoinNode(ColumnRef lref, Op op, ColumnRef rref)
      : lref(lref), rref(rref), op(op), type(COL_COL) {}

  JoinNode(ColumnRef lref, Op op, std::unique_ptr<LogicalPlan> subplan)
      : lref(lref), subplan(std::move(subplan)), op(op), type(COL_SUB) {}
};

};  // namespace komfydb::execution::logical_plan

#endif  // __JOIN_NODE_H__
