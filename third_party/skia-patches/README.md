# Skia customizations

These patches are everything dui changes in Skia, as diffs against one pinned upstream
commit. The build fetches upstream Skia from `google/skia` at that commit, applies them in
name order, and builds the result — see `dui_deps_download_skia()` in
`cmake/dui_deps.cmake`.

| | |
|---|---|
| Upstream commit | `34aa71b8bee4648a442b7125680232d803374f19` (2026-02-10) |
| Patch revision | `dui.2` |
| Files | 41: 20 modified, 21 added |
| Source of these patches | the former fork branch `steveriemannx/skia@dui`, which was this same commit plus three of its own commits |

| Patch | Files | What it is |
|---|---|---|
| `010-mingw-and-msvc.patch` | 15 | Build support for MinGW-w64 alongside MSVC: `gn/` detection (`is_mingw.py`), compiler flags, and the source workarounds that follow from MinGW's preprocessor and printf |
| `020-freebsd.patch` | 1 | `SkDebugf` implemented with `vfprintf` under `__FreeBSD__` |
| `030-text-shaping.patch` | 3 | `skshaper` break iteration, and the plain-text editor's shaping and word boundaries |
| `040-viewer-imgui.patch` | 2 | The viewer against the newer ImGui API (`SetNextTreeNodeOpen` → `SetNextItemOpen`) |
| `050-expat-vendored.patch` | 20 | expat, vendored as a regular tree instead of a submodule |

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

## The size is line endings, not content

`010` is 1.2 MB and `050` is 586 KB, but the **total real content change is about 394
lines**. Everything else is CRLF: upstream is LF, the customized files are CRLF, and a
line-ending conversion is a change on every line of a file.

That is deliberate — these patches reproduce byte-for-byte the tree that builds, and that
is the only property worth trusting. It does mean a reviewer should read them with
`diff <(tr -d '\r' < file) ...` in mind rather than expecting the line counts to mean
anything. `.gitattributes` marks `*.patch` here `-text` so git cannot normalize them
(`core.autocrlf` defaults to true in Git for Windows, which would corrupt both the hunk
syntax and the CRLF they restore).

## Regenerating

The patches must byte-exactly reproduce a tree that builds. That is checkable, and should
be checked whenever any of them changes:

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

When splitting or joining patch files: **slice the patch by bytes, not by text.** Reading
it in a text mode that translates newlines silently drops the CR from every added line,
and the result applies cleanly while producing LF files — verified by comparing against
the tree that builds, which is why that comparison is the acceptance test and not the
patch exit status.

## Two things that are easy to get wrong

- **`patch -N` can exit 0 having applied nothing.** That is why the build also checks for
  `gn/is_mingw.py`, a file the first patch adds, before accepting the result.
- **The last line of a patch file needs its newline.** A generated patch whose final line
  is unterminated produces a target file whose last line is unterminated too — one byte
  short, and only a byte comparison notices.
