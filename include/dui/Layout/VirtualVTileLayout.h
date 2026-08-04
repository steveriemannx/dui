#ifndef UI_LAYOUT_VIRTUAL_VTILELAYOUT_H_
#define UI_LAYOUT_VIRTUAL_VTILELAYOUT_H_

#include "dui/Layout/VTileLayout.h"
#include "dui/Layout/VirtualLayout.h"

namespace ui 
{
/** Virtual tile layout implemented with a virtual list (vertical layout)
 *  Horizontal alignment: centered by default
 *  Vertical alignment: top-aligned, controls arranged in order
 *  In the tile layout, the alignment specified by the child controls themselves does not take effect
 */
class VirtualListBox;
class DUI_API VirtualVTileLayout : public VTileLayout, public VirtualLayout
{
    typedef VTileLayout BaseClass;
public:
    VirtualVTileLayout();

    /** Layout type
    */
    virtual LayoutType GetLayoutType() const override { return LayoutType::VirtualVTileLayout; }

    /** Adjust the position and size of all child controls according to the layout strategy
     * @param [in] items child control list
     * @param [in] rc current container position and size info, including the padding, but not the margin
     * @param [in] bEstimateOnly true means only estimate without adjusting control positions, false means adjust control positions
     * @return returns the width and height info of the final layout after arrangement, including the Box container padding, but not the Box container's own margin (use this return value when the container supports a scrollbar)
     */
    virtual UiSize64 ArrangeChildren(const std::vector<Control*>& items, UiRect rc, bool bEstimateOnly = false) override;

    /** Estimate the container layout size based on the size of the internal child controls (used to evaluate the size of controls with "auto" width or height; stretch-type child controls are not counted in the size estimation)
     * @param [in] items child control list
     * @param [in] szAvailable available width and height of the container, including the padding allocated to the container, but not the margin allocated to the container
     * @return returns the size info (width and height) of the final layout after arrangement, including the Box container's own padding, but not the Box container's own margin;
     */
    virtual UiSize64 EstimateLayoutSize(const std::vector<Control*>& items, UiSize szAvailable) override;

public:
    /** Lazily load and display data
    * @param [in] rc current container size info; when called externally, the padding must be deflated first
    */
    virtual void LazyArrangeChild(UiRect rc) const override;

    /** Get the maximum number of real data items to display (i.e., real data items with a corresponding Control object)
    * @param [in] rc current container size info; when called externally, the padding must be deflated first
    */
    virtual size_t AjustMaxItem(UiRect rc) const override;

    /** Get the index of the element preceding the first element in the visible range
    * @param [in] rc rect of the current display area, without padding
    * @return returns the element index
    */
    virtual size_t GetTopElementIndex(UiRect rc) const override;

    /** Determine whether an element is within the visible range
    * @param[in] iIndex element index
    * @param [in] rc rect of the current display area, without padding
    * @return returns true if visible, otherwise invisible
    */
    virtual bool IsElementDisplay(UiRect rc, size_t iIndex) const override;

    /** Determine whether the layout needs to be rearranged
    */
    virtual bool NeedReArrange() const override;

    /** Get the data element indices of all currently visible controls
    * @param [in] rc rect of the current display area, without padding
    * @param[out] collection index list, range: [0, GetElementCount())
    */
    virtual void GetDisplayElements(UiRect rc, std::vector<size_t>& collection) const override;

    /** Make the control within the visible range
    * @param [in] rc rect of the current display area, without padding
    * @param[in] iIndex element index, range: [0, GetElementCount())
    * @param[in] bToTop whether to scroll to the top
    */
    virtual void EnsureVisible(UiRect rc, size_t iIndex, bool bToTop) const override;

private:
    /** Get the height of data items
    * @param [in] nCount number of data items; if it is Box::InvalidIndex, get the total height of all data items
    * @param [in] rc current container size info; when called externally, the padding must be deflated first
    * @return returns the total height of nCount data items
    */
    int64_t GetElementsHeight(const UiRect& rc, size_t nCount) const;

    /** Calculate the number of columns
    *@param [in] rcWidth available area width
    *@return the calculated number of columns, greater than or equal to 1
    */
    int32_t CalcTileColumns(int32_t rcWidth) const;

private:
    /** Get the associated Box interface
    */
    VirtualListBox* GetOwnerBox() const;
};
} // namespace ui

#endif // UI_LAYOUT_VIRTUAL_VTILELAYOUT_H_
