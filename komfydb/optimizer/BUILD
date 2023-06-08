cc_library(
  name = "table_stats",
  srcs = ["table_stats.cc",],
  hdrs = ["table_stats.h",],
  deps = [
    "//komfydb/execution:op",
    "//komfydb/common:fields",
    "//komfydb/storage",
    "//komfydb/execution:execution",
    ":histogram",
    "@com_google_absl//absl/status:statusor",
    "@com_github_google_glog//:glog",
  ],
  visibility = ["//visibility:public"],
)

cc_library(
  name = "histogram",
  srcs = [
    "int_histogram.cc",
    "string_histogram.cc",
  ],
  hdrs = [
    "int_histogram.h",
    "string_histogram.h",
    "histogram.h",
  ],
  deps = [
    "//komfydb/execution:op",
    "//komfydb/common:fields",
    "//komfydb/storage",
    "@com_google_absl//absl/status:statusor",
    "@com_github_google_glog//:glog",
  ],
  visibility = ["//visibility:public"],
)

cc_library(
  name = "join_optimizer",
  srcs = ["join_optimizer.cc",],
  hdrs = ["join_optimizer.h",],
  deps = [
    "//komfydb/execution/logical_plan:nodes",
    "//komfydb/execution",
    "//komfydb/storage",
    "@com_google_absl//absl/status:statusor",
    "@com_github_google_glog//:glog",
  ],
  visibility = ["//visibility:public"],
)
