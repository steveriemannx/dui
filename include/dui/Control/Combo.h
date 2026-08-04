#ifndef UI_CONTROL_COMBO_H_
#define UI_CONTROL_COMBO_H_

#include "dui/Core/Box.h"
#include "dui/Control/TreeView.h"
#include "dui/Control/RichEdit.h"

namespace ui 
{
class RichEdit;
class CComboWnd;

/** Combo box control
*/
class DUI_API Combo : public Box
{
    typedef Box BaseClass;
    friend class CComboWnd;
public:
    explicit Combo(Window* pWindow);
    Combo(const Combo& r) = delete;
    Combo& operator=(const Combo& r) = delete;
    virtual ~Combo() override;

    /// Overrides base class methods to provide customized functionality; please refer to the base class declarations
    virtual DString GetType() const override;
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;
    virtual bool CanPlaceCaptionBar() const override;
    virtual DString GetBorderColor(ControlStateType stateType) const override;

    /** When the DPI changes, update the control size and layout
    * @param [in] nOldDpiScale The old DPI scale percentage
    * @param [in] nNewDpiScale The new DPI scale percentage, consistent with the value of Dpi().GetScale()
    */
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;

    /** The language has changed, refresh the text-related content displayed in the interface
    */
    virtual void OnLanguageChanged() override;

protected:
    virtual void OnInit() override;

public:
    /** Combo type
    */
    enum ComboType
    {
        //Drop-down list, not editable input
        kCombo_DropList = 0,

        //Editable list, allows editing input
        kCombo_DropDown = 1
    };

    /** Set the Combo type
    */
    void SetComboType(ComboType comboType);

    /** Get the Combo type
    */
    ComboType GetComboType() const;

    /** Get the size of the drop-down box list (width and height)
    */
    const UiSize& GetDropBoxSize() const;

    /** Set the size of the drop-down box list (width and height)
     * @param [in] szDropBox The size information to set
     * @param [in] bNeedScaleDpi Whether DPI adaptation is needed
     */
    void SetDropBoxSize(UiSize szDropBox, bool bNeedScaleDpi);

    /** Set whether the Combobox pops up upward
     * @param[in] top If true, pops up upward; false is the default popup downward
     */
    void SetPopupTop(bool top) { m_bPopupTop = top; }

    /** Determine whether the Combobox popup mode is upward popup
     * @return Returns true if it pops up upward, otherwise false
     */
    bool IsPopupTop() const { return m_bPopupTop; }

    /** Set the Class attribute of the drop-down TreeView
    */
    void SetComboTreeClass(const DString& classValue);

    /** Set the node Class attribute of the drop-down TreeView
    */
    void SetComboTreeNodeClass(const DString& classValue);

    /** Set the Class attribute of the icon control
    */
    void SetIconControlClass(const DString& classValue);

    /** Set the Class attribute of the edit control
    */
    void SetEditControlClass(const DString& classValue);

    /** Set the Class attribute of the button control
    */
    void SetButtonControlClass(const DString& classValue);

public:
    /** Get the number of items in the Combo list
    */
    size_t GetCount() const;

    /** Get the current selected item index
     * @return Returns the current selected item index (returns Box::InvalidIndex if there is no valid index)
     */
    size_t GetCurSel() const;

    /** Select an item, without triggering the selection event
     * @param[in] iIndex The index of the item to select
     * @return Returns true on success, otherwise false
     */
    bool SetCurSel(size_t iIndex);

    /** Get the data associated with the item
    * @param [in] iIndex The item index
    * @return Returns the data associated with the index
    */
    size_t GetItemData(size_t iIndex) const;

    /** Set the data associated with the item
    * @param [in] iIndex The item index
    * @param [in] itemData The data to be stored
    */
    bool SetItemData(size_t iIndex, size_t itemData);

    /** Get the text of the item
    * @param [in] iIndex The item index
    */
    DString GetItemText(size_t iIndex) const;

    /** Get the text ID of the item (multilingual version supported)
    * @param [in] iIndex The item index
    */
    DString GetItemTextId(size_t iIndex) const;

    /** Set the text of the item
    * @param [in] iIndex The item index
    * @param [in] itemText The text content of the item
    */
    bool SetItemText(size_t iIndex, const DString& itemText);

    /** Set the text ID of the item (multilingual version supported)
    * @param [in] iIndex The item index
    * @param [in] itemTextId The text content ID of the item
    */
    bool SetItemTextId(size_t iIndex, const DString& itemTextId);

    /** Add an item string
    * @param [in] itemText The text content of the item
    * @return Returns the index of the newly added item
    */
    size_t AddTextItem(const DString& itemText);

    /** Add an item string ID
    * @param [in] itemTextId The text content of the item (multilingual version supported)
    * @return Returns the index of the newly added item
    */
    size_t AddTextIdItem(const DString& itemTextId);

    /** Insert an item string at the specified index position, returns the index of the newly added item
    * @param [in] iIndex The item index
    * @param [in] itemText The text content of the item
    */
    size_t InsertTextItem(size_t iIndex, const DString& itemText);

    /** Insert an item string at the specified index position, returns the index of the newly added item
    * @param [in] iIndex The item index
    * @param [in] itemText The text content ID of the item (multilingual version supported)
    */
    size_t InsertTextIdItem(size_t iIndex, const DString& itemTextId);

    /** Delete an item
    * @param [in] iIndex The item index
    */
    bool DeleteItem(size_t iIndex);

    /** Remove all child nodes
     */
    void DeleteAllItems();

    /** Select the matching text item
    * @param [in] itemText The text content of the item
    * @param [in] bTriggerEvent Whether to trigger the selection event; if true, a kEventSelect event will be triggered
    * @return Returns the index of the selected item; if no item is selected, returns Box::InvalidIndex
    */
    size_t SelectTextItem(const DString& itemText, bool bTriggerEvent = true);

public:
    /** Get the text in the current edit box
     */
    DString GetText() const;

    /** Set the text in the edit box
    */
    void SetText(const DString& text);

    /** Give focus to the control
     */
    virtual void SetFocus() override;

public:
    /** Get the tree interface of the drop-down list
    */
    TreeView* GetTreeView();

    /** Get the icon control
    */
    Control* GetIconControl() const;

    /** The edit box control
    */
    RichEdit* GetEditControl() const;

    /** The button control
    */
    Button* GetButtonContrl() const;

    /** Update the position of the drop-down list window
    */
    void UpdateComboWndPos();

    /** The window interface of the drop-down box (only available when displayed; it becomes invalid when hidden)
    */
    Window* GetComboWnd() const;

    /** Set the shadow type of the drop-down window
    */
    void SetComboWndShadowType(Shadow::ShadowType nShadowType);

    /** Get the shadow type of the drop-down window
    */
    Shadow::ShadowType GetComboWndShadowType() const;

public:
    /** Listen for the item selection event
     * @param [in] callback The callback function triggered after an item is selected
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachSelect(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventSelect, callback, callbackID);}

    /** Listen for the drop-down window creation event
     * @param [in] callback The callback function triggered after the drop-down window is closed
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachWindowCreate(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventWindowCreate, callback, callbackID); }

    /** Listen for the drop-down window close event
     * @param [in] callback The callback function triggered after the drop-down window is closed
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachWindowClose(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventWindowClose, callback, callbackID); }

protected:
    /** Show the drop-down list
    */
    virtual void ShowComboList();

    /** Close the drop-down list
    */
    virtual void HideComboList();

    /** Update the drop-down list
    */
    virtual void UpdateComboList();

    /** The default handler for item selection
     * @param[in] args The argument list
     * @return Always returns true
     */
    virtual bool OnSelectItem(const EventArgs& args);

    /** The drop-down box window is closed
    * @param [in] bCanceled true indicates cancel, otherwise a normal close
    * @param [in] needUpdateSelItem true indicates that the selected item needs to be updated, otherwise not
    * @param [in] oldEditText The text content of the edit box when the drop-down box was displayed
    */
    virtual void OnComboWndClosed(bool bCanceled, 
                                  bool needUpdateSelItem,
                                  const DString& oldEditText);

    /** Mouse button pressed
     * @param[in] args The argument list
     * @return Always returns true
     */
    virtual bool OnButtonDown(const EventArgs& args);

    /** Click the button
     * @param[in] args The argument list
     * @return Always returns true
     */
    virtual bool OnButtonClicked(const EventArgs& args);

    /** Mouse button pressed on the Edit control
     * @param[in] args The argument list
     * @return Always returns true
     */
    virtual bool OnEditButtonDown(const EventArgs& args);

    /** Mouse button released on the Edit control
     * @param[in] args The argument list
     * @return Always returns true
     */
    virtual bool OnEditButtonUp(const EventArgs& args);

    /** A key is pressed on the Edit control
     * @param[in] args The argument list
     * @return Always returns true
     */
    virtual bool OnEditKeyDown(const EventArgs& args);

    /** The Edit control gains focus
    * @param[in] args The argument list
    * @return Always returns true
    */
    virtual bool OnEditSetFocus(const EventArgs& args);

    /** The Edit control loses focus
    * @param[in] args The argument list
    * @return Always returns true
    */
    virtual bool OnEditKillFocus(const EventArgs& args);

    /** The window loses focus
    * @param[in] args The argument list
    * @return Always returns true
    */
    virtual bool OnWindowKillFocus(const EventArgs& args) override;

    /** The window moves
    * @param[in] args The argument list
    * @return Always returns true
    */
    virtual bool OnWindowMove(const EventArgs& args);

    /** The selected item changes, sync the text of the Edit control
    */
    virtual void OnSelectedItemChanged();

    /** The text content of the Edit control changes
     * @param[in] args The argument list
     * @return Always returns true
     */
    virtual bool OnEditTextChanged(const ui::EventArgs& args);

private:
    /** Parse the attribute list
    */
    void ParseAttributeList(const DString& strList,
                            std::vector<std::pair<DString, DString>>& attributeList) const;

    /** Set the attribute list of the control
    */
    void SetAttributeList(Control* pControl, const DString& classValue);

    /** Remove the control
    */
    void RemoveControl(Control* pControl);

    /** Create a new tree node
    * @param [in] itemText The text content of the item
    * @param [in] bTextId Whether the text content of itemText is a text ID (multilingual version supported)
    */
    TreeNode* CreateTreeNode(const DString& itemText, bool bTextId);

    /** Attach the mouse events of this control and set the focus
    */
    void AttachMouseEvents(Control* pControl);

    /** Insert an item string at the specified index position, returns the index of the newly added item
    * @param [in] iIndex The item index
    * @param [in] itemText The text content of the item
    * @param [in] bTextId Whether the text content of itemText is a text ID (multilingual version supported)
    */
    size_t PrivateInsertTextItem(size_t iIndex, const DString& itemText, bool bTextId);

private:
    /** Combo type
    */
    uint8_t m_comboType;

    /** The interface of the tree view
    */
    TreeView m_treeView;

    /** The window interface of the drop-down list
    */
    CComboWnd* m_pWindow;

    /** The shadow type
    */
    Shadow::ShadowType m_nShadowType;

    /** The size of the drop-down list (width and height)
    */
    UiSize m_szDropBox;

    /** Whether the drop-down list pops up upward
    */
    bool m_bPopupTop;

    /** The node Class attribute of the drop-down TreeView
    */
    UiString m_treeNodeClass;

private:
    /** The icon control
    */
    ControlPtr m_pIconControl;

    /** The edit box control
    */
    ControlPtrT<RichEdit> m_pEditControl;

    /** The button control
    */
    ControlPtrT<Button> m_pButtonControl;

    /** The current selected item index
    */
    size_t m_iCurSel;

    /** Whether the drop-down list is being displayed when the mouse button is pressed
    */
    bool m_bDropListShown;
};

} // namespace ui

#endif // UI_CONTROL_COMBO_H_
