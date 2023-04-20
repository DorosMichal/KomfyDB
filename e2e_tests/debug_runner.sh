#!/usr/bin/bash

trap '' USR1
trap '' USR2

export CC=clang-13
export BAZEL_CXXOPTS="-std=c++17"

bazel run //komfydb:e2e_tests