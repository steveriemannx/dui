/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef UI_RENDER_SKIA_SK_UTILS_H_
#define UI_RENDER_SKIA_SK_UTILS_H_

#include "dui/dui_config.h"

#include "SkiaHeaderBegin.h"
#include "include/core/SkTypes.h"
#include "include/core/SkFontTypes.h"
#include "SkiaHeaderEnd.h"

//The origin of this file's original source: skia/chrome_67/src/core/SkUtils.h
//Based on the original file, with modifications to be compatible with the latest version of the skia code (2023-06-25)
//The original file was removed in later versions.

namespace ui
{

#define kMaxBytesInUTF8Sequence     4

#ifdef SK_DEBUG
    int SkUTF8_LeadByteToCount(unsigned c);
#else
    #define SkUTF8_LeadByteToCount(c)   ((((0xE5 << 24) >> ((unsigned)c >> 4 << 1)) & 3) + 1)
#endif

inline int SkUTF8_CountUTF8Bytes(const char utf8[]) {
    SkASSERT(utf8);
    return SkUTF8_LeadByteToCount(*(const uint8_t*)utf8);
}

/**
 * @returns -1  iff invalid UTF8 byte,
 *           0  iff UTF8 continuation byte,
 *           1  iff ASCII byte,
 *           2  iff leading byte of 2-byte sequence,
 *           3  iff leading byte of 3-byte sequence, and
 *           4  iff leading byte of 4-byte sequence.
 *
 * I.e.: if return value > 0, then gives length of sequence.
*/
int SkUTF8_ByteType(uint8_t c);
bool SkUTF8_TypeIsValidLeadingByte(int type);
bool SkUTF8_ByteIsContinuation(uint8_t c);

int SkUTF8_CountUnichars(const char utf8[]);

/** These functions are safe: invalid sequences will return -1; */
int SkUTF8_CountUnichars(const void* utf8, size_t byteLength);
int SkUTF16_CountUnichars(const void* utf16, size_t byteLength);
int SkUTF32_CountUnichars(const void* utf32, size_t byteLength);

/** This function is safe: invalid UTF8 sequences will return -1
 *  When -1 is returned, ptr is unchanged.
 *  Precondition: *ptr < end;
 */
SkUnichar SkUTF8_NextUnicharWithError(const char** ptr, const char* end);

/** this version replaces invalid utf-8 sequences with code point U+FFFD. */
inline SkUnichar SkUTF8_NextUnichar(const char** ptr, const char* end) {
    SkUnichar val = SkUTF8_NextUnicharWithError(ptr, end);
    if (val < 0) {
        *ptr = end;
        return 0xFFFD;  // REPLACEMENT CHARACTER
    }
    return val;
}

SkUnichar   SkUTF8_ToUnichar(const char utf8[]);
SkUnichar   SkUTF8_NextUnichar(const char**);
SkUnichar   SkUTF8_PrevUnichar(const char**);

/** Return the number of bytes need to convert a unichar
    into a utf8 sequence. Will be 1..kMaxBytesInUTF8Sequence,
    or 0 if uni is illegal.
*/
size_t      SkUTF8_FromUnichar(SkUnichar uni, char utf8[] = nullptr);

///////////////////////////////////////////////////////////////////////////////

#define SkUTF16_IsHighSurrogate(c)  (((c) & 0xFC00) == 0xD800)
#define SkUTF16_IsLowSurrogate(c)   (((c) & 0xFC00) == 0xDC00)

int SkUTF16_CountUnichars(const uint16_t utf16[]);
// returns the current unichar and then moves past it (*p++)
SkUnichar SkUTF16_NextUnichar(const uint16_t**);
// this guy backs up to the previus unichar value, and returns it (*--p)
SkUnichar SkUTF16_PrevUnichar(const uint16_t**);
size_t SkUTF16_FromUnichar(SkUnichar uni, uint16_t utf16[] = nullptr);

size_t SkUTF16_ToUTF8(const uint16_t utf16[], int numberOf16BitValues,
                      char utf8[] = nullptr);

///////////////////////////////////////////////////////////////////////////////

/** Encoding-agnostic code point helpers.

    These dispatch on SkTextEncoding, so the same call works for UTF-8, UTF-16 and
    UTF-32 text. They started out as file-local helpers in SkTextBox.cpp; they live
    here now so that other render-path code can share them.

    Caution: SkUTF_CountUTFBytes is written for the library's own well-formed
    strings. It asserts on an unpaired high surrogate and reads one code unit past
    the end of a buffer ending in one. Use SkUTF_NextUnicharExtent for text that
    may be malformed - which includes anything coming from a user.
*/
SkUnichar SkUTF_NextUnichar(const void** ptr, SkTextEncoding textEncoding);
SkUnichar SkUTF_ToUnichar(const void* utf, SkTextEncoding textEncoding);
int       SkUTF_CountUTFBytes(const void* utf, SkTextEncoding textEncoding);

/** The Skia text encoding matching a character type of the given size.

    SkTextEncoding is one of kUTF8 / kUTF16 / kUTF32; which one applies is a
    compile-time property of the character type, so this is constexpr.

    Note: the character type must be passed explicitly rather than inferred from
    DString, because the library uses two character types with different widths
    on the same platform (for example on macOS, DString::value_type is 1 byte
    while DStringW::value_type is 4).
*/
template <typename TChar>
constexpr SkTextEncoding GetTextEncodingForCharType()
{
    if constexpr (sizeof(TChar) == 1) {
        return SkTextEncoding::kUTF8;
    }
    else if constexpr (sizeof(TChar) == 2) {
        return SkTextEncoding::kUTF16;
    }
    else if constexpr (sizeof(TChar) == 4) {
        return SkTextEncoding::kUTF32;
    }
    else {
#ifdef DUI_UNICODE
        return SkTextEncoding::kUTF16;
#else
        return SkTextEncoding::kUTF8;
#endif
    }
}

/** The Skia text encoding matching the platform-native DString type.
    This is the encoding to hand to Skia for any DString's bytes.
*/
constexpr SkTextEncoding GetDStringTextEncoding()
{
    return GetTextEncodingForCharType<DString::value_type>();
}

/** One decoded code point, together with the extent it occupies in the source.

    This is the safe counterpart of SkUTF_CountUTFBytes for *user* text: it never
    asserts and never reads past pEnd, so malformed input (a truncated sequence, or
    an unpaired surrogate, which a Windows DString can legitimately hold) ends the
    iteration instead of crashing or over-reading.
*/
struct SkUnicharExtent
{
    SkUnichar   unichar = 0;        //The decoded code point
    const char* pText   = nullptr;  //The encoded code point, inside the source buffer
    int32_t     nBytes  = 0;        //How many bytes of the source it occupies
};

/** Decode one code point at *ppText.

    Always advances by at least the minimum code unit and never past pEnd.
    A zero nBytes result means there was nothing left to decode.

    @param [in,out] ppText   On entry the current position; on return the next one.
    @param [in]     pEnd     One past the end of the source buffer.
    @param [in]     textEncoding  The encoding of the source buffer.
    @return The decoded code point and its extent. Invalid input yields U+FFFD
            with a one-code-unit extent, so the caller always makes progress.
*/
SkUnicharExtent SkUTF_NextUnicharExtent(const char** ppText, const char* pEnd,
                                        SkTextEncoding textEncoding);

inline bool SkUnichar_IsVariationSelector(SkUnichar uni) {
/*  The 'true' ranges are:
 *      0x180B  <= uni <=  0x180D
 *      0xFE00  <= uni <=  0xFE0F
 *      0xE0100 <= uni <= 0xE01EF
 */
    if (uni < 0x180B || uni > 0xE01EF) {
        return false;
    }
    if ((uni > 0x180D && uni < 0xFE00) || (uni > 0xFE0F && uni < 0xE0100)) {
        return false;
    }
    return true;
}

namespace SkHexadecimalDigits {
    extern const char gUpper[16];  // 0-9A-F
    extern const char gLower[16];  // 0-9a-f
}

}// namespace ui

#endif //UI_RENDER_SKIA_SK_UTILS_H_
