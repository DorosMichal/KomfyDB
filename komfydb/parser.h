#ifndef __PARSER_H__
#define __PARSER_H__

#include <hsql/sql/SelectStatement.h>
#include "absl/status/statusor.h"
#include "hsql/SQLParser.h"

#include "komfydb/execution/logical_plan/logical_plan.h"

namespace {

using komfydb::execution::logical_plan::LogicalPlan;

};

namespace komfydb {

class Parser {
 private:
  std::shared_ptr<Catalog> catalog;

  absl::StatusOr<LogicalPlan> ParseSelectStatement(
      const hsql::SelectStatement* stmt);

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
  Parser(std::shared_ptr<Catalog> catalog) : catalog(std::move(catalog)) {}

  absl::StatusOr<LogicalPlan> ParseQuery(std::string_view query);
};

};  // namespace komfydb

#endif  // __PARSER_H__
