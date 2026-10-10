#!/usr/bin/env bash
# Compiles the program and runs it on every test in tests/,
# comparing the output with the expected one.
set -u
gcc -O2 -Wall -std=gnu11 -o planner src/main.c || exit 1
pass=0; total=0
for input in $(ls tests/open_*.txt | grep -v output | sort -V); do
  name=$(basename "$input" .txt)
  total=$((total + 1))
  if ./planner < "$input" 2>/dev/null | cmp -s - "tests/$name.output.txt"; then
    pass=$((pass + 1))
  else
    echo "FAIL $name"
  fi
done
echo "Passed $pass/$total tests"
