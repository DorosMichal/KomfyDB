cc_binary(
  name = "main",
  deps = [
    ":hello_greeter",
    "@com_google_absl//absl/strings",
  ],
  srcs = ["main.cc"],
)

cc_library(
  name = "hello_greeter",
  deps = ["@com_google_absl//absl/strings"],
  srcs = ["hello_greeter.cc"],
  hdrs = ["hello_greeter.h"],
)

cc_test(
  name = "hello_greeter_test",
  size = "small",
  srcs = ["hello_greeter_test.cc"],
  deps = [
    ":hello_greeter",
    "@com_google_googletest//:gtest_main"
  ],
)
