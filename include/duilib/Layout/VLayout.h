#ifndef UI_LAYOUT_VLAYOUT_H_
#define UI_LAYOUT_VLAYOUT_H_

#include "duilib/Layout/Layout.h"

namespace ui 
{

/** Vertical layout: child controls are aligned to the left horizontally and arranged sequentially in the vertical direction
 *  Horizontal alignment: none; during layout, child controls are arranged according to their own specified alignment
 *  Vertical alignment: top-aligned by default; the vertical alignment specified by child controls themselves is ignored
 */
class DUILIB_API VLayout : public Layout
{
    typedef Layout BaseClass;
public:
    VLayout();

    /** Layout type
    */
    virtual LayoutType GetLayoutType() const override { return LayoutType::VLayout; }
    
    /** Adjust the positions and sizes of all child controls according to the layout policy
     * @param [in] items The list of child controls
     * @param [in] rc The current position and size info of the container, including padding but not outer margins
     * @param [in] bEstimateOnly true means only estimate without adjusting control positions; false means adjust control positions
     * @return The final layout width and height after arrangement, including the Box container padding but not the Box container's own outer margins (use this return value when the container supports a scroll bar)
     */
    virtual UiSize64 ArrangeChildren(const std::vector<Control*>& items, UiRect rc, bool bEstimateOnly = false) override;

    /** Estimate the container layout size based on the sizes of its child controls (used to evaluate the size of controls whose width or height is "auto"; stretch-type child controls are not included in the size estimation)
     * @param [in] items The list of child controls
     * @param [in] szAvailable The available width and height of the container, including the padding assigned to this container but not the outer margins assigned to the container
     * @return The final layout size info (width and height) after arrangement, including the Box container's own padding but not the Box container's own outer margins;
     */
    virtual UiSize64 EstimateLayoutSize(const std::vector<Control*>& items, UiSize szAvailable) override;
};

} // namespace ui

#endif // UI_LAYOUT_VLAYOUT_H_
