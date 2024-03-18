#! /bin/bash

set -euxo pipefail

BASE_PATH="$(dirname "$0")/../.."
cd "$BASE_PATH"

rm --force --recursive "$BASE_PATH/build"
rm "$BASE_PATH/src/frontend/lexical-analysis/flex-scanner.c"
rm "$BASE_PATH/src/frontend/syntactic-analysis/bison-parser.c"
rm "$BASE_PATH/src/frontend/syntactic-analysis/bison-parser.h"

cmake -S . -B build
cd build
make
cd ..

echo "All done."
