#ifndef __JOIN_OPTIMIZER_H__
#define __JOIN_OPTIMIZER_H__

#include <memory>

#include "komfydb/execution/join.h"
#include "komfydb/execution/logical_plan/join_node.h"
#include "komfydb/execution/op_iterator.h"
#include "komfydb/storage/catalog.h"
#include "komfydb/optimizer/table_stats.h"
#include "komfydb/execution/logical_plan/logical_plan.h"

namespace {

using komfydb::execution::Join;
using komfydb::execution::OpIterator;
using komfydb::execution::logical_plan::JoinNode;
using komfydb::storage::Catalog;

};  // namespace

namespace komfydb::optimizer {

class JoinOptimizer {
 public:
  JoinOptimizer(std::shared_ptr<Catalog> catalog)
      : catalog(std::move(catalog)) {}

  absl::Status OrderJoins(std::vector<JoinNode>& joins,
                          TableStatsMap& table_stats_map);

  absl::StatusOr<std::unique_ptr<Join>> InstatiateJoin(
      JoinNode& join_node, std::unique_ptr<OpIterator> l_child,
      std::unique_ptr<OpIterator> r_child);

 private:
  std::shared_ptr<Catalog> catalog;
};

};  // namespace komfydb::optimizer

#endif
