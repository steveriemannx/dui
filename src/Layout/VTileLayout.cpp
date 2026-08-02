#include "duilib/Layout/VTileLayout.h"
#include "duilib/Core/Box.h"
#include "duilib/Utils/AttributeUtil.h"
#include "duilib/Core/GlobalManager.h"
#include "duilib/Utils/StringUtil.h"
#include <numeric>

namespace ui 
{
VTileLayout::VTileLayout(): 
    m_nColumns(0), 
    m_bAutoCalcColumns(false),
    m_bScaleDown(true),
    m_bAutoCalcItemWidth(false)
{
    // Align to center by default
    SetChildHAlignType(HorAlignType::kAlignCenter);
}

bool VTileLayout::SetAttribute(const DString& strName, const DString& strValue, const DpiManager& dpiManager)
{
    bool hasAttribute = true;
    if ((strName == _T("item_size")) || (strName == _T("itemsize"))) {
        UiSize szItem;
        AttributeUtil::ParseSizeValue(strValue.c_str(), szItem);
        dpiManager.ScaleSize(szItem);
        SetItemSize(szItem, true);
    }
    else if ((strName == _T("columns")) || (strName == _T("rows"))) {
        if (strValue == _T("auto")) {
            // Auto-calculate the number of columns
            SetAutoCalcColumns(true);
        }
        else {
            SetAutoCalcColumns(false);
            SetColumns(StringUtil::StringToInt32(strValue));
        }
    }
    else if (strName == _T("auto_calc_item_size")) {
        SetAutoCalcItemWidth(strValue == _T("true"));
    }
    else if ((strName == _T("scale_down")) || (strName == _T("scaledown"))) {
        SetScaleDown(strValue == _T("true"));
    }
    else {
        hasAttribute = BaseClass::SetAttribute(strName, strValue, dpiManager);
    }
    return hasAttribute;
}

UiSize VTileLayout::CalcEstimateSize(Control* pControl, const UiSize& szItem, const UiRect& rc)
{
    if ((pControl == nullptr) || !pControl->IsVisible()) {
        return UiSize();
    }
    
    // Estimated available area; always uses the total area
    UiMargin rcMargin = pControl->GetMargin();
    int32_t width = rc.Width() - rcMargin.left - rcMargin.right;
    int32_t height = rc.Height() - rcMargin.top - rcMargin.bottom;
    UiSize szAvailable(width, height);
    szAvailable.Validate();
    UiEstSize estSize = pControl->EstimateSize(szAvailable);
    UiSize childSize(estSize.cx.GetInt32(), estSize.cy.GetInt32());
    if (estSize.cy.IsStretch()) {
        childSize.cy = szItem.cy;
    }
    if (childSize.cy < pControl->GetMinHeight()) {
        childSize.cy = pControl->GetMinHeight();
    }
    if (childSize.cy > pControl->GetMaxHeight()) {
        childSize.cy = pControl->GetMaxHeight();
    }

    if (estSize.cx.IsStretch()) {
        childSize.cx = szItem.cx;
    }
    if (childSize.cx < pControl->GetMinWidth()) {
        childSize.cx = pControl->GetMinWidth();
    }
    if (childSize.cx > pControl->GetMaxWidth()) {
        childSize.cx = pControl->GetMaxWidth();
    }
    return childSize;
}

void VTileLayout::CalcTileColumns(const std::vector<ItemSizeInfo>& normalItems, const UiRect& rc,
                                 int32_t tileWidth, int32_t childMarginX, int32_t childMarginY,
                                 int32_t& nColumns)
{
    nColumns = 0;
    if (tileWidth <= 0) {
        // The tile control width must be calculated first, then the column count is calculated from it
        int32_t maxWidth = 0;
        int64_t areaTotal = 0;
        const int64_t maxArea = (int64_t)rc.Width() * rc.Height();
        for (const ItemSizeInfo& itemSizeInfo : normalItems) {
            UiMargin rcMargin = itemSizeInfo.pControl->GetMargin();
            UiSize childSize(itemSizeInfo.cx, itemSizeInfo.cy);
            if (childSize.cx > 0) {
                maxWidth = std::max(maxWidth, childSize.cx + rcMargin.left + rcMargin.right);
            }
            int32_t childWidth = childSize.cx;
            int32_t childHeight = childSize.cy;
            if (childWidth > 0) {
                childWidth += childMarginX;
            }            
            if (childHeight > 0) {
                childHeight += childMarginY;
            }
            areaTotal += ((int64_t)childWidth * childHeight);
            if (areaTotal > maxArea) {
                // Estimate by the area displayed in the current viewport
                break;
            }
        }
        // Take the maximum control width in the viewport as the width of each item
        tileWidth = maxWidth;
    }
    if (tileWidth > 0) {
        // Use the configured width as the tile control width, and calculate the column count from it
        int32_t totalWidth = rc.Width();
        while (totalWidth > 0) {
            totalWidth -= tileWidth;
            if (nColumns != 0) {
                totalWidth -= childMarginX;
            }
            if (totalWidth >= 0) {
                ++nColumns;
            }
        }
    }
}

UiSize64 VTileLayout::EstimateFloatSize(Control* pControl, const UiRect& rc)
{
    ASSERT(pControl != nullptr);
    if ((pControl == nullptr) || !pControl->IsVisible()) {
        return UiSize64();
    }
    UiMargin margin = pControl->GetMargin();
    UiSize childSize = CalcEstimateSize(pControl, UiSize(0, 0), rc);
    if (childSize.cx > 0) {
        childSize.cx += (margin.left + margin.right);
    }
    if (childSize.cy > 0) {
        childSize.cy += (margin.top + margin.bottom);
    }    
    return UiSize64(childSize.cx, childSize.cy);
}

UiSize64 VTileLayout::ArrangeFloatChild(const std::vector<Control*>& items,
                                       const UiRect& rc,
                                       const UiSize& szItem,
                                       bool bEstimateOnly,
                                       std::vector<ItemSizeInfo>& normalItems)
{
    int64_t cxNeededFloat = 0;    // Total width needed by floating controls
    int64_t cyNeededFloat = 0;    // Total height needed by floating controls
    for (Control* pControl : items) {
        if ((pControl == nullptr) || !pControl->IsVisible()) {
            continue;
        }
        if (pControl->IsFloat()) {
            // Floating control
            UiSize64 floatSize;
            if (!bEstimateOnly) {
                // Set the floating control's position (the container's own alignment does not apply)
                floatSize = SetFloatPos(pControl, rc);
            }
            else {
                // Calculate the size of the float control
                floatSize = EstimateFloatSize(pControl, rc);
            }
            if (cxNeededFloat < floatSize.cx) {
                cxNeededFloat = floatSize.cx;
            }
            if (cyNeededFloat < floatSize.cy) {
                cyNeededFloat = floatSize.cy;
            }
        }
        else {
            // Normal control
            UiSize childSize = CalcEstimateSize(pControl, szItem, rc);
            if ((childSize.cx <= 0) || (childSize.cy <= 0)) {
                // Zero-sized controls cannot be displayed (may be stretch controls)
                if (!bEstimateOnly) {
                    UiRect rcPos(rc);
                    rcPos.right = rcPos.left;
                    rcPos.bottom = rcPos.top;
                    pControl->SetPos(rcPos);
                }
            }
            else {
                ItemSizeInfo info;
                info.pControl = pControl;
                info.cx = childSize.cx;
                info.cy = childSize.cy;
                normalItems.push_back(info);
            }
        }
    }
    return UiSize64(cxNeededFloat, cyNeededFloat);
}

int32_t VTileLayout::CalcTileRowHeight(const std::vector<ItemSizeInfo>& normalItems,
                                      const std::vector<ItemSizeInfo>::const_iterator iterBegin,
                                      int32_t nColumns,
                                      const UiSize& szItem)
{
    // The width and height of szItem include the control's outer and inner margins
    ASSERT(nColumns > 0);
    int32_t cyHeight = szItem.cy;
    if (cyHeight > 0) {
        // If a height is configured, use the configured value with priority
        return cyHeight;
    }
    if (nColumns <= 0) {
        return 0;
    }

    int32_t iIndex = 0;
    for (auto it = iterBegin; it != normalItems.end(); ++it) {
        const ItemSizeInfo& itemSizeInfo = *it;
        UiMargin rcMargin = itemSizeInfo.pControl->GetMargin();
        UiSize szTile(itemSizeInfo.cx, itemSizeInfo.cy);

        // Keep the maximum height
        if (szTile.cy > 0) {
            cyHeight = std::max(cyHeight, szTile.cy + rcMargin.top + rcMargin.bottom);
        }

        ++iIndex;
        if ((iIndex % nColumns) == 0) {
            // Line wrap; exit
            break;
        }
    }
    return cyHeight;
}

UiSize VTileLayout::CalcTilePosition(const ItemSizeInfo& itemSizeInfo,
                                    int32_t tileWidth, int32_t tileHeight,
                                    const UiPoint& ptTile, bool bScaleDown, UiRect& szTilePos)
{
    szTilePos.Clear();
    // Target area size (width and height)
    UiSize szItem(tileWidth, tileHeight);
    szItem.Validate();

    // Tile control size (width and height), including outer margins
    UiMargin rcMargin = itemSizeInfo.pControl->GetMargin();
    UiSize childSize(itemSizeInfo.cx + rcMargin.left + rcMargin.right, 
                     itemSizeInfo.cy + rcMargin.top + rcMargin.bottom);
    childSize.Validate();

    if ((szItem.cx == 0) && (childSize.cx > 0)) {
        szItem.cx = childSize.cx;
    }
    if ((childSize.cx == 0) && (szItem.cx > 0)) {
        childSize.cx = szItem.cx;
    }

    if ((szItem.cy == 0) && (childSize.cy > 0)) {
        szItem.cy = childSize.cy;
    }
    if ((childSize.cy == 0) && (szItem.cy > 0)) {
        childSize.cy = szItem.cy;
    }
    
    int32_t cxWidth = szItem.cx;    // Width of each control (tile) (dynamically calculated)
    int32_t cyHeight = szItem.cy;    // Height of each control (tile) (dynamically calculated)

    // Target area rect (top-left coordinates, width and height)
    UiRect rcTile(ptTile.x, ptTile.y, ptTile.x + cxWidth, ptTile.y + cyHeight);

    // Scale the control proportionally (outer margins must be removed when scaling)
    UiRect rcRealTile = rcTile;
    rcRealTile.Deflate(rcMargin);
    UiSize realSize(childSize.cx - rcMargin.left - rcMargin.right,
                    childSize.cy - rcMargin.top - rcMargin.bottom);
    if (bScaleDown && 
        (rcRealTile.Width() > 0) && (rcRealTile.Height() > 0) &&
        (realSize.cx > 0) && (realSize.cy > 0) ) {
        if ((realSize.cx > rcRealTile.Width()) || (realSize.cy > rcRealTile.Height())) {
            // Scaling conditions met; scale proportionally
            double cx = realSize.cx;
            double cy = realSize.cy;
            double cxRatio = cx / rcRealTile.Width();
            double cyRatio = cy / rcRealTile.Height();
            if (cxRatio > cyRatio) {
                ASSERT(realSize.cx > rcRealTile.Width());
                double ratio = cx / realSize.cy;
                realSize.cx = rcRealTile.Width();
                realSize.cy = static_cast<int32_t>(realSize.cx / ratio + 0.5);
            }
            else {
                ASSERT(realSize.cy > rcRealTile.Height());
                double ratio = cy / realSize.cx;
                realSize.cy = rcRealTile.Height();
                realSize.cx = static_cast<int32_t>(realSize.cy / ratio + 0.5);
            }
        }
    }

    // rcTile includes outer margins, while realSize does not (the container's own alignment does not apply)
    szTilePos = GetFloatPos(itemSizeInfo.pControl, rcTile, realSize);
    if (szTilePos.left < ptTile.x) {
        // If the control is larger than the boundary, align it to the left
        int32_t width = szTilePos.Width();
        szTilePos.left = ptTile.x;
        szTilePos.right = ptTile.x + width;
    }
    if (szTilePos.top < ptTile.y) {
        // If the control is larger than the boundary, align it to the top
        int32_t height = szTilePos.Height();
        szTilePos.top = ptTile.y;
        szTilePos.bottom = ptTile.y + height;
    }
    return UiSize(cxWidth, cyHeight);
}

UiSize64 VTileLayout::ArrangeChildren(const std::vector<Control*>& items, UiRect rc, bool bEstimateOnly)
{
    // Overall layout strategy:
    // (1) Avoid exceeding the boundary horizontally as much as possible (unless the first element
    //     of a row is wider than rc, in which case the horizontal direction will exceed the boundary),
    //     while the vertical direction may exceed the boundary.
    // (2) Tile aspect ratio: by default, scale down while keeping the aspect ratio to fit the target
    //     display area (this behavior can be controlled by m_bScaleDown).
    //     If either the width or the height is scaled, the aspect ratio must be kept to avoid distortion;
    //     however, an option SetScaleDown() is provided that does not keep the aspect ratio, in which
    //     case the boundary may be exceeded.
    // (3) For non-free-layout cases, if m_bAutoCalcColumns is true, m_nColumns is set to zero.
    // Layout use cases:
    // (1) !m_bAutoCalcColumns && (m_nColumns == 0) && (m_szItem.cx == 0)
    //     Layout strategy: 1. Column count: free layout without fixed columns. When a row reaches the
    //                          boundary, it wraps (each row may have a different number of columns);
    //                      2. Tile control width: displayed at its actual width;
    //                      3. Tile control height:
    //                         (1) If m_szItem.cy > 0: limited to the fixed m_szItem.cy;
    //                         (2) If m_szItem.cy == 0: displayed at its actual height;
    // (2) (m_nColumns == 0) && (m_szItem.cx > 0)
    //     Layout strategy: 1. Column count: calculated from rc.Width() and m_szItem.cx; the column
    //                          count is fixed;
    //                      2. Tile control width: fixed to m_szItem.cx;
    //                      3. Tile control height:
    //                         (1) If m_szItem.cy > 0: limited to the fixed m_szItem.cy;
    //                         (2) If m_szItem.cy == 0: displayed at its actual height;
    // (3) (m_nColumns > 0) && (m_szItem.cx == 0)
    //     Layout strategy: 1. Column count: fixed to m_nColumns;
    //                      2. Tile control width: displayed at its actual width;
    //                      3. Tile control height:
    //                         (1) If m_szItem.cy > 0: limited to the fixed m_szItem.cy;
    //                         (2) If m_szItem.cy == 0: displayed at its actual height;
    // (4) (m_nColumns > 0) && (m_szItem.cx > 0)
    //     Layout strategy: 1. Column count: fixed to m_nColumns;
    //                      2. Tile control width: fixed to m_szItem.cx;
    //                      3. Tile control height:
    //                         (1) If m_szItem.cy > 0: limited to the fixed m_szItem.cy;
    //                         (2) If m_szItem.cy == 0: displayed at its actual height;

    if (IsFreeLayout()) {
        // Arrange controls using a free layout (no fixed column count; make full use of the display space to show as much content as possible)
        return ArrangeChildFreeLayout(items, rc, bEstimateOnly);
    }
    else {
        if ((GetColumns() > 0) && IsAutoCalcItemWidth()) {
            // Fixed columns are set, and auto-calculation of the child item width is enabled
            UiRect rect = rc;
            DeflatePadding(rect); // Subtract the padding; the rest is the available area
            int32_t nNewItemWidth = 0;
            if (AutoCalcItemWidth(GetColumns(), GetChildMarginX(), rect.Width(), nNewItemWidth)) {
                UiSize szNewItemSize = GetItemSize();
                szNewItemSize.cx = nNewItemWidth;
                SetItemSize(szNewItemSize, false);
            }
        }
        std::vector<int32_t> inColumnWidths;
        std::vector<int32_t> outColumnWidths;
        ArrangeChildNormal(items, rc, true, inColumnWidths, outColumnWidths);
        inColumnWidths.swap(outColumnWidths);
        return ArrangeChildNormal(items, rc, bEstimateOnly, inColumnWidths, outColumnWidths);
    }
}

bool VTileLayout::IsFreeLayout() const
{
    return (!m_bAutoCalcColumns && (m_nColumns == 0) && (m_szItem.cx == 0) && m_szItem.cy == 0);
}

UiSize64 VTileLayout::ArrangeChildNormal(const std::vector<Control*>& items,
                                        UiRect rect,
                                        bool bEstimateOnly,
                                        const std::vector<int32_t>& inColumnWidths,
                                        std::vector<int32_t>& outColumnWidths) const
{
    ASSERT(!IsFreeLayout());
    DeflatePadding(rect); // Subtract the padding; the rest is the available area
    const UiRect& rc = rect;

    // Arrange the floating controls, filtering out hidden controls, non-displayable controls, etc.
    // Stretch-type child controls: they can be displayed if (m_szItem.cx > 0) && (m_szItem.cy > 0) is true, otherwise they are filtered out
    std::vector<ItemSizeInfo> normalItems;
    ArrangeFloatChild(items, rc, m_szItem, bEstimateOnly, normalItems); // Total width and height needed by floating controls

    int32_t nColumns = m_nColumns;  // Number of columns (configured value)
    if (m_bAutoCalcColumns) {
        // If the column count is auto-calculated, recalculate it
        nColumns = 0;
    }
    if (nColumns <= 0) {
        CalcTileColumns(normalItems, rc, m_szItem.cx, GetChildMarginX(), GetChildMarginY(), nColumns);
    }
    if (nColumns < 1) {
        // When it cannot be calculated precisely, the default value is 1
        nColumns = 1;
    }

    // Column width settings, fixed values
    std::vector<int32_t> fixedColumnWidths = inColumnWidths;
    if (m_szItem.cx > 0) {
        fixedColumnWidths.clear();
        fixedColumnWidths.resize(nColumns, m_szItem.cx);
    }

    int32_t cyRowHeight = 0;   // Height of the controls (tiles) in each row (dynamically calculated)

    std::vector<int32_t> rowHeights;   // Height value of each column, calculated value
    std::vector<int32_t> columnWidths; // Width value of each row, calculated value
    columnWidths.resize(nColumns);
    rowHeights.resize(1);

    int32_t nRowTileCount = 0;  // Number of tile controls accommodated in the current row
    int32_t nRowIndex = 0;      // Current row number

    int32_t xPosLeft = rc.left; // Left coordinate of the first column
    // Left coordinate of the control's displayed content
    if (!bEstimateOnly && !fixedColumnWidths.empty()) {
        int32_t cxTotal = std::accumulate(fixedColumnWidths.begin(), fixedColumnWidths.end(), 0);
        if (fixedColumnWidths.size() > 1) {
            cxTotal += ((int32_t)fixedColumnWidths.size() - 1) * GetChildMarginX();
        }
        if (cxTotal < rc.Width()) {
            HorAlignType hAlign = GetChildHAlignType();
            if (hAlign == HorAlignType::kAlignCenter) {
                // Center alignment
                xPosLeft = rc.CenterX() - cxTotal / 2;
            }
            else if (hAlign == HorAlignType::kAlignRight) {
                // Right alignment
                xPosLeft = rc.right - cxTotal;
            }
        }
    }

    UiPoint ptTile(xPosLeft, rc.top);    // Top-left vertex coordinates of each control (tile)    
    for( auto it = normalItems.begin(); it != normalItems.end(); ++it ) {
        const ItemSizeInfo& itemSizeInfo = *it;
        Control* pControl = itemSizeInfo.pControl;
        if (pControl == nullptr) {
            continue;
        }
        if (nRowTileCount == 0) {
            // Start of a row; calculate the height of this row
            nRowTileCount = nColumns;
            cyRowHeight = CalcTileRowHeight(normalItems, it, nColumns, m_szItem);
            ASSERT(cyRowHeight > 0);// Cannot be zero
        }
        
        const int32_t colIndex = nColumns - nRowTileCount;// Current column index [0, nColumns)
        int32_t fixedColumnWidth = 0; // Column width passed in, fixed value
        if (((int32_t)fixedColumnWidths.size() == nColumns) &&
            (colIndex < (int32_t)fixedColumnWidths.size())) {
            fixedColumnWidth = fixedColumnWidths[colIndex];
        }

        // Calculate the position, width (cxWidth) and height (cyHeight) of the current tile control
        UiRect rcTilePos;

        UiPoint posLeftTop = ptTile;         // Top-left coordinates of this control
        int32_t posWidth = fixedColumnWidth; // Maximum available width of this control
        int32_t posHeight = cyRowHeight;     // Maximum available height of this control
        UiSize szTileSize = CalcTilePosition(itemSizeInfo, posWidth, posHeight,
                                             posLeftTop, m_bScaleDown, rcTilePos);// The return value includes the control's outer margins
        
        if (!bEstimateOnly) {
            pControl->SetPos(rcTilePos);
        }

        int32_t cxWidth = szTileSize.cx;
        if (fixedColumnWidth > 0) {
            cxWidth = fixedColumnWidth;
        }

        // Calculate the maximum height of this row; the height must include the outer margins
        int32_t tileHeight = (m_szItem.cy > 0) ? m_szItem.cy : szTileSize.cy;
        rowHeights[nRowIndex] = std::max(tileHeight, rowHeights[nRowIndex]);

        --nRowTileCount;
        if(nRowTileCount == 0 ) {
            // Line wrap
            rowHeights.push_back(0);
            nRowIndex = (int32_t)rowHeights.size() - 1;            
            
            // Reset the X and Y coordinates (start of the row)
            ptTile.x = xPosLeft;
            // Switch the Y coordinate to the next row, moving by the row height
            ptTile.y += cyRowHeight + GetChildMarginY();
        }
        else {
            // On the same row, move the coordinate to the right by the width of the current tile control
            ptTile.x += cxWidth + GetChildMarginX();
        }        
        // Record the width of each column (take the maximum control width in this column, including the control's outer margins)
        if (colIndex < (int32_t)columnWidths.size()) {
            int32_t tileWidth = (m_szItem.cx > 0) ? m_szItem.cx : cxWidth;    
            columnWidths[colIndex] = std::max(tileWidth, columnWidths[colIndex]);
        }
    }

    // Since the padding has already been removed, it must be added back when calculating the width and height
    UiPadding rcPadding;
    if (GetOwner() != nullptr) {
        rcPadding = GetOwner()->GetPadding();
    }
    // Calculate the required width
    int64_t cxNeeded = std::accumulate(columnWidths.begin(), columnWidths.end(), 0);
    if (columnWidths.size() > 1) {
        cxNeeded += (columnWidths.size() - 1) * GetChildMarginX();
    }
    cxNeeded += ((int64_t)rcPadding.left + rcPadding.right);

    // Calculate the required height
    int64_t cyNeeded = std::accumulate(rowHeights.begin(), rowHeights.end(), 0);
    if (rowHeights.size() > 1) {
        cyNeeded += (rowHeights.size() - 1) * GetChildMarginY();
    }
    cyNeeded += ((int64_t)rcPadding.top + rcPadding.bottom);

    outColumnWidths.swap(columnWidths);
    UiSize64 size(cxNeeded, cyNeeded);
    return size;
}

UiSize64 VTileLayout::ArrangeChildFreeLayout(const std::vector<Control*>& items, 
                                            UiRect rect, bool bEstimateOnly) const
{
    const UiRect rcBox = rect; // Rect bounds of the container
    DeflatePadding(rect); // Subtract the padding; the rest is the available area
    const UiRect& rc = rect;

    // Arrange the floating controls, filtering out hidden controls, non-displayable controls, etc.
    // Stretch-type child controls: they can be displayed if (m_szItem.cx > 0) && (m_szItem.cy > 0) is true, otherwise they are filtered out
    std::vector<ItemSizeInfo> normalItems;
    ArrangeFloatChild(items, rc, m_szItem, bEstimateOnly, normalItems); // Total width and height needed by floating controls

    int64_t cxNeeded = 0;        // Total width needed by non-floating controls
    int64_t cyNeeded = 0;        // Total height needed by non-floating controls

    int32_t cyRowHeight = 0;    // Height of the controls (tiles) in each row (dynamically calculated)

    std::vector<std::pair<Control*, UiRect>> childPosList; // Record the position and size of each control
    const int32_t xPosLeft = rc.left;         // Left coordinate of the control's displayed content; always left-aligned
    UiPoint ptTile(xPosLeft, rc.top);    // Top-left vertex coordinates of each control (tile)
    const size_t itemCount = normalItems.size();
    for (size_t index = 0; index < itemCount; ++index) {
        const ItemSizeInfo& itemSizeInfo = normalItems[index];
        Control* pControl = itemSizeInfo.pControl;
        if (pControl == nullptr) {
            continue;
        }

        // Calculate the position, width and height of the current tile control
        UiRect rcTilePos;
        UiSize szTileSize = CalcTilePosition(itemSizeInfo, 0, 0,
                                             ptTile, m_bScaleDown, rcTilePos);
        if (rcTilePos.right >= rc.right) {
            // The right side has exceeded the boundary; if not at the leftmost position, wrap the line first, then display
            if (ptTile.x != xPosLeft) {
                // Wrap the line first, then display
                ptTile.x = xPosLeft;
                ptTile.y += cyRowHeight + GetChildMarginY();// Next row
                cyRowHeight = 0;

                szTileSize = CalcTilePosition(itemSizeInfo, 0, 0,
                                              ptTile, m_bScaleDown, rcTilePos);                
            }
        }
        if (!bEstimateOnly) {
            childPosList.push_back(std::pair<Control*, UiRect>(pControl, rcTilePos));// Record the position and size; adjust later
        }

        UiMargin rcMargin = pControl->GetMargin();
        cxNeeded = std::max((int64_t)rcTilePos.right + rcMargin.right, cxNeeded);
        cyNeeded = std::max((int64_t)rcTilePos.bottom + rcMargin.bottom, cyNeeded);

        // Update the control width and row height values
        int32_t cxWidth = rcTilePos.Width() + rcMargin.left + rcMargin.right;
        cyRowHeight = std::max(rcTilePos.Height() + rcMargin.top + rcMargin.bottom, cyRowHeight);

        if (rcTilePos.right >= rc.right) {
            // The current control has exceeded the boundary; a line wrap is needed
            ptTile.x = xPosLeft;
            ptTile.y += cyRowHeight + GetChildMarginY();// Next row
            cyRowHeight = 0;
        }
        else {
            // No line wrap; move the horizontal coordinate forward
            ptTile.x += cxWidth + GetChildMarginX();
        }
    }

    // Since the right and bottom coordinate values are used for the calculation, the left and top values must be subtracted to get the true width and height
    cxNeeded -= rc.left;
    cyNeeded -= rc.top;

    // Since the padding has already been removed, it must be added back when calculating the width and height
    UiPadding rcPadding;
    if (GetOwner() != nullptr) {
        rcPadding = GetOwner()->GetPadding();
    }
    cxNeeded += (rcPadding.left + rcPadding.right);
    cyNeeded += (rcPadding.top + rcPadding.bottom);

    if (bEstimateOnly) {
        // The returned width does not exceed the space of the outer container, since this return value becomes the container's final width
        if (cxNeeded > (rect.Width())) {
            cxNeeded = rect.Width();
        }
    }
    UiSize64 size(cxNeeded, cyNeeded);
    if (!bEstimateOnly) {
        if (size.cx < rcBox.Width()) {
            HorAlignType hAlign = GetChildHAlignType();
            int32_t nOffset = 0;
            if (hAlign == HorAlignType::kAlignCenter) {
                // Center alignment
                nOffset = (int32_t)(rcBox.Width() - size.cx) / 2;
            }
            else if (hAlign == HorAlignType::kAlignRight) {
                // Right alignment
                nOffset = (int32_t)(rcBox.Width() - size.cx);
            }
            if (nOffset != 0) {
                for (auto& iter : childPosList) {
                    Control* pControl = iter.first;
                    UiRect& rcChildPos = iter.second;
                    if (!pControl->IsFloat()) {
                        rcChildPos.Offset(nOffset, 0);
                    }
                }
            }
        }
        // Adjust the positions of the child controls
        for (const auto& iter : childPosList) {
            Control* pControl = iter.first;
            const UiRect& rcChildPos = iter.second;
            if (pControl->IsFloat()) {
                // Floating control (the container's own alignment does not apply)
                SetFloatPos(pControl, rcChildPos);
            }
            else {
                pControl->SetPos(rcChildPos);
            }
        }
    }
    return size;
}

UiSize64 VTileLayout::EstimateLayoutSize(const std::vector<Control*>& items, UiSize szAvailable)
{
    szAvailable.Validate();
    UiRect rc(0, 0, szAvailable.Width(), szAvailable.Height());
    UiSize64 requiredSize;
    if (IsFreeLayout()) {
        requiredSize = ArrangeChildFreeLayout(items, rc, true);
    }
    else {
        if ((GetColumns() > 0) && IsAutoCalcItemWidth()) {
            // Fixed columns are set, and auto-calculation of the child item width is enabled
            UiRect rect = rc;
            DeflatePadding(rect); // Subtract the padding; the rest is the available area
            int32_t nNewItemWidth = 0;
            if (AutoCalcItemWidth(GetColumns(), GetChildMarginX(), rect.Width(), nNewItemWidth)) {
                UiSize szNewItemSize = GetItemSize();
                szNewItemSize.cx = nNewItemWidth;
                SetItemSize(szNewItemSize, false);
            }
        }
        std::vector<int32_t> inColumnWidths;
        std::vector<int32_t> outColumnWidths;
        requiredSize = ArrangeChildNormal(items, rc, true, inColumnWidths, outColumnWidths);
    }
    return requiredSize;
}

void VTileLayout::ChangeDpiScale(const DpiManager& dpiManager, uint32_t nOldDpiScale)
{
    UiSize szItem = GetItemSize();
    szItem = dpiManager.GetScaleSize(szItem, nOldDpiScale);
    SetItemSize(szItem, true);
    BaseClass::ChangeDpiScale(dpiManager, nOldDpiScale);
}

const UiSize& VTileLayout::GetItemSize() const
{
    return m_szItem;
}

void VTileLayout::SetItemSize(UiSize szItem, bool bArrange)
{
    szItem.cx = std::max(szItem.cx, 0);
    szItem.cy = std::max(szItem.cy, 0);
    if( (m_szItem.cx != szItem.cx) || (m_szItem.cy != szItem.cy) ) {
        m_szItem = szItem;
        if (bArrange && (GetOwner() != nullptr)) {
            GetOwner()->Arrange();
        }        
    }
}

int32_t VTileLayout::GetColumns() const
{
    return m_nColumns;
}

void VTileLayout::SetColumns(int32_t nCols)
{
    nCols = std::max(nCols, 0);
    if (m_nColumns != nCols) {
        m_nColumns = nCols;
        if (GetOwner() != nullptr) {
            GetOwner()->Arrange();
        }
    }    
}

void VTileLayout::SetAutoCalcColumns(bool bAutoCalcColumns)
{
    if (m_bAutoCalcColumns != bAutoCalcColumns) {
        m_bAutoCalcColumns = bAutoCalcColumns;
        if (GetOwner() != nullptr) {
            GetOwner()->Arrange();
        }
    }
}

bool VTileLayout::IsAutoCalcColumns() const
{
    return m_bAutoCalcColumns;
}

void VTileLayout::SetScaleDown(bool bScaleDown)
{
    if (m_bScaleDown != bScaleDown) {
        m_bScaleDown = bScaleDown;
        if (GetOwner() != nullptr) {
            GetOwner()->Arrange();
        }
    }
}

bool VTileLayout::IsScaleDown() const
{
    return m_bScaleDown;
}

void VTileLayout::SetAutoCalcItemWidth(bool bAutoCalcItemWidth)
{
    if (m_bAutoCalcItemWidth != bAutoCalcItemWidth) {
        m_bAutoCalcItemWidth = bAutoCalcItemWidth;
        if (GetOwner() != nullptr) {
            GetOwner()->Arrange();
        }
    }
}

bool VTileLayout::IsAutoCalcItemWidth() const
{
    return m_bAutoCalcItemWidth;
}

bool VTileLayout::AutoCalcItemWidth(int32_t nColumns, int32_t nMarginX, int32_t szAvailable, int32_t& nItemWidth) const
{
    if ((nColumns < 1) || (szAvailable < 1)) {
        return false;
    }
    if (nMarginX < 0) {
        nMarginX = 0;
    }
    nItemWidth  = (szAvailable - (nColumns - 1) * nMarginX) / nColumns;
    return nItemWidth > 0;
}

} // namespace ui
