#! /bin/bash

set -euxo pipefail

BASE_PATH="$(dirname "$0")/../.."
cd "$BASE_PATH"

# @see https://docs.docker.com/reference/cli/docker/image/build/
docker build \
	--progress=plain \
	--tag=flex-bison-compiler \
	$BASE_PATH

echo "All done."
