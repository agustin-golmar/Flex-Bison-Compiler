#! /bin/bash

ROOT="$(dirname "$0")"

echo "Relocating script to '$ROOT'..."
cd "$ROOT"

echo "Building the compiler..."
cmake -S .. -B ../bin
cd ../bin
make

echo "All done."
