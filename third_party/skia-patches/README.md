# Skia customizations

These patches are everything dui changes in Skia, as diffs against one pinned upstream
commit. The build fetches upstream Skia from `google/skia` at that commit, applies them in
name order, and builds the result — see `dui_deps_download_skia()` in
`cmake/dui_deps.cmake`.

| | |
|---|---|
| Upstream commit | `34aa71b8bee4648a442b7125680232d803374f19` (2026-02-10) |
| Patch revision | `dui.3` |
| Files | 41: 20 modified, 21 added |
| Source of these patches | the former fork branch `steveriemannx/skia@dui`, which was this same commit plus three of its own commits |

| Patch | Files | Lines | What it is |
|---|---|---|---|
| `010-mingw-and-msvc.patch` | 15 | 767 | Build support for MinGW-w64 alongside MSVC: `gn/` detection (`is_mingw.py`), compiler flags, and the source workarounds MinGW's preprocessor and printf require |
| `020-freebsd.patch` | 1 | 42 | `SkDebugf` implemented with `vfprintf` under `__FreeBSD__` |
| `030-text-shaping.patch` | 3 | 143 | `skshaper` break iteration, and the plain-text editor's shaping and word boundaries |
| `040-viewer-imgui.patch` | 2 | 31 | The viewer against the newer ImGui API (`SetNextTreeNodeOpen` → `SetNextItemOpen`) |
| `050-expat-vendored.patch` | 20 | 16735 | expat, vendored as a regular tree instead of a submodule |

The order is fixed and the list in `cmake/dui_deps.cmake` is explicit, not a glob: the
order is part of the patch set, and a glob would impose an alphabetical one that happens
to be correct today.

## Why patches instead of a fork

A fork archive is opaque: nothing says which upstream commit it started from, so "what
exactly did we change in Skia, and is it still what we think it is" has no answer short of
diffing by hand. A patch against a named public commit answers both, and moving to a newer
Skia becomes "rebase these files".

Patches are applied to a freshly extracted upstream tree, never to a tree that has already
been patched, and the tree is deleted outright if any of them fails rather than left
half-applied.

## The tree is LF, like upstream

The customized files used to be CRLF, which made the patches an order of magnitude larger
than their content: a line-ending conversion is a change on every line of a file, so 394
lines of real change came to 63,200 lines of diff. They are LF now, and the four content
patches total under a thousand lines — `010` went from 1.2 MB to 24 KB.

`050-expat-vendored` is the one that stays large, and it is irreducible: those are 16,735
lines of new source, not churn.

Windows does not care. Both MSVC and MinGW read LF sources, and **upstream Skia is LF** —
so LF is the state the gn/ninja build is developed and tested against; CRLF was the
unusual case here. The one thing on Windows that does care about line endings is
`.bat`/`.cmd`, and nothing in this patch set is a batch file.

`.gitattributes` marks `*.patch` here `-text` for the opposite reason to the usual one:
not to protect CRLF, but to stop git from *adding* it. `core.autocrlf` defaults to true in
Git for Windows, and a patch body converted to CRLF has context lines that no longer match
the LF files it is meant to patch.

## Regenerating

The patches must reproduce a tree that builds. That is checkable, and should be checked
whenever any of them changes:

```sh
# 1. upstream, at the pinned commit
curl -L -o base.zip https://github.com/google/skia/archive/34aa71b8bee4648a442b7125680232d803374f19.zip
unzip -q base.zip && cd skia-34aa71b8bee4648a442b7125680232d803374f19

# 2. apply the current set, then change whatever needs changing in the tree
for p in .../skia-patches/*.patch; do patch -p1 -N -f -i "$p"; done
#    ... edit ...

# 3. regenerate against the *pristine* upstream, not against the patched tree:
#    diff the edited tree against a fresh extraction, with the same exclusions the build
#    uses (out/, bin/, .dui_skia_version, platform_tools/android/apps/{.gradle,build})
```

Then bump `_skia_patch_revision` in `cmake/dui_deps.cmake`. The revision is part of the
extracted tree's version marker, so a stale tree re-extracts instead of silently keeping
the old patches. Bumping `_skia_upstream_commit` does the same, and is also what forces a
re-download.

## Two things that are easy to get wrong

- **`patch -N` can exit 0 having applied nothing.** That is why the build also checks for
  `gn/is_mingw.py`, a file the first patch adds, before accepting the result.
- **Compare bytes, not exit codes.** Both mistakes made while splitting these patches
  applied cleanly and exited 0 while producing the wrong tree: reading the patch in a text
  mode that translates newlines quietly dropped the CR from every added line (40 files
  wrong), and a generated patch whose last line was unterminated left four target files one
  byte short. Only a byte comparison against the tree that builds catches either.
