#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

#include "komfydb/common/status_macros.h"
#include "komfydb/common/utility.h"

namespace {

using namespace komfydb::common;
using ::testing::ElementsAre;

TEST(Utility, JoinVectors) {
  std::vector<int> v1{1, 2, 3}, v2{4, 5, 6, 7}, v3;
  JoinVectors(v1, v2, v3);
  EXPECT_THAT(v3, ElementsAre(1, 2, 3, 4, 5, 6, 7));
}

absl::StatusOr<int> MaybeFunction() {
  return 42;
}

absl::StatusOr<int> SampleFunction() {
  ASSIGN_OR_RETURN(int x, MaybeFunction());
  return x;
}

TEST(StatusMacros, Check) {
  auto x = SampleFunction();
  EXPECT_TRUE(x.ok());
  EXPECT_EQ(*x, 42);
}

};  // namespace
