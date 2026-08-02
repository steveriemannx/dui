#include "duilib/Layout/VFlowLayout.h"
#include "duilib/Box/ScrollBox.h"
#include <unordered_map>

namespace ui 
{

VFlowLayout::VFlowLayout()
{
    // Align to the top by default
    SetChildVAlignType(VerAlignType::kAlignTop);
}

UiSize64 VFlowLayout::ArrangeChildInternal(const std::vector<Control*>& items, UiRect rc, bool bEstimateOnly, bool bEstimateLayoutSize) const
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

    if (!bEstimateOnly) {
        bEstimateLayoutSize = false;
    }
    DeflatePadding(rc);
    const UiRect rcBox = rc; // Rect bounds of the container
    const UiSize szAvailable(rc.Width(), rc.Height());

    // All controls to be laid out (KEY: control, VALUE: width and height)
    std::unordered_map<Control*, UiEstSize> itemsMap;

    // Calculate the width and height of each control and record them in the map
    for (auto pControl : visibleControls) {
        UiEstSize estSize = pControl->EstimateSize(szAvailable);
        UiSize sz = UiSize(estSize.cx.GetInt32(), estSize.cy.GetInt32());
        UiMargin rcMargin = pControl->GetMargin();

        // Calculate the width
        if (estSize.cx.IsStretch()) {
            if (bEstimateLayoutSize) {
                // Stretch-type child controls are excluded; if a min value is specified, the min value is used
                sz.cx = 0;
                if (sz.cx < pControl->GetMinWidth()) {
                    sz.cx = pControl->GetMinWidth();
                }
            }
            else {
                if (estSize.cx.GetStretchPercentValue() >= 100) {
                    // Fully stretched width: cannot be displayed
                    ASSERT(0);
                    sz.cx = 0;
                }
                else {
                    sz.cx = (CalcStretchValue(estSize.cx, szAvailable.cx) - rcMargin.left - rcMargin.right);
                }
            }            
            sz.cx = std::max(sz.cx, 0);
        }
        sz.cx = std::clamp(sz.cx, pControl->GetMinWidth(), pControl->GetMaxWidth());
        sz.cx = std::max(sz.cx, 0);
        estSize.cx.SetInt32(sz.cx);// cx is a definite value already calculated; it no longer has stretch or auto type values

        // Calculate the height
        if (estSize.cy.IsStretch()) {
            if (bEstimateLayoutSize) {
                // Stretch-type child controls are excluded; if a min value is specified, the min value is used
                sz.cy = 0;
                if (sz.cy < pControl->GetMinHeight()) {
                    sz.cy = pControl->GetMinHeight();
                }
            }
            else {
                sz.cy = (CalcStretchValue(estSize.cy, szAvailable.cy) - rcMargin.top - rcMargin.bottom);
            }            
            sz.cy = std::max(sz.cy, 0);
        }
        sz.cy = std::clamp(sz.cy, pControl->GetMinHeight(), pControl->GetMaxHeight());
        sz.cy = std::max(sz.cy, 0);
        estSize.cy.SetInt32(sz.cy);// cy is a definite value already calculated; it no longer has stretch or auto type values

        itemsMap[pControl] = estSize;
    }
    struct TColumnControls
    {
        // Rect bounds occupied by the data of this column
        UiRect m_columnRect;

        // List of controls in this column
        std::vector<Control*> m_pControlList;

        // Positions and sizes of the controls in this column
        std::vector<UiRect> m_pControlRects;
    };

    // Split into columns
    int32_t nPosY = rc.top;
    int32_t nPosX = rc.left;
    std::vector<TColumnControls> columnControlList; // Row height of the controls in each column

    // Total number of columns
    size_t nColumnCount = 1;

    const size_t nItemCount = visibleControls.size();
    for (size_t nItem = 0; nItem < nItemCount; ++nItem) {
        Control* pControl = visibleControls[nItem];

        // Mark whether this is the last control
        bool bLastControl = (nItem == (nItemCount - 1)) ? true : false;

        // Lay out one control
        UiMargin rcMargin = pControl->GetMargin();// Margin of the child control
        nPosY += rcMargin.top;

        UiEstSize estSize = itemsMap[pControl];
        UiRect rcChild;
        rcChild.left = nPosX + rcMargin.left;
        rcChild.top = nPosY;
        rcChild.right = rcChild.left + estSize.cx.GetInt32();
        rcChild.bottom = rcChild.top + estSize.cy.GetInt32();

        nPosY += rcChild.Height();
        nPosY += rcMargin.bottom;

        // Collect statistics
        if (nColumnCount != columnControlList.size()) {
            columnControlList.resize(nColumnCount);
        }
        TColumnControls& controls = columnControlList[nColumnCount - 1];
        controls.m_pControlList.push_back(pControl);
        controls.m_pControlRects.push_back(rcChild);

        // Actual occupied space (including margins)
        rcChild.top -= rcMargin.top;
        rcChild.bottom += rcMargin.bottom;
        rcChild.left -= rcMargin.left;
        rcChild.right += rcMargin.right;
        controls.m_columnRect.Union(rcChild);

        if (!bLastControl) {
            // Not yet reached the last control
            nPosY += GetChildMarginY(); // Add the control spacing

            bool bNeedNewColumn = false;
            if (nPosY > rc.bottom) {
                bNeedNewColumn = true;
            }
            else if (nItem < (nItemCount - 1)) {
                // Determine whether the next control fits; if it exceeds the boundary, a column break is needed
                Control* pNextControl = visibleControls[nItem + 1];// The next control
                UiMargin rcNextMargin = pNextControl->GetMargin();// Margin of the child control
                UiEstSize estNextSize = itemsMap[pNextControl];
                int32_t nNextHeight = rcNextMargin.top + estNextSize.cy.GetInt32() + rcNextMargin.bottom;
                if ((nPosY + nNextHeight) > rc.bottom) {
                    bNeedNewColumn = true;
                }
            }

            if (bNeedNewColumn) {
                // Column break
                nColumnCount += 1;
                nPosY = rc.top;
                nPosX += controls.m_columnRect.Width();
                nPosX += GetChildMarginX();
            }
        }
    }

    UiRect childrenRect;
    for (const TColumnControls& control : columnControlList) {
        childrenRect.Union(control.m_columnRect);
    }
    const int32_t cxNeeded = childrenRect.Width(); // Total width needed
    const int32_t cyNeeded = childrenRect.Height();// Total height needed
    UiSize64 szChildren((int64_t)cxNeeded, (int64_t)cyNeeded);
    UiPadding rcPadding;
    if (GetOwner() != nullptr) {
        rcPadding = GetOwner()->GetPadding();
    }
    if (szChildren.cx > 0) {
        szChildren.cx += ((int64_t)rcPadding.left + rcPadding.right);
    }
    if (szChildren.cy > 0) {
        szChildren.cy += ((int64_t)rcPadding.top + rcPadding.bottom);
    }

    if (bEstimateLayoutSize) {
        // Estimation mode; return the result directly (the case where the container's width and height are set to "auto")
        return szChildren;
    }

    // Handle the overall alignment (the child control alignment set on the Box container layout)
    if (cxNeeded < rcBox.Width()) {
        // In a ScrollBox, the vertical alignment takes effect only when no vertical scroll bar appears
        UiRect rcRealBox;
        bool bEnableHScrollBar = false;
        ScrollBox* pScrollBox = dynamic_cast<ScrollBox*>(GetOwner());
        if (pScrollBox != nullptr) {
            bEnableHScrollBar = pScrollBox->GetHScrollBar() != nullptr;
            rcRealBox = pScrollBox->GetPos();
        }
        if (!bEnableHScrollBar || (cxNeeded < rcRealBox.Width())) {
            // Horizontal alignment: arrange all items as a whole
            int32_t nOffsetX = 0;
            const HorAlignType hAlignType = GetChildHAlignType();
            if (hAlignType == HorAlignType::kAlignCenter) {
                // Vertically centered
                nOffsetX = (rcBox.Width() - cxNeeded) / 2;
            }
            else if (hAlignType == HorAlignType::kAlignRight) {
                // Right-aligned
                nOffsetX = rcBox.Width() - cxNeeded;
            }
            if (nOffsetX != 0) {
                for (TColumnControls& control : columnControlList) {
                    control.m_columnRect.Offset(nOffsetX, 0);
                    for (UiRect& rcChild : control.m_pControlRects) {
                        rcChild.Offset(nOffsetX, 0);
                    }
                    // The returned area size must include the rightward adjustment offset
                    childrenRect.Union(control.m_columnRect);
                }
            }
        }
    }

    if ((GetChildVAlignType() == VerAlignType::kAlignCenter) || (GetChildVAlignType() == VerAlignType::kAlignBottom)) {
        // Vertical alignment: applied per column
        const VerAlignType vAlignType = GetChildVAlignType();
        for (TColumnControls& control : columnControlList) {
            UiRect& rowRect = control.m_columnRect;
            if (rowRect.Height() < rcBox.Height()) {
                int32_t nOffsetY = 0;
                if (vAlignType == VerAlignType::kAlignCenter) {
                    // Vertically centered
                    nOffsetY = (rcBox.Height() - rowRect.Height()) / 2;
                }
                else if (vAlignType == VerAlignType::kAlignBottom) {
                    // Bottom-aligned
                    nOffsetY = rcBox.Height() - rowRect.Height();
                }
                if (nOffsetY != 0) {
                    rowRect.Offset(0, nOffsetY);
                    for (UiRect& rcChild : control.m_pControlRects) {
                        rcChild.Offset(0, nOffsetY);
                    }

                    // The returned area size must include the downward adjustment offset
                    childrenRect.Union(control.m_columnRect);
                }
            }
        }
    }

    // Adjust the layout of the child controls
    if (!bEstimateOnly) {
        // Adjust the layout of the child controls (non-floating controls)
        for (TColumnControls& control : columnControlList) {
            ASSERT(control.m_pControlRects.size() == control.m_pControlList.size());
            if (control.m_pControlRects.size() != control.m_pControlList.size()) {
                // Error
                return szChildren;
            }
            const UiRect& rowRect = control.m_columnRect;
            const size_t nCount = control.m_pControlList.size();
            for (size_t nIndex = 0; nIndex < nCount; ++nIndex) {
                Control* pControl = control.m_pControlList[nIndex];
                UiRect& rcChild = control.m_pControlRects[nIndex];

                // Handle the alignment of the control (the alignment set by the control itself)
                // Only the horizontal alignment of the control needs to be handled
                const HorAlignType hAlignType = pControl->GetHorAlignType();
                if ((hAlignType == HorAlignType::kAlignCenter) || (hAlignType == HorAlignType::kAlignRight)) {
                    UiRect rcFullRect = rowRect;
                    UiMargin rcMargin = pControl->GetMargin();// Margin of the child control
                    rcFullRect.left += rcMargin.left;
                    rcFullRect.right -= rcMargin.right;
                    if (rcChild.Width() < rcFullRect.Width()) {
                        int32_t nOffset = rcFullRect.Width() - rcChild.Width();
                        if (hAlignType == HorAlignType::kAlignCenter) {
                            rcChild.Offset(nOffset / 2, 0);
                        }
                        else if (hAlignType == HorAlignType::kAlignRight) {
                            rcChild.Offset(nOffset, 0);
                        }
                    }
                }

                // Adjust the layout of the child controls
                pControl->SetPos(rcChild);
            }
        }

        // Adjust the layout of the child controls (floating controls)
        for (auto pControl : items) {
            if ((pControl != nullptr) && pControl->IsVisible() && pControl->IsFloat()) {
                // Floating control (the container's own alignment does not apply)
                SetFloatPos(pControl, rc);
            }
        }
    }

    // Needs to be recalculated because the alignment changes the area
    szChildren.cx = (int64_t)childrenRect.Width();
    szChildren.cy = (int64_t)childrenRect.Height();
    if (szChildren.cx > 0) {
        szChildren.cx += ((int64_t)rcPadding.left + rcPadding.right);
    }
    if (szChildren.cy > 0) {
        szChildren.cy += ((int64_t)rcPadding.top + rcPadding.bottom);
    }
    return szChildren;
}

UiSize64 VFlowLayout::ArrangeChildren(const std::vector<Control*>& items, UiRect rc, bool bEstimateOnly)
{
    return ArrangeChildInternal(items, rc, bEstimateOnly, false);
}

UiSize64 VFlowLayout::EstimateLayoutSize(const std::vector<Control*>& items, UiSize szAvailable)
{
    UiRect rc(0, 0, szAvailable.cx, szAvailable.cy);
    return ArrangeChildInternal(items, rc, true, true);
}

} // namespace ui

