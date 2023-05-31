load("@rules_foreign_cc//foreign_cc:defs.bzl", "make")
load("@rules_foreign_cc//foreign_cc:defs.bzl", "cmake")

make(
  name = "hsql",
  lib_source = "@sql_parser//:all_srcs",
  args = ["static=yes INSTALL=$$INSTALLDIR$$"],
  out_static_libs = ["libsqlparser.a"],
  visibility = ["//visibility:public"],
)


cmake(
  name = "sockpp",
  lib_source = "@sockpp//:all_srcs",
  generate_args = ["-DSOCKPP_BUILD_STATIC=ON -DSOCKPP_BUILD_SHARED=OFF"],
  # build_args = ["--build build/"],
  out_static_libs = ["libsockpp.a"],
  visibility = ["//visibility:public"],
)
