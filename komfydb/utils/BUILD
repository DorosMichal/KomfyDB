cc_library(
  name = "utility",
  srcs = ["utility.cc"],
  hdrs = [
    "utility.h",
    "status_macros.h",
    "status_builder.h",
  ],
  deps = [
    "@com_google_absl//absl/status:statusor",
    "@com_google_absl//absl/status",
    "@com_google_absl//absl/strings",
    "@com_google_absl//absl/random",
  ],
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
