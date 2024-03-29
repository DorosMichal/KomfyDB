#include "komfydb/parser.h"
#include <hsql/sql/ShowStatement.h>

#include <vector>

#include "glog/logging.h"

#include "hsql/sql/Expr.h"
#include "hsql/sql/SQLStatement.h"
#include "hsql/sql/SelectStatement.h"
#include "hsql/sql/Table.h"
#include "hsql/util/sqlhelper.h"

#include "komfydb/common/column_ref.h"
#include "komfydb/common/string_field.h"
#include "komfydb/execution/fixed_iterator.h"
#include "komfydb/execution/insert.h"
#include "komfydb/execution/limit.h"
#include "komfydb/execution/logical_plan/join_node.h"
#include "komfydb/execution/order_by.h"
#include "komfydb/utils/status_macros.h"

namespace {

using komfydb::common::ColumnRef;
using komfydb::common::Type;

std::string GetErrorMessage(std::string_view query,
                            hsql::SQLParserResult& result) {
  std::string indication = absl::StrCat(
      std::string(result.errorColumn(), ' ') + "^ ", result.errorMsg());
  std::string msg =
      absl::StrCat("Parsing error:\n\t", query, "\n\t", indication);
  return msg;
}

absl::StatusOr<Op> HsqlOpToOp(hsql::OperatorType op_type) {
  switch (op_type) {
    case hsql::OperatorType::kOpGreater:
      return Op(Op::Value::GREATER_THAN);
    case hsql::OperatorType::kOpGreaterEq:
      return Op(Op::Value::GREATER_THAN_OR_EQ);
    case hsql::OperatorType::kOpLess:
      return Op(Op::Value::LESS_THAN);
    case hsql::OperatorType::kOpLessEq:
      return Op(Op::Value::LESS_THAN_OR_EQ);
    case hsql::OperatorType::kOpLike:
      return Op(Op::Value::LIKE);
    case hsql::OperatorType::kOpEquals:
      return Op(Op::Value::EQUALS);
    case hsql::OperatorType::kOpNotEquals:
      return Op(Op::Value::NOT_EQUALS);
    default:
      return absl::InvalidArgumentError(
          absl::StrCat("Unsupported binary operator ", op_type));
  }
}

bool isConstantExpr(const hsql::Expr* expr) {
  return expr->type == hsql::ExprType::kExprColumnRef ||
         expr->type == hsql::ExprType::kExprLiteralInt ||
         expr->type == hsql::ExprType::kExprLiteralString;
}

absl::Status CompareTypes(ColumnRef lref, ColumnRef rref, LogicalPlan& lp) {
  ASSIGN_OR_RETURN(Type lt, lp.GetColumnType(lref));
  ASSIGN_OR_RETURN(Type rt, lp.GetColumnType(rref));

  if (lt != rt) {
    return absl::InvalidArgumentError(absl::StrCat(
        "Cannot compare columns with different type: ",
        static_cast<std::string>(lref), ":", static_cast<std::string>(lt), " ",
        static_cast<std::string>(rref), ":", static_cast<std::string>(rt)));
  }
  return absl::OkStatus();
}

static const std::string unsuported_msg =
    "Only supported operators are e1 AND e2 and simple binary "
    "expressions like A op B, where A, B are constatns, table columns "
    "or B is a subquery.";

absl::StatusOr<Tuple> GetTuple(std::vector<hsql::Expr*>* values,
                               TupleDesc* tuple_desc) {
  if (values->size() != tuple_desc->Length()) {
    return absl::InvalidArgumentError(
        absl::StrCat("Invalid number of values: ", values->size(),
                     " instead of ", tuple_desc->Length()));
  }
  std::vector<TDItem> tditems = tuple_desc->GetItems();
  Tuple result(tuple_desc->Length());
  for (int i = 0; i < values->size(); i++) {
    hsql::Expr* value = (*values)[i];
    TDItem tditem = tditems[i];
    switch (value->type) {
      case hsql::ExprType::kExprLiteralInt: {
        if (tditem.field_type != Type::INT) {
          return absl::InvalidArgumentError(absl::StrCat(
              "Invalid value ", i + 1, " type, string instead of int."));
        }
        RETURN_IF_ERROR(
            result.SetField(i, std::make_unique<IntField>(value->ival)));
        break;
      }
      case hsql::ExprType::kExprLiteralString: {
        if (tditem.field_type != Type::STRING) {
          return absl::InvalidArgumentError(absl::StrCat(
              "Invalid value ", i + 1, " type, int instead of string."));
        }
        RETURN_IF_ERROR(
            result.SetField(i, std::make_unique<StringField>(value->name)));
        break;
      }
      default: {
        return absl::InvalidArgumentError("Unsupported value type.");
      }
    }
  }
  return std::move(result);
}

};  // namespace

namespace komfydb {

Parser::Parser(std::shared_ptr<Catalog> catalog,
               std::shared_ptr<BufferPool> buffer_pool,
               TableStatsMap& table_stats_map)
    : catalog(std::move(catalog)),
      buffer_pool(std::move(buffer_pool)),
      table_stats_map(table_stats_map) {}

absl::Status Parser::ParseFromClause(LogicalPlan& lp,
                                     const hsql::TableRef* from) {
  std::vector<std::string_view> names;
  std::vector<std::string_view> aliases;
  std::vector<int> ids;

  switch (from->type) {
    case hsql::TableRefType::kTableName:
      names.push_back(from->name);
      aliases.push_back(from->getName());
      break;
    case hsql::TableRefType::kTableCrossProduct:
      for (const hsql::TableRef* tbl : *from->list) {
        if (tbl->type != hsql::TableRefType::kTableName) {
          return absl::InvalidArgumentError(
              "Only table names/aliases in FROM currently supported");
        }
        names.push_back(tbl->name);
        aliases.push_back(tbl->getName());
      }
      break;
    default:
      return absl::InvalidArgumentError(
          "Only table names/aliases in FROM currently supported");
  }

  for (auto name : names) {
    ASSIGN_OR_RETURN(int id, catalog->GetTableId(name));
    ids.push_back(id);
  }

  for (int i = 0; i < aliases.size(); i++) {
    RETURN_IF_ERROR(lp.AddScan(ids[i], aliases[i]));
  }

  return absl::OkStatus();
}

absl::Status Parser::ParseSimpleExpression(LogicalPlan& lp, hsql::Expr* lexpr,
                                           Op op, hsql::Expr* rexpr) {
  LOG(INFO) << "Parsing simple expression";
  bool is_join = false;
  // This expr creates join if and only if we have a binary expr A op B where
  // - Both A, B are column references from different tables,
  // - A is a column reference and B is a nested query
  if (lexpr->isType(hsql::ExprType::kExprColumnRef) &&
      rexpr->isType(hsql::ExprType::kExprColumnRef)) {
    ASSIGN_OR_RETURN(ColumnRef lref,
                     lp.GetColumnRef(lexpr->table, lexpr->name));
    ASSIGN_OR_RETURN(ColumnRef rref,
                     lp.GetColumnRef(rexpr->table, rexpr->name));
    if (lref.table != rref.table) {
      is_join = true;
    }
  } else if (rexpr->isType(hsql::ExprType::kExprSelect)) {
    if (!lexpr->isType(hsql::ExprType::kExprColumnRef)) {
      return absl::InvalidArgumentError(
          absl::StrCat("Expression with nested query requires left operand to "
                       "be a column reference."));
    }
    is_join = true;
  }
  if (!isConstantExpr(lexpr) ||
      (!isConstantExpr(rexpr) && !rexpr->isType(hsql::ExprType::kExprSelect))) {
    return absl::InvalidArgumentError(unsuported_msg);
  }
  LOG(INFO) << "Is_join: " << is_join;

  if (is_join) {
    ASSIGN_OR_RETURN(ColumnRef lref,
                     lp.GetColumnRef(lexpr->table, lexpr->name));
    if (rexpr->isType(hsql::ExprType::kExprColumnRef)) {
      ASSIGN_OR_RETURN(ColumnRef rref,
                       lp.GetColumnRef(rexpr->table, rexpr->name));
      RETURN_IF_ERROR(CompareTypes(lref, rref, lp));
      RETURN_IF_ERROR(lp.AddJoin(lref, op, rref));
      return absl::OkStatus();
    }
    // Else it's a subquery join.
    ASSIGN_OR_RETURN(LogicalPlan subplan, GenerateLogicalPlan(rexpr->select));
    RETURN_IF_ERROR(lp.AddSubqueryJoin(lref, op, std::move(subplan)));
    return absl::OkStatus();
  }

  // Else we got a filter.
  if (lexpr->isLiteral() && rexpr->isLiteral()) {
    return absl::InvalidArgumentError(
        absl::StrCat("Filters with two literals are unsupported."));
  }
  // For convenience, make left always a column ref.
  if (lexpr->isLiteral()) {
    std::swap(lexpr, rexpr);
    op.Flip();
  }
  ASSIGN_OR_RETURN(ColumnRef lref, lp.GetColumnRef(lexpr->table, lexpr->name));

  // column op const
  if (rexpr->isLiteral()) {
    ASSIGN_OR_RETURN(Type t, lp.GetColumnType(lref));
    std::unique_ptr<Field> const_field;
    switch (t.GetValue()) {
      case Type::INT:
        if (rexpr->type != hsql::ExprType::kExprLiteralInt) {
          return absl::InvalidArgumentError(
              absl::StrCat("Cannot compare string literal with int column ",
                           static_cast<std::string>(lref)));
        }
        const_field = std::make_unique<IntField>(rexpr->ival);
        break;
      case Type::STRING:
        if (rexpr->type != hsql::ExprType::kExprLiteralString) {
          return absl::InvalidArgumentError(
              absl::StrCat("Cannot compare int literal with string column ",
                           static_cast<std::string>(lref)));
        }
        const_field = std::make_unique<StringField>(rexpr->name);
        break;
    }
    RETURN_IF_ERROR(lp.AddFilterColConst(lref, op, std::move(const_field)));
    return absl::OkStatus();
  }

  // column op column
  ASSIGN_OR_RETURN(ColumnRef rref, lp.GetColumnRef(rexpr->table, rexpr->name));
  RETURN_IF_ERROR(CompareTypes(lref, rref, lp));
  RETURN_IF_ERROR(lp.AddFilterColCol(lref, op, rref));

  return absl::OkStatus();
}

absl::Status Parser::ParseWhereExpression(LogicalPlan& lp, hsql::Expr* expr) {
  if (expr == nullptr) {
    return absl::OkStatus();
  }
  switch (expr->type) {
    case hsql::ExprType::kExprOperator:
      switch (expr->opType) {
        case hsql::OperatorType::kOpAnd:
          RETURN_IF_ERROR(ParseWhereExpression(lp, expr->expr));
          RETURN_IF_ERROR(ParseWhereExpression(lp, expr->expr2));
          break;
        default:
          ASSIGN_OR_RETURN(Op op, HsqlOpToOp(expr->opType));
          RETURN_IF_ERROR(
              ParseSimpleExpression(lp, expr->expr, op, expr->expr2));
          break;
      }
      break;
    default:
      return absl::InvalidArgumentError(unsuported_msg);
  }
  return absl::OkStatus();
}

absl::Status Parser::ParseGroupBy(LogicalPlan& lp,
                                  hsql::GroupByDescription* descr) {
  if (descr == nullptr) {
    return absl::OkStatus();
  }
  if (descr->having) {
    return absl::InvalidArgumentError("Having expressions are unsupported.");
  }

  for (auto expr = descr->columns->begin(); expr != descr->columns->end();
       expr++) {
    if (!(*expr)->isType(hsql::ExprType::kExprColumnRef)) {
      return absl::InvalidArgumentError(
          "GroupBy expression must be a column reference.");
    }
    ASSIGN_OR_RETURN(ColumnRef ref,
                     lp.GetColumnRef((*expr)->table, (*expr)->name));
    RETURN_IF_ERROR(lp.AddGroupBy(ref));
  }

  return absl::OkStatus();
}

absl::Status Parser::ParseColumnSelection(LogicalPlan& lp,
                                          std::vector<hsql::Expr*>* columns) {
  for (auto& it : *columns) {
    ColumnRef ref;
    hsql::Expr* col;
    switch (it->type) {
      case hsql::ExprType::kExprFunctionRef: {
        if (it->exprList == nullptr || it->exprList->size() != 1) {
          return absl::InvalidArgumentError(absl::StrCat(
              "Only one column name as aggregate argument supported."));
        }
        col = it->exprList->back();
        if (!col->isType(hsql::ExprType::kExprColumnRef) &&
            !col->isType(hsql::ExprType::kExprStar)) {
          return absl::InvalidArgumentError(absl::StrCat(
              "Only one column name as aggregate argument supported."));
        }
        if (col->isType(hsql::ExprType::kExprStar)) {
          ref = common::COLUMN_REF_STAR;
        } else {
          ASSIGN_OR_RETURN(ref, lp.GetColumnRef(col->table, col->name));
        }
        RETURN_IF_ERROR(lp.AddAggregate(it->name, ref));
        break;
      }
      case hsql::ExprType::kExprColumnRef: {
        ASSIGN_OR_RETURN(ref, lp.GetColumnRef(it->table, it->name));
        RETURN_IF_ERROR(lp.AddSelect(ref));
        break;
      }
      case hsql::ExprType::kExprStar: {
        RETURN_IF_ERROR(lp.AddSelect(common::COLUMN_REF_STAR));
        break;
      }
      default:
        return absl::InvalidArgumentError(
            "Possible selects are only star, aggregates and column "
            "references.");
    }
  }
  return absl::OkStatus();
}

absl::Status Parser::ParseOrderBy(LogicalPlan& lp,
                                  std::vector<hsql::OrderDescription*>* descr) {
  if (!descr) {
    return absl::OkStatus();
  }

  if (descr->size() != 1) {
    return absl::InvalidArgumentError("Order by only by one column supported.");
  }
  hsql::OrderDescription* order_descr = descr->back();
  hsql::Expr* expr = order_descr->expr;
  execution::OrderBy::Order asc =
      order_descr->type == hsql::OrderType::kOrderAsc
          ? execution::OrderBy::ASCENDING
          : execution::OrderBy::DESCENDING;

  if (!expr->isType(hsql::ExprType::kExprColumnRef)) {
    return absl::InvalidArgumentError("Order by only by one column supported.");
  }

  ASSIGN_OR_RETURN(ColumnRef ref, lp.GetColumnRef(expr->table, expr->name));
  RETURN_IF_ERROR(lp.AddOrderBy(ref, asc));
  return absl::OkStatus();
}

absl::Status Parser::ParseLimit(LogicalPlan& lp,
                                const hsql::LimitDescription* limit) {
  if (!limit) {
    return absl::OkStatus();
  }
  hsql::Expr* limit_expr = limit->limit;
  if (!limit_expr->isType(hsql::kExprLiteralInt)) {
    return absl::InvalidArgumentError("Only literal int limit supported.");
  }

  return lp.AddLimit(limit_expr->ival);
}

absl::StatusOr<LogicalPlan> Parser::GenerateLogicalPlan(
    const hsql::SelectStatement* stmt) {
  LogicalPlan logical_plan(catalog, buffer_pool);

  LOG(INFO) << "Parsing From clause";
  RETURN_IF_ERROR(ParseFromClause(logical_plan, stmt->fromTable));
  LOG(INFO) << "Parsing where clause";
  RETURN_IF_ERROR(ParseWhereExpression(logical_plan, stmt->whereClause));
  LOG(INFO) << "Parsing group by";
  RETURN_IF_ERROR(ParseGroupBy(logical_plan, stmt->groupBy));
  LOG(INFO) << "Parsing column selection";
  RETURN_IF_ERROR(ParseColumnSelection(logical_plan, stmt->selectList));
  LOG(INFO) << "Parsing order by";
  RETURN_IF_ERROR(ParseOrderBy(logical_plan, stmt->order));
  LOG(INFO) << "Parsing limit";
  RETURN_IF_ERROR(ParseLimit(logical_plan, stmt->limit));

  return std::move(logical_plan);
}

absl::StatusOr<std::unique_ptr<OpIterator>> Parser::ParseSelectStatement(
    const hsql::SelectStatement* stmt, TransactionId tid,
    bool explain_optimizer) {
  ASSIGN_OR_RETURN(LogicalPlan logical_plan, GenerateLogicalPlan(stmt));

  return logical_plan.GeneratePhysicalPlan(tid, table_stats_map,
                                           explain_optimizer);
}

absl::StatusOr<std::unique_ptr<OpIterator>> Parser::ParseInsertStatement(
    const hsql::InsertStatement* stmt, TransactionId tid) {
  ASSIGN_OR_RETURN(int table_id, catalog->GetTableId(stmt->tableName));
  ASSIGN_OR_RETURN(TupleDesc * tuple_desc, catalog->GetTupleDesc(table_id));
  std::unique_ptr<OpIterator> subiterator;

  switch (stmt->type) {
    case hsql::InsertType::kInsertSelect: {
      ASSIGN_OR_RETURN(subiterator,
                       ParseSelectStatement(stmt->select, tid,
                                            /*explain_optimizer=*/false));
      break;
    }
    case hsql::InsertType::kInsertValues: {
      ASSIGN_OR_RETURN(Tuple tuple, GetTuple(stmt->values, tuple_desc));
      Record record(std::move(tuple), RecordId(PageId(0, 0), -1));
      ASSIGN_OR_RETURN(subiterator, execution::FixedIterator::Create(
                                        *tuple_desc, {std::move(record)}));
      break;
    }
  }

  return execution::Insert::Create(std::move(subiterator), table_id,
                                   buffer_pool, tid);
}

absl::StatusOr<Query> Parser::ParseCreateStatement(
    const hsql::CreateStatement* stmt) {
  if (stmt->type != hsql::CreateType::kCreateTable) {
    return absl::UnimplementedError("Only CREATE TABLE supported.");
  }

  std::string table_name = stmt->tableName;
  std::vector<Type> types;
  std::vector<std::string> names;
  std::string primary_key = "";
  for (int i = 0; i < stmt->columns->size(); i++) {
    hsql::ColumnDefinition* column_def = (*(stmt->columns))[i];
    names.push_back(column_def->name);
    switch (column_def->type.data_type) {
      case hsql::DataType::INT: {
        types.push_back(Type::INT);
        break;
      }
      case hsql::DataType::TEXT: {
        types.push_back(Type::STRING);
        break;
      }
      default: {
        return absl::UnimplementedError(
            absl::StrCat("Only INT and TEXT column types supported."));
      }
    }
    if (column_def->column_constraints->count(
            hsql::ConstraintType::PrimaryKey)) {
      primary_key = column_def->name;
    }
  }
  if (primary_key == "") {
    primary_key = names[0];
  }

  TupleDesc tuple_desc(types, names);
  return Query(table_name, tuple_desc, primary_key);
}

absl::StatusOr<Query> Parser::ParseShowStatement(
    const hsql::ShowStatement* stmt) {
  switch (stmt->type) {
    case hsql::ShowType::kShowTables: {
      return Query();
    }
    case hsql::ShowType::kShowColumns: {
      return Query(stmt->name);
    }
  }
}

absl::StatusOr<Query> Parser::ParseQuery(std::string_view query,
                                         TransactionId tid,
                                         bool explain_optimizer) {
  LOG(INFO) << "Parsing query: " << query;
  hsql::SQLParserResult result;
  hsql::SQLParser::parse(std::string(query), &result);
  if (!result.isValid()) {
    return absl::InvalidArgumentError(GetErrorMessage(query, result));
  }

  const std::vector<hsql::SQLStatement*> stmts = result.getStatements();
  if (stmts.size() != 1) {
    return absl::UnimplementedError(
        "Currently only one statement per query is supported.");
  }

  const hsql::SQLStatement* stmt = stmts.back();

  switch (stmt->type()) {
    case hsql::StatementType::kStmtSelect: {
      return ParseSelectStatement(
          static_cast<const hsql::SelectStatement*>(stmt), tid,
          explain_optimizer);
    }
    case hsql::StatementType::kStmtInsert: {
      return ParseInsertStatement(
          static_cast<const hsql::InsertStatement*>(stmt), tid);
    }
    case hsql::StatementType::kStmtCreate: {
      return ParseCreateStatement(
          static_cast<const hsql::CreateStatement*>(stmt));
    }
    case hsql::StatementType::kStmtShow: {
      return ParseShowStatement(static_cast<const hsql::ShowStatement*>(stmt));
    }
    default: {
      return absl::UnimplementedError(
          "Currently only select and insert statements are supported.");
    }
  }
}

};  // namespace komfydb
