load("@rules_foreign_cc//foreign_cc:defs.bzl", "make")

make(
  name = "hsql",
  lib_source = "@sql_parser//:all_srcs",
  args = ["static=yes INSTALL=$$INSTALLDIR$$"],
  out_static_libs = ["libsqlparser.a"],
  visibility = ["//visibility:public"],
)
