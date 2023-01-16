#include "komfydb/execution/op_iterator.h"

#include "gtest/gtest.h"

#include "komfydb/common/int_field.h"
#include "komfydb/common/string_field.h"
#include "komfydb/common/type.h"
#include "komfydb/execution/mock_iterator.h"
#include "komfydb/storage/page_id.h"

namespace {

using namespace komfydb::execution;
using komfydb::common::IntField;
using komfydb::common::StringField;
using komfydb::common::Type;
using komfydb::storage::PageId;

using IntStrTupleDesc = std::vector<std::tuple<int, std::string>>;
using IntStrIntStrTupleDesc =
    std::vector<std::tuple<int, std::string, int, std::string>>;

enum OpIteratorTypes {
  FILTER,
  JOIN,
  ORDER_BY,
  PROJECT,
};

class OpIteratorTestParams {
 public:
  std::unique_ptr<MockIterator> child;
  // Used only for join.
  std::unique_ptr<MockIterator> r_child;
  std::vector<Record> expected;
  TupleDesc tuple_desc;
  TupleDesc join_tuple_desc;
  bool is_join_test;

  IteratorTestParams(IntStrTupleDesc input_fields,
                     IntStrTupleDesc expected_fields)
      : r_child(nullptr),
        tuple_desc({Type::INT, Type::STRING}),
        join_tuple_desc({Type::INT, Type::STRING, Type::INT, Type::STRING}),
        is_join_test(false) {
    child = MockIterator::Create(GenerateRecords(input_fields), &tuple_desc);
    expected = GenerateRecords(expected_fields);
  }

  IteratorTestParams(IntStrTupleDesc l_input_fields,
                     IntStrTupleDesc r_input_fields,
                     IntStrIntStrTupleDesc expected_fields)
      : tuple_desc({Type::INT, Type::STRING}),
        join_tuple_desc({Type::INT, Type::STRING, Type::INT, Type::STRING}),
        is_join_test(true) {
    child = MockIterator::Create(GenerateRecords(l_input_fields), &tuple_desc);
    r_child =
        MockIterator::Create(GenerateRecords(r_input_fields), &tuple_desc);
    expected = GenerateJoinRecords(expected_fields);
  }

 private:
  Record CreateRecord(int int_val, std::string str_val) {
    PageId pid(0, 0);
    Record record(&tuple_desc, pid, 0);
    assert(record.SetField(0, std::make_unique<IntField>(int_val)).ok());
    assert(record.SetField(1, std::make_unique<StringField>(str_val)).ok());
    return record;
  }

  Record CreateJoinRecord(int int1, std::string str1, int int2,
                          std::string str2) {
    PageId pid(0, 0);
    Record record(&join_tuple_desc, pid, 0);
    assert(record.SetField(0, std::make_unique<IntField>(int1)).ok());
    assert(record.SetField(1, std::make_unique<StringField>(str1)).ok());
    assert(record.SetField(2, std::make_unique<IntField>(int2)).ok());
    assert(record.SetField(3, std::make_unique<StringField>(str2)).ok());
    return record;
  }

  std::vector<Record> GenerateRecords(IntStrTupleDesc desc) {
    std::vector<Record> result;
    for (auto [int_val, str_val] : desc) {
      result.push_back(CreateRecord(int_val, str_val));
    }
    return result;
  }

  std::vector<Record> GenerateJoinRecords(IntStrIntStrTupleDesc desc) {
    std::vector<Record> result;
    for (auto [int1, str1, int2, str2] : desc) {
      result.push_back(CreateJoinRecord(int1, str1, int2, str2));
    }
    return result;
  }
};

static const std::vector<OpIteratorTestParams> params = {
    OpIteratorTestParams({{1, "a"}, {2, "b"}, {3, "c"}, {4, "d"}, {5, "e"}},
                         {}),
};

};  // namespace
