#ifndef UI_BOX_MENU_LIST_BOX_H_
#define UI_BOX_MENU_LIST_BOX_H_

#include "duilib/Box/ListBox.h"

namespace ui
{

/** ListBox control used in menus
*/
class DUILIB_API MenuListBox : public VListBox
{
    typedef VListBox BaseClass;
public:
    explicit MenuListBox(Window* pWindow) :
        VListBox(pWindow)
    {
    }

    virtual DString GetType() const override { return DUI_CTR_MENU_LISTBOX; }

    /** Calculate how many sub-items are displayed in this page
    * @param [in] bIsHorizontal Whether the current layout is a horizontal layout
    * @param [out] nColumns Returns the number of columns
    * @param [out] nRows Returns the number of rows
    * @return Returns the number of records displayed in the visible area
    */
    virtual size_t GetDisplayItemCount(bool bIsHorizontal, size_t& nColumns, size_t& nRows) const override
    {
        size_t nCount = 0;
        nRows = 0;
        nColumns = 1;
        if (bIsHorizontal) {
            //There is currently no such case: the vertical layout is already fixed
            nCount = BaseClass::GetDisplayItemCount(bIsHorizontal, nColumns, nRows);
            ASSERT(0);
        }
        else {
            const size_t nItemCount = GetItemCount();
            for (size_t nItemIndex = 0; nItemIndex < nItemCount; ++nItemIndex) {
                Control* pControl = GetItemAt(nItemIndex);
                if ((pControl == nullptr) || !pControl->IsVisible() || pControl->IsFloat()) {
                    continue;
                }
                ++nCount;
            }
        }
        nRows = nCount;
        return nCount;
    }

    /** Respond to the KeyDown message
    * @return Returns true to indicate successful handling, returns false to indicate this message was not handled
    */
    virtual bool OnListBoxKeyDown(const EventArgs& msg) override
    {
        ASSERT(msg.eventType == kEventKeyDown);
        bool bHandled = false;
        bool bArrowKeyDown = (msg.eventType == kEventKeyDown) && ((msg.vkCode == kVK_UP) || (msg.vkCode == kVK_DOWN));
        if (!bArrowKeyDown || (GetItemCount() == 0) || IsMultiSelect() || IsHorizontalScrollBar()) {
            return BaseClass::OnListBoxKeyDown(msg);
        }
        //Support for switching menu items with the up/down keys
        bHandled = true;
        switch (msg.vkCode) {
        case kVK_UP:
            //Up one row
            {
                size_t nColumns = 0;
                size_t nRows = 0;
                GetDisplayItemCount(false, nColumns, nRows);
                size_t nCurSel = GetCurSel();
                if (nCurSel >= GetItemCount()) {
                    //When nothing is selected, select the last item
                    SelectItem(GetItemCount() - 1);
                }
                else if (nCurSel == 0) {
                    //When the first item is selected, select the last item for circular selection
                    SelectItem(GetItemCount() - 1);
                }
                else if ((nCurSel < GetItemCount()) && IsSelectableItem(nCurSel) && (GetItemCountBefore(nCurSel) >= nColumns)) {
                    //Can scroll up one row
                    SelectItemCountN(true, true, false, nColumns);
                }
            }
            break;
        case kVK_DOWN:
            //Down one row
            {
                size_t nColumns = 0;
                size_t nRows = 0;
                GetDisplayItemCount(false, nColumns, nRows);
                size_t nCurSel = GetCurSel();
                if (nCurSel >= GetItemCount()) {
                    //When nothing is selected, select the first item
                    SelectItem(0);
                }
                else if (nCurSel == (GetItemCount() - 1)) {
                    //When the last item is selected, select the first item for circular selection
                    SelectItem(0);
                }
                else if ((nCurSel < GetItemCount()) && IsSelectableItem(nCurSel) && (GetItemCountAfter(nCurSel) >= nColumns)) {
                    SelectItemCountN(true, true, true, nColumns);
                }
            }
            break;
        default:
            break;
        }
        return bHandled;
    }
};

}
#endif // UI_BOX_MENU_LIST_BOX_H_
