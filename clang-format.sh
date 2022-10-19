#!/bin/bash

CF=clang-format-13
find komfydb -regex ".*\.\(cc\|h\)" | xargs $CF -i --verbose
