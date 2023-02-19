#include "komfydb/storage/buffer_pool.h"

#include <filesystem>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "komfydb/database.h"
#include "komfydb/storage/heap_page.h"
#include "komfydb/transaction/transaction_id.h"

namespace {

using komfydb::Database;
using komfydb::common::Permissions;
using komfydb::transaction::TransactionId;
using namespace komfydb::storage;
using ::testing::ContainerEq;

class BufferPoolTest : public ::testing::Test {
 protected:
  std::unique_ptr<Database> db;
  std::shared_ptr<Catalog> catalog;
  int table_id;

  void SetUp() override {
    std::string test_dir = testing::TempDir();
    const auto copy_options = std::filesystem::copy_options::update_existing |
                              std::filesystem::copy_options::recursive;
    std::filesystem::copy("komfydb/storage/testdata", test_dir, copy_options);
    std::string schema_file = test_dir + "/buffer_pool_test_db_schema.txt";
    db = Database::Create(test_dir);
    assert(db->LoadSchema(schema_file).ok());
    catalog = db->GetCatalog();
    table_id = *catalog->GetTableId("buffer_pool_test");
  }

  std::list<PageId> create_list(std::vector<int> values) {
    std::list<PageId> l;
    for (auto v : values) {
      l.push_back(PageId(table_id, v));
    }
    return l;
  }
};

class MockPage : public HeapPage {
 public:
  bool IsDirty() { return false; }
};

TEST_F(BufferPoolTest, Eviction) {
  const int pages_cnt = 5;
  const TransactionId tid;
  Permissions perms(Permissions::READ_ONLY);
  BufferPool buffer_pool(db->GetCatalog(), pages_cnt);

  EXPECT_TRUE(buffer_pool.GetLru().size() == 0);

  for (int i = 0; i < 5; i++) {
    ASSERT_TRUE(buffer_pool.GetPage(tid, PageId(table_id, i), perms).ok());
  }
  EXPECT_THAT(buffer_pool.GetLru(), ContainerEq(create_list({4, 3, 2, 1, 0})));

  ASSERT_TRUE(buffer_pool.GetPage(tid, PageId(table_id, 0), perms).ok());
  EXPECT_THAT(buffer_pool.GetLru(), ContainerEq(create_list({0, 4, 3, 2, 1})));

  ASSERT_TRUE(buffer_pool.GetPage(tid, PageId(table_id, 2), perms).ok());
  EXPECT_THAT(buffer_pool.GetLru(), ContainerEq(create_list({2, 0, 4, 3, 1})));

  ASSERT_TRUE(buffer_pool.GetPage(tid, PageId(table_id, 6), perms).ok());
  EXPECT_THAT(buffer_pool.GetLru(), ContainerEq(create_list({6, 2, 0, 4, 3})));

  for (int i = 10; i < 15; i++) {
    ASSERT_TRUE(buffer_pool.GetPage(tid, PageId(table_id, i), perms).ok());
  }
  EXPECT_THAT(buffer_pool.GetLru(),
              ContainerEq(create_list({14, 13, 12, 11, 10})));
}

};  // namespace
