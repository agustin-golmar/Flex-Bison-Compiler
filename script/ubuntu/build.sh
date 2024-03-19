#! /bin/bash

set -euxo pipefail

BASE_PATH="$(dirname "$0")/../.."
cd "$BASE_PATH"

rm --force --recursive "$BASE_PATH/build"
rm --force "$BASE_PATH/src/main/c/frontend/lexical-analysis/FlexScanner.c"
rm --force "$BASE_PATH/src/main/c/frontend/syntactic-analysis/BisonParser.c"
rm --force "$BASE_PATH/src/main/c/frontend/syntactic-analysis/BisonParser.h"

cmake -S . -B build
cd build
make
cd ..

echo "All done."
