load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_github_gflags_gflags",
    sha256 = "34af2f15cf7367513b352bdcd2493ab14ce43692d2dcd9dfc499492966c64dcf",
    strip_prefix = "gflags-2.2.2",
    urls = ["https://github.com/gflags/gflags/archive/v2.2.2.tar.gz"],
)

http_archive(
    name = "com_github_google_glog",
    sha256 = "122fb6b712808ef43fbf80f75c52a21c9760683dae470154f02bddfc61135022",
    strip_prefix = "glog-0.6.0",
    urls = ["https://github.com/google/glog/archive/v0.6.0.zip"],
)

http_archive(
  name = "com_google_absl",
  urls = ["https://github.com/abseil/abseil-cpp/archive/refs/tags/20220623.1.zip"],
  strip_prefix = "abseil-cpp-20220623.1",
  sha256 = "54707f411cb62a26a776dad5fd60829098c181700edcd022ea5c2ca49e9b7ef1"
)

http_archive(
  name = "bazel_skylib",
  urls = ["https://github.com/bazelbuild/bazel-skylib/releases/download/1.2.1/bazel-skylib-1.2.1.tar.gz"],
  sha256 = "f7be3474d42aae265405a592bb7da8e171919d74c16f082a5457840f06054728",
)

http_archive(
  name = "com_google_googletest",
  urls = ["https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip"],
  strip_prefix = "googletest-release-1.12.1",
  sha256 = "24564e3b712d3eb30ac9a85d92f7d720f60cc0173730ac166f27dda7fed76cb2"
)

http_archive(
    name = "rules_foreign_cc",
    sha256 = "076b8217296ca25d5b2167a832c8703cc51cbf8d980f00d6c71e9691876f6b08",
    strip_prefix = "rules_foreign_cc-2c6262f8f487cd3481db27e2c509d9e6d30bfe53",
    url = "https://github.com/bazelbuild/rules_foreign_cc/archive/2c6262f8f487cd3481db27e2c509d9e6d30bfe53.tar.gz",
)
load("@rules_foreign_cc//foreign_cc:repositories.bzl", "rules_foreign_cc_dependencies")
rules_foreign_cc_dependencies()

_ALL_CONTENT = """\
filegroup(
    name = "all_srcs",
    srcs = glob(["**"]),
    visibility = ["//visibility:public"],
)
"""

http_archive(
  name = "sql_parser",
  build_file_content = _ALL_CONTENT,
  urls = ["https://github.com/hyrise/sql-parser/archive/5a5fd248496e8d415fd2a4b1787e7114d90e60de.zip"],
  strip_prefix = "sql-parser-5a5fd248496e8d415fd2a4b1787e7114d90e60de",
  sha256 = "d20239bb1028a621109e22e570c7c59f2baf3461e0fde6c57bea7856e77d0011",
  patches = ["patches/sql_parser/makefile.patch"],
  patch_args = ["-p1"],
)

http_archive(
  name = "sockpp",
  build_file_content = _ALL_CONTENT,
  urls = ["https://github.com/fpagliughi/sockpp/archive/refs/tags/v0.8.1.zip"],
  sha256 = "233c093e5150346779f5562e0a1b409cb4cd53ad16628de51ada2d85b5bc76ab",
  strip_prefix = "sockpp-0.8.1",
)

http_archive(
    name = "rules_python",
    sha256 = "a644da969b6824cc87f8fe7b18101a8a6c57da5db39caa6566ec6109f37d2141",
    strip_prefix = "rules_python-0.20.0",
    url = "https://github.com/bazelbuild/rules_python/releases/download/0.20.0/rules_python-0.20.0.tar.gz",
)

load("@rules_python//python:repositories.bzl", "py_repositories")
py_repositories()

load("@rules_python//python:repositories.bzl", "python_register_toolchains")
python_register_toolchains(
    name = "python3_10",
    # Available versions are listed in @rules_python//python:versions.bzl.
    # We recommend using the same version your team is already standardized on.
    python_version = "3.10",
)

load("@python3_10//:defs.bzl", "interpreter")
load("@rules_python//python:pip.bzl", "pip_parse")

pip_parse(
  name = "pip",
  incompatible_generate_aliases = True,
  python_interpreter_target = interpreter,
  quiet = False,
  requirements_lock = "//e2e_tests:requirements_lock.txt",
)

load("@pip//:requirements.bzl", "install_deps")
install_deps()

