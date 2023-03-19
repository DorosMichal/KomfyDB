#include "komfydb/parser.h"

#include <filesystem>

#include "gtest/gtest.h"

#include "absl/status/statusor.h"

#include "komfydb/database.h"
#include "transaction/transaction_id.h"

namespace {

using komfydb::Database;
using komfydb::Parser;
using komfydb::execution::logical_plan::LogicalPlan;

class ParserTest
    : public ::testing::TestWithParam<std::tuple<std::string, std::string>> {
 protected:
  std::unique_ptr<Database> db;
  std::unique_ptr<Parser> parser;
  absl::StatusOr<LogicalPlan> lp;
  TableStatsMap table_stats_map;

  void SetUp() override {
    std::string test_dir = testing::TempDir();
    const auto copy_options = std::filesystem::copy_options::update_existing |
                              std::filesystem::copy_options::recursive;
    std::filesystem::copy("komfydb/testdata", test_dir, copy_options);
    std::string schema_file = test_dir + "/database_catalog_test.txt";
    db = Database::Create(test_dir);
    assert(db->LoadSchema(schema_file).ok());
    parser = std::make_unique<Parser>(db->GetCatalog(), db->GetBufferPool(),
                                      table_stats_map);
  }
};

// TODO: This isn't unfortunatelly a unit test, as we need to create a 'true'
// database object... It would be nice to create some mock objects for catalog
// and database for unit tests.
TEST_P(ParserTest, Queries) {
  auto [query, result] = GetParam();
  absl::Status status =
      parser->ParseQuery(query, TransactionId(), false).status();
  if (result == "OK") {
    EXPECT_TRUE(status.ok());
  } else {
    EXPECT_FALSE(status.ok());
    EXPECT_EQ(status.message(), result);
  }
}

const std::vector<std::tuple<std::string, std::string>> test_queries = {
    {"SELECT * FROM first_table", "OK"},
    {"SELECT * FROM first_table WHERE name1 > name3", "OK"},
    {"SELECT * FROM first_table WHERE name1 > name2",
     "Cannot compare columns with different type: first_table.name1:int "
     "first_table.name2:str"},
    {"SELECT no_name FROM first_table",
     "Column no_name does not appear in any table"},
    {"SELECT t1.name1, t2.name2, t3.name1 "
     "FROM first_table t1, second_table t2, first_table t3 "
     "WHERE t1.name1 == t3.name3 "
     "AND t2.name4 LIKE t3.name2",
     "OK"},
    {"SELECT * FROM first_table WHERE name1 > (SELECT * FROM first_table)",
     "OK"},
    {"SELECT * FROM first_table WHERE (SELECT * FROM first_table) < name1",
     "Only supported operators are e1 AND e2 and simple binary expressions "
     "like A op B, where A, B are constatns, table columns or B is a "
     "subquery."},
    {"SELECT * FROM first_table WHERE 1 > 2",
     "Filters with two literals are unsupported."},
    {"SELECT * FROM first_table WHERE name1 > 2", "OK"},
    {"SELECT * FROM first_table WHERE name2 > 2",
     "Cannot compare int literal with string column first_table.name2"},
    {"SELECT * FROM first_table, second_table WHERE name1 > 2",
     "Column name1 without table name is ambigous"},
};

INSTANTIATE_TEST_SUITE_P(Queries, ParserTest, testing::ValuesIn(test_queries));

};  // namespace
