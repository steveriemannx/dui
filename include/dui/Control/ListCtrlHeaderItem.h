#ifndef UI_CONTROL_LIST_CTRL_HEADER_ITEM_H_
#define UI_CONTROL_LIST_CTRL_HEADER_ITEM_H_

#include "dui/Control/Button.h"
#include "dui/Control/CheckBox.h"
#include "dui/Control/Split.h"
#include "dui/Image/Image.h"
#include "dui/Core/ControlDragable.h"

namespace ui
{

/** The display item of the ListCtrl header control
*/
class ListCtrlHeader;
class DUI_API ListCtrlHeaderItem:
    public ControlDragableT<CheckBoxHBox>
{
    typedef ControlDragableT<CheckBoxHBox> BaseClass;
public:
    explicit ListCtrlHeaderItem(Window* pWindow);
    virtual ~ListCtrlHeaderItem() override;

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

    /** Draw the text
    */
    virtual void PaintText(IRender* pRender) override;

    /** Button click event
    */
    virtual void Activate(const EventArgs* pMsg) override;

public:
    /** Sort mode
    */
    enum class SortMode: int8_t
    {
        kNone, //sorting not supported
        kUp,   //ascending order
        kDown  //descending order
    };

    /** Set the sort mode
    * @param [in] sortMode The sort mode
    * @param [in] bTriggerEvent true means triggering the sort change event to re-sort the data; otherwise the event is not triggered
    */
    void SetSortMode(SortMode sortMode, bool bTriggerEvent = false);

    /** Get the sort mode
    */
    SortMode GetSortMode() const;

    /** Set the sort icon: descending
    */
    void SetSortedDownImage(const DString& sImageString);

    /** Set the sort icon: ascending
    */
    void SetSortedUpImage(const DString& sImageString);

    /** Set whether to show the sort icon (among all HeaderItems, only the sorted column shows it, other columns do not)
    * @param [in] bShowSortImage Whether to show the sort icon
    */
    void SetShowSortImage(bool bShowSortImage);

    /** Get whether the sort icon is shown
    */
    bool IsShowSortImage() const;

    /** Get the unique ID
    */
    size_t GetColumnId() const;

    /** Set the associated Split control interface
    */
    void SetSplitBox(SplitBox* pSplitBox);

    /** Get the associated Split control interface
    */
    SplitBox* GetSplitBox() const;

    /** Set whether the column width can be adjusted
    */
    void SetColumnResizeable(bool bResizeable);

    /** Get whether the column width can be adjusted
    */
    bool IsColumnResizeable() const;

    /** Set the column width
    * @param [in] nWidth The column width value
    * @param [in] bNeedDpiScale Whether the column width value needs DPI scaling
    */
    void SetColumnWidth(int32_t nWidth, bool bNeedDpiScale);

    /** Get the column width value
    */
    int32_t GetColumnWidth() const;

    /** Set the spacing between text and icon (in pixels)
    */
    void SetIconSpacing(int32_t nIconSpacing, bool bNeedDpiScale);

    /** Get the spacing between text and icon (in pixels)
    */
    int32_t GetIconSpacing() const;

    /** Set the icon to be displayed above the text, centered
    */
    void SetShowIconAtTop(bool bShowIconAtTop);

    /** Get whether the icon is displayed above the text
    */
    bool IsShowIconAtTop() const;

    /** Set the horizontal alignment of the text
    */
    void SetTextHorAlign(HorAlignType alignType);

    /** Get the horizontal alignment of the text
    */
    HorAlignType GetTextHorAlign() const;

    /** Set the associated icon Id; if it is -1, the icon is not shown, and the icon is displayed in front of the text
    */
    void SetImageId(int32_t imageId);

    /** Get the associated icon Id
    */
    int32_t GetImageId() const;

    /** Whether this column supports dragging the list header to change the column order
    */
    virtual bool IsEnableDragOrder() const override;

public:
    /** Set whether to show the CheckBox
    * @param [in] bShow true means show, false means not show
    */
    bool SetShowCheckBox(bool bShow);

    /** Determine whether the current CheckBox is in the shown state
    @return Returns true if the CheckBox exists and is visible; returns false if there is no CheckBox
    */
    bool IsShowCheckBox() const;

    /** Set the check state of the CheckBox
    * @param [in] bChecked true means checked, false means unchecked
    * @param [in] If both bChecked and bPartChecked are true, it means partially selected
    */
    bool SetCheckBoxCheck(bool bChecked, bool bPartChecked);

    /** Get the check state of the CheckBox
    * @param [out] bChecked true means checked, false means unchecked
    * @param [out] If both bSelected and bPartSelect are true, it means partially selected
    */
    bool GetCheckBoxCheck(bool& bChecked, bool& bPartChecked) const;

    /** Show/hide this column
    */
    void SetColumnVisible(bool bColumnVisible);

    /** Determine whether this column is shown
    * @return Returns true if this column is shown, otherwise it means this column is hidden
    */
    bool IsColumnVisible() const;

    /** Set the associated Header interface
    */
    void SetHeaderCtrl(ListCtrlHeader* pHeaderCtrl);

    /** Get the associated Header interface
    */
    ListCtrlHeader* GetHeaderCtrl() const;

private:
    /** Synchronize the column width with the UI control width
    */
    void CheckColumnWidth();

    /** Vertically align the target rect
    */
    void VAlignRect(UiRect& rc, uint32_t textStyle, int32_t nImageHeight);

    /** Get the image width of the CheckBox
    */
    int32_t GetCheckBoxImageWidth();

protected:
    /** @name Member functions related to drag reordering
    * @{ */
    /** Adjust the position of each control according to the mouse position (part of the drag operation)
    * @param [in] pt The current mouse position
    * @param [in] ptMouseDown The position where the mouse was pressed
    * @param [in] rcItemList The list of child controls
    */
    virtual void AdjustItemPos(const UiPoint& pt, const UiPoint& ptMouseDown,
                               const std::vector<ItemStatus>& rcItemList) const override;

    /** Swap the positions of two controls to complete the order adjustment
    * @param [in] pt The current mouse position
    * @param [in] rcItemList The list of child controls
    * @param [in] nOldItemIndex The original child item index
    * @param [in] nNewItemIndex The new child item index
    * @return Returns true if the order is adjusted; otherwise returns false
    */
    virtual bool AdjustItemOrders(const UiPoint& pt,
                                  const std::vector<ItemStatus>& rcItemList,
                                  size_t& nOldItemIndex,
                                  size_t& nNewItemIndex) override;

    /** Control position drag complete event
    * @param [in] nOldItemIndex The original child item index
    * @param [in] nNewItemIndex The new child item index
    */
    virtual void OnItemOrdersChanged(size_t nOldItemIndex, size_t nNewItemIndex) override;

    /** @} */

protected:
    //Forbid external calls to the visibility-adjusting functions to avoid data inconsistency
    virtual void SetFadeVisible(bool bVisible) override;
    void SetVisible(bool bVisible);

    /** Whether the check mode is supported (currently used by TreeView/ListCtrl)
        The check mode means:
        (1) Only when clicking on the CheckBox image is the check box image in the selected state (in non-check mode, clicking anywhere within the control rect selects it)
        (2) The checked state and the selected state are separated, two different states
    */
    virtual bool SupportCheckMode() const override;

private:
    /** The associated Header interface
    */
    ListCtrlHeader* m_pHeaderCtrl;

    /** Sort icon: descending
    */
    Image* m_pSortedDownImage;

    /** Sort icon: ascending
    */
    Image* m_pSortedUpImage;

    /** The associated Split control interface
    */
    SplitBox* m_pSplitBox;

    /** Column width
    */
    int32_t m_nColumnWidth;

    /** The associated icon Id; if it is -1, the icon is not shown, and the icon is displayed in front of the text
    */
    int32_t m_imageId;

    /** The spacing between text and icon (when the icon is displayed to the right or left of the text)
    */
    int16_t m_nIconSpacing;

    /** Sort mode
    */
    SortMode m_sortMode;

    /** Whether the column width can be changed
    */
    bool m_bColumnResizeable;

    /** The icon is displayed above the text, centered
    */
    bool m_bShowIconAtTop;

    /** Show/hide this column
    */
    bool m_bColumnVisible;

    /** Whether the sort icon is shown (among all HeaderItems, only the sorted column shows it, other columns do not)
    */
    bool m_bShowSortImage;
};

}//namespace ui

#endif //UI_CONTROL_LIST_CTRL_HEADER_ITEM_H_
