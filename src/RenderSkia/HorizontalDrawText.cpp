#include "dui/RenderSkia/HorizontalDrawText.h"
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

HorizontalDrawText::HorizontalDrawText(SkCanvas* pSkCanvas, SkPaint* pSkPaint, SkPoint* pSkPointOrg) :
    m_pSkCanvas(pSkCanvas),
    m_pSkPaint(pSkPaint),
    m_pSkPointOrg(pSkPointOrg)
{
}

UTF16String HorizontalDrawText::GetDrawStringUTF16(const DString& strText, bool bSingleLineMode) const
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

/** Character attributes for horizontally drawn text
*/
struct THorizontalChar
{
    DUTF16Char ch;
    bool bNewLine;  //Whether it is a newline character
    SkSize size;    //The width and height after drawing the character
    SkRect bounds;  //The bounding information after drawing the character
};

bool HorizontalDrawText::CalculateTextCharBounds(const UTF16String& textUTF16, const SkFont* pSkFont, const SkPaint* skPaint,
                                                 float fFontHeight, std::vector<THorizontalChar>& charRects) const
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

    THorizontalChar horizontalChar;
    for (DUTF16Char ch : textUTF16) {
        horizontalChar.ch = ch;
        if (ch == L'\n') {
            //Newline character
            horizontalChar.bNewLine = true;
            horizontalChar.size = SkSize();
            horizontalChar.bounds = SkRect();
            charRects.push_back(horizontalChar);
        }
        else {
            horizontalChar.bNewLine = false;
            SkScalar fTextWidth = pSkFont->measureText(&ch,
                                                       sizeof(DUTF16Char),
                                                       SkTextEncoding::kUTF16,
                                                       &horizontalChar.bounds,//For italic text, this width includes the extended width
                                                       skPaint);
            if ((horizontalChar.bounds.width() <= 0) || (horizontalChar.bounds.height() <= 0)) {
                //Space or invisible character (the display area is determined using a lowercase letter)
                ch = 'a';
                fTextWidth = pSkFont->measureText(&ch,
                                                  sizeof(DUTF16Char),
                                                  SkTextEncoding::kUTF16,
                                                  &horizontalChar.bounds,//For italic text, this width includes the extended width
                                                  skPaint);
            }

            //Use the font height as the character height; all characters are the same height
            horizontalChar.size = SkSize::Make(std::max(fTextWidth, horizontalChar.bounds.width()), (SkScalar)fFontHeight);
            charRects.push_back(horizontalChar);
        }
    }
    return (charRects.size() == textUTF16.size());
}

SkRect HorizontalDrawText::CalculateHorizontalTextBounds(const std::vector<THorizontalChar>& charRects, int32_t width, bool bSingleLineMode,
                                                         float fSpacingMul, float fSpacingAdd, float fWordHorizontalSpacing,
                                                         float fDefaultCharWidth, float fDefaultCharHeight,
                                                         std::vector<std::vector<int32_t>>* pRowColumns,
                                                         std::vector<float>* pRowHeights,
                                                         std::vector<float>* pRowWidths) const
{
    if (charRects.empty()) {
        return SkRect::MakeEmpty();
    }
    if (width <= 0) {
        width = INT32_MAX; // Treat as infinite width
    }
    if (fSpacingMul <= 0.01f) {
        fSpacingMul = 1.0f;
    }
    if (fSpacingAdd < 0) {
        fSpacingAdd = 0;
    }
    if (fWordHorizontalSpacing < 0) {
        fWordHorizontalSpacing = 0;
    }

    std::vector<std::vector<int32_t>> rowColumns;   // The index of each character per row per column in charRects
    std::vector<float> rowWidths;  // The width of each row

    float currentX = 0.0f;       // The current X starting position (left to right)
    float maxX = 0.0f;           // The maximum X value

    // Row and column counting
    int32_t currentRow = 0;      // The current row index

    bool bNextLine = false; // Marks whether to wrap
    const int32_t nCharCount = (int32_t)charRects.size();
    for (int32_t nCharIndex = 0; nCharIndex < nCharCount; ++nCharIndex) {
        const THorizontalChar& horizontalChar = charRects[nCharIndex];

        // Handle wrapping
        if (!bSingleLineMode && (horizontalChar.bNewLine || bNextLine)) {
            currentX = 0.0f;
            currentRow++;             // Row index +1
            rowColumns.emplace_back();  // Add a new row
            rowWidths.emplace_back(fDefaultCharWidth); // Row width
  
            bNextLine = false;
            if (horizontalChar.bNewLine) {
                continue;
            }
        }

        // Calculate the character position
        float charLeft = currentX;
        float charRight = charLeft + horizontalChar.size.width();

        // Record the character's row and column index
        while (rowColumns.size() <= (size_t)currentRow) {
            rowColumns.emplace_back(); // Automatically add a new row if insufficient
        }
        // Add the current character index to the current column of the current row
        rowColumns[currentRow].push_back(nCharIndex);

        // Record the row width
        while (rowWidths.size() <= (size_t)currentRow) {
            rowWidths.emplace_back(fDefaultCharWidth); // Automatically add a new row if insufficient
        }
        float& currentRowWidth = rowWidths[currentRow];
        currentRowWidth = std::max(currentRowWidth, charRight);

        // Update the maximum X value
        maxX = std::max(maxX, charRight);

        // Calculate the X position of the next character
        float nextX = charRight;

        // Add character spacing (horizontal)
        nextX += fWordHorizontalSpacing;

        // Determine whether wrapping is needed
        if (!bSingleLineMode) {
            bNextLine = nextX > width ? true : false;
            if (!bNextLine && (nCharIndex < (nCharCount - 1))) {
                const THorizontalChar& nextHorizontalChar = charRects[nCharIndex + 1];
                if (nextHorizontalChar.size.width() > 0) {
                    if ((nextX + (nextHorizontalChar.size.width() * 0.75f)) > width) {
                        bNextLine = true;
                    }
                }
            }
        }

        if (!bNextLine) {
            // No wrap: update the X position and column index
            currentX = nextX;
        }
    }

    //No valid data
    if (rowColumns.empty()) {
        return SkRect::MakeEmpty();
    }

    // Calculate the height of each row
    std::vector<float> rowHeights;
    rowHeights.resize(rowColumns.size());

    const size_t nRowCount = rowColumns.size();
    for (size_t nRowIndex = 0; nRowIndex < nRowCount; ++nRowIndex) {
        const std::vector<int32_t>& row = rowColumns[nRowIndex];
        SkScalar rowHeight = 0;
        const size_t nColumnCount = row.size();
        for (size_t nColumnIndex = 0; nColumnIndex < nColumnCount; ++nColumnIndex) {
            const size_t nCharIndex = (size_t)row[nColumnIndex];
            ASSERT(nCharIndex < charRects.size());
            if (nCharIndex < charRects.size()) {
                //Draw one character           
                const THorizontalChar& horizontalChar = charRects[nCharIndex];
                rowHeight = std::max(rowHeight, horizontalChar.size.height());
            }
        }
        rowHeights[nRowIndex] = (float)rowHeight;
    }

    //Detect an empty row that contains only a newline character
    const float fMinValue = 0.001f;
    float lastRowHeight = 0;
    for (size_t nRowIndex = 0; nRowIndex < nRowCount; ++nRowIndex) {
        float& rowHeight = rowHeights[nRowIndex];
        if (rowHeight < fMinValue) {
            if (lastRowHeight > fMinValue) {
                rowHeight = lastRowHeight;
            }
            else {
                for (size_t j = nRowIndex + 1; j < nRowCount; ++j) {
                    if (rowHeights[j] > fMinValue) {
                        rowHeight = rowHeights[j];
                        break;
                    }
                }
            }
        }
        else {
            lastRowHeight = rowHeight;
        }
    }

    // If all are empty rows, recalculate (fallback handling)
    for (size_t nRowIndex = 0; nRowIndex < nRowCount; ++nRowIndex) {
        if (rowHeights[nRowIndex] < fMinValue) {
            rowHeights[nRowIndex] = fDefaultCharHeight;
        }
    }

    // Calculate the total height
    SkScalar maxY = 0;
    for (size_t nRowIndex = 0; nRowIndex < nRowCount; ++nRowIndex) {
        float fHeight = rowHeights[nRowIndex];
        if (nRowIndex != 0) {
            fHeight = fHeight * fSpacingMul + fSpacingAdd;
        }
        maxY += fHeight;
    }
    maxX = std::max(maxX, fDefaultCharWidth); //When there are only newline characters and no other characters, the default character width must be used

    if (pRowColumns != nullptr) {
        pRowColumns->swap(rowColumns);
    }
    if (pRowHeights != nullptr) {
        pRowHeights->swap(rowHeights);
    }
    if (pRowWidths != nullptr) {
        pRowWidths->swap(rowWidths);
    }
    return SkRect::MakeWH(maxX, maxY);
}

float HorizontalDrawText::CalculateDefaultCharWidth(const SkFont* pSkFont, const SkPaint* skPaint) const
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

UiRect HorizontalDrawText::MeasureString(const DString& strText, const MeasureStringParam& measureParam)
{
    PerformanceStat statPerformance(_T("HorizontalDrawText::MeasureString"));
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

    // Calculate the font height (used as the character row height)
    SkFontMetrics fontMetrics;
    const SkScalar fFontHeight = pSkFont->getMetrics(&fontMetrics);
    ASSERT(fFontHeight > 0);
    if (fFontHeight <= 0) {
        return UiRect();
    }

    //Text is always drawn using UTF16 encoding
    const UTF16String textUTF16 = GetDrawStringUTF16(strText, bSingleLineMode);

    std::vector<THorizontalChar> charRects;
    if (!CalculateTextCharBounds(textUTF16, pSkFont, &skPaint, (float)fFontHeight, charRects)) {
        return UiRect();
    }
    ASSERT(charRects.size() == textUTF16.size());
    if (charRects.size() != textUTF16.size()) {
        return UiRect();
    }

    //The default character width
    float fDefaultCharWidth = CalculateDefaultCharWidth(pSkFont, &skPaint);
    SkRect skTextBounds = CalculateHorizontalTextBounds(charRects, measureParam.rectSize, bSingleLineMode,
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

void HorizontalDrawText::DrawString(const DString& strText, const DrawStringParam& drawParam)
{
    // Note: horizontal text drawing does not support the following features
    // 1. Text style: DrawStringFormat::TEXT_PATH_ELLIPSIS is not supported; it is treated as DrawStringFormat::TEXT_END_ELLIPSIS
    PerformanceStat statPerformance(_T("HorizontalDrawText::DrawString"));
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

    // Drawing area
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

    // Row spacing settings
    float fSpacingMul = drawParam.fSpacingMul;
    float fSpacingAdd = drawParam.fSpacingAdd;
    float fWordHorizontalSpacing = drawParam.fWordSpacing;
    if (fSpacingMul <= 0.01f) {
        fSpacingMul = 1.0f;
    }
    if (fSpacingAdd < 0) {
        fSpacingAdd = 0;
    }
    if (fWordHorizontalSpacing < 0) {
        fWordHorizontalSpacing = 0;
    }

    //Text is always drawn using UTF16 encoding
    const UTF16String textUTF16 = GetDrawStringUTF16(strText, bSingleLineMode);

    std::vector<THorizontalChar> charRects;
    if (!CalculateTextCharBounds(textUTF16, pSkFont, &skPaint, (float)fFontHeight, charRects)) {
        return;
    }
    ASSERT(charRects.size() == textUTF16.size());
    if (charRects.size() != textUTF16.size()) {
        return;
    }

    // The default character width
    float fDefaultCharWidth = CalculateDefaultCharWidth(pSkFont, &skPaint);

    std::vector<std::vector<int32_t>> rowColumns;
    std::vector<float> rowHeights;
    std::vector<float> rowWidths;
    SkRect skTextBounds = CalculateHorizontalTextBounds(charRects, drawParam.textRect.Width(), bSingleLineMode,
                                                        fSpacingMul, fSpacingAdd, fWordHorizontalSpacing,
                                                        fDefaultCharWidth, fFontHeight,
                                                        &rowColumns, &rowHeights, &rowWidths);

    ASSERT(rowColumns.size() == rowHeights.size());
    if (rowColumns.size() != rowHeights.size()) {
        return;
    }

    //Record the drawing position of each character; alignment will be handled later
    struct TDrawCharPos
    {
        DUTF16Char ch = 0;          //Character
        int32_t nRowIndex = 0;      //Row number
        int32_t nColumnIndex = 0;   //Column number
        SkScalar xPos = 0;          //The X coordinate when drawing
        SkScalar yPos = 0;          //The Y coordinate when drawing
        int32_t chWidth = 0;        //Character width (used for drawing strikethrough and underline)
        bool bDrew = false;         //Whether the character has been drawn (used for drawing strikethrough and underline)
    };
    std::vector<TDrawCharPos> drawCharPos;
    drawCharPos.reserve(charRects.size());

    // The number of characters in each row
    std::vector<int32_t> rowColumnCount;
    rowColumnCount.resize(rowColumns.size(), 0);

    // Calculate the drawing positions (drawn row by row)
    SkScalar yPos = rcSkDest.top();
    const size_t nRowCount = rowColumns.size();
    for (size_t nRowIndex = 0; nRowIndex < nRowCount; ++nRowIndex) {
        float fRowHeight = rowHeights[nRowIndex]; //Row height (pre-calculated)
        SkScalar xPos = rcSkDest.left();
        const std::vector<int32_t>& column = rowColumns[nRowIndex];
        const size_t nColCount = column.size();
        rowColumnCount[nRowIndex] = (int32_t)nColCount;//Record how many characters are in each row

        for (size_t nColIndex = 0; nColIndex < nColCount; ++nColIndex) {
            const size_t nCharIndex = (size_t)column[nColIndex];
            ASSERT(nCharIndex < charRects.size());
            if (nCharIndex >= charRects.size()) {
                continue;
            }
            //Draw one character
            TDrawCharPos& charPos = drawCharPos.emplace_back(); //Add a new character
            const THorizontalChar& horizontalChar = charRects[nCharIndex];

            //Record the drawing position of the character; draw it after handling alignment
            charPos.ch = horizontalChar.ch;
            charPos.nRowIndex = (int32_t)nRowIndex;
            charPos.nColumnIndex = (int32_t)nColIndex;
            charPos.chWidth = (int32_t)horizontalChar.size.width();

            // Horizontal position
            charPos.xPos = xPos + (horizontalChar.bounds.left() < 0 ? -horizontalChar.bounds.left() : 0);

            // Vertical position; all fonts are the same height
            charPos.yPos = yPos - fontMetrics.fAscent;

            // Update the X position of the next character
            xPos += (horizontalChar.size.width() + fWordHorizontalSpacing);
        }

        // Update the Y position of the next row
        yPos += (fRowHeight * fSpacingMul + fSpacingAdd);
    }

    // Vertical alignment: all drawn content is aligned as a whole
    int32_t textBoundHeight = SkScalarTruncToInt(skTextBounds.height() + 0.5f);
    if (textBoundHeight < drawParam.textRect.Height()) {
        int32_t nOffsetY = 0;
        if (drawParam.uFormat & DrawStringFormat::TEXT_VCENTER) {
            // Vertical alignment: center (shift down)
            nOffsetY = (drawParam.textRect.Height() - textBoundHeight) / 2;
        }
        else if (drawParam.uFormat & DrawStringFormat::TEXT_BOTTOM) {
            // Vertical alignment: bottom (shift down)
            nOffsetY = (drawParam.textRect.Height() - textBoundHeight);
        }
        else {
            //Vertical alignment: top (default, no handling)
            nOffsetY = 0;
        }
        if (nOffsetY != 0) {
            for (TDrawCharPos& charPos : drawCharPos) {
                charPos.yPos += nOffsetY;
            }
        }
    }

    //Horizontal alignment: aligned by the actual area occupied by each row
    std::vector<float> rowOffsets;
    if (drawParam.uFormat & DrawStringFormat::TEXT_HCENTER) {
        //Horizontal alignment: center
        rowOffsets.resize(rowWidths.size(), 0.0f);
        int32_t textRectWidth = drawParam.textRect.Width();
        for (size_t nRowIndex = 0; nRowIndex < rowWidths.size(); ++nRowIndex) {
            if (rowWidths[nRowIndex] < textRectWidth) {
                rowOffsets[nRowIndex] = (textRectWidth - rowWidths[nRowIndex]) / 2;
            }
        }
    }
    else if (drawParam.uFormat & DrawStringFormat::TEXT_RIGHT) {
        //Horizontal alignment: right
        rowOffsets.resize(rowWidths.size(), 0.0f);
        int32_t textRectWidth = drawParam.textRect.Width();
        for (size_t nRowIndex = 0; nRowIndex < rowWidths.size(); ++nRowIndex) {
            if (rowWidths[nRowIndex] < textRectWidth) {
                rowOffsets[nRowIndex] = (textRectWidth - rowWidths[nRowIndex]);
            }
        }
    }
    else if (drawParam.uFormat & DrawStringFormat::TEXT_HJUSTIFY) {
        //Horizontal alignment: justify
        ASSERT(rowColumnCount.size() == rowWidths.size());
        if (!rowColumnCount.empty() && rowColumnCount.size() == rowWidths.size()) {
            std::vector<std::vector<float>> rowColOffsets; //The offset of each character
            rowColOffsets.resize(rowWidths.size());

            int32_t textRectWidth = drawParam.textRect.Width();
            for (size_t nRowIndex = 0; nRowIndex < rowWidths.size(); ++nRowIndex) {
                const int32_t nColCount = rowColumnCount[nRowIndex];
                // Calculate the offset of each character
                rowColOffsets[nRowIndex].resize(nColCount, 0.0f);

                if (nColCount < 2) {
                    //This row has fewer than 2 characters, no justification needed
                    continue;
                }

                if (rowWidths[nRowIndex] < textRectWidth) {
                    float fOffsetTotal = (textRectWidth - rowWidths[nRowIndex]); //The total offset
                    float fOffset = fOffsetTotal / (nColCount - 1); //The offset of each character

                    for (int32_t nCol = 0; nCol < nColCount; ++nCol) {
                        rowColOffsets[nRowIndex][nCol] = fOffset * nCol;
                    }
                }
            }

            //Set the horizontal offset for each character
            const int32_t nOffsetRowCount = (int32_t)rowColOffsets.size();
            for (TDrawCharPos& charPos : drawCharPos) {
                ASSERT(charPos.nRowIndex < nOffsetRowCount);
                if (charPos.nRowIndex < nOffsetRowCount) {
                    const std::vector<float>& colOffsets = rowColOffsets[charPos.nRowIndex];
                    ASSERT(charPos.nColumnIndex < (int32_t)colOffsets.size());
                    if (charPos.nColumnIndex < (int32_t)colOffsets.size()) {
                        charPos.xPos += colOffsets[charPos.nColumnIndex];
                    }
                }
            }
        }
    }
    else {
        //Horizontal alignment: left (default, no handling)
    }

    if (rowOffsets.size() == rowWidths.size()) {
        const size_t nOffsetCount = rowOffsets.size();
        for (TDrawCharPos& charPos : drawCharPos) {
            if (charPos.nRowIndex < (int32_t)nOffsetCount) {
                charPos.xPos += rowOffsets[charPos.nRowIndex];
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
        if (bSingleLineMode && charPos.nRowIndex != 0) {
            //Single-line mode: draw only one row
            break;
        }

        if (bNeedEllipsis && (charIndex < (drawCharCount - 1))) {// Business logic for whether to draw "..."
            //Determine whether the next character is out of bounds (horizontal)
            float fRowHeight = 0;
            if (charPos.nRowIndex < (int32_t)rowHeights.size()) {
                fRowHeight = rowHeights[charPos.nRowIndex]; //Row height
            }
            bool bNeedDrawEllipsis = false;
            const TDrawCharPos& nextCharPos = drawCharPos[charIndex + 1];
            if (nextCharPos.nRowIndex == charPos.nRowIndex) {
                SkScalar xPos = nextCharPos.xPos;
                xPos += nextCharPos.chWidth;
                if ((xPos > (SkScalar)rcSkDest.fRight) &&
                    (bSingleLineMode || (charPos.yPos + fRowHeight) > (SkScalar)rcSkDest.fBottom)) {
                    //The next character cannot be displayed
                    bNeedDrawEllipsis = true;
                }
            }
            else if (!bSingleLineMode && (charPos.yPos + fRowHeight) > (SkScalar)rcSkDest.fBottom) {
                //The data of other rows below cannot be displayed
                bNeedDrawEllipsis = true;
            }

            if (bNeedDrawEllipsis) {
                //Character out of bounds, draw "..."
                std::string ellipsis = "...";
                skCanvas->drawSimpleText(ellipsis.data(), ellipsis.size(), SkTextEncoding::kUTF8,
                                         charPos.xPos, charPos.yPos,
                                         *pSkFont, skPaint);
                //Stop drawing after going out of bounds
                break;
            }
        }

        if (!skTextRect.contains(charPos.xPos, charPos.yPos)) {
            //The current character is not in the text display area; do not draw it
            continue;
        }

        charPos.bDrew = true;
        skCanvas->drawSimpleText(&charPos.ch, sizeof(charPos.ch), SkTextEncoding::kUTF16,
                                 charPos.xPos, charPos.yPos,
                                 *pSkFont, skPaint);
    }

    // Draw underline/strikethrough
    if ((drawParam.pFont->IsUnderline() || drawParam.pFont->IsStrikeOut()) && !rowHeights.empty()) {
        struct TextRowInfo
        {
            UiPoint ptStart;
            UiPoint ptEnd;
            bool bStartFlag = false;
            bool bEndFlag = false;
            int32_t chStartWidth = 0;
            int32_t chEndWidth = 0;
        };
        std::vector<TextRowInfo> textRowInfo;
        textRowInfo.resize(rowHeights.size());

        for (const TDrawCharPos& charPos : drawCharPos) {
            if (!charPos.bDrew) {
                continue;
            }
            ASSERT(charPos.nRowIndex <= (int32_t)textRowInfo.size());
            if (charPos.nRowIndex >= (int32_t)textRowInfo.size()) {
                continue;
            }
            TextRowInfo& textRow = textRowInfo[charPos.nRowIndex];
            if (!textRow.bStartFlag) {
                textRow.bStartFlag = true;
                textRow.ptStart.x = (int32_t)charPos.xPos;
                textRow.ptStart.y = (int32_t)(charPos.yPos + 0.5f);
                textRow.chStartWidth = charPos.chWidth;
            }
            else {
                textRow.bEndFlag = true;
                textRow.ptEnd.x = (int32_t)charPos.xPos;
                textRow.ptEnd.y = (int32_t)charPos.yPos;
                textRow.chEndWidth = charPos.chWidth;
            }
        }

        // Draw lines
        for (size_t nRowIndex = 0; nRowIndex < textRowInfo.size(); ++nRowIndex) {
            const TextRowInfo& textRow = textRowInfo[nRowIndex];
            if (!textRow.bStartFlag && !textRow.bEndFlag) {
                continue;
            }
            SkScalar left = (SkScalar)textRow.ptStart.x;
            SkScalar right = left + (SkScalar)textRow.chStartWidth;
            if (textRow.bEndFlag) {
                right = (SkScalar)textRow.ptEnd.x + (SkScalar)textRow.chEndWidth;
            }
            SkScalar width = right - left;

            SkScalar x = (SkScalar)textRow.ptStart.x;
            SkScalar y = (SkScalar)textRow.ptStart.y;
            if (textRow.bEndFlag) {
                y = ((SkScalar)textRow.ptStart.y + (SkScalar)textRow.ptEnd.y) / 2 ;
            }

            // Default fraction of the text size to use for a strike-through or underline.
            static constexpr SkScalar kLineThicknessFactor = (SK_Scalar1 / 18);
            // Fraction of the text size to raise the center of a strike-through line above
            // the baseline.
            const SkScalar kStrikeThroughOffset = (SK_Scalar1 * 65 / 252);
            // Fraction of the text size to lower an underline below the baseline.
            const SkScalar kUnderlineOffset = (SK_Scalar1 / 9);

            if (drawParam.pFont->IsStrikeOut()) {
                //Draw the strikethrough
                SkScalar thickness_factor = kLineThicknessFactor;
                const SkScalar text_size = pSkFont->getSize();
                const SkScalar height = text_size * thickness_factor;
                const SkScalar top = y - text_size * kStrikeThroughOffset - height / 2;
                SkScalar x_scalar = SkIntToScalar(x);
                const SkRect r = SkRect::MakeLTRB(x_scalar, top, x_scalar + width, top + height);
                skCanvas->drawRect(r, skPaint);
            }
            if (drawParam.pFont->IsUnderline()) {
                //Draw the underline
                SkScalar thickness_factor = 1.5;
                SkScalar x_scalar = SkIntToScalar(x);
                const SkScalar text_size = pSkFont->getSize();
                SkRect r = SkRect::MakeLTRB(
                    x_scalar, y + text_size * kUnderlineOffset, x_scalar + width,
                    y + (text_size *
                        (kUnderlineOffset +
                            (thickness_factor * kLineThicknessFactor))));
                skCanvas->drawRect(r, skPaint);
            }
        }
    }
}

} // namespace ui
