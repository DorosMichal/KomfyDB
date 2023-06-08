cc_library(
  name = "storage",
  srcs = [
    "catalog.cc",
    "log_file.cc",
    "record_id.cc",
    "page_id.cc",
    "buffer_pool.cc",
    "heap_file.cc",
    "heap_page.cc",
    "record.cc",
    "table_iterator.cc"
  ],
  hdrs = [
    "catalog.h",
    "page_id.h",
    "table_iterator.h",
    "log_file.h",
    "db_file.h",
    "record_id.h",
    "record.h",
    "page.h",
    "buffer_pool.h",
    "heap_file.h",
    "heap_page.h",
  ],
  deps = [
    "//komfydb/transaction:transaction",
    "//komfydb/utils:utility",
    "//komfydb/common:common",
    "//komfydb:config",
    "@com_github_google_glog//:glog",
    "@com_google_absl//absl/status",
    "@com_google_absl//absl/container:flat_hash_map",
  ],
  visibility = ["//visibility:public"],
)

cc_test(
  name = "heap_file_test",
  srcs = ["heap_file_test.cc"],
  data = ["testdata/heap_file_test.dat"],
  deps = [
    ":storage",
    "@com_google_googletest//:gtest_main",
  ],
)

cc_test(
  name = "heap_page_test",
  srcs = ["heap_page_test.cc"],
  data = ["testdata/heap_page_test.dat"],
  deps = [
    ":storage",
    "@com_google_googletest//:gtest_main",
  ],
)

cc_test(
  name = "buffer_pool_test",
  srcs = ["buffer_pool_test.cc",],
  data= [
    "testdata/buffer_pool_test.dat",
    "testdata/buffer_pool_test_db_schema.txt",
  ],
  deps = [
    ":storage",
    "//komfydb:database",
    "//komfydb/optimizer:table_stats",
    "@com_google_googletest//:gtest_main",
  ],
)
