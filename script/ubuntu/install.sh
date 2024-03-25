#! /bin/bash

set -euxo pipefail

BASE_PATH="$(dirname "$0")/../.."
cd "$BASE_PATH"

sudo apt-get install bison --yes
sudo apt-get install cmake --yes
sudo apt-get install flex --yes
sudo apt-get install gcc --yes
sudo apt-get install make --yes

echo "All done."
