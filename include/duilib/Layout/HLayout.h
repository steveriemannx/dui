#ifndef UI_LAYOUT_HLAYOUT_H_
#define UI_LAYOUT_HLAYOUT_H_

#include "duilib/Layout/Layout.h"

namespace ui 
{

/** Horizontal layout: child controls are top-aligned vertically and arranged in sequence horizontally
 *  Horizontal alignment: left-aligned by default; the alignment specified by a child control itself does not take effect
 *  Vertical alignment: none; child controls are arranged according to the alignment specified by each child control
 */
class DUILIB_API HLayout : public Layout
{
    typedef Layout BaseClass;
public:
    HLayout();

    /** Layout type
    */
    virtual LayoutType GetLayoutType() const override { return LayoutType::HLayout; }

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
};

} // namespace ui

#endif // UI_LAYOUT_HLAYOUT_H_
