load("@rules_python//python:defs.bzl", "py_binary", "py_library")
load("@rules_python//python:pip.bzl", "compile_pip_requirements")

compile_pip_requirements(
    name = "requirements",
    extra_args = ["--allow-unsafe"],
    requirements_in = "requirements.in",
    requirements_txt = "requirements_lock.txt",
)

py_library(
  name = "e2e_tests_lib",
  srcs = [
    "__init__.py",
    "create_database.py",
    "database.py",
    "database_creator.py",
    "fields_generators.py",
    "komfy_database.py",
    "sqlite_database.py",
    "test_runner.py",
    "utils.py",
  ],
  deps = [
    # If we ever need a dependency, then add it to requirements.in, run
    #   bazel run //e2e_tests:requirements.update
    # and add it here in the following format:
    # "@pip//<dep name>",
  ],
)

py_test(
  name = "e2e_tests",
  srcs = ["run_tests.py"],
  main = "run_tests.py",
  args = ["--verbose"],
  deps = [
    ":e2e_tests_lib",
  ],
  data = [
    "database.sql",
    "//komfydb:e2e_tests",
  ]
)


