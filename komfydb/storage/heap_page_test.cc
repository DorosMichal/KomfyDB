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
  std::vector<uint8_t> test_data, empty_data;
  std::fstream file;
  std::unique_ptr<TupleDesc> tuple_desc;
  int tuple_sz;
  int tuples_on_page;

  void SetUp() override {
    tuple_desc = std::make_unique<TupleDesc>(types);
    tuple_sz = tuple_desc->GetSize();
    tuples_on_page = (CONFIG_PAGE_SIZE * 8) / (tuple_sz * 8 + 1);

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

    empty_data.insert(empty_data.begin(), CONFIG_PAGE_SIZE, 0);

    pid = PageId(69, 420);
  }

  void TearDown() override { file.close(); }
};

TEST_F(HeapPageTest, Records) {
  absl::StatusOr<std::unique_ptr<HeapPage>> hpage =
      HeapPage::Create(pid, tuple_desc.get(), test_data);

  ASSERT_TRUE(hpage.ok());
  std::vector<Record> records = (*hpage)->GetRecords();

  // Test data has one record missing.
  EXPECT_EQ(records.size(), tuples_on_page - 1);

  for (int i = 0, rid = 0; i < records.size(); i++) {
    if (i == 1) {
      continue;
    }
    Record& record = records[rid++];
    EXPECT_EQ(record.GetTupleDesc(), tuple_desc.get());
    EXPECT_EQ(record.GetId(), RecordId(pid, i));

    Record comp_record(record.GetTupleDesc(), pid, i);
    ASSERT_TRUE(
        comp_record.SetField(0, std::make_unique<IntField>(i + 1)).ok());
    ASSERT_TRUE(
        comp_record
            .SetField(1, std::make_unique<StringField>(std::string(i + 1, 'a')))
            .ok());
    ASSERT_TRUE(
        comp_record.SetField(2, std::make_unique<IntField>(i + 1)).ok());
    ASSERT_TRUE(
        comp_record
            .SetField(3, std::make_unique<StringField>(std::string(i + 1, 'b')))
            .ok());
    EXPECT_EQ(record, comp_record);
  }
}

TEST_F(HeapPageTest, GetPageData) {
  absl::StatusOr<std::unique_ptr<HeapPage>> hpage =
      HeapPage::Create(pid, tuple_desc.get(), test_data);

  ASSERT_TRUE(hpage.ok());
  absl::StatusOr<std::vector<uint8_t>> page_data = (*hpage)->GetPageData();
  ASSERT_TRUE(page_data.ok());
  EXPECT_EQ(page_data.value(), test_data);
}

TEST_F(HeapPageTest, AddAndRemoveTuples) {
  absl::StatusOr<std::unique_ptr<HeapPage>> hpage =
      HeapPage::Create(pid, tuple_desc.get(), empty_data);
  ASSERT_TRUE(hpage.ok());

  Tuple t[3] = {Tuple(tuple_desc.get()), Tuple(tuple_desc.get()),
                Tuple(tuple_desc.get())};
  ASSERT_TRUE(t[0].SetField(0, std::make_unique<IntField>(0)).ok());
  ASSERT_TRUE(t[0].SetField(1, std::make_unique<StringField>("a")).ok());
  ASSERT_TRUE(t[0].SetField(2, std::make_unique<IntField>(1)).ok());
  ASSERT_TRUE(t[0].SetField(3, std::make_unique<StringField>("b")).ok());
  t[1] = t[0];
  t[2] = t[0];
  ASSERT_TRUE(t[1].SetField(0, std::make_unique<IntField>(2)).ok());
  ASSERT_TRUE(t[2].SetField(1, std::make_unique<StringField>("c")).ok());

  for (int i = 0; i < 3; i++) {
    ASSERT_TRUE((*hpage)->AddTuple(t[i]).ok());
  }

  bool have_tuple[3] = {false, false, false};
  RecordId ids[3] = {RecordId(pid, 0), RecordId(pid, 0), RecordId(pid, 0)};
  for (Record rec : (*hpage)->GetRecords()) {
    for (int i = 0; i < 3; i++) {
      if (t[i] == rec) {
        have_tuple[i] = true;
        ids[i] = rec.GetId();
      }
    }
  }
  for (int i = 0; i < 3; i++) {
    ASSERT_TRUE(have_tuple[i]);
    have_tuple[i] = false;
  }

  ASSERT_TRUE((*hpage)->RemoveRecord(ids[0]).ok());
  ASSERT_TRUE((*hpage)->RemoveRecord(ids[2]).ok());
  for (Record rec : (*hpage)->GetRecords()) {
    for (int i = 0; i < 3; i++) {
      if (t[i] == rec) {
        have_tuple[i] = true;
      }
    }
  }
  ASSERT_FALSE(have_tuple[0]);
  ASSERT_TRUE(have_tuple[1]);
  ASSERT_FALSE(have_tuple[2]);

  for (int i = 0; i < tuples_on_page - 1; i++) {
    ASSERT_TRUE((*hpage)->AddTuple(t[0]).ok());
  }
  /* No more space */
  ASSERT_FALSE((*hpage)->AddTuple(t[0]).ok());
}

};  // namespace
