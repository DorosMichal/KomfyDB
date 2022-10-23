cc_library(
  name = "storage",
  srcs = [
    "log_file.cc",
    "record_id.cc",
    "heap_page_id.cc",
    "buffer_pool.cc",
    "heap_file.cc",
    "heap_page.cc",
  ],
  hdrs = [
    "page_id.h",
    "db_file_iterator.h",
    "log_file.h",
    "db_file.h",
    "record_id.h",
    "page.h",
    "heap_page_id.h",
    "buffer_pool.h",
    "heap_file.h",
    "heap_page.h",
    "heap_page_factory.h",
  ],
  deps = [
    "//komfydb/common:common",
    "@com_google_absl//absl/status",
  ],
  visibility = ["//visibility:public"],)
