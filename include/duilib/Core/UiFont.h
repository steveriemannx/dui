#ifndef UI_CORE_UIFONT_H_
#define UI_CORE_UIFONT_H_

#include "duilib/Core/UiString.h"
#include "duilib/Core/SharePtr.h"
#include <cstdint>

namespace ui
{

/** Basic data structure of the font interface
*/
class DUILIB_API UiFont
{
public:
    UiFont() :
        m_fontSize(0),
        m_bBold(false),
        m_bUnderline(false),
        m_bItalic(false),
        m_bStrikeOut(false)
    {}

    /** Font name (if empty, no valid font name is included)
    */
    UiString m_fontName;

    /** Font size (unit: pixels); if 0, no font size information is included
    */
    int32_t m_fontSize;

    /** Whether it is bold
    */
    bool m_bBold;

    /** Font underline state
    */
    bool m_bUnderline;

    /** Font italic state
    */
    bool m_bItalic;

    /** Font strikethrough state
    */
    bool m_bStrikeOut;
};

#pragma pack(4)
class DUILIB_API UiFontEx: public UiFont, public NVRefCount<UiFontEx>
{
public:
    /** Copy data
    */
    void CopyFrom(const UiFont& r)
    {
        m_fontName = r.m_fontName;
        m_fontSize = r.m_fontSize;
        m_bBold = r.m_bBold;
        m_bUnderline = r.m_bUnderline;
        m_bItalic = r.m_bItalic;
        m_bStrikeOut = r.m_bStrikeOut;
    }
};
#pragma pack()


/** Comparison operator
*/
static inline bool operator == (const UiFont& a, const UiFont& b)
{
    return (a.m_fontName == b.m_fontName) && (a.m_fontSize == b.m_fontSize)      &&
           (a.m_bBold == b.m_bBold)       && (a.m_bUnderline == b.m_bUnderline)  &&
           (a.m_bItalic == b.m_bItalic)   && (a.m_bStrikeOut == b.m_bStrikeOut);
}

static inline bool operator != (const UiFont& a, const UiFont& b)
{
    return (a.m_fontName != b.m_fontName) || (a.m_fontSize != b.m_fontSize)       ||
           (a.m_bBold != b.m_bBold)       || (a.m_bUnderline != b.m_bUnderline)   ||
           (a.m_bItalic != b.m_bItalic)   || (a.m_bStrikeOut != b.m_bStrikeOut);
}

/** Comparison operator
*/
static inline bool operator == (const UiFontEx& a, const UiFontEx& b)
{
    return (a.m_fontName == b.m_fontName) && (a.m_fontSize == b.m_fontSize) &&
           (a.m_bBold == b.m_bBold) && (a.m_bUnderline == b.m_bUnderline) &&
           (a.m_bItalic == b.m_bItalic) && (a.m_bStrikeOut == b.m_bStrikeOut);
}

static inline bool operator != (const UiFontEx& a, const UiFontEx& b)
{
    return (a.m_fontName != b.m_fontName) || (a.m_fontSize != b.m_fontSize) ||
           (a.m_bBold != b.m_bBold) || (a.m_bUnderline != b.m_bUnderline) ||
           (a.m_bItalic != b.m_bItalic) || (a.m_bStrikeOut != b.m_bStrikeOut);
}

} // namespace ui

#endif // UI_CORE_UIFONT_H_
