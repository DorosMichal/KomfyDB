#include <filesystem>

#include "gtest/gtest.h"

#include "absl/status/statusor.h"

#include "komfydb/database.h"

namespace {

using komfydb::Database;

TEST(Database, LoadSchema) {
  std::string test_dir = testing::TempDir();
  std::filesystem::copy("komfydb/testdata", test_dir);

  std::string schema_file = test_dir + "/database_catalog_test.txt";
  std::unique_ptr<Database> db = Database::Create(test_dir);
  ASSERT_TRUE(db->LoadSchema(schema_file).ok());
}

};  // namespace
