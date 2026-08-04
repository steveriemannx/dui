/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef UI_RENDER_SKIA_SK_TEXT_BOX_H_
#define UI_RENDER_SKIA_SK_TEXT_BOX_H_

#include "SkiaHeaderBegin.h"
#include "include/core/SkCanvas.h"
#include "SkiaHeaderEnd.h"

#include <vector>

//The origin of this file's original source: skia/chrome_67/include/utils/SkTextBox.h
//Based on the original file, with modifications to be compatible with the latest version of the skia code (2023-06-25)
//The original file was removed after chrome 68.

namespace ui
{

/** \class SkTextBox

    SkTextBox is a helper class for drawing 1 or more lines of text
    within a rectangle. The textbox is positioned and clipped by its Frame.
    The Margin rectangle controls where the text is drawn relative to
    the Frame. Line-breaks occur inside the Margin rectangle.

    Spacing is a linear equation used to compute the distance between lines
    of text. Spacing consists of two scalars: mul and add, and the spacing
    between lines is computed as: spacing = font.getSize() * mul + add
*/
class SkTextBox 
{
public:
    SkTextBox();

    //Original function: SkPaint::breakText has been removed in the latest Skia code; this is a self-implemented version.
    /** Returns the bytes of text that fit within maxWidth.
        @param text           character codes or glyph indices to be measured
        @param byteLength     number of bytes of text to measure
        @param textEncoding   text encoding, SkTextEncoding::kUTF8 or kUTF16 or kUTF32
        @param maxWidth       advance limit; text is measured while advance is less than maxWidth
        @param measuredWidth  returns the width of the text less than or equal to maxWidth
        @param measuredHeight  returns the height of the text
        @return               bytes of text that fit, always less than or equal to length
    */
    static size_t breakText(const void* text, size_t byteLength, SkTextEncoding textEncoding,
                            const SkFont& font, const SkPaint& paint, SkScalar maxWidth,
                            SkScalar* measuredWidth = nullptr, SkScalar* measuredHeight = nullptr);

    /** Special version, with performance optimizations
    * @param [out] glyphs How many Glyph characters were drawn
    * @param [out] glyphChars Returns how many input characters each Glyph character occupies
    * @param [out] glyphWidths Returns the drawing width of each Glyph character
    * @param [out] glyphCharList Returns how many input characters each glyph character is made of
    * @param [out] glyphWidthList Returns the output width value of each glyph character
    */
    static size_t breakText(const void* text, size_t byteLength, SkTextEncoding textEncoding,
                            const SkFont& font, const SkPaint& paint, SkScalar maxWidth,
                            SkScalar* measuredWidth, SkScalar* measuredHeight,
                            std::vector<SkGlyphID>& glyphs,
                            std::vector<uint8_t>& glyphChars,
                            std::vector<SkScalar>& glyphWidths,
                            std::vector<uint8_t>* glyphCharList,
                            std::vector<SkScalar>* glyphWidthList);
public:
    //Line break mode
    enum LineMode {
        kOneLine_Mode,   //Single-line mode
        kWordBreak_Mode, //Word Break mode: multi-line mode, breaking lines by word (default)
        kCharBreak_Mode, //Char Break mode: multi-line mode, breaking lines by character (English words and digits are also split by wrapping)

        kModeCount
    };
    LineMode getLineMode() const { return fLineMode; }
    void setLineMode(LineMode);

    //Vertical alignment
    enum SpacingAlign {
        kStart_SpacingAlign,  //Top-aligned, equivalent to: top alignment (default)
        kCenter_SpacingAlign, //Center-aligned, equivalent to: vcener alignment
        kEnd_SpacingAlign,    //Bottom-aligned, equivalent to: bottom alignment

        kSpacingAlignCount
    };
    SpacingAlign getSpacingAlign() const { return (SpacingAlign)fSpacingAlign; }
    void         setSpacingAlign(SpacingAlign);

    //Horizontal alignment
    enum TextAlign {
        kLeft_Align,        //Left-aligned (default)
        kCenter_Align,      //Center-aligned
        kRight_Align,       //Right-aligned

        kAlignCount
    };
    TextAlign getTextAlign() const { return (TextAlign)fTextAlign; }
    void      setTextAlign(TextAlign);

    //When the drawing area is insufficient, automatically draw an ellipsis at the end
    bool getEndEllipsis() const { return fEndEllipsis; };
    void setEndEllipsis(bool);

    //When the drawing area is insufficient, automatically draw an ellipsis instead of the text (only in single-line text mode; multi-line text mode does not support this attribute)
    //If the string contains a backslash (\\) character, keep as much text as possible after the last backslash.
    bool getPathEllipsis() const { return fPathEllipsis; };
    void setPathEllipsis(bool);

    //Font attribute: underline
    bool getUnderline() const { return fUnderline; }
    void setUnderline(bool);

    //Font attribute: strikethrough
    bool getStrikeOut() const { return fStrikeOut; }
    void setStrikeOut(bool);

    void getBox(SkRect*) const;
    void setBox(const SkRect&);
    void setBox(SkScalar left, SkScalar top, SkScalar right, SkScalar bottom);

    //Sets whether to clip the Box area; clipping avoids text drawing beyond the bounds
    bool getClipBox() const { return fClipBox; }
    void setClipBox(bool bClipBox);

    //Row spacing: mul is the multiplier of the row spacing, add is the additional amount
    //After setting, the actual row spacing is: fontHeight * mul + add;
    void getSpacing(SkScalar* mul, SkScalar* add) const;
    void setSpacing(SkScalar mul, SkScalar add);

    /** Draw text; this function is a convenience wrapper around draw(SkCanvas*) and setText.
    *   It is equivalent to the following code:
    *       setText(text, len, textEncoding, font, paint);
    *       draw(canvas);
    */
    void draw(SkCanvas*, 
              const char text[], size_t len, SkTextEncoding, 
              const SkFont&, const SkPaint&);

    void setText(const char text[], size_t len, SkTextEncoding, 
                 const SkFont&, const SkPaint&);
    void draw(SkCanvas*);

    int  countLines() const;
    SkScalar getTextHeight() const;

    sk_sp<SkTextBlob> snapshotTextBlob(SkScalar* computedBottom) const;

    class Visitor {
    public:
        virtual ~Visitor() {}
        virtual void operator()(const char*, size_t, SkTextEncoding, 
                                SkScalar x, SkScalar y, 
                                const SkFont&, const SkPaint&,
                                bool hasMoreText, bool isLastLine) = 0;
    };

private:
    SkScalar visit(Visitor& visitor) const;

    /** Convert text to Glyphs
    * @param [out] glyphs The converted Glyphs
    * @param [out] The number of characters in the original text string corresponding to each SkGlyphID
    * @param [out] The number of bytes occupied by each character
    */
    static bool TextToGlyphs(const void* text, size_t byteLength, SkTextEncoding textEncoding, 
                             const SkFont& font,
                             std::vector<SkGlyphID>& glyphs,
                             std::vector<uint8_t>& glyphChars,
                             size_t& charBytes);

private:
    //Text drawing area
    SkRect fBox;

    //Whether to clip the Box area; defaults to true
    bool fClipBox;

    //Row spacing setting parameters
    SkScalar fSpacingMul, fSpacingAdd;

    //Line break mode
    LineMode fLineMode;

    //Text vertical alignment
    uint8_t fSpacingAlign;

    //Text horizontal alignment
    uint8_t fTextAlign;

    //Text data
    const char* fText;

    //Text length
    size_t fLen;

    //Text encoding
    SkTextEncoding fTextEncoding;

    //Drawing attribute settings
    const SkPaint* fPaint;

    //Font settings for drawing
    const SkFont* fFont;

    //When the drawing area is insufficient, automatically draw an ellipsis at the end
    bool fEndEllipsis;

    //When the drawing area is insufficient, automatically draw an ellipsis instead of the text
    //If the string contains a backslash (\\) character, keep as much text as possible after the last backslash.
    bool fPathEllipsis;

    //Font attribute: underline
    bool fUnderline;

    //Font attribute: strikethrough
    bool fStrikeOut;
};

class SkTextLineBreaker {
public:
    /** Count how many lines are needed for drawing
     * @param [in] text The start address of the text string
     * @param [in] len The length of the text string (bytes)
     * @param [in] textEncoding The encoding of the text
     * @param [in] font The font
     * @param [in] paint The drawing attributes
     * @param [in] width The width of the drawing area
     * @param [in] lineMode The line break mode
     * @param [out] lineLenList Returns the length of each line's text data (bytes)
     */
    static int CountLines(const char text[], size_t len, SkTextEncoding textEncoding,
                          const SkFont& font,  const SkPaint& paint,
                          SkScalar width, SkTextBox::LineMode lineMode,
                          std::vector<size_t>* lineLenList = nullptr);
};

} //namespace ui

#endif //UI_RENDER_SKIA_SK_TEXT_BOX_H_
