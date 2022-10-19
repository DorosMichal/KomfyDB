cc_library(
  name = "storage",
  srcs = [
    "tuple_iterator.cc",
    "page_id.cc",
    "int_field.cc",
    "db_file_iterator.cc",
    "tuple_desc.cc",
    "string_field.cc",
    "log_file.cc",
    "db_file.cc",
    "record_id.cc",
    "page.cc",
    "heap_page_id.cc",
    "field.cc",
    "buffer_pool.cc",
    "tuple.cc",
    "heap_file.cc",
    "heap_page.cc",
  ],
  hdrs = [
    "tuple_iterator.h",
    "page_id.h",
    "int_field.h",
    "db_file_iterator.h",
    "tuple_desc.h",
    "string_field.h",
    "log_file.h",
    "db_file.h",
    "record_id.h",
    "page.h",
    "heap_page_id.h",
    "field.h",
    "buffer_pool.h",
    "tuple.h",
    "heap_file.h",
    "heap_page.h",
  ],
  deps = [
    "//komfydb/common:common",
    "@com_google_absl//absl/status",
  ],
  visibility = ["//visibility:public"],)