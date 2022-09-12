#! /bin/bash

ROOT="$(dirname "$0")"

echo "Relocating script to '$ROOT'..."
cd "$ROOT/.."

GREEN='\033[0;32m'
RED='\033[0;31m'
OFF='\033[0m'

echo "Compiler should accept..."
echo ""

for test in $(ls test/accept/); do
	cat "test/accept/$test" | ./bin/Compiler >/dev/null 2>&1
	RESULT="$?"
	if [ "$RESULT" == "0" ]; then
		echo -e "    $test, ${GREEN}and it does${OFF} (status $RESULT)"
	else
		echo -e "    $test, ${RED}but it rejects${OFF} (status $RESULT)"
	fi
done
echo ""

echo "Compiler should reject..."
echo ""

for test in $(ls test/reject/); do
	cat "test/reject/$test" | ./bin/Compiler >/dev/null 2>&1
	RESULT="$?"
	if [ "$RESULT" != "0" ]; then
		echo -e "    $test, ${GREEN}and it does${OFF} (status $RESULT)"
	else
		echo -e "    $test, ${RED}but it accepts${OFF} (status $RESULT)"
	fi
done
echo ""

echo "All done."
