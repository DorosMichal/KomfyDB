#include "gtest/gtest.h"

#include "komfydb/common/int_field.h"
#include "komfydb/common/string_field.h"
#include "komfydb/common/tuple.h"
#include "komfydb/common/tuple_desc.h"

namespace {

using namespace komfydb::common;

TEST(Tuple, StringConversion) {
  Type int_t(Type::INT);
  Type str_t(Type::STRING);
  std::vector<Type> tv{int_t, str_t};

  Tuple tuple(tv.size());

  EXPECT_TRUE(tuple.SetField(0, std::make_unique<IntField>(1)).ok());

  auto f1 = tuple.GetField(0);
  ASSERT_TRUE(f1.ok());
  int i = (static_cast<IntField*>(*f1))->GetValue();
  EXPECT_EQ(i, 1);
};

TEST(Tuple, Comparison) {
  const std::vector<Type> types1 = {Type::INT, Type::STRING, Type::INT,
                                    Type::STRING};
  const std::vector<Type> types2 = {Type::INT, Type::STRING};

  Tuple t1(types1.size()), t2(types1.size()), t3(types1.size()),
      t4(types2.size());
  ASSERT_TRUE(t1.SetField(0, std::make_unique<IntField>(1)).ok());
  ASSERT_TRUE(t1.SetField(1, std::make_unique<StringField>("a")).ok());
  ASSERT_TRUE(t1.SetField(2, std::make_unique<IntField>(2)).ok());
  ASSERT_TRUE(t1.SetField(3, std::make_unique<StringField>("b")).ok());

  ASSERT_TRUE(t2.SetField(0, std::make_unique<IntField>(1)).ok());
  ASSERT_TRUE(t2.SetField(1, std::make_unique<StringField>("a")).ok());
  ASSERT_TRUE(t2.SetField(2, std::make_unique<IntField>(2)).ok());
  ASSERT_TRUE(t2.SetField(3, std::make_unique<StringField>("b")).ok());

  ASSERT_TRUE(t3.SetField(0, std::make_unique<IntField>(1)).ok());
  ASSERT_TRUE(t3.SetField(1, std::make_unique<StringField>("a")).ok());
  ASSERT_TRUE(t3.SetField(2, std::make_unique<IntField>(2)).ok());
  ASSERT_TRUE(t3.SetField(3, std::make_unique<StringField>("c")).ok());

  ASSERT_TRUE(t4.SetField(0, std::make_unique<IntField>(1)).ok());
  ASSERT_TRUE(t4.SetField(1, std::make_unique<StringField>("a")).ok());

  EXPECT_EQ(t1, t1);
  EXPECT_EQ(t1, t2);
  EXPECT_EQ(t2, t1);
  EXPECT_FALSE(t1 != t2);
  EXPECT_NE(t1, t3);
  EXPECT_NE(t1, t4);
}

TEST(Tuple, CopyAssignment) {
  const std::vector<Type> types1 = {Type::INT, Type::STRING, Type::INT,
                                    Type::STRING};
  Tuple t1(types1.size());
  ASSERT_TRUE(t1.SetField(0, std::make_unique<IntField>(1)).ok());
  ASSERT_TRUE(t1.SetField(1, std::make_unique<StringField>("a")).ok());
  ASSERT_TRUE(t1.SetField(2, std::make_unique<IntField>(2)).ok());
  ASSERT_TRUE(t1.SetField(3, std::make_unique<StringField>("b")).ok());

  Tuple t2(types1.size());
  t2 = t1;
  Tuple t3(t1);

  EXPECT_EQ(t1, t2);
  EXPECT_EQ(t1, t3);
  EXPECT_EQ(t2, t3);
}

};  // namespace
