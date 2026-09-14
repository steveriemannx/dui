# dui Production Readiness

**Status:** assessment complete; Wave 1 complete, Wave 2 mostly complete (see below)
**Baseline:** assessed at `a353844b`; status updated at `574e59a7` and 2026-09-14
**Date:** 2026-09-14 (revised)
**Companion documents:** [`modern.md`](modern.md) — language and build modernization;
[`string.md`](string.md) — the string-encoding question

## Verdict

dui's feature surface is broad and genuinely complete: the control set, XML-driven
layout, the three development modes, Skia rendering, CEF/WebView2, and the optional
binding module all work. The gap to production-grade is **not missing features**.

The gap is that **every internal safety check disappears in shipped builds, and
nothing runs automatically**. In a released dui application:

- roughly 2,700 `ASSERT()` calls compile to `((void)(0))` — on macOS, Linux *and*
  FreeBSD this is true even in Debug builds (P0-1);
- the log system is a no-op on macOS and X11, so failures leave no trace (P1-3);
- there is no CI, so nothing catches regressions (P1-1);
- there is no sanitizer configuration, so memory errors are found by users (P1-2).

There is at least one reachable use-after-free in the event dispatch path, reachable
from the library's own example code (P0-2).

The encouraging part: every P0 item is a small, local change. None of them require
re-architecting anything.

**And that is not a hypothetical.** Working on the string encoding (see `string.md`)
turned up a defect that this assessment had *not* predicted, and it was a hard crash
rather than a cosmetic bug — see [P0-5](#p0-5-fixed-non-bmp-text-aborted-the-process-in-skia).
It was found only because someone read the code; no test, no CI run and no amount of
"it works on my machine" would have surfaced it.

---

## Status: what has been done

Wave 1 was partially executed after this assessment was written. The rest is untouched.

| Item | State | Commit |
|---|---|---|
| Render path made encoding-agnostic (`HorizontalDrawText` / `VerticalDrawText`) | **done** | `574e59a7` |
| Non-BMP text crash in Skia glyph lookup (P0-5) | **done** | `574e59a7` |
| `StringConvert` fixed 8192-element buffer (8–32 KB alloc+zero per call) | **done** | `574e59a7` |
| Unit test covering the UTF-16 decode branch | **done** | `574e59a7` |
| `DString`/`DUI_T`/`DUI_UNICODE` removed — strings are UTF-8 `std::string` everywhere | **done on a branch, Windows-unverified** | `eaa70572` (`utf8string`) |
| P0-1 `ASSERT` guard | **done** | `2812c547` |
| P0-2 dispatch use-after-free | **done** | `2812c547` |
| P0-3 stray `fprintf` | **done** | `2812c547` |
| P0-4 Wayland `ScreenCapture` missing return | **done** | `2812c547` |
| CI — macOS, Linux, Windows, plus a sanitizer job | **written; macOS job executed locally, Linux and Windows never run** | working tree, 2026-09-14 |
| Sanitizers (`DUI_ENABLE_SANITIZERS`, `sanitize` preset) | **done** | working tree, 2026-09-14 |
| P0-6 `EventSource` throws when a callback is removed during dispatch | **done** | working tree, 2026-09-14 |
| P0-7 detached child kept its parent pointer | **done** | working tree, 2026-09-14 |
| P0-8 `SharePtr` aborts on every last release in a Debug build | **done** | working tree, 2026-09-14 |
| P0-9 default configuration does not compile CEF-free | **done** | working tree, 2026-09-14 |
| P0-10 installed package is unusable by consumers | **done** | working tree, 2026-09-14 |
| P0-11 installed package carried no Skia, so nothing could link against it | **done** | working tree, 2026-09-14 |

**All four original P0 items are fixed.** They were, as predicted, ~1–3 lines each.

Rows marked "working tree, 2026-09-14" are complete and verified but not yet committed.
P0-6 through P0-11 were all found by doing the work above, not by re-reading this
document — see "Defects this document did not predict" below.

---

## What is already solid

Worth recording, so remediation effort goes where it is actually needed.

- **Lifetime management at the top level is correct.** Window teardown
  (`Window::FinalMessage` → `ClearWindow`) is ordered and idempotent, and control
  sub-objects are already `unique_ptr`-managed
  (`include/dui/Core/Control.h:1697-1824`).
- **The weak-callback machinery is real and used.** `ControlPtrT`, `WeakCallback`,
  `UiBind`, and per-callback `WeakFlag` checks exist; the timer system reaps
  callbacks whose owner expired (`src/Core/TimerManager.cpp:171,232`).
- **Image decode failure is reported**, not silent — `kEventImageLoad` /
  `kEventImageDecode` carry an error flag (`src/Core/Control.cpp:4706-4735`).
- **PNG decoding uses `setjmp`/`longjmp` correctly**, including cleanup on the
  error path (`src/Image/Image_PNG.cpp:116-120,220-226`).
- **The install package works** — `cmake/dui_install.cmake`, `cmake/duiConfig.cmake.in`.
  `find_package(dui CONFIG REQUIRED)` plus `dui::dui` now builds **and runs** a real
  application. It did not before 2026-09-14: the package carried no Skia, so a consumer's
  link failed on `sk_*`/`Sk*` symbols. The reason this entry claimed otherwise for so long
  is worth recording — it was "verified" by *configuring* the package and linking a
  program that called nothing, which exercises neither the dependencies nor the link. The
  verification is now an external project built against an installed prefix, running the
  hello example's own sources; see P0-11 below.
- **Dependency versions and licenses are tracked** (`third_party/README.md`,
  `licenses/`, `docs/ThirdParty.md`).
- **GCC/Clang warning levels are configured** — `-Wall -Wextra` and friends
  (`src/CMakeLists.txt:423-452`).

---

## P0 — Must fix first

Each item below was verified by reading the code, not merely reported.

### P0-1. `ASSERT()` is dead on non-Windows platforms, including Debug builds

`include/dui/dui_config.h:119-143` guards the macro with `#ifdef _DEBUG`:

```c
#elif defined DUI_BUILD_FOR_MACOS
    #ifdef _DEBUG
        #define ASSERT(expr)  assert(expr)
    #else
        #define ASSERT(expr)  ((void)(0))
    #endif
```

CMake never defines `_DEBUG` on these platforms. `CMAKE_CXX_FLAGS_DEBUG` is just
`-g`, and the generated flags confirm it:

```
CXX_DEFINES = -DSK_GANESH -DSK_GL -DSK_RELEASE     # no _DEBUG
CXX_FLAGS   = -O3 -DNDEBUG ...                      # Release; Debug is identical in this respect
```

Only Windows gets live assertions, via `_ASSERTE` tracking the debug CRT.

Consequence: **a passing Debug run on macOS proves nothing about the library's
internal invariants**, including the ~100 `AssertUIThread` UI-thread checks.
Note that `ASSERT` here is `_DEBUG`-based, not `NDEBUG`-based, so the `#undef NDEBUG`
trick used by `tests/core_tests.cpp` does *not* revive library assertions.

**Fix:** change the guard to `#if !defined(NDEBUG)`, and define `_DEBUG` alongside
`NDEBUG` handling on MSVC (or drop `_DEBUG` entirely and rely on `NDEBUG`).

### P0-2. Destroying a control inside its own event callback is a use-after-free

`src/Core/EventArgs.cpp:242-261`:

```cpp
EventCallback callback = m_callbackList.at(nIndex).m_callback;  // 252
if ((callback == nullptr) || !callback(args)) return false;     // 253  <- callback deletes the sender
if (nIndex >= m_callbackList.size()) { break; }                 // 256  <- reads freed memory
```

The `args.IsSenderExpired()` check at line 247 runs at the *top of the next
iteration*, after line 256 has already executed. The `weakflag.expired()` checks in
`Control::FireNormalEvents` (`src/Core/Control.cpp:5060-5091`) run after
`EventSource::operator()` returns, so they are equally too late.

The triggering pattern is the most common one in the codebase, and the library's own
examples teach it:

- `examples/ListBox/Item.cpp:36-43` — a delete button whose callback calls
  `parent->RemoveItem(this)`, destroying the very `EventSource` that is executing;
- any click handler that calls `Close()` — on Windows `::SendMessage(WM_CLOSE)`
  destroys the control tree synchronously and returns into a dangling frame.

It does not reproduce today because nothing allocates between the `delete` and the
read, so the freed memory still holds its old value. Under production allocator
reuse this becomes an intermittent crash.

**Fix:** snapshot the callback (and the size) into locals before invoking, so no
member of `this` is touched after the callback returns. Preserve the existing
"callbacks may add/remove themselves during dispatch" semantics.

### P0-3. Five stray debug `fprintf` calls on hot paths

```
src/Core/Window.cpp:2083       every mouse button down (also prints the control name)
src/Core/Window.cpp:2144       every mouse button up
src/Core/Control.cpp:2824      every ButtonUp
src/Core/Control.cpp:2835      every Activate
src/Utils/WinImplBase.cpp:342  every button click
```

Each is followed by `fflush(stderr)` — two to six `write()` syscalls per click, plus
I/O noise, plus control names leaking into logs.

### P0-4. Non-void function with no return value under Wayland

`src/Utils/ScreenCapture_Linux.cpp:14-27` — in a `DUI_BUILD_FOR_WAYLAND` build the
function body is an empty comment, and the non-void function falls off the end.
Undefined behaviour; returns a garbage `std::shared_ptr`, which likely crashes on
destruction.

### P0-5. (fixed) Non-BMP text aborted the process in Skia

Not in the original assessment. Found while working on the string model; fixed in
`574e59a7`.

`HorizontalDrawText` and `VerticalDrawText` converted their input to UTF-16 and then
walked it **one UTF-16 code unit at a time**, measuring and drawing each unit
separately (`HorizontalDrawText.cpp:74`, `:731`). A character outside the BMP occupies
two units, so it was split into two isolated surrogates. The second half of the bug is
what made it fatal: each lone surrogate was handed to `SkFont::measureText` as if it
were a whole character.

On macOS that aborts inside Skia:

```
abort  <-  sk_malloc_flags
SkTypeface_Mac::onCharsToGlyphs  <-  SkFont::measureText
ui::HorizontalDrawText::CalculateTextCharBounds
ui::HorizontalDrawText::DrawString
```

Reproduced directly: the pre-fix build dies with `SIGABRT` when the render example's
text page contains an emoji; the post-fix build runs indefinitely. Two crash reports
on the development machine confirm it, and it is not a fringe case — any user typing
an emoji into a `RichEdit` reaches this path.

The fix iterates by code point, in the `DString`'s own encoding, and hands Skia the
byte range of one whole character. See `string.md` for why this was possible without
touching the public API.

**Why this is in the P0 list even though it is fixed:** it is the strongest available
demonstration of the thesis at the top of this document. The defect was reachable from
library example code, was a hard crash, and was invisible to every mechanical check the
project currently has. The only reason it is fixed is that a human read the code.

---

### P0-6 … P0-11 — found by doing the work, not by this assessment

None of these were predicted above. All five were found within a day of someone
touching the code the previous items touch, which is the same pattern as P0-5.

- **P0-6. `EventSource::operator()` threw `std::out_of_range` when a callback removed a
  callback during dispatch** (`src/Core/EventArgs.cpp`). The loop snapshotted the
  callback count before dispatch but advanced the index unconditionally, so removing a
  callback shifted the list under it: with `[A, B]`, A removing itself left the exit
  guard testing `0 >= 1`, and the next turn ran `m_callbackList.at(1)` on a one-element
  vector. This is the semantics the removal guard's own comment claims to support. Fixed
  allocation-free — the next position depends on whether the callback that just ran is
  still in the list — because this runs on every event of every control.
- **P0-7. A detached child kept its parent pointer** (`Box::DoRemoveItem`,
  `Box::RemoveAllItems`). With `SetAutoDestroyChild(false)` — the documented "the caller
  still owns it" mode — a live control held a back-pointer to a box that could be
  destroyed first, and `PlaceHolder::SetRect`, `GetScrollOffsetInScrollBox()` and
  `IsControlRelated()` all walk it.
- **P0-8. `SharePtr` aborted on every last release in a Debug build**
  (`include/dui/Core/SharePtr.h`). `~NVRefCount` asserts the count is 1, but the
  `store(1)` that satisfies the assert was `#ifdef _DEBUG` — and `_DEBUG` is an MSVC
  macro that this CMake never defines on macOS or Linux. Reviving the asserts (P0-1)
  turned a dormant wrong gate into a live abort on a core facility.
- **P0-9. The default configuration did not compile on a clean checkout**
  (`src/dui.cpp`, `include/dui/dui_config.h:75`). `DUI_BUILD_FOR_CEF` was defined
  unconditionally on non-Windows and `dui.cpp` included `dui_cef.h` unconditionally, but
  the CEF distribution is only downloaded when CEF is enabled — so
  `include/cef_app.h` was not found in the default `-DDUI_ENABLE_CEF=OFF` build. It was
  invisible on any machine that had ever built with CEF on, which is every machine that
  has run the examples.
- **P0-10. The installed package could not be used by a consumer**
  (`cmake/duiConfig.cmake.in`). The exported targets reference `Freetype::Freetype`,
  `Fontconfig::Fontconfig` and `ZLIB::ZLIB` — for a static library CMake records even
  private dependencies as `$<LINK_ONLY:…>`, which is still a target reference — but the
  config file never called `find_dependency`, so `find_package(dui)` failed at generate
  time with "imported targets are referenced, but are missing". This contradicts
  "the install package works" in §What is already solid above; that entry was verified
  by configuring the package, not by consuming it.
- **P0-11. The installed package carried no Skia, so nothing could link against it.**
  Found immediately after P0-10 was fixed, by pointing a real consumer at the package: a
  program made of the `hello` example's own sources failed with `sk_malloc_flags`,
  `SkFontMgr_New_CoreText`, `SkDebugf`, `sk_abort_no_print` and more undefined. dui is a
  **static** library, so it resolves nothing itself — Skia's symbols are satisfied when
  the final executable links, from archives that the build tree's `dui_skia_libs`
  INTERFACE target points at. That target is not exported and the archives were never
  installed. `find_package(dui)` therefore succeeded, `dui::dui` resolved, and every
  program that actually called into dui failed to link.

  Fixed by installing the six archives in `DUI_SKIA_LIBS` (which is exactly the set the
  in-tree applications link, checked against a generated `link.txt`) and recreating the
  import as `dui::skia` in the package config, appended to `dui::dui`'s interface. The
  install now fails outright if no Skia archives are found, because a package without them
  is a package nobody can link against.

  **This is the same failure mode as P0-10 and the same lesson**: the previous "verified"
  claim came from a check that could not fail — a consumer whose `main()` returned 0
  without calling anything. The replacement is an external CMake project, built against
  an installed prefix, that compiles the example's real sources and runs the result.

### Sanitizers find a false positive before they find a bug

Worth recording, because it is convincing and wrong: with `DUI_ENABLE_SANITIZERS=ON`,
ASan reports a deterministic heap-buffer-overflow in `ImageDecoder_SVG.cpp` — a 20-byte
read at the end of an 840-byte allocation made by Skia's SVG DOM builder. It is an
artifact of the sanitizer configuration, not a defect in dui.

`SkASAN.h` defines `SK_SANITIZE_ADDRESS` whenever a translation unit is compiled with
`-fsanitize=address`, and `SkTArray` has a member under it. `SkSVGContainer::fChildren`
is a `TArray`, so an instrumented dui and an uninstrumented Skia disagree about the
layout of every type containing one. Measured: `sizeof(SkSVGSVG)` is **848** in dui's
translation units and **840** in Skia's; the read lands 8 bytes past the end of the
object Skia allocated.

The fix is structural rather than in the decoder: `DUI_ENABLE_SANITIZERS` now builds
Skia with `sanitize = "ASAN"` into `lib/<config>-asan`, so the two can never be mixed
and switching the option on an existing tree cannot silently reuse the uninstrumented
archives. The cost is a full Skia build per sanitizer tree.

**The lesson is the one this document keeps relearning.** A sanitizer configuration that
instruments half the program does not give partial coverage — it gives *unsound* results,
and the first thing a new user of it sees is a fabricated bug in a file that is fine.

---

## P1 — Engineering infrastructure (entirely absent)

| Item | Current state | Impact |
|---|---|---|
| **CI** | **Written, never run.** `.github/workflows/ci.yml` covers macOS, Linux, Windows and a sanitizer job; the macOS job's commands were executed locally, the other two are written from the CMake files and have never executed anywhere | The regression net now exists on paper. Until the workflow has actually run, treat every platform claim below as unverified |
| **Sanitizers** | `DUI_ENABLE_SANITIZERS` + the `sanitize` preset, which builds Skia with ASan too | Working, and the P0-2 use-after-free is reproduced and cleared under it |
| **Logging** | **Fixed.** Five levels, a sink on every platform (macOS gets `stderr` *and* the unified log), default WARN, `DUI_LOG_LEVEL` to raise it, and 24 call sites | The silent-drop on macOS and X11 is gone. Most valuable call site: an unknown XML node name used to mean a missing piece of UI with no diagnostic at all |
| **Error reporting** | Partly. The load failures that mattered (XML, resource, font, image) are logged now | The return-value scheme itself is unchanged; there is still no error-code type |
| **Thread contract** | UI-thread ownership is still enforced only by asserts | `GlobalManager`, `ImageManager`, `FontManager`, `WindowManager` are **unsynchronized**. Fetching an image or font from a worker thread is a data race — now at least a *diagnosed* one in Debug, since the asserts are live again |
| **Crash handling** | No `set_terminate`, no minidump, no reporting | No post-mortem data from production crashes |
| **Layout/render tests** | **Fixed for layout, dispatch and hit-testing.** `tests/behaviour_tests.cpp`: 21 test functions in 7 CTest entries covering event dispatch, weak references, the control tree, layout geometry and hit-testing | Rendering itself still needs a paint target and is untested |
| **Benchmarks** | `PerformanceUtil` exists but has **zero call sites** in the library | No way to measure render or layout performance |
| **Cross-platform build** | **No CI run yet, and no Windows machine was reachable during this work either** | Windows is a first-class target that has still never been compiled. The Windows jobs and the ~8 Windows-only edits made in this pass are all unverified — see below. |

### The Windows gap is not theoretical

On 2026-09-13 the Windows build host (`steve@192.168.0.107`, see
`Progress.md:156-166`) was unreachable. That mattered immediately: the string
unification on branch `utf8string` (`eaa70572`) required Windows-only boundary
conversions, roughly ten of which were written and **not one of them could be compiled**.

Worse, that branch is **pixel-identical to the previous build on macOS** — full build
clean, `ctest` 4/4, screenshots matching to the last pixel. Every local signal says it
is correct. It is nonetheless expected to fail on Windows, because on macOS the change
is provably a no-op while on Windows it changes the string type.

This is the shape of the problem: **the platform that breaks is the platform that is
never built.** A CI job that compiles the library and runs `ctest` on Windows is worth
more than any amount of care on the development machine.

### A cheap way to keep Windows honest

For the string work specifically, the migration can be staged so that it is
continuously compiled rather than discovered in a cliff:

```cmake
option(DUI_STRING_UTF8 "Use UTF-8 internally on all platforms" OFF)
```

With the default `OFF` the existing behaviour is preserved exactly, and CI can build
`Windows + DUI_STRING_UTF8=ON` **without shipping it**. Breakages surface one at a
time. When that job is green, flip the default; then delete the option.

The general lesson applies beyond strings: **add the CI matrix entry before making the
change, not after.**

---

## P2 — Platform gaps: Linux/FreeBSD are second-class

The README claims Windows 7+, Linux, macOS 12+, and FreeBSD. Platform-integration
capabilities differ sharply:

| Capability | Windows | macOS | Wayland | X11 |
|---|---|---|---|---|
| IME (CJK input) | Yes, complete (candidate window positioning) | Yes, complete | **No** | **No** |
| Clipboard | Yes (text) | Yes (text) | Stub | Stub |
| Drag & drop | Yes, control-level | Window-level only | Stub | Absent |
| DPI scaling | Yes, Per-Monitor V2 | Static query only | Hardcoded 1.0 | Hardcoded 1.0 |
| Tray icon | Yes (428 lines) | Stub | Stub | Stub |
| File dialog | Yes | Yes | Stub | Stub |
| GPU/GL rendering | Yes | Yes | Raster only | Raster only |

Notes:

- `src/Core/NativeWindow_Wayland.cpp:568-570` returns `1.0f` from
  `GetWindowDisplayScale` and friends; the primary monitor work area is hardcoded to
  1920x1080.
- **The largest macOS gap is font fallback.** `src/RenderSkia/FontMgr_Skia.cpp:372-411`
  selects a single typeface per font; the code comments admit it "cannot display
  Chinese". A missing glyph renders as a tofu box. Windows is partly shielded by
  DirectWrite, so this surfaces mainly on macOS and Linux.
- **No text shaping.** `skshaper` is linked (`cmake/dui_common.cmake:232`) but never
  used — text goes through `SkFont::measureText`/`getGlyphs`. Arabic/Indic shaping,
  ligatures, and bidi are unsupported.
- **No colour font support** (COLR/CPAL/CBDT/sbix, emoji).
- **Accessibility does not exist.** No UIAutomation, NSAccessibility, or AT-SPI.
  Screen readers cannot read any dui control. (The CEF control has its own handler,
  which covers only embedded browser content.)

### Note on the SDL3 backend

`third_party/SDL3/`, `build-dui-sdl3/`, and the `.tmp_*` scratch files are
**untracked work in progress**. The tracked `src/` and `cmake/` contain no SDL code;
`DUI_ENABLE_SDL` appears in an old `CMakeCache.txt` but has no corresponding option
in the current CMake. The actual work lives in `.tmp_NativeWindow_X11.*` and
`.tmp_RasterWindowContext_unix.cpp`. Historically a complete SDL backend existed on
`origin/dui-sdl3` and was removed by `cf0f67ea`.

---

## P3 — Release engineering

- **No version number.** No `DUI_VERSION` macro, no `project(... VERSION ...)`. The
  only version string is a hardcoded `0.1.0` in two install call sites.
- **No SOVERSION, no shared library support.** `BUILD_SHARED_LIBS` has zero matches;
  the build is always static.
- **A shared build would export nothing.** On non-Windows platforms `DUI_API`
  expands to nothing (`include/dui/dui_config_macos.h:10`), while the library
  compiles with `-fvisibility=hidden` (`src/CMakeLists.txt:427`).
- No CPack, no vcpkg, no Conan, no `CMakePresets.json`.
- **No `-Werror`**, and **no warning level at all on MSVC** (no `/W4`, no `/WX`).
- **Documentation drift:** `CLAUDE.md` still documents `scripts/build_dui_all_in_one.sh|.bat`,
  deleted in `7918e1a9`; it also says C++17 while the build is C++20.
  `third_party/README.md` lists skia `0.1.0` while the code uses `0.1.1`.

---

## Recommended sequence

Ordered so each stage makes the next one cheaper. Items marked ✅ are done.

**Wave 1 — half a day, low risk, immediate payoff**

1. ✅ Change the `ASSERT` guard to `#if !defined(NDEBUG)` (P0-1). It was one line, and it
   immediately found P0-8 — a false assert that aborted on every last `SharePtr`
   release. This item earned its billing.
2. ✅ Delete the five `fprintf` calls (P0-3).
3. ✅ Fix `EventSource::operator()` to snapshot before dispatch (P0-2).
4. ✅ Add the missing `return` in the Wayland `ScreenCapture` path (P0-4).
5. ✅ Stand up a minimal CI: configure, build, `ctest`, **on Windows as well as macOS**.
   Written. The macOS job has been executed locally; the Windows and Linux jobs have
   not run anywhere yet, which is the one part of this wave that is not discharged.

**Wave 2 — once CI exists**

6. ✅ Add ASan/UBSan CMake options and run them in CI. One finding (P0-2, reproduced and
   cleared) and one false positive, which is documented in full above because it is
   convincing enough to waste a day on.
7. ✅ Give `LogUtil` severity levels and a macOS/X11 sink, so resource-load failures
   stop being silent.
8. ✅ (done ahead of schedule) The render path no longer transcodes; see `string.md`
   for why this was a prerequisite for both remaining encoding choices.

**Wave 3 — needs design decisions**

9. ◐ Version number done as a single source of truth (`project(dui VERSION 0.1.0)`).
   Shared library and SOVERSION still require solving `DUI_API` symbol export first.
   **Decide the string encoding before this**, because it is an ABI break and becomes a
   Qt5→Qt6-scale event once a SONAME exists (`string.md`).
10. ☐ An error-code scheme to replace "return false + dead assert".
11. ☐ Linux/FreeBSD IME and font fallback. High effort, requires real-hardware
    verification.

**Before any of that: run the CI.** Every platform claim made in this document is a
claim about macOS, because macOS is the only platform anyone has built on.

---

## Appendix: method

Four parallel surveys (build/packaging, tests and quality, runtime robustness,
platform integration) over the tree at `a353844b`, followed by direct verification of
each load-bearing claim. Findings cited with file and line were read directly; the P0
items in particular were each confirmed by opening the source rather than taken from
the surveys' summaries.

Subsequently revised against work done at `574e59a7` and `eaa70572`: the status
section, P0-5, and the Windows-verification note are new; the sequence now carries
completion markers.

Two things this assessment got wrong, recorded so they are not re-derived:

- It did **not** predict the P0-5 crash. The render path was surveyed for encoding
  cost, and the surrogate-splitting defect underneath it was missed. Cost analysis
  and correctness analysis are not the same activity, and this document only did the
  former for that code.
- The original wording implied the Windows gap was a survey limitation. It is worse
  than that: it is a **standing condition of the project**. The `utf8string` work made
  that concrete — a change that is pixel-identical on macOS and expected to fail on
  Windows, with no way to check.

Known limitation, unchanged: **no Windows host was available.** All Windows-platform
claims here are from reading code, not from building or running. `Progress.md:145-160`
records the same gap independently.
