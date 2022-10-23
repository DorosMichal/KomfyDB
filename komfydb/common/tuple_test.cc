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
  Tuple tuple(td);

  std::shared_ptr<IntField> f(new IntField(1));
  std::cout << tuple.SetField(0, f).ok() << "\n";
  std::cout << tuple.SetField(0, f).message() << "\n";
  EXPECT_TRUE(tuple.SetField(0, f).ok());
};

};  // namespace komfydb::common
