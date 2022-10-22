#include <vector>

#include "absl/status/statusor.h"
#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

#include "komfydb/common/td_item.h"
#include "komfydb/common/tuple_desc.h"
#include "komfydb/common/type.h"

namespace komfydb::common {

using ::testing::ElementsAre;

TEST(TupleDesc, JoinTwoTDs) {
  Type int_t(Type::INT);
  Type str_t(Type::INT);
  std::vector<Type> tv1{int_t, str_t};
  std::vector<Type> tv2{str_t, int_t, str_t};
  std::vector<std::string> nv1{"f1", "f2"};
  std::vector<std::string> nv2{"f3", "f4", "f5"};

  TupleDesc td1(tv1, nv1);
  TupleDesc td2(tv2, nv2);

  ASSERT_EQ(td1.Length(), 2);
  ASSERT_EQ(td2.Length(), 3);

  TupleDesc td3(td1, td2);
  ASSERT_EQ(td3.Length(), 5);

  EXPECT_THAT(td1.GetItems(),
              ElementsAre(TDItem(int_t, "f1"), TDItem(str_t, "f2")));
  EXPECT_THAT(td2.GetItems(),
              ElementsAre(TDItem(str_t, "f3"), TDItem(int_t, "f4"),
                          TDItem(str_t, "f5")));
  EXPECT_THAT(
      td3.GetItems(),
      ElementsAre(TDItem(int_t, "f1"), TDItem(str_t, "f2"), TDItem(str_t, "f3"),
                  TDItem(int_t, "f4"), TDItem(str_t, "f5")));

  absl::StatusOr<Type> t = td3.GetFieldType(3);
  EXPECT_TRUE(t.ok());
  EXPECT_EQ(*t, int_t);
  EXPECT_FALSE(td3.GetFieldType(6).ok());
}
};  // namespace komfydb::common
