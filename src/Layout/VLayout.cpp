#include "dui/Layout/VLayout.h"
#include "dui/Core/Box.h"
#include <unordered_map>

namespace ui 
{

VLayout::VLayout()
{
    // Align to the top by default
    SetChildVAlignType(VerAlignType::kAlignTop);
}

/** Vertical layout algorithm description
I. Layout scheme:
    Children are arranged sequentially by height, in the order: [top margin] + [child height] + [bottom margin] + GetChildMarginY () + ... (no spacing after the last child).
II. When stretch controls exist, their heights are computed by the following algorithm:
    1. Total usable space = total height (rc.Height ()) - total height of fixed controls (own height, excluding margins) - (top margin + bottom margin) of all controls - (total control count - 1) * GetChildMarginY ().
    2. The stretch ratio of a control applies directly to the total usable space (the ratio is computed based on the "total usable space").
    3. Height is limited by min/max: the final height must be within the [minHeight, maxHeight] range.
    4. When space is insufficient (total usable space < the sum of the minimum heights of all stretch controls), the minimum height requirements of the upper controls are satisfied first in order.
    5. The total height after stretching will not exceed the container bounds (only takes effect when the total height of fixed controls < total height rc.Height ()).
    6. If the sum of the stretch ratios of all stretch controls is >= 100%, and the total height of fixed controls < total height rc.Height (),
       the total height after layout must be equal to rc.Height () (achieved by allocating the remaining space or reducing the excess space).
    7. If the sum of the stretch ratios of all stretch controls is < 100%, and the total height of fixed controls < total height rc.Height (),
       the total height after layout does not need to be equal to rc.Height () (total height < rc.Height () is allowed; the remaining space is not forcibly allocated).
    8. Whether the sum of the stretch ratios is >= 100% or not, if the sum of the target heights of stretch controls exceeds the total usable space (due to min/max limits),
       the excess must be reduced to ensure the total height does not exceed the container bounds (supplementary rule, corresponding to the "space excess reduction" logic in the code).
    9. If a control's height is 0, it is ignored in the layout, and its outer margins (top margin + bottom margin) are not counted in the layout.
*/
UiSize64 VLayout::ArrangeChildren(const std::vector<Control*>& items, UiRect rc, bool bEstimateOnly)
{
    if (items.empty()) {
        return UiSize64();
    }
    DeflatePadding(rc);
    const UiSize szAvailable(rc.Width(), rc.Height());

    // Store the sizes of controls to be laid out (key: control, value: width/height info)
    std::unordered_map<Control*, UiEstSize> itemsMap;       // non-stretch child controls
    std::vector<Control*> stretchControls;                  // stretch controls stored in order
    std::unordered_map<Control*, UiEstSize> stretchItemsMap;// temporary storage of stretch control sizes

    int32_t cyStretchPercentageTotal = 0;  // total percentage of stretch controls (vertical)
    int32_t cyFixedSelfTotal = 0;          // total own height of fixed controls (excluding margins and spacing)
    int32_t cyFixedTotal = 0;              // total height of fixed controls (including margins, excluding spacing)
    int32_t totalAllControlsCount = 0;     // total number of controls participating in layout (fixed + stretch, non-float, visible)
    int32_t totalAllMargin = 0;            // sum of margins of all controls participating in layout (top + bottom)

    // Compute the base size of each control, store by category + collect key parameters
    for (auto pControl : items) {
        if ((pControl == nullptr) || !pControl->IsVisible() || pControl->IsFloat()) {
            continue;
        }

        totalAllControlsCount++;  // Count the total number of controls participating in layout
        UiMargin rcMargin = pControl->GetMargin();
        totalAllMargin += rcMargin.top + rcMargin.bottom;  // Sum the margins of all controls (top + bottom)

        UiEstSize estSize = pControl->EstimateSize(szAvailable);
        UiSize sz = UiSize(estSize.cx.GetInt32(), estSize.cy.GetInt32());

        // Handle width (stretch type)
        if (estSize.cx.IsStretch()) {
            sz.cx = (CalcStretchValue(estSize.cx, szAvailable.cx) - rcMargin.left - rcMargin.right);
        }
        // Clamp the width to the [min, max] range
        sz.cx = std::clamp(sz.cx, pControl->GetMinWidth(), pControl->GetMaxWidth());
        sz.cx = std::max(sz.cx, 0);
        estSize.cx.SetInt32(sz.cx);

        // Handle height (fixed/stretch classification)
        if (estSize.cy.IsStretch()) {
            cyStretchPercentageTotal += estSize.cy.GetStretchPercentValue();
            stretchControls.push_back(pControl);
            stretchItemsMap[pControl] = estSize;
        }
        else {
            // Clamp the height to the [min, max] range (own height of fixed controls)
            sz.cy = std::clamp(sz.cy, pControl->GetMinHeight(), pControl->GetMaxHeight());
            sz.cy = std::max(sz.cy, 0);
            estSize.cy.SetInt32(sz.cy);

            cyFixedSelfTotal += sz.cy;  // Accumulate the total own height of fixed controls (excluding margins)
            // Accumulate the total height of fixed controls (including margins, excluding spacing)
            if (sz.cy > 0) {
                cyFixedTotal += (sz.cy + rcMargin.top + rcMargin.bottom);
            }
            itemsMap[pControl] = estSize;
        }
    }

    // Ensure the stretch percentage base is no less than 100 (only for ratio computation, does not affect rule evaluation)
    int32_t cyStretchPercentBase = std::max(cyStretchPercentageTotal, 100);

    // Handle height allocation for stretch controls
    if ((cyFixedTotal < rc.Height()) && !stretchControls.empty()) {
        const int32_t totalSpacing = (totalAllControlsCount - 1) * GetChildMarginY();  // Total spacing (for all controls participating in layout; Y-axis spacing is used vertically)

        // 1. Compute the total usable space (strictly per the formula; height is used vertically)
        // Total usable space = total height - total own height of fixed controls - sum of all control margins (top + bottom) - total spacing
        int32_t totalUsableSpace = rc.Height() - cyFixedSelfTotal - totalAllMargin - totalSpacing;
        totalUsableSpace = std::max(totalUsableSpace, 0);

        // Collect the min/max height requirements of stretch controls
        struct StretchInfo {
            Control* pCtrl;
            UiEstSize estSize;
            UiMargin margin;
            int32_t minHeight;   // own minimum height of the control
            int32_t maxHeight;   // own maximum height of the control
            int32_t finalHeight; // final height (own)
        };
        std::vector<StretchInfo> stretchInfos;
        int32_t totalStretchMin = 0;  // total minimum height of stretch controls (own)

        for (auto pControl : stretchControls) {
            auto& estSize = stretchItemsMap[pControl];
            UiMargin margin = pControl->GetMargin();
            int32_t minH = pControl->GetMinHeight();
            int32_t maxH = pControl->GetMaxHeight();

            stretchInfos.push_back({
                pControl, estSize, margin,
                minH, maxH, 0
                });
            totalStretchMin += minH;
        }

        // 2. Allocate heights by scenario (following all rules)
        if (totalUsableSpace == 0) {
            // Scenario 0: total usable space is 0; set all stretch control heights to 0 (avoid layout overflow)
            for (auto& info : stretchInfos) {
                info.finalHeight = 0;
            }
        }
        else if (totalStretchMin <= totalUsableSpace) {
            // Scenario 1: usable space satisfies the minimum height requirements of all stretch controls
            // 1. Compute the target height as total usable space x ratio (rule 2)
            for (auto& info : stretchInfos) {
                int32_t targetHeight = static_cast<int32_t>((float)info.estSize.cy.GetStretchPercentValue() * totalUsableSpace / cyStretchPercentBase + 0.5f);
                // 2. Limited by min/max (rule 3)
                info.finalHeight = std::clamp(targetHeight, info.minHeight, info.maxHeight);
            }

            // 3. Compute the current total height of stretch controls; handle remaining/excess space (rules 6, 7, 8)
            int32_t currentStretchTotal = 0;
            for (auto& info : stretchInfos) {
                currentStretchTotal += info.finalHeight;
            }

            int32_t diff = totalUsableSpace - currentStretchTotal;
            if (diff > 0) {
                // Space remains: allocate forcibly only when the ratio is >= 100 (rule 6)
                if (cyStretchPercentageTotal >= 100) {
                    // Compute the total adjustable extra space (sum of maxHeight - finalHeight over all controls)
                    int64_t totalAdjustableSpace = 0;
                    for (auto& info : stretchInfos) {
                        totalAdjustableSpace += (info.maxHeight - info.finalHeight);
                    }
                    if (totalAdjustableSpace <= 0) {
                        // No adjustable space available; exit directly
                    }
                    else {
                        int32_t remainingDiff = diff;
                        // Batch allocation: allocate most of the diff proportionally
                        for (auto& info : stretchInfos) {
                            if (remainingDiff <= 0) break;
                            int64_t ctrlAdjustable = info.maxHeight - info.finalHeight;
                            if (ctrlAdjustable <= 0) continue;

                            // Allocate the current control's adjustable space proportionally (use integer arithmetic to avoid floating-point precision issues)
                            int64_t batchAdd = (ctrlAdjustable * remainingDiff) / totalAdjustableSpace;
                            // Correct the computation deviation (ensure it does not exceed the remaining diff and the control's adjustable space)
                            batchAdd = std::min({ batchAdd, (int64_t)remainingDiff, ctrlAdjustable });
                            if (batchAdd > 0) {
                                // Verify that batchAdd is within the int32_t range
                                ASSERT(batchAdd <= static_cast<int64_t>(INT32_MAX));
                                int32_t batchAdd32 = ui::TruncateToInt32(batchAdd);
                                info.finalHeight += batchAdd32;
                                remainingDiff -= batchAdd32;
                                totalAdjustableSpace -= batchAdd;
                            }
                        }

                        // A small amount of diff remains (usually 0 to the control count); allocate pixel by pixel
                        while (remainingDiff > 0) {
                            bool allocated = false;
                            for (auto& info : stretchInfos) {
                                if (info.finalHeight < info.maxHeight) {
                                    info.finalHeight += 1;
                                    remainingDiff -= 1;
                                    allocated = true;
                                    if (remainingDiff <= 0) break;
                                }
                            }
                            if (!allocated) break;
                        }
                    }
                }
            }
            else if (diff < 0) {
                // Space exceeds: must be reduced regardless of whether the ratio is >= 100 (rule 8)
                int32_t reduceDiff = -diff; // avoid operations on negative values
                // Compute the total reducible space (sum of finalHeight - minHeight over all controls)
                int64_t totalAdjustableSpace = 0;
                for (auto& info : stretchInfos) {
                    totalAdjustableSpace += (info.finalHeight - info.minHeight);
                }
                if (totalAdjustableSpace <= 0) {
                    // No adjustable space available; exit directly
                }
                else {
                    int32_t remainingDiff = reduceDiff;
                    // Batch reduction: reduce most of the diff proportionally
                    for (auto& info : stretchInfos) {
                        if (remainingDiff <= 0) break;
                        int64_t ctrlAdjustable = info.finalHeight - info.minHeight;
                        if (ctrlAdjustable <= 0) continue;

                        // Reduce the current control's adjustable space proportionally (integer arithmetic avoids precision issues)
                        int64_t batchSub = (ctrlAdjustable * remainingDiff) / totalAdjustableSpace;
                        // Correct the computation deviation (ensure it does not exceed the remaining diff and the control's adjustable space)
                        batchSub = std::min({ batchSub, (int64_t)remainingDiff, ctrlAdjustable });
                        if (batchSub > 0) {
                            // Verify that batchSub is within the int32_t range
                            ASSERT(batchSub <= static_cast<int64_t>(INT32_MAX));
                            int32_t batchSub32 = ui::TruncateToInt32(batchSub);
                            info.finalHeight -= batchSub32;
                            remainingDiff -= batchSub32;
                            totalAdjustableSpace -= batchSub;
                        }
                    }

                    // A small amount of diff remains (usually 0 to the control count); reduce pixel by pixel
                    while (remainingDiff > 0) {
                        bool reduced = false;
                        for (auto& info : stretchInfos) {
                            if (info.finalHeight > info.minHeight) {
                                info.finalHeight -= 1;
                                remainingDiff -= 1;
                                reduced = true;
                                if (remainingDiff <= 0) break;
                            }
                        }
                        if (!reduced) break; // no adjustable space available
                    }
                }
            }
        }
        else {
            // Scenario 2: usable space is insufficient; prioritize displaying the upper controls (vertical order)
            int32_t remainingSpace = totalUsableSpace;
            for (auto& info : stretchInfos) {
                if (remainingSpace <= 0) break;

                // Allocate the minimum height first
                int32_t allocHeight = std::min(info.minHeight, remainingSpace);
                info.finalHeight = allocHeight;
                remainingSpace -= allocHeight;
            }

            // If space still remains, allocate it to the upper controls (proportionally)
            if (remainingSpace > 0) {
                for (auto& info : stretchInfos) {
                    if (remainingSpace <= 0) break;
                    if (info.finalHeight >= info.maxHeight) continue;

                    int32_t maxAdd = info.maxHeight - info.finalHeight;
                    int32_t addHeight = static_cast<int32_t>(
                        1.0f * info.estSize.cy.GetStretchPercentValue() * remainingSpace / cyStretchPercentBase + 0.5f
                        );
                    addHeight = std::min({ addHeight, maxAdd, remainingSpace });

                    info.finalHeight += addHeight;
                    remainingSpace -= addHeight;
                }
            }
        }

        // Update the stretch control sizes in itemsMap
        for (auto& info : stretchInfos) {
            info.estSize.cy.SetInt32(info.finalHeight);
            itemsMap[info.pCtrl] = info.estSize;
        }
    }

    // Handle stretch controls without allocated space (set height to 0)
    for (auto pControl : stretchControls) {
        if (itemsMap.find(pControl) == itemsMap.end()) {
            UiEstSize estSize;
            estSize.cy.SetInt32(0);
            itemsMap[pControl] = estSize;
        }
    }
    stretchItemsMap.clear();
    stretchControls.clear();

    // Compute the final positions of child controls (in the order: top margin + child height + bottom margin + spacing)
    std::vector<Control*> childrenControlList;  // control list
    std::vector<UiRect> childrenControlRects;   // positions and sizes of the controls
    UiRect childrenRect;                        // original bounds of the child controls (before alignment)
    int32_t nPosX = rc.left;
    int32_t nPosY = rc.top;
    bool isFirstControl = true; // Whether this is the first control; no spacing is added before the first control

    for (auto pControl : items) {
        if ((pControl == nullptr) || !pControl->IsVisible() || pControl->IsFloat()) {
            continue;
        }

        UiRect rcChild;
        auto estSizeIter = itemsMap.find(pControl);
        if ((estSizeIter == itemsMap.end()) || (estSizeIter->second.cy.GetInt32() <= 0)) {
            // Height is 0; ignore in the layout
            rcChild = { nPosX, nPosY, nPosX, nPosY };
        }
        else {
            // Lay out a control (strictly per the layout scheme)
            const UiEstSize& estSize = estSizeIter->second;
            UiMargin rcMargin = pControl->GetMargin();

            // Handle spacing uniformly (add it before non-first controls)
            if (!isFirstControl) {
                nPosY += GetChildMarginY();  // Y-axis spacing is used vertically
            }
            isFirstControl = false;

            // Compute the vertical position (including margins)
            nPosY += rcMargin.top;           // top margin
            rcChild.top = nPosY;
            rcChild.bottom = rcChild.top + estSize.cy.GetInt32(); // own height of the control
            nPosY = rcChild.bottom + rcMargin.bottom; // bottom margin

            // Compute the horizontal position (handle alignment)
            rcChild.left = nPosX + rcMargin.left;
            rcChild.right = rcChild.left + estSize.cx.GetInt32();

            // Handle horizontal alignment (based on the container bounds excluding padding)
            HorAlignType hAlign = pControl->GetHorAlignType();
            if (hAlign == HorAlignType::kAlignCenter || hAlign == HorAlignType::kAlignRight) {
                UiRect rcValidArea = rc; // effective area of the container (padding already subtracted)

                // Subtract the control's outer margins
                rcValidArea.left += rcMargin.left;
                rcValidArea.right -= rcMargin.right;

                int32_t nOffset = rcValidArea.Width() - rcChild.Width();
                if (nOffset > 0) {
                    if (hAlign == HorAlignType::kAlignCenter) {
                        rcChild.Offset(nOffset / 2, 0);
                    }
                    else if (hAlign == HorAlignType::kAlignRight) {
                        rcChild.Offset(nOffset, 0);
                    }
                }
            }

            // Update the original bounds of the child controls (including margins)
            UiRect rcChildWithMargin = rcChild;
            rcChildWithMargin.top -= rcMargin.top;
            rcChildWithMargin.bottom += rcMargin.bottom;
            rcChildWithMargin.left -= rcMargin.left;
            rcChildWithMargin.right += rcMargin.right;
            childrenRect.Union(rcChildWithMargin);
        }

        childrenControlList.push_back(pControl);
        childrenControlRects.push_back(rcChild);
    }

    // Handle the overall vertical alignment of child controls (center/bottom)
    UiRect fullChildrenRect = childrenRect;     // overall bounds of the child controls (including the alignment offset)
    UiRect alignedChildrenRect = childrenRect;  // actual bounds of the child controls after alignment
    const VerAlignType vAlign = GetChildVAlignType();  // vertical alignment type
    int32_t containerHeight = rc.Height();

    if (((vAlign == VerAlignType::kAlignCenter) || (vAlign == VerAlignType::kAlignBottom)) &&
        childrenRect.Height() > 0 &&
        childrenRect.Height() < containerHeight) {

        int32_t nOffsetY = 0;
        if (vAlign == VerAlignType::kAlignCenter) {
            nOffsetY = (containerHeight - childrenRect.Height()) / 2;
        }
        else if (vAlign == VerAlignType::kAlignBottom) {
            nOffsetY = containerHeight - childrenRect.Height();
        }

        // Apply the vertical offset
        if (nOffsetY != 0) {
            alignedChildrenRect.Offset(0, nOffsetY);
            for (UiRect& rcChild : childrenControlRects) {
                rcChild.Offset(0, nOffsetY);
            }
            fullChildrenRect.Union(alignedChildrenRect);
        }
    }

    // Apply the final layout (non-estimate mode)
    if (!bEstimateOnly) {
        // Handle non-float controls
        ASSERT(childrenControlList.size() == childrenControlRects.size());
        if (childrenControlList.size() == childrenControlRects.size()) {
            const size_t nCount = childrenControlList.size();
            for (size_t i = 0; i < nCount; ++i) {
                Control* pControl = childrenControlList[i];
                pControl->SetPos(childrenControlRects[i]);
            }
        }

        // Handle float controls
        for (auto pControl : items) {
            if ((pControl != nullptr) && pControl->IsVisible() && pControl->IsFloat()) {
                SetFloatPos(pControl, rc);  // The float control position calculation logic is consistent with the horizontal layout (usually based on the parent container)
            }
        }
    }

    // Compute the final returned size (including the parent container padding)
    UiPadding rcPadding = (GetOwner() != nullptr) ? GetOwner()->GetPadding() : UiPadding();
    UiSize64 resultSize(
        (int64_t)std::max(fullChildrenRect.Width() + rcPadding.left + rcPadding.right, 0),
        (int64_t)std::max(fullChildrenRect.Height() + rcPadding.top + rcPadding.bottom, 0)
    );
    return resultSize;
}

UiSize64 VLayout::EstimateLayoutSize(const std::vector<Control*>& items, UiSize szAvailable)
{
    // Width: the maximum width of all valid child controls (including margins) + container padding
    // Height: the sum of heights of all valid child controls (including margins) + child spacing + container padding
    // Valid child controls: visible and non-float; stretch controls use their minimum size in the computation
    UiSize64 totalSize(0, 0);  // 64-bit computation to avoid overflow
    int32_t validCount = 0;    // Count the valid controls participating in spacing computation (those with height or margins)

    for (Control* pControl : items) {
        if ((pControl == nullptr) || !pControl->IsVisible() || pControl->IsFloat()) {
            continue;
        }

        UiMargin rcMargin = pControl->GetMargin();
        UiEstSize estSize = pControl->EstimateSize(szAvailable);
        int32_t minWidth = pControl->GetMinWidth();
        int32_t maxWidth = pControl->GetMaxWidth();
        int32_t minHeight = pControl->GetMinHeight();
        int32_t maxHeight = pControl->GetMaxHeight();

        // Handle width (including stretch logic and boundary limits)
        int32_t itemWidth = 0;
        if (estSize.cx.IsStretch()) {
            itemWidth = std::max(minWidth, 0);  // stretch controls use the minimum width (at least 0)
        }
        else {
            // Non-stretch controls: clamp to the [minWidth, maxWidth] range with std::clamp
            itemWidth = std::clamp(estSize.cx.GetInt32(), minWidth, maxWidth);
        }

        // Handle height (including stretch logic and boundary limits)
        int32_t itemHeight = 0;
        if (estSize.cy.IsStretch()) {
            itemHeight = std::max(minHeight, 0);  // stretch controls use the minimum height (at least 0)
        }
        else {
            // Non-stretch controls: clamp to the [minHeight, maxHeight] range with std::clamp
            itemHeight = std::clamp(estSize.cy.GetInt32(), minHeight, maxHeight);
        }

        // Compute the maximum width (including the control width and left/right margins; always considered in the comparison)
        if (itemWidth > 0) {
            int64_t widthWithMargin = (int64_t)itemWidth + rcMargin.left + rcMargin.right;
            if (widthWithMargin > totalSize.cx) {
                totalSize.cx = widthWithMargin;
            }
        }

        // Accumulate heights (including the control height and top/bottom margins; always counted)
        if (itemHeight > 0) {
            int64_t heightWithMargin = (int64_t)itemHeight + rcMargin.top + rcMargin.bottom;
            totalSize.cy += heightWithMargin;

            // Count valid controls (those with height or margins participate in the spacing computation)
            validCount++;
        }
    }

    // Accumulate the child spacing (when valid controls > 1, ChildMarginY is used vertically)
    if (validCount > 1) {
        totalSize.cy += (int64_t)(validCount - 1) * GetChildMarginY();
    }

    // Accumulate the container padding (always counted)
    UiPadding rcPadding;
    if (GetOwner() != nullptr) {
        rcPadding = GetOwner()->GetPadding();
    }
    totalSize.cx += rcPadding.left + rcPadding.right;
    totalSize.cy += rcPadding.top + rcPadding.bottom;

    // Check the configuration only when both width and height are 0 (avoid misjudging a valid stretch layout)
    if (totalSize.cx == 0 && totalSize.cy == 0) {
        CheckConfig(items);
    }

    return totalSize;
}

} // namespace ui
