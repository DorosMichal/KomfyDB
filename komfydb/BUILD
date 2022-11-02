cc_test(
  name = "custom_hash_test",
  srcs = ["custom_hash_test.cc"],
  deps = [
    "//komfydb/storage:storage",
    "@com_google_googletest//:gtest_main",
    "@com_google_absl//absl/hash:hash_testing"
  ],
)

cc_library(
  name = "config",
  hdrs = ["config.h"],
  visibility = ["//visibility:public"],
)

cc_binary(
  name = "komfydb",
  srcs = ["komfydb.cc"],
  deps = [
    "//komfydb/storage:storage",
    "//komfydb/common:common",
    "//komfydb/transaction:transaction",
    "//komfydb/execution:execution",
    "@com_github_google_glog//:glog",
  ],
)
