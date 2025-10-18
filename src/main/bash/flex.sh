#! /bin/bash

set -euxo pipefail

BASE_PATH="$(dirname "$0")/../../.."
cd "$BASE_PATH"

GREEN='\033[0;32m'
OFF='\033[0m'

flex \
	--bison-bridge \
	--bison-locations \
	--header-file="src/main/c/frontend/lexical-analysis/FlexScanner.h" \
	--never-interactive \
	--nounistd \
	--noyywrap \
	--outfile="src/main/c/frontend/lexical-analysis/FlexScanner.c" \
	--reentrant \
	--stack \
	--yylineno \
	"src/main/c/frontend/lexical-analysis/FlexPatterns.l"

echo -e "${GREEN}Flex done.${OFF}"
