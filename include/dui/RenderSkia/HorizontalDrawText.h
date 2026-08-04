#ifndef UI_RENDER_SKIA_HORIZONTAL_DRAW_TEXT_H_
#define UI_RENDER_SKIA_HORIZONTAL_DRAW_TEXT_H_

#include "dui/Render/IRender.h"

//Forward declarations of Skia-related classes
class SkCanvas;
class SkPaint;
struct SkRect;
class SkFont;
struct SkPoint;

namespace ui
{
/** Character attributes for horizontally drawn text
*/
struct THorizontalChar;

/** Implementation wrapper for horizontal text drawing (left to right, top to bottom)
*/
class HorizontalDrawText
{
public:
    HorizontalDrawText(SkCanvas* pSkCanvas, SkPaint* pSkPaint, SkPoint* pSkPointOrg);
    HorizontalDrawText(const HorizontalDrawText& r) = delete;
    HorizontalDrawText& operator = (const HorizontalDrawText& r) = delete;
    ~HorizontalDrawText() = default;

public:
    /** Evaluation function for horizontally drawn text: the drawing direction is left to right, top to bottom
    * @param [in] strText The text content to be evaluated
    * @param [in] measureParam The parameters required for evaluation
    */
    UiRect MeasureString(const DString& strText, const MeasureStringParam& measureParam);

    /** Draw text horizontally: the drawing direction is left to right, top to bottom
    * @param [in] strText The text content to be drawn
    * @param [in] measureParam The parameters required for drawing
    */
    void DrawString(const DString& strText, const DrawStringParam& drawParam);

private:
    /** Get the UTF16 string and preprocess it (for horizontal text drawing)
    */
    UTF16String GetDrawStringUTF16(const DString& strText, bool bSingleLineMode) const;

    /** Calculate the rectangle range occupied by the drawing of each character
    * @param [in] textUTF16 The string
    * @param [in] pSkFont The font
    * @param [in] skPaint The drawing attributes
    * @param [in] fFontHeight The font height
    */
    bool CalculateTextCharBounds(const UTF16String& textUTF16, const SkFont* pSkFont, const SkPaint* skPaint,
                                 float fFontHeight, std::vector<THorizontalChar>& charRects) const;

    /** Calculate the total rectangle of the drawing area for horizontal text (left to right, top to bottom)
     * @param [in] charRects The drawing rectangle of each character (a width or height of 0 means a newline)
     * @param [in] width The width of the destination drawing area
     * @param [in] bSingleLineMode true means single-line text mode, true means multi-line text mode
     * @param [in] fSpacingMul Row spacing multiplier: the ratio as a multiple of the font size (usually 1.0 by default, i.e. 100% of the font size), used to adjust the row spacing proportionally
     * @param [in] fSpacingAdd Row spacing addition: a fixed additional pixel value (usually 0 by default), used to add a fixed offset (pixels) on top of the proportional adjustment
     * @param [in] fWordHorizontalSpacing The horizontal spacing between characters (pixels)
     * @param [in] fDefaultCharWidth The default column width
     * @param [in] fDefaultCharHeight The default row height
     * @param [out] pColumnRows Returns the index of each character per row per column in the charRects container
     * @param [out] pColumnWidths Returns the width of each column
     * @param [out] pColumnHeights Returns the height of each column
     */
    SkRect CalculateHorizontalTextBounds(const std::vector<THorizontalChar>& charRects, int32_t width, bool bSingleLineMode,
                                         float fSpacingMul, float fSpacingAdd, float fWordHorizontalSpacing,
                                         float fDefaultCharWidth, float fDefaultCharHeight,
                                         std::vector<std::vector<int32_t>>* pRowColumns,
                                         std::vector<float>* pRowHeights,
                                         std::vector<float>* pRowWidths) const;

    /** Calculate the default character width (used for the width calculation of empty rows)
    */
    float CalculateDefaultCharWidth(const SkFont* pSkFont, const SkPaint* skPaint) const;

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

#endif // UI_RENDER_SKIA_HORIZONTAL_DRAW_TEXT_H_
