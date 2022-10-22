cc_library(
  name = "op",
  srcs = ["op.cc"],
  hdrs = ["op.h"],
  visibility = ["//visibility:public"],
)

cc_library(
  name = "execution",
  hdrs = [
    "op_iterator.h",
    "seq_scan.h",
  ],
  srcs = [
    "seq_scan.cc",
  ],
  deps = [
    ":op",
    "@com_google_absl//absl/status",
  ],
  visibility = ["//visibility:public"],
)
