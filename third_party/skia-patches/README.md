# Skia customizations

`dui.patch` is everything dui changes in Skia, as a diff against one pinned upstream
commit. The build fetches upstream Skia from `google/skia` at that commit, applies this
patch, and builds the result — see `dui_deps_download_skia()` in `cmake/dui_deps.cmake`.

| | |
|---|---|
| Upstream commit | `34aa71b8bee4648a442b7125680232d803374f19` (2026-02-10) |
| Patch revision | `dui.1` |
| Contents | 41 files: 20 modified, 21 added (the expat sources under `third_party/externals/expat/`) |
| Source of this file | the former fork branch `steveriemannx/skia@dui`, which was this same commit plus three of its own commits |

## Why a patch instead of a fork

A fork archive is opaque: nothing says which upstream commit it started from, so
"what exactly did we change in Skia, and is it still what we think it is" has no
answer short of diffing by hand. A patch against a named public commit answers both,
and moving to a newer Skia becomes "rebase this one file".

`patch` is applied to the freshly extracted upstream tree, never to a tree that has
already been patched, and the tree is deleted outright if any part of it fails rather
than left half-applied.

## Regenerating

The patch must byte-exactly reproduce a tree that builds. That is checkable, and
should be checked whenever the file changes:

```sh
# 1. upstream, at the pinned commit
curl -L -o base.zip https://github.com/google/skia/archive/34aa71b8bee4648a442b7125680232d803374f19.zip
unzip -q base.zip && cd skia-34aa71b8bee4648a442b7125680232d803374f19

# 2. apply the current patch, then change whatever needs changing in the tree
patch -p1 -N -f -i /path/to/dui.patch
#    ... edit ...

# 3. regenerate against the *pristine* upstream, not against the patched tree:
#    diff the edited tree against a fresh extraction, with the same exclusions the
#    build uses (out/, bin/, .dui_skia_version, platform_tools/android/apps/{.gradle,build})
```

Then bump `_skia_patch_revision` in `cmake/dui_deps.cmake`. The revision is part of the
extracted tree's version marker, so a stale tree re-extracts instead of silently
keeping the old patch. Bumping `_skia_upstream_commit` does the same, and is also what
forces a re-download.

## Two things that are easy to get wrong

- **Line endings are part of the patch.** Most of the diff is larger than the semantic
  change because the customized files are CRLF while upstream is LF. That is
  deliberate: it reproduces the tree that builds. `.gitattributes` marks the file
  `-text` so git cannot normalize it (`core.autocrlf` is on by default in Git for
  Windows, and would corrupt both the hunk syntax and the CRLF).
- **`patch -N` can exit 0 having applied nothing.** That is why the build also checks
  for `gn/is_mingw.py`, a file this patch adds, before accepting the result.
