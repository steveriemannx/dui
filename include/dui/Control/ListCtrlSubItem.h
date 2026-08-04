#ifndef UI_CONTROL_LIST_CTRL_SUB_ITEM_H_
#define UI_CONTROL_LIST_CTRL_SUB_ITEM_H_

#include "dui/Core/ImageList.h"
#include "dui/Control/ListCtrlDefs.h"

namespace ui
{
/** Sub-item of a list item in the Report view
*/
class ListCtrlItem;
class DUI_API ListCtrlSubItem : public ListCtrlLabel
{
    typedef ListCtrlLabel BaseClass;
    friend class ListCtrlReportView;//Allows calling the SetDataColumnId function
public:
    explicit ListCtrlSubItem(Window* pWindow);

    /** Get the control type
    */
    virtual DString GetType() const override;

    /** Set attributes
    */
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;

    /** DPI has changed, update the control size and layout
    * @param [in] nOldDpiScale The old DPI scale percentage
    * @param [in] nNewDpiScale The new DPI scale percentage, consistent with the value of Dpi().GetScale()
    */
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;

    /** Whether check mode is supported (currently TreeView/ListCtrl use this mode)
        Check mode means:
        (1) Only when clicking on the CheckBox image is the check box image in the selected state (in non-check mode, clicking anywhere within the control rectangle selects it)
        (2) The checked state and the selected state are separated, which are two different states
    */
    virtual bool SupportCheckMode() const override;

public:
    /** Set the associated Item interface
    */
    void SetListCtrlItem(ListCtrlItem* pItem);

    /** Get the associated Item interface
    */
    ListCtrlItem* GetListCtrlItem() const;

    /** Get the associated data item index, representing which row's data it is associated with
    * @return Returns the data item index, valid range: [0, ListCtrl::GetDataItemCount())
    */
    size_t GetDataItemIndex() const;

    /** Get the associated column index, representing which column's data it is associated with
    * @return Returns the column index of the data item, valid range: [0, ListCtrl::GetColumnCount())
    *         You can pass it to the ListCtrl::GetColumnId() function to get the corresponding column ID
    */
    size_t GetDataColumnIndex() const;

    /** Get the associated column ID, representing which column's data it is associated with
    * @return Returns the column ID of the data item; you can pass it to the ListCtrl::GetColumnIndex() function to get the corresponding column index
    */
    size_t GetDataColumnId() const;

    /** Set whether the CheckBox is displayed in the data item of this column
    * @param [in] bShow true means displaying the CheckBox at the beginning of the row, false means not displaying it
    */
    bool SetShowCheckBox(bool bShow);

    /** Determine whether the CheckBox of the current column's data item is in the displayed state
    @return Returns true if the CheckBox exists and is visible; returns false if there is no CheckBox
    */
    bool IsShowCheckBox() const;

    /** Determine whether the CheckBox of the current column's data item is checked
    */
    bool IsCheckBoxChecked() const;

    /** Set the associated icon Id; if it is -1, the icon is not displayed; the icon is displayed in front of the text
    */
    void SetImageId(int32_t imageId);

    /** Get the associated icon Id
    */
    int32_t GetImageId() const;

    /** Set the spacing between text and icon (in pixels)
    */
    void SetIconSpacing(int32_t nIconSpacing, bool bNeedDpiScale);

    /** Get the spacing between text and icon (in pixels)
    */
    int32_t GetIconSpacing() const;

protected:
    /** Draw text
    */
    virtual void PaintText(IRender* pRender) override;

    /** Calculate the text area size (width and height)
     *  @param [in] szAvailable The available size, not including padding, not including margin
     *  @return The estimated text size of the control, including padding (Box), not including margin
     */
    virtual UiSize EstimateText(UiSize szAvailable) override;

    /** Load the icon resource
    */
    ImagePtr LoadItemImage() const;

    /** Align the target rect vertically
    */
    void VAlignRect(UiRect& rc, uint32_t textStyle, int32_t nImageHeight);

    /** Get the image width of the CheckBox
    */
    int32_t GetCheckBoxImageWidth();

    /** Set the associated column ID, representing which column's data it is associated with
    */
    void SetDataColumnId(size_t nColumnId);

private:
    /** The associated Item interface
    */
    ListCtrlItem* m_pItem;

    /** The associated icon Id; if it is -1, the icon is not displayed; the icon is displayed in front of the text
    */
    int32_t m_imageId;

    /** The spacing between text and icon
    */
    int32_t m_nIconSpacing;

    /** The associated column ID
    */
    size_t m_nColumnId;
};

}//namespace ui

#endif //UI_CONTROL_LIST_CTRL_SUB_ITEM_H_
