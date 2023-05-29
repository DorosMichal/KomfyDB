#include <algorithm>
#include <cstddef>
#include <memory>
#include <vector>

#include "komfydb/optimizer/join_optimizer.h"

#include "komfydb/execution/join_predicate.h"
#include "komfydb/utils/status_macros.h"
#include "komfydb/execution/op.h"


namespace {

using komfydb::execution::JoinPredicate;
using komfydb::execution::Op;

struct Edge{
  Edge(int join_idx, int table_idx): join_idx(join_idx), table_idx(table_idx) {}

  int join_idx;
  int table_idx;
};

struct JoinInfo{
  JoinInfo(int cost, int size, int performed_join) : cost(cost), size(size), performed_join(performed_join) {}

  int cost;
  int size;
  int performed_join;
};

bool element_in_set(int element_idx, uint set_number){
  return set_number & (1 << element_idx);
}

int singleton_set_to_idx(uint set_number){
  return __builtin_ctz(set_number);
}

int delete_element(int element_idx, uint set_number){
  int element_mask = 1 << element_idx;
  return set_number & ~element_mask;
}

 JoinInfo join_cost(int set_number, int element_number, std::vector<JoinNode>& joins, std::vector<std::vector<Edge>>& tree, std::vector<int>& predicted_size){
  // returns -1 as first element if the join cannot be performed
  int cost = -1;
  int min_size = INT_MAX, size;
  int performed_join = -1;
  const int element_idx = singleton_set_to_idx(element_number);
  const int set_size = predicted_size[set_number];
  const int element_size = predicted_size[element_number];

  for(auto [join_idx, table_idx]: tree[element_idx]){

    if(element_in_set(table_idx, set_number)){
      // TODO add different join methods and return the method as well
      cost = set_size * element_size;

      // a place for heuristics about size of join result
      if (joins[join_idx].op.value == Op::EQUALS){
        size = std::max(set_size, element_size);
      } else {
        size = (int)(0.1 * set_size * element_size);
      }

      if(min_size > size){
        min_size = size;
        performed_join = join_idx;
      }
    }
  }
  return {cost, size, performed_join};

}

};

namespace komfydb::optimizer {

absl::Status JoinOptimizer::OrderJoins(std::vector<JoinNode>& joins, TableStatsMap& table_stats_map) {
  // map tables (relations) names to numbers
  absl::flat_hash_map<std::string, int> table2num;
  std::vector<std::string> num2table;

  // a tree representing joins between relations
  std::vector< std::vector<Edge>> tree;
  int num = 0;
  for(int i = 0; i < joins.size(); i++){
    // TODO
    // we also parse subquery joins, but for now they wont show up in the tree and so they would be ommited, returning incorrect result
    assert(joins[i].type == JoinNode::Type::COL_COL);

    // lref should always be valid
    assert(joins[i].lref.IsValid());
    std::string lname = joins[i].lref.table;
    if(!table2num.count(lname)){
      table2num[lname] = num++;
      num2table.push_back(lname);
    }
    if(joins[i].rref.IsValid()){

      std::string rname = joins[i].rref.table;
      if(!table2num.count(rname)){
        table2num[rname] = num++;
        num2table.push_back(rname);
      }
      // we have col:col join so we add edge to a tree
      tree[table2num[lname]].push_back({i, table2num[rname]});
      tree[table2num[rname]].push_back({i, table2num[lname]});
    }
  }

  auto N = table2num.size();
  // since it's expontential in N, data must not be to big
  assert(N < 20);

  uint number_of_subsets = 1 << N;
  // TODO probably needs to be changed to Long Long values
  std::vector<int> optjoin(1 << N, 0);
  std::vector<int> joined_element(1 << N, 0);
  std::vector<int> predicted_size(1 << N, 0);


  for(int set_number = 0; set_number < number_of_subsets; set_number++){
    if(std::__popcount(set_number) == 1){
      std::string table_name = num2table[singleton_set_to_idx(set_number)];
      optjoin[set_number] = table_stats_map[table_name].EstimateScanCost();
      predicted_size[set_number] = table_stats_map[table_name].EstimateTableCardinality();
    } else {
      int best_to_add = -1;
      int estimated_size = -1;
      int min_cost = INT_MAX;
      // find best element to join
      for(int i = 0; i < N; i++){
        if(element_in_set(i, set_number)){
          int subset_number = delete_element(i, set_number);
          int element_number = 1 << i;
          auto [cost, size, performed_join] = join_cost(subset_number, element_number, joins, tree, predicted_size);
          if(cost == -1){
            continue;
          }
          int total_cost = optjoin[subset_number] + optjoin[element_number] + cost;

          if(total_cost < min_cost){
            min_cost = total_cost;
            best_to_add = performed_join;
            estimated_size = size;
          }
        }
      }
      optjoin[set_number] = min_cost;
      joined_element[set_number] = best_to_add;
      predicted_size[set_number] = estimated_size;
    }
  }

  // backtrack the best order
  std::vector<JoinNode> ordered_joins(joins.size());
  uint set_number = number_of_subsets - 1;
  while(set_number){
    int element_to_join_idx = joined_element[set_number];
    ordered_joins.push_back(std::move(joins[element_to_join_idx]));
    set_number = delete_element(element_to_join_idx, set_number);
  }

  for(int i = 0; i < joins.size(); i++){
    joins[i] = std::move(ordered_joins[joins.size()-i-1]);
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
