#!/bin/bash

find runtimes/ -regex '.*\(h\|c\)' -exec clang-format -i {} \;
find examples/ -regex '.*\(h\|c\)' -exec clang-format -i {} \;
find cli/ -regex '.*\(h\|c\)' -exec clang-format -i {} \;
