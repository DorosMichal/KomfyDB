cc_library(
  name = "utility",
  srcs = ["utility.cc"],
  hdrs = ["utility.h"],
  visibility = ["//visibility:public"],
)

cc_test(
  name = "utility_test",
  srcs = [ "utility_test.cc" ],
  deps = [
    ":utility",
    "@com_google_googletest//:gtest_main",
  ],
)

cc_library(
  name = "tuple",
  srcs = [
    "type.cc",
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
    ":utility",
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

cc_library(
  name = "common",
  hdrs = [
    "catalog.h",
    "database.h",
    "debug.h",
    "permissions.h",
  ],
  srcs = [
    "catalog.cc",
    "database.cc",
    "debug.cc",
    "permissions.cc",
  ],
  deps = [
    ":utility",
    ":tuple",
    "@com_google_absl//absl/strings",
    "@com_google_absl//absl/status:statusor",
  ],
  visibility = ["//visibility:public"],
)

