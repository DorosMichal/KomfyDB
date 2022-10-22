cc_library(
  name = "transaction",
  hdrs = [
    "transaction_id.h",
  ],
  srcs = [
    "transaction_id.cc",
  ],

  visibility = ["//visibility:public"],
)

cc_test(
  name = "transaction_id_test",
  size = "small",
  srcs = [
    "transaction_id_test.cc",
  ],
  deps = [
    "transaction",
    "@com_google_googletest//:gtest_main",
  ],
)
