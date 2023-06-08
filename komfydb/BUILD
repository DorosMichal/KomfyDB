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


filegroup(
  name = "database_test_data",
  srcs = glob(["testdata/*"]),
)

cc_test(
  name = "database_test",
  srcs = ["database_test.cc"],
  data = [":database_test_data"],
  deps = [
    "//komfydb:database",
    "//komfydb/storage:storage",
    "//komfydb/common:common",
    "//komfydb/transaction:transaction",
    "//komfydb/execution:execution",
    "//komfydb/optimizer:table_stats",
    "@com_google_googletest//:gtest_main",
  ],
)

cc_library(
  name = "parser",
  srcs = ["parser.cc",],
  hdrs = ["parser.h",],
  deps = [
    "//komfydb/execution/logical_plan:logical_plan",
    "//komfydb/execution:query",
    "//:hsql",
    "@com_google_absl//absl/status:statusor",
    "@com_github_google_glog//:glog",
  ],
)

cc_test(
  name = "parser_test",
  srcs = ["parser_test.cc",],
  data = [":database_test_data"],
  deps = [
    ":parser",
    ":database",
    "//komfydb/optimizer:table_stats",
    "@com_google_googletest//:gtest_main",
  ],
)

cc_library(
  name = "database",
  srcs = [
    "database.cc",
  ],
  hdrs = [
    "database.h",
  ],
  deps = [
    ":parser",
    "//komfydb/execution/logical_plan",
    "//komfydb/execution:executor",
    "//komfydb/storage",
    "//komfydb/common",
    "//komfydb/transaction",
    "//komfydb/execution",
    "//komfydb/execution:query",
    "@com_github_google_glog//:glog",
    "//:hsql",
  ],
  linkopts = ["-lreadline",],
  visibility = ["//visibility:public"],
)

cc_binary(
  name = "komfydb",
  srcs = ["main.cc",],
  data = [":database_test_data"],
  deps = [
    ":database",
    "//komfydb/optimizer:table_stats",
    "//komfydb/optimizer:histogram",
    "@com_github_google_glog//:glog",
  ],
)

cc_binary(
  name = "e2e_tests",
  srcs = ["e2e_tests.cc",],
  data = [":database_test_data"],
  deps = [
    ":database",
    "//komfydb/optimizer:table_stats",
    "//komfydb/optimizer:histogram",
    "@com_github_google_glog//:glog",
  ],
  visibility = ["//visibility:public"],
)

cc_binary(
  name = "sql_parser_test",
  srcs = ["sql_parser_test.cc",],
  deps = [
    "//:hsql",
  ],
  linkopts = ["-lreadline",],
)
