#include "komfydb/optimizer/join_optimizer.h"

#include "komfydb/execution/join_predicate.h"
#include "komfydb/utils/status_macros.h"

namespace {

using komfydb::execution::JoinPredicate;

};

namespace komfydb::optimizer {

absl::Status JoinOptimizer::OrderJoins(std::vector<JoinNode>& joins) {
  // TODO(JoinOptimizer)
  return absl::OkStatus();
}

absl::StatusOr<std::unique_ptr<Join>> JoinOptimizer::InstatiateJoin(
    JoinNode& join_node, std::unique_ptr<OpIterator> l_child,
    std::unique_ptr<OpIterator> r_child) {
  // TODO(JoinOptimizer)

  // This may return an error, as this is the only place where we can
  // check if the right subquery the same type as the left child.
  ASSIGN_OR_RETURN(int l_field, l_child->GetIndexForColumnRef(join_node.lref));
  int r_field = 0;
  if (join_node.type == JoinNode::COL_COL) {
    ASSIGN_OR_RETURN(r_field, r_child->GetIndexForColumnRef(join_node.rref));
  }

  common::Type l_type = l_child->GetTupleDesc()->GetFieldType(l_field);
  common::Type r_type = r_child->GetTupleDesc()->GetFieldType(r_field);

  if (l_type != r_type) {
    return absl::FailedPreconditionError(absl::StrCat(
        "Join with a subquery has wrong types: left", l_field,
        " field has type ", static_cast<std::string>(l_type), ", right",
        r_field, " field has type ", static_cast<std::string>(r_type)));
  }
  return Join::Create(std::move(l_child),
                      JoinPredicate(l_field, join_node.op, r_field),
                      std::move(r_child));
}

};  // namespace komfydb::optimizer
