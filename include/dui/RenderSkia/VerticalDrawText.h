#ifndef UI_RENDER_SKIA_VERTICAL_DRAW_TEXT_H_
#define UI_RENDER_SKIA_VERTICAL_DRAW_TEXT_H_

#include "dui/Render/IRender.h"

//Forward declarations of Skia-related classes
class SkCanvas;
class SkPaint;
struct SkRect;
class SkFont;
struct SkPoint;

namespace ui 
{
/** Character attributes for vertically drawn text
*/
struct TVerticalChar;

/** Implementation wrapper for vertical text drawing (top to bottom, right to left)
*/
class VerticalDrawText
{
public:
    VerticalDrawText(SkCanvas* pSkCanvas, SkPaint* pSkPaint, SkPoint* pSkPointOrg);
    VerticalDrawText(const VerticalDrawText& r) = delete;
    VerticalDrawText& operator = (const VerticalDrawText& r) = delete;
    ~VerticalDrawText() = default;

public:
    /** Evaluation function for vertically drawn text: the drawing direction is top to bottom, right to left
    * @param [in] strText The text content to be evaluated
    * @param [in] measureParam The parameters required for evaluation
    */
    UiRect MeasureString(const DString& strText, const MeasureStringParam& measureParam);

    /** Draw text vertically: the drawing direction is top to bottom, right to left
    * @param [in] strText The text content to be drawn
    * @param [in] measureParam The parameters required for drawing
    */
    void DrawString(const DString& strText, const DrawStringParam& drawParam);

private:
    /** Get the UTF16 string and preprocess it (for vertical text drawing)
    */
    UTF16String GetDrawStringUTF16(const DString& strText, bool bSingleLineMode) const;

    /** Calculate the rectangle range occupied by the drawing of each character
    * @param [in] textUTF16 The string
    * @param [in] pSkFont The font
    * @param [in] skPaint The drawing attributes
    * @param [in] bUseFontHeight When drawing vertically, use the font's default height instead of each font's height (all fonts display at the same height)
    * @param [in] fFontHeight The font height
    * @param [in] bRotate90ForAscii When drawing vertically, rotate letters, digits, etc. by 90 degrees for display
    * @param [out] charRects Returns the rectangle range occupied by the drawing of each character
    */
    bool CalculateTextCharBounds(const UTF16String& textUTF16, const SkFont* pSkFont, const SkPaint* skPaint,
                                 bool bUseFontHeight, float fFontHeight, bool bRotate90ForAscii,
                                 std::vector<TVerticalChar>& charRects) const;

    /** Calculate the total rectangle of the drawing area for vertical text (top to bottom, right to left)
     * @param [in] charRects The drawing rectangle of each character (a width or height of 0 means a newline)
     * @param [in] height The height of the destination drawing area
     * @param [in] bSingleLineMode true means single-line text mode, true means multi-line text mode
     * @param [in] fSpacingMul Row spacing multiplier: the ratio as a multiple of the font size (usually 1.0 by default, i.e. 100% of the font size), used to adjust the row spacing proportionally
     * @param [in] fSpacingAdd Row spacing addition: a fixed additional pixel value (usually 0 by default), used to add a fixed offset (pixels) on top of the proportional adjustment
     * @param [in] fWordVerticalSpacing The vertical spacing between characters (pixels)
     * @param [in] fDefaultCharWidth The default column width
     * @param [in] fDefaultCharHeight The default row height
     * @param [out] pColumnRows Returns the index of each character per row per column in the charRects container
     * @param [out] pColumnWidths Returns the width of each column
     * @param [out] pColumnHeights Returns the height of each column
     * @return The total drawing rectangle containing all characters (SkRect)
     */
    SkRect CalculateVerticalTextBounds(const std::vector<TVerticalChar>& charRects, int32_t height, bool bSingleLineMode,
                                       float fSpacingMul, float fSpacingAdd, float fWordVerticalSpacing,
                                       float fDefaultCharWidth, float fDefaultCharHeight,
                                       std::vector<std::vector<int32_t>>* pColumnRows,
                                       std::vector<float>* pColumnWidths,
                                       std::vector<float>* pColumnHeights) const;

    /** Calculate the default character width (used for the width calculation of empty columns)
    */
    float CalculateDefaultCharWidth(const SkFont* pSkFont, const SkPaint* skPaint) const;

    /** Determine whether characters in vertical text need to be rotated 90 degrees for display
     * @param ch The wide character (wchar_t)
     * @return true: needs to be rotated 90 degrees; false: remains upright
    */
    bool NeedRotateForVertical(DUTF16Char ch) const;

private:
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

#endif // UI_RENDER_SKIA_VERTICAL_DRAW_TEXT_H_
