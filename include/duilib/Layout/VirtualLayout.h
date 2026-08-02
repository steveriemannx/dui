#ifndef UI_LAYOUT_VIRTUAL_LAYOUT_H_
#define UI_LAYOUT_VIRTUAL_LAYOUT_H_

#include "duilib/Core/UiTypes.h"

namespace ui 
{
/** Layout interface implemented with virtual tables
*/
class DUILIB_API VirtualLayout
{
public:
    VirtualLayout() = default;
    VirtualLayout(const VirtualLayout& r) = delete;
    VirtualLayout& operator=(const VirtualLayout& r) = delete;
    virtual ~VirtualLayout() = default;

public:
    /** Lazily load the display data
    * @param [in] rc The current container size info; when called externally, the padding must be subtracted first
    */
    virtual void LazyArrangeChild(UiRect rc) const = 0;

    /** Get the maximum number of real data items to display (i.e., real data items with a corresponding Control object)
    * @param [in] rc The current container size info; when called externally, the padding must be subtracted first
    */
    virtual size_t AjustMaxItem(UiRect rc) const = 0;

    /** Get the index of the element before the first element in the visible range
    * @param [in] rc The rect of the current display area, not including padding
    * @return The index of the element
    */
    virtual size_t GetTopElementIndex(UiRect rc) const = 0 ;

    /** Determine whether an element is within the visible range
    * @param[in] iIndex The element index
    * @param [in] rc The rect of the current display area, not including padding
    * @return Returns true if visible, otherwise not visible
    */
    virtual bool IsElementDisplay(UiRect rc, size_t iIndex) const = 0;

    /** Determine whether the layout needs to be rearranged
    */
    virtual bool NeedReArrange() const = 0;

    /** Get the data element indices of all currently visible controls
    * @param [in] rc The rect of the current display area, not including padding
    * @param[out] collection The index list, in the range: [0, GetElementCount())
    */
    virtual void GetDisplayElements(UiRect rc, std::vector<size_t>& collection) const = 0;

    /** Make the control within the visible range
    * @param [in] rc The rect of the current display area, not including padding
    * @param[in] iIndex The element index, in the range: [0, GetElementCount())
    * @param[in] bToTop Whether to place it at the top
    */
    virtual void EnsureVisible(UiRect rc, size_t iIndex, bool bToTop) const = 0;
};

} // namespace ui

#endif // UI_LAYOUT_VIRTUAL_LAYOUT_H_
