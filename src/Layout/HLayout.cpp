#include "dui/Layout/HLayout.h"
#include "dui/Core/Box.h"
#include <unordered_map>

namespace ui 
{

HLayout::HLayout()
{
    // Align to the left by default
    SetChildHAlignType(HorAlignType::kAlignLeft);
}

/** Layout algorithm description
I. Layout scheme:
    Arrange controls sequentially by width, in the order: [control left margin] + [control width] + [control right margin] + GetChildMarginX() + ... (no spacing after the last control).
II. When there are stretch controls, the width of a stretch control is calculated as follows:
    1. Total allocatable space = total width (rc.Width()) - total width of fixed controls (self width, excluding margins) - (left margin + right margin) of all controls - (total control count - 1) * GetChildMarginX().
    2. The stretch percentage of a control is directly applied to the total allocatable space (the base of the ratio calculation is the "total allocatable space").
    3. The width is limited by min/max: the final width must be within [minWidth, maxWidth].
    4. When space is insufficient (total allocatable space < the sum of the min widths of all stretch controls), the min width requirements of the preceding controls are satisfied first in order.
    5. The total width after stretching never exceeds the container bounds (effective only when the total width of the fixed controls < the total width rc.Width()).
    6. If the sum of the stretch percentages of all stretch controls is >= 100%, and the total width of the fixed controls < the total width rc.Width(),
       the total width after layout must equal rc.Width() (achieved by allocating the remaining space or reducing the excess space).
    7. If the sum of the stretch percentages of all stretch controls is < 100%, and the total width of the fixed controls < the total width rc.Width(),
       the total width after layout need not equal rc.Width() (the total width may be < rc.Width(), and the remaining space is not forcibly allocated).
    8. Regardless of whether the sum of the stretch percentages is >= 100%, if the target width sum of the stretch controls exceeds the total allocatable space (due to min/max limits),
       the excess must be reduced to ensure the total width does not exceed the container bounds (supplementary rule, corresponding to the "excess space reduction" logic in the code).
    9. If a control's width is 0, the layout is ignored and the control's margins (left margin + right margin) are not counted in the layout.
*/
UiSize64 HLayout::ArrangeChildren(const std::vector<Control*>& items, UiRect rc, bool bEstimateOnly)
{
    if (items.empty()) {
        return UiSize64();
    }
    DeflatePadding(rc);
    const UiSize szAvailable(rc.Width(), rc.Height());

    // Store the sizes of the controls to be laid out (key: control, value: width/height info)
    std::unordered_map<Control*, UiEstSize> itemsMap;       // Non-stretch child controls
    std::vector<Control*> stretchControls;                  // Stretch controls stored in order
    std::unordered_map<Control*, UiEstSize> stretchItemsMap;// Temporary storage of stretch control sizes

    int32_t cxStretchPercentageTotal = 0;  // Total stretch percentage of the stretch controls
    int32_t cxFixedSelfTotal = 0;          // Total self width of fixed controls (excluding margins and spacing)
    int32_t cxFixedTotal = 0;              // Total width of fixed controls (including margins, excluding spacing)
    int32_t totalAllControlsCount = 0;     // Total number of controls participating in the layout (fixed + stretch, non-floating, visible)
    int32_t totalAllMargin = 0;            // Total margin of all controls participating in the layout (left + right)

    // Calculate the base size of each control, store by category + collect key statistics
    for (auto pControl : items) {
        if ((pControl == nullptr) || !pControl->IsVisible() || pControl->IsFloat()) {
            continue;
        }

        totalAllControlsCount++;  // Count the total number of controls participating in the layout
        UiMargin rcMargin = pControl->GetMargin();
        totalAllMargin += rcMargin.left + rcMargin.right;  // Sum the total margins of all controls

        UiEstSize estSize = pControl->EstimateSize(szAvailable);
        UiSize sz = UiSize(estSize.cx.GetInt32(), estSize.cy.GetInt32());

        // Handle the height (stretch type)
        if (estSize.cy.IsStretch()) {
            sz.cy = (CalcStretchValue(estSize.cy, szAvailable.cy) - rcMargin.top - rcMargin.bottom);
        }
        // Clamp the height within the [min, max] range
        sz.cy = std::clamp(sz.cy, pControl->GetMinHeight(), pControl->GetMaxHeight());
        sz.cy = std::max(sz.cy, 0);
        estSize.cy.SetInt32(sz.cy);

        // Handle the width (fixed/stretch classification)
        if (estSize.cx.IsStretch()) {
            cxStretchPercentageTotal += estSize.cx.GetStretchPercentValue();
            stretchControls.push_back(pControl);
            stretchItemsMap[pControl] = estSize;
        }
        else {
            // Clamp the width within the [min, max] range (self width of fixed controls)
            sz.cx = std::clamp(sz.cx, pControl->GetMinWidth(), pControl->GetMaxWidth());
            sz.cx = std::max(sz.cx, 0);
            estSize.cx.SetInt32(sz.cx);

            cxFixedSelfTotal += sz.cx;  // Accumulate the total self width of fixed controls (excluding margins)
            // Accumulate the total width of fixed controls (including margins, excluding spacing)
            if (sz.cx > 0) {
                cxFixedTotal += (sz.cx + rcMargin.left + rcMargin.right);
            }
            itemsMap[pControl] = estSize;
        }
    }

    // Ensure the stretch percentage base is not less than 100 (only used to calculate the ratio; does not affect rule evaluation)
    int32_t cxStretchPercentBase = std::max(cxStretchPercentageTotal, 100);

    // Allocate widths to the stretch controls
    if ((cxFixedTotal < rc.Width()) && !stretchControls.empty()) {
        const int32_t totalSpacing = (totalAllControlsCount - 1) * GetChildMarginX();  // Total spacing (for all controls participating in the layout)

        // 1. Calculate the total allocatable space (strictly per the formula)
        // Total allocatable space = total width - total self width of fixed controls - total margins of all controls - total spacing
        int32_t totalUsableSpace = rc.Width() - cxFixedSelfTotal - totalAllMargin - totalSpacing;
        totalUsableSpace = std::max(totalUsableSpace, 0);

        // Collect the min/max width requirements of the stretch controls
        struct StretchInfo {
            Control* pCtrl;
            UiEstSize estSize;
            UiMargin margin;
            int32_t minWidth;    // Min self width of the control
            int32_t maxWidth;    // Max self width of the control
            int32_t finalWidth;  // Final width (self)
        };
        std::vector<StretchInfo> stretchInfos;
        int32_t totalStretchMin = 0;  // Total min width of the stretch controls (self)

        for (auto pControl : stretchControls) {
            auto& estSize = stretchItemsMap[pControl];
            UiMargin margin = pControl->GetMargin();
            int32_t minW = pControl->GetMinWidth();
            int32_t maxW = pControl->GetMaxWidth();

            stretchInfos.push_back({
                pControl, estSize, margin,
                minW, maxW, 0
                });
            totalStretchMin += minW;
        }

        // 2. Allocate widths per scenario (following all rules)
        if (totalUsableSpace == 0) {
            // Scenario 0: total allocatable space is 0; set all stretch control widths to 0 (to avoid layout overflow)
            for (auto& info : stretchInfos) {
                info.finalWidth = 0;
            }
        }
        else if (totalStretchMin <= totalUsableSpace) {
            // Scenario 1: available space satisfies the min width requirements of all stretch controls
            // 1. Calculate the target width as allocatable space x ratio (rule 2)
            for (auto& info : stretchInfos) {
                int32_t targetWidth = static_cast<int32_t>((float)info.estSize.cx.GetStretchPercentValue() * totalUsableSpace / cxStretchPercentBase + 0.5f);
                // 2. Clamp by min/max (rule 3)
                info.finalWidth = std::clamp(targetWidth, info.minWidth, info.maxWidth);
            }

            // 3. Calculate the current total width of the stretch controls; handle the remaining/excess space (rules 6, 7, 8)
            int32_t currentStretchTotal = 0;
            for (auto& info : stretchInfos) {
                currentStretchTotal += info.finalWidth;
            }

            int32_t diff = totalUsableSpace - currentStretchTotal;
            if (diff > 0) {
                // Space remains: force allocation only when the percentage is >= 100 (rule 6)
                if (cxStretchPercentageTotal >= 100) {
                    // Calculate the total extra space that can be allocated (sum of maxWidth - finalWidth over all controls)
                    int64_t totalAdjustableSpace = 0;
                    for (auto& info : stretchInfos) {
                        totalAdjustableSpace += (info.maxWidth - info.finalWidth);
                    }
                    if (totalAdjustableSpace <= 0) {
                        // No adjustable space available; exit directly
                    }
                    else {
                        int32_t remainingDiff = diff;
                        // Batch allocation: allocate most of the diff proportionally
                        for (auto& info : stretchInfos) {
                            if (remainingDiff <= 0) break;
                            int64_t ctrlAdjustable = info.maxWidth - info.finalWidth;
                            if (ctrlAdjustable <= 0) continue;

                            // Allocate the current control's adjustable space proportionally (use integer arithmetic to avoid floating-point precision issues)
                            int64_t batchAdd = (ctrlAdjustable * remainingDiff) / totalAdjustableSpace;
                            // Correct calculation deviations (ensure the remaining diff and the control's adjustable space are not exceeded)
                            batchAdd = std::min({ batchAdd, (int64_t)remainingDiff, ctrlAdjustable });
                            if (batchAdd > 0) {
                                // Verify that batchAdd is within the int32_t range
                                ASSERT(batchAdd <= static_cast<int64_t>(INT32_MAX));
                                int32_t batchAdd32 = ui::TruncateToInt32(batchAdd);
                                info.finalWidth += batchAdd32;
                                remainingDiff -= batchAdd32;
                                totalAdjustableSpace -= batchAdd;
                            }
                        }

                        // A small amount of diff remains (usually 0 to the total control count); allocate pixel by pixel
                        while (remainingDiff > 0) {
                            bool allocated = false;
                            for (auto& info : stretchInfos) {
                                if (info.finalWidth < info.maxWidth) {
                                    info.finalWidth += 1;
                                    remainingDiff -= 1;
                                    allocated = true;
                                    if (remainingDiff <= 0) break;
                                }
                            }
                            if (!allocated) break; // No adjustable space available
                        }
                    }
                }
            }
            else if (diff < 0) {
                // Space exceeds: must be reduced regardless of whether the percentage is >= 100 (rule 8)
                int32_t reduceDiff = -diff; // Avoid negative value operations
                // Calculate the total space that can be reduced (sum of finalWidth - minWidth over all controls)
                int64_t totalAdjustableSpace = 0;
                for (auto& info : stretchInfos) {
                    totalAdjustableSpace += (info.finalWidth - info.minWidth);
                }
                if (totalAdjustableSpace <= 0) {
                    // No adjustable space available; exit directly
                }
                else {
                    int32_t remainingDiff = reduceDiff;
                    // Batch reduction: reduce most of the diff proportionally
                    for (auto& info : stretchInfos) {
                        if (remainingDiff <= 0) break;
                        int64_t ctrlAdjustable = info.finalWidth - info.minWidth;
                        if (ctrlAdjustable <= 0) continue;

                        // Reduce the current control's adjustable space proportionally (integer arithmetic avoids precision issues)
                        int64_t batchSub = (ctrlAdjustable * remainingDiff) / totalAdjustableSpace;
                        // Correct calculation deviations (ensure the remaining diff and the control's adjustable space are not exceeded)
                        batchSub = std::min({ batchSub, (int64_t)remainingDiff, ctrlAdjustable });
                        if (batchSub > 0) {
                            // Verify that batchSub is within the int32_t range
                            ASSERT(batchSub <= static_cast<int64_t>(INT32_MAX));
                            int32_t batchSub32 = ui::TruncateToInt32(batchSub);
                            info.finalWidth -= batchSub32;
                            remainingDiff -= batchSub32;
                            totalAdjustableSpace -= batchSub;
                        }
                    }

                    // A small amount of diff remains (usually 0 to the total control count); reduce pixel by pixel
                    while (remainingDiff > 0) {
                        bool reduced = false;
                        for (auto& info : stretchInfos) {
                            if (info.finalWidth > info.minWidth) {
                                info.finalWidth -= 1;
                                remainingDiff -= 1;
                                reduced = true;
                                if (remainingDiff <= 0) break;
                            }
                        }
                        if (!reduced) break; // No adjustable space available
                    }
                }
            }
        }
        else {
            // Scenario 2: insufficient available space; prioritize the preceding controls (rule 4)
            int32_t remainingSpace = totalUsableSpace;
            for (auto& info : stretchInfos) {
                if (remainingSpace <= 0) break;

                // Allocate the min width first
                int32_t allocWidth = std::min(info.minWidth, remainingSpace);
                info.finalWidth = allocWidth;
                remainingSpace -= allocWidth;
            }

            // If space still remains, allocate it to the preceding controls (proportionally; the < 100% scenario is not specially handled)
            if (remainingSpace > 0) {
                for (auto& info : stretchInfos) {
                    if (remainingSpace <= 0) break;
                    if (info.finalWidth >= info.maxWidth) continue;

                    int32_t maxAdd = info.maxWidth - info.finalWidth;
                    int32_t addWidth = static_cast<int32_t>(
                        1.0f * info.estSize.cx.GetStretchPercentValue() * remainingSpace / cxStretchPercentBase + 0.5f
                        );
                    addWidth = std::min({ addWidth, maxAdd, remainingSpace });

                    info.finalWidth += addWidth;
                    remainingSpace -= addWidth;
                }
            }
        }

        // Update the stretch control sizes into itemsMap
        for (auto& info : stretchInfos) {
            info.estSize.cx.SetInt32(info.finalWidth);
            itemsMap[info.pCtrl] = info.estSize;
        }
    }

    // Handle stretch controls that were not allocated space (set the width to 0)
    for (auto pControl : stretchControls) {
        if (itemsMap.find(pControl) == itemsMap.end()) {
            UiEstSize estSize;
            estSize.cx.SetInt32(0);
            itemsMap[pControl] = estSize;
        }
    }
    stretchItemsMap.clear();
    stretchControls.clear();

    // Calculate the final positions of the child controls (in the order: left margin + control width + right margin + spacing)
    std::vector<Control*> childrenControlList;  // Control list
    std::vector<UiRect> childrenControlRects;   // Positions and sizes of the controls
    UiRect childrenRect;                        // Original bounds of the child controls (before alignment)
    int32_t nPosX = rc.left;
    int32_t nPosY = rc.top;
    bool isFirstControl = true; // Marks whether it is the first control; no spacing is added before the first control

    for (auto pControl : items) {
        if ((pControl == nullptr) || !pControl->IsVisible() || pControl->IsFloat()) {
            continue;
        }

        UiRect rcChild;
        auto estSizeIter = itemsMap.find(pControl);
        if ((estSizeIter == itemsMap.end()) || (estSizeIter->second.cx.GetInt32() <= 0)) {
            // Zero width; ignore the layout
            rcChild = { nPosX, nPosY, nPosX, nPosY };
        }
        else {
            // Lay out one control (strictly per the layout scheme)
            const UiEstSize& estSize = estSizeIter->second;
            UiMargin rcMargin = pControl->GetMargin();

            // Handle the spacing uniformly (added before controls other than the first)
            if (!isFirstControl) {
                nPosX += GetChildMarginX();
            }
            isFirstControl = false;

            // Horizontal position calculation (including margins)
            nPosX += rcMargin.left;          // Left margin
            rcChild.left = nPosX;
            rcChild.right = rcChild.left + estSize.cx.GetInt32(); // Self width of the control
            nPosX = rcChild.right + rcMargin.right; // Right margin

            // Vertical position calculation (handles alignment)
            rcChild.top = nPosY + rcMargin.top;
            rcChild.bottom = rcChild.top + estSize.cy.GetInt32();

            // Handle vertical alignment (based on the container bounds with the padding removed)
            VerAlignType vAlign = pControl->GetVerAlignType();
            if (vAlign == VerAlignType::kAlignCenter || vAlign == VerAlignType::kAlignBottom) {
                UiRect rcValidArea = rc; // Valid bounds of the container (padding already removed)

                // Subtract the control's margins
                rcValidArea.top += rcMargin.top;
                rcValidArea.bottom -= rcMargin.bottom;

                int32_t nOffset = rcValidArea.Height() - rcChild.Height();
                if (nOffset > 0) {
                    if (vAlign == VerAlignType::kAlignCenter) {
                        rcChild.Offset(0, nOffset / 2);
                    }
                    else if (vAlign == VerAlignType::kAlignBottom) {
                        rcChild.Offset(0, nOffset);
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

    // Handle the overall horizontal alignment of the child controls (centered/right)
    UiRect fullChildrenRect = childrenRect;     // Overall bounds of the child controls (including the alignment offset)
    UiRect alignedChildrenRect = childrenRect;  // Actual bounds of the child controls after alignment
    const HorAlignType hAlign = GetChildHAlignType();
    int32_t containerWidth = rc.Width();

    if (((hAlign == HorAlignType::kAlignCenter) || (hAlign == HorAlignType::kAlignRight)) &&
        childrenRect.Width() > 0 &&
        childrenRect.Width() < containerWidth) {

        int32_t nOffsetX = 0;
        if (hAlign == HorAlignType::kAlignCenter) {
            nOffsetX = (containerWidth - childrenRect.Width()) / 2;
        }
        else if (hAlign == HorAlignType::kAlignRight) {
            nOffsetX = containerWidth - childrenRect.Width();
        }

        // Apply the horizontal offset
        if (nOffsetX != 0) {
            alignedChildrenRect.Offset(nOffsetX, 0);
            for (UiRect& rcChild : childrenControlRects) {
                rcChild.Offset(nOffsetX, 0);
            }
            fullChildrenRect.Union(alignedChildrenRect);
        }
    }

    // Apply the final layout (non-estimation mode)
    if (!bEstimateOnly) {
        // Handle non-floating controls
        ASSERT(childrenControlList.size() == childrenControlRects.size());
        if (childrenControlList.size() == childrenControlRects.size()) {
            const size_t nCount = childrenControlList.size();
            for (size_t i = 0; i < nCount; ++i) {
                Control* pControl = childrenControlList[i];
                pControl->SetPos(childrenControlRects[i]);
            }
        }

        // Handle floating controls
        for (auto pControl : items) {
            if ((pControl != nullptr) && pControl->IsVisible() && pControl->IsFloat()) {
                SetFloatPos(pControl, rc);
            }
        }
    }

    // Calculate the final returned size (including the parent container's padding)
    UiPadding rcPadding = (GetOwner() != nullptr) ? GetOwner()->GetPadding() : UiPadding();
    UiSize64 resultSize(
        (int64_t)std::max(fullChildrenRect.Width() + rcPadding.left + rcPadding.right, 0),
        (int64_t)std::max(fullChildrenRect.Height() + rcPadding.top + rcPadding.bottom, 0)
    );
    return resultSize;
}

UiSize64 HLayout::EstimateLayoutSize(const std::vector<Control*>& items, UiSize szAvailable)
{
    // Width: sum of the widths of all valid child controls (including margins) + child spacing + container padding
    // Height: maximum height of all valid child controls (including margins) + container padding
    // Valid child controls: visible, non-floating; stretch controls use their minimum size in the calculation
    UiSize64 totalSize(0, 0);  // 64-bit calculation to avoid overflow
    int32_t validCount = 0;    // Count the valid controls participating in the spacing calculation (those with width or margins)

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

        // Handle the width (including stretch logic and boundary limits)
        int32_t itemWidth = 0;
        if (estSize.cx.IsStretch()) {
            itemWidth = std::max(minWidth, 0);  // Stretch controls use the min width (at least 0)
        }
        else {
            // Non-stretch controls: clamp within [minWidth, maxWidth]
            itemWidth = std::clamp(estSize.cx.GetInt32(), minWidth, maxWidth);
        }

        // Handle the height (including stretch logic and boundary limits)
        int32_t itemHeight = 0;
        if (estSize.cy.IsStretch()) {
            itemHeight = std::max(minHeight, 0);  // Stretch controls use the min height (at least 0)
        }
        else {
            // Non-stretch controls: clamp within [minHeight, maxHeight]
            itemHeight = std::clamp(estSize.cy.GetInt32(), minHeight, maxHeight);
        }

        // Accumulate the width (including the control width and the left/right margins, counted unconditionally)
        if (itemWidth > 0) {
            int64_t widthWithMargin = (int64_t)itemWidth + rcMargin.left + rcMargin.right;
            totalSize.cx += widthWithMargin;

            // Count the valid controls (those with width or margins participate in the spacing calculation)
            validCount++;
        }

        // Calculate the max height (including the control height and the top/bottom margins, always compared)
        if (itemHeight > 0) {
            int64_t heightWithMargin = (int64_t)itemHeight + rcMargin.top + rcMargin.bottom;
            if (heightWithMargin > totalSize.cy) {
                totalSize.cy = heightWithMargin;
            }
        }
    }

    // Accumulate the child spacing (when the number of valid controls > 1)
    if (validCount > 1) {
        totalSize.cx += (int64_t)(validCount - 1) * GetChildMarginX();
    }

    // Accumulate the container padding (counted unconditionally)
    UiPadding rcPadding;
    if (GetOwner() != nullptr) {
        rcPadding = GetOwner()->GetPadding();
    }
    totalSize.cx += rcPadding.left + rcPadding.right;
    totalSize.cy += rcPadding.top + rcPadding.bottom;

    // Check the configuration only when both the width and height are 0 (to avoid misjudging a valid stretch layout)
    if (totalSize.cx == 0 && totalSize.cy == 0) {
        CheckConfig(items);
    }

    return totalSize;
}

} // namespace ui
