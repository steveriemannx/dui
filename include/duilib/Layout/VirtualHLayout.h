#ifndef UI_LAYOUT_VIRTUAL_HLAYOUT_H_
#define UI_LAYOUT_VIRTUAL_HLAYOUT_H_

#include "duilib/Layout/HLayout.h"
#include "duilib/Layout/VirtualLayout.h"

namespace ui 
{
/** Tile layout implemented with a virtual list (a vertical layout with only 1 row)
 *  Horizontal alignment: left-aligned, controls arranged in sequence
 *  Vertical alignment: centered by default
 *  In this layout, the alignment specified by a child control itself does not take effect
 */
class VirtualListBox;
class DUILIB_API VirtualHLayout : public HLayout, public VirtualLayout
{
    typedef HLayout BaseClass;
public:
    VirtualHLayout();

    /** Layout type
    */
    virtual LayoutType GetLayoutType() const override { return LayoutType::VirtualHLayout; }

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

    /** Set layout attributes
     * @param [in] strName The attribute name to set
     * @param [in] strValue The attribute value to set
     * @param [in] dpiManager DPI management interface
     * @return true on success, false if the attribute does not exist
     */
    virtual bool SetAttribute(const DString& strName, 
                              const DString& strValue,
                              const DpiManager& dpiManager) override;

    /** When the DPI changes, update the control sizes and layout
    * @param [in] nOldDpiScale The old DPI scale percentage
    * @param [in] dpiManager The DPI scale manager
    */
    virtual void ChangeDpiScale(const DpiManager& dpiManager, uint32_t nOldDpiScale) override;

public:
    /** Lazily load and display data
    * @param [in] rc The current container size information; the padding must be removed before calling from outside
    */
    virtual void LazyArrangeChild(UiRect rc) const override;

    /** Get the maximum number of real data items to display (i.e., real data items that have a corresponding Control object)
    * @param [in] rc The current container size information; the padding must be removed before calling from outside
    */
    virtual size_t AjustMaxItem(UiRect rc) const override;

    /** Get the index of the element before the first element in the visible range
    * @param [in] rc The rect of the current display area, excluding padding
    * @return The index of the element
    */
    virtual size_t GetTopElementIndex(UiRect rc) const override;

    /** Determine whether an element is within the visible range
    * @param[in] iIndex Element index
    * @param [in] rc The rect of the current display area, excluding padding
    * @return true if visible, false otherwise
    */
    virtual bool IsElementDisplay(UiRect rc, size_t iIndex) const override;

    /** Determine whether the layout needs to be rearranged
    */
    virtual bool NeedReArrange() const override;

    /** Get the data element indices of all currently visible controls
    * @param [in] rc The rect of the current display area, excluding padding
    * @param[out] collection The index list, in the range: [0, GetElementCount())
    */
    virtual void GetDisplayElements(UiRect rc, std::vector<size_t>& collection) const override;

    /** Bring the control into the visible range
    * @param [in] rc The rect of the current display area, excluding padding
    * @param[in] iIndex Element index, in the range: [0, GetElementCount())
    * @param[in] bToTop Whether to position it at the top
    */
    virtual void EnsureVisible(UiRect rc, size_t iIndex, bool bToTop) const override;

public:
    /** Set the child item size
     * @param [in] szItem The child item size data; the width and height include the control's outer and inner margins
     * @param [in] bArrange Whether to re-arrange when it changes
     */
    void SetItemSize(UiSize szItem, bool bArrange = true);

    /** Get the child item size; the width and height include the control's outer and inner margins
     */
    const UiSize& GetItemSize() const;

    /** Set whether to auto-calculate the height of child items (effective only when fixed rows are set)
    */
    void SetAutoCalcItemHeight(bool bAutoCalcItemHeight);

    /** Whether to auto-calculate the height of child items
    */
    bool IsAutoCalcItemHeight() const;

private:
    /** Get the width of a data item
    * @param [in] nCount Number of data items; if Box::InvalidIndex, get the total height of all data items
    * @param [in] rc The current container size information; the padding must be removed before calling from outside
    * @return The total height of nCount data items
    */
    int64_t GetElementsWidth(UiRect rc, size_t nCount) const;

private:
    /** Get the associated Box interface
    */
    VirtualListBox* GetOwnerBox() const;

private:
    // Child item size; the width and height include the control's outer and inner margins
    UiSize m_szItem;

    // Whether to auto-calculate the height of child items (adapts to the overall height of the parent control; effective only when fixed rows are set)
    bool m_bAutoCalcItemHeight;
};
} // namespace ui

#endif // UI_LAYOUT_VIRTUAL_HLAYOUT_H_
