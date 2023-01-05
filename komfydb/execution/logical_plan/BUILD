cc_library(
  name = "nodes",
  srcs = [
    "join_node.cc",
  ],
  hdrs = [
    "aggregate_node.h",
    "filter_node.h",
    "join_node.h",
    "logical_plan.h",
    "scan_node.h",
    "select_node.h",
  ],
  deps = [
    "//komfydb/common",
    "//komfydb/storage",
    "//komfydb/execution",
    "//komfydb/execution:aggregator",
    "//komfydb/optimizer:table_stats",
    "//:hsql",
    "@com_google_absl//absl/container:flat_hash_map",
    "@com_google_absl//absl/status:statusor",
  ],
  visibility = ["//visibility:public"],
)

cc_library(
  name = "logical_plan",
  srcs = [
    "logical_plan.cc",
  ],
  deps = [
    ":nodes",
    "//komfydb/optimizer:join_optimizer",
    "//komfydb/storage",
    "//komfydb/common",
    "//komfydb/execution",
    "//komfydb/optimizer:table_stats",
    "//:hsql",
    "@com_google_absl//absl/container:flat_hash_map",
    "@com_google_absl//absl/status:statusor",
    "@com_github_google_glog//:glog",
  ],
  visibility = ["//visibility:public"],
)
