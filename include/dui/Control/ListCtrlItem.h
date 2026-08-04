#ifndef UI_CONTROL_LIST_CTRL_ITEM_H_
#define UI_CONTROL_LIST_CTRL_ITEM_H_

#include "dui/Box/ListBox.h"
#include "dui/Control/Label.h"
#include "dui/Core/ImageList.h"
#include "dui/Control/ListCtrlDefs.h"

namespace ui
{
/** The ListCtrl list data item UI control (row)
*    Basic structure: <ListCtrlItem> <ListCtrlSubItem/> ... <ListCtrlSubItem/>  </ListCtrlItem>
*    Additional notes:
*       1. ListCtrlItem represents the UI control of a row in the table
*         (1) The inheritance hierarchy of ListCtrlItem's parent class is: ListCtrlItemTemplate<HBox>: public ListBoxItemTemplate<HBox>: pubic OptionTemplate<HBox>)
*         (2) The child controls in ListCtrlItem are of type ListCtrlSubItem, each child control represents a cell in a column
*         (3) When a CheckBox needs to be shown, ListCtrlItem sets its own attributes to show the CheckBox, because it is a subclass of OptionTemplate<HBox> and has CheckBox attributes
*       2. ListCtrlSubItem represents the UI control of a cell at a specific row and column in the table
*         (1) The inheritance relationship of ListCtrlSubItem's parent class is: ListCtrlSubItem public : ListCtrlLabel: public CheckBoxTemplate<HBox>
*         (2) ListCtrlSubItem inherits from CheckBoxTemplate<HBox> to display text itself
*         (3) When a CheckBox needs to be shown, ListCtrlSubItem sets its own attributes to show the CheckBox, because it is a subclass of CheckBoxTemplate<HBox> and has CheckBox attributes
*         (4) When showing icons, the ImageList resources of ListCtrl are used, implemented by custom drawing
*/
class ListCtrl;
class ListCtrlSubItem;
class DUI_API ListCtrlItem : public ListCtrlItemBaseH
{
    typedef ListCtrlItemBaseH BaseClass;
    friend class ListCtrlReportView;//Expose part of the protected interfaces
public:
    explicit ListCtrlItem(Window* pWindow);
    virtual ~ListCtrlItem() override;

    /** Get the control type
    */
    virtual DString GetType() const override;

    /** Set attributes
    */
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;

    /** DPI changed, update the control size and layout
    * @param [in] nOldDpiScale The old DPI scale percentage
    * @param [in] nNewDpiScale The new DPI scale percentage, consistent with the value of Dpi().GetScale()
    */
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;

    /** Event handler function
    */
    virtual void HandleEvent(const EventArgs& msg) override;

    /** Determine whether the control type is selectable
     * @return Returns false by default
     */
    virtual bool IsSelectableType() const override;

    /** Set whether it can be selected (pinned data cannot be selected, other data can be selected)
    */
    void SetSelectableType(bool bSelectable);

public:
    /** Set the associated ListCtrl interface
    */
    void SetListCtrl(ListCtrl* pListCtrl);

    /** Get the associated ListCtrl interface
    */
    ListCtrl* GetListCtrl() const;

    /** Get the index of the associated data item, which represents the data of which row it is associated with
    * @return Returns the index of the data item, valid range: [0, ListCtrl::GetDataItemCount())
    */
    size_t GetDataItemIndex() const;

    /** Get the number of child controls
    */
    size_t GetSubItemCount() const;

    /** Get the child control at columnIndex
    * @param [in] columnIndex The column index: [0, GetSubItemCount())
    */
    ListCtrlSubItem* GetSubItem(size_t columnIndex) const;

    /** Get the child control at the mouse position
    * @param [in] ptMouse The mouse position, a point in screen coordinates
    */
    ListCtrlSubItem* GetSubItem(const UiPoint& ptMouse) const;

    /** Get the column index of the child control at the mouse position (which column)
    * @param [in] ptMouse The mouse position, a point in screen coordinates
    */
    size_t GetSubItemIndex(const UiPoint& ptMouse) const;

    /** Get the column index of the child control (which column)
    * @param [in] pSubItem The interface of the child control
    */
    size_t GetSubItemIndex(ListCtrlSubItem* pSubItem) const;

    /** Set whether to show the CheckBox at the beginning of the row
    * @param [in] bShow true means showing the CheckBox at the beginning of the row, false means not showing it
    */
    bool SetShowCheckBox(bool bShow);

    /** Determine whether the CheckBox is shown at the beginning of the row
    */
    bool IsShowCheckBox() const;

    /** Determine whether the CheckBox at the beginning of the current row is checked
    */
    bool IsCheckBoxChecked() const;

    /** Set the associated icon Id; if it is -1, the icon is not shown, and the icon is displayed in front of the text
    */
    void SetImageId(int32_t imageId);

    /** Get the associated icon Id
    */
    int32_t GetImageId() const;

    /** Set the spacing between icons (in pixels)
    */
    void SetIconSpacing(int32_t nIconSpacing, bool bNeedDpiScale);

    /** Get the spacing between icons (in pixels)
    */
    int32_t GetIconSpacing() const;

protected:
    /** Get the width occupied by the associated icon/CheckBox etc. (left side)
    */
    int32_t GetItemPaddingLeft();

    /** Load the icon resource
    */
    ImagePtr LoadItemImage() const;

    /** Vertically align the target rect
    */
    void VAlignRect(UiRect& rc, uint32_t textStyle, int32_t nImageHeight);

    /** Get the image width of the CheckBox
    */
    int32_t GetCheckBoxImageWidth();

protected:
    
    /** Mouse left button up event
    */
    virtual bool ButtonUp(const EventArgs& msg) override;

    /** Whether the check mode is supported (currently used by TreeView/ListCtrl)
        The check mode means:
        (1) Only when clicking on the CheckBox image is the check box image in the selected state (in non-check mode, clicking anywhere within the control rect selects it)
        (2) The checked state and the selected state are separated, two different states
    */
    virtual bool SupportCheckMode() const override;

    /** Draw function
    */
    virtual void Paint(IRender* pRender, const UiRect& rcPaint) override;

private:
    /** Whether it can be selected (affects switching the selection with arrow keys)
    */
    bool m_bSelectable;

    /** The associated ListCtrl interface
    */
    ListCtrl* m_pListCtrl;

    /** The associated icon Id; if it is -1, the icon is not shown, and the icon is displayed in front of the text
    */
    int32_t m_imageId;

    /** The spacing between icons
    */
    int32_t m_nIconSpacing;
};

}//namespace ui

#endif //UI_CONTROL_LIST_CTRL_ITEM_H_
