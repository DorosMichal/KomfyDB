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
  absl::StatusOr<Database> db = Database::LoadSchema(schema_file);
  std::cout << db.status().message() << "\n";
  std::cout << testing::TempDir() << "\n";
  ASSERT_TRUE(db.ok());
}

};  // namespace
