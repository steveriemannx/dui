#ifndef UI_CONTROL_CHECKCOMBO_H_
#define UI_CONTROL_CHECKCOMBO_H_

#include "dui/Box/ScrollBox.h"
#include "dui/Core/Window.h"

namespace ui
{

class CCheckComboWnd;
class DUI_API CheckCombo : public Box
{
    typedef Box BaseClass;
    friend class CCheckComboWnd;
public:
    explicit CheckCombo(Window* pWindow);
    CheckCombo(const CheckCombo& r) = delete;
    CheckCombo& operator=(const CheckCombo& r) = delete;
    virtual ~CheckCombo() override;

public:
    /// Override the parent class methods to provide personalized features; please refer to the parent class declarations
    virtual DString GetType() const override;
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;
    virtual void Activate(const EventArgs* pMsg) override;

    /** DPI change: update the control size and layout
    * @param [in] nOldDpiScale The old DPI scale percentage
    * @param [in] nNewDpiScale The new DPI scale percentage, consistent with the value of Dpi().GetScale()
    */
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;

public:
    /// Override the parent class methods to provide personalized features; please refer to the parent class declarations
    virtual bool AddItem(Control* pControl) override;
    virtual bool AddItemAt(Control* pControl, size_t iIndex) override;
    virtual bool RemoveItem(Control* pControl) override;
    virtual bool RemoveItemAt(size_t iIndex) override;
    virtual void RemoveAllItems() override;
    virtual Control* GetItemAt(size_t iIndex) const override;
    virtual size_t GetItemIndex(Control* pControl) const override;
    virtual bool SetItemIndex(Control* pControl, size_t iIndex) override;
    virtual size_t GetItemCount() const override;

    /** Add a list item to the drop-down box
    * @param [in] itemText The text content
    */
    bool AddTextItem(const DString& itemText);

    /** Add a list item to the drop-down box
    * @param [in] itemTextId The text content ID (supports multilingual versions)
    */
    bool AddTextIdItem(const DString& itemTextId);

    /** Get the list of selected texts
    */
    void GetSelectedText(std::vector<DString>& selectedText) const;

    /** Clear all list items and selected items
    */
    void ClearAll();

public:
    /** Get the currently owned List object
    */
    ScrollBox* GetListBox() { return m_pDropList.get(); }

    /** Set the attribute information of the drop-down box
    * @param [in] pstrList The escaped XML-format attribute list
    */
    void SetDropBoxAttributeList(const DString& pstrList);

    /** Get the size of the drop-down box container
    */
    const UiSize& GetDropBoxSize() const;

    /** Set the drop-down list size (width and height)
     * @param [in] szDropBox The size information to set
     * @param [in] bNeedScaleDpi Whether DPI adaptation is needed
     */
    void SetDropBoxSize(UiSize szDropBox, bool bNeedScaleDpi);

    /** Set whether the ComboBox pops up upward
    * @param [in] top true pops up upward, false pops up downward
    */
    void SetPopupTop(bool top) { m_bPopupTop = top; };

    /** Judge whether the ComboBox popup mode is popping up upward
    */
    bool IsPopupTop() const { return m_bPopupTop; };

    /** Set the attribute of each list item in the drop-down list
    */
    void SetDropboxItemClass(const DString& classValue);

    /** Set the attribute of each child item in the selected items
    */
    void SetSelectedItemClass(const DString& classValue);

    /** Update the position of the drop-down list window
    */
    void UpdateComboWndPos();

    /** The window interface of the drop-down box (can only be obtained while displayed; invalid when hidden)
    */
    Window* GetCheckComboWnd() const;

    /** Set the shadow type of the drop-down window
    */
    void SetComboWndShadowType(Shadow::ShadowType nShadowType);

    /** Get the shadow type of the drop-down window
    */
    Shadow::ShadowType GetComboWndShadowType() const;

public:
    /** Listen to the drop-down window creation event
     * @param [in] callback The callback function triggered after the drop-down window is closed
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachWindowCreate(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventWindowCreate, callback, callbackID); }

    /** Listen to the drop-down window close event
    * @param [in] callback The callback function triggered after the drop-down window is closed
    * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
    */
    void AttachWindowClose(const ui::EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(ui::kEventWindowClose, callback, callbackID); }

private:
    /** The default handler for a child item being selected
    * @param[in] args The parameter list
    * @return Always returns true
    */
    bool OnSelectItem(const ui::EventArgs& args);
    bool OnUnSelectItem(const ui::EventArgs& args);
    bool OnListButtonDown(const ui::EventArgs& args);

private:
    /** Parse the attribute list
    */
    void ParseAttributeList(const DString& strList,
                            std::vector<std::pair<DString, DString>>& attributeList) const;

    /** Set the attribute list of the control
    */
    void SetAttributeList(Control* pControl, const DString& classValue);

    /** Update the height of the selected list
    */
    void UpdateSelectedListHeight();

private:
    /** The window interface of the drop-down box
    */
    CCheckComboWnd* m_pCheckComboWnd;

    /** Shadow type
    */
    Shadow::ShadowType m_nShadowType;

    /** The drop-down list container
    */
    std::unique_ptr<ui::ScrollBox> m_pDropList;

    /** The container of the selected list items
    */
    std::unique_ptr<ui::ScrollBox> m_pList;

    /** The width and height of the drop-down box
    */
    ui::UiSize m_szDropBox;

    /** Whether the drop-down box pops up upward
    */
    bool m_bPopupTop;

    /** The height of the container
    */
    int32_t m_iOrgHeight;

    /** The text of the selected items
    */
    std::vector<std::string> m_vecDate;

    /** The attribute of each list item in the drop-down list
    */
    UiString m_dropboxItemClass;

    /** The attribute of each child item in the selected items
    */
    UiString m_selectedItemClass;
};

} //namespace ui

#endif //UI_CONTROL_CHECKCOMBO_H_
