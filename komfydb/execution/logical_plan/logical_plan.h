#ifndef __LOGICAL_PLAN_H__
#define __LOGICAL_PLAN_H__

#include <string>
#include <string_view>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/status/statusor.h"

#include "hsql/SQLParser.h"

#include "komfydb/common/column_ref.h"
#include "komfydb/execution/aggregate.h"
#include "komfydb/execution/logical_plan/aggregate_node.h"
#include "komfydb/execution/logical_plan/filter_node.h"
#include "komfydb/execution/logical_plan/join_node.h"
#include "komfydb/execution/logical_plan/scan_node.h"
#include "komfydb/execution/logical_plan/select_node.h"
#include "komfydb/execution/op_iterator.h"
#include "komfydb/execution/order_by.h"
#include "komfydb/optimizer/table_stats.h"
#include "komfydb/storage/buffer_pool.h"
#include "komfydb/storage/catalog.h"

namespace {

using komfydb::common::ColumnRef;
using komfydb::optimizer::TableStats;
using komfydb::optimizer::TableStatsMap;
using komfydb::storage::BufferPool;
using komfydb::storage::Catalog;
using komfydb::transaction::TransactionId;

};  // namespace

namespace komfydb::execution::logical_plan {

class LogicalPlan {
 public:
  LogicalPlan(std::shared_ptr<Catalog> catalog,
              std::shared_ptr<BufferPool> buffer_pool)
      : catalog(std::move(catalog)), buffer_pool(std::move(buffer_pool)) {}

  absl::Status AddFilterColCol(ColumnRef lref, Op op, ColumnRef rref);
  absl::Status AddFilterColConst(ColumnRef ref, Op op,
                                 std::unique_ptr<Field> const_field);

  absl::Status AddJoin(ColumnRef lref, Op op, ColumnRef rref);
  absl::Status AddSubqueryJoin(ColumnRef ref, Op op, LogicalPlan subplan);

  absl::Status AddScan(int table_id, std::string_view alias);

  absl::Status AddGroupBy(ColumnRef ref);

  absl::Status AddAggregate(std::string_view agg_fun, ColumnRef ref);

  absl::Status AddSelect(ColumnRef ref);

  absl::Status AddOrderBy(ColumnRef ref, OrderBy::Order asc);

  absl::StatusOr<common::ColumnRef> GetColumnRef(std::string_view table,
                                                 std::string_view column);

  absl::StatusOr<common::ColumnRef> GetColumnRef(char* table, char* column);

  absl::StatusOr<common::Type> GetColumnType(ColumnRef ref);

  absl::StatusOr<std::unique_ptr<OpIterator>> GeneratePhysicalPlan(
      TransactionId tid, TableStatsMap& table_stats, bool explain);

  void Dump();

 private:
  std::shared_ptr<Catalog> catalog;
  std::shared_ptr<BufferPool> buffer_pool;
  std::vector<FilterNode> filters;
  std::vector<JoinNode> joins;
  std::vector<SelectNode> selects;
  std::vector<ScanNode> scans;
  std::vector<AggregateNode> aggregates;
  std::vector<ColumnRef> groupby_cols;

  absl::flat_hash_map<std::string, int> alias_to_id;
  absl::flat_hash_map<std::string, std::string> equiv;
  absl::flat_hash_map<std::string, double> filter_selectivities;
  absl::flat_hash_map<std::string, std::unique_ptr<OpIterator>> subplans;

  ColumnRef order_by_column = {};
  OrderBy::Order order;

  absl::Status ProcessScanNodes(TransactionId tid);
  absl::Status ProcessFilterNodes(TableStatsMap& table_stats);
  absl::Status ProcessJoinNodes(TransactionId tid, TableStatsMap& table_stats,
                                bool explain);
  absl::StatusOr<std::unique_ptr<OpIterator>> ProcessSelectNodes(
      std::unique_ptr<OpIterator> plan);
  absl::StatusOr<std::unique_ptr<OpIterator>> ProcessAggregateNodes(
      std::unique_ptr<OpIterator> plan);
  absl::StatusOr<std::unique_ptr<OpIterator>> ProcessOrderBy(
      std::unique_ptr<OpIterator> plan);

  // Returns InvalidArgumentError if this reference is invalid.
  absl::Status CheckColumnRef(ColumnRef ref);
};

};  // namespace komfydb::execution::logical_plan

#endif  // __LOGICAL_PLAN_H__
