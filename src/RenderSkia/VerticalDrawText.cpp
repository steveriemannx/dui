#include "dui/RenderSkia/VerticalDrawText.h"
#include "dui/RenderSkia/Font_Skia.h"

#include "dui/Utils/StringUtil.h"
#include "dui/Utils/StringConvert.h"
#include "dui/Utils/PerformanceUtil.h"

#include "dui/RenderSkia/SkiaHeaderBegin.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/core/SkFont.h"
#include "include/core/SkFontMetrics.h"

#include "dui/RenderSkia/SkiaHeaderEnd.h"

namespace ui {

VerticalDrawText::VerticalDrawText(SkCanvas* pSkCanvas, SkPaint* pSkPaint, SkPoint* pSkPointOrg) :
    m_pSkCanvas(pSkCanvas),
    m_pSkPaint(pSkPaint),
    m_pSkPointOrg(pSkPointOrg)
{
}


UTF16String VerticalDrawText::GetDrawStringUTF16(const DString& strText, bool bSingleLineMode) const
{
    DString text = strText;
    StringUtil::ReplaceAll(_T("\r\n"), _T("\n"), text);
    StringUtil::ReplaceAll(_T("\r"), _T(""), text);
    StringUtil::ReplaceAll(_T("\t"), _T(" "), text);
    if (bSingleLineMode) {
        StringUtil::ReplaceAll(_T("\n"), _T(" "), text);
    }
#if defined DUI_UNICODE && defined WCHAR_T_IS_UTF16
    return text;
#else
    std::string textUTF8 = StringConvert::TToUTF8(text);
    return StringConvert::UTF8ToUTF16(textUTF8.c_str(), textUTF8.size());
#endif
}

bool VerticalDrawText::NeedRotateForVertical(DUTF16Char ch) const
{
    // Latin letters, digits, and common symbols: need rotation
    if (
        // Uppercase letters
        (ch >= 0x41 && ch <= 0x5A) ||
        // Lowercase letters
        (ch >= 0x61 && ch <= 0x7A) ||
        // Digits
        (ch >= 0x30 && ch <= 0x39) ||
        // Basic punctuation marks (e.g. !@#$%^&*)
        (ch >= 0x21 && ch <= 0x2F) ||
        (ch >= 0x3A && ch <= 0x40) ||
        (ch >= 0x5B && ch <= 0x60) ||
        (ch >= 0x7B && ch <= 0x7E) ||
        // Half-width punctuation
        (ch >= 0xFF01 && ch <= 0xFF5E) ||
        // Other symbols (e.g. arrows, math symbols)
        (ch >= 0x2190 && ch <= 0x21FF) ||
        (ch >= 0x2200 && ch <= 0x22FF)
        ) {
        return true; // needs to be rotated 90 degrees
    }

    // Other characters: not rotated by default
    return false;
}

/** Character attributes for vertically drawn text
*/
struct TVerticalChar
{
    DUTF16Char ch;
    bool bNewLine;  //Whether it is a newline character
    bool bRotate90; //The character needs to be rotated 90 degrees when drawn
    SkSize size;    //The width and height after drawing the character
    SkRect bounds;  //The bounding information after drawing the character
};

bool VerticalDrawText::CalculateTextCharBounds(const UTF16String& textUTF16, const SkFont* pSkFont, const SkPaint* skPaint,
                                               bool bUseFontHeight, float fFontHeight, bool bRotate90ForAscii,
                                               std::vector<TVerticalChar>& charRects) const
{
    if (textUTF16.empty()) {
        return false;
    }
    ASSERT(fFontHeight > 0);
    if (fFontHeight <= 0) {
        return false;
    }
    ASSERT(pSkFont != nullptr);
    if (pSkFont == nullptr) {
        return false;
    }
    ASSERT(skPaint != nullptr);
    if (skPaint == nullptr) {
        return false;
    }
    //The rectangle range occupied by the drawing of each character
    charRects.clear();
    charRects.reserve(textUTF16.size());

    TVerticalChar verticalChar;
    for (DUTF16Char ch : textUTF16) {
        verticalChar.ch = ch;
        if (ch == L'\n') {
            //Newline character
            verticalChar.bNewLine = true;
            verticalChar.bRotate90 = false;
            verticalChar.size = SkSize();
            verticalChar.bounds = SkRect();
            charRects.push_back(verticalChar);
        }
        else {
            verticalChar.bNewLine = false;
            verticalChar.bRotate90 = false;
            SkScalar fTextWidth = pSkFont->measureText(&ch,
                                                       sizeof(DUTF16Char),
                                                       SkTextEncoding::kUTF16,
                                                       &verticalChar.bounds,//For italic text, this width includes the extended width
                                                       skPaint);
            if ((verticalChar.bounds.width() <= 0) || (verticalChar.bounds.height() <= 0)) {
                //Space or invisible character (the display area is determined using a lowercase letter)
                ch = 'a';
                fTextWidth = pSkFont->measureText(&ch,
                                                  sizeof(DUTF16Char),
                                                  SkTextEncoding::kUTF16,
                                                  &verticalChar.bounds,//For italic text, this width includes the extended width
                                                  skPaint);
            }
            if (bUseFontHeight) {
                //Use the font height as the character height; all characters are the same height
                verticalChar.size = SkSize::Make(std::max(fTextWidth, verticalChar.bounds.width()), (SkScalar)fFontHeight);
            }
            else {
                //Use the font's actual height as the character height
                verticalChar.size = SkSize::Make(std::max(fTextWidth, verticalChar.bounds.width()), verticalChar.bounds.height()); // Use the actual height
            }
            if (bRotate90ForAscii && NeedRotateForVertical(verticalChar.ch)) {
                verticalChar.bRotate90 = true;
                std::swap(verticalChar.size.fHeight, verticalChar.size.fWidth);
            }
            charRects.push_back(verticalChar);
        }
    }
    ASSERT(charRects.size() == textUTF16.size());
    return (charRects.size() == textUTF16.size());
}

SkRect VerticalDrawText::CalculateVerticalTextBounds(const std::vector<TVerticalChar>& charRects, int32_t height, bool bSingleLineMode,
                                                     float fSpacingMul, float fSpacingAdd, float fWordVerticalSpacing,
                                                     float fDefaultCharWidth, float fDefaultCharHeight,
                                                     std::vector<std::vector<int32_t>>* pColumnRows,
                                                     std::vector<float>* pColumnWidths,
                                                     std::vector<float>* pColumnHeights) const
{
    if (charRects.empty()) {
        return SkRect::MakeEmpty();
    }
    if (height <= 0) {
        height = INT32_MAX; // Treat as infinite height
    }
    if (fSpacingMul <= 0.01f) {
        fSpacingMul = 1.0f;
    }
    if (fSpacingAdd < 0) {
        fSpacingAdd = 0;
    }
    if (fWordVerticalSpacing < 0) {
        fWordVerticalSpacing = 0;
    }

    std::vector<std::vector<int32_t>> columnRows;   // The index of each character per row per column in the charRects container
    std::vector<float> columnHeights;  //Returns the height of each column

    float currentY = 0.0f;       // The Y starting position of the current column (drawn top to bottom)
    float maxY = 0.0f;           // The maximum Y coordinate value

    // Row and column counting (starting from 0, consistent with vector indices)
    int32_t currentColumn = 0;   // The current column index (0-based)
 
    bool bNextRow = false; // Marks whether to wrap
    const int32_t nCharCount = (int32_t)charRects.size();
    for (int32_t nCharIndex = 0; nCharIndex < nCharCount; ++nCharIndex) {
        const TVerticalChar& verticalChar = charRects[nCharIndex];

        // Handle wrapping
        if (!bSingleLineMode && (verticalChar.bNewLine || bNextRow)) {
            // Wrap to a new column on the left
            currentY = 0.0f;
            currentColumn++;            // Column index +1
            columnRows.emplace_back();  // Add a new column
            columnHeights.emplace_back(fDefaultCharHeight); //Column height, added for the new column

            bNextRow = false;
            if (verticalChar.bNewLine) {
                continue;
            }
        }

        // Calculate the actual drawing position of the character
        float charTop = currentY;
        float charBottom = charTop + verticalChar.size.height();

        // Record the row and column index of the current character
        while (columnRows.size() <= (size_t)currentColumn) {
            columnRows.emplace_back(); // Automatically add a new column if insufficient
        }
        // Add the current character index to the current row of the current column
        columnRows[currentColumn].push_back(nCharIndex);

        // Record the current column height
        while (columnHeights.size() <= (size_t)currentColumn) {
            columnHeights.emplace_back(fDefaultCharHeight); // Automatically add a new column if insufficient
        }
        float& currentColumnHeight = columnHeights[currentColumn];
        currentColumnHeight = std::max(currentColumnHeight, charBottom);

        // Record the maximum Y coordinate of the characters
        maxY = std::max(maxY, charBottom);

        // Calculate the Y position of the next character (the bottom of the current character)
        float nextY = charBottom;

        // Add character spacing (vertical)
        nextY += fWordVerticalSpacing;

        // Marks whether to wrap
        if (!bSingleLineMode) {
            bNextRow = nextY > height ? true : false;
            if (!bNextRow && (nCharIndex < (nCharCount - 1))) {
                // Determine whether the next character can be drawn; wrap if it cannot be drawn
                const TVerticalChar& nextVerticalChar = charRects[nCharIndex + 1];
                if (nextVerticalChar.size.height() > 0) {
                    if ((nextY + (nextVerticalChar.size.height() * 0.75f)) > height) {
                        bNextRow = true;
                    }
                }
            }
        }
        if (!bNextRow) {
            // No wrap: update the Y position and row index
            currentY = nextY;
        }
    }

    //No valid data
    if (columnRows.empty()) {
        return SkRect::MakeEmpty();
    }

    //Calculate the width of each column
    std::vector<float> columnWidths;  //Returns the width of each column
    columnWidths.resize(columnRows.size());

    const size_t nColumnCount = columnRows.size();
    for (size_t nColumnIndex = 0; nColumnIndex < nColumnCount; ++nColumnIndex) {
        const std::vector<int32_t>& row = columnRows[nColumnIndex];
        SkScalar columnWidth = 0;
        const size_t nRowCount = row.size();
        for (size_t nRowIndex = 0; nRowIndex < nRowCount; ++nRowIndex) {
            const size_t nCharIndex = (size_t)row[nRowIndex];
            ASSERT(nCharIndex < charRects.size());
            if (nCharIndex < charRects.size()) {
                //Draw one character           
                const TVerticalChar& verticalChar = charRects[nCharIndex];
                SkScalar nWidthDiff = 0;
                if (verticalChar.bounds.fLeft < 0) {
                    //Italic text overflow on the left side
                    nWidthDiff += -verticalChar.bounds.fLeft;
                }
                if (verticalChar.bounds.fRight > verticalChar.size.width()) {
                    //Italic text overflow on the right side
                    nWidthDiff += verticalChar.bounds.fRight - verticalChar.size.width();
                }
                columnWidth = std::max(columnWidth, verticalChar.size.width() + nWidthDiff);
            }
        }
        columnWidths[nColumnIndex] = (float)columnWidth;
    }

    //Detect an empty column that contains only a newline character
    const float fMinValue = 0.001f;
    float lastColumnWidth = 0;
    for (size_t nColumnIndex = 0; nColumnIndex < nColumnCount; ++nColumnIndex) {
        float& columnWidth = columnWidths[nColumnIndex];
        if (columnWidth < fMinValue) {
            if (lastColumnWidth > fMinValue) {
                columnWidth = lastColumnWidth;
            }
            else {
                for (size_t j = nColumnIndex + 1; j < nColumnCount; ++j) {
                    if (columnWidths[j] > fMinValue) {
                        columnWidth = columnWidths[j];
                        break;
                    }
                }
            }
        }
        else {
            lastColumnWidth = columnWidth;
        }
    }

    //If all are empty columns, recalculate (fallback handling)
    for (size_t nColumnIndex = 0; nColumnIndex < nColumnCount; ++nColumnIndex) {
        float& columnWidth = columnWidths[nColumnIndex];
        if (columnWidth < fMinValue) {
            columnWidth = fDefaultCharWidth;
        }
    }

    // Calculate the total width
    SkScalar maxX = 0;
    for (size_t nColumnIndex = 0; nColumnIndex < nColumnCount; ++nColumnIndex) {
        float fWidth = columnWidths[nColumnIndex];
        if (nColumnIndex != 0) {
            fWidth = fWidth * fSpacingMul + fSpacingAdd;
        }
        maxX += fWidth;
    }
    maxY = std::max(maxY, fDefaultCharHeight); //When there are only newline characters and no other characters, the default character height must be used

    if (pColumnRows != nullptr) {
        pColumnRows->swap(columnRows);
    }
    if (pColumnWidths != nullptr) {
        pColumnWidths->swap(columnWidths);
    }
    if (pColumnHeights != nullptr) {
        pColumnHeights->swap(columnHeights);
    }
    return SkRect::MakeWH(maxX, maxY);
}

float VerticalDrawText::CalculateDefaultCharWidth(const SkFont* pSkFont, const SkPaint* skPaint) const
{
    if ((pSkFont == nullptr) || (skPaint == nullptr)) {
        return 0;
    }
    DUTF16Char ch = L'W';
    SkRect bounds;
    SkScalar fCharWidth = pSkFont->measureText(&ch,
                                               sizeof(DUTF16Char),
                                               SkTextEncoding::kUTF16,
                                               &bounds,//For italic text, this width includes the extended width
                                               skPaint);

    SkScalar nWidthDiff = 0;
    if (bounds.fLeft < 0) {
        //Italic text overflow on the left side
        nWidthDiff += -bounds.fLeft;
    }
    if (bounds.fRight > fCharWidth) {
        //Italic text overflow on the right side
        nWidthDiff += bounds.fRight - fCharWidth;
    }
    fCharWidth += nWidthDiff;
    return fCharWidth;
}

UiRect VerticalDrawText::MeasureString(const DString& strText, const MeasureStringParam& measureParam)
{
    PerformanceStat statPerformance(_T("VerticalDrawText::MeasureString"));
    ASSERT((m_pSkCanvas != nullptr) && (m_pSkPaint != nullptr) && (m_pSkPointOrg != nullptr));
    if ((m_pSkCanvas == nullptr) || (m_pSkPaint == nullptr) || (m_pSkPointOrg == nullptr)) {
        return UiRect();
    }
    ASSERT(!strText.empty());
    if (strText.empty()) {
        return UiRect();
    }
    ASSERT(measureParam.pFont != nullptr);
    if (measureParam.pFont == nullptr) {
        return UiRect();
    }

    //Get the font interface
    Font_Skia* pSkiaFont = dynamic_cast<Font_Skia*>(measureParam.pFont);
    ASSERT(pSkiaFont != nullptr);
    if (pSkiaFont == nullptr) {
        return UiRect();
    }
    const SkFont* pSkFont = pSkiaFont->GetFontHandle();
    ASSERT(pSkFont != nullptr);
    if (pSkFont == nullptr) {
        return UiRect();
    }

    //Drawing attribute settings
    SkPaint skPaint = *m_pSkPaint;

    bool bSingleLineMode = (measureParam.uFormat & DrawStringFormat::TEXT_SINGLELINE) != 0;

    // Calculate the font height
    SkFontMetrics fontMetrics;
    const SkScalar fFontHeight = pSkFont->getMetrics(&fontMetrics);
    ASSERT(fFontHeight > 0);
    if (fFontHeight <= 0) {
        return UiRect();
    }

    //Text is always drawn using UTF16 encoding
    const UTF16String textUTF16 = GetDrawStringUTF16(strText, bSingleLineMode);

    std::vector<TVerticalChar> charRects;
    if (!CalculateTextCharBounds(textUTF16, pSkFont, &skPaint, measureParam.bUseFontHeight, (float)fFontHeight, measureParam.bRotate90ForAscii, charRects)) {
        return UiRect();
    }
    ASSERT(charRects.size() == textUTF16.size());
    if (charRects.size() != textUTF16.size()) {
        return UiRect();
    }

    //The default character width
    float fDefaultCharWidth = CalculateDefaultCharWidth(pSkFont, &skPaint);
    SkRect skTextBounds = CalculateVerticalTextBounds(charRects, measureParam.rectSize, bSingleLineMode,
                                                      measureParam.fSpacingMul, measureParam.fSpacingAdd,
                                                      measureParam.fWordSpacing,
                                                      fDefaultCharWidth, fFontHeight,
                                                      nullptr, nullptr, nullptr);
    int32_t nTextWidth = SkScalarTruncToInt(skTextBounds.width() + 0.5f);
    if (skTextBounds.width() > nTextWidth) {
        nTextWidth += 1;
    }
    int32_t nTextHeight = SkScalarTruncToInt(skTextBounds.height() + 0.5f);
    if (skTextBounds.height() > nTextHeight) {
        nTextHeight += 1;
    }
    return UiRect(0, 0, nTextWidth, nTextHeight);
}

void VerticalDrawText::DrawString(const DString& strText, const DrawStringParam& drawParam)
{
    // Note: vertical text drawing does not support the following features
    // 1. Text style: DrawStringFormat::TEXT_PATH_ELLIPSIS is not supported; it is treated as DrawStringFormat::TEXT_END_ELLIPSIS
    PerformanceStat statPerformance(_T("VerticalDrawText::DrawString"));
    ASSERT((m_pSkCanvas != nullptr) && (m_pSkPaint != nullptr) && (m_pSkPointOrg != nullptr));
    if ((m_pSkCanvas == nullptr) || (m_pSkPaint == nullptr) || (m_pSkPointOrg == nullptr)) {
        return;
    }

    ASSERT(!strText.empty());
    if (strText.empty()) {
        return;
    }
    ASSERT(!drawParam.textRect.IsEmpty());
    if (drawParam.textRect.IsEmpty()) {
        return;
    }

    ASSERT(drawParam.pFont != nullptr);
    if (drawParam.pFont == nullptr) {
        return;
    }

    SkCanvas* skCanvas = m_pSkCanvas;

    //Get the font interface    
    Font_Skia* pSkiaFont = dynamic_cast<Font_Skia*>(drawParam.pFont);
    ASSERT(pSkiaFont != nullptr);
    if (pSkiaFont == nullptr) {
        return;
    }
    const SkFont* pSkFont = pSkiaFont->GetFontHandle();
    ASSERT(pSkFont != nullptr);
    if (pSkFont == nullptr) {
        return;
    }

    // Set the drawing attributes
    SkPaint skPaint = *m_pSkPaint;
    skPaint.setARGB(drawParam.dwTextColor.GetA(), drawParam.dwTextColor.GetR(),
                    drawParam.dwTextColor.GetG(), drawParam.dwTextColor.GetB());
    if (drawParam.uFade != 0xFF) {
        skPaint.setAlpha(drawParam.uFade);
    }

    //Drawing area
    SkIRect rcSkDestI = { drawParam.textRect.left, drawParam.textRect.top,
                          drawParam.textRect.right, drawParam.textRect.bottom };
    SkRect rcSkDest = SkRect::Make(rcSkDestI);
    rcSkDest.offset(*m_pSkPointOrg);

    bool bSingleLineMode = (drawParam.uFormat & DrawStringFormat::TEXT_SINGLELINE) != 0;

    // Calculate the font height
    SkFontMetrics fontMetrics;
    const SkScalar fFontHeight = pSkFont->getMetrics(&fontMetrics);
    ASSERT(fFontHeight > 0);
    if (fFontHeight <= 0) {
        return;
    }

    // Column spacing settings
    float fSpacingMul = drawParam.fSpacingMul;
    float fSpacingAdd = drawParam.fSpacingAdd;
    float fWordVerticalSpacing = drawParam.fWordSpacing;
    if (fSpacingMul <= 0.01f) {
        fSpacingMul = 1.0f;
    }
    if (fSpacingAdd < 0) {
        fSpacingAdd = 0;
    }
    if (fWordVerticalSpacing < 0) {
        fWordVerticalSpacing = 0;
    }

    //Text is always drawn using UTF16 encoding
    const UTF16String textUTF16 = GetDrawStringUTF16(strText, bSingleLineMode);

    std::vector<TVerticalChar> charRects;
    if (!CalculateTextCharBounds(textUTF16, pSkFont, &skPaint, drawParam.bUseFontHeight, (float)fFontHeight, drawParam.bRotate90ForAscii, charRects)) {
        return;
    }
    ASSERT(charRects.size() == textUTF16.size());
    if (charRects.size() != textUTF16.size()) {
        return;
    }

    // The default character width
    float fDefaultCharWidth = CalculateDefaultCharWidth(pSkFont, &skPaint);

    std::vector<std::vector<int32_t>> columnRows;
    std::vector<float> columnWidths;
    std::vector<float> columnHeights;
    SkRect skTextBounds = CalculateVerticalTextBounds(charRects, drawParam.textRect.Height(), bSingleLineMode,
                                                      fSpacingMul, fSpacingAdd, fWordVerticalSpacing,
                                                      fDefaultCharWidth, fFontHeight,
                                                      &columnRows, &columnWidths, &columnHeights);

    ASSERT(columnRows.size() == columnWidths.size());
    if (columnRows.size() != columnWidths.size()) {
        return;
    }

    //Record the drawing position of each character; alignment will be handled later
    struct TDrawCharPos
    {
        DUTF16Char ch = 0;          //Character
        bool bRotate90 = false;     //Whether it needs to be rotated 90 degrees when drawn
        bool bDrew = false;         //Whether the character has been drawn (used for drawing strikethrough and underline)
        int32_t nColumnIndex = 0;   //Column number
        int32_t nRowIndex = 0;      //Row number
        SkScalar xPos = 0;          //The X coordinate when drawing
        SkScalar yPos = 0;          //The Y coordinate when drawing
        int32_t chHeight = 0;       //Character height (used for drawing strikethrough and underline)
    };
    std::vector<TDrawCharPos> drawCharPos;
    drawCharPos.reserve(charRects.size());

    //How many characters are in each column
    std::vector<int32_t> columnRowCount;
    columnRowCount.resize(columnRows.size(), 0);

    //Start drawing text (column by column, character by character)
    SkScalar xPos = rcSkDest.right();
    const size_t nColumnCount = columnRows.size();
    for (size_t nColumnIndex = 0; nColumnIndex < nColumnCount; ++nColumnIndex) {
        float fColumnWidth = columnWidths[nColumnIndex]; //Column width (pre-calculated)
        SkScalar yPos = rcSkDest.top();
        const std::vector<int32_t>& row = columnRows[nColumnIndex];
        const size_t nRowCount = row.size();
        columnRowCount[nColumnIndex] = (int32_t)nRowCount; //Record how many characters are in each column
        for (size_t nRowIndex = 0; nRowIndex < nRowCount; ++nRowIndex) {
            const size_t nCharIndex = (size_t)row[nRowIndex];
            ASSERT(nCharIndex < charRects.size());
            if (nCharIndex >= charRects.size()) {
                continue;
            }
            //Draw one character
            TDrawCharPos& charPos = drawCharPos.emplace_back(); //Add a new character
            const TVerticalChar& verticalChar = charRects[nCharIndex];
                
            //Record the drawing position of the character; draw it after handling alignment
            charPos.ch = verticalChar.ch;
            charPos.nColumnIndex = (int32_t)nColumnIndex;
            charPos.nRowIndex = (int32_t)nRowIndex;
            charPos.bRotate90 = verticalChar.bRotate90;
            charPos.chHeight = (int32_t)verticalChar.size.height();

            if (charPos.bRotate90) { // The character needs to be rotated 90 degrees when drawn                    
                charPos.xPos = xPos + fontMetrics.fAscent / fFontHeight * fColumnWidth;
                charPos.yPos = yPos;
            }
            else { //The character is drawn normally

                //The actual width occupied by the character (including overflow)
                SkScalar fCharWidth = verticalChar.size.width();
                SkScalar nWidthDiff = 0;
                if (verticalChar.bounds.fLeft < 0) {
                    //Italic text overflow on the left side
                    nWidthDiff += -verticalChar.bounds.fLeft;
                }
                if (verticalChar.bounds.fRight > fCharWidth) {
                    //Italic text overflow on the right side
                    nWidthDiff += verticalChar.bounds.fRight - fCharWidth;
                }
                fCharWidth += nWidthDiff;

                //Alignment of the character within the same column: choose one of three (center alignment suits the current logic best, since the calculation of the character width already accounts for the overflowed fLeft and fRight values)
                //charPos.xPos = xPos - fCharWidth;                     //Character is right-aligned in the column
                //charPos.xPos = xPos - fColumnWidth;                   //Left-aligned
                charPos.xPos = xPos - (fColumnWidth + fCharWidth) / 2;  //Center-aligned

                if (verticalChar.bounds.fLeft < 0) {
                    charPos.xPos += -verticalChar.bounds.fLeft; //The leftmost side of the font is to the left of the base point; the base point needs to be shifted
                }
                if (drawParam.bUseFontHeight) {
                    //All fonts are the same height
                    charPos.yPos = yPos - fontMetrics.fAscent;
                }
                else {
                    //Use the actual height
                    charPos.yPos = yPos - verticalChar.bounds.top();
                }
            }

            //Update the Y coordinate of the next character (with vertical character spacing)
            yPos += (verticalChar.size.height() + fWordVerticalSpacing); //The font height uses the height determined during evaluation
        }

        //The coordinate of the next column, with row spacing
        xPos -= (fColumnWidth * fSpacingMul + fSpacingAdd);
    }

    //Horizontal alignment: all drawn content is aligned as a whole
    int32_t textBoundWidth = SkScalarTruncToInt(skTextBounds.width() + 0.5f);
    if (textBoundWidth < drawParam.textRect.Width()) {
        int32_t nOffsetX = 0;
        if (drawParam.uFormat & DrawStringFormat::TEXT_HCENTER) {
            //Horizontal alignment: center (shift left)
            nOffsetX = -(drawParam.textRect.Width() - textBoundWidth) / 2;
        }
        else if (drawParam.uFormat & DrawStringFormat::TEXT_LEFT) {
            //Horizontal alignment: left (shift left)
            nOffsetX = -(drawParam.textRect.Width() - textBoundWidth);
        }
        else {
            //Horizontal alignment: right (default, no handling)
            nOffsetX = 0;
        }
        if (nOffsetX != 0) {
            for (TDrawCharPos& charPos : drawCharPos) {
                charPos.xPos += nOffsetX;
            }
        }
    }

    //Vertical alignment: aligned by the actual area occupied by each column
    std::vector<float> columnOffsets;    
    if (drawParam.uFormat & DrawStringFormat::TEXT_VCENTER) {
        //Vertical alignment: center
        columnOffsets.resize(columnHeights.size(), 0.0f);
        int32_t textRectHeight = drawParam.textRect.Height();
        for (size_t nColumnIndex = 0; nColumnIndex < columnHeights.size(); ++nColumnIndex) {
            if (columnHeights[nColumnIndex] < textRectHeight) {
                columnOffsets[nColumnIndex] = (textRectHeight - columnHeights[nColumnIndex]) / 2;
            }
        }
    }
    else if (drawParam.uFormat & DrawStringFormat::TEXT_BOTTOM) {
        //Vertical alignment: bottom
        columnOffsets.resize(columnHeights.size(), 0.0f);
        int32_t textRectHeight = drawParam.textRect.Height();
        for (size_t nColumnIndex = 0; nColumnIndex < columnHeights.size(); ++nColumnIndex) {
            if (columnHeights[nColumnIndex] < textRectHeight) {
                columnOffsets[nColumnIndex] = (textRectHeight - columnHeights[nColumnIndex]);
            }
        }
    }
    else if (drawParam.uFormat & DrawStringFormat::TEXT_VJUSTIFY) {
        //Vertical alignment: justify
        ASSERT(columnRowCount.size() == columnHeights.size());
        if (!columnRowCount.empty() && columnRowCount.size() == columnHeights.size()) {
            std::vector<std::vector<float>> columnRowOffsets; //The offset of each character
            columnRowOffsets.resize(columnHeights.size());

            int32_t textRectHeight = drawParam.textRect.Height();
            for (size_t nColumnIndex = 0; nColumnIndex < columnHeights.size(); ++nColumnIndex) {
                const int32_t nRowCount = columnRowCount[nColumnIndex];
                // Calculate the offset of each character
                columnRowOffsets[nColumnIndex].resize(nRowCount, 0.0f);

                if (nRowCount < 2) {
                    //This column has fewer than 2 characters, no justification needed
                    continue;
                }

                if (columnHeights[nColumnIndex] < textRectHeight) {
                    float fOffsetTotal = (textRectHeight - columnHeights[nColumnIndex]); //The total offset
                    float fOffset = fOffsetTotal / (nRowCount - 1); //The offset of each character

                    for (int32_t nRow = 0; nRow < nRowCount; ++nRow) {
                        columnRowOffsets[nColumnIndex][nRow] = fOffset * nRow;
                    }
                }
            }

            //Set the vertical offset for each character
            const int32_t nOffsetColumnCount = (int32_t)columnRowOffsets.size();
            for (TDrawCharPos& charPos : drawCharPos) {
                ASSERT(charPos.nColumnIndex < nOffsetColumnCount);
                if (charPos.nColumnIndex < nOffsetColumnCount) {
                    const std::vector<float>& rowOffsets = columnRowOffsets[charPos.nColumnIndex];
                    ASSERT(charPos.nRowIndex < (int32_t)rowOffsets.size());
                    if (charPos.nRowIndex < (int32_t)rowOffsets.size()) {
                        charPos.yPos += rowOffsets[charPos.nRowIndex];
                    }
                }
            }
        }
    }
    else {
        //Vertical alignment: top (default, no handling)
    }
    if (columnOffsets.size() == columnHeights.size()) {
        const size_t nOffsetCount = columnOffsets.size();
        for (TDrawCharPos& charPos : drawCharPos) {
            if (charPos.nColumnIndex < (int32_t)nOffsetCount) {
                charPos.yPos += columnOffsets[charPos.nColumnIndex];
            }
        }
    }

    //Draw each character
    
    //Enlarge the range to avoid misjudgments that prevent characters from being drawn
    SkRect skTextRect = rcSkDest;
    skTextRect.fLeft -= fFontHeight;
    skTextRect.fRight += fFontHeight;
    skTextRect.fTop -= fFontHeight;
    skTextRect.fBottom += fFontHeight;

    bool bNeedEllipsis = false;
    if ((drawParam.uFormat & TEXT_PATH_ELLIPSIS) || (drawParam.uFormat & TEXT_END_ELLIPSIS)) {
        //If the drawing area is insufficient, add "..." at the end to ellipsize part of the text (TEXT_PATH_ELLIPSIS is not supported; it is treated as TEXT_END_ELLIPSIS)
        bNeedEllipsis = true;
    }

    //Restore the clip region automatically in the destructor
    struct AutoClipInfo
    {
        bool bCliped = false;
        int drawSaveCount = 0;
        SkCanvas* skCanvas = nullptr;

        ~AutoClipInfo()
        {
            if (bCliped && (skCanvas != nullptr)) {
                skCanvas->restoreToCount(drawSaveCount);
            }
        }
    };

    //Start drawing
    AutoClipInfo autoClip;
    if (!(drawParam.uFormat & TEXT_NOCLIP)) {
        //Set the clip region
        autoClip.bCliped = true;
        autoClip.drawSaveCount = skCanvas->save();
        autoClip.skCanvas = skCanvas;
        skCanvas->clipRect(rcSkDest, true);
    }

    // Draw characters
    const size_t drawCharCount = drawCharPos.size();
    for (size_t charIndex = 0; charIndex < drawCharCount; ++charIndex) {
        TDrawCharPos& charPos = drawCharPos[charIndex];
        if (bSingleLineMode && charPos.nColumnIndex != 0) {
            //Single-line mode: draw only one column
            break;
        }

        if (bNeedEllipsis && (charIndex < (drawCharCount - 1))) {// Business logic for whether to draw "..."
            //Determine whether the next character is out of bounds (vertical)
            float fColumnWidth = 0;
            if (charPos.nColumnIndex < (int32_t)columnWidths.size()) {
                fColumnWidth = columnWidths[charPos.nColumnIndex]; //Column width
            }
            bool bNeedDrawEllipsis = false;
            const TDrawCharPos& nextCharPos = drawCharPos[charIndex + 1];
            if (nextCharPos.nColumnIndex == charPos.nColumnIndex) {
                SkScalar yPos = nextCharPos.yPos;
                if (nextCharPos.bRotate90) {
                    yPos += nextCharPos.chHeight;
                }                
                if ((yPos > (SkScalar)rcSkDest.fBottom) &&
                    (bSingleLineMode || (charPos.xPos - fColumnWidth) < (SkScalar)rcSkDest.fLeft)) {
                    //The next character cannot be displayed
                    bNeedDrawEllipsis = true;
                }
            }
            else if (!bSingleLineMode && (charPos.xPos - fColumnWidth) < (SkScalar)rcSkDest.fLeft) {
                //The data of other columns on the left cannot be displayed
                bNeedDrawEllipsis = true;
            }
            if (bNeedDrawEllipsis) {
                //Character out of bounds, draw "..."
                std::string ellipsis = "...";
                skCanvas->save();
                skCanvas->translate(charPos.xPos, charPos.bRotate90 ? charPos.yPos : charPos.yPos - charPos.chHeight / 2);
                skCanvas->rotate(90);
                skCanvas->drawSimpleText(ellipsis.data(), ellipsis.size(), SkTextEncoding::kUTF8,
                                         0, -fColumnWidth / 5,
                                         *pSkFont, skPaint);
                skCanvas->restore();

                //Stop drawing after going out of bounds
                break;
            }
        }

        if (!skTextRect.contains(charPos.xPos, charPos.yPos)) {
            //The current character is not in the text display area; do not draw it
            continue;
        }
        charPos.bDrew = true;
        if (charPos.bRotate90) {
            //The character needs to be rotated 90 degrees when drawn
            int saveCount = skCanvas->save();
            skCanvas->translate(charPos.xPos, charPos.yPos);
            skCanvas->rotate(90);
            skCanvas->drawSimpleText(&charPos.ch, sizeof(charPos.ch), SkTextEncoding::kUTF16,
                                     0, 0,
                                     *pSkFont, skPaint);
            skCanvas->restoreToCount(saveCount);
        }
        else {
            skCanvas->drawSimpleText(&charPos.ch, sizeof(charPos.ch), SkTextEncoding::kUTF16,
                                     charPos.xPos, charPos.yPos,
                                     *pSkFont, skPaint);
        }
    }

    //Draw underline/strikethrough
    if ((drawParam.pFont->IsUnderline() || drawParam.pFont->IsStrikeOut()) && !columnWidths.empty()) {
        struct TextColumnInfo
        {
            UiPoint ptStart;
            UiPoint ptEnd;
            bool bStartRotate90 = false;
            bool bEndRotate90 = false;
            bool bStartFlag = false;
            bool bEndFlag = false;
            int32_t chStartHeight = 0;
            int32_t chEndHeight = 0;
        };
        std::vector<TextColumnInfo> textColumnInfo;
        textColumnInfo.resize(columnWidths.size());
        for (const TDrawCharPos& charPos : drawCharPos) {
            if (!charPos.bDrew) {
                continue;
            }
            ASSERT(charPos.nColumnIndex < (int32_t)textColumnInfo.size());
            if (charPos.nColumnIndex >= (int32_t)textColumnInfo.size()) {
                continue;
            }
            TextColumnInfo& textColumn = textColumnInfo[charPos.nColumnIndex];
            if (!textColumn.bStartFlag) {
                textColumn.bStartRotate90 = charPos.bRotate90;
                textColumn.bStartFlag = true;
                textColumn.ptStart.x = (int32_t)charPos.xPos;
                textColumn.ptStart.y = (int32_t)(charPos.yPos + 0.5f);
                textColumn.chStartHeight = charPos.chHeight;
            }
            else {
                textColumn.bEndRotate90 = charPos.bRotate90;
                textColumn.bEndFlag = true;
                textColumn.ptEnd.x = (int32_t)charPos.xPos;
                textColumn.ptEnd.y = (int32_t)charPos.yPos;
                textColumn.chEndHeight = charPos.chHeight;
            }
        }

        // Draw lines
        for (size_t nColumnIndex = 0; nColumnIndex < textColumnInfo.size(); ++nColumnIndex) {
            const TextColumnInfo& textColumn = textColumnInfo[nColumnIndex];
            if (!textColumn.bStartFlag && !textColumn.bEndFlag) {
                continue;
            }

            // Default fraction of the text size to use for a strike-through or underline.
            static constexpr SkScalar kLineThicknessFactor = (SK_Scalar1 / 18);

            SkScalar top = (SkScalar)textColumn.ptStart.y;
            SkScalar x_scalar = SkIntToScalar(textColumn.ptStart.x);
            if (textColumn.bEndFlag) {
                x_scalar = (SkIntToScalar(textColumn.ptStart.x) + SkIntToScalar(textColumn.ptEnd.x)) / 2;
            }
            if (!textColumn.bStartRotate90) {
                top -= textColumn.chStartHeight;
            }
            else {
                top += fFontHeight * 1 / 9;
            }
            if (top <= rcSkDest.fTop) {
                top = rcSkDest.fTop + fFontHeight * 1 / 9;
            }

            SkScalar height = (SkScalar)textColumn.chStartHeight;
            if (textColumn.bEndFlag) {
                height = (SkScalar)(textColumn.ptEnd.y - textColumn.ptStart.y + textColumn.chEndHeight * 4 / 5);
                if (textColumn.bStartRotate90) {
                    height -= fFontHeight * 4 / 5;
                }
            }

            if (drawParam.pFont->IsStrikeOut()) {
                //Strikethrough
                const SkScalar width = pSkFont->getSize() * kLineThicknessFactor;

                const float fColumnWidth = columnWidths[nColumnIndex];
                const SkScalar xOffset = fColumnWidth * 2 / 5;

                const SkRect r = SkRect::MakeLTRB(x_scalar + xOffset, top, x_scalar + xOffset + width, top + height);
                skCanvas->drawRect(r, skPaint);

            }
            if (drawParam.pFont->IsUnderline()) {
                //Underline
                const SkScalar width = pSkFont->getSize() * kLineThicknessFactor * 1.5f;

                const SkRect r = SkRect::MakeLTRB(x_scalar, top, x_scalar + width, top + height);
                skCanvas->drawRect(r, skPaint);
            }            
        }
    }
}

} // namespace ui
