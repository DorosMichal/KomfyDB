#include "komfydb/execution/logical_plan/join_node.h"

#include "komfydb/execution/logical_plan/logical_plan.h"

namespace komfydb::execution::logical_plan {

JoinNode::JoinNode(ColumnRef lref, Op op, ColumnRef rref)
    : lref(lref), rref(rref), op(op), type(COL_COL) {}

JoinNode::JoinNode(ColumnRef lref, Op op, std::unique_ptr<LogicalPlan> subplan)
    : lref(lref), op(op), type(COL_SUB) {}

};  // namespace komfydb::execution::logical_plan
