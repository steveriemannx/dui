# dui Modernization Assessment

**Status:** assessment complete; Tier 1 and most of Tier 2 of Part 4 executed (see Part 4)
**Baseline:** assessed at `a353844b`; status updated at `eaa70572` and 2026-09-14
**Date:** 2026-09-14 (revised)
**Scope:** C++ language modernization, pointer/ownership model, CMake modernization
**Companion documents:** [`production.md`](production.md) — production readiness;
[`string.md`](string.md) — the string-encoding question in full

## Verdict

Three separate questions were asked. They have three different answers.

| Question | Answer |
|---|---|
| Can we adopt modern C++ (C++20)? | **Yes, and it is already switched on.** `-std=gnu++20` is active. The problem is that the *code* uses roughly 3% of C++20. |
| Can we convert the control tree to smart pointers? | **Not as a whole-tree change.** Two hard obstacles make it infeasible, and it would not fix the bugs it is expected to fix. A targeted subset is worth doing. |
| Can we modernize CMake? | **Yes, with one structural exception** — Skia is a gn/ninja project and cannot become a CMake target. The rest is ordinary work. |

The single most important finding: **these three goals are mostly independent, and the highest-value work is not the work that looks most like "modernization."** Converting 200,000 lines to smart pointers would be the largest and riskiest change on this list, and it would not fix a single known defect. Several much smaller changes would.

Scale, for calibrating every estimate below: the library is **~209,000 lines** (68k headers, 141k implementation) across 318 headers and 275 source files; examples add another ~60,000 lines.

### Revision note

Two things in the original version needed correcting, and both are recorded here
rather than quietly edited out:

- **§1.3 recommended unifying on `char8_t`/`std::u8string` as one of two options.**
  That is wrong — `u8string` is a dead end, and §1.3 now says so. See §1.6.
- **The estimated cost of the string migration was too pessimistic.** It was written
  as "3,485 `DUI_T` sites, 182 files, plus the public API surface", implying a
  large, delicate refactor. The actual mechanical change turned out to be **two
  lines of definition**; §1.6 gives the measured result.

---

## Part 1 — C++ language level

### 1.1 The standard is already C++20

`src/CMakeLists.txt:35-36` sets `CMAKE_CXX_STANDARD 20` with `CXX_STANDARD_REQUIRED ON`, and the build output confirms it:

```
CXX_FLAGS = -O3 -DNDEBUG -std=gnu++20 -arch arm64 ...
```

Two caveats, both since fixed:

- ~~`CMAKE_CXX_EXTENSIONS` is never set, so the default `ON` applies — the build is **`gnu++20`, not `c++20`**.~~
  **Fixed 2026-09-14.** `CMAKE_CXX_EXTENSIONS OFF` is set in `cmake/dui_common.cmake`, which
  every module includes, so library, tests and examples all compile as strict C++ 20. Verified
  by building them that way — not by assuming, because the flags could have been relying on
  GNU extensions and there was no way to know without compiling.
- ~~`dui` and `dui_entry` never call `target_compile_features(dui PUBLIC cxx_std_20)`, so
  **consumers of the installed package are not told the headers require C++20**.~~
  **Fixed 2026-09-14**, on both targets.
  **And the surrounding install package turned out to be worse than this caveat described** —
  see P0-10 in `production.md`: `find_package(dui)` failed outright for any consumer, because
  the config file never resolved the dependencies the exported targets name.

### 1.2 Adoption is layered, and the layers diverge sharply

| Layer | Adoption | Evidence |
|---|---|---|
| **C++11 hygiene** | **~85%** | `nullptr` 8,366 vs `NULL` 30 (99.6%); `override` ~85-90% of non-pure virtuals; `= delete` 200 uses; `enum class` 52; `std::function` 149 uses vs **2** raw function-pointer typedefs; ~700 smart-pointer uses; `std::filesystem` 60 uses; `using namespace` used exactly once |
| **C++11 conveniences** | **~35%** | range-for 440 vs classic `for` 530 (~46%); `auto` density ~0.4% (679 uses in 209k lines); `constexpr` only 63 uses; explicit move semantics in only 3 files; `std::forward` **zero** uses in `src/` |
| **C++14/17** | **~25%** | `make_unique` 146 uses (good), but `std::optional` 3, `std::variant` **0**, `std::string_view` **0**, structured bindings **1**, `if constexpr` 7; `typedef` : `using` = 370 : 23 (**93% typedef**); raw `new` 298 / raw `delete` 212 |
| **C++20** | **~3%** | `concepts` **0**, `requires` **0**, `<ranges>` **0**, `<format>` **0**, `std::span` **0**, `consteval` **0**, `constinit` **0**, `[[nodiscard]]` **1**, `<bit>`/`<numbers>`/`<source_location>` **0** |

**Weighted overall: roughly 35%.** The most striking fact is the gap between the layers: the standard was raised to C++20, but the code's center of gravity is C++03 with C++11 patches. C++20's three flagship features — concepts, ranges, and `<format>` — have a combined usage count of **zero**.

### 1.3 The root cause is the string model, not coding discipline

This is the key diagnostic finding, and it changes what "modernization" should mean here.

`DString` is **platform-dependent** (`include/dui/dui_string.h:59-97`):

| Platform | `DUI_UNICODE` | `DString` is | Encoding |
|---|---|---|---|
| Windows | defined | `std::wstring` (wchar_t = 2B) | UTF-16 |
| macOS | **not defined** | **`std::string`** | **UTF-8** |
| Linux / FreeBSD | **not defined** | **`std::string`** | **UTF-8** |

To paper over this, the library uses `DUI_T("...")` (`dui_string.h:13-21`), which expands to `L"..."` on Windows and `"..."` elsewhere. That macro appears **3,485 times across 182 files**.

That single design decision cascades:

1. **`DUI_T` is a macro**, so every literal is invisible to refactoring tools, static analysis, and IDEs.
2. It **blocks `constexpr` strings and `std::string_view` parameters**. Any API taking a literal must take `const DString&`. This is why `include/dui/dui_string.h:65-77` defines six `*StringView` typedefs that have **zero references** anywhere in the repo.
3. It is **why the 110 `DUI_CTR_*` macros in `include/dui/dui_defs.h:12+` cannot become `inline constexpr`**.
4. It is a significant share of the **~1,200 C-style casts**, because the Win32 parameter model (`WPARAM`/`LPARAM`/`MAKEWORD`/`GET_X_LPARAM`) was rebuilt in `include/dui/dui_config_macos.h:14-41` to make the dual-type scheme work.
5. That in turn forces `dui_config_macos.h:35-36` to pull `<Carbon/Carbon.h>` into a cross-platform public header.

**Implication:** the correct "first cut" for C++ modernization is the string model —
unify the internal encoding, and convert only at the platform boundary. That one change
makes `DUI_T`, the `WCHAR_T_IS_*` detection block, the six unused `*StringView`
typedefs, and a large fraction of the C-style casts all disappear together.

**Correction to the original text:** this paragraph used to offer
`char8_t`/`std::u8string` as one of the two unification targets. That was wrong, and
the mistake is worth spelling out because it is the obvious-looking choice:

| Claim | Reality |
|---|---|
| It gives you type-safe UTF-8 | **It does not.** `char` and `char8_t` "maintain exactly the same set of invariants — the empty set"; even `u8"text"` is not guaranteed UTF-8 and can vary translation unit by translation unit. |
| The ecosystem will adopt it | **It has not.** A GitHub code search cited in P2728 found ~15.3 M `std::string` references against ~6.7 k `std::u8string` — roughly 2300:1, six years after C++20. |
| It interoperates | **It does not.** No system API takes `char8_t` "and likely never will be", and `std::print`/streams do not support it. P1747R0 is literally titled *"Don't use `char8_t` and `std::u8string` yet"*. |

So the target is `std::string` holding UTF-8. That is a **convention, not a type
guarantee** — nothing enforces it — which is exactly why the boundary discipline in
`string.md` matters more than the type choice does. §1.6 has the measured result of
doing it.

### 1.4 A concrete correctness risk, not just a style question

`src/CMakeLists.txt:437` sets **`-fno-threadsafe-statics`** on the `dui` target, and the build output confirms it is active.

This disables the C++11 guarantee that a function-local `static` with non-trivial initialization is initialized exactly once, safely, under concurrency. With the flag, two threads can both run the constructor.

That guarantee is load-bearing here, because the codebase has:

- **function-local `static` singletons**: `src/Core/GlobalManager.cpp:82` (`static GlobalManager self`), `src/Utils/PerformanceUtil.cpp:30`, `src/WebView2/WebView2Manager.cpp:40`, plus several more;
- **function-local `static` containers** read from multiple paths: `src/Core/EventArgs.cpp:177,189` (two `static std::unordered_map`), `src/Animation/EasingFunctions.cpp:440,484` (two `static std::map`), and ~25 `GetProcAddress` caches in `src/Utils/ApiWrapper_Windows.cpp`;
- a genuinely multithreaded architecture: 57 `std::mutex`, 41 `std::lock_guard`, 13 `std::unique_lock`, plus `FrameworkThread`, `ThreadManager`, and separate image-decode threads.

So "it's effectively single-threaded" is not available as a defence.

The flag looks inherited from Chromium/Skia build conventions, where such trade-offs are made knowingly. It is cheap to fix: either drop the flag from the `dui` target, or convert the ~10 non-trivial function-local statics to `std::call_once` + `std::once_flag`. The repo already contains a correct example to copy: `src/Core/NativeWindow_MacOS.mm:1448`.

**Fixed 2026-09-14, by the first option** — the flag is gone rather than worked around, so the C++11 guarantee is restored for every static in the library, including ones added later. The removal carries a comment naming the statics that depend on it, so the next person to consider re-adding it for build speed finds the reason not to.

### 1.5 Low-cost, high-value language cleanups

| Change | Scale | Why |
|---|---|---|
| `constexpr` on value-type accessors | `UiRect.h`, `UiPoint.h`, `UiSize.h`, `UiSize64.h`, `UiPointF.h`, `UiRectF.h` — e.g. `UiRect` has only 4 of 66 members marked | Enables constant folding and `constexpr` callers; no API break |
| `inline constexpr` for constant macros | ~140 colours in `UiColors.h:14`; 77 in `Utils/Macros_Windows.h`; 12 in `dui_config_macos.h` | Removes macro invisibility |
| `typedef` → `using` | 370 sites | Mechanical; `src/` currently has **zero** `using` aliases |
| C-style casts → `static_cast` | ~1,200, mostly numeric (`(int32_t)` alone is 616) | Mechanical and safe for the numeric ones |
| `std::size` instead of `sizeof/sizeof` | 3 sites | Trivial |
| Range-for where a classic loop adds nothing | ~530 classic loops remain | Readability, not correctness |

None of these change behaviour, and each is reviewable in isolation.

### 1.6 The string migration, measured

Executed on branch `utf8string`, commit `eaa70572`. Replaces the estimate this
document originally gave.

**The scale is misleading in one direction and honest in another.**

| | |
|---|---|
| Files changed | **685** |
| `DString` / `DStringA` / `DStringW` occurrences rewritten | ~4,033 |
| `DUI_T(...)` unwrapped | ~29,164 |
| **Lines of definition actually written** | **a few** |

That last row is the point. The whole migration is a handful of lines in two headers.
(`DUI_T` and `DString` were separate blocks; shown together here for clarity.)

```cpp
// dui_string.h -- before, the string type
#ifdef DUI_UNICODE
    typedef std::wstring DString;
#else
    typedef std::string  DString;
#endif
// dui_string.h -- before, the literal macro
#if !defined(DUI_T)
    #if defined (DUI_UNICODE)
        #define DUI_T(x)  L##x
    #else
        #define DUI_T(x)  x
    #endif
#endif

// after: one typedef, no macro
typedef std::string DString;
```

Everything else is consequences. The 34,000 site rewrites are mechanical; the real
work is the fallout, and the fallout is **platform-specific**.

**What the fallout actually was:**

| Platform | Effect of the change |
|---|---|
| macOS / Linux / FreeBSD | **None.** `DString` was already `std::string` there — provably a no-op. |
| Windows | Every place that relied on `DString` being `std::wstring` breaks. |

The macOS result is verified, not assumed: full build clean, `ctest` 4/4, and a
screenshot of the render example identical to the pre-change build **pixel for pixel**.

**Windows: done, and the estimate was low.** No Windows host was reachable when this
was written; one was reached on 2026-09-14 and the branch was built on it. Roughly ten
boundary sites had been found by reading and "not one was compiled" — of those, every
one turned out to be wrong or incomplete, and **fifteen** conversions plus six
platform-guard defects were needed in total, arriving over seven build rounds. The
enumeration is in `production.md`. The prediction that "the only way to enumerate them
is a Windows build" was exactly right; the count was not.

**The transferable lesson:** the mechanical size of a refactor is a poor guide to its
risk. This one touched 685 files and was safe; it also introduced a Windows regression
in a file that never appears in the diff as "interesting", on a platform that cannot be
built. The risk sits at the *platform boundary*, and its magnitude is invisible from
the development machine.

See [`string.md`](string.md) for the encoding decision itself, the alternative
(`u16string`) that was considered and rejected, and the boundary discipline that has
to accompany either choice.

---

## Part 2 — The pointer and ownership question

### 2.1 The situation is not "everything is a raw pointer"

Smart pointers are already used substantially: **~700 uses** of `unique_ptr`/`shared_ptr`/`weak_ptr`, concentrated in `Image`, `RenderSkia`, `Control`, and `Utils`.

What exists instead of a single model is **four ownership mechanisms coexisting in one tree**:

| Relationship | How it is expressed | Evidence |
|---|---|---|
| Window → root Box | Non-owning `BoxPtr` (raw + weak flag), manual `delete` | `include/dui/Core/Window.h:1085`; `src/Core/Window.cpp:472-477` |
| Box → children | Owning `std::vector<Control*>` + a runtime `bool` | `include/dui/Core/Box.h:242,249` |
| Control → sub-control (some) | Owning `std::unique_ptr` | `include/dui/Box/ScrollBox.h:308,311` |
| Control → sub-control (some) | **Non-owning weak handle, but used to `delete`** | `include/dui/Control/Combo.h:414,418,422` + `src/Control/Combo.cpp:369` |

That last row is the dangerous one: `Combo`'s sub-controls are declared as weak `ControlPtr`, yet destroyed via `delete m_pXxxControl.get()`. Ownership representation and ownership reality have already drifted apart.

### 2.2 The library already has a weak-reference system

`ControlPtrT`, `WeakFlag`, `WeakCallback`, `UiBind`, and `EventArgs::m_senderFlag` together provide **exactly what a smart pointer would provide for dangling detection** — a handle that becomes null when the target dies. `ControlPtr` does it with a raw pointer plus a `weak_ptr` expiry check, i.e. without a control block or atomic traffic.

`Window` is already fully converted to this model (`Window.h:1059-1085`), and `AnimationManager` uses it as standard practice (`AnimationManager.cpp:45,81,114`):

```cpp
ControlPtr pControl(m_pControl);
AnimationPlayCallback cb = [pControl](int32_t v) {   // weak handle captured by value
    if (pControl != nullptr) { ... }                 // null after destruction
};
```

**Consequence: introducing `unique_ptr` into the tree adds nothing where dangling detection is concerned. That problem is already solved.** What `unique_ptr` would add is a *compile-time single-owner guarantee* — a different and narrower benefit.

### 2.3 Two hard obstacles to a whole-tree conversion

**Hard obstacle 1 — the layout API's currency is `const std::vector<Control*>&`.**

`include/dui/Layout/Layout.h:110,117`:

```cpp
virtual UiSize64 ArrangeChildren(const std::vector<Control*>& items, UiRect rc, bool bEstimateOnly = false);
virtual UiSize64 EstimateLayoutSize(const std::vector<Control*>& items, UiSize szAvailable);
```

This is a public virtual API with 7+ public subclasses intended for user extension. The pattern appears **106 times across 39 files**. If `Box::m_items` becomes `vector<unique_ptr<Control>>`, there is no way to hand the layout layer a `vector<Control*>`. The only options are (a) change 30+ public virtual signatures, or (b) maintain a parallel raw-pointer view — which reintroduces two bookkeeping structures and forfeits precisely the guarantee `unique_ptr` was supposed to provide.

**Hard obstacle 2 — `m_bAutoDestroyChild` is a runtime switch.**

`include/dui/Core/Box.h:249`, checked at `src/Core/Box.cpp:24,507,526`. Its meaning is "should removing this child also destroy it?" Reparenting depends on it, and reparenting is a first-class, high-frequency operation — six independent implementations (`src/Core/FullscreenBox.cpp:115-118`, `include/dui/Core/ControlDragable.h:742-744`, `CheckCombo.cpp:322-324`, `ComboButton.cpp:403`, `Menu.cpp:861`, `Combo.cpp:271`), all following the same manual protocol:

```cpp
bool b = pOldParent->IsAutoDestroyChild();
pOldParent->SetAutoDestroyChild(false);
pOldParent->RemoveItem(control);       // detach without destroying
pOldParent->SetAutoDestroyChild(b);
pNewParent->AddItem(control);          // re-adopt
```

This is a hand-rolled `unique_ptr::release()` + re-adopt dance. `unique_ptr` cannot express it, because "do I own this?" here is decided at runtime, not by type.

### 2.4 Why smart pointers would not fix the known use-after-free

The known defect (`src/Core/EventArgs.cpp:242-261`, documented as P0-2 in `production.md`) is a **timing** problem, not an ownership problem. `unique_ptr` changes *who* deletes, not *when*. When a callback calls `parent->RemoveItem(this)`, the `EventSource` is still destroyed synchronously in the middle of its own dispatch loop, and line 256 still reads freed memory.

Preventing it would require `shared_ptr` + `enable_shared_from_this`, taking a strong reference at the dispatch entry point. That costs a heap control block and atomic traffic per control, and — worse — **makes destruction asynchronous**, which breaks the six reparent paths above that rely on `RemoveItem` destroying immediately. That is trading one bug for a batch of lifecycle changes. Net negative.

The actual fix is about three lines: snapshot the size and the callback into locals before invoking, and never touch `this` after the callback returns.

### 2.5 What is *not* an obstacle (ruled out)

- **Pointer stability.** `unique_ptr` moves pointers, not objects. `(size_t)this` as a callback ID (`src/Box/XmlBox.cpp:20,110`, `src/Control/ListCtrlHeaderItem.cpp:423`) and `std::unordered_map<Control*, UiRect>` (`include/dui/Layout/GridLayout.h:146`) all remain valid. **But this also means those sites need zero changes, i.e. zero benefit.**
- **Performance.** A control already pays one heap allocation for its `WeakFlag` (`include/dui/Callback.h:96-103`). `unique_ptr` itself is free. (`shared_ptr` would not be.)
- **The code generator.** `tools/xml_to_code` emits `ui::Create<T>(...)` / `ui::Attach(...)` — of 9,436 generated lines, only **2** contain a raw `new`. If the `ui::Create`/`ui::Attach` signatures were preserved, the generator changes **not at all**.

### 2.6 Recommendation on pointers

**Do not convert the control tree.** The cost concentrates in exactly the places that yield no benefit (the layout layer's 106 `vector<Control*>` sites, the 5,391 `ui::Create`/`ui::Attach` call sites in examples), while the benefit is confined to places that can be fixed individually.

Instead, in priority order:

1. **Fix the dispatch UAF** (~3 lines, `EventArgs.cpp:246-260`). Highest value per line in the entire document.
2. **Implement the deferred-deletion queue the comments already promise.** `src/Core/Window.cpp:498-500` says "We'll delay the cleanup" and then deletes immediately at line 505. No deferred-deletion facility exists anywhere in the library. A `std::vector<Control*> m_pendingDelete` flushed at the top of the message loop kills the entire class of "destroyed during dispatch" reentrancy bugs — **a class that neither `unique_ptr` nor `shared_ptr` addresses.** The primitive already exists (`FrameworkThread::PostTask`, `FrameworkThread.h:95`). ~50 lines.
3. **Convert the genuinely exclusive sub-control members** to `unique_ptr` — roughly 15-20 members across `ComboButton`, `RichEdit×3`, `Split`, `TabCtrl`, `ListCtrlIcon`, `Shadow::m_pShadowBox`. Does not touch `Box::m_items`, the layout API, or any public signature. **Requires manually confirming each member is never also `AddItem`ed into a parent's `m_items`** — `Combo.cpp:369` proves that ambiguity is real.
4. **Name the ownership protocol.** Add `Control* Box::ReleaseItem(Control*)` encapsulating the four-line dance, and add a duplicate-parent assertion in `Box::DoAddItemAt`. Replaces six duplicated sites with one well-named operation. ~20 lines.
5. **Restore asserts and add sanitizers** (see `production.md` P0-1/P1-2). Until this is done, any ownership-model change is unverifiable — `ControlPtr::operator->` (`include/dui/Core/ControlPtrT.h:94-99`) is a raw dereference whenever `ASSERT` is compiled out.

**All five are done as of 2026-09-14** — item 3 in the targeted form described above, not
the whole-tree conversion. The order was right, and item 5 turned out to be the
load-bearing one: reviving the asserts found a live abort (P0-8 in `production.md`), and
the sanitizer configuration turned out to have been unsound — see the false-positive
section there, which is the most useful thing this pass produced for whoever runs the
sanitizers next.

**One correction to this section.** It presents the deferred-deletion queue (item 2) as
the answer to the "destroyed during dispatch" bug class. That held. What it did not
anticipate is that the class had a second member, in the same file, with nothing to do
with ownership: `EventSource::operator()` threw `std::out_of_range` when a callback
removed a callback mid-dispatch. Fixed — P0-6.

---

## Part 3 — CMake

### 3.1 Score: 5.5 / 10

Roughly "2018-2020 era modern CMake," not current toolchain practice.

| Dimension | Score | Basis |
|---|---|---|
| Target-based configuration | 6/10 | `include`/`definitions`/`compile_options` fully keyword-qualified; but **16 legacy `target_link_libraries()` with no keyword**, 7 of them on the `dui` library itself |
| Source collection | ~~4/10~~ → **fixed** | It was 88% `file(GLOB)` with `CONFIGURE_DEPENDS`, which is unreliable on VS/Xcode — and this repo leans on multi-config generators. Both the library and every example now list their sources explicitly; the acceptance test was that the compiled object set and the compile-command stream were unchanged, not that it built |
| Dependency management | 2/10 | **Zero `FetchContent`, zero `ExternalProject`.** Skia, CEF, gn, WebView2 all fetched via `file(DOWNLOAD)` plus 200 lines of hand-rolled retry/checksum/shell-extract |
| Packaging and export | **7/10** | `install(EXPORT)` + `Config.cmake.in` + `write_basic_package_version_file` + `GNUInstallDirs` — the strongest area |
| Build quality | 5/10 | Real investment in multi-config and dual Skia builds; but no `/W4` on MSVC, no `-Werror`, no `CMAKE_EXPORT_COMPILE_COMMANDS`, `CXX_EXTENSIONS` left on, warning flags entirely absent on the Windows and Linux example paths |
| Tooling | 3/10 | **No `.clang-format`, no `.clang-tidy`, no `.editorconfig`**, no `CMakePresets.json`; `.gitignore` is genuinely good |

### 3.2 The biggest obstacle is structural, not neglect

**Skia is not a CMake project.** `third_party/skia` is a gn/ninja build (`BUILD.gn`) and cannot be expressed as a CMake target. This one fact generates most of the remaining legacy patterns:

- `dui_skia_libs` is assembled from bare archive paths by hand (`cmake/dui_deps.cmake:372-392`), with its own `$<CONFIG>` branching, because no target can carry `IMPORTED_LOCATION`.
- Build ordering must be stitched with **12 `add_dependencies` calls**, because `dui_skia` is an `add_custom_target`, not a linkable library.
- `target_link_directories` is forced into use, because the link items are raw path strings.
- Skia must be **built twice** for multi-config (`dui_deps.cmake:266-332`), which is why a hand-rolled `DUI_MULTI_CONFIG` boolean exists at all.
- `ExternalProject` could wrap gn/ninja in a target, but it remains a black box outside CMake's model; `FetchContent` does not apply at all.

**Conclusion: "fully target-based" is mathematically unreachable while Skia stays a gn project.** This is a hard constraint, not a backlog item.

The second obstacle is CEF: the repo `add_subdirectory`s the CEF distribution's own CMake (`third_party/CMakeLists.txt:40`), so CEF's older idioms leak into dui's own files — `SET_EXECUTABLE_TARGET_PROPERTIES`, directory-level `CMAKE_RUNTIME_OUTPUT_DIRECTORY` rewrites, and an `unset(CMAKE_OSX_DEPLOYMENT_TARGET)`.

The third was architectural: **55 examples each called `project()` and re-included the same modules** (`examples/basic/CMakeLists.txt:16-18`). That was the structural reason directory-level commands survived — `add_link_options` at `cmake/dui_common.cmake:54` ran once per example, because there was no shared target to hang it on.

**That is now fixed**, and it is the item this document would have been wrong to leave alone. Applications link `dui::app`, an INTERFACE target carrying the include directories, the C++ standard, the compile definitions and options, the link options and directories, and the libraries; the two remaining directory-scoped commands (both Linux/FreeBSD-only) moved onto targets. What could not move is handled by `dui_finalize_app(target)`, a function, because `RUNTIME_OUTPUT_DIRECTORY` is not an interface property, a POST_BUILD step has no interface form, and `add_dependencies` has to name a concrete target. **So the sentence above — "fully target-based is unreachable" — was right about Skia and wrong about the examples.** Skia stays a gn/ninja black box; the 55-project structure did not have to.

### 3.3 Cheap wins, in order

**All seven are done as of 2026-09-14** (items 1–3, 5–7 on that date; item 4 — presets and
`COMPILE_COMMANDS` — slightly earlier). Item 2 is the one whose prediction above came true
most literally: the aliases made the application layer expressible as INTERFACE properties,
which is now what `cmake/dui_app.cmake` does.

1. **`add_library(dui::dui ALIAS dui)`** plus aliases for `dui_entry` and the dependency targets. The `dui::` namespace currently exists **only in the install export** (`cmake/dui_install.cmake:104`), so the documented `target_link_libraries(app PRIVATE dui::dui)` does not work inside the source tree. One line, and it makes the whole `dui_bin*.cmake` layer expressible as `INTERFACE` properties on a real target.
2. **`target_compile_features(dui PUBLIC cxx_std_20)`** — fixes the install-package defect noted in §1.1.
3. **Keyword-qualify the 7 legacy `target_link_libraries` on `dui`** (`src/CMakeLists.txt:333,336,346,355`; `cmake/dui_install.cmake:57,62,66`). These currently behave as `PUBLIC`, so system libraries and `-framework` strings propagate to every consumer.
4. **`CMakePresets.json`** — the repo has 15 options, 4 example modes, and multi-platform/multi-generator combinations, all currently documented only in prose (`Progress.md:131-143`).
5. **`CMAKE_EXPORT_COMPILE_COMMANDS ON`** and a `.clang-format` — zero cost, immediate tooling payoff.
6. **`/W4` and `/permissive-` on MSVC** — currently the MSVC path sets only `/utf-8` and `/MP`.
7. **Fix the duplicated `dui-png` / `png_static` naming** — ✅ **fixed 2026-09-14.** The
   library is referenced by two names in-tree, and that is now deliberate rather than
   drift: `png_static` is libpng's own target name and stays it (`tests/CMakeLists.txt`
   and the install list refer to it), while `dui-png` is the alias dui's own code links
   and the name the target exports, via `EXPORT_NAME`. What is gone is the leak into
   the published interface: the installed package used to export `dui::png_static`, the
   one name in the `dui::` namespace that meant nothing to a consumer.

---

## Part 4 — Recommended sequence

Ordered by value per unit of risk. Note that the top items are small.

**As of this revision, Tier 1 is complete and Tier 2 is half done.** The prediction this
paragraph used to make — that the small items at the top have the best ratio of value to
effort — held: the Tier 1 items took hours, each was verified by building, and item 1
paid for itself immediately by exposing a defect (P0-8 in `production.md`) that no amount
of reading had found. What did *not* hold is the assumption that the list is the whole
job: five further defects surfaced from touching this code, none of them on this list.

**Tier 1 — small, safe, real payoff** — complete

| # | Change | Scale | State |
|---|---|---|---|
| 1 | Fix the dispatch UAF (`EventArgs.cpp:246-260`) | ~3 lines | ✅ `2812c547` |
| 2 | Remove `-fno-threadsafe-statics`, or convert ~10 statics to `std::call_once` | 1 line or ~10 sites | ✅ flag removed |
| 3 | `add_library(dui::dui ALIAS dui)` + `target_compile_features(... cxx_std_20)` | ~3 lines | ✅ both targets |
| 4 | `CMakePresets.json` + `CMAKE_EXPORT_COMPILE_COMMANDS` + `.clang-format` | new files | ✅ + `.editorconfig` |
| 5 | Deferred-deletion queue (`Window.cpp:498-500`) | ~50 lines | ✅ flush at message, paint and teardown entry |
| 6 | `Control* Box::ReleaseItem(Control*)` + duplicate-parent assert | ~20 lines | ✅ 4 of the 6 cited sites; 2 are the borrow half, not the dance |

**Tier 2 — mechanical, reviewable in batches**

7. ☐ `typedef` → `using` (370), `std::size` (3), value-type `constexpr` accessors.
   **Not started.** Still the cheapest remaining mechanical item, and the least urgent:
   it changes no behaviour, and this pass found behaviour to fix.
8. ☐ C-style casts → `static_cast` (~1,200, numeric first). **Not started.**
9. ☐ `inline constexpr` for constant macros (~140 colours, 77 in `Macros_Windows.h`).
   **Not started.**
10. ✅ Keyword-qualify the remaining legacy `target_link_libraries`; add `/W4`.
    **One correction to this item's premise:** for a *static* library, `PRIVATE` does not
    remove system libraries from consumers' link lines — CMake re-emits them as
    `$<LINK_ONLY:…>` because the symbols still have to resolve. What it stops is the
    interface leaking usage requirements. The comment in `src/CMakeLists.txt` says that,
    not this line. `/permissive-` was deliberately *not* added: `/std:c++20` already
    implies it on current MSVC, so it would be unverifiable noise on older toolchains.
11. ◐ Convert the ~15-20 exclusive sub-control members to `unique_ptr` (§2.6 item 3).
    Six converted, all in `RichEdit`'s three platform variants (`m_pFocusedImage`,
    `m_pTextData` × 2, `m_pControlDropTarget` on Windows); thirteen examined and
    **deliberately left alone**, each with recorded evidence. Two would have been
    double-frees: `Shadow::m_pShadowBox` is owned by the Window, not the Shadow
    (`Window::ClearWindow` deletes it, and `m_shadow.reset()` runs first), and
    `SplitTemplate::m_pLeftTop`/`m_pRightBottom` are the sibling controls on either side
    of the split bar, not sub-controls at all. The rest are `AddItem`ed into a parent —
    the ambiguity §2.6 warns about, confirmed rather than assumed. Note that the
    sub-controls this item named for `ListCtrlIcon` and `TabCtrl` turned out not to be
    candidates at all: every one of them is `AddItem`ed, so that scope yields zero
    conversions.

**Tier 3 — large, needs a decision and a budget**

12. **The string model unification.** ~~The largest single item.~~ **Executed** — see
    §1.6. What remains is the Windows half, which is unverified and needs a Windows
    build to finish. **Do not merge branch `utf8string` until that passes.**
13. ☐ Concepts/`requires` for the template layer (390 `template<`, currently
    constrained by 6 `enable_if`s). Note that `std::string_view` and `constexpr`
    literals — the two things §1.3 said the string model was blocking — are now
    unblocked as a side effect of item 12.
14. ☐ Ranges and `<format>` adoption — genuinely optional; do it incrementally as
    files are touched.

**Not recommended:** a whole-tree `unique_ptr` conversion. See §2.6 for why the cost lands where the benefit is not, and §2.4 for why it would not fix the bug motivating it.

---

## Appendix: methodology and corrections

Three parallel surveys — C++ language census, pointer/ownership analysis, CMake census — over the tree at `a353844b`, followed by direct verification of each load-bearing claim. Verified by reading source:

- `Layout.h:110,117` `std::vector<Control*>` public virtual signatures (106 sites repo-wide)
- `Box.h:249` / `Box.cpp:24,507,526` `m_bAutoDestroyChild` runtime semantics
- `src/CMakeLists.txt:437` `-fno-threadsafe-statics`, and its presence in the generated `flags.make`
- Function-local `static` singletons at `GlobalManager.cpp:82`, `PerformanceUtil.cpp:30`, `WebView2Manager.cpp:40`
- `-std=gnu++20` and the absence of `_DEBUG` in the real build flags

All counts are `grep`-based token or line counts and are order-of-magnitude accurate, not exact; where a number is load-bearing it is cited with its file and line. Comment text was manually excluded from counts in ambiguous categories (`concept`, `requires`, `final`).

### Corrections made in this revision

Recorded so they are not re-derived:

1. **`u8string` was offered as a unification target (§1.3).** Wrong — it is not
   type-safe, not interoperable, and not adopted. Corrected in place, with evidence.
2. **The string migration was estimated as a large, delicate refactor.** It is two
   lines of definition plus platform fallout. Corrected in §1.6 with measurements.
3. **The Windows limitation was described as a survey limitation.** It is a standing
   condition of the project, with demonstrated consequences — see the Windows note in
   [`production.md`](production.md).

### On the counts in this document

Several figures here come from surveys rather than from the compiler, and one was
caught being wrong in the other direction: an early count of platform conditionals
inside `_Windows`-suffixed files returned zero because of a shell-globbing error, not
because none existed. **Treat every count here as an order of magnitude, and re-measure
before acting on one.** The figures that were checked against the compiler — the
`-fno-threadsafe-statics` flag, the `-std=` level, the absence of `_DEBUG` — are the
ones to trust.

Known limitation, lifted 2026-09-14: **a Windows host was reached and the branch was
built on it** — see the measurement in `production.md`. It no longer stands as a
caveat on §1.6: the Windows half of the string migration is compiled and its tests
pass, and the fifteen boundary conversions it needed are recorded there.
