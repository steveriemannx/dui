#!/usr/bin/env python3
"""Compiler-guided DUI_T() wrapper for dui example sources.

Reads clang error output (from scripts/check_dui_t.sh -q, which compiles with
DString == std::wstring), and wraps the bare narrow string literals on each
flagged line in DUI_T(...).

The compiler is the oracle in both directions: a literal that must be wide but
isn't produces an error (we wrap it), and a literal that must stay narrow but
got wrapped also produces an error (we unwrap it). Iterating to a clean build
therefore proves the result compiles in the Windows string configuration.
"""
import re
import sys

ERR_RE = re.compile(r'^([^:]+):(\d+):(\d+): (?:fatal )?error: (.*)$')

# Errors that mean "a wide literal was used where a narrow one is required",
# i.e. we over-wrapped and must undo.
OVERWRAP_HINTS = (
    "basic_string<char>", "const char *", "char[", "std::string",
)


def parse_errors(text):
    """-> {path: {line: [messages]}}"""
    out = {}
    for line in text.splitlines():
        m = ERR_RE.match(line.strip())
        if not m:
            continue
        path, ln, _col, msg = m.groups()
        out.setdefault(path, {}).setdefault(int(ln), []).append(msg)
    return out


def find_literals(line):
    """Yield (start, end, already_wrapped) for each string literal in `line`.

    Skips literals with a prefix that makes them non-narrow-char (L"", u8"",
    R"()", @"") and stops at a line comment outside of a string.
    """
    i, n = 0, len(line)
    while i < n:
        c = line[i]
        if c == '/' and i + 1 < n and line[i + 1] == '/':
            return
        if c == "'":  # char literal - skip over it
            i += 1
            while i < n and line[i] != "'":
                i += 2 if line[i] == '\\' else 1
            i += 1
            continue
        if c != '"':
            i += 1
            continue
        # Found a literal. Inspect its prefix.
        start = i
        prefix_end = start
        j = start - 1
        while j >= 0 and (line[j].isalnum() or line[j] == '_'):
            j -= 1
        prefix = line[j + 1:prefix_end]
        # Scan to the closing quote.
        i += 1
        while i < n and line[i] != '"':
            i += 2 if line[i] == '\\' else 1
        i += 1
        end = i
        if prefix in ('L', 'u8', 'u', 'U', 'R', 'LR', 'u8R'):
            continue
        if j >= 0 and line[j] == '@':
            continue
        if prefix:  # some other identifier glued to the quote - leave alone
            continue
        wrapped = line[:start].rstrip().endswith('DUI_T(')
        yield (start, end, wrapped)


def wrap_line(line):
    """Wrap every bare literal on `line` in DUI_T(). Returns (new_line, count)."""
    spans = [(s, e) for s, e, w in find_literals(line) if not w]
    if not spans:
        return line, 0
    out, prev = [], 0
    for s, e in spans:
        out.append(line[prev:s])
        out.append('DUI_T(' + line[s:e] + ')')
        prev = e
    out.append(line[prev:])
    return ''.join(out), len(spans)


def unwrap_line(line):
    """Remove DUI_T() around literals on `line`. Returns (new_line, count)."""
    new, cnt = re.subn(r'DUI_T\((\"(?:[^\"\\\\]|\\\\.)*\")\)', r'\1', line)
    return new, cnt


def statement_range(lines, idx):
    """Extend from line idx to the end of its statement (balanced ()/{}).

    Used for multi-line constructs where clang reports the error on the first
    line but the offending literals live on continuation lines.
    """
    depth = 0
    for k in range(idx, min(idx + 40, len(lines))):
        text = re.sub(r'//.*$', '', lines[k])
        text = re.sub(r'"(?:[^"\\]|\\.)*"', '""', text)
        depth += text.count('(') + text.count('{') - text.count(')') - text.count('}')
        if depth <= 0 and (';' in text or k > idx):
            return k
    return idx


def main():
    err_text = sys.stdin.read()
    errors = parse_errors(err_text)
    total_wrapped = total_unwrapped = 0
    touched = []

    for path, linemap in sorted(errors.items()):
        if path.endswith('.inc'):        # generated files: excluded for now
            continue
        if '/build-merge' in path or path.startswith('/Library'):
            continue
        try:
            with open(path, encoding='utf-8') as fh:
                lines = fh.read().split('\n')
        except OSError:
            continue

        changed = 0
        for ln, msgs in sorted(linemap.items()):
            idx = ln - 1
            if idx < 0 or idx >= len(lines):
                continue
            if lines[idx].lstrip().startswith('#'):
                continue

            over = any(h in m for m in msgs for h in OVERWRAP_HINTS)
            if over and 'wchar_t' in ' '.join(msgs):
                # A wide literal reached a narrow-string context: undo the wrap.
                new, cnt = unwrap_line(lines[idx])
                if cnt:
                    lines[idx] = new
                    total_unwrapped += cnt
                    changed += cnt
                    continue

            end = statement_range(lines, idx)
            for k in range(idx, end + 1):
                if lines[k].lstrip().startswith('#'):
                    continue
                new, cnt = wrap_line(lines[k])
                if cnt:
                    lines[k] = new
                    total_wrapped += cnt
                    changed += cnt

        if changed:
            with open(path, 'w', encoding='utf-8') as fh:
                fh.write('\n'.join(lines))
            touched.append((path, changed))

    for path, cnt in touched:
        print(f'{cnt:5d}  {path}')
    print(f'--- wrapped {total_wrapped}, unwrapped {total_unwrapped}, '
          f'files {len(touched)}')


if __name__ == '__main__':
    main()
