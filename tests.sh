#!/bin/bash

gcc -std=c11 -Wall -Wextra hw2.c -o hw2
PROGRAM=./hw2

for i in {1..10}; do
    echo "=== Test $i ==="
    $PROGRAM "${i}_in.txt" > "${i}_myout.txt"

    if diff -q "${i}_myout.txt" "${i}_out.txt" > /dev/null; then
        echo "PASS"
    else
        echo "FAIL"
    fi
done
