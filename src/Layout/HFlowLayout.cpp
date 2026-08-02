#include "duilib/Layout/HFlowLayout.h"
#include "duilib/Box/ScrollBox.h"
#include <unordered_map>

namespace ui 
{

HFlowLayout::HFlowLayout()
{
    // Align to the left by default
    SetChildHAlignType(HorAlignType::kAlignLeft);
}

UiSize64 HFlowLayout::ArrangeChildInternal(const std::vector<Control*>& items, UiRect rc, bool bEstimateOnly, bool bEstimateLayoutSize) const
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
                sz.cx = (CalcStretchValue(estSize.cx, szAvailable.cx) - rcMargin.left - rcMargin.right);
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
                if (estSize.cy.GetStretchPercentValue() >= 100) {
                    // Fully stretched height: cannot be displayed
                    ASSERT(0);
                    sz.cy = 0;
                }
                else {
                    sz.cy = (CalcStretchValue(estSize.cy, szAvailable.cy) - rcMargin.top - rcMargin.bottom);
                }
            }
            sz.cy = std::max(sz.cy, 0);
        }
        sz.cy = std::clamp(sz.cy, pControl->GetMinHeight(), pControl->GetMaxHeight());
        sz.cy = std::max(sz.cy, 0);
        estSize.cy.SetInt32(sz.cy);// cy is a definite value already calculated; it no longer has stretch or auto type values

        itemsMap[pControl] = estSize;
    }
    struct TRowControls
    {
        // Rect bounds occupied by the data of this row
        UiRect m_rowRect;

        // List of controls in this row
        std::vector<Control*> m_pControlList;

        // Positions and sizes of the controls in this row
        std::vector<UiRect> m_pControlRects;
    };

    // Split into rows
    int32_t nPosY = rc.top;
    int32_t nPosX = rc.left;
    std::vector<TRowControls> rowControlList; // Controls and row heights in each row

    // Total number of rows
    size_t nRowCount = 1;

    const size_t nItemCount = visibleControls.size();
    for (size_t nItem = 0; nItem < nItemCount; ++nItem) {
        Control* pControl = visibleControls[nItem];

        // Mark whether this is the last control
        bool bLastControl = (nItem == (nItemCount - 1)) ? true : false;

        // Lay out one control
        UiMargin rcMargin = pControl->GetMargin();// Margin of the child control
        nPosX += rcMargin.left;

        UiEstSize estSize = itemsMap[pControl];
        UiRect rcChild;
        rcChild.left = nPosX;
        rcChild.top = nPosY + rcMargin.top;
        rcChild.right = rcChild.left + estSize.cx.GetInt32();
        rcChild.bottom = rcChild.top + estSize.cy.GetInt32();

        nPosX += rcChild.Width();
        nPosX += rcMargin.right;

        // Collect statistics
        if (nRowCount != rowControlList.size()) {
            rowControlList.resize(nRowCount);
        }
        TRowControls& controls = rowControlList[nRowCount - 1];
        controls.m_pControlList.push_back(pControl);
        controls.m_pControlRects.push_back(rcChild);

        // Actual occupied space (including margins)
        rcChild.top -= rcMargin.top;
        rcChild.bottom += rcMargin.bottom;
        rcChild.left -= rcMargin.left;
        rcChild.right += rcMargin.right;
        controls.m_rowRect.Union(rcChild);

        if (!bLastControl) {
            // Not yet reached the last control
            nPosX += GetChildMarginX();// Add the control spacing

            bool bNeedNewRow = false;
            if (nPosX > rc.right) {
                bNeedNewRow = true;
            }
            else if (nItem < (nItemCount - 1)) {
                // Determine whether the next control fits; if it exceeds the boundary, a row wrap is needed
                Control* pNextControl = visibleControls[nItem + 1];// The next control
                UiMargin rcNextMargin = pNextControl->GetMargin();// Margin of the child control
                UiEstSize estNextSize = itemsMap[pNextControl];
                int32_t nNextWidth = rcNextMargin.left + estNextSize.cx.GetInt32() + rcNextMargin.right;
                if ((nPosX + nNextWidth) > rc.right) {
                    bNeedNewRow = true;
                }
            }

            if (bNeedNewRow) {
                // Row wrap
                nRowCount += 1;
                nPosX = rc.left;
                nPosY += controls.m_rowRect.Height();
                nPosY += GetChildMarginY();
            }
        }
    }

    UiRect childrenRect;
    for (const TRowControls& control : rowControlList) {
        childrenRect.Union(control.m_rowRect);
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
    if ((GetChildHAlignType() == HorAlignType::kAlignCenter) || (GetChildHAlignType() == HorAlignType::kAlignRight)) {
        const HorAlignType hAlignType = GetChildHAlignType();
        // Horizontal alignment: applied per row
        for (TRowControls& control : rowControlList) {
            UiRect& rowRect = control.m_rowRect;
            if (rowRect.Width() < rcBox.Width()) {
                int32_t nOffsetX = 0;
                if (hAlignType == HorAlignType::kAlignCenter) {
                    // Horizontally centered
                    nOffsetX = (rcBox.Width() - rowRect.Width()) / 2;
                }
                else if (hAlignType == HorAlignType::kAlignRight) {
                    // Right-aligned
                    nOffsetX = rcBox.Width() - rowRect.Width();
                }
                if (nOffsetX != 0) {
                    rowRect.Offset(nOffsetX, 0);
                    for (UiRect& rcChild : control.m_pControlRects) {
                        rcChild.Offset(nOffsetX, 0);
                    }

                    // The returned area size must include the rightward adjustment offset
                    childrenRect.Union(control.m_rowRect);
                }
            }
        }        
    }

    if (cyNeeded < rcBox.Height()) {
        // In a ScrollBox, the vertical alignment takes effect only when no vertical scroll bar appears
        UiRect rcRealBox;
        bool bEnableVScrollBar = false;
        ScrollBox* pScrollBox = dynamic_cast<ScrollBox*>(GetOwner());
        if (pScrollBox != nullptr) {
            bEnableVScrollBar = pScrollBox->GetVScrollBar() != nullptr;
            rcRealBox = pScrollBox->GetPos();
        }
        if (!bEnableVScrollBar || (cyNeeded < rcRealBox.Height())) {
            // Vertical alignment: arrange all items as a whole
            int32_t nOffsetY = 0;
            const VerAlignType vAlignType = GetChildVAlignType();
            if (vAlignType == VerAlignType::kAlignCenter) {
                // Vertically centered
                nOffsetY = (rcBox.Height() - cyNeeded) / 2;
            }
            else if (vAlignType == VerAlignType::kAlignBottom) {
                // Bottom-aligned
                nOffsetY = rcBox.Height() - cyNeeded;
            }
            if (nOffsetY != 0) {
                for (TRowControls& control : rowControlList) {
                    control.m_rowRect.Offset(0, nOffsetY);
                    for (UiRect& rcChild : control.m_pControlRects) {
                        rcChild.Offset(0, nOffsetY);
                    }

                    // The returned area size must include the downward adjustment offset
                    childrenRect.Union(control.m_rowRect);
                }
            }
        }
    }

    // Adjust the layout of the child controls
    if (!bEstimateOnly) {
        // Adjust the layout of the child controls (non-floating controls)
        for (TRowControls& control : rowControlList) {
            ASSERT(control.m_pControlRects.size() == control.m_pControlList.size());
            if (control.m_pControlRects.size() != control.m_pControlList.size()) {
                // Error
                return szChildren;
            }
            const UiRect& rowRect = control.m_rowRect;
            const size_t nCount = control.m_pControlList.size();
            for (size_t nIndex = 0; nIndex < nCount; ++nIndex) {
                Control* pControl = control.m_pControlList[nIndex];
                UiRect& rcChild = control.m_pControlRects[nIndex];

                // Handle the alignment of the control (the alignment set by the control itself)
                // Only the vertical alignment of the control needs to be handled
                const VerAlignType vAlignType = pControl->GetVerAlignType();
                if ((vAlignType == VerAlignType::kAlignCenter) || (vAlignType == VerAlignType::kAlignBottom)) {
                    UiRect rcFullRect = rowRect;
                    UiMargin rcMargin = pControl->GetMargin();// Margin of the child control
                    rcFullRect.top += rcMargin.top;
                    rcFullRect.bottom -= rcMargin.bottom;
                    if (rcChild.Height() < rcFullRect.Height()) {
                        int32_t nOffset = rcFullRect.Height() - rcChild.Height();
                        if (vAlignType == VerAlignType::kAlignCenter) {
                            rcChild.Offset(0, nOffset / 2);
                        }
                        else if (vAlignType == VerAlignType::kAlignBottom) {
                            rcChild.Offset(0, nOffset);
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

UiSize64 HFlowLayout::ArrangeChildren(const std::vector<Control*>& items, UiRect rc, bool bEstimateOnly)
{
    return ArrangeChildInternal(items, rc, bEstimateOnly, false);
}

UiSize64 HFlowLayout::EstimateLayoutSize(const std::vector<Control*>& items, UiSize szAvailable)
{
    UiRect rc(0, 0, szAvailable.cx, szAvailable.cy);
    return ArrangeChildInternal(items, rc, true, true);
}

} // namespace ui
