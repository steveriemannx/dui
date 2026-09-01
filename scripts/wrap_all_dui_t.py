#!/usr/bin/env python3
"""Wrap every remaining bare narrow string literal in the dui examples in DUI_T().

Phase A (fix_dui_t.py) handles literals the compiler rejects. But several dui
APIs carry const char* convenience overloads (ui::Find, StringUtil::Printf,
UiAttr, ...), so bare literals at those call sites compile silently while still
being wrong style for the Windows build. This pass wraps them by rule.

Over-wrapping is caught afterwards by re-running scripts/check_dui_t.sh: a wide
literal reaching a narrow-string context is a hard compile error there.
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from fix_dui_t import find_literals, wrap_line  # noqa: E402

SKIP_DIRS = ('CefBrowser', '/cef/', 'WebView2', 'build-merge', '/build/')
SKIP_EXT = ('.inc', '.mm')

# Lines matching these keep their literals narrow: the target really is a
# std::string / char-based API, so DUI_T() there would break the Windows build.
NARROW_MARKERS = (
    'std::string', 'DStringA', 'U8String', 'UTF8String',
    '.NativePath()', 'UTF8ToT', 'TToUTF8', 'MBCSToT', 'TToMBCS',
    'std::ifstream', 'std::ofstream', 'std::fstream',
    'printf(', 'fprintf(', 'sprintf(', 'snprintf(',
    'SDL_', 'setenv(', 'getenv(',
    # Not DString at all: language linkage, static assertions, pragmas.
    'extern "C"', 'static_assert(', '_Pragma(', 'deprecated(',
)


def should_skip_line(line):
    # Several sources carry a UTF-8 BOM, which would otherwise hide the '#'
    # of a first-line preprocessor directive.
    stripped = line.lstrip('﻿').lstrip()
    if stripped.startswith('#'):
        return True
    if stripped.startswith('//') or stripped.startswith('*'):
        return True
    return any(m in line for m in NARROW_MARKERS)


def main():
    roots = sys.argv[1:] or ['examples']
    total, touched = 0, []
    for root in roots:
        for dirpath, _dirs, files in os.walk(root):
            if any(s in dirpath + '/' for s in SKIP_DIRS):
                continue
            for fn in sorted(files):
                if not fn.endswith(('.cpp', '.h', '.hpp')):
                    continue
                if fn.endswith(SKIP_EXT):
                    continue
                path = os.path.join(dirpath, fn)
                with open(path, encoding='utf-8') as fh:
                    lines = fh.read().split('\n')
                count = 0
                for i, line in enumerate(lines):
                    if should_skip_line(line):
                        continue
                    if not any(not w for _s, _e, w in find_literals(line)):
                        continue
                    new, cnt = wrap_line(line)
                    if cnt:
                        lines[i] = new
                        count += cnt
                if count:
                    with open(path, 'w', encoding='utf-8') as fh:
                        fh.write('\n'.join(lines))
                    touched.append((path, count))
                    total += count
    for path, cnt in touched:
        print(f'{cnt:5d}  {path}')
    print(f'--- wrapped {total} literals in {len(touched)} files')


if __name__ == '__main__':
    main()
