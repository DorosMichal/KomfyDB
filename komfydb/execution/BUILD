cc_library(
  name = "op",
  srcs = ["op.cc"],
  hdrs = ["op.h"],
  deps = [
    "@com_google_absl//absl/status:statusor",
    "@com_google_absl//absl/container:flat_hash_map",
  ],
  visibility = ["//visibility:public"],
)

cc_library(
  name = "aggregator",
  srcs = [],
  hdrs = ["aggregator.h"],
  deps = [
    "@com_google_absl//absl/status:statusor",
    "@com_google_absl//absl/container:flat_hash_map",
  ],
  visibility = ["//visibility:public"],
)

cc_library(
  name = "execution",
  hdrs = [
    "aggregate.h",
    "aggregator.h",
    "aggregate_tuple.h",
    "delete.h",
    "filter.h",
    "fixed_iterator.h",
    "insert.h",
    "join.h",
    "join_predicate.h",
    "limit.h",
    "op_iterator.h",
    "order_by.h",
    "predicate.h",
    "project.h",
    "seq_scan.h",
    "hash_join.h",
  ],
  srcs = [
    "aggregate.cc",
    "aggregate_tuple.cc",
    "delete.cc",
    "filter.cc",
    "fixed_iterator.cc",
    "insert.cc",
    "join.cc",
    "join_predicate.cc",
    "limit.cc",
    "op_iterator.cc",
    "order_by.cc",
    "predicate.cc",
    "project.cc",
    "seq_scan.cc",
    "hash_join.cc",
  ],
  deps = [
    ":op",
    "//komfydb/storage:storage",
    "//komfydb/utils:utility",
    "//komfydb/common:common",
    "@com_google_absl//absl/status",
  ],
  visibility = ["//visibility:public"],
)

cc_library(
  name = "executor",
  srcs = ["executor.cc"],
  hdrs = ["executor.h"],
  deps = [
    ":execution", 
    "@com_google_absl//absl/status",
  ],
  visibility = ["//visibility:public"],
)

cc_library(
  name = "query",
  srcs = ["query.cc"],
  hdrs = ["query.h"],
  deps = [
    ":execution",
    "@com_google_absl//absl/status",
  ],
  visibility = ["//visibility:public"],
)
