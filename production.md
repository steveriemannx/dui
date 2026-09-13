# dui Production Readiness

**Status:** assessment complete, remediation not started
**Baseline:** branch `production`, commit `a353844b` (identical to `main` at time of writing)
**Date:** 2026-09-13

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
- **The install package works.** `find_package(dui CONFIG REQUIRED)` plus
  `dui::dui` is implemented and verified (`cmake/dui_install.cmake`,
  `cmake/duiConfig.cmake.in`, `Progress.md:60-92`).
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

---

## P1 — Engineering infrastructure (entirely absent)

| Item | Current state | Impact |
|---|---|---|
| **CI** | **None.** `.github/` contains only templates; no workflow files anywhere | The 4 CTest cases have never been run automatically |
| **Sanitizers** | Zero matches repo-wide | The P0-2 use-after-free would have been caught by ASan |
| **Logging** | `src/Utils/LogUtil.cpp:34-38` has `#ifdef DUI_BUILD_FOR_WIN` and `DUI_BUILD_FOR_WAYLAND` branches with **no `else`** | **Log output is silently dropped on macOS and X11.** No INFO/WARN/ERROR levels. Only 8 call sites in the whole library |
| **Error reporting** | Errors are return values plus `ASSERT` | With asserts dead, resource-load failures are silent. An unknown XML node name (`src/Core/WindowBuilder.cpp:1206`) means a missing piece of UI with no diagnostic |
| **Thread contract** | UI-thread ownership enforced only by the ~100 dead asserts | `GlobalManager`, `ImageManager`, `FontManager`, `WindowManager` are **unsynchronized**. Fetching an image or font from a worker thread is a data race with no Release-mode diagnostic |
| **Crash handling** | No `set_terminate`, no minidump, no reporting | No post-mortem data from production crashes |
| **Layout/render tests** | Tests cover utilities and strings only | Layout engine, rendering, hit-testing, event dispatch: **zero coverage** |
| **Benchmarks** | `PerformanceUtil` exists but has **zero call sites** in the library | No way to measure render or layout performance |

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

Ordered so each stage makes the next one cheaper.

**Wave 1 — half a day, low risk, immediate payoff**

1. Change the `ASSERT` guard to `#if !defined(NDEBUG)` (P0-1). One line; revives ~2,700
   checks in Debug.
2. Delete the five `fprintf` calls (P0-3).
3. Fix `EventSource::operator()` to snapshot before dispatch (P0-2).
4. Add the missing `return` in the Wayland `ScreenCapture` path (P0-4).
5. Stand up a minimal CI: configure, build, `ctest`. The regression net comes first.

**Wave 2 — once CI exists**

6. Add ASan/UBSan CMake options and run them in CI. This will likely surface more
   memory errors beyond P0-2.
7. Give `LogUtil` severity levels and a macOS/X11 sink, so resource-load failures
   stop being silent.

**Wave 3 — needs design decisions**

8. Shared library, version number, SOVERSION — requires solving `DUI_API` symbol
   export first.
9. An error-code scheme to replace "return false + dead assert".
10. Linux/FreeBSD IME and font fallback. High effort, requires real-hardware
    verification.

---

## Appendix: method

Four parallel surveys (build/packaging, tests and quality, runtime robustness,
platform integration) over the tree at `a353844b`. Findings cited with file and line
were read directly; the P0 items in particular were each confirmed by opening the
source rather than taken from the surveys' summaries.

Known limitation: **no Windows host was available.** All Windows-platform claims
here are from reading code, not from building or running. `Progress.md:145-160`
records the same gap independently.
