cc_library(
  name = "logical_plan",
  srcs = [
    "logical_plan.cc",
  ],
  hdrs = [
    "filter_node.h",
    "join_node.h",
    "logical_plan.h",
    "scan_node.h",
    "select_node.h",
  ],
  deps = [
    "//komfydb/storage",
    "//komfydb/common",
    "//komfydb/execution",
    "//:hsql",
    "@com_google_absl//absl/container:flat_hash_map",
    "@com_google_absl//absl/status:statusor",
  ],
  visibility = ["//visibility:public"],
)
