#ifndef UI_LAYOUT_VFLOWLAYOUT_H_
#define UI_LAYOUT_VFLOWLAYOUT_H_

#include "dui/Layout/Layout.h"

namespace ui 
{

/** Vertical flow layout: child controls are arranged in sequence vertically; when the target area height is exceeded, the column wraps automatically
 *  Horizontal alignment: none by default; child controls are arranged according to the alignment specified by each child control (within the same column)
 *  Vertical alignment: top-aligned by default; the vertical alignment specified by a child control itself does not take effect
 */
class DUI_API VFlowLayout : public Layout
{
    typedef Layout BaseClass;
public:
    VFlowLayout();

    /** Layout type
    */
    virtual LayoutType GetLayoutType() const override { return LayoutType::VFlowLayout; }

    /** Arrange the positions and sizes of all internal child controls according to the layout strategy
     * @param [in] items List of child controls
     * @param [in] rc Position and size information of the current container, including padding but not margin
     * @param [in] bEstimateOnly true means only estimate without adjusting control positions; false means adjust control positions
     * @return The final width and height of the arranged layout, including the Box container's padding but not its margin (used when the container supports a scroll bar)
     */
    virtual UiSize64 ArrangeChildren(const std::vector<Control*>& items, UiRect rc, bool bEstimateOnly = false) override;

    /** Estimate the container layout size from the sizes of the internal child controls (used to evaluate controls whose width or height is "auto"; stretch-type child controls are not counted in the size estimation)
     * @param [in] items List of child controls
     * @param [in] szAvailable The container's available width and height, including the padding allocated to the container but not its margin
     * @return The final layout size (width and height) after arrangement, including the Box container's padding but not its margin;
     */
    virtual UiSize64 EstimateLayoutSize(const std::vector<Control*>& items, UiSize szAvailable) override;

private:
    /** Adjust the position information of all internal controls
     * @param [in] items List of controls
     * @param [in] rc The current container position information, including padding but not margin
     * @param [in] bEstimateOnly true means only estimate without adjusting control positions; false means adjust control positions
     * @param [in] bEstimateLayoutSize Whether it is currently used to estimate the control size ("auto" attribute); in this case, the handling logic for stretch controls differs
     * @return The final width and height of the arranged box, including the Owner Box's padding, excluding the margin
     */
    UiSize64 ArrangeChildInternal(const std::vector<Control*>& items, UiRect rc, bool bEstimateOnly, bool bEstimateLayoutSize) const;
};

} // namespace ui

#endif // UI_LAYOUT_VFLOWLAYOUT_H_
