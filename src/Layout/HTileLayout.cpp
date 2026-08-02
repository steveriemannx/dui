#include "duilib/Layout/HTileLayout.h"
#include "duilib/Core/Box.h"
#include "duilib/Core/GlobalManager.h"
#include "duilib/Utils/AttributeUtil.h"
#include "duilib/Utils/StringUtil.h"
#include <numeric>

namespace ui 
{
HTileLayout::HTileLayout():
    m_nRows(0), 
    m_bAutoCalcRows(false),
    m_bScaleDown(true),
    m_bAutoCalcItemHeight(false)
{
    // Default: center-aligned
    SetChildVAlignType(VerAlignType::kAlignCenter);
}

bool HTileLayout::SetAttribute(const DString& strName, const DString& strValue, const DpiManager& dpiManager)
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
            SetAutoCalcRows(true);
        }
        else {
            SetAutoCalcRows(false);
            SetRows(StringUtil::StringToInt32(strValue));
        }
    }
    else if (strName == _T("auto_calc_item_size")) {
        SetAutoCalcItemHeight(strValue == _T("true"));
    }
    else if ((strName == _T("scale_down")) || (strName == _T("scaledown"))) {
        SetScaleDown(strValue == _T("true"));
    }
    else {
        hasAttribute = BaseClass::SetAttribute(strName, strValue, dpiManager);
    }
    return hasAttribute;
}

UiSize HTileLayout::CalcEstimateSize(Control* pControl, const UiSize& szItem, const UiRect& rc)
{
    if ((pControl == nullptr) || !pControl->IsVisible()) {
        return UiSize();
    }
    
    // Estimated available area; always use the total area
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

void HTileLayout::CalcTileRows(const std::vector<ItemSizeInfo>& normalItems, const UiRect& rc,
                                 int32_t tileHeight, int32_t childMarginX, int32_t childMarginY,
                                 int32_t& nRows)
{
    nRows = 0;
    if (tileHeight <= 0) {
        // Need to calculate the tile control height first, then calculate the number of rows based on the width
        int32_t maxHeight = 0;
        int64_t areaTotal = 0;
        const int64_t maxArea = (int64_t)rc.Width() * rc.Height();
        for (const ItemSizeInfo& itemSizeInfo : normalItems) {
            UiMargin rcMargin = itemSizeInfo.pControl->GetMargin();
            UiSize childSize(itemSizeInfo.cx, itemSizeInfo.cy);
            if (childSize.cy > 0) {
                maxHeight = std::max(maxHeight, childSize.cy + rcMargin.top + rcMargin.bottom);
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
                // Estimate based on the area displayed in the current viewport
                break;
            }
        }
        // Use the maximum control height in the viewport as the height of each item
        tileHeight = maxHeight;
    }
    if (tileHeight > 0) {
        // Use the configured height as the tile width, and calculate the number of rows based on the configured height
        int32_t totalHeight = rc.Height();
        while (totalHeight > 0) {
            totalHeight -= tileHeight;
            if (nRows != 0) {
                totalHeight -= childMarginX;
            }
            if (totalHeight >= 0) {
                ++nRows;
            }
        }
    }
}

UiSize64 HTileLayout::EstimateFloatSize(Control* pControl, const UiRect& rc)
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

UiSize64 HTileLayout::ArrangeFloatChild(const std::vector<Control*>& items,
                                       const UiRect& rc,
                                       const UiSize& szItem,
                                       bool bEstimateOnly,
                                       std::vector<ItemSizeInfo>& normalItems)
{
    int64_t cxNeededFloat = 0;    // Total width needed by float controls
    int64_t cyNeededFloat = 0;    // Total height needed by float controls
    for (Control* pControl : items) {
        if ((pControl == nullptr) || !pControl->IsVisible()) {
            continue;
        }
        if (pControl->IsFloat()) {
            // Float control
            UiSize64 floatSize;
            if (!bEstimateOnly) {
                // Set the position of the float control (the container's own alignment does not apply)
                floatSize = SetFloatPos(pControl, rc);
            }
            else {
                // Calculate the size of the Float control
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
                // Controls with zero size cannot be displayed (possibly stretch controls)
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

int32_t HTileLayout::CalcTileColumnWidth(const std::vector<ItemSizeInfo>& normalItems,
                                         const std::vector<ItemSizeInfo>::const_iterator iterBegin,
                                         int32_t nRows,
                                         const UiSize& szItem)
{
    // The width and height values of szItem include the control's margins and padding
    ASSERT(nRows > 0);
    int32_t cyWidth = szItem.cx;
    if (cyWidth > 0) {
        // If a width is configured, use the configured width value first
        return cyWidth;
    }
    if (nRows <= 0) {
        return 0;
    }

    int32_t iIndex = 0;
    for (auto it = iterBegin; it != normalItems.end(); ++it) {
        const ItemSizeInfo& itemSizeInfo = *it;
        UiMargin rcMargin = itemSizeInfo.pControl->GetMargin();
        UiSize szTile(itemSizeInfo.cx, itemSizeInfo.cy);

        // Keep the maximum height
        if (szTile.cx > 0) {
            cyWidth = std::max(cyWidth, szTile.cx + rcMargin.left + rcMargin.right);
        }

        ++iIndex;
        if ((iIndex % nRows) == 0) {
            // Wrap to the next row, then break
            break;
        }
    }
    return cyWidth;
}

UiSize HTileLayout::CalcTilePosition(const ItemSizeInfo& itemSizeInfo,
                                    int32_t tileWidth, int32_t tileHeight,
                                    const UiPoint& ptTile, bool bScaleDown, UiRect& szTilePos)
{
    szTilePos.Clear();
    // Target area size (width and height)
    UiSize szItem(tileWidth, tileHeight);
    szItem.Validate();

    // Tile control size (width and height), including margins
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
    
    int32_t cxWidth = szItem.cx;    // Width of each control (tile), a dynamically computed value
    int32_t cyHeight = szItem.cy;    // Height of each control (tile), a dynamically computed value

    // Target area rect (top-left coordinates, width and height)
    UiRect rcTile(ptTile.x, ptTile.y, ptTile.x + cxWidth, ptTile.y + cyHeight);

    // Scale the control proportionally (margins must be removed before scaling)
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

    // rcTile includes margins, while realSize does not (the container's own alignment does not apply)
    szTilePos = GetFloatPos(itemSizeInfo.pControl, rcTile, realSize);
    if (szTilePos.left < ptTile.x) {
        // If the control is too large and exceeds the boundary, align it to the left
        int32_t width = szTilePos.Width();
        szTilePos.left = ptTile.x;
        szTilePos.right = ptTile.x + width;
    }
    if (szTilePos.top < ptTile.y) {
        // If the control is too large and exceeds the boundary, align it to the top
        int32_t height = szTilePos.Height();
        szTilePos.top = ptTile.y;
        szTilePos.bottom = ptTile.y + height;
    }
    return UiSize(cxWidth, cyHeight);
}

UiSize64 HTileLayout::ArrangeChildren(const std::vector<Control*>& items, UiRect rc, bool bEstimateOnly)
{
    // Overall layout strategy:
    // (1) Horizontally, try not to exceed the boundary (unless the first element of a row is wider than rc, in which case it will exceed the boundary),
    //     while vertically it may exceed the boundary.
    // (2) Tile aspect ratio: by default, tiles are scaled down proportionally to fit the target display area (this behavior can be controlled with m_bScaleDown).
    //     If either the width or the height is scaled, the aspect ratio must be preserved to avoid distortion;
    //     however, the SetScaleDown() option is provided to skip aspect ratio preservation, in which case the content may exceed the boundary.
    // (3) For non-free-layout mode, if m_bAutoCalcColumns is true, m_nColumns is set to zero
    // Layout use cases:
    // (1) !m_bAutoCalcRows && (m_nRows == 0) && (m_szItem.cx == 0)
    //     Layout strategy: 1. Columns: free layout, no fixed columns; when a row reaches the boundary, wrap to a new row (the number of columns per row may differ);
    //                      2. Tile control width: displayed at its actual width;
    //                      3. Tile control height:
    //                        (1) If m_szItem.cy > 0: limited to a fixed m_szItem.cy
    //                        (2) If m_szItem.cy == 0: displayed at its actual height;
    // (2) (m_nRows == 0) && (m_szItem.cx > 0)
    //     Layout strategy: 1. Columns: the number of columns is calculated from rc.Width() and m_szItem.cx, and is fixed;
    //                      2. Tile control width: fixed to m_szItem.cx;
    //                      3. Tile control height:
    //                       (1) If m_szItem.cy > 0: limited to a fixed m_szItem.cy
    //                       (2) If m_szItem.cy == 0: displayed at its actual height;
    // (3) (m_nRows > 0) && (m_szItem.cx == 0)
    //     Layout strategy: 1. Columns: the column count is fixed to m_nColumns;
    //                      2. Tile control width: displayed at its actual width;
    //                      3. Tile control height:
    //                       (1) If m_szItem.cy > 0: limited to a fixed m_szItem.cy
    //                       (2) If m_szItem.cy == 0: displayed at its actual height;
    // (4) (m_nRows > 0) && (m_szItem.cx > 0)
    //     Layout strategy: 1. Columns: the column count is fixed to m_nColumns;
    //                      2. Tile control width: fixed to m_szItem.cx;
    //                      3. Tile control height:
    //                       (1) If m_szItem.cy > 0: limited to a fixed m_szItem.cy
    //                       (2) If m_szItem.cy == 0: displayed at its actual height;

    if (IsFreeLayout()) {
        // Arrange controls using free layout (no fixed row count; make full use of the display space to show as much content as possible)
        return ArrangeChildFreeLayout(items, rc, bEstimateOnly);
    }
    else {
        if ((GetRows() > 0) && IsAutoCalcItemHeight()) {
            // Fixed rows are configured, and auto-calculating the child item height is enabled
            UiRect rect = rc;
            DeflatePadding(rect); // Remove the padding; what remains is the available area
            int32_t nNewItemHeight = 0;
            if (AutoCalcItemHeight(GetRows(), GetChildMarginY(), rect.Height(), nNewItemHeight)) {
                UiSize szNewItemSize = GetItemSize();
                szNewItemSize.cy = nNewItemHeight;
                SetItemSize(szNewItemSize, false);
            }
        }
        std::vector<int32_t> inRowHeights;
        std::vector<int32_t> outRowHeights;
        ArrangeChildNormal(items, rc, true, inRowHeights, outRowHeights);
        inRowHeights.swap(outRowHeights);
        return ArrangeChildNormal(items, rc, bEstimateOnly, inRowHeights, outRowHeights);
    }
}

bool HTileLayout::IsFreeLayout() const
{
    return (!m_bAutoCalcRows && (m_nRows == 0) && (m_szItem.cx == 0) && m_szItem.cy == 0);
}

UiSize64 HTileLayout::ArrangeChildNormal(const std::vector<Control*>& items,
                                        UiRect rect,
                                        bool bEstimateOnly,
                                        const std::vector<int32_t>& inRowHeights,
                                        std::vector<int32_t>& outRowHeights) const
{
    ASSERT(!IsFreeLayout());
    DeflatePadding(rect); // Remove the padding; what remains is the available area
    const UiRect& rc = rect;

    // Arrange float controls, and filter out hidden controls and controls that cannot be displayed
    // Stretch-type child controls: if (m_szItem.cx > 0) && (m_szItem.cy > 0) is true, they can be displayed; otherwise they are filtered out
    std::vector<ItemSizeInfo> normalItems;
    ArrangeFloatChild(items, rc, m_szItem, bEstimateOnly, normalItems); // Total width and height needed by float controls

    int32_t nRows = m_nRows;  // Number of rows (configured value)
    if (m_bAutoCalcRows) {
        // If auto-calculating the number of columns, recalculate the number of rows
        nRows = 0;
    }
    if (nRows <= 0) {
        CalcTileRows(normalItems, rc, m_szItem.cy, GetChildMarginX(), GetChildMarginY(), nRows);
    }
    if (nRows < 1) {
        // When an exact value cannot be calculated, set the default to 1
        nRows = 1;
    }

    // Column width setting, fixed value
    std::vector<int32_t> fixedRowHeights = inRowHeights;
    if (m_szItem.cy > 0) {
        fixedRowHeights.clear();
        fixedRowHeights.resize(nRows, m_szItem.cy);
    }

    int32_t cxColumnWidth = 0;         // Width of each column of controls (tiles), a dynamically computed value

    std::vector<int32_t> rowHeights;   // Row height values, computed
    std::vector<int32_t> columnWidths; // Column width values, computed
    rowHeights.resize(nRows);
    columnWidths.resize(1);

    int32_t nColumnTileCount = 0;      // Number of tile controls in the current column
    int32_t nColumnIndex = 0;          // Current column index

    int32_t yPosTop = rc.top; // Top coordinate of the first row
    // Top coordinate of the control's display area
    if (!bEstimateOnly && !fixedRowHeights.empty()) {
        int32_t cyTotal = std::accumulate(fixedRowHeights.begin(), fixedRowHeights.end(), 0);
        if (fixedRowHeights.size() > 1) {
            cyTotal += ((int32_t)fixedRowHeights.size() - 1) * GetChildMarginX();
        }
        if (cyTotal < rc.Height()) {
            VerAlignType vAlign = GetChildVAlignType();
            if (vAlign == VerAlignType::kAlignCenter) {
                // Center-aligned
                yPosTop = rc.CenterY() - cyTotal / 2;
            }
            else if (vAlign == VerAlignType::kAlignBottom) {
                // Bottom-aligned
                yPosTop = rc.bottom - cyTotal;
            }            
        }
    }

    UiPoint ptTile(rc.left, yPosTop);    // Top-left coordinate of each control (tile)    
    for( auto it = normalItems.begin(); it != normalItems.end(); ++it ) {
        const ItemSizeInfo& itemSizeInfo = *it;
        Control* pControl = itemSizeInfo.pControl;
        if (pControl == nullptr) {
            continue;
        }
        if (nColumnTileCount == 0) {
            // Start of a column; calculate the height of this row
            nColumnTileCount = nRows;
            cxColumnWidth = CalcTileColumnWidth(normalItems, it, nRows, m_szItem);
            ASSERT(cxColumnWidth > 0);// Cannot be zero
        }
        
        const int32_t rowIndex = nRows - nColumnTileCount;// Current column index [0, nRows)
        int32_t fixedRowHeight = 0; // Row height passed in for the current row; fixed value
        if (((int32_t)fixedRowHeights.size() == nRows) &&
            (rowIndex < (int32_t)fixedRowHeights.size())) {
            fixedRowHeight = fixedRowHeights[rowIndex];
        }

        // Calculate the position coordinates, width (cxWidth) and height (cyHeight) of the current tile control
        UiRect rcTilePos;

        UiPoint posLeftTop = ptTile;         // Top-left coordinates of the control
        int32_t posWidth = cxColumnWidth;    // Maximum available width of the control
        int32_t posHeight = fixedRowHeight;  // Maximum available height of the control
        UiSize szTileSize = CalcTilePosition(itemSizeInfo, posWidth, posHeight,
                                             posLeftTop, m_bScaleDown, rcTilePos);// The return value includes the control's margins
        
        if (!bEstimateOnly) {
            pControl->SetPos(rcTilePos);
        }

        int32_t cyHeight = szTileSize.cy;
        if (fixedRowHeight > 0) {
            cyHeight = fixedRowHeight;
        }

        // Calculate the maximum width of this column; the width must include the margins
        int32_t tileWidth = (m_szItem.cx > 0) ? m_szItem.cx : szTileSize.cx;
        columnWidths[nColumnIndex] = std::max(tileWidth, columnWidths[nColumnIndex]);

        --nColumnTileCount;
        if(nColumnTileCount == 0 ) {
            // Move to the next column
            columnWidths.push_back(0);
            nColumnIndex = (int32_t)columnWidths.size() - 1;
            
            // Reset the X and Y coordinates (start of the row)
            ptTile.x += cxColumnWidth + GetChildMarginX();
            // Switch the Y coordinate to the next row, according to the row height
            ptTile.y = yPosTop;
        }
        else {
            // In the same column, move the coordinate downward by the width of the current tile control
            ptTile.y += cyHeight + GetChildMarginY();
        }        
        // Record the height of each row (take the maximum control height in the row, including the control's margins)
        if (rowIndex < (int32_t)rowHeights.size()) {
            int32_t tileHeight = (m_szItem.cy > 0) ? m_szItem.cy : cyHeight;    
            rowHeights[rowIndex] = std::max(tileHeight, rowHeights[rowIndex]);
        }
    }

    // Since the padding has been removed, it must be added back when calculating the width and height
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

    outRowHeights.swap(rowHeights);
    UiSize64 size(cxNeeded, cyNeeded);
    return size;
}

UiSize64 HTileLayout::ArrangeChildFreeLayout(const std::vector<Control*>& items,
                                            UiRect rect, bool bEstimateOnly) const
{
    const UiRect rcBox = rect; // Rectangular extent of the container
    DeflatePadding(rect); // Remove the padding; what remains is the available area
    const UiRect& rc = rect;

    // Arrange float controls, and filter out hidden controls and controls that cannot be displayed
    // Stretch-type child controls: if (m_szItem.cx > 0) && (m_szItem.cy > 0) is true, they can be displayed; otherwise they are filtered out
    std::vector<ItemSizeInfo> normalItems;
    ArrangeFloatChild(items, rc, m_szItem, bEstimateOnly, normalItems); // Total width and height needed by float controls

    int64_t cxNeeded = 0;        // Total width needed by non-float controls    
    int64_t cyNeeded = 0;        // Total height needed by non-float controls

    int32_t cxColumnWidth = 0;   // Width of each column of controls (tiles), a dynamically computed value

    std::vector<std::pair<Control*, UiRect>> childPosList; // Records the position and size of each control
    const int32_t yPosTop = rc.top;         // Top coordinate of the control's display area; always top-aligned
    UiPoint ptTile(rc.left, yPosTop); // Top-left coordinate of each control (tile)
    const size_t itemCount = normalItems.size();
    for (size_t index = 0; index < itemCount; ++index) {
        const ItemSizeInfo& itemSizeInfo = normalItems[index];
        Control* pControl = itemSizeInfo.pControl;
        if (pControl == nullptr) {
            continue;
        }

        // Calculate the position coordinates, width and height of the current tile control
        UiRect rcTilePos;
        UiSize szTileSize = CalcTilePosition(itemSizeInfo, 0, 0,
                                             ptTile, m_bScaleDown, rcTilePos);
        if (rcTilePos.bottom >= rc.bottom) {
            // The bottom has exceeded the boundary; if not near the top, switch to the next column first, then display
            if (ptTile.y != yPosTop) {
                // Switch to the next column first, then display
                ptTile.y = yPosTop;
                ptTile.x += cxColumnWidth + GetChildMarginX();// Next column            
                cxColumnWidth = 0;

                szTileSize = CalcTilePosition(itemSizeInfo, 0, 0,
                                              ptTile, m_bScaleDown, rcTilePos);                
            }
        }
        if (!bEstimateOnly) {
            childPosList.push_back(std::pair<Control*, UiRect>(pControl, rcTilePos));// Record position and size; adjust later
        }

        UiMargin rcMargin = pControl->GetMargin();
        cxNeeded = std::max((int64_t)rcTilePos.right + rcMargin.right, cxNeeded);
        cyNeeded = std::max((int64_t)rcTilePos.bottom + rcMargin.bottom, cyNeeded);

        // Update the control width and row height values
        int32_t cyHeight = rcTilePos.Height() + rcMargin.top + rcMargin.bottom;
        cxColumnWidth = std::max(rcTilePos.Width() + rcMargin.left + rcMargin.right, cxColumnWidth);

        if (rcTilePos.bottom >= rc.bottom) {
            // The current control has exceeded the boundary; wrap to a new row
            ptTile.y = yPosTop;
            ptTile.x += cxColumnWidth + GetChildMarginX();// Next column        
            cxColumnWidth = 0;
        }
        else {
            // Do not wrap; move the Y coordinate to the next position
            ptTile.y += cyHeight + GetChildMarginY();
        }
    }

    // Since the right and bottom coordinate values are used for the calculation, subtract the left and top values to get the actual width and height
    cxNeeded -= rc.left;
    cyNeeded -= rc.top;

    // Since the padding has been removed, it must be added back when calculating the width and height
    UiPadding rcPadding;
    if (GetOwner() != nullptr) {
        rcPadding = GetOwner()->GetPadding();
    }
    cxNeeded += (rcPadding.left + rcPadding.right);
    cyNeeded += (rcPadding.top + rcPadding.bottom);

    if (bEstimateOnly) {
        // The returned width must not exceed the outer container's space, because this return value will become the container's final width
        if (cyNeeded > (rect.Height())) {
            cyNeeded = rect.Height();
        }
    }
    UiSize64 size(cxNeeded, cyNeeded);
    if (!bEstimateOnly) {
        if (size.cy < rcBox.Height()) {
            // The implementation above defaults to top alignment
            VerAlignType vAlign = GetChildVAlignType();
            int32_t nOffset = 0;
            if (vAlign == VerAlignType::kAlignCenter) {
                // Center-aligned
                nOffset = (int32_t)(rcBox.Height() - size.cy) / 2;
            }
            else if (vAlign == VerAlignType::kAlignBottom) {
                // Align to the bottom (needs to move downward)
                nOffset = (int32_t)(rcBox.Height() - size.cy);
            }
            if (nOffset != 0) {
                for (auto& iter : childPosList) {
                    Control* pControl = iter.first;
                    UiRect& rcChildPos = iter.second;
                    if (!pControl->IsFloat()) {
                        rcChildPos.Offset(0, nOffset);
                    }
                }
            }
        }
        // Adjust the positions of the child controls
        for (const auto& iter : childPosList) {
            Control* pControl = iter.first;
            const UiRect& rcChildPos = iter.second;
            if (pControl->IsFloat()) {
                // Float control (the container's own alignment does not apply)
                SetFloatPos(pControl, rcChildPos);
            }
            else {
                pControl->SetPos(rcChildPos);
            }
        }
    }
    return size;
}

UiSize64 HTileLayout::EstimateLayoutSize(const std::vector<Control*>& items, UiSize szAvailable)
{
    szAvailable.Validate();
    UiRect rc(0, 0, szAvailable.Width(), szAvailable.Height());
    UiSize64 requiredSize;
    if (IsFreeLayout()) {
        requiredSize = ArrangeChildFreeLayout(items, rc, true);
    }
    else {
        if ((GetRows() > 0) && IsAutoCalcItemHeight()) {
            // Fixed rows are configured, and auto-calculating the child item height is enabled
            UiRect rect = rc;
            DeflatePadding(rect); // Remove the padding; what remains is the available area
            int32_t nNewItemHeight = 0;
            if (AutoCalcItemHeight(GetRows(), GetChildMarginY(), rect.Height(), nNewItemHeight)) {
                UiSize szNewItemSize = GetItemSize();
                szNewItemSize.cy = nNewItemHeight;
                SetItemSize(szNewItemSize, false);
            }
        }
        std::vector<int32_t> inRowHeights;
        std::vector<int32_t> outRowHeights;
        requiredSize = ArrangeChildNormal(items, rc, true, inRowHeights, outRowHeights);
    }
    return requiredSize;
}

void HTileLayout::ChangeDpiScale(const DpiManager& dpiManager, uint32_t nOldDpiScale)
{
    UiSize szItem = GetItemSize();
    szItem = dpiManager.GetScaleSize(szItem, nOldDpiScale);
    SetItemSize(szItem, true);
    BaseClass::ChangeDpiScale(dpiManager, nOldDpiScale);
}

const UiSize& HTileLayout::GetItemSize() const
{
    return m_szItem;
}

void HTileLayout::SetItemSize(UiSize szItem, bool bArrange)
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

int32_t HTileLayout::GetRows() const
{
    return m_nRows;
}

void HTileLayout::SetRows(int32_t nRows)
{
    nRows = std::max(nRows, 0);
    if (m_nRows != nRows) {
        m_nRows = nRows;
        if (GetOwner() != nullptr) {
            GetOwner()->Arrange();
        }
    }
}

void HTileLayout::SetAutoCalcRows(bool bAutoCalcRows)
{
    if (m_bAutoCalcRows != bAutoCalcRows) {
        m_bAutoCalcRows = bAutoCalcRows;
        if (GetOwner() != nullptr) {
            GetOwner()->Arrange();
        }
    }
}

bool HTileLayout::IsAutoCalcRows() const
{
    return m_bAutoCalcRows;
}

void HTileLayout::SetScaleDown(bool bScaleDown)
{
    if (m_bScaleDown != bScaleDown) {
        m_bScaleDown = bScaleDown;
        if (GetOwner() != nullptr) {
            GetOwner()->Arrange();
        }
    }
}

bool HTileLayout::IsScaleDown() const
{
    return m_bScaleDown;
}

void HTileLayout::SetAutoCalcItemHeight(bool bAutoCalcItemHeight)
{
    if (m_bAutoCalcItemHeight != bAutoCalcItemHeight) {
        m_bAutoCalcItemHeight = bAutoCalcItemHeight;
        if (GetOwner() != nullptr) {
            GetOwner()->Arrange();
        }
    }
}

bool HTileLayout::IsAutoCalcItemHeight() const
{
    return m_bAutoCalcItemHeight;
}

bool HTileLayout::AutoCalcItemHeight(int32_t nRows, int32_t nMarginY, int32_t szAvailable, int32_t& nItemHeight) const
{
    if ((nRows < 1) || (szAvailable < 1)) {
        return false;
    }
    if (nMarginY < 0) {
        nMarginY = 0;
    }
    nItemHeight = (szAvailable - (nRows - 1) * nMarginY) / nRows;
    return nItemHeight > 0;
}

} // namespace ui
