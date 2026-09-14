# dui String Model Assessment

**Status:** assessment complete, no remediation started
**Baseline:** branch `production`, commit `a353844b`
**Date:** 2026-09-13
**Scope:** the `DString` platform split, where conversion costs actually fall, and the options for changing it

## Verdict

The question that prompted this was: *"there is no loss on Windows or Unix today, but unifying would cost us at the Windows boundary — what should we do?"*

**The premise does not hold. There is no loss on Windows. Unix is already paying.**

| | Windows | macOS / Linux |
|---|---|---|
| Plain text drawing | zero conversion | zero conversion |
| Drawing (vertical / justified / word-spaced) | **zero** | **converts per draw** into a 16 KB buffer |
| RichEdit: every keystroke and paste | **zero** | **converts per keystroke** into a 32 KB buffer |
| Rich text parsing | zero | converts on text change |
| Win32 API calls | **zero** | — |
| All OS calls (files, clipboard, CEF) | **zero** | zero (natively UTF-8) |

The current design is **Windows-native, with Unix taxed**. So the real question is not "will Windows lose something" but **"which platform do we want to be native for?"**

Two further findings change the shape of the decision:

1. **The dominant conversion cost is a fixable implementation bug, not an encoding choice.** Every conversion allocates a fixed 8192-element buffer and value-initialises it — 8 KB, 16 KB or 32 KB of `memset` per call, regardless of input length. Fixing that shrinks the whole issue.
2. **The Windows boundary is far smaller than it looks**: zero GDI text calls, zero DirectWrite text layout (all text goes through Skia on every platform), and only ~60-70 Win32 calls that carry a runtime string, almost all of them one-time or low-frequency.

Unifying on UTF-8 is the right destination, but the first two steps below are worth doing regardless of which encoding wins, and they are the ones with no architectural risk.

---

## Part 1 — The current model

### 1.1 `DString` is two different types

`include/dui/dui_string.h:59-97`:

| Platform | `DUI_UNICODE` | `DString` is | Encoding | `wchar_t` |
|---|---|---|---|---|
| Windows | defined | `std::wstring` | UTF-16 | 2 bytes |
| macOS | **not defined** | **`std::string`** | **UTF-8** | 4 bytes |
| Linux / FreeBSD | **not defined** | **`std::string`** | **UTF-8** | 4 bytes |

`DUI_UNICODE` is set only when `UNICODE`/`_UNICODE` are, which `src/CMakeLists.txt:383-385` does **only on Windows**. The consequence is that `DStringW` (= `std::wstring`) is UTF-16 on Windows but **UTF-32** on Unix, and the two types are interchangeable on Windows but not on Unix.

### 1.2 `DUI_T` exists to paper over the split

`include/dui/dui_string.h:13-21` — expands to `L"..."` on Windows, `"..."` elsewhere. **3,485 uses across 182 files**, plus the 110 `DUI_CTR_*` macros in `dui_defs.h:12+` that each wrap one.

This is the root of the modernization cost described in `modern.md`: because the literal type differs per platform, it must be a macro, and because it is a macro, literals are invisible to refactoring tools and cannot be `constexpr` or `string_view`.

### 1.3 There is a third string type: `UiString`

`include/dui/Core/UiString.h` (236 lines) defines `UiStringT<T>` — a hand-rolled null-terminated string with **raw `new[]`/`delete[]`** and, verified in this assessment, **no move constructor and no move assignment**. Its own class comment concedes "due to more string copies, the performance is weaker".

It is not a fringe type: it is used in **50 files**, including `Control.h`, `Combo.h`, `CheckBox.h`, `CefControl.h`, and `Control.cpp`.

So the model is not "one string type with a platform split" — it is **three representations** (`DString`, `UiString`, and the `std::wstring_view` used by the rich-text render interface), and the third is a modernization liability independent of the encoding question.

---

## Part 2 — Where conversion actually happens

### 2.1 The conversion layer already exists

`src/Utils/StringConvert.{h,cpp}` provides `TToUTF8`, `UTF8ToT`, `TToWString`, `WStringToT`, `TToLocal`, `LocalToT`, plus the raw UTF-8/16/32 converters. There are **~210 call sites across 47 files**.

Critically, several of these are **identity functions on Windows today**:

```cpp
const DStringW& StringConvert::TToWString(const std::wstring& str) { return str; }   // :260-263
const DString&  StringConvert::WStringToT(const std::wstring& wstr) { return wstr; } // :271-274, DUI_UNICODE branch
```

So the boundary is already *encapsulated* — at the type level, not the call level. Unifying to UTF-8 would turn ~106 of these from free into real conversions, **without changing a single line of calling code**. That is convenient, and also the main risk: the cost appears with no diff.

### 2.2 The typed costs, by frequency

| Frequency | Location | Count | Notes |
|---|---|---|---|
| **Per draw** | `HorizontalDrawText.cpp:24-39`, `VerticalDrawText.cpp:27-42` | 2 functions, 4 call sites | Unix converts UTF-8→UTF-16 into a 16 KB buffer, plus two whole-string copies. Triggered by `TEXT_VERTICAL`, `TEXT_HJUSTIFY`, or `fWordSpacing > 0` |
| **Per keystroke** | `RichEdit_MacOS.cpp:1281`, `RichEdit_Native.cpp` | ~2 | UTF-8→UTF-32, 32 KB buffer, on every `ReplaceSel` |
| Per window operation | `SetWindowText`×3, `CreateWindowEx`×2, `RegisterClassEx`×2, `UnregisterClassW`×2, `GetClassInfoExW`×2, font creation | ~14 | One-time each |
| Per text change | `WindowBuilder.cpp:1382,1390` (rich text) | 2 | Cached via `RichTextImpl::m_textData`; invalidated on text or DPI change |
| Low frequency | file dialogs, clipboard, tray, registry, COM/WebView2, RichEdit font names | **~90** | Never in a hot path |

### 2.3 The dominant real cost is the fixed buffer, not the encoding

`src/Utils/StringConvert.cpp` has **six** conversions that each do this:

```cpp
// :92-94  (UTF8ToUTF32)
std::vector<DUTF32Char> data;
data.resize(8192);        // 32 KB allocation + 32 KB value-initialisation
DUTF32Char* output = &data[0];
```

| Line | Function | Element | Cost per call |
|---|---|---|---|
| `:16` | `UTF8ToUTF16` | 2 B | 16 KB |
| `:55` | `UTF16ToUTF8` | 1 B | 8 KB |
| `:93` | `UTF8ToUTF32` | 4 B | **32 KB** |
| `:123` | `UTF32ToUTF8` | 1 B | 8 KB |
| `:156` | `UTF16ToUTF32` | 4 B | **32 KB** |
| `:199` | `UTF32ToWString` | 2 B | 16 KB |

On macOS the hot path is `UTF8ToWString` → `UTF8ToUTF32` → **32 KB allocate + 32 KB `memset` per call**, independent of input length. Typical UI strings are under 100 characters.

Sizing the buffer from the input instead (UTF-8→UTF-16 worst case 2×, UTF-16→UTF-8 1.5×, UTF-32→UTF-8 4×, plus one) removes almost all of this. This is worth doing **whatever encoding is chosen**.

### 2.4 Malformed input silently destroys the whole string

Six conversions do this on error (`:33, :71, :112, :142, :172, :214`):

```cpp
if (result == sourceIllegal || result == sourceExhausted) {
    utf16.clear();   // the entire converted prefix is discarded
    break;
}
```

A single bad byte from a file path, the clipboard, or a network string makes the entire text empty — no replacement character, no error flag, no log.

Note the correct interpretation of the two error codes (this was checked against the vendored source, see the Appendix): `sourceIllegal` means "malformed source", `sourceExhausted` means "partial character at end of source" (`third_party/convert_utf/ConvertUTF.h:149-152`). **Neither is the normal termination path** — a successful conversion returns `conversionOK`. So this is a genuine error branch, not a bug that empties every conversion. The defect is the *handling*, not the *condition*.

---

## Part 3 — The Windows boundary, measured

| Fact | Value |
|---|---|
| GDI text APIs (`DrawTextW`, `ExtTextOutW`, `TextOutW`) | **0** — all text is drawn by Skia on every platform |
| DirectWrite text layout (`IDWriteTextLayout`, `CreateTextLayout`) | **0** — the only reference is `FontMgr_Skia.cpp:198` `SkFontMgr_New_DirectWrite()` for font enumeration |
| Win32 W-series call sites in `src/` | ~96 |
| …of which carry a runtime string | ~60-70 |
| …of which are per-frame | **0** |
| …of which are per-keystroke | ~3 (all in RichEdit limit-char paths, which should be refactored rather than converted) |
| …one-time (window creation) | ~14 |
| …low frequency (dialogs, files, clipboard, tray, registry, COM) | ~90 |

A trap worth recording: ranking Win32 calls by raw frequency badly overstates the boundary. The top four — `SendMessage`, `SetMenuItemInfo`, `GetPropW`, `SetPropW`, 64 calls between them — carry a runtime string in only **5** of them. `GetPropW`/`SetPropW` pass compile-time literals such as `L"DuiWindow"` (`NativeWindow_Windows.cpp:2031-2032`) and need no change at all.

### 3.1 What unifying would actually cost on Windows

| Category | Count |
|---|---|
| Existing conversion points that become real (no code change) | **~106** — 91 in 14 Windows-only files, plus 13 in `FilePath.cpp` and 2 in `FilePathUtil.cpp` |
| New conversion points required | **~10-15** |
| Functions needing refactor rather than a conversion | 2 (the Skia encoding selection) |

The ~90 low-frequency sites are already wrapped with a consistent idiom (`StringConvert::TToWString(x).c_str()`, 18 literal occurrences), concentrated in 14 files:

| File | `StringConvert::` sites |
|---|---|
| `src/WebView2/WebView2ControlImpl.cpp` | 27 |
| `src/Control/RichEdit_Windows.cpp` | 22 |
| `src/Utils/DiskUtils_Windows.cpp` | 7 |
| `src/Core/NativeWindow_Windows.cpp` | 7 |
| `src/Utils/TrayIcon_Windows.cpp` | 4 |
| `src/Utils/FileDialog_Windows.cpp` | 4 |
| `src/Core/ControlDropTargetImpl_Windows.cpp` | 4 |
| others (7 files) | ~16 |

### 3.2 The path layer is already paying, and would not get worse

`include/dui/Utils/FilePath.h` wraps `std::filesystem::path`, whose `value_type` is `wchar_t` on Windows and `char` on Unix. It therefore already converts on both sides:

```cpp
src/Utils/FilePath.cpp:12   m_filePath(StringConvert::UTF8ToWString(filePath))   // Windows
src/Utils/FilePath.cpp:30   m_filePath(StringConvert::WStringToUTF8(filePath))   // Unix
src/Utils/FilePath.cpp:192  return StringConvert::UTF8ToWString(m_filePath.native());
src/Utils/FilePath.cpp:207  return StringConvert::WStringToUTF8(m_filePath.native());
```

UTF-8 as the internal representation makes this layer **more** natural, not less. New conversion points required here: **0**.

### 3.3 The reverse direction: Unix already needs UTF-16

CEF is UTF-16 on all platforms, and the conversions already run on macOS with no platform guard (`src/CEFControl/CefControl.cpp:181,197,205,226,227,251,252`; `internal/CefClientApp.cpp:101,105,106`). CEF has absorbed its own boundary, so unifying is **neutral** for it.

---

## Part 4 — The render path

This is the only genuinely hot conversion path, and it should not be fixed by inserting a conversion.

`src/RenderSkia/HorizontalDrawText.cpp:24-39`:

```cpp
UTF16String HorizontalDrawText::GetDrawStringUTF16(const DString& strText, bool bSingleLineMode) const
{
    DString text = strText;                                    // ① whole-string copy
    StringUtil::ReplaceAll(DUI_T("\r\n"), DUI_T("\n"), text);
    ...
#if defined DUI_UNICODE && defined WCHAR_T_IS_UTF16
    return text;                                               // Windows: free
#else
    std::string textUTF8 = StringConvert::TToUTF8(text);       // ② identity on Unix, another copy
    return StringConvert::UTF8ToUTF16(textUTF8.c_str(), textUTF8.size());   // ③ 16 KB buffer
#endif
}
```

`VerticalDrawText.cpp:27-42` is identical, with call sites at `:407` (measure) and `:513` (draw).

**The right fix is to use Skia's native `kUTF8`**, not to convert. Skia already accepts it (`SkTextEncoding::kUTF8` is in use at `HorizontalDrawText.cpp:717`, `DrawRichText.cpp:529,1114`), and the dispatch logic already exists:

```cpp
// src/RenderSkia/Render_Skia.cpp:2128-2147
SkTextEncoding Render_Skia::GetTextEncoding() const
{
    constexpr const size_t nValueLen = sizeof(DString::value_type);
    if constexpr (nValueLen == 1) { return SkTextEncoding::kUTF8; }
    else if constexpr (nValueLen == 2) { return SkTextEncoding::kUTF16; }
    else if constexpr (nValueLen == 4) { return SkTextEncoding::kUTF32; }
```

The main non-rich-text path already does this and is therefore **zero-conversion on both platforms** (`Render_Skia.cpp:1600-1605` passes the raw pointer and byte count).

The only obstacle is `CalculateTextCharBounds`, which iterates `DUTF16Char` (`HorizontalDrawText.cpp:74`). It needs rewriting — and it is **already buggy on both platforms**:

```cpp
for (DUTF16Char ch : textUTF16) {                     // :74  one UTF-16 unit at a time
    SkScalar fTextWidth = pSkFont->measureText(&ch, sizeof(DUTF16Char), kUTF16, ...);
    if ((horizontalChar.bounds.width() <= 0) || (horizontalChar.bounds.height() <= 0)) {
        ch = 'a';                                     // :92  zero-width → measure as 'a'
```

A lone high surrogate measures as zero width, so every non-BMP character (emoji, rare CJK) is split into two cells, each measured as the width of the letter `a`, and drawn in two separate `drawSimpleText` calls (`:731`). This is wrong on Windows and Unix alike.

The rich-text path by contrast gets this **right**, via `glyphCharCount` from `SkTextBox::TextToGlyphs`, which handles surrogates correctly (`DrawRichText.cpp:394-404`, `SkTextBox.cpp:895-932`). That is the model to copy.

---

## Part 5 — Encoding-semantics defects that exist today

These are independent of which encoding is chosen; they are consequences of `.size()`, `[]` and `substr()` meaning "UTF-16 units" on one platform and "UTF-8 bytes" on the other — **neither of which is "characters"**.

| Severity | Location | Platform | Symptom |
|---|---|---|---|
| High | `PropertyGrid.cpp:1182,1205,1241` | Unix | Password masking emits one `*` **per byte** — a 5-character Chinese password shows 15 asterisks |
| High | `RichEditData.cpp:685,948-953,2747-2759` + `RichEdit_MacOS.cpp:936-952` | both | "Character limit" means UTF-16 units on Windows (splits surrogate pairs) and UTF-32 code points on Unix — the same limit behaves differently per platform |
| High | `CefControlOffScreen.cpp:847,862-863,992-1000` ← `NativeWindow_MacOS.mm:2415,2434` | macOS | UTF-32 code points are assigned to CEF's `char16_t`; `CefRange` indices are code points where CEF expects UTF-16 units |
| High | `HorizontalDrawText.cpp:74,85-98,731`; `VerticalDrawText.cpp:107,120-133,827-833` | both | Per-UTF-16-unit measure and draw; emoji split and mis-measured (see Part 4) |
| Medium | `ToolTip_Windows.cpp:119-120` | Windows | Truncation by UTF-16 unit splits surrogate pairs |
| Medium | `RichEditData.cpp:2761-2873` `FindRichText` | Windows | `iswalnum` applied to half a surrogate pair; case-folding is ASCII-only |
| Medium | `StringUtil.cpp:668-708` `StringCompare` | both | `strcmp` byte order == code point order; `wcscmp` UTF-16 unit order is **not**. The same data sorts differently per platform |
| Medium | `StringUtil.cpp:302-420` | both | Case conversion folds ASCII only; `Ä`→`ä` never happens on either platform |
| Low | `StringUtil.cpp:15-36` `StringTokenizeT`, `:501-550` `Split` | Unix | Byte-wise delimiter sets; a multi-byte delimiter would corrupt. All current callers use ASCII delimiters, so not currently triggered |

Two related notes: there is **no string hashing** anywhere in the library, and **no regex or wildcard matching** — so those surfaces carry no encoding risk.

### 5.1 There is no `SkTextBlob` cache

`SkTextBox.cpp:815-838` defines `snapshotTextBlob`, but it has **no call sites** in the drawing path. Skia's internal strike cache still applies, but the application layer re-runs layout every repaint (except for the rich-text `DrawRichTextCache`, which is keyed on the `wstring_view` data pointer, `DrawRichText.cpp:714-717`).

---

## Part 6 — Options

| | **A. Keep the split** | **B. Unify on UTF-8** | **C. Split storage, UTF-8 render interface** |
|---|---|---|---|
| Windows runtime | zero | conversions at low-frequency OS boundaries | **zero** |
| Unix runtime | converts on hot paths | **zero** | **zero** |
| 3,485 `DUI_T` | kept | **gone** | kept |
| `string_view` / `constexpr` literals | unusable | **usable** | unusable |
| Public API break | none | yes | none |
| Effort | none | large | moderate |

**Option C deserves to be considered a destination, not a compromise.** It keeps the platform-native storage but makes the render interface UTF-8 on all platforms (Part 4), which removes the Unix hot-path cost without touching the public API or the `DUI_T` surface. If the priority is runtime rather than tooling, C is a reasonable end state.

**Option D — unify on UTF-16 — should be rejected.** `char16_t` is not `wchar_t`, so Windows would *still* convert for every Win32 call, and Unix would convert for every OS call. It is worse than the status quo on both platforms.

**Recommendation: B, but staged, with C as its first step.** The tooling costs recorded in `modern.md` — 3,485 macro-wrapped literals invisible to refactoring tools, no `string_view`, no `constexpr` strings — are solvable *only* by unifying. The Windows cost, measured, is ~106 low-frequency boundary conversions plus ~10-15 new sites, not a rewrite.

---

## Part 7 — Recommended sequence

**Step 1 — do now, zero risk, needed by every option**

1. Size the conversion buffers from the input length instead of a fixed 8192 elements (6 sites in `StringConvert.cpp`). Removes 8-32 KB of allocation and `memset` per call.
2. Stop discarding the whole string on malformed input — use replacement characters and log.
3. Fix `UiString`: add move semantics, and reconsider whether a hand-rolled raw-pointer string is still justified now that `std::string`/`std::wstring` have SSO. It is used in 50 files including `Control.h`.

**Step 2 — fix the character-semantics defects (wrong on both platforms today)**

4. `HorizontalDrawText.cpp:74` / `VerticalDrawText.cpp:107` — measure and draw by code point, copying the correct approach already in `DrawRichText.cpp:394-404`.
5. `PropertyGrid.cpp:1182,1205,1241` password masking.
6. `RichEditData.cpp:2747-2759` `TruncateLimitText`.
7. `CefControlOffScreen.cpp:992-1000` — UTF-32 to `char16_t` on macOS.

**Step 3 — make the render path conversion-free on both platforms (this is Option C)**

8. Rewrite `CalculateTextCharBounds` to work by code point, and route `GetDrawStringUTF16` through Skia's `kUTF8` on Unix. Both platforms become zero-conversion on the hot path, with no API change.

**Step 4 — unify, if still desired**

9. Change `DString` to UTF-8 everywhere; add the ~10-15 new conversion points; publish as a versioned breaking change. By this stage the Windows cost has already been confined to low-frequency OS boundaries.

---

## Appendix: method and corrections

Two parallel surveys (Windows boundary surface; encoding-dependent operations plus conversion implementation) over the tree at `a353844b`, with the load-bearing claims verified directly in this session:

- `HorizontalDrawText.cpp:24-39` and `VerticalDrawText.cpp:27-42` — confirmed the platform split, and that Unix converts while Windows does not.
- `StringConvert.cpp:10-38` — confirmed the 8192-element buffer and the `clear()`-on-error behaviour.
- `StringConvert.cpp:260-263, 271-274` — confirmed the Windows identity overloads.
- `UiString.h` — confirmed the absence of move semantics and the raw `new[]`/`delete[]` (grepped for `UiStringT(UiStringT&&`, `operator=(UiStringT&&`, `std::move`, `swap`, `noexcept`: zero hits in 236 lines).

**One survey claim was checked and rejected.** It was reported that `sourceExhausted` is the normal termination condition of the conversion loop, and that consequently *every* conversion returns an empty string. Reading `third_party/convert_utf/ConvertUTF.cpp:567+` shows the success path returns `conversionOK` and exits the `while (source < sourceEnd)` loop normally; `sourceExhausted` is returned only when a multi-byte sequence is truncated at the end of the source (`ConvertUTF.h:149-152`). The claim is false, and the behaviour it described would have broken the library immediately. It is recorded here so that a future reader does not re-derive it. The genuine, narrower defect is the error *handling* described in §2.4.

Counts are `grep`-based and order-of-magnitude accurate. Where a number is load-bearing it is cited with file and line.

Known limitation: **no Windows host was available.** All Windows-specific claims come from reading code and build configuration, not from building or running.
