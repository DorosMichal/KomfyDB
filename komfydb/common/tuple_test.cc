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
  std::vector<std::string> nv{"f1", "f2"};

  TupleDesc td(tv, nv);
  Tuple tuple(&td);

  IntField f(1);
  std::cout << tuple.SetField(0, &f).ok() << "\n";
  std::cout << tuple.SetField(0, &f).message() << "\n";
  EXPECT_TRUE(tuple.SetField(0, &f).ok());

  auto f1 = tuple.GetField(0);
  ASSERT_TRUE(f1.ok());
  int i;
  (*f1)->GetValue(i);
  EXPECT_EQ(i, 1);
};

};  // namespace
