#include "gtest/gtest.h"

#include "absl/status/statusor.h"

#include "komfydb/database.h"

namespace {

using komfydb::Database;

TEST(Database, LoadSchema) {
  absl::StatusOr<Database> db =
      Database::LoadSchema("komfydb/testdata/database_catalog_test.txt");
  ASSERT_TRUE(db.ok());
}

};  // namespace
