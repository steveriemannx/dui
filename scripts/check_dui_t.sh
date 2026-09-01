#!/bin/bash
# Type-check example sources with DString == std::wstring (the Windows configuration),
# so that missing/incorrect DUI_T() wrappers surface as compile errors on macOS.
#
# Usage: check_dui_t.sh [-q] <source-file> [<source-file> ...]
#   -q   concise mode: print only "file:line:col: error: ..." lines
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD="$ROOT/build-merge-ready"

QUIET=0
if [ "$1" = "-q" ]; then QUIET=1; shift; fi

status=0
for src in "$@"; do
    dir="$(cd "$(dirname "$src")" && pwd)"
    name="$(basename "$dir")"
    out=$(/usr/bin/c++ -fsyntax-only -std=gnu++20 -arch arm64 \
        -DUNICODE -D_UNICODE \
        -ferror-limit=0 -fno-caret-diagnostics -fno-diagnostics-fixit-info \
        -I"$BUILD/examples/$name" \
        -I"$ROOT" -I"$ROOT/include" -I"$dir" \
        -I"$BUILD/src/third_party/zlib" -I"$ROOT/third_party/zlib" \
        -F/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/Frameworks \
        -Wno-everything \
        "$src" 2>&1)
    rc=$?
    if [ $rc -ne 0 ]; then status=1; fi
    if [ $QUIET -eq 1 ]; then
        echo "$out" | grep -E "(error|fatal error):" | grep -v "^/Library/" || true
    else
        echo "$out"
    fi
done
exit $status
