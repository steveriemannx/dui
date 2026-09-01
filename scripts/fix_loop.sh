#!/bin/bash
# Iteratively type-check the examples in the Windows string configuration and
# wrap the offending literals in DUI_T(), until the tree converges.
cd "$(dirname "${BASH_SOURCE[0]}")/.." || exit 1

FILES="$1"
[ -z "$FILES" ] && { echo "usage: fix_loop.sh <file-list>"; exit 1; }

for round in 1 2 3 4 5 6 7 8; do
    xargs -P 8 -n 1 ./scripts/check_dui_t.sh -q < "$FILES" > /tmp/round_err.txt 2>&1
    n=$(grep -cE ": error:" /tmp/round_err.txt)
    echo "--- round $round: $n errors"
    [ "$n" = "0" ] && break
    res=$(python3 scripts/fix_dui_t.py < /tmp/round_err.txt | tail -1)
    echo "    $res"
    case "$res" in *"wrapped 0, unwrapped 0"*) echo "    no progress; stopping"; break;; esac
done
