#include "gtest/gtest.h"

#include "hello_greeter.h"

TEST(HelloGreeter, GetGreeter) {
  std::string result = get_greeter("Franek");
  EXPECT_EQ(result, "Hello, Franek!");
}
