#include "komfydb/optimizer/join_optimizer.h"

#include "komfydb/execution/join_predicate.h"
#include "komfydb/utils/status_macros.h"

namespace {

using komfydb::execution::HashJoin;
using komfydb::execution::Join;
using komfydb::execution::JoinPredicate;
using komfydb::execution::LoopsJoin;

};  // namespace

namespace komfydb::optimizer {

absl::Status JoinOptimizer::OrderJoins(std::vector<JoinNode>& joins) {
  // TODO(JoinOptimizer)
  return absl::OkStatus();
}

absl::StatusOr<std::unique_ptr<Join>> JoinOptimizer::InstatiateJoin(
    JoinNode& join_node, std::unique_ptr<OpIterator> l_child,
    std::unique_ptr<OpIterator> r_child) {
  // TODO(JoinOptimizer)

  // I think this MAY return an error, as this is the only place where we can
  // check if the right subquery the same type as the left child.
  ASSIGN_OR_RETURN(int l_field, l_child->GetIndexForColumnRef(join_node.lref));
  int r_field = 0;
  if (join_node.type == JoinNode::COL_COL) {
    ASSIGN_OR_RETURN(r_field, r_child->GetIndexForColumnRef(join_node.rref));
  }

  ASSIGN_OR_RETURN(common::Type l_type,
                   l_child->GetTupleDesc()->GetFieldType(l_field));
  ASSIGN_OR_RETURN(common::Type r_type,
                   r_child->GetTupleDesc()->GetFieldType(r_field));

  if (l_type != r_type) {
    return absl::FailedPreconditionError(absl::StrCat(
        "Join with a subquery has wrong types: left", l_field,
        " field has type ", static_cast<std::string>(l_type), ", right",
        r_field, " field has type ", static_cast<std::string>(r_type)));
  }
  switch (join_node.op.value) {
    case Op::EQUALS: {
      return HashJoin::Create(std::move(l_child),
                              JoinPredicate(l_field, join_node.op, r_field),
                              std::move(r_child));
    }
    default: {
      break;
    }
  }
  return LoopsJoin::Create(std::move(l_child),
                           JoinPredicate(l_field, join_node.op, r_field),
                           std::move(r_child));
}

};  // namespace komfydb::optimizer
