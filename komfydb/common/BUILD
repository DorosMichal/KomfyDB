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
  name = "common",
  hdrs = [
    "catalog.h",
    "database.h",
    "debug.h",
    "permissions.h",
    "type.h",
    "tuple.h",
    "tuple_desc.h",
    "td_item.h",
  ],
  srcs = [
    "catalog.cc",
    "database.cc",
    "debug.cc",
    "permissions.cc",
    "td_item.cc",
    "tuple.cc",
    "tuple_desc.cc",
    "type.cc",
  ],
  deps = [
    ":utility",
    "@com_google_absl//absl/strings",
    "@com_google_absl//absl/status:statusor",
  ],
  visibility = ["//visibility:public"],
)

