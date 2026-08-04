#ifndef UI_RENDER_SKIA_DRAW_RICH_TEXT_H_
#define UI_RENDER_SKIA_DRAW_RICH_TEXT_H_

#include "dui/Render/IRender.h"

//Forward declarations of Skia-related classes
class SkCanvas;
class SkPaint;
struct SkRect;
class SkFont;
struct SkPoint;
enum class SkTextEncoding;

namespace ui 
{
class IRender;

/** Drawing implementation for RichText formatted text
*/
class DrawRichText
{
public:
    DrawRichText(IRender* pRender, SkCanvas* pSkCanvas, SkPaint* pSkPaint, SkPoint* pSkPointOrg);
    DrawRichText(const DrawRichText& r) = delete;
    DrawRichText& operator = (const DrawRichText& r) = delete;
    ~DrawRichText() = default;

public:

    /** Draw the rich text
    */
    void InternalDrawRichText(const UiRect& rcTextRect,
                              const UiSize& szScrollOffset,
                              IRenderFactory* pRenderFactory, 
                              const std::vector<RichTextData>& richTextData,                   
                              uint8_t uFade,
                              bool bMeasureOnly,
                              RichTextLineInfoParam* pLineInfoParam,
                              std::shared_ptr<DrawRichTextCache>* pDrawRichTextCache,
                              std::vector<std::vector<UiRect>>* pRichTextRects);

    /** Determine whether the RichText draw cache is valid
    * @param [in] textRect The rectangle area for drawing the text
    * @param [in] richTextData The formatted text content; returns the areas where the text is drawn
    * @param [out] spDrawRichTextCache Returns the draw cache
    */
    bool IsValidDrawRichTextCache(const UiRect& textRect,
                                  const std::vector<RichTextData>& richTextData,
                                  const std::shared_ptr<DrawRichTextCache>& spDrawRichTextCache);

    /** Update the RichText draw cache (incremental calculation)
    * @param [in] spOldDrawRichTextCache The cache to be updated
    * @param [in] spUpdateDrawRichTextCache The cache for incremental drawing
    * @param [in,out] richTextDataNew The latest full data; the data is swapped into the internal container
    * @param [in] nStartLine The starting line number for recalculation
    * @param [in] modifiedLines The line numbers that were modified
    * @param [in] nModifiedRows The modified text, split into how many lines (logical lines) after calculation
    * @param [in] deletedLines The deleted lines
    * @param [in] nDeletedRows How many logical lines were deleted
    * @param [in] rowRectTopList The top coordinate of each logical line, used to update line coordinates (the index is the logical line, numbered from 0)
    */
    bool UpdateDrawRichTextCache(std::shared_ptr<DrawRichTextCache>& spOldDrawRichTextCache,
                                 const std::shared_ptr<DrawRichTextCache>& spUpdateDrawRichTextCache,
                                 std::vector<RichTextData>& richTextDataNew,
                                 size_t nStartLine,
                                 const std::vector<size_t>& modifiedLines,
                                 size_t nModifiedRows,
                                 const std::vector<size_t>& deletedLines,
                                 size_t nDeletedRows,
                                 const std::vector<int32_t>& rowRectTopList);

    /** Compare whether the data of two draw caches is identical
    */
    bool IsDrawRichTextCacheEqual(const DrawRichTextCache& first, const DrawRichTextCache& second) const;

    /** Draw the content in the RichText cache (before drawing, use IsValidDrawRichTextCache to check whether the cache is invalid)
    * @param [in] spDrawRichTextCache The cached data
    * @param [in] rcNewTextRect The rectangle area for drawing the text
    * @param [in] szNewScrollOffset The new scroll position
    * @param [in] rowXOffset The horizontal offset list of each line (logical lines)
    * @param [in] uFade The alpha (0 - 255)
    * @param [out] pRichTextRects If not nullptr, returns the list of rectangle ranges drawn for each item in richTextData
    */
    void DrawRichTextCacheData(const std::shared_ptr<DrawRichTextCache>& spDrawRichTextCache,
                               const UiRect& textRect,
                               const UiSize& szNewScrollOffset,
                               const std::vector<int32_t>& rowXOffset,
                               uint8_t uFade,
                               std::vector<std::vector<UiRect>>* pRichTextRects = nullptr);

private:
    /** Split the text into multiple lines by the newline characters ('\r' or '\n')
    */
    void SplitLines(const std::wstring_view& lineText, std::vector<uint32_t>& lineSeprators, std::vector<std::wstring_view>& lineTextViewList);

    /** Draw one character and record its character attributes
    * @param [in] pLineInfoParam The character attribute record table
    * @param [in] ch The character currently being drawn; only valid for special characters such as carriage return and newline
    * @param [in] glyphChars The number of Unicode characters corresponding to the current glyph (1 or 2)
    * @param [in] glyphCount The total number of characters
    * @param [in] nLineTextIndex The physical line number of the text (i.e. the line split by the newline character '\n')
    * @param [in] nLineTextRowIndex The logical row number within the physical line (numbered from 0 in each physical line)
    * @param [in] xPos The X coordinate for drawing the character
    * @param [in] yPos The Y coordinate for drawing the character
    * @param [in] glyphWidth The drawing width of the current character
    * @param [in] nRowHeight The current row height
    */
    void OnDrawUnicodeChar(RichTextLineInfoParam* pLineInfoParam, DStringW::value_type ch, uint8_t glyphChars, size_t glyphCount,
                           size_t nLineTextIndex, uint32_t nLineTextRowIndex, float xPos, int32_t yPos, float glyphWidth, int32_t nRowHeight);

    /** Draw text according to the set attributes; the character encoding is specified externally
    */
    void DrawTextString(const UiRect& textRect,
                        const char* text, size_t len, SkTextEncoding textEncoding,
                        uint32_t uFormat, const SkPaint& skPaint, IFont* pFont) const;

    /** Draw text according to the set attributes
    */
    void DrawTextString(const UiRect& textRect, const DString& strText, uint32_t uFormat,
                        const SkPaint& skPaint, IFont* pFont) const;

    /** Get the text encoding
    */
    SkTextEncoding GetTextEncoding() const;

private:
    /** The associated render interface
    */
    IRender* m_pRender;

    /** The drawing canvas
    */
    SkCanvas* m_pSkCanvas;

    /** Drawing attributes
    */
    SkPaint* m_pSkPaint;

    /** The origin coordinates of the view
    */
    SkPoint* m_pSkPointOrg;
};

} // namespace ui

#endif // UI_RENDER_SKIA_DRAW_RICH_TEXT_H_
