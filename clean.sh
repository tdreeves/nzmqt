#! /bin/bash

set -e
set -x

cd $(dirname $0)

rm -rf build rbuild pbuild
rm -f CMakeLists.txt.user
