#include "komfydb/execution/logical_plan/logical_plan.h"

#include <iostream>
#include <memory>
#include <string_view>

#include "komfydb/common/tuple_desc.h"
#include "komfydb/optimizer/join_optimizer.h"
#include "komfydb/utils/status_macros.h"

namespace {

using komfydb::common::ColumnRef;
using komfydb::common::TupleDesc;
using komfydb::common::Type;

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
