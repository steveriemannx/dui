#ifndef UI_LAYOUT_VIRTUAL_HTILELAYOUT_H_
#define UI_LAYOUT_VIRTUAL_HTILELAYOUT_H_

#include "dui/Layout/HTileLayout.h"
#include "dui/Layout/VirtualLayout.h"

namespace ui 
{
/** Tile layout implemented with a virtual list (horizontal layout)
 *  Horizontal alignment: left-aligned, controls arranged in sequence
 *  Vertical alignment: center-aligned by default
 *  In a tile layout, the alignment specified by the child controls themselves does not take effect
 */
class VirtualListBox;
class DUI_API VirtualHTileLayout : public HTileLayout, public VirtualLayout
{
    typedef HTileLayout BaseClass;
public:
    VirtualHTileLayout();

    /** Layout type
    */
    virtual LayoutType GetLayoutType() const override { return LayoutType::VirtualHTileLayout; }

    /** Arrange the positions and sizes of all child controls according to the layout strategy
     * @param [in] items list of child controls
     * @param [in] rc position and size info of the current container, including padding but not margins
     * @param [in] bEstimateOnly true means only estimate without adjusting control positions; false means adjust control positions
     * @return the width and height of the final layout after arrangement, including the padding of the Box container, but not the margins of the Box container itself (used when the container supports a scroll bar)
     */
    virtual UiSize64 ArrangeChildren(const std::vector<Control*>& items, UiRect rc, bool bEstimateOnly = false) override;

    /** Estimate the container layout size based on the sizes of the internal child controls (used to evaluate the size of controls whose width or height is "auto"; stretch-type child controls are not included in the size estimation)
     * @param [in] items list of child controls
     * @param [in] szAvailable available width and height of the container, including the padding allocated to the container, but not the margins allocated to the container
     * @return the size info (width and height) of the final layout after arrangement, including the padding of the Box container itself, but not the margins of the Box container itself;
     */
    virtual UiSize64 EstimateLayoutSize(const std::vector<Control*>& items, UiSize szAvailable) override;

public:
    /** Lazy-load the data to be displayed
    * @param [in] rc current container size info; the padding must be removed before external calls
    */
    virtual void LazyArrangeChild(UiRect rc) const override;

    /** Get the maximum number of real data items to display (i.e., real data items corresponding to Control objects)
    * @param [in] rc current container size info; the padding must be removed before external calls
    */
    virtual size_t AjustMaxItem(UiRect rc) const override;

    /** Get the index of the element preceding the first element in the visible range
    * @param [in] rc rect of the current display area, not including padding
    * @return the index of the element
    */
    virtual size_t GetTopElementIndex(UiRect rc) const override;

    /** Check whether an element is within the visible range
    * @param[in] iIndex element index
    * @param [in] rc rect of the current display area, not including padding
    * @return true if visible, otherwise not visible
    */
    virtual bool IsElementDisplay(UiRect rc, size_t iIndex) const override;

    /** Check whether the layout needs to be rearranged
    */
    virtual bool NeedReArrange() const override;

    /** Get the data element indices of all currently visible controls
    * @param [in] rc rect of the current display area, not including padding
    * @param[out] collection index list, range: [0, GetElementCount())
    */
    virtual void GetDisplayElements(UiRect rc, std::vector<size_t>& collection) const override;

    /** Ensure the control is within the visible range
    * @param [in] rc rect of the current display area, not including padding
    * @param[in] iIndex element index, range: [0, GetElementCount())
    * @param[in] bToTop whether to place it at the top
    */
    virtual void EnsureVisible(UiRect rc, size_t iIndex, bool bToTop) const override;

private:
    /** Get the width of data items
    * @param [in] nCount number of data items; if Box::InvalidIndex, get the total height of all data items
    * @param [in] rc current container size info; the padding must be removed before external calls
    * @return the total width of nCount data items
    */
    int64_t GetElementsWidth(const UiRect& rc, size_t nCount) const;

    /** Calculate the number of rows
    *@param [in] rcHeight height of the available area
    *@return the calculated number of rows, greater than or equal to 1
    */
    int32_t CalcTileRows(int32_t rcHeight) const;

private:
    /** Get the associated Box interface
    */
    VirtualListBox* GetOwnerBox() const;
};
} // namespace ui

#endif // UI_LAYOUT_VIRTUAL_HTILELAYOUT_H_
