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

  tuple.SetField(0, std::make_unique<IntField>(1));

  Field* f1 = tuple.GetField(0);
  int i = (static_cast<IntField*>(f1))->GetValue();
  EXPECT_EQ(i, 1);
};

TEST(Tuple, Comparison) {
  const std::vector<Type> types1 = {Type::INT, Type::STRING, Type::INT,
                                    Type::STRING};
  const std::vector<Type> types2 = {Type::INT, Type::STRING};

  Tuple t1(types1.size()), t2(types1.size()), t3(types1.size()),
      t4(types2.size());
  t1.SetField(0, std::make_unique<IntField>(1));
  t1.SetField(1, std::make_unique<StringField>("a"));
  t1.SetField(2, std::make_unique<IntField>(2));
  t1.SetField(3, std::make_unique<StringField>("b"));

  t2.SetField(0, std::make_unique<IntField>(1));
  t2.SetField(1, std::make_unique<StringField>("a"));
  t2.SetField(2, std::make_unique<IntField>(2));
  t2.SetField(3, std::make_unique<StringField>("b"));

  t3.SetField(0, std::make_unique<IntField>(1));
  t3.SetField(1, std::make_unique<StringField>("a"));
  t3.SetField(2, std::make_unique<IntField>(2));
  t3.SetField(3, std::make_unique<StringField>("c"));

  t4.SetField(0, std::make_unique<IntField>(1));
  t4.SetField(1, std::make_unique<StringField>("a"));

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
  t1.SetField(0, std::make_unique<IntField>(1));
  t1.SetField(1, std::make_unique<StringField>("a"));
  t1.SetField(2, std::make_unique<IntField>(2));
  t1.SetField(3, std::make_unique<StringField>("b"));

  Tuple t2(types1.size());
  t2 = t1;
  Tuple t3(t1);

  EXPECT_EQ(t1, t2);
  EXPECT_EQ(t1, t3);
  EXPECT_EQ(t2, t3);
}

};  // namespace
