#!/bin/bash

assert() {
    expected="$1"
    input="$2"

    gcc -o cc cc.c
    ./cc "$input" > tmp.s
    gcc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$actual => $expected"
    else
        echo "unexpected value: expected $expected, but got $actual"
    fi
}

assert 0 0
assert 42 42
assert 21 '5+20-4'
assert 32 ' 13   -5 + 26     -    2'
echo OK