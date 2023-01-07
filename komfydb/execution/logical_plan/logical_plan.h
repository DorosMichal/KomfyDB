#ifndef __LOGICAL_PLAN_H__
#define __LOGICAL_PLAN_H__

#include <string>
#include <string_view>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/status/statusor.h"
#include "hsql/SQLParser.h"

#include "komfydb/common/column_ref.h"
#include "komfydb/execution/aggregator.h"
#include "komfydb/execution/logical_plan/filter_node.h"
#include "komfydb/execution/logical_plan/join_node.h"
#include "komfydb/execution/logical_plan/scan_node.h"
#include "komfydb/execution/logical_plan/select_node.h"
#include "komfydb/execution/op_iterator.h"
#include "komfydb/execution/order_by.h"
#include "komfydb/storage/catalog.h"

namespace {

using komfydb::common::ColumnRef;
using komfydb::storage::Catalog;

};  // namespace

namespace komfydb::execution::logical_plan {

class LogicalPlan {
 public:
  LogicalPlan(std::shared_ptr<Catalog> catalog) : catalog(std::move(catalog)) {}

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

  void Dump();

 private:
  // Returns InvalidArgumentError if this reference is invalid.
  absl::Status CheckColumnRef(ColumnRef ref);

  std::shared_ptr<Catalog> catalog;
  std::vector<FilterNode> filters;
  std::vector<JoinNode> joins;
  std::vector<SelectNode> selects;
  std::vector<ScanNode> scans;

  absl::flat_hash_map<std::string, int> alias_to_id;
  Aggregator::AggregateType agg_type = Aggregator::NONE;
  ColumnRef agg_column = {};
  ColumnRef group_by_column = {};
  ColumnRef order_by_column = {};
  OrderBy::Order order;
};

};  // namespace komfydb::execution::logical_plan

#endif  // __LOGICAL_PLAN_H__
