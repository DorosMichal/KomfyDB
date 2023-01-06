#ifndef __JOIN_OPTIMIZER_H__
#define __JOIN_OPTIMIZER_H__

#include <memory>

#include "komfydb/execution/logical_plan/join_node.h"
#include "komfydb/storage/catalog.h"

namespace {

using komfydb::execution::logical_plan::JoinNode;
using komfydb::storage::Catalog;

};  // namespace

namespace komfydb::optimizer {

class JoinOptimizer {
 public:
  JoinOptimizer(std::shared_ptr<Catalog> catalog)
      : catalog(std::move(catalog)) {}

  absl::Status OrderJoins(std::vector<JoinNode>& joins);

 private:
  std::shared_ptr<Catalog> catalog;
};

};  // namespace komfydb::optimizer

#endif
