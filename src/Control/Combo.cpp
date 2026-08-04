#include "dui/Control/Combo.h"
#include "dui/Core/Window.h"
#include "dui/Core/Keyboard.h"
#include "dui/Box/ListBox.h"
#include "dui/Box/HBox.h"
#include "dui/Core/WindowCreateParam.h"

namespace ui
{

/** Drop-down list window
*/
class CComboWnd: public Window
{
    typedef Window BaseClass;
public:
    /** Create and display the drop-down window
    */
    void InitComboWnd(Combo* pOwner, bool bActivated);

    /** Update the position and size of the drop-down window
    */
    void UpdateComboWnd();

    //Overrides of base class virtual functions
    virtual void OnInitWindow() override;
    virtual void OnCloseWindow() override;
    virtual void OnFinalMessage() override;
    virtual void OnWindowShadowTypeChanged() override;

    virtual LRESULT OnKeyDownMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;
    virtual LRESULT OnKillFocusMsg(WindowBase* pSetFocusWindow, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Close the drop-down box
    * @param [in] bCanceled true indicates cancel, otherwise a normal close
    * @param [in] needUpdateSelItem true indicates that the selected item needs to be updated, otherwise not
    */
    void CloseComboWnd(bool bCanceled, bool needUpdateSelItem);

private:
    /** Calculate the display rectangle of the drop-down box
    */
    UiRect GetComboWndRect() const;

private:
    //The associated Combo interface
    ControlPtrT<Combo> m_pOwner;

    //The original selected item index
    size_t m_iOldSel = Box::InvalidIndex;

    //The original text content of the Edit control
    UiString m_editText;

    //Whether it has been closed
    bool m_bIsClosed = false;
};

void CComboWnd::InitComboWnd(Combo* pOwner, bool bActivated)
{
    ASSERT(pOwner != nullptr);
    if (pOwner == nullptr) {
        return;
    }
    m_pOwner = pOwner;
    m_iOldSel = m_pOwner->GetCurSel();
    m_editText = m_pOwner->GetText();
    m_bIsClosed = false;

    //Set the display position and size of the drop-down box to avoid a black screen when the popup is shown
    UiRect rcWnd = GetComboWndRect();
    WindowCreateParam createWndParam;
    createWndParam.m_dwStyle = kWS_POPUP;
    createWndParam.m_dwExStyle = kWS_EX_LAYERED;
#ifdef DUI_BUILD_FOR_SDL
    createWndParam.m_dwExStyle |= kWS_EX_NOACTIVATE;
#endif
    createWndParam.m_nX = rcWnd.left;
    createWndParam.m_nY = rcWnd.top;
    createWndParam.m_nWidth = rcWnd.Width();
    createWndParam.m_nHeight = rcWnd.Height();
    CreateWnd(pOwner->GetWindow(), createWndParam);

    UpdateComboWnd();
    if (bActivated) {
        ShowWindow(ui::kSW_SHOW_NORMAL);
        SetWindowForeground();
        KeepParentActive();
        pOwner->GetTreeView()->SetFocus();
        pOwner->SetState(kControlStateHot);
    }
    else {
        ShowWindow(ui::kSW_SHOW_NA);
    }
    if (Box::IsValidItemIndex(m_iOldSel)) {
        //When expanded, ensure the selection is visible
        UpdateWindow();
        pOwner->GetTreeView()->EnsureVisible(m_iOldSel, ListBoxVerVisible::kVisibleAtCenter);
    }

    //Send an event
    pOwner->SendEvent(kEventWindowCreate);
}

UiRect CComboWnd::GetComboWndRect() const
{
    ControlPtrT<Combo> pOwner = m_pOwner;
    if (pOwner == nullptr) {
        return UiRect();
    }
    //The size of the shadow
    ui::UiPadding rcPadding;
    if (IsWindow()) {
        rcPadding = GetCurrentShadowCorner();
    }

    // Position the popup window in absolute space
    UiSize szDrop = pOwner->GetDropBoxSize();
    UiRect rcOwner = pOwner->GetPos();
    UiPoint scrollBoxOffset = pOwner->GetScrollOffsetInScrollBox();
    rcOwner.Offset(-scrollBoxOffset.x, -scrollBoxOffset.y);

    UiRect rc = rcOwner;
    rc.top = rc.bottom + Dpi().GetScaleInt(1);  // The parent window's left and bottom positions are used as the starting point of the popup window
    rc.bottom = rc.top + szDrop.cy;             // Calculate the popup window height
    if (szDrop.cx > 0) {
        rc.right = rc.left + szDrop.cx;         // Calculate the popup window width
    }

    //If all items in the child container are of the stretch type, there is no need to estimate the size (it would fail and cannot be estimated); display according to the configured size of the drop-down box instead
    bool bCanEstimateSize = true;
    if (pOwner->GetTreeView()->GetFixedHeight().IsStretch() && pOwner->GetTreeView()->GetFixedWidth().IsStretch()) {
        size_t nItemCount = pOwner->GetTreeView()->GetItemCount();
        if (nItemCount > 0) {
            bCanEstimateSize = false;
            for (size_t nItemIndex = 0; nItemIndex < nItemCount; nItemIndex++) {
                Control* pControl = pOwner->GetTreeView()->GetItemAt(nItemIndex);
                if ((pControl == nullptr) || !pControl->IsVisible() || pControl->IsFloat()) {
                    continue;
                }
                if (!pControl->GetFixedHeight().IsStretch() || !pControl->GetFixedWidth().IsStretch()) {
                    bCanEstimateSize = true;
                    break;
                }
            }
        }
    }

    int32_t cyFixed = 0;
    if (bCanEstimateSize && pOwner->GetTreeView()->GetItemCount() > 0) {
        UiSize szAvailable(rc.Width(), rc.Height());
        UiFixedInt oldFixedHeight = pOwner->GetTreeView()->GetFixedHeight();
        pOwner->GetTreeView()->SetFixedHeight(UiFixedInt::MakeAuto(), false, false);
        UiEstSize estSize = pOwner->GetTreeView()->EstimateSize(szAvailable);
        pOwner->GetTreeView()->SetFixedHeight(oldFixedHeight, false, false);
        cyFixed = estSize.cy.GetInt32();
    }
    if (cyFixed == 0) {
        cyFixed = szDrop.cy;
    }
    rc.bottom = rc.top + std::min(cyFixed, szDrop.cy);

    rc.Inflate(rcPadding);
    pOwner->GetWindow()->ClientToScreen(rc);

    UiRect rcWork;
    pOwner->GetWindow()->GetMonitorWorkRect(rcWork);
    if (rc.bottom > rcWork.bottom || m_pOwner->IsPopupTop()) {
        rc.left = rcOwner.left;
        rc.right = rcOwner.right;
        if (szDrop.cx > 0) {
            rc.right = rc.left + szDrop.cx;
        }
        rc.top = rcOwner.top - std::min(cyFixed, szDrop.cy);
        rc.bottom = rcOwner.top;
        rc.Inflate(rcPadding);
        pOwner->GetWindow()->ClientToScreen(rc);
    }
    return rc;
}

void CComboWnd::UpdateComboWnd()
{
    UiRect rc = GetComboWndRect();
    if (rc.IsEmpty()) {
        return;
    }
    SetWindowPos(InsertAfterWnd(), rc.left, rc.top, rc.Width(), rc.Height(), kSWP_NOZORDER | kSWP_NOACTIVATE);
}

void CComboWnd::OnFinalMessage()
{
    if (m_pOwner != nullptr) {
        if (m_pOwner->GetTreeView()->GetWindow() == this) {
            m_pOwner->GetTreeView()->SetWindow(nullptr);
            m_pOwner->GetTreeView()->SetParent(nullptr);
        }
        if (m_pOwner->m_pWindow == this) {            
            m_pOwner->m_pWindow = nullptr;
            m_pOwner->SetState(kControlStateNormal);
            m_pOwner->Invalidate();
        }
    }
    BaseClass::OnFinalMessage();
}

void CComboWnd::OnWindowShadowTypeChanged()
{
    if (IsWindow() && (GetRoot() != nullptr)) {
        UpdateComboWnd();
    }
}

void CComboWnd::CloseComboWnd(bool bCanceled, bool needUpdateSelItem)
{
    if (m_bIsClosed) {
        return;
    }
    m_bIsClosed = true;
    Box* pRootBox = GetRoot();
    if ((pRootBox != nullptr) && (pRootBox->GetItemCount() > 0)) {
        m_pOwner->GetTreeView()->SetWindow(nullptr);
        m_pOwner->GetTreeView()->SetParent(nullptr);
        pRootBox->RemoveAllItems();
    }
    //Switch the foreground window to the parent window first, to avoid switching to another window after the foreground window is closed
    ControlPtrT<Combo> pOwner = m_pOwner;
    if ((pOwner != nullptr) && (pOwner->GetWindow() != nullptr)) {
        if (IsWindowForeground()) {
            pOwner->GetWindow()->SetWindowForeground();
        }
    }

    CloseWnd();
    if (m_pOwner != nullptr) {
        if (bCanceled) {
            m_pOwner->GetTreeView()->SelectItem(m_iOldSel, false, false);
        }
        m_pOwner->OnComboWndClosed(bCanceled, needUpdateSelItem, m_editText.c_str());
    }
}

void CComboWnd::OnInitWindow()
{
    BaseClass::OnInitWindow();

    SetResourcePath(m_pOwner->GetWindow()->GetResourcePath());
    SetShadowType(m_pOwner->GetComboWndShadowType());

    Box* pRoot = new Box(this);
    pRoot->SetAutoDestroyChild(false);
    pRoot->AddItem(m_pOwner->GetTreeView());
    AttachBox(AttachShadow(pRoot));

    //Update the window position
    UpdateComboWnd();
}

void CComboWnd::OnCloseWindow()
{
    Box* pRootBox = GetRoot();
    if ((pRootBox != nullptr) && (pRootBox->GetItemCount() > 0)) {
        m_pOwner->GetTreeView()->SetWindow(nullptr);
        m_pOwner->GetTreeView()->SetParent(nullptr);
        pRootBox->RemoveAllItems();
    }
    if ((m_pOwner->GetWindow() != nullptr) && m_pOwner->GetWindow()->IsWindow()) {
        m_pOwner->SetPos(m_pOwner->GetPos());
        m_pOwner->SetFocus();
    }    
    BaseClass::OnCloseWindow();
}

LRESULT CComboWnd::OnKeyDownMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    LRESULT lResult = BaseClass::OnKeyDownMsg(vkCode, modifierKey, nativeMsg, bHandled);
    if (vkCode == kVK_ESCAPE) {
        //Press and hold the ESC key to cancel
        CloseComboWnd(true, false);
    }
    else if (vkCode == kVK_RETURN) {
        //Press the Enter key to close the window normally
        CloseComboWnd(false, true);
    }
    return lResult;
}

LRESULT CComboWnd::OnKillFocusMsg(WindowBase* pSetFocusWindow, const NativeMsg& nativeMsg, bool& bHandled)
{
    LRESULT lResult = BaseClass::OnKillFocusMsg(pSetFocusWindow, nativeMsg, bHandled);
    //Lost focus, close the window normally
    if (pSetFocusWindow != this) {
        CloseComboWnd(false, false);
    }
    return lResult;
}

////////////////////////////////////////////////////////

Combo::Combo(Window* pWindow) :
    Box(pWindow),
    m_treeView(pWindow),
    m_pWindow(nullptr),
    m_bPopupTop(false),
    m_iCurSel(Box::InvalidIndex),
    m_pIconControl(nullptr),
    m_pEditControl(nullptr),
    m_pButtonControl(nullptr),
    m_comboType(kCombo_DropDown),
    m_bDropListShown(false),
    m_nShadowType(Shadow::ShadowType::kShadowMenu)
{
    SetDropBoxSize({0, 150}, true);
    m_treeView.SetSelectNextWhenActiveRemoved(false);
    m_treeView.AttachSelect(UiBind(&Combo::OnSelectItem, this, std::placeholders::_1));
}

Combo::~Combo()
{
    if (!IsInited()) {
        if (m_pIconControl != nullptr) {
            delete m_pIconControl.get();
            m_pIconControl = nullptr;
        }
        if (m_pEditControl != nullptr) {
            delete m_pEditControl.get();
            m_pEditControl = nullptr;
        }
        if (m_pButtonControl != nullptr) {
            delete m_pButtonControl.get();
            m_pButtonControl = nullptr;
        }
    }
}

DString Combo::GetType() const { return DUI_CTR_COMBO; }

void Combo::SetAttribute(const DString& strName, const DString& strValue)
{
    if (strName == _T("combo_type")) {
        if (strValue == _T("drop_list")) {
            SetComboType(kCombo_DropList);
        }
        else if (strValue == _T("drop_down")) {
            SetComboType(kCombo_DropDown);
        }
    }
    else if (strName == _T("shadow_type")) {
        //Set the shadow type of the drop-down window
        Shadow::ShadowType nShadowType = Shadow::ShadowType::kShadowCount;
        if (Shadow::GetShadowType(strValue, nShadowType)) {
            SetComboWndShadowType(nShadowType);
        }
    }
    else if ((strName == _T("dropbox_size")) || (strName == _T("dropboxsize")) ) {
        //Set the size of the drop-down list (width and height)
        UiSize szDropBoxSize;
        AttributeUtil::ParseSizeValue(strValue.c_str(), szDropBoxSize);
        SetDropBoxSize(szDropBoxSize, true);
    }
    else if ((strName == _T("popup_top")) || (strName == _T("popuptop"))) {
        //Whether the drop-down list pops up upward
        SetPopupTop(strValue == _T("true"));
    }
    else if (strName == _T("combo_tree_view_class")) {
        SetComboTreeClass(strValue);
    }
    else if (strName == _T("combo_tree_node_class")) {
        SetComboTreeNodeClass(strValue);
    }
    else if (strName == _T("combo_icon_class")) {
        SetIconControlClass(strValue);
    }
    else if (strName == _T("combo_edit_class")) {
        SetEditControlClass(strValue);
    }
    else if (strName == _T("combo_button_class")) {
        SetButtonControlClass(strValue);
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

void Combo::SetComboWndShadowType(Shadow::ShadowType nShadowType)
{
    m_nShadowType = nShadowType;
    if (m_pWindow != nullptr) {
        m_pWindow->SetShadowType(nShadowType);
    }
}

Shadow::ShadowType Combo::GetComboWndShadowType() const
{
    return m_nShadowType;
}

void Combo::ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale)
{
    if (!Dpi().CheckDisplayScaleFactor(nNewDpiScale)) {
        return;
    }

    UiSize szDropBoxSize = GetDropBoxSize();
    szDropBoxSize = Dpi().GetScaleSize(szDropBoxSize, nOldDpiScale);
    SetDropBoxSize(szDropBoxSize, false);

    if (m_treeView.GetWindow() == nullptr) {
        m_treeView.SetWindow(GetWindow());
    }
    if ((m_treeView.GetWindow() == GetWindow()) && (m_treeView.GetParent() == nullptr)) {
        m_treeView.ChangeDpiScale(nOldDpiScale, nNewDpiScale);
    }
    BaseClass::ChangeDpiScale(nOldDpiScale, nNewDpiScale);
}

void Combo::SetComboTreeClass(const DString& classValue)
{
    if (m_treeView.GetWindow() == nullptr) {
        m_treeView.SetWindow(GetWindow());
    }
    SetAttributeList(&m_treeView, classValue);
}

void Combo::SetComboTreeNodeClass(const DString& classValue)
{
    m_treeNodeClass = classValue;
}

void Combo::SetIconControlClass(const DString& classValue)
{
    if (classValue.empty()) {
        RemoveControl(m_pIconControl.get());
        if (m_pIconControl != nullptr) {
            delete m_pIconControl.get();
            m_pIconControl = nullptr;
        }
    }
    else {
        if (m_pIconControl == nullptr) {
            m_pIconControl = new Control(GetWindow());
        }
        SetAttributeList(m_pIconControl.get(), classValue);
    }
}

void Combo::SetEditControlClass(const DString& classValue)
{
    if (classValue.empty()) {
        RemoveControl(m_pEditControl.get());
        if (m_pEditControl != nullptr) {
            delete m_pEditControl.get();
            m_pEditControl = nullptr;
        }
    }
    else {
        if (m_pEditControl == nullptr) {
            m_pEditControl = new RichEdit(GetWindow());
        }
        SetAttributeList(m_pEditControl.get(), classValue);
    }    
}

void Combo::SetButtonControlClass(const DString& classValue)
{
    if (classValue.empty()) {
        RemoveControl(m_pButtonControl.get());
        if (m_pButtonControl != nullptr) {
            delete m_pButtonControl.get();
            m_pButtonControl = nullptr;
        }
    }
    else {
        if (m_pButtonControl == nullptr) {
            m_pButtonControl = new Button(GetWindow());
        }
        SetAttributeList(m_pButtonControl.get(), classValue);
    }
}

void Combo::ParseAttributeList(const DString& strList,
                               std::vector<std::pair<DString, DString>>& attributeList) const
{
    if (strList.empty()) {
        return;
    }
    DString strValue = strList;
    //These are hand-written attributes; curly braces {} are used instead of double quotes, so no escape characters are needed when writing them;
    StringUtil::ReplaceAll(_T("{"), _T("\""), strValue);
    StringUtil::ReplaceAll(_T("}"), _T("\""), strValue);
    if (strValue.find(_T("\"")) != DString::npos) {
        AttributeUtil::ParseAttributeList(strValue, _T('\"'), attributeList);
    }
    else if (strValue.find(_T("\'")) != DString::npos) {
        AttributeUtil::ParseAttributeList(strValue, _T('\''), attributeList);
    }
}

void Combo::SetAttributeList(Control* pControl, const DString& classValue)
{
    ASSERT(pControl != nullptr);
    if (pControl == nullptr) {
        return;
    }
    std::vector<std::pair<DString, DString>> attributeList;
    ParseAttributeList(classValue, attributeList);
    if (!attributeList.empty()) {
        //Set according to the attribute list
        for (const auto& attribute : attributeList) {
            pControl->SetAttribute(attribute.first, attribute.second);
        }
    }
    else if(!classValue.empty()) {
        //Set according to the Class name
        pControl->SetClass(classValue);
    }
}

void Combo::RemoveControl(Control* pControl)
{
    if (IsInited() && (GetItemCount() > 0)) {
        HBox* pBox = dynamic_cast<HBox*>(GetItemAt(0));
        if (pBox != nullptr) {
            pBox->RemoveItem(pControl);
        }
    }
}

bool Combo::CanPlaceCaptionBar() const
{
    return true;
}

DString Combo::GetBorderColor(ControlStateType stateType) const
{
    DString borderColor;
    if (m_pIconControl != nullptr) {
        if (m_pIconControl->IsFocused() || m_pIconControl->IsMouseFocused()) {
            borderColor = BaseClass::GetBorderColor(kControlStateHot);
        }
    }
    if (borderColor.empty() && (m_pEditControl != nullptr)) {
        if (m_pEditControl->IsFocused() || m_pEditControl->IsMouseFocused()) {
            borderColor = BaseClass::GetBorderColor(kControlStateHot);
        }
    }
    if (borderColor.empty() && (m_pButtonControl != nullptr)) {
        if (m_pButtonControl->IsFocused() || m_pButtonControl->IsMouseFocused()) {
            borderColor = BaseClass::GetBorderColor(kControlStateHot);
        }
    }
    if (borderColor.empty() && (m_pWindow != nullptr) && !m_pWindow->IsClosingWnd()) {
        borderColor = BaseClass::GetBorderColor(kControlStateHot);
    }
    if (borderColor.empty()) {
        borderColor = BaseClass::GetBorderColor(stateType);
    }
    return borderColor;
}

void Combo::OnInit()
{
    if (IsInited()) {
        return;
    }
    BaseClass::OnInit();

    HBox* pBox = new HBox(GetWindow());
    AddItem(pBox);
    pBox->SetNoFocus();
    AttachMouseEvents(pBox);

    if (m_pIconControl != nullptr) {        
        pBox->AddItem(m_pIconControl.get());
        AttachMouseEvents(m_pIconControl.get());
    }
    if (m_pEditControl != nullptr) {
        pBox->AddItem(m_pEditControl.get());
    }
    if (m_pButtonControl != nullptr) {
        pBox->AddItem(m_pButtonControl.get());
        AttachMouseEvents(m_pButtonControl.get());
    }

    if (m_pIconControl != nullptr) {
        m_pIconControl->SetNoFocus();
    }
    if (m_pButtonControl != nullptr) {
        m_pButtonControl->SetNoFocus();        
        m_pButtonControl->AttachButtonDown(UiBind(&Combo::OnButtonDown, this, std::placeholders::_1));
        m_pButtonControl->AttachClick(UiBind(&Combo::OnButtonClicked, this, std::placeholders::_1));
    }
    if (m_pEditControl != nullptr) {
        m_pEditControl->SetWantReturn(true);
        m_pEditControl->AttachButtonDown(UiBind(&Combo::OnEditButtonDown, this, std::placeholders::_1));
        m_pEditControl->AttachButtonUp(UiBind(&Combo::OnEditButtonUp, this, std::placeholders::_1));
        m_pEditControl->AttachEvent(kEventKeyDown, UiBind(&Combo::OnEditKeyDown, this, std::placeholders::_1), 0);
        m_pEditControl->AttachSetFocus(UiBind(&Combo::OnEditSetFocus, this, std::placeholders::_1));
        m_pEditControl->AttachKillFocus(UiBind(&Combo::OnEditKillFocus, this, std::placeholders::_1));
        m_pEditControl->AttachEvent(kEventWindowKillFocus, UiBind(&Combo::OnWindowKillFocus, this, std::placeholders::_1), 0);
        m_pEditControl->AttachEvent(kEventWindowMove, UiBind(&Combo::OnWindowMove, this, std::placeholders::_1), 0);
        m_pEditControl->AttachEvent(kEventWindowPosChanged, UiBind(&Combo::OnWindowMove, this, std::placeholders::_1), 0);
        m_pEditControl->AttachTextChanged(UiBind(&Combo::OnEditTextChanged, this, std::placeholders::_1));
    }
    SetNoFocus();
    SetComboType(GetComboType());
}

TreeView* Combo::GetTreeView()
{
    return &m_treeView;
}

Control* Combo::GetIconControl() const
{
    return m_pIconControl.get();
}

RichEdit* Combo::GetEditControl() const
{
    return m_pEditControl.get();
}

Button* Combo::GetButtonContrl() const
{
    return m_pButtonControl.get();
}

void Combo::SetComboType(ComboType comboType)
{
    if (comboType == kCombo_DropList) {
        m_comboType = kCombo_DropList;
        if (m_pEditControl != nullptr) {
            //Set to read-only and hide the caret
            m_pEditControl->SetReadOnly(true);
            m_pEditControl->SetNoCaretReadonly();
            m_pEditControl->SetCursorType(CursorType::kCursorArrow);
            m_pEditControl->SetUseControlCursor(true);
        }
    }
    else if (comboType == kCombo_DropDown) {
        m_comboType = kCombo_DropDown;
        if (m_pEditControl != nullptr) {
            m_pEditControl->SetReadOnly(false);
            m_pEditControl->SetUseControlCursor(false);
        }
    }
}

Combo::ComboType Combo::GetComboType() const
{
    if (m_comboType == kCombo_DropList) {
        return kCombo_DropList;
    }
    else if (m_comboType == kCombo_DropDown) {
        return kCombo_DropDown;
    }
    return kCombo_DropList;
}

const UiSize& Combo::GetDropBoxSize() const
{
    return m_szDropBox;
}

void Combo::SetDropBoxSize(UiSize szDropBox, bool bNeedScaleDpi)
{
    ASSERT(szDropBox.cy > 0);
    if (szDropBox.cy <= 0) {
        return;
    }
    szDropBox.Validate();
    if (bNeedScaleDpi) {
        Dpi().ScaleSize(szDropBox);
    }
    m_szDropBox = szDropBox;
}

size_t Combo::GetCount() const
{
    return m_treeView.GetRootNode()->GetDescendantNodeCount();
}

size_t Combo::GetCurSel() const
{ 
    return m_treeView.GetCurSel();
}

bool Combo::SetCurSel(size_t iIndex)
{
    size_t iOldSel = m_iCurSel;
    bool bRet = m_treeView.SelectItem(iIndex, false, false);
    m_iCurSel = m_treeView.GetCurSel();
    OnSelectedItemChanged();
    if (m_iCurSel != iOldSel) {
        Invalidate();
    }
    return bRet;
}

size_t Combo::GetItemData(size_t iIndex) const
{
    Control* pControl = m_treeView.GetItemAt(iIndex);
    if (pControl != nullptr) {
        ASSERT(dynamic_cast<TreeNode*>(pControl) != nullptr);
        return pControl->GetUserDataID();
    }
    return 0;
}

bool Combo::SetItemData(size_t iIndex, size_t itemData)
{
    Control* pControl = m_treeView.GetItemAt(iIndex);
    if (pControl != nullptr) {
        ASSERT(dynamic_cast<TreeNode*>(pControl) != nullptr);
        pControl->SetUserDataID(itemData);
        return true;
    }
    return false;
}

DString Combo::GetItemText(size_t iIndex) const
{
    Control* pControl = m_treeView.GetItemAt(iIndex);
    if (pControl != nullptr) {
        TreeNode* pTreeNode = dynamic_cast<TreeNode*>(pControl);
        ASSERT(pTreeNode != nullptr);
        if (pTreeNode != nullptr) {
            return pTreeNode->GetText();
        }        
    }
    return DString();
}

DString Combo::GetItemTextId(size_t iIndex) const
{
    Control* pControl = m_treeView.GetItemAt(iIndex);
    if (pControl != nullptr) {
        TreeNode* pTreeNode = dynamic_cast<TreeNode*>(pControl);
        ASSERT(pTreeNode != nullptr);
        if (pTreeNode != nullptr) {
            return pTreeNode->GetTextId();
        }
    }
    return DString();
}

bool Combo::SetItemText(size_t iIndex, const DString& itemText)
{
    Control* pControl = m_treeView.GetItemAt(iIndex);
    if (pControl != nullptr) {
        TreeNode* pTreeNode = dynamic_cast<TreeNode*>(pControl);
        ASSERT(pTreeNode != nullptr);
        if (pTreeNode != nullptr) {
            pTreeNode->SetText(itemText);
            OnSelectedItemChanged();
            return true;
        }
    }
    return false;
}

bool Combo::SetItemTextId(size_t iIndex, const DString& itemTextId)
{
    Control* pControl = m_treeView.GetItemAt(iIndex);
    if (pControl != nullptr) {
        TreeNode* pTreeNode = dynamic_cast<TreeNode*>(pControl);
        ASSERT(pTreeNode != nullptr);
        if (pTreeNode != nullptr) {
            pTreeNode->SetTextId(itemTextId);
            OnSelectedItemChanged();
            return true;
        }
    }
    return false;
}

size_t Combo::AddTextItem(const DString& itemText)
{
    return InsertTextItem(GetCount(), itemText);
}

size_t Combo::AddTextIdItem(const DString& itemTextId)
{
    return PrivateInsertTextItem(GetCount(), itemTextId, true);
}

size_t Combo::InsertTextItem(size_t iIndex, const DString& itemText)
{
    return PrivateInsertTextItem(iIndex, itemText, false);
}

size_t Combo::InsertTextIdItem(size_t iIndex, const DString& itemTextId)
{
    return PrivateInsertTextItem(iIndex, itemTextId, true);
}

size_t Combo::PrivateInsertTextItem(size_t iIndex, const DString& itemText, bool bTextId)
{
    ASSERT(iIndex <= GetCount());
    if (iIndex > GetCount()) {
        return Box::InvalidIndex;
    }
    size_t newIndex = Box::InvalidIndex;
    if (iIndex == GetCount()) {
        //Insert a new node at the end
        TreeNode* pNewNode = CreateTreeNode(itemText, bTextId);
        m_treeView.GetRootNode()->AddChildNode(pNewNode);
        newIndex = m_treeView.GetItemIndex(pNewNode);
    }
    else {
        //Insert a new node at the specified position
        Control* pControl = m_treeView.GetItemAt(iIndex);
        if (pControl != nullptr) {
            TreeNode* pTreeNode = dynamic_cast<TreeNode*>(pControl);
            ASSERT(pTreeNode != nullptr);
            if (pTreeNode != nullptr) {
                TreeNode* pParentNode = pTreeNode->GetParentNode();
                ASSERT(pParentNode != nullptr);
                if (pParentNode != nullptr) {
                    size_t iChildIndex = pParentNode->GetChildNodeIndex(pTreeNode);
                    TreeNode* pNewNode = CreateTreeNode(itemText, bTextId);
                    pParentNode->AddChildNodeAt(pNewNode, iChildIndex);
                    newIndex = m_treeView.GetItemIndex(pNewNode);
                }
            }
        }
    }
    ASSERT(newIndex != Box::InvalidIndex);
    return newIndex;
}

bool Combo::DeleteItem(size_t iIndex)
{
    bool bRemoved = false;
    Control* pControl = m_treeView.GetItemAt(iIndex);
    if (pControl != nullptr) {
        TreeNode* pTreeNode = dynamic_cast<TreeNode*>(pControl);
        ASSERT(pTreeNode != nullptr);
        if (pTreeNode != nullptr) {
            TreeNode* pParentNode = pTreeNode->GetParentNode();
            ASSERT(pParentNode != nullptr);
            if (pParentNode != nullptr) {
                bRemoved = pParentNode->RemoveChildNode(pTreeNode);
            }
        }
    }
    if (bRemoved) {
        OnSelectedItemChanged();
    }
    return bRemoved;
}

void Combo::DeleteAllItems()
{
    m_treeView.GetRootNode()->RemoveAllChildNodes();
    OnSelectedItemChanged();
}

size_t Combo::SelectTextItem(const DString& itemText, bool bTriggerEvent)
{
    size_t nSelIndex = Box::InvalidIndex;
    size_t itemCount = m_treeView.GetItemCount();
    for (size_t nIndex = 0; nIndex < itemCount; ++nIndex) {
        Control* pControl = m_treeView.GetItemAt(nIndex);
        if (pControl != nullptr) {
            TreeNode* pTreeNode = dynamic_cast<TreeNode*>(pControl);
            ASSERT(pTreeNode != nullptr);
            if (pTreeNode != nullptr) {
                if (pTreeNode->GetText() == itemText) {
                    nSelIndex = nIndex;
                    break;
                }
            }
        }
    }
    m_treeView.SelectItem(nSelIndex, false, bTriggerEvent);
    if (!bTriggerEvent) {
        OnSelectedItemChanged();
    }
    return nSelIndex;
}

TreeNode* Combo::CreateTreeNode(const DString& itemText, bool bTextId)
{
    TreeNode* pNewNode = new TreeNode(GetWindow());
    if (!m_treeNodeClass.empty()) {
        SetAttributeList(pNewNode, m_treeNodeClass.c_str());
    }
    if (bTextId) {
        pNewNode->SetTextId(itemText);
    }
    else {
        pNewNode->SetText(itemText);
    }    
    return pNewNode;
}

DString Combo::GetText() const
{
    if (m_pEditControl != nullptr) {
        return m_pEditControl->GetText();
    }
    return DString();
}

void Combo::SetText(const DString& text)
{
    if (m_pEditControl != nullptr) {
        m_pEditControl->SetText(text);
    }
}

bool Combo::OnSelectItem(const EventArgs& /*args*/)
{
    size_t iOldSel = m_iCurSel;
    m_iCurSel = m_treeView.GetCurSel();
    OnSelectedItemChanged();
    if (m_iCurSel != iOldSel) {
        SendEvent(kEventSelect, m_iCurSel, iOldSel);
        Invalidate();
    }
    if (Box::IsValidItemIndex(m_iCurSel) && (m_pWindow != nullptr) && !m_pWindow->IsClosingWnd()) {
        const Control* pControl = m_treeView.GetItemAt(m_iCurSel);
        if ((pControl != nullptr) && (m_pWindow->GetEventClick() == pControl)) {
            //If the selection is triggered by a mouse click, close the drop-down list
            m_pWindow->CloseComboWnd(false, false);
        }
    }
    return true;
}

void Combo::OnComboWndClosed(bool bCanceled, bool needUpdateSelItem, const DString& oldEditText)
{
    if (bCanceled) {
        size_t iOldSel = m_iCurSel;
        m_iCurSel = m_treeView.GetCurSel();
        if (m_iCurSel != iOldSel) {
            SendEvent(kEventSelect, m_iCurSel, iOldSel);
            Invalidate();
        }
        SetText(oldEditText);
    }
    if (needUpdateSelItem) {
        OnSelectedItemChanged();
    }
    SendEvent(kEventWindowClose);
    Invalidate();
}

bool Combo::OnButtonDown(const EventArgs& /*args*/)
{
    m_bDropListShown = (m_pWindow != nullptr) ? true : false;
    if (m_pWindow != nullptr) {
        //If the drop-down box is being displayed, close it when clicked again
        HideComboList();
    }
    return true;
}

bool Combo::OnButtonClicked(const EventArgs& /*args*/)
{
    //If the list is being displayed when the mouse button is pressed, do not show the drop-down list after the click
    if (!m_bDropListShown) {        
        ShowComboList();
    }
    if (m_comboType == kCombo_DropDown) {
        if (m_pEditControl != nullptr) {
            m_pEditControl->SetFocus();
        }
    }
    return true;
}

bool Combo::OnEditButtonDown(const EventArgs& /*args*/)
{
    if (m_comboType == kCombo_DropList) {
        m_bDropListShown = (m_pWindow != nullptr) ? true : false;
    }
    return true;
}

bool Combo::OnEditButtonUp(const EventArgs& /*args*/)
{
    if (m_comboType == kCombo_DropList) {
        if (m_bDropListShown) {
            //If the list is being displayed when the mouse button is pressed, do not show the drop-down list after the click
            return true;
        }
        //Show the drop-down list
        ShowComboList();
    }
    return true;
}

bool Combo::OnEditKeyDown(const EventArgs& args)
{
    if (m_comboType == kCombo_DropList) {
        return true;
    }
    if (args.wParam == kVK_DOWN) {
        if ((m_pWindow == nullptr) || m_pWindow->IsClosingWnd()) {
            //When the down arrow is pressed: if the drop-down list is not displayed, show it
            ShowComboList();
        }
        else {
            //When the down arrow is pressed: if it is already displayed, switch the selection
            const size_t itemCount = GetCount();
            const size_t nCurSel = m_treeView.GetCurSel();
            if (!Box::IsValidItemIndex(nCurSel)) {
                //If nothing is selected, start by selecting the first item
                if (itemCount > 0) {
                    m_treeView.SelectItem(0, false, true);
                }
            }
            else {                
                if ((itemCount > 0) && (nCurSel < itemCount)) {
                    if (nCurSel == (itemCount - 1)) {
                        //If it is already the last item, cancel the selection
                        m_treeView.SelectItem(Box::InvalidIndex, false, true);
                    }
                    else {
                        //If it is not the last item, select the next item
                        m_treeView.SelectItem(nCurSel + 1, false, true);
                    }
                }
            }
        }
    }
    else if (args.wParam == kVK_UP) {
        //Press the up arrow
        if ((m_pWindow != nullptr) && !m_pWindow->IsClosingWnd()) {
            const size_t itemCount = GetCount();
            const size_t nCurSel = m_treeView.GetCurSel();
            if (Box::IsValidItemIndex(nCurSel)) {
                if (nCurSel == 0) {
                    //It is already the first item, cancel the selection
                    m_treeView.SelectItem(Box::InvalidIndex, false, true);
                }
                else {
                    //It is not the first item, select the previous item
                    m_treeView.SelectItem(nCurSel - 1, false, true);
                }
            }
            else {
                //If nothing is selected, select the last item
                m_treeView.SelectItem(itemCount - 1, false, true);
            }
        }
    }
    else if (args.wParam == kVK_ESCAPE) {
        //Press and hold the ESC key to cancel
        if (m_pWindow != nullptr) {
            m_pWindow->CloseComboWnd(true, false);
        }
    }
    else if (args.wParam == kVK_RETURN) {
        //Press the Enter key to close the window normally
        if (m_pWindow != nullptr) {
            m_pWindow->CloseComboWnd(false, false);
        }
    }
    return true;
}

bool Combo::OnEditSetFocus(const EventArgs& /*args*/)
{
    //Use the focus state of the RichEdit control as the focus state of the Combo
    SendEvent(kEventSetFocus);
    return true;
}

bool Combo::OnEditKillFocus(const EventArgs& /*args*/)
{
    if (m_pWindow != nullptr) {
        if (m_pWindow->IsWindowFocused()) {
            return true;
        }
    }
    HideComboList();
    Invalidate();
    //Use the focus state of the RichEdit control as the focus state of the Combo
    SendEvent(kEventKillFocus);
    return true;
}

bool Combo::OnWindowKillFocus(const EventArgs& /*args*/)
{
    if (m_pWindow != nullptr) {
        if (m_pWindow->IsWindowFocused()) {
            return true;
        }
    }
    HideComboList();
    return true;
}

bool Combo::OnWindowMove(const EventArgs& /*args*/)
{
    UpdateComboList();
    return true;
}

void Combo::OnSelectedItemChanged()
{
    if (m_pEditControl != nullptr) {
        size_t nSelIndex = GetCurSel();
        if (Box::IsValidItemIndex(nSelIndex)) {
            m_pEditControl->SetTextNoEvent(GetItemText(nSelIndex));
        }
        else {
            m_pEditControl->SetTextNoEvent(DString());
        }
    }
}

void Combo::OnLanguageChanged()
{
    //The language has changed
    ComboType comboType = GetComboType();
    if (comboType == ComboType::kCombo_DropList) {
        //When not editable, directly overwrite the edit box
        OnSelectedItemChanged();
    }
    else {
        //When editable, also directly overwrite the edit box (consider changing this when a better strategy becomes available)
        if (m_pEditControl != nullptr) {
            size_t nSelIndex = GetCurSel();
            if (Box::IsValidItemIndex(nSelIndex)) {
                m_pEditControl->SetTextNoEvent(GetItemText(nSelIndex));
            }
        }
    }
}

bool Combo::OnEditTextChanged(const ui::EventArgs& /*args*/)
{
    if ((m_pWindow != nullptr) && !m_pWindow->IsClosingWnd()) {
        DString editText = GetText();
        //Convert to lowercase so that the comparison is case-insensitive
        editText = StringUtil::MakeLowerString(editText);
        size_t itemCount = m_treeView.GetItemCount();
        for (size_t iIndex = 0; iIndex < itemCount; ++iIndex) {
            Control* pControl = m_treeView.GetItemAt(iIndex);
            if (pControl != nullptr) {
                TreeNode* pTreeNode = dynamic_cast<TreeNode*>(pControl);
                ASSERT(pTreeNode != nullptr);
                if (pTreeNode != nullptr) {
                    pTreeNode->SetExpand(true, false);
                    DString nodeText = StringUtil::MakeLowerString(pTreeNode->GetText());
                    if (nodeText.find(editText) != DString::npos) {
                        m_treeView.EnsureVisible(iIndex, ListBoxVerVisible::kVisibleAtCenter);
                        break;
                    }
                }
            }
        }
    }
    return true;
}

void Combo::ShowComboList()
{
    //Show the drop-down list
    if ((m_pWindow == nullptr) || m_pWindow->IsClosingWnd()) {
        m_pWindow = new CComboWnd();
        if (m_comboType == kCombo_DropList) {
            m_pWindow->InitComboWnd(this, true);
        }
        else {
            m_pWindow->InitComboWnd(this, false);
        }        
    }
}

void Combo::HideComboList()
{
    if(m_pWindow != nullptr) {
        m_pWindow->CloseComboWnd(false, false);
    }
}

void Combo::UpdateComboList()
{
    if (m_pWindow != nullptr) {
        m_pWindow->UpdateComboWnd();
    }
}

void Combo::UpdateComboWndPos()
{
    if (m_pWindow != nullptr) {
        m_pWindow->UpdateComboWnd();
    }
}

Window* Combo::GetComboWnd() const
{
    return m_pWindow;
}

void Combo::AttachMouseEvents(Control* pControl)
{
    if (pControl == nullptr) {
        return;
    }
    auto SetRichEditFocus = [this]() {
        if ((m_pEditControl != nullptr) && m_pEditControl->IsVisible()) {
            if (!m_pEditControl->IsFocused()) {
                m_pEditControl->SetFocus();
            }
        }
        };
    pControl->AttachButtonDown([SetRichEditFocus](const EventArgs&) {
        SetRichEditFocus();
        return true;
        });
    pControl->AttachRButtonDown([SetRichEditFocus](const EventArgs&) {
        SetRichEditFocus();
        return true;
        });
}

void Combo::SetFocus()
{
    //Set the focus to the Edit control
    if (IsNoFocus()) {
        if ((m_pEditControl != nullptr) && m_pEditControl->IsVisible()) {
            if (!m_pEditControl->IsFocused()) {
                m_pEditControl->SetFocus();
            }
        }
    }
    else {
        BaseClass::SetFocus();
    }
}

} // namespace ui
