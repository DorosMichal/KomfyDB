#include "komfydb/storage/heap_file.h"

#include <filesystem>
#include <vector>

#include "gtest/gtest.h"

#include "komfydb/config.h"
#include "komfydb/storage/heap_page.h"

namespace {

using namespace komfydb::storage;
using komfydb::common::Type;

class HeapFileTest : public ::testing::Test {
 protected:
  const char* kTestDataFilePath = "komfydb/storage/heap_file_test.dat";
  const int tuples = 1000;
  int tuple_sz;
  int pages_cnt;
  int tuples_per_page;
  int table_id;
  const std::vector<Type> types = {Type::INT, Type::STRING, Type::INT,
                                   Type::STRING};
  std::unique_ptr<TupleDesc> td;
  std::unique_ptr<HeapFile> hp;

  void SetUp() override {
    td = std::make_unique<TupleDesc>(types);
    tuple_sz = td->GetSize();
    tuples_per_page = (CONFIG_PAGE_SIZE * 8) / (tuple_sz * 8 + td->Length());
    pages_cnt = tuples / tuples_per_page;

    absl::StatusOr<std::unique_ptr<HeapFile>> status_or_hp =
        HeapFile::Create(kTestDataFilePath, *td, Permissions::READ_ONLY);

    if (!status_or_hp.ok()) {
      FAIL() << "HeapFile::Create failed: " << status_or_hp.status();
    }

    hp = std::move(status_or_hp.value());
    table_id = hp->GetId();
  }
};

TEST_F(HeapFileTest, ReadPageErrors) {
  absl::StatusOr<std::unique_ptr<Page>> page;

  page = hp->ReadPage(PageId(table_id, pages_cnt + 1));
  ASSERT_FALSE(page.ok());
  EXPECT_EQ(page.status().message(), "Page number out of range.");

  page = hp->ReadPage(PageId(table_id + 1, 0));
  ASSERT_FALSE(page.ok());
  EXPECT_EQ(page.status().message(), "Table ID does not match.");
}

TEST_F(HeapFileTest, ReadPage) {
  absl::StatusOr<std::unique_ptr<Page>> page;
  for (int i = 0; i < pages_cnt; i++) {
    ASSERT_TRUE(hp->ReadPage(PageId(table_id, i)).ok());
  }
}

};  // namespace
