#!/bin/sh
BIN=${BIN:-./pipeline}
pass=0
fail=0

check() {
    name=$1; want_rc=$2; want_out=$3; shift 3
    out=$(timeout 5 "$@" 2>/dev/null </dev/null)
    rc=$?
    if [ "$rc" -eq "$want_rc" ] && [ "$out" = "$want_out" ]; then
        echo "PASS: $name"
        pass=$((pass + 1))
    else
        echo "FAIL: $name (rc=$rc out='$out')"
        fail=$((fail + 1))
    fi
}

printf 'apple\nbanana\ncherry\n' > /tmp/pl_test.txt
: > /tmp/pl_empty.txt

check "cat | grep match"        0 "banana" $BIN cat /tmp/pl_test.txt -- grep banana
check "cat | grep no match"     1 ""       $BIN cat /tmp/pl_test.txt -- grep zzz
check "cat | wc -l"             0 "3"      $BIN cat /tmp/pl_test.txt -- wc -l
check "empty input"             1 ""       $BIN cat /tmp/pl_empty.txt -- grep x
check "missing file"            1 ""       $BIN cat /tmp/no_such_file -- grep x
check "bad right command"       127 ""     $BIN cat /tmp/pl_test.txt -- nosuchcmd
check "no arguments"            2 ""       $BIN
check "no separator"            2 ""       $BIN cat /tmp/pl_test.txt
check "empty left command"      2 ""       $BIN -- grep x
check "empty right command"     2 ""       $BIN cat /tmp/pl_test.txt --

echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
