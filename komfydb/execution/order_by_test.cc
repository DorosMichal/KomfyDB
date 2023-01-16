#include "komfydb/execution/order_by.h"

#include <memory>

#include "gtest/gtest.h"

#include "komfydb/common/type.h"
#include "komfydb/execution/mock_iterator.h"
#include "komfydb/storage/page_id.h"

namespace {

using namespace komfydb::execution;
using namespace komfydb::common;
using namespace komfydb::common;
using komfydb::storage::PageId;

class OrderByTest : public ::testing::Test {
 protected:
  std::unique_ptr<OpIterator> mock_iterator;
  TupleDesc td;
  PageId pid;

  void SetUp() {
    std::vector<Type> types = {Type::INT, Type::STRING};
    td = TupleDesc(types);
    std::vector<Record> records = {
        Record(&td, pid, 0), Record(&td, pid, 1), Record(&td, pid, 2),
        Record(&td, pid, 3), Record(&td, pid, 4),
    };
    std::vector<int> int_values = {4, 2, 1, 5, 3};
    std::vector<std::string> str_values = {"e", "d", "c", "b", "a"};
    for (int i = 0; i < records.size(); i++) {
      // I don't know how to 'not ignore' the return value.
      records[i].SetField(0, std::make_unique<IntField>(int_values[i]));
      records[i].SetField(1, std::make_unique<StringField>(str_values[i]));
    }
    mock_iterator = std::move(*MockIterator::Create(std::move(records), td));
  }
};

TEST_F(OrderByTest, FirstField) {
  absl::StatusOr<std::unique_ptr<OrderBy>> orderby =
      OrderBy::Create(std::move(mock_iterator), 0, OrderBy::ASCENDING);
  ASSERT_EQ(orderby.status(), absl::OkStatus());
  ASSERT_TRUE((*orderby)->Open().ok());

  std::vector<int> expected_ints = {1, 2, 3, 4, 5};
  std::vector<std::string> expected_strs = {"c", "d", "a", "e", "b"};
  for (int iter = 0; iter < 2; iter++) {
    for (int i = 0; i < 5; i++) {
      ASSERT_TRUE((*orderby)->HasNext().ok());
      absl::StatusOr<std::unique_ptr<Record>> record = (*orderby)->Next();
      ASSERT_TRUE(record.ok());
      EXPECT_EQ(*static_cast<IntField*>(*(*record)->GetField(0)),
                IntField(expected_ints[i]));
      EXPECT_EQ(*static_cast<StringField*>(*(*record)->GetField(1)),
                StringField(expected_strs[i]));
    }
    EXPECT_TRUE(absl::IsOutOfRange((*orderby)->HasNext()));
    ASSERT_TRUE((*orderby)->Rewind().ok());
  }
}

};  // namespace
