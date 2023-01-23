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
    "op_iterator.h",
    "seq_scan.h",
    "order_by.h",
    "join_predicate.h",
    "join.h",
    "aggregate.h",
    "predicate.h",
    "filter.h",
    "project.h",
    "aggregator.h",
    "aggregate_tuple.h",
    "insert.h",
    "fixed_iterator.h",
  ],
  srcs = [
    "op_iterator.cc",
    "seq_scan.cc",
    "order_by.cc",
    "join_predicate.cc",
    "join.cc",
    "predicate.cc",
    "filter.cc",
    "project.cc",
    "aggregate.cc",
    "aggregate_tuple.cc",
    "insert.cc",
    "fixed_iterator.cc",
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
