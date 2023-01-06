cc_library(
  name = "nodes",
  srcs = [
    "join_node.cc",
  ],
  hdrs = [
    "logical_plan.h",
    "filter_node.h",
    "join_node.h",
    "scan_node.h",
    "select_node.h",
  ],
  deps = [
    "//komfydb/common",
    "//komfydb/storage",
    "//komfydb/execution",
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
    "//:hsql",
    "@com_google_absl//absl/container:flat_hash_map",
    "@com_google_absl//absl/status:statusor",
  ],
  visibility = ["//visibility:public"],
)
