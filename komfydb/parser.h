#ifndef __PARSER_H__
#define __PARSER_H__

#include <hsql/sql/SelectStatement.h>
#include "absl/status/statusor.h"
#include "hsql/SQLParser.h"

#include "komfydb/execution/logical_plan/logical_plan.h"
#include "komfydb/execution/op_iterator.h"

namespace {

using komfydb::execution::OpIterator;
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
      const hsql::SelectStatement* stmt, TransactionId tid, uint64_t* limit,
      bool explain_optimizer);

  absl::StatusOr<std::unique_ptr<OpIterator>> ParseInsertStatement(
      const hsql::InsertStatement* stmt, TransactionId tid);

  absl::Status ParseFromClause(LogicalPlan& lp, const hsql::TableRef* from);

  absl::Status ParseSimpleExpression(LogicalPlan& lp, hsql::Expr* lexpr, Op op,
                                     hsql::Expr* rexpr);

  absl::Status ParseWhereExpression(LogicalPlan& lp, hsql::Expr* expr);

  absl::Status ParseGroupBy(LogicalPlan& lp, hsql::GroupByDescription* descr);

  absl::Status ParseColumnSelection(LogicalPlan& lp,
                                    std::vector<hsql::Expr*>* columns);

  absl::Status ParseOrderBy(LogicalPlan& lp,
                            std::vector<hsql::OrderDescription*>* descr);

 public:
  Parser(std::shared_ptr<Catalog> catalog,
         std::shared_ptr<BufferPool> buffer_pool,
         TableStatsMap& table_stats_map);

  absl::StatusOr<std::unique_ptr<OpIterator>> ParseQuery(
      std::string_view query, TransactionId tid, uint64_t* limit,
      bool explain_optimizer);
};

};  // namespace komfydb

#endif  // __PARSER_H__
