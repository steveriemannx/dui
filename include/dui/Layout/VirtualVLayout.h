#ifndef UI_LAYOUT_VIRTUAL_VLAYOUT_H_
#define UI_LAYOUT_VIRTUAL_VLAYOUT_H_

#include "dui/Layout/VLayout.h"
#include "dui/Layout/VirtualLayout.h"

namespace ui 
{
/** Virtual-table-based tile layout (a vertical layout with only 1 column)
 *  Horizontal alignment: center-aligned by default
 *  Vertical alignment: top-aligned, arranged sequentially by control
 *  In this layout, the alignment specified by child controls themselves is ignored
 */
class VirtualListBox;
class DUI_API VirtualVLayout : public VLayout, public VirtualLayout
{
    typedef VLayout BaseClass;
public:
    VirtualVLayout();

    /** Layout type
    */
    virtual LayoutType GetLayoutType() const override { return LayoutType::VirtualVLayout; }

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

    /** Set layout attributes
     * @param [in] strName The attribute name to set
     * @param [in] strValue The attribute value to set
     * @param [in] dpiManager The DPI manager interface
     * @return true if set successfully, false if the attribute does not exist
     */
    virtual bool SetAttribute(const DString& strName, 
                              const DString& strValue,
                              const DpiManager& dpiManager) override;

    /** DPI changed, update the control size and layout
    * @param [in] nOldDpiScale The old DPI scale percentage
    * @param [in] dpiManager The DPI scale manager
    */
    virtual void ChangeDpiScale(const DpiManager& dpiManager, uint32_t nOldDpiScale) override;

public:
    /** Lazily load the display data
    * @param [in] rc The current container size info; when called externally, the padding must be subtracted first
    */
    virtual void LazyArrangeChild(UiRect rc) const override;

    /** Get the maximum number of real data items to display (i.e., real data items with a corresponding Control object)
    * @param [in] rc The current container size info; when called externally, the padding must be subtracted first
    */
    virtual size_t AjustMaxItem(UiRect rc) const override;

    /** Get the index of the element before the first element in the visible range
    * @param [in] rc The rect of the current display area, not including padding
    * @return The index of the element
    */
    virtual size_t GetTopElementIndex(UiRect rc) const override;

    /** Determine whether an element is within the visible range
    * @param[in] iIndex The element index
    * @param [in] rc The rect of the current display area, not including padding
    * @return Returns true if visible, otherwise not visible
    */
    virtual bool IsElementDisplay(UiRect rc, size_t iIndex) const override;

    /** Determine whether the layout needs to be rearranged
    */
    virtual bool NeedReArrange() const override;

    /** Get the data element indices of all currently visible controls
    * @param [in] rc The rect of the current display area, not including padding
    * @param[out] collection The index list, in the range: [0, GetElementCount())
    */
    virtual void GetDisplayElements(UiRect rc, std::vector<size_t>& collection) const override;

    /** Make the control within the visible range
    * @param [in] rc The rect of the current display area, not including padding
    * @param[in] iIndex The element index, in the range: [0, GetElementCount())
    * @param[in] bToTop Whether to place it at the top
    */
    virtual void EnsureVisible(UiRect rc, size_t iIndex, bool bToTop) const override;

public:
    /** Set the child item size
     * @param [in] szItem The child item size data; the width and height include the control's outer margins and padding
     * @param [in] bArrange Whether to rearrange when the size changes
     */
    void SetItemSize(UiSize szItem, bool bArrange = true);

    /** Get the child item size; the width and height include the control's outer margins and padding
     */
    const UiSize& GetItemSize() const;

    /** Set whether to auto-calculate the width of child items (only valid when a fixed column is set)
    */
    void SetAutoCalcItemWidth(bool bAutoCalcItemWidth);

    /** Whether to auto-calculate the width of child items
    */
    bool IsAutoCalcItemWidth() const;

private:
    /** Get the height of data items
    * @param [in] nCount The number of data items; if Box::InvalidIndex, the total height of all data items is returned
    * @param [in] rc The current container size info; when called externally, the padding must be subtracted first
    * @return The total height of nCount data items
    */
    int64_t GetElementsHeight(UiRect rc, size_t nCount) const;

private:
    /** Get the associated Box interface
    */
    VirtualListBox* GetOwnerBox() const;

private:
    // The child item size; the width and height include the control's outer margins and padding
    UiSize m_szItem;

    // Whether to auto-calculate the child item width (auto-adapts to the overall width of the parent control; only valid when a fixed column is set)
    bool m_bAutoCalcItemWidth;
};
} // namespace ui

#endif // UI_LAYOUT_VIRTUAL_VLAYOUT_H_
