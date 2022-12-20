cc_library(
  name = "column_ref",
  hdrs = ["column_ref.h"],
  visibility = ["//visibility:public"],
)

cc_library(
  name = "fields",
  srcs = [
    "string_field.cc",
    "int_field.cc",
    "type.cc",
  ],
  hdrs = [
    "string_field.h",
    "int_field.h",
    "field.h",
    "type.h",
  ],
  deps = [
    "//komfydb/execution:op",
    "@com_google_absl//absl/status:statusor",
    "@com_google_absl//absl/status",
    "@com_google_absl//absl/strings",
  ],
)

cc_library(
  name = "tuple",
  srcs = [
    "tuple.cc",
    "tuple_desc.cc",
    "td_item.cc",
  ],
  hdrs = [
    "type.h",
    "tuple.h",
    "tuple_desc.h",
    "td_item.h",
  ],
  deps = [
    ":fields",
    "//komfydb/utils:utility",
    "@com_google_absl//absl/strings",
    "@com_google_absl//absl/status:statusor",
  ],
)

cc_test(
  name = "tuple_desc_test",
  srcs = ["tuple_desc_test.cc"],
  deps = [
    ":tuple",
    "@com_google_googletest//:gtest_main"
  ],
)

cc_test(
  name = "tuple_test",
  srcs = ["tuple_test.cc"],
  deps = [
    ":tuple",
    "@com_google_googletest//:gtest_main"
  ],
)

cc_library(
  name = "common",
  hdrs = [
    "debug.h",
    "permissions.h",
  ],
  srcs = [
    "debug.cc",
  ],
  deps = [
    ":column_ref",
    ":fields",
    ":tuple",
    "//komfydb/utils:utility",
    "@com_google_absl//absl/strings",
    "@com_google_absl//absl/status:statusor",
  ],
  visibility = ["//visibility:public"],
)

