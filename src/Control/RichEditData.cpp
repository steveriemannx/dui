#include "duilib/Control/RichEditData.h"
#include "duilib/Utils/PerformanceUtil.h"
#include <unordered_set>

namespace ui
{
RichEditData::RichEditData(IRichTextData* pRichTextData):
    m_pRichText(pRichTextData),
    m_hAlignType(HorAlignType::kAlignLeft),
    m_vAlignType(VerAlignType::kAlignTop),
    m_bSingleLineMode(true),
    m_pRender(nullptr),
    m_pRenderFactory(nullptr),
    m_bCacheDirty(true),
    m_nUndoLimit(64),
    m_bTextRectYOffsetUpdated(false),
    m_bTextRectXOffsetUpdated(false)
{
    ASSERT(pRichTextData != nullptr);

}

RichEditData::~RichEditData()
{
}

void RichEditData::SetRender(IRender* pRender)
{
    m_pRender = pRender;
}

void RichEditData::SetRenderFactory(IRenderFactory* pRenderFactory)
{
    m_pRenderFactory = pRenderFactory;
}

void RichEditData::SetTextDrawRect(const UiRect& rcTextDrawRect, bool bCheckDirty)
{
    if (m_rcTextDrawRect != rcTextDrawRect) {
        //When the width changes, the coordinates of the text need to be recalculated
        if (bCheckDirty && (m_rcTextDrawRect.Width() != rcTextDrawRect.Width())) {
            SetCacheDirty(true);
        }
        m_rcTextDrawRect = rcTextDrawRect;
    }
}

void RichEditData::SetScrollOffset(const UiSize& szScrollOffset)
{
    //When the scrollbar position changes, no recalculation is needed
    m_szScrollOffset = szScrollOffset;
}

void RichEditData::SetTextHAlignType(HorAlignType hAlignType)
{
    if (m_hAlignType != hAlignType) {
        m_hAlignType = hAlignType;
        SetCacheDirty(true);
    }
}

HorAlignType RichEditData::GetHAlignType() const
{
    return m_hAlignType;
}

void RichEditData::SetTextVAlignType(VerAlignType vAlignType)
{
    if (m_vAlignType != vAlignType) {
        m_vAlignType = vAlignType;
        SetCacheDirty(true);
    }
}

VerAlignType RichEditData::GetVAlignType() const
{
    return m_vAlignType;
}

void RichEditData::UnionRect(UiRect& rect, const UiRect& r) const
{
    if ((r.left >= r.right) && (r.top >= r.bottom)) {
        //r is an empty rectangle
        return;
    }
    if ((rect.left >= rect.right) && (rect.top >= rect.bottom)) {
        //Self is an empty rectangle
        rect = r;
    }
    else {
        //Merge the rectangles horizontally and vertically separately
        if (r.right > r.left) {
            if (rect.right > rect.left) {
                //Both are not empty
                rect.left = (std::min)(rect.left, r.left);
                rect.top = (std::min)(rect.top, r.top);
            }
            else {
                //Self is empty
                rect.left = r.left;
                rect.top  = r.top;
            }
        }
        if (r.bottom > r.top) {
            if (rect.bottom > rect.top) {
                //Both are not empty
                rect.right = (std::max)(rect.right, r.right);
                rect.bottom = (std::max)(rect.bottom, r.bottom);
            }
            else {
                //Self is empty
                rect.right = r.right;
                rect.bottom = r.bottom;
            }
        }
    }
}

void RichEditData::UnionRectF(UiRectF& rect, const UiRectF& r) const
{
    if ((r.left >= r.right) && (r.top >= r.bottom)) {
        //r is an empty rectangle
        return;
    }
    if ((rect.left >= rect.right) && (rect.top >= rect.bottom)) {
        //Self is an empty rectangle
        rect = r;
    }
    else {
        //Merge the rectangles horizontally and vertically separately
        if (r.right > r.left) {
            if (rect.right > rect.left) {
                //Both are not empty
                rect.left = (std::min)(rect.left, r.left);
                rect.top = (std::min)(rect.top, r.top);
            }
            else {
                //Self is empty
                rect.left = r.left;
                rect.top = r.top;
            }
        }
        if (r.bottom > r.top) {
            if (rect.bottom > rect.top) {
                //Both are not empty
                rect.right = (std::max)(rect.right, r.right);
                rect.bottom = (std::max)(rect.bottom, r.bottom);
            }
            else {
                //Self is empty
                rect.right = r.right;
                rect.bottom = r.bottom;
            }
        }
    }
}

UiRect RichEditData::EstimateTextDisplayBounds(const UiRect& rcAvailable)
{
    UiRect rect;
    ASSERT(m_pRender != nullptr);
    if (m_pRender == nullptr) {
        return rect;
    }
    ASSERT(m_pRenderFactory != nullptr);
    if (m_pRenderFactory == nullptr) {
        return rect;
    }
    ASSERT(m_pRichText != nullptr);
    if (m_pRichText == nullptr) {
        return rect;
    }

    UiRect rcDrawRect = m_pRichText->GetRichTextDrawRect();
    if (rcAvailable.Width() == rcDrawRect.Width()) {
        //Check and calculate the character positions
        CheckCalcTextRects();
        rect = GetTextRect();
    }
    else {
        //Re-estimate
        std::vector<std::wstring_view> textView;
        GetTextView(textView);
        if (!textView.empty()) {
            bool bFirst = true;
            std::vector<RichTextData> richTextDataList;
            if (m_pRichText->GetRichTextForDraw(textView, richTextDataList)) {
                std::vector<std::vector<UiRect>> richTextRects;
                m_pRender->MeasureRichText(rcAvailable, UiSize(), m_pRenderFactory, richTextDataList, &richTextRects);
                for (const std::vector<UiRect>& data : richTextRects) {
                    for (const UiRect& textRect : data) {
                        if (bFirst) {
                            //The first one
                            rect = textRect;
                            bFirst = false;
                        }
                        else {
                            UnionRect(rect, textRect);
                        }                        
                    }
                }
            }
        }
    }
    return rect;
}

void RichEditData::CalcCacheTextRects(UiRect& rcTextRect)
{
    rcTextRect.Clear();
    UiRectF rowRects;
    bool bFirst = true;
    for (RichTextLineInfoPtr& pLineInfo : m_lineTextInfo) {
        ASSERT(pLineInfo != nullptr);
        const size_t nRowCount = pLineInfo->m_rowInfo.size();
        for (size_t nRow = 0; nRow < nRowCount; ++nRow) {
            const UiRectF& rowRect = pLineInfo->m_rowInfo[nRow]->m_rowRect;
            if (bFirst) {
                rowRects = rowRect;
                bFirst = false;
            }
            else {
                UnionRectF(rowRects, rowRect);
            }
        }
    }
    rcTextRect.left = (int32_t)rowRects.left;
    rcTextRect.right = (int32_t)(ui::CEILF(rowRects.right));
    rcTextRect.top = (int32_t)rowRects.top;
    rcTextRect.bottom = (int32_t)(ui::CEILF(rowRects.bottom));
}

void RichEditData::UpdateRowTextOffsetY(RichTextLineInfoList& lineTextInfo, int32_t nOffsetY) const
{
    float fRowHeight = 0;
    float fLastRowBottom = 0;    
    bool bFirstRow = true;
    for (RichTextLineInfoPtr& pLineInfo : lineTextInfo) {
        ASSERT(pLineInfo != nullptr);
        const size_t nRowCount = pLineInfo->m_rowInfo.size();
        for (size_t nRow = 0; nRow < nRowCount; ++nRow) {
            UiRectF& rowRect = pLineInfo->m_rowInfo[nRow]->m_rowRect;
            if (bFirstRow) {
                //The first row
                fRowHeight = rowRect.Height();
                rowRect.top = (float)nOffsetY;
                rowRect.bottom = rowRect.top + fRowHeight;
                fLastRowBottom = rowRect.bottom;
                bFirstRow = false;
            }
            else {
                fRowHeight = rowRect.Height();
                rowRect.top = fLastRowBottom;
                rowRect.bottom = rowRect.top + fRowHeight;
                fLastRowBottom = rowRect.bottom;
            }
        }
    }
}

void RichEditData::UpdateRowTextOffsetX(RichTextLineInfoList& lineTextInfo, HorAlignType hAlignType, std::vector<int32_t>& rowXOffset, bool& bTextRectXOffsetUpdated) const
{
    rowXOffset.clear();
    if (!bTextRectXOffsetUpdated && (hAlignType == HorAlignType::kAlignLeft)) {
        //No update needed
        return;
    }
    bTextRectXOffsetUpdated = false;
    const int32_t nDrawRectWidth = m_rcTextDrawRect.Width();//The total width of the rectangle
    for (RichTextLineInfoPtr& pLineInfo : lineTextInfo) {
        ASSERT(pLineInfo != nullptr);
        const size_t nRowCount = pLineInfo->m_rowInfo.size();
        for (size_t nRow = 0; nRow < nRowCount; ++nRow) {
            RichTextRowInfo& rowInfo = *pLineInfo->m_rowInfo[nRow];
            UiRectF& rowRect = rowInfo.m_rowRect;
            if (rowInfo.m_xOffset > 0) {
                //Restore
                rowRect.Offset(-(float)rowInfo.m_xOffset, 0.0f);
                rowInfo.m_xOffset = 0;
            }
            if (rowRect.Width() < nDrawRectWidth) {
                if (hAlignType == HorAlignType::kAlignCenter) {
                    //Center aligned
                    float diff = nDrawRectWidth - rowRect.Width();
                    rowInfo.m_xOffset = (int32_t)(diff / 2);
                    if (rowInfo.m_xOffset > 0) {
                        rowRect.Offset((float)rowInfo.m_xOffset, 0.0f);
                        bTextRectXOffsetUpdated = true;
                    }
                }
                else if (hAlignType == HorAlignType::kAlignRight) {
                    //Right aligned
                    float diff = nDrawRectWidth - rowRect.Width();
                    rowInfo.m_xOffset = (int32_t)diff;
                    if (rowInfo.m_xOffset > 0) {
                        rowRect.Offset((float)rowInfo.m_xOffset, 0.0f);
                        bTextRectXOffsetUpdated = true;
                    }
                }
            }
            ASSERT(rowInfo.m_xOffset >= 0);
            rowXOffset.push_back(rowInfo.m_xOffset);
        }
    }
    if (!bTextRectXOffsetUpdated) {
        std::vector<int32_t> temp;
        rowXOffset.swap(temp);
    }
}

int32_t RichEditData::GetTextRectOfssetY() const
{
    int32_t yOffset = 0;
    if (m_rcTextRect.Height() < m_rcTextDrawRect.Height()) {
        VerAlignType vAlignType = GetVAlignType();
        if (vAlignType == VerAlignType::kAlignCenter) {
            //Center aligned
            int32_t nDiff = m_rcTextDrawRect.Height() - m_rcTextRect.Height();
            yOffset = nDiff / 2;
        }
        else if (vAlignType == VerAlignType::kAlignBottom) {
            //Bottom aligned
            int32_t nDiff = m_rcTextDrawRect.Height() - m_rcTextRect.Height();
            yOffset = nDiff;
        }
    }    
    return yOffset;
}

const std::vector<int32_t>& RichEditData::GetTextRowXOffset() const
{
    return m_rowXOffset;
}

const UiRect& RichEditData::GetTextRect() const
{
    return m_rcTextRect;
}

void RichEditData::CheckCalcTextRects()
{
    SetTextDrawRect(m_pRichText->GetRichTextDrawRect(), true);
    if (m_bCacheDirty) {
        CalcTextRects();
        SetCacheDirty(false);
        m_pRichText->OnTextRectsChanged();
    }
}

void RichEditData::CalcTextRects()
{
    PerformanceStat statPerformance(_T("RichEditData::CalcTextRects"));
    //Clear the cache data of all rows
    for (RichTextLineInfoPtr& pLineInfo : m_lineTextInfo) {
        ASSERT(pLineInfo != nullptr);
        pLineInfo->m_rowInfo.clear();
    }
    m_rcTextRect.Clear();

    ASSERT(m_pRender != nullptr);
    if (m_pRender == nullptr) {
        return;
    }
    ASSERT(m_pRenderFactory != nullptr);
    if (m_pRenderFactory == nullptr) {
        return;
    }
    ASSERT(m_pRichText != nullptr);
    if (m_pRichText == nullptr) {
        return;
    }

    std::vector<std::wstring_view> textView;
    GetTextView(textView);
    if (textView.empty()) {
        return;
    }

    size_t nTextLen = 0;
    for (const std::wstring_view& text : textView) {
        nTextLen += text.size();
    }
    if (nTextLen == 0) {
        return;
    }

    UiRect rcDrawText = m_pRichText->GetRichTextDrawRect();
    if (rcDrawText.IsEmpty()) {
        std::vector<int32_t> temp;
        m_rowXOffset.swap(temp);
        m_bTextRectXOffsetUpdated = false;
        m_bTextRectYOffsetUpdated = false;
        return;
    }
    //When estimating, the scrollbar position is always (0,0)
    UiSize szScrollOffset;
    RichTextLineInfoParam lineInfoParam;
    lineInfoParam.m_nStartLineIndex = 0;
    lineInfoParam.m_nStartRowIndex = 0;
    lineInfoParam.m_pLineInfoList = &m_lineTextInfo;

    //Draw all data, clear the row data information
    std::vector<RichTextData> richTextDataList;
    m_pRichText->GetRichTextForDraw(textView, richTextDataList);
    if (richTextDataList.empty()) {
        return;
    }
    m_spDrawRichTextCache.reset();
    if (m_pRichText->IsTextPasswordMode()) {
        //In password mode, do not use the drawing cache
        ASSERT(richTextDataList.size() == 1);
        if (richTextDataList.size() == 1) {
            RichTextData& richTextData = richTextDataList.front();
            DStringW text = GetText();
            m_pRichText->ReplacePasswordChar(text);
            ASSERT(!text.empty());
            richTextData.m_textView = text;
            m_pRender->MeasureRichText2(rcDrawText, szScrollOffset, m_pRenderFactory, richTextDataList, &lineInfoParam, nullptr);
            richTextData.m_textView = std::wstring_view();
        }
        else {
            m_pRender->MeasureRichText2(rcDrawText, szScrollOffset, m_pRenderFactory, richTextDataList, &lineInfoParam, nullptr);
        }
    }
    else {
        m_pRender->MeasureRichText3(rcDrawText, szScrollOffset, m_pRenderFactory, richTextDataList, &lineInfoParam, m_spDrawRichTextCache, nullptr);
    }    
    SetTextDrawRect(rcDrawText, false);
    CalcCacheTextRects(m_rcTextRect);

    m_bTextRectYOffsetUpdated = false;    
    int32_t nOffsetY = GetTextRectOfssetY();
    if (nOffsetY > 0) {
        UpdateRowTextOffsetY(m_lineTextInfo, nOffsetY);
        m_bTextRectYOffsetUpdated = true;
    }

    UpdateRowTextOffsetX(m_lineTextInfo, GetHAlignType(), m_rowXOffset, m_bTextRectXOffsetUpdated);
}

void RichEditData::CalcTextRects(size_t nStartLine,
                                 const std::vector<size_t>& modifiedLines,
                                 const std::vector<size_t>& deletedLines,
                                 size_t nDeletedRows)
{
    PerformanceStat statPerformance(_T("RichEditData::CalcTextRects2"));
    ASSERT(!m_pRichText->IsTextPasswordMode());//In password mode, this function should not be used
    if (nStartLine != (size_t)-1) {
        ASSERT(!modifiedLines.empty() || !deletedLines.empty());
        if (!modifiedLines.empty()) {
            ASSERT(modifiedLines[0] == nStartLine);
            if (modifiedLines[0] != nStartLine) {
                nStartLine = (size_t)-1;
            }
        }
        else if (!deletedLines.empty()) {
            ASSERT(deletedLines[0] == nStartLine);
            if (deletedLines[0] != nStartLine) {
                nStartLine = (size_t)-1;
            }
        }
    }
    ASSERT(nStartLine != (size_t)-1);
    ASSERT(nStartLine <= m_lineTextInfo.size());
    if (nStartLine > m_lineTextInfo.size()) {
        return;
    }
    if (nStartLine == m_lineTextInfo.size()) {
        //The case of deleting the last row
        ASSERT(modifiedLines.empty());
    }

    //Draw the changed data, clear the related row data information
    if (!modifiedLines.empty()) {
        //Rows that have been modified
        size_t nLineIndex = 0;
        const size_t nCount = modifiedLines.size();
        for (size_t nIndex = 0; nIndex < nCount; ++nIndex) {
            nLineIndex = modifiedLines[nIndex];
            ASSERT(nLineIndex < m_lineTextInfo.size());
            if (nLineIndex < m_lineTextInfo.size()) {
                RichTextLineInfoPtr& pLineInfo = m_lineTextInfo[nLineIndex];
                ASSERT(pLineInfo != nullptr);
                pLineInfo->m_rowInfo.clear();
            }
            else {
                //Data error encountered
                return;
            }
        }
    }

    ASSERT(m_pRender != nullptr);
    if (m_pRender == nullptr) {
        return;
    }
    ASSERT(m_pRenderFactory != nullptr);
    if (m_pRenderFactory == nullptr) {
        return;
    }
    ASSERT(m_pRichText != nullptr);
    if (m_pRichText == nullptr) {
        return;
    }

    std::vector<std::wstring_view> textView;
    GetTextView(textView);
    if (textView.empty()) {
        return;
    }

    size_t nTextLen = 0;
    for (const std::wstring_view& text : textView) {
        nTextLen += text.size();
    }
    if (nTextLen == 0) {
        return;
    }

    UiRect rcDrawText = m_pRichText->GetRichTextDrawRect();
    if (rcDrawText.IsEmpty()) {
        std::vector<int32_t> temp;
        m_rowXOffset.swap(temp);
        m_bTextRectXOffsetUpdated = false;
        m_bTextRectYOffsetUpdated = false;
        return;
    }

    //When estimating, the scrollbar position is always (0,0)
    UiSize szScrollOffset;
    RichTextLineInfoParam lineInfoParam;
    lineInfoParam.m_nStartLineIndex = (uint32_t)nStartLine;
    lineInfoParam.m_nStartRowIndex = 0;
    lineInfoParam.m_pLineInfoList = &m_lineTextInfo;
    if (nStartLine > 0) {
        //Calculate the starting logical line number
        for (size_t nLine = 0; nLine < nStartLine; ++nLine) {
            const RichTextLineInfo& lineInfo = *m_lineTextInfo[nLine];
            lineInfoParam.m_nStartRowIndex += (uint32_t)lineInfo.m_rowInfo.size();
        }
    }

    //The current latest data to be drawn
    std::vector<RichTextData> richTextDataListAll;
    if (m_spDrawRichTextCache != nullptr) {
        //Compare whether the drawing cache is invalid; if invalid, clear it
        m_pRichText->GetRichTextForDraw(textView, richTextDataListAll);
    }
    //The drawing cache regenerated from the modified text
    std::shared_ptr<DrawRichTextCache> spDrawRichTextCacheUpdated;

    size_t nModifiedRows = 0;//The modified text, how many rows (logical rows) it is split into after calculation
    if (!modifiedLines.empty()) {
        //Rows have been modified, recalculate the row data
        std::vector<RichTextData> richTextDataListModified;
        m_pRichText->GetRichTextForDraw(textView, richTextDataListModified, nStartLine, modifiedLines);
        if (richTextDataListModified.empty()) {
            return;
        }
        m_pRender->MeasureRichText3(rcDrawText, szScrollOffset, m_pRenderFactory, richTextDataListModified, &lineInfoParam, spDrawRichTextCacheUpdated, nullptr);
        std::unordered_set<uint32_t> modifiedLineSet;
        for (size_t nLine : modifiedLines) {
            modifiedLineSet.insert((uint32_t)nLine);
        }
        const size_t nLineCount = m_lineTextInfo.size();
        for (uint32_t nLine = 0; nLine < nLineCount; ++nLine) {
            if (modifiedLineSet.find(nLine) != modifiedLineSet.end()) {
                const RichTextLineInfo& lineInfo = *m_lineTextInfo[nLine];
                nModifiedRows += (uint32_t)lineInfo.m_rowInfo.size();
            }
        }
    }

    //After drawing, the row height data after incremental drawing
    if (nStartLine < m_lineTextInfo.size()) {
        UpdateRowInfo(nStartLine);
    }    
    if (m_bTextRectYOffsetUpdated) {
        UpdateRowTextOffsetY(m_lineTextInfo, 0);
    }
    UpdateRowTextOffsetX(m_lineTextInfo, HorAlignType::kAlignLeft, m_rowXOffset, m_bTextRectXOffsetUpdated);
    
    //Update the drawing cache
    if (m_spDrawRichTextCache != nullptr) {
        std::vector<int32_t> rowRectTopList;
        const size_t nLineCount = m_lineTextInfo.size();
        rowRectTopList.reserve(nLineCount);
        for (size_t nLineIndex = 0; nLineIndex < nLineCount; ++nLineIndex) {
            const RichTextLineInfo& lineInfo = *m_lineTextInfo[nLineIndex];
            const size_t nRowCount = lineInfo.m_rowInfo.size();
            for (size_t nRow = 0; nRow < nRowCount; ++nRow) {
                rowRectTopList.push_back((int32_t)lineInfo.m_rowInfo[nRow]->m_rowRect.top);
            }
        }
        if (!m_pRender->UpdateDrawRichTextCache(m_spDrawRichTextCache, spDrawRichTextCacheUpdated, richTextDataListAll,
                                                nStartLine, modifiedLines, nModifiedRows, deletedLines, nDeletedRows, rowRectTopList)) {
            m_spDrawRichTextCache.reset();
        }
        //The data has been returned to the cache and can no longer be used
        richTextDataListAll.clear();
    }
    CalcCacheTextRects(m_rcTextRect);

    m_bTextRectYOffsetUpdated = false;
    const int32_t nOffsetY = GetTextRectOfssetY();
    if (nOffsetY > 0) {
        UpdateRowTextOffsetY(m_lineTextInfo, nOffsetY);
        m_bTextRectYOffsetUpdated = true;
    }
    UpdateRowTextOffsetX(m_lineTextInfo, GetHAlignType(), m_rowXOffset, m_bTextRectXOffsetUpdated);
    
#ifdef _DEBUG
    //Compare whether it is consistent with a full draw
    if (nStartLine != (size_t)-1) {
        std::vector<std::wstring_view> textView2;
        RichTextLineInfoList lineTextInfoList;
        for (RichTextLineInfoPtr& pLineInfo : m_lineTextInfo) {
            ASSERT(pLineInfo != nullptr);
            RichTextLineInfoPtr spLineInfo(new RichTextLineInfo);
            spLineInfo->m_nLineTextLen = pLineInfo->m_nLineTextLen;
            spLineInfo->m_lineText = pLineInfo->m_lineText;
            textView2.push_back(std::wstring_view(spLineInfo->m_lineText.c_str(), spLineInfo->m_nLineTextLen));
            lineTextInfoList.push_back(spLineInfo);
        }
        std::vector<RichTextData> richTextDataList2;
        m_pRichText->GetRichTextForDraw(textView2, richTextDataList2);

        std::shared_ptr<DrawRichTextCache> spDrawRichTextCacheNew;

        RichTextLineInfoParam lineInfoParam2;
        lineInfoParam2.m_pLineInfoList = &lineTextInfoList;
        lineInfoParam2.m_nStartLineIndex = 0;
        lineInfoParam2.m_nStartRowIndex = 0;
        m_pRender->MeasureRichText3(rcDrawText, szScrollOffset, m_pRenderFactory, richTextDataList2, &lineInfoParam2, spDrawRichTextCacheNew, nullptr);

        if (nOffsetY > 0) {
            UpdateRowTextOffsetY(lineTextInfoList, nOffsetY);
        }

        std::vector<int32_t> rowXOffset;
        bool bTextRectXOffsetUpdated = false;
        UpdateRowTextOffsetX(lineTextInfoList, GetHAlignType(), rowXOffset, bTextRectXOffsetUpdated);

        //Compare the consistency of the data; the result of incremental drawing should be the same as the result of full drawing
        ASSERT(lineTextInfoList.size() == m_lineTextInfo.size());
        const size_t nDataCount = lineTextInfoList.size();
        for (size_t nDataIndex = 0; nDataIndex < nDataCount; ++nDataIndex) {
            const RichTextLineInfo& infoOld = *m_lineTextInfo[nDataIndex];
            const RichTextLineInfo& infoNew = *lineTextInfoList[nDataIndex];
            ASSERT(infoOld.m_lineText == infoNew.m_lineText);
            ASSERT(infoOld.m_nLineTextLen == infoNew.m_nLineTextLen);
            ASSERT(infoOld.m_rowInfo.size() == infoNew.m_rowInfo.size());

            const size_t nRowCount = infoOld.m_rowInfo.size();
            for (size_t nRow = 0; nRow < nRowCount; ++nRow) {
                const RichTextRowInfo& rowOld = *infoOld.m_rowInfo[nRow];
                const RichTextRowInfo& rowNew = *infoNew.m_rowInfo[nRow];
                ASSERT(rowOld.m_rowRect == rowNew.m_rowRect);
                ASSERT(rowOld.m_charInfo.size() == rowNew.m_charInfo.size());
                ASSERT(rowOld.m_charInfo == rowNew.m_charInfo);
            }
        }

        //Compare the data of the drawing cache
        if ((m_spDrawRichTextCache != nullptr) && (spDrawRichTextCacheNew != nullptr)) {
            bool bRet = m_pRender->IsDrawRichTextCacheEqual(*m_spDrawRichTextCache, *spDrawRichTextCacheNew);
            ASSERT(bRet);
        }
    }
#endif
}

bool RichEditData::SetText(const DStringW& text)
{
    PerformanceStat statPerformance(_T("RichEditData::SetText"));
    if (text.empty()) {
        Clear();
        return true;
    }

    DStringW textLimit;
    DStringW validText;
    std::vector<std::wstring_view> lineTextViewList;
    int32_t nLimitLength = m_pRichText->GetTextLimitLength();
    if ((nLimitLength > 0) && ((int32_t)text.size() > nLimitLength)){
        //Truncate the string
        textLimit = text;
        if (text.find(L'\0') != DStringW::npos) {
            //If it contains the L'\0' character, it needs to be truncated
            textLimit = text.c_str();
        }
        TruncateLimitText(textLimit, nLimitLength);
        std::wstring_view textView = textLimit;
        SplitLines(textView, lineTextViewList);
    }
    else {
        if (text.find(L'\0') != DStringW::npos) {
            //If it contains the L'\0' character, it needs to be truncated
            validText = text.c_str();
            std::wstring_view textView = validText;
            SplitLines(textView, lineTextViewList);
        }
        else {
            std::wstring_view textView = text;
            SplitLines(textView, lineTextViewList);
        }        
    }
    
    const size_t nLineCount = lineTextViewList.size();
    bool bTextChanged = false;
    if (m_lineTextInfo.size() == lineTextViewList.size()) { //Compare whether the text content has changed
        //First compare the string lengths
        for (size_t nIndex = 0; nIndex < nLineCount; ++nIndex) {
            if (m_lineTextInfo[nIndex]->m_nLineTextLen != lineTextViewList[nIndex].size()) {
                bTextChanged = true;
                break;
            }
        }
        if (!bTextChanged) {
            //If the lengths are all the same, compare the content of the strings
            for (size_t nIndex = 0; nIndex < nLineCount; ++nIndex) {
                if (std::wstring_view(m_lineTextInfo[nIndex]->m_lineText.c_str(), m_lineTextInfo[nIndex]->m_nLineTextLen) != lineTextViewList[nIndex]) {
                    bTextChanged = true;
                    break;
                }
            }
        }
    }
    else {
        bTextChanged = true;
    }
    if (bTextChanged) {
        RichTextLineInfoList lineTextInfo;
        if (nLineCount > 0) {
            lineTextInfo.resize(nLineCount);
            for (size_t nIndex = 0; nIndex < nLineCount; ++nIndex) {
                const std::wstring_view& lineTextView = lineTextViewList[nIndex];
                RichTextLineInfoPtr& lineText = lineTextInfo[nIndex];
                lineText.reset(new RichTextLineInfo);
                lineText->m_lineText = lineTextView; //Copy a copy of the text data and save it
                lineText->m_nLineTextLen = (uint32_t)lineTextView.size();
                ASSERT(lineText->m_nLineTextLen > 0);
            }
        }
        m_lineTextInfo.swap(lineTextInfo);
        SetCacheDirty(true);
        ClearUndoList();
    }
    return bTextChanged;
}

void RichEditData::SplitLines(const std::wstring_view& textView, std::vector<std::wstring_view>& lineTextViewList)
{
    if (textView.empty()) {
        return;
    }
    if (m_bSingleLineMode || m_pRichText->IsTextPasswordMode()) {
        //In single-line text mode and password mode, no line splitting
        lineTextViewList.push_back(textView);
        return;
    }
    //Multi-line text mode
    std::vector<size_t> lineSeprators;
    const size_t nTextLen = textView.size();
    for (size_t nTextIndex = 0; nTextIndex < nTextLen; ++nTextIndex) {
        if (textView[nTextIndex] == L'\n') {
            lineSeprators.push_back(nTextIndex);
        }
    }
    if (lineSeprators.empty()) {
        //No newline separator, single line
        lineTextViewList.push_back(std::wstring_view(textView.data(), textView.size()));
    }
    else {
        //Has newline separators, split into multiple lines, and keep the newline characters
        size_t nLastIndex = 0;
        size_t nCurrentIndex = 0;
        size_t nCharCount = 0;
        const size_t nLineSepCount = lineSeprators.size();
        for (size_t nLine = 0; nLine < nLineSepCount; ++nLine) {
            if (nLine == 0) {
                //First line
                nLastIndex = 0;
                nCurrentIndex = lineSeprators[nLine];
                ASSERT(nCurrentIndex < textView.size());
                nCharCount = nCurrentIndex - nLastIndex + 1;
                if (nCharCount > 0) {
                    lineTextViewList.push_back(std::wstring_view(textView.data(), nCharCount));
                }
            }
            else {
                //Middle lines
                nLastIndex = lineSeprators[nLine - 1];
                nCurrentIndex = lineSeprators[nLine];
                ASSERT(nCurrentIndex > nLastIndex);
                ASSERT(nCurrentIndex < textView.size());
                nCharCount = nCurrentIndex - nLastIndex;
                if (nCharCount > 0) {
                    lineTextViewList.push_back(std::wstring_view(textView.data() + nLastIndex + 1, nCharCount));
                }
            }

            if (nLine == (nLineSepCount - 1)) {
                //Last line: add the last line of data
                nLastIndex = lineSeprators[nLine];
                nCurrentIndex = textView.size() - 1;
                ASSERT(nCurrentIndex >= nLastIndex);
                nCharCount = nCurrentIndex - nLastIndex;
                if (nCharCount > 0) {
                    lineTextViewList.push_back(std::wstring_view(textView.data() + nLastIndex + 1, nCharCount));
                }
            }
        }
    }
}

void RichEditData::GetTextView(std::vector<std::wstring_view>& textView) const
{
    const size_t nLineCount = m_lineTextInfo.size();
    for (size_t nIndex = 0; nIndex < nLineCount; ++nIndex) {
        const RichTextLineInfo& lineText = *m_lineTextInfo[nIndex];
        ASSERT(lineText.m_nLineTextLen > 0);
        if (lineText.m_nLineTextLen > 0) {
            textView.push_back(std::wstring_view(lineText.m_lineText.data(), lineText.m_nLineTextLen));
        }
    }
}

size_t RichEditData::GetTextLength() const
{
    size_t nTextLen = 0;
    const size_t nLineCount = m_lineTextInfo.size();
    for (size_t nIndex = 0; nIndex < nLineCount; ++nIndex) {
        const RichTextLineInfo& lineText = *m_lineTextInfo[nIndex];
        ASSERT(lineText.m_nLineTextLen > 0);
        nTextLen += lineText.m_nLineTextLen;
    }
    return nTextLen;
}

bool RichEditData::IsEmpty() const
{
    bool bEmpty = true;
    const size_t nLineCount = m_lineTextInfo.size();
    for (size_t nIndex = 0; nIndex < nLineCount; ++nIndex) {
        const RichTextLineInfo& lineText = *m_lineTextInfo[nIndex];
        if (lineText.m_nLineTextLen > 0) {
            bEmpty = false;
            break;
        }
    }
    return bEmpty;
}

DStringW RichEditData::GetText() const
{
    DStringW text;
    text.reserve(GetTextLength() + 2);
    std::vector<std::wstring_view> textViewList;
    GetTextView(textViewList);
    for (const std::wstring_view& textView : textViewList) {
        text += textView;
    }
    return text;
}

bool RichEditData::FindLineTextPos(int32_t nStartChar, int32_t nEndChar,
                                   size_t& nStartLine, size_t& nEndLine,
                                   size_t& nStartCharLineOffset, size_t& nEndCharLineOffset) const
{
    //This function does not need to calculate the rectangle range of characters
    ASSERT((nStartChar >= 0) && (nEndChar >= 0) && (nEndChar >= nStartChar));
    if ((nStartChar < 0) || (nEndChar < 0) || (nStartChar > nEndChar)) {
        return false;
    }
    if (m_lineTextInfo.empty()) {
        //Currently empty
        nStartLine = 0;
        nEndLine = 0;
        nStartCharLineOffset = 0;
        nEndCharLineOffset = 0;
        if ((nStartChar == 0) && (nEndChar == 0)) {
            return true;
        }
        return false;
    }

    constexpr const size_t nNotFound = (size_t)-1;
    nStartLine = nNotFound;                 //Start line
    nEndLine = nNotFound;                   //End line
    size_t nStartCharBaseLen = nNotFound;   //The total length before the start line
    size_t nEndCharBaseLen = nNotFound;     //The total length before the end line
    nStartCharLineOffset = nNotFound;       //The offset of the start character in the start line
    nEndCharLineOffset = nNotFound;         //The offset of the end character in the end line
    size_t nTextLen = 0;                    //The total length of the text
    const size_t nLineCount = m_lineTextInfo.size();
    for (size_t nIndex = 0; nIndex < nLineCount; ++nIndex) {
        const RichTextLineInfo& lineText = *m_lineTextInfo[nIndex];
        ASSERT(lineText.m_nLineTextLen > 0);
        nTextLen += lineText.m_nLineTextLen;
        if ((nStartChar < (int32_t)nTextLen) && (nStartLine == nNotFound)) {
            nStartLine = nIndex;
            nStartCharBaseLen = nTextLen - lineText.m_nLineTextLen;
            nStartCharLineOffset = (size_t)nStartChar - nStartCharBaseLen;
            ASSERT(nStartCharLineOffset < lineText.m_nLineTextLen);
        }
        if ((nEndChar < (int32_t)nTextLen) && (nEndLine == nNotFound)) {
            nEndLine = nIndex;
            nEndCharBaseLen = nTextLen - lineText.m_nLineTextLen;
            nEndCharLineOffset = (size_t)nEndChar - nEndCharBaseLen;
            ASSERT(nEndCharLineOffset < lineText.m_nLineTextLen);
        }
        if ((nStartLine != nNotFound) && (nEndLine != nNotFound)) {
            break;
        }
        if (nIndex == (nLineCount - 1)) {
            //Last line
            if ((nStartChar == (int32_t)nTextLen) && (nStartLine == nNotFound)) {
                nStartLine = nIndex;
                nStartCharBaseLen = nTextLen - lineText.m_nLineTextLen;
                nStartCharLineOffset = (size_t)nStartChar - nStartCharBaseLen;
                ASSERT(nStartCharLineOffset == lineText.m_nLineTextLen);
            }
            if ((nEndChar == (int32_t)nTextLen) && (nEndLine == nNotFound)) {
                nEndLine = nIndex;
                nEndCharBaseLen = nTextLen - lineText.m_nLineTextLen;
                nEndCharLineOffset = (size_t)nEndChar - nEndCharBaseLen;
                ASSERT(nEndCharLineOffset == lineText.m_nLineTextLen);
            }
        }
    }
    if ((nStartLine != nNotFound) && (nEndLine != nNotFound) &&
        (nStartCharLineOffset != nNotFound) && (nEndCharLineOffset != nNotFound)) {
        ASSERT(nEndLine >= nStartLine);
        return true;
    }
    return false;
}

bool RichEditData::ReplaceText(int32_t nStartChar, int32_t nEndChar, const DStringW& text, bool bCanUndo, bool bClearRedo)
{
    PerformanceStat statPerformance(_T("RichEditData::ReplaceText"));
    ASSERT((nStartChar >= 0) && (nEndChar >= 0) && (nEndChar >= nStartChar));
    if ((nStartChar < 0) || (nEndChar < 0) || (nStartChar > nEndChar)) {
        return false;
    }

    int32_t nLimitLength = m_pRichText->GetTextLimitLength();
    int32_t nTextLenDiff = (int32_t)text.size() - (nEndChar - nStartChar);
    if ((nTextLenDiff > 0) && (nLimitLength > 0)) {
        //The string will become longer, check whether the string length exceeds the limit
        int32_t nDestTextLen = (int32_t)GetTextLength() + nTextLenDiff;
        if (nDestTextLen > nLimitLength) {
            //Exceeds the limit length, return an error
            return false;
        }
    }

    //Check and calculate the character positions
    CheckCalcTextRects();

    constexpr const size_t nNotFound = (size_t)-1;
    size_t nStartLine = nNotFound;              //Start line
    size_t nEndLine = nNotFound;                //End line
    size_t nStartCharLineOffset = nNotFound;    //The offset of the start character in the start line
    size_t nEndCharLineOffset = nNotFound;      //The offset of the end character in the end line
    if (!FindLineTextPos(nStartChar, nEndChar, nStartLine, nEndLine, nStartCharLineOffset, nEndCharLineOffset)) {
        return false;
    }

    DStringW oldText; //The old text content

    //Whether the undo operation needs to be recorded
    if (m_nUndoLimit == 0) {
        bCanUndo = false;
    }
    if (bCanUndo && (nEndChar > nStartChar)) {
        oldText = GetTextRange(nStartChar, nEndChar);
    }
    //Operation result
    std::wstring_view startLineTextView; //The remaining text of the start line
    std::wstring_view endLineTextView;   //The remaining text of the end line
    if (nStartLine == nEndLine) {
        //In the same line
        if (nStartLine < m_lineTextInfo.size()) {
            const RichTextLineInfo& lineText = *m_lineTextInfo[nStartLine];
            std::wstring_view textView(lineText.m_lineText.c_str(), lineText.m_nLineTextLen);
            startLineTextView = textView.substr(0, nStartCharLineOffset); //Keep the text up to the beginning of the line
            if (nEndCharLineOffset < textView.size()) {
                endLineTextView = textView.substr(nEndCharLineOffset);        //Keep the text up to the end of the line
            }
        }
    }
    else if (nEndLine > nStartLine) {
        //In different lines
        ASSERT(nEndLine < m_lineTextInfo.size());
        if (nEndLine >= m_lineTextInfo.size()) {
            //Error
            return false;
        }
        for (size_t nIndex = nStartLine; nIndex <= nEndLine; ++nIndex) {
            const RichTextLineInfo& lineText = *m_lineTextInfo[nIndex];
            std::wstring_view textView(lineText.m_lineText.c_str(), lineText.m_nLineTextLen);
            if (nIndex == nStartLine) {
                //First line, keep the text up to the beginning of the line
                startLineTextView = textView.substr(0, nStartCharLineOffset);
            }
            else if (nIndex == nEndLine) {
                //Last line, keep the text up to the end of the line
                if (nEndCharLineOffset < textView.size()) {
                    endLineTextView = textView.substr(nEndCharLineOffset);
                }
            }
        }
    }

    //Concatenate the changed new text and split into lines again
    DStringW newText;
    newText.reserve(startLineTextView.size() + text.size() + endLineTextView.size() + 1);
    newText = startLineTextView;
    newText += text;
    newText += endLineTextView;

    //The lines to be deleted
    std::vector<size_t> deletedLines;
    for (size_t nIndex = nStartLine; nIndex <= nEndLine; ++nIndex) {
        deletedLines.push_back(nIndex);
    }
    //How many rows were deleted
    size_t nDeletedRows = 0;
    //Delete in reverse order
    if (!deletedLines.empty()) {
        int32_t nDelIndex = (int32_t)deletedLines.size() - 1;
        for (; nDelIndex >= 0; --nDelIndex) {
            if (deletedLines[nDelIndex] < m_lineTextInfo.size()) {
                nDeletedRows += m_lineTextInfo[deletedLines[nDelIndex]]->m_rowInfo.size();
                m_lineTextInfo.erase(m_lineTextInfo.begin() + deletedLines[nDelIndex]);
            }
        }
    }

    std::wstring_view newTextView = newText;
    std::vector<std::wstring_view> lineTextViewList;
    SplitLines(newTextView, lineTextViewList);

    size_t nNewLineCount = 0;
    for (const std::wstring_view& textView : lineTextViewList) {
        if (!textView.empty()) {
            //Insert a new line
            RichTextLineInfoPtr lineTextInfo(new RichTextLineInfo);
            lineTextInfo->m_lineText = textView;
            lineTextInfo->m_nLineTextLen = (uint32_t)textView.size();
            m_lineTextInfo.insert(m_lineTextInfo.begin() + nStartLine + nNewLineCount, lineTextInfo);
            ++nNewLineCount;
        }
    }

    //The lines where the text has changed
    std::vector<size_t> modifiedLines;
    for (size_t nIndex = 0; nIndex < nNewLineCount; ++nIndex) {
        modifiedLines.push_back(nStartLine + nIndex);
    }

    if (!m_bCacheDirty && (!modifiedLines.empty() || !deletedLines.empty())) {
        //The modified lines need to be recalculated (incremental calculation)
        if ((m_lineTextInfo.size() <= 1) || m_pRichText->IsTextPasswordMode()) {
            //In single-line mode, password mode, or when the text is empty, draw fully
            CalcTextRects();
        }
        else {
            //In multi-line mode, use incremental drawing
            CalcTextRects(nStartLine, modifiedLines, deletedLines, nDeletedRows);
        }        
    }
    if (bCanUndo) {
        //Generate the undo list
        AddToUndoList(nStartChar, text, oldText);
    }
    else if (bClearRedo){
        ClearUndoList();
    }
    return true;
}

DStringW RichEditData::GetTextRange(int32_t nStartChar, int32_t nEndChar) const
{
    if ((nStartChar < 0) || (nEndChar < 0) || (nStartChar >= nEndChar)) {
        return DStringW();
    }

    constexpr const size_t nNotFound = (size_t)-1;
    size_t nStartLine = nNotFound;              //Start line
    size_t nEndLine = nNotFound;                //End line
    size_t nStartCharLineOffset = nNotFound;    //The offset of the start character in the start line
    size_t nEndCharLineOffset = nNotFound;      //The offset of the end character in the end line
    if (!FindLineTextPos(nStartChar, nEndChar, nStartLine, nEndLine, nStartCharLineOffset, nEndCharLineOffset)) {
        return DStringW();
    }

    DStringW selText; //The text content
    if (nStartLine == nEndLine) {
        //In the same line
        const RichTextLineInfo& lineText = *m_lineTextInfo[nStartLine];
        DStringW newText = lineText.m_lineText.c_str();
        if (nEndCharLineOffset > nStartCharLineOffset) {
            //There is selected text
            size_t nCharCount = nEndCharLineOffset - nStartCharLineOffset;
            selText = newText.substr(nStartCharLineOffset, nCharCount);
        }
    }
    else if (nEndLine > nStartLine) {
        //In different lines
        DStringW newText;
        for (size_t nIndex = nStartLine; nIndex <= nEndLine; ++nIndex) {
            const RichTextLineInfo& lineText = *m_lineTextInfo[nIndex];
            newText = lineText.m_lineText.c_str();
            if (nIndex == nStartLine) {
                //First line, select up to the end of the line
                selText = newText.substr(nStartCharLineOffset);
            }
            else if (nIndex == nEndLine) {
                //Last line, select up to the beginning of the line
                newText = lineText.m_lineText.c_str();
                if (nEndCharLineOffset > 0) {
                    selText += newText.substr(0, nEndCharLineOffset);
                }                    
            }
            else {
                //Middle lines
                selText += lineText.m_lineText.c_str();
            }
        }
    }
    return selText;
}

bool RichEditData::HasTextRange(int32_t nStartChar, int32_t nEndChar) const
{
    if ((nStartChar < 0) || (nEndChar < 0) || (nStartChar >= nEndChar)) {
        return false;
    }
    constexpr const size_t nNotFound = (size_t)-1;
    size_t nStartLine = nNotFound;              //Start line
    size_t nEndLine = nNotFound;                //End line
    size_t nStartCharLineOffset = nNotFound;    //The offset of the start character in the start line
    size_t nEndCharLineOffset = nNotFound;      //The offset of the end character in the end line
    if (!FindLineTextPos(nStartChar, nEndChar, nStartLine, nEndLine, nStartCharLineOffset, nEndCharLineOffset)) {
        return false;
    }

    bool bHasText = false;
    if (nStartLine == nEndLine) {
        //In the same line
        bHasText = (nEndCharLineOffset > nStartCharLineOffset) ? true : false;
    }
    else if (nEndLine > nStartLine) {
        //In different lines
        bHasText = true;
    }
    return bHasText;
}

void RichEditData::SetSingleLineMode(bool bSingleLineMode)
{
    if (m_bSingleLineMode != bSingleLineMode) {
        m_bSingleLineMode = bSingleLineMode;
        SetCacheDirty(true);

        //Switch between single-line mode and multi-line mode
        DStringW text = GetText();
        SetText(text);
    }
}

bool RichEditData::IsSingleLineMode() const
{
    return m_bSingleLineMode;
}

bool RichEditData::GetCharLineRowIndex(int32_t nCharIndex, size_t& nLineNumber, size_t& nLineRowIndex, size_t& nStartCharRowOffset) const
{
    ASSERT(!m_bCacheDirty);
    ASSERT(nCharIndex >= 0);
    if (nCharIndex < 0) {
        return false;
    }
    bool bFound = false;
    size_t nTextLen = 0; //The total length of the text
    const RichTextLineInfoList& lineTextInfoList = m_lineTextInfo;
    const size_t nLineCount = lineTextInfoList.size();
    for (size_t nLineIndex = 0; nLineIndex < nLineCount; ++nLineIndex) {
        ASSERT(lineTextInfoList[nLineIndex] != nullptr);
        const RichTextLineInfo& lineTextInfo = *lineTextInfoList[nLineIndex];
        ASSERT(lineTextInfo.m_nLineTextLen > 0);
        nTextLen += lineTextInfo.m_nLineTextLen;
        if ((size_t)nCharIndex < nTextLen) {
            const size_t nStartBaseLen = nTextLen - lineTextInfo.m_nLineTextLen;
            const size_t nStartLineOffset = (size_t)nCharIndex - nStartBaseLen;
            ASSERT(nStartLineOffset < lineTextInfo.m_nLineTextLen);
            //Locate it in the physical line, then locate it in which logical row
            size_t nRowTextLen = 0;
            const size_t nRowCount = lineTextInfo.m_rowInfo.size();
            for (size_t nRow = 0; nRow < nRowCount; ++nRow) {
                ASSERT(lineTextInfo.m_rowInfo[nRow] != nullptr);
                const RichTextRowInfo& rowInfo = *lineTextInfo.m_rowInfo[nRow];
                nRowTextLen += rowInfo.m_charInfo.size();
                if (nStartLineOffset < nRowTextLen) {
                    //Locate it in this logical row
                    const size_t nStartCharBaseLen = nRowTextLen - rowInfo.m_charInfo.size();
                    bFound = true;
                    nStartCharRowOffset = (size_t)nStartLineOffset - nStartCharBaseLen;
                    nLineNumber = nLineIndex;
                    nLineRowIndex = nRow;
                    break;
                }
            }
            break;
        }
        else if (((size_t)nCharIndex == nTextLen) && (nLineIndex == (nLineCount - 1))) {
            //The position after the last character of the last line
            const size_t nRowCount = lineTextInfo.m_rowInfo.size();
            ASSERT(nRowCount != 0);
            if (nRowCount > 0) {
                const RichTextRowInfo& rowInfo = *lineTextInfo.m_rowInfo[nRowCount - 1];                
                nStartCharRowOffset = rowInfo.m_charInfo.size();
                nLineNumber = nLineIndex;
                nLineRowIndex = nRowCount - 1;
                bFound = true;
                break;
            }
        }
    }
    return bFound;
}

RichTextRowInfoPtr RichEditData::GetRowInfoFromPoint(const UiPoint& pt) const
{
    ASSERT(!m_bCacheDirty);
    RichTextRowInfoPtr spRowInfo;
    const RichTextLineInfoList& lineTextInfoList = m_lineTextInfo;
    const size_t nLineCount = lineTextInfoList.size();
    for (size_t nLineIndex = 0; nLineIndex < nLineCount; ++nLineIndex) {
        ASSERT(lineTextInfoList[nLineIndex] != nullptr);
        const RichTextLineInfo& lineTextInfo = *lineTextInfoList[nLineIndex];
        const size_t nRowCount = lineTextInfo.m_rowInfo.size();
        for (size_t nRow = 0; nRow < nRowCount; ++nRow) {
            ASSERT(lineTextInfo.m_rowInfo[nRow] != nullptr);
            const RichTextRowInfo& rowInfo = *lineTextInfo.m_rowInfo[nRow];
            const UiRectF& rowRect = rowInfo.m_rowRect;
            if ((pt.y >= rowRect.top) && (pt.y < rowRect.bottom)) {
                spRowInfo = lineTextInfo.m_rowInfo[nRow];
                break;
            }
        }
        if (spRowInfo != nullptr) {
            break;
        }
    }
    return spRowInfo;
}

RichTextRowInfoPtr RichEditData::GetCharRowInfo(int32_t nCharIndex, size_t& nStartCharRowOffset) const
{
    ASSERT(!m_bCacheDirty);
    size_t nLineNumber = 0;
    size_t nLineRowIndex = 0;
    RichTextRowInfoPtr spRowInfo;
    if (GetCharLineRowIndex(nCharIndex, nLineNumber, nLineRowIndex, nStartCharRowOffset)) {
        if (nLineNumber < m_lineTextInfo.size()) {
            const RichTextLineInfo& lineTextInfo = *m_lineTextInfo[nLineNumber];
            spRowInfo = lineTextInfo.m_rowInfo[nLineRowIndex];
        }
    }
    return spRowInfo;
}

RichTextRowInfoPtr RichEditData::GetFirstRowInfo() const
{
    ASSERT(!m_bCacheDirty);
    RichTextRowInfoPtr spRowInfo;
    const RichTextLineInfoList& lineTextInfoList = m_lineTextInfo;
    if (!lineTextInfoList.empty()) {
        const RichTextLineInfo& lineTextInfo = *lineTextInfoList[0];
        ASSERT(!lineTextInfo.m_rowInfo.empty());
        if (!lineTextInfo.m_rowInfo.empty()) {
            spRowInfo = lineTextInfo.m_rowInfo[0];
        }
    }
    return spRowInfo;
}

RichTextRowInfoPtr RichEditData::GetLastRowInfo() const
{
    ASSERT(!m_bCacheDirty);
    RichTextRowInfoPtr spRowInfo;
    const RichTextLineInfoList& lineTextInfoList = m_lineTextInfo;
    const size_t nLineCount = lineTextInfoList.size();
    if (nLineCount != 0) {
        const RichTextLineInfo& lineTextInfo = *lineTextInfoList[nLineCount - 1];
        ASSERT(!lineTextInfo.m_rowInfo.empty());
        const size_t nRowCount = lineTextInfo.m_rowInfo.size();
        if (nRowCount != 0) {
            spRowInfo = lineTextInfo.m_rowInfo[nRowCount - 1];
        }
    }
    return spRowInfo;
}

size_t RichEditData::GetRowInfoStartIndex(const RichTextRowInfoPtr& spRowInfo) const
{
    ASSERT(!m_bCacheDirty);
    size_t nStartIndex = (size_t)-1;
    size_t nTextLen = 0; //The total length of the text
    const RichTextLineInfoList& lineTextInfoList = m_lineTextInfo;
    const size_t nLineCount = lineTextInfoList.size();
    for (size_t nLineIndex = 0; nLineIndex < nLineCount; ++nLineIndex) {
        ASSERT(lineTextInfoList[nLineIndex] != nullptr);
        const RichTextLineInfo& lineTextInfo = *lineTextInfoList[nLineIndex];
        ASSERT(lineTextInfo.m_nLineTextLen > 0);

        size_t nRowTextLen = 0;
        const size_t nRowCount = lineTextInfo.m_rowInfo.size();
        for (size_t nRow = 0; nRow < nRowCount; ++nRow) {
            ASSERT(lineTextInfo.m_rowInfo[nRow] != nullptr);
            const RichTextRowInfo& rowInfo = *lineTextInfo.m_rowInfo[nRow];            
            if (lineTextInfo.m_rowInfo[nRow] == spRowInfo) {
                //Found this row
                nStartIndex = nTextLen + nRowTextLen;
                break;
            }
            nRowTextLen += rowInfo.m_charInfo.size();
        }

        nTextLen += lineTextInfo.m_nLineTextLen;
        if (nStartIndex != (size_t)-1) {
            break;
        }
    }
    return nStartIndex;
}

void RichEditData::UpdateRowInfo(size_t nDrawStartLineIndex)
{
    RichTextLineInfoList& lineTextInfoList = m_lineTextInfo;
    const size_t nLineCount = lineTextInfoList.size();
    ASSERT(nDrawStartLineIndex < nLineCount);
    if (nDrawStartLineIndex >= nLineCount) {
        return;
    }
    size_t nLineIndex = nDrawStartLineIndex;
    if (nDrawStartLineIndex > 0) {
        nLineIndex -= 1;//Start from the previous line to get the bottom coordinate of the previous line
    }
    float fLastRowHeight = 0.0f;   //The row height of this row
    float fLastBottomValue = 0.0f; //The bottom value of the previous row
    for (; nLineIndex < nLineCount; ++nLineIndex) {
        ASSERT(lineTextInfoList[nLineIndex] != nullptr);
        if (lineTextInfoList[nLineIndex] == nullptr) {
            continue;
        }
        const size_t nLineRowCount = lineTextInfoList[nLineIndex]->m_rowInfo.size();
        ASSERT(nLineRowCount > 0);
        for (size_t nLineRowIndex = 0; nLineRowIndex < nLineRowCount; ++nLineRowIndex) {
            ASSERT(lineTextInfoList[nLineIndex]->m_rowInfo[nLineRowIndex] != nullptr);
            if (lineTextInfoList[nLineIndex]->m_rowInfo[nLineRowIndex] == nullptr) {
                continue;
            }
            UiRectF& rowRect = lineTextInfoList[nLineIndex]->m_rowInfo[nLineRowIndex]->m_rowRect;
            if (nLineIndex >= nDrawStartLineIndex) {
                //Update the vertical coordinate value of this row
                fLastRowHeight = rowRect.bottom - rowRect.top;
                rowRect.top = fLastBottomValue;
                rowRect.bottom = rowRect.top + fLastRowHeight;
            }
            fLastBottomValue = rowRect.bottom;
        }        
    }
}

UiPoint RichEditData::PosForEmptyText() const
{
    UiRect rcDrawRect = m_pRichText->GetRichTextDrawRect();
    HorAlignType hAlignType = GetHAlignType();
    VerAlignType vAlignType = GetVAlignType();
    UiPoint pt;
    if (hAlignType == HorAlignType::kAlignCenter) {
        pt.x = rcDrawRect.Width() / 2;
    }
    else if (hAlignType == HorAlignType::kAlignRight) {
        pt.x = rcDrawRect.Width();
    }
    else {
        pt.x = 0;
    }

    if (vAlignType == VerAlignType::kAlignCenter) {
        const int32_t nRowHeight = m_pRichText->GetTextRowHeight();
        if (rcDrawRect.Height() <= nRowHeight) {
            pt.y = 0;
        }
        else {
            pt.y = rcDrawRect.Height() / 2;
            if (nRowHeight > 0) {
                pt.y -= nRowHeight / 2;
                if (pt.y < 0) {
                    pt.y = 0;
                }
            }
        }
    }
    else if (vAlignType == VerAlignType::kAlignBottom) {
        const int32_t nRowHeight = m_pRichText->GetTextRowHeight();
        if (rcDrawRect.Height() <= nRowHeight) {
            pt.y = 0;
        }
        else {
            pt.y = rcDrawRect.Height();
            if (nRowHeight > 0) {
                pt.y -= nRowHeight;
                if (pt.y < 0) {
                    pt.y = 0;
                }
            }
        }
    }
    else {
        pt.y = 0;
    }
    return pt;
}

UiPoint RichEditData::CaretPosFromChar(int32_t nCharIndex)
{
    //Check and calculate the character positions
    CheckCalcTextRects();

    if (m_rcTextDrawRect.IsEmpty()) {
        //The drawing area is empty
        return UiPoint(m_rcTextDrawRect.left, m_rcTextDrawRect.top);
    }

    UiPoint cursorPos;
    if (m_lineTextInfo.empty()) {
        //Empty text
        cursorPos = PosForEmptyText();
        if (GetHAlignType() == HorAlignType::kAlignRight) {
            cursorPos.x -= m_pRichText->GetTextCaretWidth();
        }
    }
    else if (nCharIndex < 0) {
        //Invalid parameter
        cursorPos.x = 0;
        cursorPos.y = 0;
    }
    else {
        size_t nStartCharRowOffset = 0;
        RichTextRowInfoPtr spRowInfo = GetCharRowInfo(nCharIndex, nStartCharRowOffset);
        if (spRowInfo != nullptr) {
            const RichTextRowInfo& rowInfo = *spRowInfo;
            const size_t nIndexOffset = nStartCharRowOffset;
            float xPos = rowInfo.m_rowRect.left;//Top-left corner coordinate
            cursorPos.y = (int32_t)rowInfo.m_rowRect.top;
            for (size_t i = 0; i < nIndexOffset; ++i) {
                const RichTextCharInfo& charInfo = rowInfo.m_charInfo[i];
                xPos += charInfo.CharWidth();
            }
            cursorPos.x = (int32_t)xPos;
        }        
        else {
            //Get the top-right corner coordinate of the last character
            spRowInfo = GetLastRowInfo();
            if (spRowInfo != nullptr) {
                const RichTextRowInfo& rowInfo = *spRowInfo;
                float xPos = rowInfo.m_rowRect.left;
                cursorPos.y = (int32_t)rowInfo.m_rowRect.top;
                const size_t nCharCount = rowInfo.m_charInfo.size();
                for (size_t i = 0; i < nCharCount; ++i) {
                    const RichTextCharInfo& charInfo = rowInfo.m_charInfo[i];
                    xPos += charInfo.CharWidth();//Top-right corner coordinate
                }
                cursorPos.x = (int32_t)ui::CEILF(xPos);
            }
        }
    }

    //Convert to external coordinates
    ConvertToExternal(cursorPos);
    return cursorPos;
}

UiRect RichEditData::GetCharRowRect(int32_t nCharIndex)
{
    //Check and calculate the character positions
    CheckCalcTextRects();

    UiRect rowRect;
    if (m_lineTextInfo.empty()) {
        //Empty text
        if (nCharIndex == 0) {
            const int32_t nRowHeight = m_pRichText->GetTextRowHeight();
            const UiRect rc = m_pRichText->GetRichTextDrawRect();
            UiPoint pt = PosForEmptyText();
            rowRect.left = 0;
            rowRect.right = rc.Width();
            rowRect.top = pt.y;
            rowRect.bottom = rowRect.top + nRowHeight;
        }
    }
    else {
        size_t nStartCharRowOffset = 0;
        RichTextRowInfoPtr spRowInfo = GetCharRowInfo(nCharIndex, nStartCharRowOffset);
        if (spRowInfo != nullptr) {
            const RichTextRowInfo& rowInfo = *spRowInfo;
            const UiRectF& rowRectF = rowInfo.m_rowRect;
            UiRect rc = m_pRichText->GetRichTextDrawRect();
            rowRect.left = 0;
            rowRect.right = std::max(rc.Width(), (int32_t)rowRectF.Width());
            rowRect.top = (int32_t)rowRectF.top;
            rowRect.bottom = (int32_t)ui::CEILF(rowRectF.bottom);
        }
    }

    //Convert to external coordinates
    ConvertToExternal(rowRect);
    return rowRect;
}

UiPoint RichEditData::PosFromChar(int32_t nCharIndex)
{
    //Check and calculate the character positions
    CheckCalcTextRects();

    UiPoint pt;
    if (m_lineTextInfo.empty()) {
        //Empty text
        pt = PosForEmptyText();
    }
    else if (nCharIndex < 0) {
        //Invalid parameter
        pt.x = 0;
        pt.y = 0;
    }
    else {     
        size_t nStartCharRowOffset = 0;
        RichTextRowInfoPtr spRowInfo = GetCharRowInfo(nCharIndex, nStartCharRowOffset);
        if (spRowInfo != nullptr) {
            const RichTextRowInfo& rowInfo = *spRowInfo;
            const size_t nIndexOffset = nStartCharRowOffset;
            float xPos = rowInfo.m_rowRect.left;//Top-left corner coordinate
            pt.y = (int32_t)rowInfo.m_rowRect.top;
            for (size_t i = 0; i < nIndexOffset; ++i) {
                const RichTextCharInfo& charInfo = rowInfo.m_charInfo[i];
                xPos += charInfo.CharWidth();
            }
            pt.x = (int32_t)xPos;
        }
        else {
            //Get the top-left corner coordinate of the last character
            spRowInfo = GetLastRowInfo();
            if (spRowInfo != nullptr) {
                const RichTextRowInfo& rowInfo = *spRowInfo;
                float xPos = rowInfo.m_rowRect.left;
                pt.y = (int32_t)rowInfo.m_rowRect.top;
                if (!rowInfo.m_charInfo.empty()) {
                    const size_t nIndexOffset = rowInfo.m_charInfo.size() - 1;
                    for (size_t i = 0; i < nIndexOffset; ++i) {
                        const RichTextCharInfo& charInfo = rowInfo.m_charInfo[i];
                        xPos += charInfo.CharWidth();//Top-left corner coordinate
                    }
                }
                pt.x = (int32_t)xPos;
            }
        }
    }

    //Convert to external coordinates
    ConvertToExternal(pt);
    return pt;
}

int32_t RichEditData::CharFromPos(UiPoint pt)
{
    const int32_t nTextLength = (int32_t)GetTextLength();
    if (nTextLength == 0) {
        return 0;
    }

    //Check and calculate the character positions
    CheckCalcTextRects();

    if (m_rcTextDrawRect.IsEmpty()) {
        //The text display area is empty
        return 0;
    }

    //Convert to internal coordinates
    ConvertToInternal(pt);

    //Align horizontally by character boundaries and vertically by row height
    int32_t nCharPosIndex = -1;
    RichTextRowInfoPtr spDestRow;
    RichTextRowInfoPtr spFirstRow = GetFirstRowInfo();
    if (spFirstRow != nullptr) {
        const UiRectF& rowRect = spFirstRow->m_rowRect;
        if (pt.y < rowRect.top) {
            //The point is above the area, locate to the first row
            spDestRow = spFirstRow;
        }
    }
    if (spDestRow == nullptr) {
        RichTextRowInfoPtr spLastRow = GetLastRowInfo();
        if (spLastRow != nullptr) {
            const UiRectF& rowRect = spLastRow->m_rowRect;
            if (pt.y >= rowRect.bottom) {
                //The point is below the area, locate to the last row
                spDestRow = spLastRow;
            }
        }        
    }

    if (spDestRow == nullptr) {
        spDestRow = GetRowInfoFromPoint(pt);        
    }
    ASSERT(spDestRow != nullptr);
    if (spDestRow != nullptr) {
        const RichTextRowInfo& rowInfo = *spDestRow;
        const size_t nCharCount = rowInfo.m_charInfo.size();
        ASSERT(!rowInfo.m_charInfo.empty());

        if (pt.x <= rowInfo.m_rowRect.left) {
            //The point is on the left side of this row, pointing to the first character of this row
            nCharPosIndex = (int32_t)GetRowInfoStartIndex(spDestRow);
        }
        else if (pt.x >= rowInfo.m_rowRect.right) {
            //The point is on the right side of this row, pointing to the last character of this row
            if ((nCharCount >= 2) && rowInfo.m_charInfo[nCharCount - 1].IsNewLine() && rowInfo.m_charInfo[nCharCount - 2].IsReturn()){
                //This row ends with carriage return + newline: point to the carriage return character
                nCharPosIndex = (int32_t)(GetRowInfoStartIndex(spDestRow) + rowInfo.m_charInfo.size() - 2);
            }
            else if ((nCharCount >= 1) && rowInfo.m_charInfo[nCharCount - 1].IsNewLine()) {
                //This row ends with a newline: point to the newline character
                nCharPosIndex = (int32_t)(GetRowInfoStartIndex(spDestRow) + rowInfo.m_charInfo.size() - 1);
            }
            else {
                //This row has no carriage return or newline at the end, point after this character
                nCharPosIndex = (int32_t)(GetRowInfoStartIndex(spDestRow) + rowInfo.m_charInfo.size());
            }
        }
        else if ((nCharCount == 2) && rowInfo.m_charInfo[nCharCount - 1].IsNewLine() && rowInfo.m_charInfo[nCharCount - 2].IsReturn()) {
            //This row is an empty row with only one carriage return + newline: point to the carriage return character
            nCharPosIndex = (int32_t)(GetRowInfoStartIndex(spDestRow) + rowInfo.m_charInfo.size() - 2);
        }
        else if (nCharCount == 1) {
            //This row has only one character
            nCharPosIndex = (int32_t)GetRowInfoStartIndex(spDestRow);
        }
        else {
            float xRowPos = rowInfo.m_rowRect.left;
            const size_t nCount = rowInfo.m_charInfo.size();
            for (size_t nIndex = 0; nIndex < nCount; ++nIndex) {
                const RichTextCharInfo& charInfo = rowInfo.m_charInfo[nIndex];
                if (charInfo.IsIgnoredChar()) {
                    continue;
                }
                if ((pt.x >= xRowPos) && (pt.x < (xRowPos + charInfo.CharWidth()))) {
                    if (pt.x <= (xRowPos + charInfo.CharWidth() / 2)) {
                        //If the X coordinate is less than or equal to the center point, take the current character
                        nCharPosIndex = (int32_t)(GetRowInfoStartIndex(spDestRow) + nIndex);
                    }
                    else {
                        //If the X coordinate is greater than the center point, take the next character
                        for (size_t i = nIndex + 1; i < nCount; ++i) {
                            const RichTextCharInfo& nextCharInfo = rowInfo.m_charInfo[i];
                            if (nextCharInfo.IsLowSurrogate()) {
                                continue;
                            }
                            nCharPosIndex = (int32_t)(GetRowInfoStartIndex(spDestRow) + i);
                            break;
                        }
                        if (nCharPosIndex == -1) {
                            nCharPosIndex = (int32_t)(GetRowInfoStartIndex(spDestRow) + nIndex);
                        }
                    }
                    break;
                }
                xRowPos += charInfo.CharWidth();
            }
        }
    }
    
    if (nCharPosIndex < 0) {
        nCharPosIndex = 0;
    }
    else if (nCharPosIndex > nTextLength) {
        nCharPosIndex = nTextLength;
    }
    return nCharPosIndex;
}

#define SkUTF16_IsHighSurrogate(c)  (((c) & 0xFC00) == 0xD800)
#define SkUTF16_IsLowSurrogate(c)   (((c) & 0xFC00) == 0xDC00)

int32_t RichEditData::GetNextValidCharIndex(const int32_t nCharIndex)
{
    ASSERT(nCharIndex >= 0);
    if (nCharIndex < 0) {
        return 0;
    }
    const int32_t nTextLength = (int32_t)GetTextLength();
    if (nTextLength < 1) {
        return 0;
    }
    if (nCharIndex > nTextLength) {
        return nTextLength;
    }
    //Check and calculate the character positions
    CheckCalcTextRects();

    int32_t nNewCharIndex = nCharIndex;
    size_t nTextLen = 0; //The total length of the text
    const size_t nLineCount = m_lineTextInfo.size();
    for (size_t nIndex = 0; nIndex < nLineCount; ++nIndex) {
        const RichTextLineInfo& lineText = *m_lineTextInfo[nIndex];
        ASSERT(lineText.m_nLineTextLen > 0);
        nTextLen += lineText.m_nLineTextLen;
        if (nCharIndex < (int32_t)nTextLen) {
            const size_t nStartCharBaseLen = nTextLen - lineText.m_nLineTextLen;
            const size_t nStartCharLineOffset = (size_t)nCharIndex - nStartCharBaseLen;
            ASSERT(nStartCharLineOffset < lineText.m_nLineTextLen);
            //Search in this line
            size_t i = nStartCharLineOffset + 1;
            while ( i < lineText.m_nLineTextLen) {
                const uint16_t* src = (const uint16_t*)(lineText.m_lineText.c_str() + i);
                if (SkUTF16_IsHighSurrogate(*src)) {
                    ASSERT(SkUTF16_IsLowSurrogate(*(src + 1)));
                    nNewCharIndex = (int32_t)(nStartCharBaseLen + i);
                    break;
                }
                else if (SkUTF16_IsLowSurrogate(*src)) {
                    i += 1;//Skip this character
                }
                else {
                    nNewCharIndex = (int32_t)(nStartCharBaseLen + i);
                    break;
                }
            }
            size_t nNewOffset = (size_t)nNewCharIndex - nStartCharBaseLen;
            if ((nNewOffset == (lineText.m_nLineTextLen - 1)) && (lineText.m_lineText.data()[nNewOffset] == L'\n')) {
                //If it already points to a newline character, jump to the next character (i.e. avoid jumping from '\r' to '\n')
                if ((nNewOffset >= 1) && (lineText.m_lineText.data()[nNewOffset - 1] == L'\r')) {
                    nNewCharIndex += 1;
                }
            }
            break;
        }
    }
    if (nNewCharIndex == nCharIndex) {
        nNewCharIndex += 1;        
    }
    if (nNewCharIndex < 0) {
        nNewCharIndex = 0;
    }
    if (nNewCharIndex > nTextLength) {
        nNewCharIndex = nTextLength;
    }
    return nNewCharIndex;
}

int32_t RichEditData::GetPrevValidCharIndex(int32_t nCharIndex)
{
    ASSERT(nCharIndex >= 0);
    if (nCharIndex < 0) {
        return 0;
    }
    const int32_t nTextLength = (int32_t)GetTextLength();
    if (nTextLength < 1) {
        return 0;
    }
    if (nCharIndex > nTextLength) {
        return nTextLength ;
    }
    //Check and calculate the character positions
    CheckCalcTextRects();

    int32_t nNewCharIndex = nCharIndex;
    size_t nTextLen = 0; //The total length of the text
    const size_t nLineCount = m_lineTextInfo.size();
    for (size_t nIndex = 0; nIndex < nLineCount; ++nIndex) {
        const RichTextLineInfo& lineText = *m_lineTextInfo[nIndex];
        ASSERT(lineText.m_nLineTextLen > 0);
        nTextLen += lineText.m_nLineTextLen;
        if (nCharIndex < (int32_t)nTextLen) {
            const size_t nStartCharBaseLen = nTextLen - lineText.m_nLineTextLen;
            const size_t nStartCharLineOffset = (size_t)nCharIndex - nStartCharBaseLen;
            ASSERT(nStartCharLineOffset < lineText.m_nLineTextLen);
            //Search in this line
            int32_t i = (int32_t)nStartCharLineOffset - 1;
            while (i >= 0) {
                const uint16_t* src = (const uint16_t*)(lineText.m_lineText.c_str() + i);
                if (SkUTF16_IsHighSurrogate(*src)) {
                    ASSERT(SkUTF16_IsLowSurrogate(*(src + 1)));
                    nNewCharIndex = (int32_t)(nStartCharBaseLen + i);
                    break;
                }
                else if (SkUTF16_IsLowSurrogate(*src)) {
                    i -= 1;//Skip this character
                }
                else {
                    nNewCharIndex = (int32_t)(nStartCharBaseLen + i);
                    break;
                }
            }
            if ((nNewCharIndex == nCharIndex) && (i <= 0) && (nIndex >= 1)) {
                //Already at the beginning of the line, jump to the last character of the previous line
                const RichTextLineInfo& prevLineText = *m_lineTextInfo[nIndex - 1];
                ASSERT(prevLineText.m_nLineTextLen > 0);
                if (prevLineText.m_nLineTextLen > 1) {
                    ASSERT(prevLineText.m_lineText.data()[prevLineText.m_nLineTextLen - 1] == L'\n');
                    nNewCharIndex = nCharIndex - 2; //Skip the last '\n' character
                }
                else if (prevLineText.m_nLineTextLen == 1) {
                    nNewCharIndex = nCharIndex - 1; //Point to this character
                }
            }
            else {
                size_t nNewOffset = (size_t)nNewCharIndex - nStartCharBaseLen;
                if ((nNewOffset == (lineText.m_nLineTextLen - 1)) && (lineText.m_lineText.data()[nNewOffset] == L'\n')) {
                    //If it already points to a newline character, jump to the preceding carriage return character
                    if ((nNewOffset >= 1) && (lineText.m_lineText.data()[nNewOffset - 1] == L'\r')) {
                        nNewCharIndex -= 1;
                    }
                }
            }
            break;
        }
    }
    if ((nNewCharIndex == nCharIndex) && (nNewCharIndex > 0)) {
        nNewCharIndex -= 1;
    }
    if (nNewCharIndex < 0) {
        nNewCharIndex = 0;
    }
    if (nNewCharIndex > nTextLength) {
        nNewCharIndex = nTextLength;
    }
    return nNewCharIndex;
}

bool RichEditData::IsSeperatorChar(DStringW::value_type ch) const
{
    static const DStringW sep = L"`~!@#$%^&*()-=+\t[]{}|\\;:'\"\r\n,<.>/?·！￥…、，。《》？“”；：‘’（）【】";
    return sep.find(ch) != DStringW::npos;
}

int32_t RichEditData::GetNextValidWordIndex(int32_t nCharIndex)
{
    ASSERT(nCharIndex >= 0);
    if (nCharIndex < 0) {
        return 0;
    }
    const int32_t nTextLength = (int32_t)GetTextLength();
    if (nTextLength < 1) {
        return 0;
    }
    if (nCharIndex > nTextLength) {
        return nTextLength;
    }
    //Check and calculate the character positions
    CheckCalcTextRects();

    int32_t nNewCharIndex = nCharIndex;
    size_t nTextLen = 0; //The total length of the text
    const size_t nLineCount = m_lineTextInfo.size();
    for (size_t nIndex = 0; nIndex < nLineCount; ++nIndex) {
        const RichTextLineInfo& lineText = *m_lineTextInfo[nIndex];
        ASSERT(lineText.m_nLineTextLen > 0);
        nTextLen += lineText.m_nLineTextLen;
        if (nCharIndex < (int32_t)nTextLen) {
            const size_t nStartCharBaseLen = nTextLen - lineText.m_nLineTextLen;
            const size_t nStartCharLineOffset = (size_t)nCharIndex - nStartCharBaseLen;
            ASSERT(nStartCharLineOffset < lineText.m_nLineTextLen);
            //Search in this line until a separator is found (space, punctuation, etc.)
            size_t i = nStartCharLineOffset + 1;
            bool bFoundBlank = lineText.m_lineText.data()[nStartCharLineOffset] == L' ';
            while (i < lineText.m_nLineTextLen) {
                //If it is a space, skip all consecutive spaces
                while ((i < lineText.m_nLineTextLen) && lineText.m_lineText.data()[i] == L' ') {
                    bFoundBlank = true;
                    ++i;
                }
                if (i >= lineText.m_nLineTextLen) {
                    //Already reached the end of the line
                    nNewCharIndex = (int32_t)(nStartCharBaseLen + lineText.m_nLineTextLen - 1);
                    break;
                }
                if (bFoundBlank) {
                    //When there is a space, stop at the character after the space
                    nNewCharIndex = (int32_t)(nStartCharBaseLen + i);
                    break;
                }
                if (IsSeperatorChar(lineText.m_lineText.data()[nStartCharLineOffset]) ||
                    IsSeperatorChar(lineText.m_lineText.data()[i])) {
                    //The current character is a separator, stop
                    nNewCharIndex = (int32_t)(nStartCharBaseLen + i);
                    break;
                }
                const uint16_t* src = (const uint16_t*)(lineText.m_lineText.c_str() + i);
                if (SkUTF16_IsHighSurrogate(*src)) {
                    ASSERT(SkUTF16_IsLowSurrogate(*(src + 1)));
                    i += 2;//Skip this double-byte character
                }
                else {
                    i += 1;//Skip this character
                }
            }
            size_t nNewOffset = (size_t)nNewCharIndex - nStartCharBaseLen;
            if ((nNewOffset == (lineText.m_nLineTextLen - 1)) && (lineText.m_lineText.data()[nNewOffset] == L'\n')) {
                //If it already points to a newline character, jump to the next character (i.e. avoid jumping from '\r' to '\n')
                if ((nNewOffset >= 1) && (lineText.m_lineText.data()[nNewOffset - 1] == L'\r')) {
                    nNewCharIndex += 1;
                }
            }
            break;
        }
    }
    if (nNewCharIndex == nCharIndex) {
        nNewCharIndex += 1;
    }
    if (nNewCharIndex < 0) {
        nNewCharIndex = 0;
    }
    if (nNewCharIndex > nTextLength) {
        nNewCharIndex = nTextLength;
    }
    return nNewCharIndex;
}

int32_t RichEditData::GetPrevValidWordIndex(int32_t nCharIndex)
{
    ASSERT(nCharIndex >= 0);
    if (nCharIndex < 0) {
        return 0;
    }
    const int32_t nTextLength = (int32_t)GetTextLength();
    if (nTextLength < 1) {
        return 0;
    }
    if (nCharIndex > nTextLength) {
        return nTextLength;
    }
    //Check and calculate the character positions
    CheckCalcTextRects();

    int32_t nNewCharIndex = nCharIndex;
    size_t nTextLen = 0; //The total length of the text
    const size_t nLineCount = m_lineTextInfo.size();
    for (size_t nIndex = 0; nIndex < nLineCount; ++nIndex) {
        const RichTextLineInfo& lineText = *m_lineTextInfo[nIndex];
        ASSERT(lineText.m_nLineTextLen > 0);
        nTextLen += lineText.m_nLineTextLen;
        if (nCharIndex < (int32_t)nTextLen) {
            const size_t nStartCharBaseLen = nTextLen - lineText.m_nLineTextLen;
            const size_t nStartCharLineOffset = (size_t)nCharIndex - nStartCharBaseLen;
            ASSERT(nStartCharLineOffset < lineText.m_nLineTextLen);
            //Search in this line
            int32_t i = (int32_t)nStartCharLineOffset - 1;            
            bool bFoundBlank = lineText.m_lineText.data()[nStartCharLineOffset] == L' ';
            while (i >= 0) {
                //Skip consecutive spaces
                while ((i >= 0) && (lineText.m_lineText.data()[i] == L' ')) {
                    bFoundBlank = true;
                    i -= 1;//Skip this character
                }

                if (i > 0) {
                    const uint16_t* src = (const uint16_t*)(lineText.m_lineText.c_str() + i);
                    if (SkUTF16_IsLowSurrogate(*src)) {
                        i -= 1;//Skip the low surrogate character
                    }
                }

                if (i <= 0) {
                    //Already reached the beginning of the line
                    nNewCharIndex = (int32_t)nStartCharBaseLen;
                    break;
                }

                if (bFoundBlank) {
                    //When there is a space, stop at the character before the space, but do not include the character before the space
                    nNewCharIndex = (int32_t)(nStartCharBaseLen + i + 1);
                    break;
                }

                if (IsSeperatorChar(lineText.m_lineText.data()[i])) {
                    //The current character is a separator, stop, but do not include the separator itself
                    nNewCharIndex = (int32_t)(nStartCharBaseLen + i + 1);
                    break;
                }
            
                i -= 1;//Skip this character
            }
            if ((nNewCharIndex == nCharIndex) && (i <= 0) && (nIndex >= 1)) {
                //Already at the beginning of the line, jump to the last character of the previous line
                const RichTextLineInfo& prevLineText = *m_lineTextInfo[nIndex - 1];
                ASSERT(prevLineText.m_nLineTextLen > 0);
                if (prevLineText.m_nLineTextLen > 1) {
                    ASSERT(prevLineText.m_lineText.data()[prevLineText.m_nLineTextLen - 1] == L'\n');
                    nNewCharIndex = nCharIndex - 2; //Skip the last '\n' character
                }
                else if (prevLineText.m_nLineTextLen == 1) {
                    nNewCharIndex = nCharIndex - 1; //Point to this character
                }
            }
            else {
                size_t nNewOffset = (size_t)nNewCharIndex - nStartCharBaseLen;
                if ((nNewOffset == (lineText.m_nLineTextLen - 1)) && (lineText.m_lineText.data()[nNewOffset] == L'\n')) {
                    //If it already points to a newline character, jump to the preceding carriage return character
                    if ((nNewOffset >= 1) && (lineText.m_lineText.data()[nNewOffset - 1] == L'\r')) {
                        nNewCharIndex -= 1;
                    }
                }
            }
            break;
        }
    }
    if ((nNewCharIndex == nCharIndex) && (nNewCharIndex > 0)) {
        nNewCharIndex -= 1;
    }
    if (nNewCharIndex < 0) {
        nNewCharIndex = 0;
    }
    if (nNewCharIndex > nTextLength) {
        nNewCharIndex = nTextLength;
    }
    return nNewCharIndex;
}

bool RichEditData::GetCurrentWordIndex(int32_t nCharIndex, int32_t& nWordStartIndex, int32_t& nWordEndIndex)
{
    nWordStartIndex = -1;
    nWordEndIndex = -1;
    ASSERT(nCharIndex >= 0);
    if (nCharIndex < 0) {
        return false;
    }
    const int32_t nTextLength = (int32_t)GetTextLength();
    if (nTextLength < 1) {
        return false;
    }
    if (nCharIndex > nTextLength) {
        return false;
    }
    //Check and calculate the character positions
    CheckCalcTextRects();

    size_t nTextLen = 0; //The total length of the text
    const size_t nLineCount = m_lineTextInfo.size();
    for (size_t nIndex = 0; nIndex < nLineCount; ++nIndex) {
        const RichTextLineInfo& lineText = *m_lineTextInfo[nIndex];
        ASSERT(lineText.m_nLineTextLen > 0);
        nTextLen += lineText.m_nLineTextLen;
        if (nCharIndex < (int32_t)nTextLen) {
            const size_t nStartCharBaseLen = nTextLen - lineText.m_nLineTextLen;
            const size_t nStartCharLineOffset = (size_t)nCharIndex - nStartCharBaseLen;
            ASSERT(nStartCharLineOffset < lineText.m_nLineTextLen);

            if (IsSeperatorChar(lineText.m_lineText.data()[nStartCharLineOffset])) {
                //The current character is a separator, select this separator
                nWordStartIndex = (int32_t)(nStartCharBaseLen + nStartCharLineOffset);
                nWordEndIndex = (int32_t)(nStartCharBaseLen + nStartCharLineOffset + 1);
                break;
            }
            else if (lineText.m_lineText.data()[nStartCharLineOffset] == L' ') {
                //The current character is a space, select the consecutive spaces
                size_t i = nStartCharLineOffset + 1;
                while (i < lineText.m_nLineTextLen) {
                    if (lineText.m_lineText.data()[i] == L' ') {
                        ++i;
                        continue;
                    }
                    nWordEndIndex = (int32_t)(nStartCharBaseLen + i);
                    break;
                }
                int32_t j = (int32_t)nStartCharLineOffset - 1;
                while (j >= 0) {
                    if (lineText.m_lineText.data()[j] == L' ') {
                        --j;
                        continue;
                    }
                    nWordStartIndex = (int32_t)(nStartCharBaseLen + j + 1);
                    break;
                }
                if ((nWordEndIndex != -1)) {
                    if (j < 0) {
                        nWordStartIndex = (int32_t)nStartCharBaseLen;
                    }
                }
                if ((nWordStartIndex != -1) && (nWordEndIndex != -1)) {
                    break;
                }
            }

            //Locate the end character: search backward until a separator is found (space, punctuation, etc.)
            size_t i = nStartCharLineOffset + 1;
            while (i < lineText.m_nLineTextLen) {
                if (IsSeperatorChar(lineText.m_lineText.data()[nStartCharLineOffset]) ||
                    IsSeperatorChar(lineText.m_lineText.data()[i]) ||
                    (lineText.m_lineText.data()[nStartCharLineOffset] == L' ') ||
                    (lineText.m_lineText.data()[i] == L' ')) {
                    //The current character is a separator, stop
                    nWordEndIndex = (int32_t)(nStartCharBaseLen + i);
                    break;
                }
                const uint16_t* src = (const uint16_t*)(lineText.m_lineText.c_str() + i);
                if (SkUTF16_IsHighSurrogate(*src)) {
                    ASSERT(SkUTF16_IsLowSurrogate(*(src + 1)));
                    i += 2;//Skip this double-byte character
                }
                else {
                    i += 1;//Skip this character
                }
            }
            if (nWordEndIndex == -1) {
                nWordEndIndex = (int32_t)lineText.m_nLineTextLen;
            }

            //Locate the start character: search forward until a separator is found (space, punctuation, etc.)
            int32_t j = (int32_t)nStartCharLineOffset - 1;
            while (j >= 0) {
                if (IsSeperatorChar(lineText.m_lineText.data()[j]) || (lineText.m_lineText.data()[j] == ' ')) {
                    //The current character is a separator, stop, but do not include the separator itself
                    nWordStartIndex = (int32_t)(nStartCharBaseLen + j + 1);
                    break;
                }
                j -= 1;//Skip this character
            }
            if (nWordStartIndex == -1) {
                nWordStartIndex = (int32_t)nStartCharBaseLen;
            }
            break;
        }
    }
    return (nWordEndIndex > nWordStartIndex) && (nWordStartIndex >= 0) && (nWordEndIndex >= 0);
}

int32_t RichEditData::GetRowStartCharIndex(int32_t nCharIndex)
{
    ASSERT(nCharIndex >= 0);
    if (nCharIndex < 0) {
        return 0;
    }
    const int32_t nTextLength = (int32_t)GetTextLength();
    if (nTextLength < 1) {
        return 0;
    }
    if (nCharIndex > nTextLength) {
        return nTextLength;
    }
    //Check and calculate the character positions
    CheckCalcTextRects();

    int32_t nNewCharIndex = nCharIndex;
    size_t nTextLen = 0; //The total length of the text
    const size_t nLineCount = m_lineTextInfo.size();
    for (size_t nIndex = 0; nIndex < nLineCount; ++nIndex) {
        const RichTextLineInfo& lineText = *m_lineTextInfo[nIndex];
        ASSERT(lineText.m_nLineTextLen > 0);
        nTextLen += lineText.m_nLineTextLen;
        if (nCharIndex < (int32_t)nTextLen) {
            //Search in this line
            nNewCharIndex = (int32_t)nTextLen - lineText.m_nLineTextLen;
            break;
        }
    }
    if (nNewCharIndex < 0) {
        nNewCharIndex = 0;
    }
    if (nNewCharIndex > nTextLength) {
        nNewCharIndex = nTextLength;
    }
    return nNewCharIndex;
}

int32_t RichEditData::GetRowEndCharIndex(int32_t nCharIndex)
{
    ASSERT(nCharIndex >= 0);
    if (nCharIndex < 0) {
        return 0;
    }
    const int32_t nTextLength = (int32_t)GetTextLength();
    if (nTextLength < 1) {
        return 0;
    }
    if (nCharIndex > nTextLength) {
        return nTextLength;
    }
    //Check and calculate the character positions
    CheckCalcTextRects();

    int32_t nNewCharIndex = nCharIndex;
    size_t nTextLen = 0; //The total length of the text
    const size_t nLineCount = m_lineTextInfo.size();
    for (size_t nIndex = 0; nIndex < nLineCount; ++nIndex) {
        const RichTextLineInfo& lineText = *m_lineTextInfo[nIndex];
        ASSERT(lineText.m_nLineTextLen > 0);
        nTextLen += lineText.m_nLineTextLen;
        if (nCharIndex < (int32_t)nTextLen) {
            //Search in this line
            const size_t nStartCharBaseLen = nTextLen - lineText.m_nLineTextLen;
            nNewCharIndex = (int32_t)(nTextLen - 1);
            size_t nNewOffset = (size_t)nNewCharIndex - nStartCharBaseLen;
            if ((nNewOffset == (lineText.m_nLineTextLen - 1)) && (lineText.m_lineText.data()[nNewOffset] == L'\n')) {
                //If it already points to a newline character, jump to the preceding carriage return '\r'
                if ((nNewOffset >= 1) && (lineText.m_lineText.data()[nNewOffset - 1] == L'\r')) {
                    nNewCharIndex -= 1;
                }
            }
            break;
        }
    }
    if (nNewCharIndex < 0) {
        nNewCharIndex = 0;
    }
    if (nNewCharIndex > nTextLength) {
        nNewCharIndex = nTextLength;
    }
    return nNewCharIndex;
}

int32_t RichEditData::GetCharWidthValue(int32_t nCharIndex)
{
    //Check and calculate the character positions
    CheckCalcTextRects();

    int32_t nCharWidth = 0;
    size_t nStartCharRowOffset = 0;
    RichTextRowInfoPtr spRowInfo = GetCharRowInfo(nCharIndex, nStartCharRowOffset);
    if (spRowInfo != nullptr) {
        const RichTextRowInfo& rowInfo = *spRowInfo;
        ASSERT(nStartCharRowOffset <= rowInfo.m_charInfo.size());
        if (nStartCharRowOffset < rowInfo.m_charInfo.size()) {
            nCharWidth = (int32_t)ui::CEILF(rowInfo.m_charInfo[nStartCharRowOffset].CharWidth());
        }
    }
    return nCharWidth;
}

void RichEditData::GetCharRangeRects(int32_t nStartChar, int32_t nEndChar, std::map<int32_t, UiRectF>& rowTextRectFs)
{
    rowTextRectFs.clear();
    const int32_t nTextLength = (int32_t)GetTextLength();
    if (nTextLength == 0) {
        return;
    }

    //Check and calculate the character positions
    CheckCalcTextRects();

    if ((nStartChar < 0) || (nStartChar >= nTextLength) || (nEndChar <= nStartChar) || (nEndChar > nTextLength)) {
        return;
    }

    bool bEnd = false;
    int32_t nCurrentRowIndex = 0; //Logical row number
    int32_t nEndRowIndex = -1;
    int32_t nStartRowIndex = -1;

    size_t nRowStartCharIndex = 0;//The index of the start character in each row
    size_t nTextLen = 0; //The total length of the text
    size_t nRowTextLen = 0; //The total length of the logical rows in the physical line
    const RichTextLineInfoList& lineTextInfoList = m_lineTextInfo;
    const size_t nLineCount = lineTextInfoList.size();
    for (size_t nLineIndex = 0; nLineIndex < nLineCount; ++nLineIndex) {
        ASSERT(lineTextInfoList[nLineIndex] != nullptr);
        const RichTextLineInfo& lineTextInfo = *lineTextInfoList[nLineIndex];
        ASSERT(lineTextInfo.m_nLineTextLen > 0);
        nRowTextLen = 0;
        const size_t nRowCount = lineTextInfo.m_rowInfo.size();
        for (size_t nRow = 0; nRow < nRowCount; ++nRow) {
            ASSERT(lineTextInfo.m_rowInfo[nRow] != nullptr);

            nRowStartCharIndex = nTextLen + nRowTextLen;
            const RichTextRowInfo& rowInfo = *lineTextInfo.m_rowInfo[nRow];

            nEndRowIndex = (int32_t)(nRowStartCharIndex + rowInfo.m_charInfo.size());
            bool bFirstLine = (nStartChar >= (int32_t)nRowStartCharIndex) && (nStartChar < nEndRowIndex);
            bool bLastLine = (nEndChar >= (int32_t)nRowStartCharIndex) && (nEndChar < nEndRowIndex);

            if (bFirstLine && bLastLine) {
                //The first row and the last row are the same row
                UiRectF rowRectF = rowInfo.m_rowRect;
                rowRectF.right = rowRectF.left;
                const size_t nStartCharIndex = (size_t)nStartChar - nRowStartCharIndex;
                const size_t nEndCharIndex = (size_t)nEndChar - nRowStartCharIndex;
                for (size_t i = 0; i < nEndCharIndex; ++i) {
                    if (rowInfo.m_charInfo[i].IsIgnoredChar() || rowInfo.m_charInfo[i].IsNewLine()) {
                        continue;
                    }
                    if (i < nStartCharIndex) {
                        rowRectF.left += rowInfo.m_charInfo[i].CharWidth();
                        rowRectF.right = rowRectF.left;
                    }
                    else {
                        rowRectF.right += rowInfo.m_charInfo[i].CharWidth();
                    }
                }
                UiRectF& destRowRect = rowTextRectFs[nCurrentRowIndex];
                if (destRowRect.IsZero()) {
                    destRowRect = rowRectF;
                }
                else {
                    UnionRectF(destRowRect, rowRectF);
                }
                bEnd = true;
                break;
            }
            else if (bFirstLine) {
                //First row: select up to the end of the row
                nStartRowIndex = nCurrentRowIndex;
                UiRectF rowRectF = rowInfo.m_rowRect;
                rowRectF.right = rowRectF.left;
                const size_t nStartCharIndex = (size_t)nStartChar - nRowStartCharIndex;
                const size_t nEndCharIndex = rowInfo.m_charInfo.size();
                for (size_t i = 0; i < nEndCharIndex; ++i) {
                    if (rowInfo.m_charInfo[i].IsIgnoredChar() || rowInfo.m_charInfo[i].IsNewLine()) {
                        continue;
                    }
                    if (i < nStartCharIndex) {
                        rowRectF.left += rowInfo.m_charInfo[i].CharWidth();
                        rowRectF.right = rowRectF.left;
                    }
                    else {
                        rowRectF.right += rowInfo.m_charInfo[i].CharWidth();
                    }
                }
                UiRectF& destRowRect = rowTextRectFs[nCurrentRowIndex];
                if (destRowRect.IsZero()) {
                    destRowRect = rowRectF;
                }
                else {
                    UnionRectF(destRowRect, rowRectF);
                }
            }
            else if (bLastLine) {
                //Last row: select up to the beginning of the row
                UiRectF rowRectF = rowInfo.m_rowRect;
                rowRectF.right = rowRectF.left;
                const size_t nEndCharIndex = (size_t)nEndChar - nRowStartCharIndex;
                for (size_t i = 0; i < nEndCharIndex; ++i) {
                    if (rowInfo.m_charInfo[i].IsIgnoredChar() || rowInfo.m_charInfo[i].IsNewLine()) {
                        continue;
                    }
                    rowRectF.right += rowInfo.m_charInfo[i].CharWidth();
                }
                UiRectF& destRowRect = rowTextRectFs[nCurrentRowIndex];
                if (destRowRect.IsZero()) {
                    destRowRect = rowRectF;
                }
                else {
                    UnionRectF(destRowRect, rowRectF);
                }
                bEnd = true;
                break;
            }
            else if ((nStartRowIndex >= 0) && (nCurrentRowIndex > (int32_t)nStartRowIndex)) {
                //Middle rows
                const UiRectF& rowRectF = rowInfo.m_rowRect;
                UiRectF& destRowRect = rowTextRectFs[nCurrentRowIndex];
                if (destRowRect.IsZero()) {
                    destRowRect = rowRectF;
                }
                else {
                    UnionRectF(destRowRect, rowRectF);
                }
            }            

            nRowTextLen += rowInfo.m_charInfo.size();
            ++nCurrentRowIndex; //Increment the logical row number
        }
        nTextLen += lineTextInfo.m_nLineTextLen;
        if (bEnd) {
            break;//Already ended
        }
    }

    //Convert to external coordinates
    for (auto iter = rowTextRectFs.begin(); iter != rowTextRectFs.end(); ++iter) {
        UiRectF& rowRect = iter->second;
        ConvertToExternal(rowRect);
    }
}

const UiPoint& RichEditData::ConvertToExternal(UiPoint& pt) const
{
    UiRect rc = m_pRichText->GetRichTextDrawRect();
    pt.Offset(rc.left, rc.top);
    pt.Offset(-m_szScrollOffset.cx, -m_szScrollOffset.cy);
    return pt;
}

const UiRect& RichEditData::ConvertToExternal(UiRect& rect) const
{
    UiRect rc = m_pRichText->GetRichTextDrawRect();
    rect.Offset(rc.left, rc.top);
    rect.Offset(-m_szScrollOffset.cx, -m_szScrollOffset.cy);
    return rect;
}

const UiRectF& RichEditData::ConvertToExternal(UiRectF& rect) const
{
    UiRect rc = m_pRichText->GetRichTextDrawRect();
    rect.Offset((float)rc.left, (float)rc.top);
    rect.Offset(-(float)m_szScrollOffset.cx, -(float)m_szScrollOffset.cy);
    return rect;
}

const UiPoint& RichEditData::ConvertToInternal(UiPoint& pt) const
{
    UiRect rc = m_pRichText->GetRichTextDrawRect();
    pt.Offset(-rc.left, -rc.top);
    pt.Offset(m_szScrollOffset.cx, m_szScrollOffset.cy);
    return pt;
}

const UiRect& RichEditData::ConvertToInternal(UiRect& rect) const
{
    UiRect rc = m_pRichText->GetRichTextDrawRect();
    rect.Offset(-rc.left, -rc.top);
    rect.Offset(m_szScrollOffset.cx, m_szScrollOffset.cy);
    return rect;
}

void RichEditData::SetCacheDirty(bool bDirty)
{
    m_bCacheDirty = bDirty;
}

void RichEditData::SetDrawRichTextCache(const std::shared_ptr<DrawRichTextCache>& spDrawRichTextCache)
{
    m_spDrawRichTextCache = spDrawRichTextCache;
}

const std::shared_ptr<DrawRichTextCache>& RichEditData::GetDrawRichTextCache() const
{
    return m_spDrawRichTextCache;
}

void RichEditData::ClearDrawRichTextCache()
{
    m_spDrawRichTextCache.reset();
}

void RichEditData::SetUndoLimit(uint32_t nUndoLimit)
{
    if (m_nUndoLimit != nUndoLimit) {
        m_nUndoLimit = nUndoLimit;
        m_redoList.clear();
        while (!m_undoList.empty() && (m_undoList.size() > m_nUndoLimit)) {
            m_undoList.pop_front();
        }
    }
}

uint32_t RichEditData::GetUndoLimit() const
{
    return m_nUndoLimit;
}

void RichEditData::ClearUndoList()
{
    m_undoList.clear();
    m_redoList.clear();
}

void RichEditData::EmptyUndoBuffer()
{
    ClearUndoList();
}

void RichEditData::AddToUndoList(int32_t nStartChar, const DStringW& newText, const DStringW& oldText)
{
    ASSERT(nStartChar >= 0);
    if (nStartChar < 0) {
        return;
    }
    if (m_nUndoLimit == 0) {
        //Feature disabled
        return;
    }

    TUndoData undoData;
    undoData.m_nStartChar = nStartChar;
    undoData.m_newText = newText;
    undoData.m_oldText = oldText;

    while (!m_undoList.empty() && (m_undoList.size() >= m_nUndoLimit)) {
        m_undoList.pop_front();
    }

    //Add to the end of the undo list
    m_undoList.emplace_back(std::move(undoData));

    //After each undo is added, clear the redo list
    m_redoList.clear();
}

bool RichEditData::CanUndo() const
{
    return !m_undoList.empty();
}

bool RichEditData::Undo(int32_t& nEndCharIndex)
{
    //Check and calculate the character positions
    CheckCalcTextRects();

    bool bRet = false;
    if (!m_undoList.empty()) {
        //Take the data at the end of the undo list
        TUndoData undoData = m_undoList.back();
        m_undoList.pop_back();

        //Add to the redo list
        m_redoList.push_back(undoData);

        //Execute the undo operation
        nEndCharIndex = undoData.m_nStartChar + (int32_t)undoData.m_newText.size();
        bRet = ReplaceText(undoData.m_nStartChar, nEndCharIndex, undoData.m_oldText, false, false);
        nEndCharIndex = undoData.m_nStartChar + (int32_t)undoData.m_oldText.size();
    }
    if (!bRet) {
        nEndCharIndex = -1;
    }
    return bRet;
}

bool RichEditData::CanRedo() const
{
    return !m_redoList.empty();
}

bool RichEditData::Redo(int32_t& nEndCharIndex)
{
    //Check and calculate the character positions
    CheckCalcTextRects();

    bool bRet = false;
    if (!m_redoList.empty()) {
        //Take the data at the end of the redo list
        TUndoData undoData = m_redoList.back();
        m_redoList.pop_back();

        //Add to the undo list
        m_undoList.push_back(undoData);

        //Execute the redo operation
        nEndCharIndex = undoData.m_nStartChar + (int32_t)undoData.m_oldText.size();
        bRet = ReplaceText(undoData.m_nStartChar, nEndCharIndex, undoData.m_newText, false, false);
        nEndCharIndex = undoData.m_nStartChar + (int32_t)undoData.m_newText.size();
    }
    if (!bRet) {
        nEndCharIndex = -1;
    }
    return bRet;
}

void RichEditData::Clear()
{
    RichTextLineInfoList lineTextInfo;
    m_lineTextInfo.swap(lineTextInfo);
    m_spDrawRichTextCache.reset();
    m_rcTextRect.Clear();

    std::vector<int32_t> temp;
    m_rowXOffset.swap(temp);
    m_bTextRectXOffsetUpdated = false;
    m_bTextRectYOffsetUpdated = false;

    ClearUndoList();
    SetCacheDirty(false);
}

int32_t RichEditData::GetRowCount()
{
    //Check and calculate the character positions
    CheckCalcTextRects();

    int32_t nRowIndex = 0; //Row number
    const size_t nLineCount = m_lineTextInfo.size();
    for (size_t nIndex = 0; nIndex < nLineCount; ++nIndex) {
        const RichTextLineInfo& lineText = *m_lineTextInfo[nIndex];
        ASSERT(lineText.m_nLineTextLen > 0);
        nRowIndex += (int32_t)lineText.m_rowInfo.size();
    }
    return nRowIndex;
}

DStringW RichEditData::GetRowText(int32_t nRowIndex)
{
    //Check and calculate the character positions
    CheckCalcTextRects();

    DStringW rowText;
    bool bFound = false;
    int32_t nRows = 0; //Logical row number
    const size_t nLineCount = m_lineTextInfo.size();
    for (size_t nIndex = 0; nIndex < nLineCount; ++nIndex) {
        const RichTextLineInfo& lineText = *m_lineTextInfo[nIndex];
        ASSERT(lineText.m_nLineTextLen > 0);
        const size_t nRowCount = lineText.m_rowInfo.size();
        for (size_t nRow = 0; nRow < nRowCount; ++nRow) {
            if (nRows == nRowIndex) {
                //Locate to this row
                ASSERT(!lineText.m_rowInfo[nRow]->m_charInfo.empty());
                size_t nStartIndex = 0;
                for (size_t i = 0; i < nRow; ++i) {
                    nStartIndex += lineText.m_rowInfo[i]->m_charInfo.size();
                }
                if (!lineText.m_rowInfo[nRow]->m_charInfo.empty()) {
                    ASSERT(nStartIndex < lineText.m_nLineTextLen);
                    std::wstring_view lineView(lineText.m_lineText.c_str(), lineText.m_nLineTextLen);
                    rowText = lineView.substr(nStartIndex, lineText.m_rowInfo[nRow]->m_charInfo.size());
                }
                bFound = true;
                break;
            }
            ++nRows;
        }
        if (bFound) {
            break;
        }
    }
    return rowText;
}

int32_t RichEditData::RowIndex(int32_t nRowIndex)
{
    //Check and calculate the character positions
    CheckCalcTextRects();

    int32_t nRowStartIndex = -1;
    bool bFound = false;
    int32_t nRows = 0; //Logical row number
    int32_t nCharCount = 0; //Total number of characters
    const size_t nLineCount = m_lineTextInfo.size();
    for (size_t nIndex = 0; nIndex < nLineCount; ++nIndex) {
        const RichTextLineInfo& lineText = *m_lineTextInfo[nIndex];
        ASSERT(lineText.m_nLineTextLen > 0);
        const size_t nRowCount = lineText.m_rowInfo.size();
        for (size_t nRow = 0; nRow < nRowCount; ++nRow) {
            if (nRows == nRowIndex) {
                //Locate to this row
                nRowStartIndex = nCharCount;
                bFound = true;
                break;
            }
            else {
                nCharCount += (int32_t)lineText.m_rowInfo[nRow]->m_charInfo.size();
            }
            ++nRows;
        }
        if (bFound) {
            break;
        }
    }
    return nRowStartIndex;
}

int32_t RichEditData::RowLength(int32_t nRowIndex)
{
    //Check and calculate the character positions
    CheckCalcTextRects();

    int32_t nRowLength = 0;
    bool bFound = false;
    int32_t nRows = 0; //Logical row number
    const size_t nLineCount = m_lineTextInfo.size();
    for (size_t nIndex = 0; nIndex < nLineCount; ++nIndex) {
        const RichTextLineInfo& lineText = *m_lineTextInfo[nIndex];
        ASSERT(lineText.m_nLineTextLen > 0);
        const size_t nRowCount = lineText.m_rowInfo.size();
        for (size_t nRow = 0; nRow < nRowCount; ++nRow) {
            if (nRows == nRowIndex) {
                //Get the character length of this row
                nRowLength = (int32_t)lineText.m_rowInfo[nRow]->m_charInfo.size();
                bFound = true;
                break;
            }
            ++nRows;
        }
        if (bFound) {
            break;
        }
    }
    return nRowLength;
}

int32_t RichEditData::RowFromChar(int32_t nCharIndex)
{
    ASSERT(nCharIndex >= 0);
    if (nCharIndex < 0) {
        return 0;
    }
    const int32_t nTextLength = (int32_t)GetTextLength();
    if (nTextLength < 1) {
        return 0;
    }
    //Check and calculate the character positions
    CheckCalcTextRects();

    int32_t nRowIndex = 0; //Logical row number
    size_t nTextLen = 0;   //The total length of the text
    const size_t nLineCount = m_lineTextInfo.size();
    for (size_t nIndex = 0; nIndex < nLineCount; ++nIndex) {
        const RichTextLineInfo& lineText = *m_lineTextInfo[nIndex];
        ASSERT(lineText.m_nLineTextLen > 0);
        nTextLen += lineText.m_nLineTextLen;
        if (nCharIndex < (int32_t)nTextLen) {
            //Locate to the line
            const size_t nStartBaseLen = nTextLen - lineText.m_nLineTextLen;
            const size_t nStartLineOffset = (size_t)nCharIndex - nStartBaseLen;
            ASSERT(nStartLineOffset < lineText.m_nLineTextLen);
            //Locate it in the physical line, then locate it in which logical row
            size_t nRowTextLen = 0;
            const size_t nRowCount = lineText.m_rowInfo.size();
            for (size_t nRow = 0; nRow < nRowCount; ++nRow) {
                ASSERT(lineText.m_rowInfo[nRow] != nullptr);
                const RichTextRowInfo& rowInfo = *lineText.m_rowInfo[nRow];
                nRowTextLen += rowInfo.m_charInfo.size();
                if (nStartLineOffset < nRowTextLen) {
                    //Locate it in this logical row
                    break;
                }
                else {
                    ++nRowIndex;
                }
            }
            break;
        }
        else {
            nRowIndex += (int32_t)lineText.m_rowInfo.size();
        }
    }
    return nRowIndex;
}

void RichEditData::TruncateLimitText(DStringW& text, int32_t nLimitLen) const
{
    if (nLimitLen <= 0) {
        return;
    }
    if ((int32_t)text.size() > nLimitLen) {
        DStringW::value_type ch = text.at(nLimitLen);
        text.resize((size_t)nLimitLen);
        if ((ch == L'\n') && (text.back() == L'\r')) {
            text.pop_back();
        }
    }
}

bool RichEditData::FindRichText(bool bMatchCase, bool bMatchWholeWord, bool bFindDown,
                                int32_t nFindStartChar, int32_t nFindEndChar,
                                const DStringW& findText,
                                int32_t& nFoundStartChar, int32_t& nFoundEndChar) const
{
    if (findText.empty() || (nFindStartChar == nFindEndChar) || (nFindStartChar < 0) || (nFindEndChar < 0)){
        return false;
    }
    const int32_t nTextLen = (int32_t)GetTextLength();
    if (nFindStartChar > nTextLen) {
        nFindStartChar = nTextLen;
    }
    if (nFindEndChar > nTextLen) {
        nFindEndChar = nTextLen;
    }

    int32_t nStartChar = nFindStartChar;
    int32_t nEndChar = nFindEndChar;
    if (!bFindDown) {
        nStartChar = nFindEndChar;
        nEndChar = nFindStartChar;
    }

    DStringW text = GetTextRange(nStartChar, nEndChar);
    DStringW findTextW = findText;
    if (!bMatchCase) {
        text = StringUtil::MakeLowerString(text);
        findTextW = StringUtil::MakeLowerString(findTextW);
    }

    size_t nPos = bFindDown ? text.find(findTextW) : text.rfind(findTextW);
    if (!bMatchWholeWord) {
        //Not a whole word match, return after one search
        bool bFound = (nPos != DStringW::npos) ? true : false;
        if (bFound) {
            nFoundStartChar = nStartChar + (int32_t)nPos;
            nFoundEndChar = nFoundStartChar + (int32_t)findTextW.size();
        }
        return bFound;
    }

    //Whole word match
    bool bFound = false;
    while (nPos != DStringW::npos) {
        bFound = true;
        if (iswalnum(findTextW[0])) {
            if (nPos == 0) {
                //The first character
                int32_t nStartCharIndex = nStartChar + (int32_t)nPos;
                if (nStartCharIndex > 0) {
                    DStringW::value_type charBeforeStart = 0;
                    DStringW temp = GetTextRange(nStartCharIndex - 1, nStartCharIndex);
                    ASSERT(temp.size() == 1);
                    if (temp.size() == 1) {
                        charBeforeStart = temp[0];
                    }
                    if (iswalnum(charBeforeStart)) {
                        bFound = false;
                    }
                }
            }
            else {
                //Not the first character
                if (iswalnum(text[nPos - 1])) {
                    bFound = false;
                }
            }
        }
        if (iswalnum(findTextW[findTextW.size() - 1])) {
            if ((nPos + findTextW.size()) >= text.size()) {
                //The last character
                int32_t nEndCharIndex = nStartChar + (int32_t)nPos + (int32_t)findTextW.size();
                if (nEndCharIndex < nTextLen) {
                    DStringW::value_type charAfterEnd = 0;
                    DStringW temp = GetTextRange(nEndCharIndex, nEndCharIndex + 1);
                    ASSERT(temp.size() == 1);
                    if (temp.size() == 1) {
                        charAfterEnd = temp[0];
                    }
                    if (iswalnum(charAfterEnd)) {
                        bFound = false;
                    }
                }
            }
            else {
                //Not the last character
                if (iswalnum(text[nPos + findTextW.size()])) {
                    bFound = false;
                }
            }            
        }
        if(bFound) {
            nFoundStartChar = nStartChar + (int32_t)nPos;
            nFoundEndChar = nFoundStartChar + (int32_t)findTextW.size();
            break;
        }
        else {
            //Continue searching
            if (!bFindDown && (nPos == 0)) {
                //Already searched to the start position of the string, no match found
                break;
            }
            size_t nLastPos = nPos;
            nPos = bFindDown ? text.find(findTextW, nPos + 1) : text.rfind(findTextW, nPos - 1);
            ASSERT(nLastPos != nPos);
            if (nLastPos == nPos) {
                //Avoid an infinite loop
                break;
            }
        }
    }
    return bFound;
}

} //namespace ui

