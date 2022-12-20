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
  name = "execution",
  hdrs = [
    "op_iterator.h",
    "seq_scan.h",
    "order_by.h",
    "aggregate.h",
  ],
  srcs = [
    "seq_scan.cc",
    "order_by.cc",
  ],
  deps = [
    ":op",
    "//komfydb/storage:storage",
    "//komfydb/utils:utility",
    "@com_google_absl//absl/status",
  ],
  visibility = ["//visibility:public"],
)
