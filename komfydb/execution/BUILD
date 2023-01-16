cc_library(
  name = "op",
  srcs = ["op.cc"],
  hdrs = ["op.h"],
  visibility = ["//visibility:public"],
  deps = [
    "@com_google_absl//absl/status:statusor",
    "@com_google_absl//absl/container:flat_hash_map",
  ],
)

cc_library(
  name = "op_iterator",
  srcs = ["op_iterator.cc",],
  hdrs = ["op_iterator.h",],
  deps = [
    "//komfydb/storage:storage",
    "//komfydb/utils:utility",
    "@com_google_absl//absl/status",
  ],
)

cc_library(
  name = "execution",
  hdrs = [
    "seq_scan.h",
    "order_by.h",
    "join_predicate.h",
    "join.h",
    "aggregate.h",
    "predicate.h",
    "filter.h",
  ],
  srcs = [
    "seq_scan.cc",
    "order_by.cc",
    "join_predicate.cc",
    "join.cc",
    "predicate.cc",
    "filter.cc",
  ],
  deps = [
    ":op",
    ":op_iterator",
    "//komfydb/storage:storage",
    "//komfydb/utils:utility",
    "@com_google_absl//absl/status",
  ],
  visibility = ["//visibility:public"],
)

cc_library(
  name = "mock_iterator",
  srcs = ["mock_iterator.cc",],
  hdrs = ["mock_iterator.h",],
  deps = [
    ":op_iterator",
    "//komfydb/storage:storage",
    "@com_google_absl//absl/status",
  ],
  visibility = ["//visibility:public"],
)

cc_test(
  name = "order_by_test",
  srcs = ["order_by_test.cc"],
  deps = [
    ":mock_iterator",
    ":execution",
    "@com_google_googletest//:gtest_main",
  ],
)
