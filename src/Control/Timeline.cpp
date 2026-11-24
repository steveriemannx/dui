#include "dui/Control/Timeline.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Core/Window.h"
#include "dui/Render/IRender.h"
#include "dui/Render/AutoClip.h"
#include "dui/Utils/StringUtil.h"
#include <algorithm>
#include <cmath>
#include <limits>

namespace ui
{

Timeline::Timeline(Window* pWindow) :
    BaseClass(pWindow),
    m_strLineColor("border_control_normal"),
    m_strScrollbarTrackColor("bg_scrollbar_normal"),
    m_strScrollbarThumbColor("bg_scrollbar_thumb_normal"),
    m_bAutoHideScrollbar(true),
    m_bMouseHover(false),
    m_lineStyle(TimelineLineStyle::kSolid),
    m_bReverse(false),
    m_nNodeSize(12),
    m_nItemSpacing(24),
    m_bLoading(false),
    m_nScrollPos(0),
    m_nContentHeight(0),
    m_bDraggingScrollbar(false),
    m_dragStartY(0),
    m_dragStartScrollPos(0)
{
    // Default node colors for each status (semantic color names)
    m_nodeColors[static_cast<int>(TimelineNodeStatus::kPending)] = "text_muted";
    m_nodeColors[static_cast<int>(TimelineNodeStatus::kInProgress)] = "color_accent";
    m_nodeColors[static_cast<int>(TimelineNodeStatus::kCompleted)] = "color_success";
    m_nodeColors[static_cast<int>(TimelineNodeStatus::kFailed)] = "color_error";
    m_nodeColors[static_cast<int>(TimelineNodeStatus::kCustom)] = "";
}

Timeline::~Timeline()
{
}

std::string Timeline::GetType() const
{
    return DUI_CTR_TIMELINE;
}

void Timeline::SetAttribute(const std::string& strName, const std::string& strValue)
{
    if ((strName == "item_time") || (strName == "itemtime")) {
        m_strItemTimeIds.clear();
        ParseItemsField(strValue, 0);
    }
    else if ((strName == "item_title") || (strName == "itemtitle")) {
        m_strItemTitleIds.clear();
        ParseItemsField(strValue, 1);
    }
    else if ((strName == "item_content") || (strName == "itemcontent")) {
        m_strItemContentIds.clear();
        ParseItemsField(strValue, 2);
    }
    else if ((strName == "item_status") || (strName == "itemstatus")) {
        ParseItemsField(strValue, 3);
    }
    else if ((strName == "item_color") || (strName == "itemcolor")) {
        ParseItemsField(strValue, 4);
    }
    else if ((strName == "item_time_id") || (strName == "itemtimeid")) {
        m_strItemTimeIds = strValue;
        ParseItemsField(strValue, 0, true);
    }
    else if ((strName == "item_title_id") || (strName == "itemtitleid")) {
        m_strItemTitleIds = strValue;
        ParseItemsField(strValue, 1, true);
    }
    else if ((strName == "item_content_id") || (strName == "itemcontentid")) {
        m_strItemContentIds = strValue;
        ParseItemsField(strValue, 2, true);
    }
    else if ((strName == "node_color_pending") || (strName == "nodecolorpending")) {
        SetNodeColor(TimelineNodeStatus::kPending, strValue);
    }
    else if ((strName == "node_color_inprogress") || (strName == "nodecolorinprogress")) {
        SetNodeColor(TimelineNodeStatus::kInProgress, strValue);
    }
    else if ((strName == "node_color_completed") || (strName == "nodecolorcompleted")) {
        SetNodeColor(TimelineNodeStatus::kCompleted, strValue);
    }
    else if ((strName == "node_color_failed") || (strName == "nodecolorfailed")) {
        SetNodeColor(TimelineNodeStatus::kFailed, strValue);
    }
    else if ((strName == "line_color") || (strName == "linecolor")) {
        SetLineColor(strValue);
    }
    else if ((strName == "scrollbar_track_color") || (strName == "scrollbartrackcolor")) {
        SetScrollbarTrackColor(strValue);
    }
    else if ((strName == "scrollbar_thumb_color") || (strName == "scrollbarthumbcolor")) {
        SetScrollbarThumbColor(strValue);
    }
    else if ((strName == "auto_hide_scrollbar") || (strName == "autohidescrollbar")) {
        SetAutoHideScrollbar((strValue == "true"));
    }
    else if ((strName == "line_style") || (strName == "linestyle")) {
        if (strValue == "dashed") {
            SetLineStyle(TimelineLineStyle::kDashed);
        }
        else {
            SetLineStyle(TimelineLineStyle::kSolid);
        }
    }
    else if (strName == "reverse") {
        SetReverse((strValue == "true"));
    }
    else if ((strName == "node_size") || (strName == "nodesize")) {
        SetNodeSize(StringUtil::StringToInt32(strValue));
    }
    else if ((strName == "item_spacing") || (strName == "itemspacing")) {
        SetItemSpacing(StringUtil::StringToInt32(strValue));
    }
    else if ((strName == "empty_text") || (strName == "emptytext")) {
        SetEmptyText(strValue);
    }
    else if ((strName == "empty_text_id") || (strName == "emptytextid")) {
        SetEmptyTextId(strValue);
    }
    else if (strName == "loading") {
        SetLoading((strValue == "true"));
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

void Timeline::OnLanguageChanged()
{
    BaseClass::OnLanguageChanged();
    // When the language changes, re-resolve the multi-language data bindings (item_*_id and empty_text_id)
    if (!m_strItemTimeIds.empty()) {
        ParseItemsField(m_strItemTimeIds, 0, true);
    }
    if (!m_strItemTitleIds.empty()) {
        ParseItemsField(m_strItemTitleIds, 1, true);
    }
    if (!m_strItemContentIds.empty()) {
        ParseItemsField(m_strItemContentIds, 2, true);
    }
    if (!m_strEmptyTextId.empty()) {
        m_strEmptyText = GlobalManager::Instance().Lang().GetStringViaID(m_strEmptyTextId);
        Invalidate();
    }
}

void Timeline::ParseItemsField(const std::string& strValue, int32_t nField, bool bTranslate)
{
    // Split the items by '|' and fill the specified field one by one; missing items are appended as empty items
    std::list<std::string> parts = StringUtil::Split(strValue, "|");
    size_t nIndex = 0;
    for (const std::string& part : parts) {
        std::string s = part;
        StringUtil::Trim(s);
        if (bTranslate && !s.empty()) {
            // Multi-language ID mode: fetch the actual text from the language file by STRID
            s = GlobalManager::Instance().Lang().GetStringViaID(s);
        }
        if (nIndex >= m_items.size()) {
            m_items.push_back(Item());
        }
        Item& item = m_items[nIndex];
        switch (nField) {
        case 0: item.time = s; break;
        case 1: item.title = s; break;
        case 2: item.content = s; break;
        case 3:
            if (s == "pending") {
                item.status = TimelineNodeStatus::kPending;
            }
            else if ((s == "inprogress") || (s == "in_progress")) {
                item.status = TimelineNodeStatus::kInProgress;
            }
            else if (s == "completed") {
                item.status = TimelineNodeStatus::kCompleted;
            }
            else if (s == "failed") {
                item.status = TimelineNodeStatus::kFailed;
            }
            else {
                item.status = TimelineNodeStatus::kCustom;
            }
            break;
        case 4: item.customColor = s; break;
        default: break;
        }
        ++nIndex;
    }
    m_nContentHeight = 0;
    Invalidate();
}

void Timeline::AddItem(const Item& item)
{
    m_items.push_back(item);
    m_nContentHeight = 0;
    // Make the new item visible after appending: in reverse order the new item is at the visual top (scroll to the top), in normal order it is at the visual bottom (scroll to the bottom, the excess is clamped while painting)
    m_nScrollPos = m_bReverse ? 0 : std::numeric_limits<int32_t>::max();
    Invalidate();
}

void Timeline::AddItem(const std::string& time, const std::string& title, const std::string& content,
                       TimelineNodeStatus status)
{
    Item item;
    item.time = time;
    item.title = title;
    item.content = content;
    item.status = status;
    AddItem(item);
}

bool Timeline::InsertItem(size_t index, const Item& item)
{
    if (index > m_items.size()) {
        return false;
    }
    m_items.insert(m_items.begin() + index, item);
    m_nContentHeight = 0;
    Invalidate();
    return true;
}

bool Timeline::RemoveItemAt(size_t index)
{
    if (index >= m_items.size()) {
        return false;
    }
    m_items.erase(m_items.begin() + index);
    m_nContentHeight = 0;
    Invalidate();
    return true;
}

void Timeline::ClearItems()
{
    if (!m_items.empty()) {
        m_items.clear();
        m_nContentHeight = 0;
        m_nScrollPos = 0;
        Invalidate();
    }
}

void Timeline::SetItems(const std::vector<Item>& items)
{
    m_items = items;
    m_nContentHeight = 0;
    m_nScrollPos = 0;
    Invalidate();
}

const Timeline::Item* Timeline::GetItemAt(size_t index) const
{
    if (index >= m_items.size()) {
        return nullptr;
    }
    return &m_items[index];
}

void Timeline::SetNodeColor(TimelineNodeStatus status, const std::string& strColor)
{
    int32_t nIndex = static_cast<int>(status);
    if ((nIndex >= 0) && (nIndex < 5)) {
        m_nodeColors[nIndex] = strColor;
        Invalidate();
    }
}

const std::string& Timeline::GetNodeColor(TimelineNodeStatus status) const
{
    int32_t nIndex = static_cast<int>(status);
    if ((nIndex >= 0) && (nIndex < 5)) {
        return m_nodeColors[nIndex];
    }
    static const std::string kEmpty;
    return kEmpty;
}

void Timeline::SetLineColor(const std::string& strColor)
{
    m_strLineColor = strColor;
    Invalidate();
}

void Timeline::SetScrollbarTrackColor(const std::string& strColor)
{
    if (m_strScrollbarTrackColor != strColor) {
        m_strScrollbarTrackColor = strColor;
        Invalidate();
    }
}

void Timeline::SetScrollbarThumbColor(const std::string& strColor)
{
    if (m_strScrollbarThumbColor != strColor) {
        m_strScrollbarThumbColor = strColor;
        Invalidate();
    }
}

void Timeline::SetAutoHideScrollbar(bool bAutoHide)
{
    if (m_bAutoHideScrollbar != bAutoHide) {
        m_bAutoHideScrollbar = bAutoHide;
        Invalidate();
    }
}

void Timeline::SetLineStyle(TimelineLineStyle style)
{
    if (m_lineStyle != style) {
        m_lineStyle = style;
        Invalidate();
    }
}

void Timeline::SetReverse(bool bReverse)
{
    if (m_bReverse != bReverse) {
        m_bReverse = bReverse;
        Invalidate();
    }
}

void Timeline::SetNodeSize(int32_t nSize)
{
    if (nSize < 4) {
        nSize = 4;
    }
    if (nSize > 64) {
        nSize = 64;
    }
    if (m_nNodeSize != nSize) {
        m_nNodeSize = nSize;
        Invalidate();
    }
}

void Timeline::SetItemSpacing(int32_t nSpacing)
{
    if (nSpacing < 0) {
        nSpacing = 0;
    }
    if (m_nItemSpacing != nSpacing) {
        m_nItemSpacing = nSpacing;
        Invalidate();
    }
}

void Timeline::SetEmptyText(const std::string& strText)
{
    if (m_strEmptyText != strText) {
        m_strEmptyText = strText;
        m_strEmptyTextId.clear();
        Invalidate();
    }
}

void Timeline::SetEmptyTextId(const std::string& strTextId)
{
    if (m_strEmptyTextId != strTextId) {
        m_strEmptyTextId = strTextId;
        m_strEmptyText = GlobalManager::Instance().Lang().GetStringViaID(strTextId);
        Invalidate();
    }
}

void Timeline::SetLoading(bool bLoading)
{
    if (m_bLoading != bLoading) {
        m_bLoading = bLoading;
        Invalidate();
    }
}

void Timeline::SetItemDrawCallback(const ItemDrawCallback& callback)
{
    m_itemDrawCallback = callback;
    Invalidate();
}

void Timeline::AttachItemClick(const EventCallback& callback, EventCallbackID callbackID)
{
    AttachEvent(kEventTimelineItemClick, callback, callbackID);
}

void Timeline::SetScrollPos(int32_t nPos)
{
    if (nPos < 0) {
        nPos = 0;
    }
    if (m_nScrollPos != nPos) {
        m_nScrollPos = nPos;
        Invalidate();
    }
}

void Timeline::ScrollToItem(size_t index)
{
    // Compute the top Y of the target item and scroll it into view (simplified: estimated from the index ratio)
    if (index >= m_items.size()) {
        return;
    }
    // The exact position is computed in Paint; use a rough estimate here and clamp it in UpdateScrollRange after the repaint
    if (m_items.empty()) {
        return;
    }
    UiRect rc = GetRect();
    UiPadding rcPadding = GetControlPadding();
    rc.Deflate(rcPadding);
    if (rc.Height() <= 0) {
        return;
    }
    // Rough estimate: the ratio of the item index to the total count
    int32_t nRough = static_cast<int32_t>(static_cast<double>(index) * static_cast<double>(m_nContentHeight) /
                                         static_cast<double>(m_items.size()));
    nRough -= rc.Height() / 2;
    SetScrollPos(nRough);
}

void Timeline::UpdateScrollRange(IRender* pRender, const UiRect& rcChart)
{
    // Compute the total content height and clamp the scroll offset accordingly
    UiRect rcContent = CalcContentRect(rcChart);
    m_nContentHeight = CalcTotalHeight(pRender, rcContent);
    int32_t nViewHeight = rcChart.Height();
    int32_t nMaxScroll = std::max(0, m_nContentHeight - nViewHeight);
    if (m_nScrollPos > nMaxScroll) {
        m_nScrollPos = nMaxScroll;
    }
    if (m_nScrollPos < 0) {
        m_nScrollPos = 0;
    }
}

UiRect Timeline::CalcContentRect(const UiRect& rcChart) const
{
    UiRect rc = rcChart;
    // Width of the node column: the node diameter plus the left and right margins
    int32_t nNodeColumn = Dpi().GetScaleInt(m_nNodeSize) + Dpi().GetScaleInt(16);
    rc.left += nNodeColumn;
    rc.right -= Dpi().GetScaleInt(8);
    return rc;
}

int32_t Timeline::GetNodeCenterX(const UiRect& rcChart) const
{
    return rcChart.left + Dpi().GetScaleInt(m_nNodeSize) / 2 + Dpi().GetScaleInt(8);
}

void Timeline::PaintStateImages(IRender* pRender)
{
    BaseClass::PaintStateImages(pRender);
    if (pRender == nullptr) {
        return;
    }
    UiRect rc = GetRect();
    UiPadding rcPadding = GetControlPadding();
    rc.Deflate(rcPadding);
    if ((rc.Width() <= 0) || (rc.Height() <= 0)) {
        return;
    }

    // Loading state
    if (m_bLoading) {
        UiRect rcText = rc;
        DrawTimelineText(pRender, "...", rcText, TEXT_HCENTER | TEXT_VCENTER | TEXT_SINGLELINE, "text_muted", "system_regular_14");
        return;
    }

    // Empty data
    if (m_items.empty()) {
        if (!m_strEmptyText.empty()) {
            UiRect rcText = rc;
            DrawTimelineText(pRender, m_strEmptyText, rcText, TEXT_HCENTER | TEXT_VCENTER | TEXT_SINGLELINE, "text_muted", "system_regular_14");
        }
        return;
    }

    // Update the scroll range
    UpdateScrollRange(pRender, rc);

    UiRect rcContent = CalcContentRect(rc);
    //int32_t nNodeCenterX = GetNodeCenterX(rc);
    int32_t nNodeRadius = Dpi().GetScaleInt(m_nNodeSize) / 2;
    int32_t nSpacing = Dpi().GetScaleInt(m_nItemSpacing);

    // Clip to the control area
    std::unique_ptr<AutoClip> spClip = CreateRectClip(pRender, rc, true);

    // Paint the items one by one (in normal or reverse order)
    int32_t y = rc.top - m_nScrollPos;
    const size_t nCount = m_items.size();
    for (size_t vi = 0; vi < nCount; ++vi) {
        size_t index = m_bReverse ? (nCount - 1 - vi) : vi;
        const Item& item = m_items[index];
        int32_t nItemHeight = CalcItemHeight(pRender, item, rcContent.Width());
        UiRect rcItem(rc.left, y, rc.right, y + nItemHeight);
        // Paint only the visible items
        if (rcItem.bottom > rc.top && rcItem.top < rc.bottom) {
            int32_t nNodeCenterY = y + Dpi().GetScaleInt(10) + nNodeRadius;
            bool bIsLast = (vi == nCount - 1);
            PaintItem(pRender, index, item, rcItem, rcContent, nNodeCenterY, bIsLast);
        }
        y += nItemHeight + nSpacing;
    }
    spClip.reset();

    // Paint the scroll bar (when the content overflows, it indicates that more data is available outside the view)
    DrawScrollbar(pRender, rc);
}

int32_t Timeline::CalcItemHeight(IRender* pRender, const Item& item, int32_t nContentWidth)
{
    if (pRender == nullptr) {
        return 0;
    }
    int32_t nTimeH = MeasureTextHeight(pRender, item.time, nContentWidth, "system_regular_12");
    int32_t nTitleH = MeasureTextHeight(pRender, item.title, nContentWidth, "system_bold_14");
    int32_t nContentH = MeasureTextHeight(pRender, item.content, nContentWidth, "system_regular_14");
    return nTimeH + nTitleH + nContentH + Dpi().GetScaleInt(8);
}

int32_t Timeline::CalcTotalHeight(IRender* pRender, const UiRect& rcContent)
{
    int32_t nTotal = 0;
    int32_t nSpacing = Dpi().GetScaleInt(m_nItemSpacing);
    for (const Item& item : m_items) {
        nTotal += CalcItemHeight(pRender, item, rcContent.Width());
    }
    if (!m_items.empty()) {
        nTotal += static_cast<int32_t>(m_items.size() - 1) * nSpacing;
    }
    return nTotal;
}

void Timeline::PaintItem(IRender* pRender, size_t index, const Item& item, const UiRect& rcItem,
                         const UiRect& rcContent, int32_t nNodeCenterY, bool bIsLast)
{
    // Custom paint callback
    if (m_itemDrawCallback) {
        UiRect rcNode(GetNodeCenterX(GetRect()) - Dpi().GetScaleInt(m_nNodeSize) / 2,
                      nNodeCenterY - Dpi().GetScaleInt(m_nNodeSize) / 2,
                      GetNodeCenterX(GetRect()) + Dpi().GetScaleInt(m_nNodeSize) / 2,
                      nNodeCenterY + Dpi().GetScaleInt(m_nNodeSize) / 2);
        if (m_itemDrawCallback(pRender, item, rcItem, rcNode, index)) {
            return;
        }
    }

    int32_t nNodeCenterX = GetNodeCenterX(GetRect());
    int32_t nNodeRadius = Dpi().GetScaleInt(m_nNodeSize) / 2;
    UiColor lineColor = GetUiColor(m_strLineColor);
    if (lineColor.IsEmpty()) {
        lineColor = UiColor(0xFFD0D0D0);
    }
    UiColor nodeColor = GetNodeUiColor(item);

    // Connector line (below the node, extending down to the node of the next item)
    if (!bIsLast) {
        int32_t nSpacing = Dpi().GetScaleInt(m_nItemSpacing);
        int32_t nLineTop = nNodeCenterY + nNodeRadius;
        int32_t nLineBottom = rcItem.bottom + nSpacing - Dpi().GetScaleInt(10);
        DrawLine(pRender, nNodeCenterX, nLineTop, nLineBottom, lineColor);
    }

    // Node circle (fill + stroke)
    pRender->FillCircle(UiPointF(static_cast<float>(nNodeCenterX), static_cast<float>(nNodeCenterY)),
                        static_cast<float>(nNodeRadius), nodeColor, GetAlpha());
    // White stroke, to make the node stand out
    pRender->DrawCircle(UiPointF(static_cast<float>(nNodeCenterX), static_cast<float>(nNodeCenterY)),
                        static_cast<float>(nNodeRadius), UiColor(0xFFFFFFFF), 2.0f);

    // Content area text
    int32_t nTextY = rcItem.top;
    // time (small text)
    if (!item.time.empty()) {
        UiRect rcTime(rcContent.left, nTextY, rcContent.right, nTextY + Dpi().GetScaleInt(18));
        DrawTimelineText(pRender, item.time, rcTime, TEXT_LEFT | TEXT_TOP | TEXT_SINGLELINE, "text_muted", "system_regular_12");
        nTextY += Dpi().GetScaleInt(18);
    }
    // title (bold)
    if (!item.title.empty()) {
        int32_t nTitleH = MeasureTextHeight(pRender, item.title, rcContent.Width(), "system_bold_14");
        UiRect rcTitle(rcContent.left, nTextY, rcContent.right, nTextY + nTitleH);
        DrawTimelineText(pRender, item.title, rcTitle, TEXT_LEFT | TEXT_TOP, "text_default", "system_bold_14");
        nTextY += nTitleH;
    }
    // content (body text, multiple lines)
    if (!item.content.empty()) {
        int32_t nContentH = MeasureTextHeight(pRender, item.content, rcContent.Width(), "system_regular_14");
        UiRect rcText(rcContent.left, nTextY, rcContent.right, nTextY + nContentH);
        DrawTimelineText(pRender, item.content, rcText, TEXT_LEFT | TEXT_TOP, "text_default", "system_regular_14");
    }
}

UiColor Timeline::GetNodeUiColor(const Item& item) const
{
    if (item.status == TimelineNodeStatus::kCustom) {
        UiColor c = GetUiColor(item.customColor);
        if (!c.IsEmpty()) {
            return c;
        }
        // The custom color is empty, fall back to the in-progress color
        return GetUiColor(m_nodeColors[static_cast<int>(TimelineNodeStatus::kInProgress)]);
    }
    UiColor c = GetUiColor(m_nodeColors[static_cast<int>(item.status)]);
    if (c.IsEmpty()) {
        c = UiColor(0xFF999999);
    }
    return c;
}

void Timeline::DrawLine(IRender* pRender, int32_t x, int32_t y1, int32_t y2, UiColor color)
{
    if (pRender == nullptr) {
        return;
    }
    if (m_lineStyle == TimelineLineStyle::kSolid) {
        pRender->DrawLine(UiPointF(static_cast<float>(x), static_cast<float>(y1)),
                          UiPointF(static_cast<float>(x), static_cast<float>(y2)),
                          color, 2.0f);
    }
    else {
        // Dashed line: drawn in segments of a 4px dash and a 4px gap
        const int32_t nDash = Dpi().GetScaleInt(4);
        const int32_t nGap = Dpi().GetScaleInt(4);
        for (int32_t y = y1; y < y2; y += nDash + nGap) {
            int32_t yEnd = std::min(y + nDash, y2);
            pRender->DrawLine(UiPointF(static_cast<float>(x), static_cast<float>(y)),
                              UiPointF(static_cast<float>(x), static_cast<float>(yEnd)),
                              color, 2.0f);
        }
    }
}

void Timeline::DrawTimelineText(IRender* pRender, const std::string& strText, const UiRect& rc, uint32_t uFormat,
                                const std::string& strColor, const std::string& strFontId)
{
    if ((pRender == nullptr) || strText.empty()) {
        return;
    }
    UiColor textColor = GetUiColor(strColor);
    if (textColor.IsEmpty()) {
        textColor = UiColor(0xFF000000);
    }
    DrawStringParam param;
    param.textRect = rc;
    param.dwTextColor = textColor;
    param.pFont = GetIFontById(strFontId);
    param.uFormat = uFormat;
    pRender->DrawString(strText, param);
}

int32_t Timeline::MeasureTextHeight(IRender* pRender, const std::string& strText, int32_t nWidth, const std::string& strFontId)
{
    if ((pRender == nullptr) || strText.empty()) {
        return 0;
    }
    MeasureStringParam param;
    param.pFont = GetIFontById(strFontId);
    param.rectSize = nWidth;
    UiRect rc = pRender->MeasureString(strText, param);
    return rc.Height();
}

int32_t Timeline::HitTestNode(IRender* pRender, const UiPoint& ptMouse)
{
    if ((pRender == nullptr) || m_items.empty()) {
        return -1;
    }
    UiRect rc = GetRect();
    UiPadding rcPadding = GetControlPadding();
    rc.Deflate(rcPadding);
    if ((rc.Width() <= 0) || (rc.Height() <= 0)) {
        return -1;
    }
    UiRect rcContent = CalcContentRect(rc);
    int32_t nNodeCenterX = GetNodeCenterX(rc);
    int32_t nNodeRadius = Dpi().GetScaleInt(m_nNodeSize) / 2;
    int32_t nHitRadius = nNodeRadius + Dpi().GetScaleInt(4);
    int32_t nSpacing = Dpi().GetScaleInt(m_nItemSpacing);

    int32_t y = rc.top - m_nScrollPos;
    const size_t nCount = m_items.size();
    for (size_t vi = 0; vi < nCount; ++vi) {
        size_t index = m_bReverse ? (nCount - 1 - vi) : vi;
        const Item& item = m_items[index];
        int32_t nItemHeight = CalcItemHeight(pRender, item, rcContent.Width());
        int32_t nNodeCenterY = y + Dpi().GetScaleInt(10) + nNodeRadius;
        int32_t dx = ptMouse.x - nNodeCenterX;
        int32_t dy = ptMouse.y - nNodeCenterY;
        if ((dx * dx + dy * dy) <= nHitRadius * nHitRadius) {
            return static_cast<int32_t>(index);
        }
        y += nItemHeight + nSpacing;
    }
    return -1;
}

bool Timeline::ButtonDown(const EventArgs& msg)
{
    if (!IsEnabled()) {
        return BaseClass::ButtonDown(msg);
    }
    // Dragging the scroll bar thumb takes precedence
    if (HitTestScrollThumb(msg.ptMouse)) {
        m_bDraggingScrollbar = true;
        m_dragStartY = msg.ptMouse.y;
        m_dragStartScrollPos = m_nScrollPos;
        if (GetWindow() != nullptr) {
            GetWindow()->SetCapture();
        }
        return true;
    }
    IRender* pRender = (GetWindow() != nullptr) ? GetWindow()->GetRender() : nullptr;
    int32_t nHit = HitTestNode(pRender, msg.ptMouse);
    if (nHit >= 0) {
        SendItemClickEvent(static_cast<size_t>(nHit));
        return true;
    }
    return BaseClass::ButtonDown(msg);
}

bool Timeline::ButtonUp(const EventArgs& msg)
{
    if (m_bDraggingScrollbar) {
        m_bDraggingScrollbar = false;
        if (GetWindow() != nullptr) {
            GetWindow()->ReleaseCapture();
        }
        return true;
    }
    return BaseClass::ButtonUp(msg);
}

bool Timeline::MouseMove(const EventArgs& msg)
{
    if (m_bDraggingScrollbar) {
        // Update the scroll offset while dragging the thumb
        UiRect rc = GetRect();
        UiPadding rcPadding = GetControlPadding();
        rc.Deflate(rcPadding);
        int32_t nViewHeight = rc.Height();
        int32_t nMaxScroll = std::max(0, m_nContentHeight - nViewHeight);
        if (nMaxScroll <= 0) {
            return true;
        }
        // Draggable track range = track height - thumb height
        int32_t nTrackHeight = rc.Height() - Dpi().GetScaleInt(4);
        int32_t nThumbHeight = m_rcScrollThumb.Height();
        int32_t nDraggableRange = std::max(1, nTrackHeight - nThumbHeight);
        int32_t dy = msg.ptMouse.y - m_dragStartY;
        int32_t nNewPos = m_dragStartScrollPos +
                          static_cast<int32_t>(static_cast<int64_t>(dy) * nMaxScroll / nDraggableRange);
        SetScrollPos(nNewPos);
        return true;
    }
    return BaseClass::MouseMove(msg);
}

bool Timeline::MouseWheel(const EventArgs& msg)
{
    if (!IsEnabled()) {
        return BaseClass::MouseWheel(msg);
    }
    // Mouse wheel scrolling
    int32_t wheelDelta = msg.eventData;
    if (wheelDelta != 0) {
        // Each wheel unit is about 120, converted to pixels (about 60px per unit)
        int32_t nDelta = -(wheelDelta / 40) * Dpi().GetScaleInt(20);
        int32_t nNewPos = m_nScrollPos + nDelta;
        SetScrollPos(nNewPos);
        return true;
    }
    return BaseClass::MouseWheel(msg);
}

bool Timeline::MouseEnter(const EventArgs& msg)
{
    bool bRet = BaseClass::MouseEnter(msg);
    if (!m_bMouseHover) {
        m_bMouseHover = true;
        Invalidate();
    }
    return bRet;
}

bool Timeline::MouseLeave(const EventArgs& msg)
{
    bool bRet = BaseClass::MouseLeave(msg);
    if (m_bMouseHover) {
        m_bMouseHover = false;
        Invalidate();
    }
    return bRet;
}

void Timeline::SendItemClickEvent(size_t index)
{
    EventArgs args;
    args.SetSender(this);
    args.wParam = static_cast<WPARAM>(index);
    SendEvent(kEventTimelineItemClick, args);
}

UiRect Timeline::CalcScrollThumbRect(const UiRect& rcChart) const
{
    UiRect rcThumb;
    int32_t nViewHeight = rcChart.Height();
    if ((nViewHeight <= 0) || (m_nContentHeight <= nViewHeight)) {
        return rcThumb; // No scrolling is needed
    }
    const int32_t nTrackTop = rcChart.top + Dpi().GetScaleInt(2);
    const int32_t nTrackBottom = rcChart.bottom - Dpi().GetScaleInt(2);
    const int32_t nTrackHeight = nTrackBottom - nTrackTop;
    if (nTrackHeight <= 0) {
        return rcThumb;
    }
    // The thumb height follows the view/content ratio, with a minimum of 20px
    int32_t nThumbHeight = static_cast<int32_t>(
        static_cast<int64_t>(nTrackHeight) * nViewHeight / m_nContentHeight);
    nThumbHeight = std::max(nThumbHeight, Dpi().GetScaleInt(20));
    nThumbHeight = std::min(nThumbHeight, nTrackHeight);
    // The thumb position follows the scroll ratio
    int32_t nMaxScroll = m_nContentHeight - nViewHeight;
    int32_t nThumbTop = nTrackTop;
    if (nMaxScroll > 0) {
        nThumbTop += static_cast<int32_t>(
            static_cast<int64_t>(nTrackHeight - nThumbHeight) * m_nScrollPos / nMaxScroll);
    }
    const int32_t nBarWidth = Dpi().GetScaleInt(6);
    rcThumb.left = rcChart.right - nBarWidth - Dpi().GetScaleInt(2);
    rcThumb.right = rcThumb.left + nBarWidth;
    rcThumb.top = nThumbTop;
    rcThumb.bottom = nThumbTop + nThumbHeight;
    return rcThumb;
}

void Timeline::DrawScrollbar(IRender* pRender, const UiRect& rcChart)
{
    if ((pRender == nullptr) || (m_nContentHeight <= rcChart.Height())) {
        return;
    }
    // The scroll bar is shown only while the mouse hovers (auto_hide_scrollbar is enabled by default)
    if (m_bAutoHideScrollbar && !m_bMouseHover) {
        return;
    }
    UiColor trackColor = GetUiColor(m_strScrollbarTrackColor);
    if (trackColor.IsEmpty()) {
        trackColor = UiColor(0xFFE0E0E0);
    }
    UiColor thumbColor = GetUiColor(m_strScrollbarThumbColor);
    if (thumbColor.IsEmpty()) {
        thumbColor = UiColor(0xFFC0C0C0);
    }
    // Track
    const int32_t nBarWidth = Dpi().GetScaleInt(6);
    UiRectF rcTrack(static_cast<float>(rcChart.right - nBarWidth - Dpi().GetScaleInt(2)),
                    static_cast<float>(rcChart.top + Dpi().GetScaleInt(2)),
                    static_cast<float>(rcChart.right - Dpi().GetScaleInt(2)),
                    static_cast<float>(rcChart.bottom - Dpi().GetScaleInt(2)));
    const float fRadius = static_cast<float>(nBarWidth) / 2.0f;
    pRender->FillRoundRect(rcTrack, fRadius, fRadius, trackColor, 120);
    // Thumb
    UiRect rcThumb = CalcScrollThumbRect(rcChart);
    if ((rcThumb.Width() > 0) && (rcThumb.Height() > 0)) {
        m_rcScrollThumb = rcThumb;
        UiRectF rcThumbF(static_cast<float>(rcThumb.left), static_cast<float>(rcThumb.top),
                         static_cast<float>(rcThumb.right), static_cast<float>(rcThumb.bottom));
        pRender->FillRoundRect(rcThumbF, fRadius, fRadius, thumbColor, 200);
    }
}

bool Timeline::HitTestScrollThumb(const UiPoint& ptMouse) const
{
    // The scroll bar is invisible unless the mouse hovers, so it does not take part in hit testing here
    if (m_bAutoHideScrollbar && !m_bMouseHover) {
        return false;
    }
    if ((m_rcScrollThumb.Width() <= 0) || (m_rcScrollThumb.Height() <= 0)) {
        return false;
    }
    // Enlarge the hit test area to make dragging easier
    UiRect rcHit = m_rcScrollThumb;
    rcHit.left -= Dpi().GetScaleInt(4);
    rcHit.right += Dpi().GetScaleInt(4);
    return rcHit.ContainsPt(ptMouse);
}

} // namespace ui
