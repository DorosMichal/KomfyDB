#include "heap_page.h"

#include <fstream>

#include "gtest/gtest.h"

#include "komfydb/common/field.h"
#include "komfydb/common/int_field.h"
#include "komfydb/common/string_field.h"
#include "komfydb/common/type.h"
#include "komfydb/config.h"
#include "komfydb/storage/page_id.h"

namespace {

using namespace komfydb::storage;
using komfydb::common::Field;
using komfydb::common::IntField;
using komfydb::common::StringField;
using komfydb::common::Type;

class HeapPageTest : public ::testing::Test {
 protected:
  const char* kTestDataFilePath = "komfydb/storage/testdata/heap_page_test.dat";
  const std::vector<Type> types = {Type::INT, Type::STRING, Type::INT,
                                   Type::STRING};
  PageId pid;
  std::vector<uint8_t> test_data;
  std::fstream file;
  std::unique_ptr<TupleDesc> td;
  int tuple_sz;
  int tuples_on_page;

  void SetUp() override {
    td = std::make_unique<TupleDesc>(types);
    tuple_sz = td->GetSize();
    tuples_on_page = (CONFIG_PAGE_SIZE * 8) / (tuple_sz * 8 + td->Length());

    file.open(std::string(kTestDataFilePath), std::ios::in | std::ios::binary);
    if (!file.good()) {
      FAIL() << "Cannot open test file " << kTestDataFilePath;
    }

    file.seekg(0, file.end);
    if (file.tellg() != CONFIG_PAGE_SIZE) {
      FAIL() << "Test file's size is " << file.tellg() << "B but should be "
             << CONFIG_PAGE_SIZE << "B.";
    }
    file.seekg(0, file.beg);
    test_data.resize(CONFIG_PAGE_SIZE);
    file.read((char*)test_data.data(), CONFIG_PAGE_SIZE);

    pid = PageId(69, 420);
  }

  void TearDown() override { file.close(); }
};

TEST_F(HeapPageTest, Records) {
  absl::StatusOr<std::unique_ptr<HeapPage>> hpage =
      HeapPage::Create(pid, td.get(), test_data);

  ASSERT_TRUE(hpage.ok());
  // std::vector<Record> records = (*hpage)->GetRecords();
  // EXPECT_EQ(records.size(), tuples_on_page);

  // for (int i = 0; i < records.size(); i++) {
  // Record& record = records[i];
  // EXPECT_EQ(record.GetTupleDesc(), td.get());
  // Record comp_record(record.GetTupleDesc(), pid, i);
  // ASSERT_TRUE(comp_record.SetField(0, std::make_unique<IntField>(i)).ok());
  // ASSERT_TRUE(comp_record.SetField(1, std::make_unique<StringField>(std::string(i+1, 'a'))).ok());
  // ASSERT_TRUE(comp_record.SetField(2, std::make_unique<IntField>(i)).ok());
  // ASSERT_TRUE(comp_record.SetField(3, std::make_unique<StringField>(std::string(i+1,'b'))).ok());
  // EXPECT_EQ(record, comp_record);
  // }
}

TEST_F(HeapPageTest, GetPageData) {
  absl::StatusOr<std::unique_ptr<HeapPage>> hpage =
      HeapPage::Create(pid, td.get(), test_data);

  ASSERT_TRUE(hpage.ok());
  absl::StatusOr<std::vector<uint8_t>> page_data = (*hpage)->GetPageData();
  ASSERT_TRUE(page_data.ok());
  EXPECT_EQ(page_data.value(), test_data);
}

};  // namespace
