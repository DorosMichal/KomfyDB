#ifndef __PARSER_H__
#define __PARSER_H__

#include <hsql/sql/SelectStatement.h>
#include "absl/status/statusor.h"
#include "hsql/SQLParser.h"

#include "komfydb/execution/logical_plan/logical_plan.h"
#include "komfydb/execution/op_iterator.h"
#include "komfydb/execution/query.h"

namespace {

using komfydb::execution::OpIterator;
using komfydb::execution::Query;
using komfydb::execution::logical_plan::LogicalPlan;

};  // namespace

namespace komfydb {

class Parser {
 private:
  std::shared_ptr<Catalog> catalog;
  std::shared_ptr<BufferPool> buffer_pool;
  TableStatsMap& table_stats_map;

  absl::StatusOr<LogicalPlan> GenerateLogicalPlan(
      const hsql::SelectStatement* stmt);

  absl::StatusOr<std::unique_ptr<OpIterator>> ParseSelectStatement(
      const hsql::SelectStatement* stmt, TransactionId tid,
      bool explain_optimizer);

  absl::StatusOr<std::unique_ptr<OpIterator>> ParseInsertStatement(
      const hsql::InsertStatement* stmt, TransactionId tid);

  absl::StatusOr<Query> ParseCreateStatement(const hsql::CreateStatement* stmt);

  absl::StatusOr<Query> ParseShowStatement(const hsql::ShowStatement* stmt);

  absl::Status ParseFromClause(LogicalPlan& lp, const hsql::TableRef* from);

  absl::Status ParseSimpleExpression(LogicalPlan& lp, hsql::Expr* lexpr, Op op,
                                     hsql::Expr* rexpr);

  absl::Status ParseWhereExpression(LogicalPlan& lp, hsql::Expr* expr);

  absl::Status ParseGroupBy(LogicalPlan& lp, hsql::GroupByDescription* descr);

  absl::Status ParseColumnSelection(LogicalPlan& lp,
                                    std::vector<hsql::Expr*>* columns);

  absl::Status ParseOrderBy(LogicalPlan& lp,
                            std::vector<hsql::OrderDescription*>* descr);

  absl::Status ParseLimit(LogicalPlan& lp, const hsql::LimitDescription* limit);

 public:
  Parser(std::shared_ptr<Catalog> catalog,
         std::shared_ptr<BufferPool> buffer_pool,
         TableStatsMap& table_stats_map);

  absl::StatusOr<Query> ParseQuery(std::string_view query, TransactionId tid,
                                   bool explain_optimizer);
};

};  // namespace komfydb

#endif  // __PARSER_H__
