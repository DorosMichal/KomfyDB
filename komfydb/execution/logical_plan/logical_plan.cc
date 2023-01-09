#include "komfydb/execution/logical_plan/logical_plan.h"

#include <iostream>
#include <memory>
#include <string_view>

#include "komfydb/common/tuple_desc.h"
#include "komfydb/execution/seq_scan.h"
#include "komfydb/optimizer/join_optimizer.h"
#include "komfydb/storage/table_iterator.h"
#include "komfydb/utils/status_macros.h"

namespace {

using komfydb::common::ColumnRef;
using komfydb::common::TupleDesc;
using komfydb::common::Type;
using komfydb::storage::TableIterator;

// Find (like in Union-Find).
std::string GetEquiv(std::string& name,
                     absl::flat_hash_map<std::string, std::string>& equiv) {
  if (equiv[name] == name)
    return name;
  return (equiv[name] = GetEquiv(equiv[name], equiv));
}

std::vector<std::string> GetSubplanRoots(
    absl::flat_hash_map<std::string, std::string>& equiv) {
  std::vector<std::string> result;
  for (auto& [table, ref_table] : equiv) {
    result.push_back(ref_table);
  }
  std::sort(result.begin(), result.end());
  result.erase(result.begin(), std::unique(result.begin(), result.end()));
  return result;
}

};  // namespace

namespace komfydb::execution::logical_plan {

absl::Status LogicalPlan::CheckColumnRef(ColumnRef ref) {
  if (ref.IsStar())
    return absl::OkStatus();
  auto it = alias_to_id.find(ref.table);
  if (it == alias_to_id.end()) {
    return absl::InvalidArgumentError(
        absl::StrCat("Unknown alias ", ref.table));
  }
  ASSIGN_OR_RETURN(TupleDesc * tuple_desc, catalog->GetTupleDesc(it->second));
  return tuple_desc->IndexForFieldName(ref.column).status();
}

absl::Status LogicalPlan::AddScan(int table_id, std::string_view alias) {
  if (alias_to_id.contains(alias)) {
    return absl::InvalidArgumentError(
        absl::StrCat("Another use of alias ", alias));
  }
  scans.push_back(ScanNode(table_id, alias));
  alias_to_id[alias] = table_id;
  return absl::OkStatus();
}

absl::Status LogicalPlan::AddJoin(ColumnRef lref, Op op, ColumnRef rref) {
  RETURN_IF_ERROR(CheckColumnRef(lref));
  RETURN_IF_ERROR(CheckColumnRef(rref));
  joins.push_back(JoinNode(lref, op, rref));
  return absl::OkStatus();
}

absl::Status LogicalPlan::AddSubqueryJoin(ColumnRef ref, Op op,
                                          LogicalPlan subplan) {
  RETURN_IF_ERROR(CheckColumnRef(ref));
  std::unique_ptr<LogicalPlan> subplan_ptr =
      std::make_unique<LogicalPlan>(std::move(subplan));
  joins.push_back(JoinNode(ref, op, std::move(subplan_ptr)));
  return absl::OkStatus();
}

absl::Status LogicalPlan::AddFilterColCol(ColumnRef lref, Op op,
                                          ColumnRef rref) {
  RETURN_IF_ERROR(CheckColumnRef(lref));
  RETURN_IF_ERROR(CheckColumnRef(rref));
  filters.push_back(FilterNode(lref, op, rref));
  return absl::OkStatus();
}

absl::Status LogicalPlan::AddFilterColConst(
    ColumnRef ref, Op op, std::unique_ptr<Field> const_field) {
  RETURN_IF_ERROR(CheckColumnRef(ref));
  filters.push_back(FilterNode(ref, op, std::move(const_field)));
  return absl::OkStatus();
}

absl::Status LogicalPlan::AddGroupBy(ColumnRef ref) {
  RETURN_IF_ERROR(CheckColumnRef(ref));
  group_by_column = ref;
  return absl::OkStatus();
}

absl::Status LogicalPlan::AddAggregate(std::string_view agg_fun,
                                       ColumnRef ref) {
  if (agg_column.IsValid()) {
    return absl::InvalidArgumentError(
        "Only one aggregate function supported at the moment.");
  }
  ASSIGN_OR_RETURN(agg_type, Aggregate::GetAggregateType(agg_fun));
  RETURN_IF_ERROR(CheckColumnRef(ref));
  agg_column = ref;
  return absl::OkStatus();
}

absl::Status LogicalPlan::AddSelect(ColumnRef ref) {
  RETURN_IF_ERROR(CheckColumnRef(ref));
  selects.push_back(SelectNode(ref));
  return absl::OkStatus();
}

absl::Status LogicalPlan::AddOrderBy(ColumnRef ref, OrderBy::Order asc) {
  if (order_by_column.IsValid()) {
    return absl::InvalidArgumentError(
        "Only one order by column supported at the moment.");
  }
  RETURN_IF_ERROR(CheckColumnRef(ref));
  order_by_column = ref;
  order = asc;
  return absl::OkStatus();
}

absl::StatusOr<ColumnRef> LogicalPlan::GetColumnRef(std::string_view table,
                                                    std::string_view column) {
  if (table != "") {
    return ColumnRef(table, column);
  }

  for (auto& it : alias_to_id) {
    ASSIGN_OR_RETURN(TupleDesc * tuple_desc, catalog->GetTupleDesc(it.second));
    if (tuple_desc->IndexForFieldName(column).ok()) {
      if (table != "") {
        return absl::InvalidArgumentError(
            absl::StrCat("Column ", column, " without table name is ambigous"));
      }
      table = it.first;
    }
  }

  if (table == "") {
    return absl::InvalidArgumentError(
        absl::StrCat("Column ", column, " does not appear in any table"));
  }
  return ColumnRef(table, column);
}

absl::StatusOr<common::ColumnRef> LogicalPlan::GetColumnRef(char* table,
                                                            char* column) {
  return GetColumnRef(table ? table : "", column ? column : "");
}

absl::StatusOr<common::Type> LogicalPlan::GetColumnType(ColumnRef ref) {
  auto it = alias_to_id.find(ref.table);
  if (it == alias_to_id.end()) {
    return absl::InvalidArgumentError(absl::StrCat(
        "Invalid column reference: no table with alias ", ref.table));
  }
  ASSIGN_OR_RETURN(TupleDesc * tuple_desc, catalog->GetTupleDesc(it->second));
  ASSIGN_OR_RETURN(int column_idx, tuple_desc->IndexForFieldName(ref.column));
  ASSIGN_OR_RETURN(Type t, tuple_desc->GetFieldType(column_idx));
  return t;
}

absl::Status LogicalPlan::ProcessScanNodes(TransactionId tid) {
  for (auto& scan : scans) {
    std::unique_ptr<TableIterator> table_iterator =
        std::make_unique<TableIterator>(tid, scan.id, catalog, buffer_pool);
    ASSIGN_OR_RETURN(std::unique_ptr<SeqScan> seq_scan,
                     SeqScan::Create(std::move(table_iterator), tid, scan.id));
    subplans[scan.alias] = std::move(seq_scan);
    equiv[scan.alias] = scan.alias;
    filter_selectivities[scan.alias] = 1.0;
  }
  return absl::OkStatus();
}

absl::Status LogicalPlan::ProcessFilterNodes(TableStatsMap& table_stats) {
  // TODO: Filter OpIterator
  for (auto& filter : filters) {
    std::string table = filter.lcol.table;
    std::unique_ptr<OpIterator> subplan = std::move(subplans[table]);
    int lcol =
        subplan->GetTupleDesc()->IndexForFieldName(filter.lcol.column).value();
    double selectivity;
    switch (filter.type) {
      case FilterNode::COLUMN_CONSTANT: {
        selectivity = table_stats[table].EstimateSelectivity(
            lcol, filter.op, filter.constant.get());

        return absl::UnimplementedError(
            "Filter OpIterator not implemented yet.");
        break;
      }
      case FilterNode::TWO_COLUMNS: {
        int rcol = subplan->GetTupleDesc()
                       ->IndexForFieldName(filter.rcol.column)
                       .value();
        selectivity =
            table_stats[table].EstimateSelectivity(lcol, filter.op, rcol);

        return absl::UnimplementedError(
            "Filter OpIterator not implemented yet.");
        break;
      }
    }
    filter_selectivities[table] *= selectivity;
  }
  return absl::OkStatus();
}

absl::Status LogicalPlan::ProcessJoinNodes(TransactionId tid,
                                           TableStatsMap& table_stats,
                                           bool explain) {
  optimizer::JoinOptimizer join_optimizer = optimizer::JoinOptimizer(catalog);
  RETURN_IF_ERROR(join_optimizer.OrderJoins(joins));

  for (auto& join : joins) {
    std::unique_ptr<OpIterator> lsubplan, rsubplan;
    std::string ltable = GetEquiv(ltable, equiv);
    std::string rtable = "";
    lsubplan = std::move(subplans[ltable]);

    switch (join.type) {
      case JoinNode::COL_COL: {
        rtable = GetEquiv(join.rref.table, equiv);
        rsubplan = std::move(subplans[rtable]);
        break;
      }
      case JoinNode::COL_SUB: {
        ASSIGN_OR_RETURN(rsubplan, join.subplan->GeneratePhysicalPlan(
                                       tid, table_stats, explain));
        break;
      }
    }

    if (!rtable.size()) {
      // Union (like in Union-Find).
      equiv[rtable] = ltable;
    }
    // TODO: Instatiate Join
    // subplan[ltable] = newly created join
  }

  return absl::OkStatus();
}

absl::StatusOr<std::unique_ptr<OpIterator>> LogicalPlan::ProcessSelectNodes(
    std::unique_ptr<OpIterator> plan) {
  std::vector<int> out_columns;
  std::vector<Type> out_types;

  for (auto& select : selects) {}
}

absl::StatusOr<std::unique_ptr<OpIterator>> LogicalPlan::GeneratePhysicalPlan(
    TransactionId tid, TableStatsMap& table_stats, bool explain) {
  equiv.clear();
  filter_selectivities.clear();
  subplans.clear();

  RETURN_IF_ERROR(ProcessScanNodes(tid));
  RETURN_IF_ERROR(ProcessFilterNodes(table_stats));
  RETURN_IF_ERROR(ProcessJoinNodes(tid, table_stats, explain));

  std::vector<std::string> subplan_roots = GetSubplanRoots(equiv);
  if (subplan_roots.size() != 1) {
    return absl::UnimplementedError("Cartesian product not supported yet.");
  }
  std::unique_ptr<OpIterator> final_plan =
      std::move(subplans[subplan_roots.back()]);
  ASSIGN_OR_RETURN(final_plan, ProcessSelectNodes(std::move(final_plan)));

  return final_plan;
}

void LogicalPlan::Dump() {
  std::cout << "alias_to_id: (" << alias_to_id.size() << ")\n";

  for (auto& it : alias_to_id) {
    std::cout << it.first << " -> " << it.second << "\n";
  }

  std::cout << "Scan nodes: (" << scans.size() << ")\n";
  for (auto& it : scans) {
    std::cout << static_cast<std::string>(it) << "\n";
  }

  std::cout << "Select nodes: (" << selects.size() << ")\n";
  for (auto& it : selects) {
    std::cout << static_cast<std::string>(it) << "\n";
  }

  std::cout << "Filter nodes: (" << filters.size() << ")\n";
  for (auto& it : filters) {
    std::cout << static_cast<std::string>(it) << "\n";
  }

  std::cout << "Join nodes: (" << joins.size() << ")\n";
  for (auto& it : joins) {
    std::cout << static_cast<std::string>(it.lref) + " " +
                     static_cast<std::string>(it.op)
              << " ";
    if (it.type == JoinNode::COL_COL) {
      std::cout << static_cast<std::string>(it.rref) << "\n";
    } else {
      std::cout << "Subplan(";
      it.subplan->Dump();
      std::cout << ")\n";
    }
  }

  std::cout << "GroupBy: " << static_cast<std::string>(group_by_column) << "\n";
  std::cout << "Aggregate: " << Aggregate::AggregateTypeToString(agg_type)
            << "(" << static_cast<std::string>(agg_column) << ")\n";

  std::cout << "OrderBy: " << static_cast<std::string>(order_by_column) << " ";
  std::cout << (order == OrderBy::ASCENDING ? "asc" : "desc") << "\n";
}

};  // namespace komfydb::execution::logical_plan
