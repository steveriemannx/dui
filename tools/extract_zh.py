#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Extract all Chinese UI strings from XML resources and C++ sources.

Usage:
  python3 tools/extract_zh.py extract [out_file]     # extract unique strings
  python3 tools/extract_zh.py apply  [map_file]      # apply translations
  python3 tools/extract_zh.py report [map_file]      # show untranslated count

Extraction targets (strings shown to users / literals):
  - XML:  any attribute value containing CJK (text, tooltip_text, prompttext, ...)
  - XML:  element text content containing CJK (RichText etc.)
  - C++:  _T("...") / L"..." / "..." literals containing CJK
Excluded (kept as-is):
  - bin/resources/lang/zh_CN.txt (language pack)
  - MultiLang/MultiLang.xml and global.xml text_id mechanism is runtime-switched,
    but their literal text attributes are still translated unless flagged below.
"""
import re
import sys
import os
import json

CJK = re.compile(r'[一-鿿]')
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# dirs to scan
XML_DIRS = [os.path.join(ROOT, 'bin/resources/themes/default')]
CODE_DIRS = [os.path.join(ROOT, 'src'), os.path.join(ROOT, 'examples'),
             os.path.join(ROOT, 'cmake'), os.path.join(ROOT, 'build')]

# files/dirs to skip entirely
SKIP_SUFFIXES = ('.png', '.jpg', '.jpeg', '.gif', '.svg', '.ico', '.icns',
                 '.bmp', '.webp', '.apng', '.lottie', '.json', '.rc', '.ico',
                 '.vcxproj', '.sln', '.filters', '.app')
SKIP_DIRS = ('build_temp', '.git', 'third_party', 'lib', 'bin/cef',
             'CefBrowser.app', 'cef.app', 'SDL', 'skia')


def skip(path):
    for s in SKIP_SUFFIXES:
        if path.endswith(s):
            return True
    for d in SKIP_DIRS:
        if ('/' + d + '/') in path.replace(ROOT, ''):
            return True
    return False


def iter_files():
    for base in XML_DIRS + CODE_DIRS:
        for dirpath, dirnames, filenames in os.walk(base):
            dirnames[:] = [d for d in dirnames if d not in ('build_temp', '.git')]
            for fn in filenames:
                p = os.path.join(dirpath, fn)
                if skip(p):
                    continue
                if p.endswith('.xml') or p.endswith('.cpp') or p.endswith('.h') \
                   or p.endswith('.cmake') or p.endswith('.sh') or p.endswith('.txt'):
                    yield p


def extract_from_xml(text):
    """attr="value-with-CJK" and element text nodes with CJK"""
    found = []
    # attribute values
    for m in re.finditer(r'([\w_]+)="([^"]*[一-鿿][^"]*)"', text):
        found.append(('attr', m.group(1), m.group(2)))
    # element text nodes containing CJK (e.g. RichText content)
    for m in re.finditer(r'>([^<>]*[一-鿿][^<>]*)<', text):
        found.append(('textnode', '', m.group(1).strip()))
    return found


def extract_from_code(text):
    found = []
    # _T("...") with CJK (single-line only; no newlines inside)
    for m in re.finditer(r'_T\("((?:[^"\\\n]|\\.)*[一-鿿](?:[^"\\\n]|\\.)*)"\)', text):
        found.append(('code', '_T', m.group(1)))
    # L"..." with CJK (single-line only)
    for m in re.finditer(r'L"((?:[^"\\\n]|\\.)*[一-鿿](?:[^"\\\n]|\\.)*)"', text):
        found.append(('code', 'L', m.group(1)))
    return found


def extract_all():
    strings = {}   # string -> list of (file, kind, attr)
    for p in iter_files():
        try:
            with open(p, encoding='utf-8') as f:
                text = f.read()
        except (UnicodeDecodeError, OSError):
            continue
        if p.endswith('.xml'):
            items = extract_from_xml(text)
        else:
            items = extract_from_code(text)
        for kind, attr, s in items:
            if s not in strings:
                strings[s] = []
            strings[s].append((p, kind, attr))
    return strings


def main():
    cmd = sys.argv[1] if len(sys.argv) > 1 else 'extract'
    map_file = sys.argv[2] if len(sys.argv) > 2 else os.path.join(ROOT, 'tools/zh_map.json')

    if cmd == 'extract':
        strings = extract_all()
        # load existing map to preserve already-translated entries
        existing = {}
        if os.path.exists(map_file):
            with open(map_file, encoding='utf-8') as f:
                existing = json.load(f)
        out = {}
        for s in sorted(strings):
            out[s] = existing.get(s, '')
        with open(map_file, 'w', encoding='utf-8') as f:
            json.dump(out, f, ensure_ascii=False, indent=1)
        total = len(out)
        done = sum(1 for v in out.values() if v)
        print(f'Unique strings: {total}, translated: {done}, pending: {total - done}')

    elif cmd == 'report':
        with open(map_file, encoding='utf-8') as f:
            mapping = json.load(f)
        pending = [s for s, v in mapping.items() if not v]
        print(f'Total: {len(mapping)}, translated: {len(mapping) - len(pending)}, pending: {len(pending)}')
        for s in pending[:50]:
            print('  ', s)

    elif cmd == 'apply':
        with open(map_file, encoding='utf-8') as f:
            mapping = json.load(f)
        missing = [s for s, v in mapping.items() if not v]
        if missing:
            print(f'ERROR: {len(missing)} strings not translated yet')
            for s in missing[:20]:
                print('  ', s)
            sys.exit(1)
        # sort by length desc to replace longer strings first
        items = sorted(mapping.items(), key=lambda kv: -len(kv[0]))
        applied = 0
        for p in iter_files():
            try:
                with open(p, encoding='utf-8') as f:
                    text = f.read()
            except (UnicodeDecodeError, OSError):
                continue
            orig = text
            if p.endswith('.xml'):
                # replace inside attribute values and text nodes
                for zh, en in items:
                    if not zh or zh == en:
                        continue
                    # attribute value replace
                    text = re.sub(r'="' + re.escape(zh) + r'"',
                                  '="' + en.replace('\\', '\\\\').replace('"', '&quot;') + '"', text)
                    # text node replace
                    text = text.replace('>' + zh + '<', '>' + en + '<')
            else:
                for zh, en in items:
                    if not zh or zh == en:
                        continue
                    # C string escaping for the replacement value:
                    # backslash -> \\, quote -> \", newline -> \n
                    esc_en = en.replace('\\', '\\\\').replace('"', '\\"').replace('\n', '\\n')
                    text = text.replace('_T("' + zh + '")', '_T("' + esc_en + '")')
                    text = text.replace('L"' + zh + '"', 'L"' + esc_en + '"')
                    text = text.replace('"' + zh + '"', '"' + esc_en + '"')
            if text != orig:
                applied += 1
                with open(p, 'w', encoding='utf-8') as f:
                    f.write(text)
        print(f'Applied to {applied} files')

    else:
        print(__doc__)


if __name__ == '__main__':
    main()
