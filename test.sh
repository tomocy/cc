#!/bin/bash

assert() {
    expected="$1"
    input="$2"

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

assert 0 '0;'
assert 42 '42;'
assert 21 '5+20-4;'
assert 3 '5 +  7- 8+ 1;'
assert 47 '5 + 6 * 7;'
assert 3 '8-10/2;'
assert 7 '(( 3*2-4 ) / 2)*7;'
assert 1 '7 / 7;'
assert 3 'a = 3;'
assert 6 'a = b = 5; a + 1;'
assert 6 'a = b = 5; b + 1;'
assert 7 'a = 2; b = (7 * 8 - 9); (a + b) / 7;'
assert 1 '(10 / 2) == 5;'
assert 0 '5 == 2;'
echo OK