#include "gtest/gtest.h"

#include "absl/hash/hash_testing.h"

#include "komfydb/common/int_field.h"
#include "komfydb/common/string_field.h"
#include "komfydb/common/tuple.h"
#include "komfydb/common/tuple_desc.h"
#include "komfydb/storage/page_id.h"

namespace {

using komfydb::common::Field;
using komfydb::common::IntField;
using komfydb::common::StringField;
using komfydb::common::Tuple;
using komfydb::common::TupleDesc;
using komfydb::common::Type;
using komfydb::storage::PageId;

TEST(PageId, Hash) {
  EXPECT_TRUE(absl::VerifyTypeImplementsAbslHashCorrectly({
      PageId(1, 2),
      PageId(2, 3),
      PageId(0, 0),
      PageId(2, 4),
  }));
}

TEST(IntField, SupportsAbslHash) {
  IntField int1(1), int2(0), int3(-42);

  EXPECT_TRUE(absl::VerifyTypeImplementsAbslHashCorrectly({
      int1,
      int2,
      int3,
  }));
}

TEST(StringField, SupportsAbslHash) {
  StringField str1(""), str2("kappa kappa"), str3("---=");

  EXPECT_TRUE(absl::VerifyTypeImplementsAbslHashCorrectly({
      str1,
      str2,
      str3,
  }));
}

TEST(Tuple, SupportsAbslHash) {
  std::vector<Type> types = {Type::INT, Type::STRING, Type::INT},
                    types2 = {Type::STRING, Type::INT}, types3 = {Type::INT};
  TupleDesc tuple_desc(types), tuple_desc2(types2), tuple_desc3(types3);
  std::unique_ptr<IntField> int1 = std::make_unique<IntField>(1);
  std::unique_ptr<IntField> int2 = std::make_unique<IntField>(2);
  std::unique_ptr<IntField> int3 = std::make_unique<IntField>(3);
  std::unique_ptr<IntField> int4 = std::make_unique<IntField>(-42);
  std::unique_ptr<IntField> int5 = std::make_unique<IntField>(10000);
  std::unique_ptr<IntField> int6 = std::make_unique<IntField>(6);
  std::unique_ptr<IntField> int7 = std::make_unique<IntField>(0);
  std::unique_ptr<StringField> str1 = std::make_unique<StringField>("a");
  std::unique_ptr<StringField> str2 =
      std::make_unique<StringField>("kappa kappa");
  std::unique_ptr<StringField> str3 = std::make_unique<StringField>("---");
  std::unique_ptr<StringField> str4 = std::make_unique<StringField>("");
  Tuple t1(types.size()), t2(types.size()), t3(types2.size()),
      t4(types2.size()), t5(types3.size());
  ASSERT_TRUE(t1.SetField(0, std::move(int1)).ok());
  ASSERT_TRUE(t1.SetField(1, std::move(str1)).ok());
  ASSERT_TRUE(t1.SetField(2, std::move(int2)).ok());
  ASSERT_TRUE(t2.SetField(0, std::move(int3)).ok());
  ASSERT_TRUE(t2.SetField(1, std::move(str2)).ok());
  ASSERT_TRUE(t2.SetField(2, std::move(int4)).ok());
  ASSERT_TRUE(t3.SetField(0, std::move(str3)).ok());
  ASSERT_TRUE(t3.SetField(1, std::move(int5)).ok());
  ASSERT_TRUE(t4.SetField(0, std::move(str4)).ok());
  ASSERT_TRUE(t4.SetField(1, std::move(int6)).ok());
  ASSERT_TRUE(t5.SetField(0, std::move(int7)).ok());

  EXPECT_TRUE(absl::VerifyTypeImplementsAbslHashCorrectly({
      t1,
      t2,
      t3,
      t4,
      t5,
  }));
}

};  // namespace
