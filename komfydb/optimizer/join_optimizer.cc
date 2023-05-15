#include <algorithm>
#include <cstddef>
#include <memory>

#include "komfydb/optimizer/join_optimizer.h"

#include "komfydb/execution/join_predicate.h"
#include "komfydb/utils/status_macros.h"

namespace {

using komfydb::execution::JoinPredicate;

int vec2int(std::vector<bool> v){
  int result = 0;
  for(auto el: v){
    result += el;
    result *= 2;
  }
  return result;
}

int hash_element_number(int element, int set_size){
  return 1 << (set_size - element - 1);
}

int delete_element(int set_hash, int element_number, int set_size){
  int element_mask = 1 << (set_size - element_number - 1);
  return set_hash & ~element_mask;
}

int join_cost(int set, int el){
  return 1;
}

};

namespace komfydb::optimizer {

absl::Status JoinOptimizer::OrderJoins(std::vector<JoinNode>& joins) {
  // since it's expontential, data must not be to big
  auto N = joins.size();
  assert(N < 32);

  auto optjoin = std::make_unique<int[]>(1 << N);
  auto joined_element = std::make_unique<int[]>(1 << N);

  std::vector<bool> subset(N, 0);

  // initialize single tables cost
  for(int i = 0; i < N; i++){
    optjoin[hash_element_number(i, N)] = 
  }


  for(int k = 2; k <= N; k++){
    // prepare the subset with i elements
    std::fill(subset.begin(), subset.end() - k, 0);
    std::fill(subset.end() - k, subset.end(), 1);

    do {
      int best_to_add = -1;
      int min_cost = INT_MAX;
      int set_hash = vec2int(subset);

      // find best element to join
      for(int i = 0; i < N; i++){
        if(subset[i]){
          int subset_hash = delete_element(set_hash, i, N);
          int element_hash = 1 << (N - i - 1);
          int cost = optjoin[subset_hash] + optjoin[element_hash] + join_cost(subset_hash, element_hash);

          if(cost < min_cost){
            min_cost = cost;
            best_to_add = i;
          }
        }
      }
      optjoin[set_hash] = min_cost;
      joined_element[set_hash] = best_to_add;
    } while(std::next_permutation(subset.begin(), subset.end()));
  }

  // backtrack the best order
  std::vector<JoinNode> ordered_joins(N);
  int set_hash = (1 << N) - 1;
  while(set_hash){
    int element_to_join_idx = joined_element[set_hash];
    ordered_joins.push_back(std::move(joins[element_to_join_idx]));
    set_hash = delete_element(set_hash, element_to_join_idx , N);
  }

  for(int i = 0; i < N; i++){
    joins[i] = std::move(ordered_joins[N-i-1]);
  }

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
  return Join::Create(std::move(l_child),
                      JoinPredicate(l_field, join_node.op, r_field),
                      std::move(r_child));
}

};  // namespace komfydb::optimizer
