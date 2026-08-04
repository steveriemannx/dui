#include "dui/Layout/GridLayout.h"
#include "dui/Box/ScrollBox.h"
#include "dui/Core/DpiManager.h"

namespace ui 
{

GridLayout::GridLayout():
    m_nRows(0),
    m_nColumns(0),
    m_nGridWidth(0),
    m_nGridHeight(0),
    m_bScaleDown(false)
{
    // Horizontally centered by default
    SetChildHAlignType(HorAlignType::kAlignCenter);

    // Top-aligned by default
    SetChildVAlignType(VerAlignType::kAlignTop);
}

bool GridLayout::SetAttribute(const DString& strName, const DString& strValue, const DpiManager& dpiManager)
{
    bool hasAttribute = true;
    if (strName == _T("rows")) {
        if (strValue == _T("auto")) {
            // Auto-calculate
            SetRows(0);
        }
        else {
            SetRows(StringUtil::StringToInt32(strValue));
        }
    }
    else if (strName == _T("columns")) {
        if (strValue == _T("auto")) {
            // Auto-calculate
            SetColumns(0);
        }
        else {
            SetColumns(StringUtil::StringToInt32(strValue));
        }
    }
    else if (strName == _T("grid_width")) {
        if (strValue == _T("auto")) {
            // Auto-calculate
            SetGridWidth(0, false);
        }
        else {
            int32_t nGridWidth = StringUtil::StringToInt32(strValue);
            dpiManager.ScaleInt(nGridWidth);
            SetGridWidth(nGridWidth, false);
        }
    }
    else if (strName == _T("grid_height")) {
        if (strValue == _T("auto")) {
            // Auto-calculate
            SetGridHeight(0, false);
        }
        else {
            int32_t nGridHeight = StringUtil::StringToInt32(strValue);
            dpiManager.ScaleInt(nGridHeight);
            SetGridHeight(nGridHeight, false);
        }
    }
    else if (strName == _T("scale_down")) {
        SetScaleDown(strValue == _T("true"));
    }
    else {
        hasAttribute = BaseClass::SetAttribute(strName, strValue, dpiManager);
    }
    return hasAttribute;
}

void GridLayout::ChangeDpiScale(const DpiManager& dpiManager, uint32_t nOldDpiScale)
{
    int32_t nGridWidth = GetGridWidth();
    nGridWidth = dpiManager.GetScaleInt(nGridWidth, nOldDpiScale);
    SetGridWidth(nGridWidth, false);

    int32_t nGridHeight = GetGridHeight();
    nGridHeight = dpiManager.GetScaleInt(nGridHeight, nOldDpiScale);
    SetGridHeight(nGridHeight, false);
}

int32_t GridLayout::GetRows() const
{
    return m_nRows;
}

void GridLayout::SetRows(int32_t nRows)
{
    ASSERT(nRows >= 0);
    nRows = std::max(nRows, 0);
    if (m_nRows != nRows) {
        m_nRows = nRows;
        if (GetOwner() != nullptr) {
            GetOwner()->Arrange();
        }
    }
}

int32_t GridLayout::GetColumns() const
{
    return m_nColumns;
}

void GridLayout::SetColumns(int32_t nCols)
{
    ASSERT(nCols >= 0);
    nCols = std::max(nCols, 0);
    if (m_nColumns != nCols) {
        m_nColumns = nCols;
        if (GetOwner() != nullptr) {
            GetOwner()->Arrange();
        }
    }
}

void GridLayout::SetGridWidth(int32_t nGridWidth, bool bNeedDpiScale)
{
    ASSERT(nGridWidth >= 0);
    if (nGridWidth >= 0) {
        if (bNeedDpiScale) {
            ASSERT(GetOwner() != nullptr);
            if (GetOwner() != nullptr) {
                GetOwner()->Dpi().ScaleInt(nGridWidth);
            }
        }
        if (m_nGridWidth != nGridWidth) {
            m_nGridWidth = nGridWidth;
            if (GetOwner() != nullptr) {
                GetOwner()->Arrange();
            }
        }
    }
}

int32_t GridLayout::GetGridWidth() const
{
    return m_nGridWidth;
}

void GridLayout::SetGridHeight(int32_t nGridHeight, bool bNeedDpiScale)
{
    ASSERT(nGridHeight >= 0);
    if (nGridHeight >= 0) {
        if (bNeedDpiScale) {
            ASSERT(GetOwner() != nullptr);
            if (GetOwner() != nullptr) {
                GetOwner()->Dpi().ScaleInt(nGridHeight);
            }
        }
        if (m_nGridHeight != nGridHeight) {
            m_nGridHeight = nGridHeight;
            if (GetOwner() != nullptr) {
                GetOwner()->Arrange();
            }
        }
    }
}

int32_t GridLayout::GetGridHeight() const
{
    return m_nGridHeight;
}

void GridLayout::SetScaleDown(bool bScaleDown)
{
    if (m_bScaleDown != bScaleDown) {
        m_bScaleDown = bScaleDown;
        if (GetOwner() != nullptr) {
            GetOwner()->Arrange();
        }
    }
}

bool GridLayout::IsScaleDown() const
{
    return m_bScaleDown;
}

UiSize64 GridLayout::ArrangeChildInternal(const std::vector<Control*>& items, UiRect rc, bool bEstimateOnly) const
{
    if (items.empty()) {
        return UiSize64();
    }
    // Pre-processing: filter out all visible, non-floating controls
    std::vector<Control*> visibleControls;
    for (auto pControl : items) {
        if ((pControl != nullptr) && pControl->IsVisible() && !pControl->IsFloat()) {
            visibleControls.push_back(pControl);
        }
    }
    if (visibleControls.empty()) {
        // No visible controls; adjust the layout of the child controls (floating controls), then return directly
        if (!bEstimateOnly) {
            for (auto pControl : items) {
                if ((pControl != nullptr) && pControl->IsVisible() && pControl->IsFloat()) {
                    // Floating control (the container's own alignment does not apply)
                    SetFloatPos(pControl, rc);
                }
            }
        }
        return UiSize64();
    }

    DeflatePadding(rc);
    const UiRect contentRect = rc; // Rect bounds of the container, with the container padding already removed

    // Calculate the actual number of rows and columns (handles auto-calculation)
    int32_t actualRows = GetRows();
    int32_t actualCols = GetColumns();

    // Calculate the number of grid cells
    int32_t gridCount = 0;
    for (auto pControl : visibleControls) {
        int32_t rowSpan = pControl->GetRowSpan();
        int32_t colSpan = pControl->GetColumnSpan();
        ASSERT(rowSpan > 0);
        ASSERT(colSpan > 0);
        gridCount += rowSpan * colSpan;
    }
    gridCount = std::max(gridCount, (int32_t)visibleControls.size());

    CalcActualGridSize(gridCount, actualRows, actualCols);

    // Calculate the base size of the cells (merging not considered)
    std::vector<int32_t> colWidths = CalcColumnWidths(contentRect, actualCols);
    std::vector<int32_t> rowHeights = CalcRowHeights(contentRect, actualRows);

    // Mark the occupied cells (handles merging)
    std::vector<std::vector<bool>> cellOccupied(actualRows, std::vector<bool>(actualCols, false));
    std::unordered_map<Control*, UiRect> ctrlCellRects; // Merged cell rect corresponding to each control
    if (!CalcMergedCellRects(visibleControls, actualRows, actualCols, colWidths, rowHeights, cellOccupied, ctrlCellRects)) {
        ASSERT(!"CalcMergedCellRects failed!");
        return UiSize64(); // Cell merge conflict; return an invalid size
    }

    // Calculate the overall position of the grid (based on the layout's alignment attributes)
    UiSize gridTotalSize = CalcGridTotalSize(colWidths, rowHeights);
    if (!bEstimateOnly) {
        UiRect gridRect = CalcGridPosition(contentRect, gridTotalSize);

        // Adjust the positions of the merged cells (based on the grid's overall offset)
        AdjustMergedCellPositions(ctrlCellRects, gridRect);

        // Arrange the controls into the merged cells (using the controls' own alignment attributes)
        ArrangeControlsInMergedCells(visibleControls, ctrlCellRects);

        // Handle floating controls
        for (auto pControl : items) {
            if ((pControl != nullptr) && pControl->IsVisible() && pControl->IsFloat()) {
                SetFloatPos(pControl, rc);
            }
        }
    }
    // Calculate the final required size (including the padding)
    UiPadding rcPadding = GetOwner() != nullptr ? GetOwner()->GetPadding() : UiPadding();
    return UiSize64(
            gridTotalSize.cx + rcPadding.left + rcPadding.right,
            gridTotalSize.cy + rcPadding.top + rcPadding.bottom
        );
}

void GridLayout::CalcActualGridSize(int32_t gridCount, int32_t& rows, int32_t& cols) const
{
    rows = std::max(rows, 0);
    cols = std::max(cols, 0);
    if (gridCount > 0) {
        if (rows == 0 && cols == 0) {
            cols = static_cast<int32_t>(std::sqrt(gridCount)) + 1;
            rows = (gridCount + cols - 1) / cols;
        }
        else if (rows == 0) {
            rows = (gridCount + cols - 1) / cols;
        }
        else if (cols == 0) {
            cols = (gridCount + rows - 1) / rows;
        }
    }
    rows = std::max(rows, 1);
    cols = std::max(cols, 1);
}

std::vector<int32_t> GridLayout::CalcColumnWidths(const UiRect& contentRect, int32_t cols) const
{
    ASSERT(cols > 0);
    if (cols <= 0) {
        return std::vector<int32_t>();
    }

    int32_t nGridWidth = GetGridWidth();
    if (nGridWidth > 0) {
        // A fixed grid width has been set, use it directly
        std::vector<int32_t> widths(cols, nGridWidth);
        return widths;
    }
    else {
        // Calculate the width of each column based on the total width of the content display area
        int32_t availableWidth = contentRect.Width() - (cols - 1) * GetChildMarginX();
        int32_t baseWidth = std::max(0, availableWidth / cols);
        std::vector<int32_t> widths(cols, baseWidth);

        // Distribute the remaining width (to resolve division rounding errors)
        int32_t remaining = availableWidth - baseWidth * cols;
        for (int32_t i = 0; i < remaining; ++i) {
            widths[i]++;
        }
        return widths;
    }
}

std::vector<int32_t> GridLayout::CalcRowHeights(const UiRect& contentRect, int32_t rows) const
{
    ASSERT(rows > 0);
    if (rows <= 0) {
        return std::vector<int32_t>();
    }

    int32_t nGridHeight = GetGridHeight();
    if (nGridHeight > 0) {
        // A fixed grid height has been set, use it directly
        std::vector<int32_t> heights(rows, nGridHeight);
        return heights;
    }
    else {
        // Calculate the height of each row based on the total height of the content display area
        int32_t availableHeight = contentRect.Height() - (rows - 1) * GetChildMarginY();
        int32_t baseHeight = std::max(0, availableHeight / rows);
        std::vector<int32_t> heights(rows, baseHeight);

        // Distribute the remaining height
        int32_t remaining = availableHeight - baseHeight * rows;
        for (int32_t i = 0; i < remaining; ++i) {
            heights[i]++;
        }
        return heights;
    }
}

bool GridLayout::CalcMergedCellRects(const std::vector<Control*>& controls,
                                     int32_t rows, int32_t cols,
                                     const std::vector<int32_t>& colWidths,
                                     const std::vector<int32_t>& rowHeights,
                                     std::vector<std::vector<bool>>& cellOccupied,
                                     std::unordered_map<Control*, UiRect>& ctrlCellRects) const
{
    ASSERT((rows > 0) && (cols > 0) && !controls.empty() && ((int32_t)colWidths.size() == cols) && ((int32_t)rowHeights.size() == rows));
    if ((rows <= 0) || (cols <= 0) || controls.empty() ||
        ((int32_t)colWidths.size() != cols) ||
        ((int32_t)rowHeights.size() != rows)) {
        return false;
    }
    int32_t ctrlIndex = 0;
    for (int32_t row = 0; row < rows && ctrlIndex < (int32_t)controls.size(); ++row) {
        for (int32_t col = 0; col < cols && ctrlIndex < (int32_t)controls.size(); ++col) {
            if (cellOccupied[row][col]) {
                continue; // skip occupied cells
            }

            Control* ctrl = controls[ctrlIndex++];
            int32_t rowSpan = std::min(ctrl->GetRowSpan(), rows - row); // limit the maximum row span
            int32_t colSpan = std::min(ctrl->GetColumnSpan(), cols - col); // limit the maximum column span

            // Mark the cells in the merged range as occupied
            for (int32_t r = row; r < row + rowSpan; ++r) {
                for (int32_t c = col; c < col + colSpan; ++c) {
                    if (cellOccupied[r][c]) {
                        return false; // merge conflict (overlap), return failure
                    }
                    cellOccupied[r][c] = true;
                }
            }

            // Calculate the original size of the merged cell (without considering the grid offset)
            UiRect cellRect;
            cellRect.left = 0;
            for (int32_t c = 0; c < col; ++c) {
                cellRect.left += colWidths[c] + GetChildMarginX();
            }
            cellRect.right = cellRect.left;
            for (int32_t c = col; c < col + colSpan; ++c) {
                cellRect.right += colWidths[c] + (c == (col + colSpan - 1) ? 0 : GetChildMarginX());
            }

            cellRect.top = 0;
            for (int32_t r = 0; r < row; ++r) {
                cellRect.top += rowHeights[r] + GetChildMarginY();
            }
            cellRect.bottom = cellRect.top;
            for (int32_t r = row; r < row + rowSpan; ++r) {
                cellRect.bottom += rowHeights[r] + (r == (row + rowSpan - 1) ? 0 : GetChildMarginY());
            }

            ctrlCellRects[ctrl] = cellRect;
        }
    }
    return true;
}

UiSize GridLayout::CalcGridTotalSize(const std::vector<int32_t>& colWidths, const std::vector<int32_t>& rowHeights) const
{
    int32_t totalWidth = 0;
    for (size_t i = 0; i < colWidths.size(); ++i) {
        totalWidth += colWidths[i] + (i == (colWidths.size() - 1) ? 0 : GetChildMarginX());
    }
    int32_t totalHeight = 0;
    for (size_t i = 0; i < rowHeights.size(); ++i) {
        totalHeight += rowHeights[i] + (i == (rowHeights.size() - 1) ? 0 : GetChildMarginY());
    }
    return UiSize(totalWidth, totalHeight);
}

UiRect GridLayout::CalcGridPosition(const UiRect& contentRect, const UiSize& gridSize) const
{
    UiRect gridRect;
    gridRect.left = contentRect.left;
    gridRect.top = contentRect.top;
    gridRect.right = gridRect.left + gridSize.cx;
    gridRect.bottom = gridRect.top + gridSize.cy;

    // Horizontal alignment (the layout's own GetChildHAlignType)
    switch (GetChildHAlignType()) {
    case HorAlignType::kAlignCenter:
        gridRect.Offset((contentRect.Width() - gridSize.cx) / 2, 0);
        break;
    case HorAlignType::kAlignRight:
        gridRect.Offset(contentRect.Width() - gridSize.cx, 0);
        break;
    default:
        break;
    }

    // Vertical alignment (the layout's own GetChildVAlignType)
    switch (GetChildVAlignType()) {
    case VerAlignType::kAlignCenter:
        gridRect.Offset(0, (contentRect.Height() - gridSize.cy) / 2);
        break;
    case VerAlignType::kAlignBottom:
        gridRect.Offset(0, contentRect.Height() - gridSize.cy);
        break;
    default:
        break;
    }
    return gridRect;
}

void GridLayout::AdjustMergedCellPositions(std::unordered_map<Control*, UiRect>& ctrlCellRects,
                                           const UiRect& gridRect) const
{
    for (auto& [ctrl, rect] : ctrlCellRects) {
        rect.Offset(gridRect.left, gridRect.top);
    }
}

void GridLayout::ArrangeControlsInMergedCells(const std::vector<Control*>& controls,
                                              const std::unordered_map<Control*, UiRect>& ctrlCellRects) const
{
    for (Control* pControl : controls) {
        auto it = ctrlCellRects.find(pControl);
        if (it == ctrlCellRects.end()) {
            continue;
        }

        const UiRect& cellRect = it->second;
        UiMargin margin = pControl->GetMargin();

        // Position of the control within the cell
        UiRect rcChild;

        // Available space within the cell (after deducting the control's margin)
        UiRect contentRect = cellRect;
        contentRect.left += margin.left;
        contentRect.top += margin.top;
        contentRect.right -= margin.right;
        contentRect.bottom -= margin.bottom;
        if (contentRect.Width() <= 0 || contentRect.Height() <= 0) {
            // Insufficient space in the cell: set its width and height to 0
            rcChild = contentRect;
            rcChild.right = rcChild.left;
            rcChild.bottom = rcChild.top;
        }
        else {
            // Sufficient space in the cell: calculate the control size (limited by its own min/max)
            UiSize childSize;
            if (IsScaleDown()) {
                UiSize szAvailable(contentRect.Width(), contentRect.Height());
                UiEstSize estSize = pControl->EstimateSize(szAvailable);
                childSize = UiSize(estSize.cx.GetInt32(), estSize.cy.GetInt32());
                if (estSize.cx.IsStretch()) {
                    childSize.cx = CalcStretchValue(estSize.cx, szAvailable.cx);
                }
                if (estSize.cy.IsStretch()) {
                    childSize.cy = CalcStretchValue(estSize.cy, szAvailable.cy);
                }
                childSize.cx = std::clamp(childSize.cx, pControl->GetMinWidth(), pControl->GetMaxWidth());
                childSize.cy = std::clamp(childSize.cy, pControl->GetMinHeight(), pControl->GetMaxHeight());

                // If the child control size exceeds the cell range, scale it down proportionally to fit within the cell to avoid overflowing the boundary
                if ((childSize.cx > 0) && (childSize.cy > 0)) {
                    if ((childSize.cx > contentRect.Width()) || (childSize.cy > contentRect.Height())) {
                        // Scaling conditions met, perform proportional scaling
                        double cx = childSize.cx;
                        double cy = childSize.cy;
                        double cxRatio = cx / contentRect.Width();
                        double cyRatio = cy / contentRect.Height();
                        if (cxRatio > cyRatio) {
                            ASSERT(childSize.cx > contentRect.Width());
                            double ratio = cx / childSize.cy;
                            childSize.cx = contentRect.Width();
                            childSize.cy = static_cast<int32_t>(childSize.cx / ratio + 0.5);
                        }
                        else {
                            ASSERT(childSize.cy > contentRect.Height());
                            double ratio = cy / childSize.cx;
                            childSize.cy = contentRect.Height();
                            childSize.cx = static_cast<int32_t>(childSize.cy / ratio + 0.5);
                        }
                    }
                }
            }
            else {
                childSize = UiSize(contentRect.Width(), contentRect.Height());
            }

            // Calculate the position of the control within the cell (using the control's own alignment attributes)
            rcChild = CalcControlPosition(contentRect, childSize, pControl);
        }
        pControl->SetPos(rcChild);
    }
}

UiRect GridLayout::CalcControlPosition(const UiRect& contentRect, const UiSize& ctrlSize, Control* ctrl) const
{
    UiRect ctrlRect;
    ctrlRect.left = contentRect.left;
    ctrlRect.top = contentRect.top;
    ctrlRect.right = ctrlRect.left + ctrlSize.cx;
    ctrlRect.bottom = ctrlRect.top + ctrlSize.cy;

    // Horizontal alignment (the control's own GetHorAlignType)
    switch (ctrl->GetHorAlignType()) {
    case HorAlignType::kAlignCenter:
        ctrlRect.Offset((contentRect.Width() - ctrlSize.cx) / 2, 0);
        break;
    case HorAlignType::kAlignRight:
        ctrlRect.Offset(contentRect.Width() - ctrlSize.cx, 0);
        break;
    default:
        break;
    }

    // Vertical alignment (the control's own GetVerAlignType)
    switch (ctrl->GetVerAlignType()) {
    case VerAlignType::kAlignCenter:
        ctrlRect.Offset(0, (contentRect.Height() - ctrlSize.cy) / 2);
        break;
    case VerAlignType::kAlignBottom:
        ctrlRect.Offset(0, contentRect.Height() - ctrlSize.cy);
        break;
    default:
        break;
    }
    return ctrlRect;
}

UiSize64 GridLayout::ArrangeChildren(const std::vector<Control*>& items, UiRect rc, bool bEstimateOnly)
{
    return ArrangeChildInternal(items, rc, bEstimateOnly);
}

UiSize64 GridLayout::EstimateLayoutSize(const std::vector<Control*>& items, UiSize szAvailable)
{
    UiRect rc(0, 0, szAvailable.cx, szAvailable.cy);
    return ArrangeChildInternal(items, rc, true);
}

} // namespace ui
