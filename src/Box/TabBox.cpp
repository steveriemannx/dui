#include "duilib/Box/TabBox.h"
#include "duilib/Animation/AnimationPlayer.h"
#include "duilib/Core/Window.h"
#include "duilib/Utils/StringUtil.h"

namespace ui
{

TabBox::TabBox(Window* pWindow, Layout* pLayout):
    Box(pWindow, pLayout),
    m_nCurSel(Box::InvalidIndex),
    m_nInitSel(Box::InvalidIndex),
    m_fadeSwithType(FadeSwitchType::kFadeInOut),
    m_frameIntervalMillSeconds(1000 / 60),
    m_totalMillSeconds(200),
    m_easingFunctionType(EasingFunctionType::EaseOutCubic)
{
}

DString TabBox::GetType() const { return DUI_CTR_TABBOX; }

void TabBox::SetAttribute(const DString& strName, const DString& strValue)
{
    if ((strName == _T("selected_id")) || (strName == _T("selectedid"))) {
        size_t iSel = (size_t)StringUtil::StringToInt32(strValue);
        if (IsInited()) {
            SelectItem(iSel);
        }
        else {
            m_nInitSel = iSel;
        }
    }
    else if ((strName == _T("fade_switch")) || (strName == _T("fadeswitch"))) {
        if (strValue == _T("true")) {
            SetFadeSwitch(true);
        }
        else if (strValue == _T("false")) {
            SetFadeSwitch(false);
        }
        else {
            SetFadeSwitchTypeByString(strValue);
        }
    }
    else if (strName == _T("fade_switch_type")) {
        SetFadeSwitchTypeByString(strValue);
    }
    else if (strName == _T("fade_switch_frame_interval_ms")) {
        SetFadeSwitchFrameIntervalMillSeconds(StringUtil::StringToInt32(strValue));
    }
    else if (strName == _T("fade_switch_total_ms")) {
        SetFadeSwitchTotalMillSeconds(StringUtil::StringToInt32(strValue));
    }
    else if (strName == _T("fade_switch_easing_function")) {
        SetFadeSwitchEasingFunctionType(EasingFunctions::GetEasingFunctionType(strValue));
    }
    else {
        Box::SetAttribute(strName, strValue);
    }
}

void TabBox::OnSetVisible(bool bChanged)
{
    BaseClass::OnSetVisible(bChanged);

    if (!IsInited() || !IsVisible()) {
        //No processing when not initialized or hidden
        return;
    }

    //When visible, only one page can be shown, other pages need to be hidden
    size_t nCurSel = GetCurSel();
    bool bSelected = false;
    if (Box::IsValidItemIndex(nCurSel)) {
        bSelected = DoSelectItem(nCurSel, false, false);
    }
    if (!bSelected && (GetItemCount() > 0) && (GetCurSel() > GetItemCount())) {
        //Select the first page by default, to avoid all pages being shown at the same time
        DoSelectItem(0, false, false);
    }
}

void TabBox::OnInit()
{
    if (IsInited()) {
        return;
    }
    BaseClass::OnInit();
    bool bSelected = false;
    if (Box::IsValidItemIndex(m_nInitSel)) {
        bSelected = DoSelectItem(m_nInitSel, false, true);
    }

    if (!bSelected && (GetItemCount() > 0) && (m_nCurSel > GetItemCount())) {
        //During initialization, select the first page by default, to avoid all pages being shown at the same time
        DoSelectItem(0, false, true);
    }
}

bool TabBox::AddItem(Control* pControl)
{
    return AddItemAt(pControl, GetItemCount());
}

bool TabBox::AddItemAt(Control* pControl, size_t iIndex)
{
    ASSERT(pControl != nullptr);
    if (pControl == nullptr) {
        return false;
    }
    bool ret = Box::AddItemAt(pControl, iIndex);
    if (!ret) {
        return ret;
    }        

    if(!Box::IsValidItemIndex(m_nCurSel) && pControl->IsVisible()) {
        //If there is no currently selected page, select the newly inserted page
        size_t iCurSel = GetItemIndex(pControl);
        ASSERT(iCurSel == iIndex);
        ret = SelectItem(iCurSel);
    }
    else if(Box::IsValidItemIndex(m_nCurSel) && (iIndex <= m_nCurSel)) {
        //Insert a new page before the current selected page
        m_nCurSel += 1;
    }

    if (m_nCurSel != iIndex) {
        //If it is not the selected page, hide it
        pControl->SetVisible(false);
        OnHideTabItem(iIndex);
    }
    return ret;
}

bool TabBox::RemoveItem(Control* pControl)
{
    if (pControl == nullptr) {
        return false;
    }

    size_t index = GetItemIndex(pControl);
    ASSERT(Box::IsValidItemIndex(index));
    bool ret = Box::RemoveItem(pControl);
    if (!ret) {
        return false;
    }

    if(m_nCurSel == index)    {
        if (GetItemCount() > 0) {
            //After removing the currently selected TAB page, select the TAB page before the removed page
            size_t newSel = m_nCurSel > 0 ? m_nCurSel - 1 : 0;
            if (newSel >= GetItemCount()) {
                newSel = Box::InvalidIndex;
            }
            m_nCurSel = Box::InvalidIndex;
            ret = DoSelectItem(newSel, false, true);
        }
        else {
            //There is only one TAB page currently; after it is removed, update the selection to no selection
            m_nCurSel = Box::InvalidIndex;
        }
        ArrangeAncestor();
    }
    else if(m_nCurSel > index ) {
        //Update the index of the current selected page so that it remains unchanged
        m_nCurSel -= 1;
    }
    return ret;
}

bool TabBox::RemoveItemAt(size_t iIndex)
{
    Control* pControl = GetItemAt(iIndex);
    if (pControl == nullptr) {
        return false;
    }
    return RemoveItem(pControl);
}

void TabBox::RemoveAllItems()
{
    m_nCurSel = Box::InvalidIndex;
    Box::RemoveAllItems();
    ArrangeAncestor();
}

size_t TabBox::GetCurSel() const
{
    return m_nCurSel;
}
    
bool TabBox::DoSelectItem(size_t nItemIndex, bool bFadeSwith, bool bCheckChanged)
{
    ASSERT(nItemIndex < m_items.size());
    if (!Box::IsValidItemIndex(nItemIndex) || (nItemIndex >= m_items.size())) {
        return false;
    }
    if (bCheckChanged) {
        if (nItemIndex == m_nCurSel) {
            return true;
        }
    }
    if (nItemIndex == m_nCurSel) {
        //If the selected item has not changed, do not enable the switch animation
        bFadeSwith = false;
    }
    if (m_nCurSel >= m_items.size()) {
        //There is no old selected item, so do not enable the switch animation
        bFadeSwith = false;
    }

    const size_t nOldSelIndex = m_nCurSel;
    Control* pNewItemControl = nullptr;
    Control* pOldItemControl = nullptr;

    m_nCurSel = nItemIndex;
    const size_t itemCount = m_items.size();
    for(size_t it = 0; it < itemCount; ++it ){
        Control* pItemControl = m_items.at(it);
        ASSERT(pItemControl != nullptr);
        if (it == nItemIndex) {
            //The currently selected TAB item
            OnShowTabItem(it);
            if (bFadeSwith) {
                // Trigger the animation effect
                pNewItemControl = pItemControl;
            }
            else {
                pItemControl->SetFadeVisible(true);
            }
        }
        else {
            //Not the currently selected TAB page
            OnHideTabItem(it);
            if ((it == nOldSelIndex) && bFadeSwith) {
                // Trigger the animation effect
                pOldItemControl = pItemControl;
            }
            else {                
                pItemControl->SetFadeVisible(false);
            }
        }
    }

    //Trigger the tab switch animation
    bool bSwitchResult = false;
    if (bFadeSwith && (pNewItemControl != nullptr) && (pOldItemControl != nullptr)) {
        bSwitchResult = StartSwitchItemAnimation(pNewItemControl, pOldItemControl);
    }
    if (!bSwitchResult) {
        if (pOldItemControl != nullptr) {
            pOldItemControl->SetFadeVisible(false);
        }
        if (pNewItemControl != nullptr) {
            pNewItemControl->SetFadeVisible(true);
        }
    }
    if (bCheckChanged && (nOldSelIndex != m_nCurSel)) {
        SendEvent(kEventTabSelect, m_nCurSel, nOldSelIndex);
    }    
    return true;
}

void TabBox::OnHideTabItem(size_t index)
{
    ASSERT(index < m_items.size());
    if (index >= m_items.size()) {
        return;
    }
    Control* pContol = m_items.at(index);
    ASSERT(pContol != nullptr);
    if (pContol == nullptr) {
        return;
    }
    pContol->SetMouseEnabled(false);
    Box* pBox = dynamic_cast<Box*>(pContol);
    if (pBox != nullptr) {
        pBox->SetMouseChildEnabled(false);
    }
}

void TabBox::OnShowTabItem(size_t index)
{
    ASSERT(index < m_items.size());
    if (index >= m_items.size()) {
        return;
    }
    Control* pContol = m_items.at(index);
    ASSERT(pContol != nullptr);
    if (pContol == nullptr) {
        return;
    }
    pContol->SetMouseEnabled(true);
    Box* pBox = dynamic_cast<Box*>(pContol);
    if (pBox != nullptr) {
        pBox->SetMouseChildEnabled(true);
    }
}

bool TabBox::SelectItem(size_t iIndex)
{
    return DoSelectItem(iIndex, IsFadeSwitch(), true);
}

bool TabBox::SelectItem(Control* pControl)
{
    size_t iIndex = GetItemIndex(pControl);
    if (!Box::IsValidItemIndex(iIndex)) {
        return false;
    }
    else {
        return SelectItem(iIndex);
    }
}

bool TabBox::SelectItem(const DString& pControlName)
{
    Control* pControl = FindSubControl(pControlName);
    ASSERT(pControl != nullptr);
    return SelectItem(pControl);
}

void TabBox::SetFadeSwitch(bool bFadeSwitch)
{
    if (bFadeSwitch) {
        if (m_fadeSwithType == FadeSwitchType::kNone) {
            //The default animation effect is fade in and out
            m_fadeSwithType = FadeSwitchType::kFadeInOut;
        }
    }
    else {
        m_fadeSwithType = FadeSwitchType::kNone;
    }
}

bool TabBox::IsFadeSwitch() const
{
    return m_fadeSwithType != FadeSwitchType::kNone;
}

void TabBox::SetFadeSwitchType(FadeSwitchType fadeSwitchType)
{
    m_fadeSwithType = fadeSwitchType;
}

TabBox::FadeSwitchType TabBox::GetFadeSwitchType() const
{
    return m_fadeSwithType;
}

void TabBox::SetFadeSwitchTypeByString(const DString& fadeSwitchType)
{
    if (fadeSwitchType == _T("FadeInOutX")) {
        SetFadeSwitchType(FadeSwitchType::kFadeInOutX);
    }
    else if (fadeSwitchType == _T("FadeInOut")) {
        SetFadeSwitchType(FadeSwitchType::kFadeInOut);
    }
}

void TabBox::SetFadeSwitchFrameIntervalMillSeconds(int32_t frameIntervalMillSeconds)
{
    m_frameIntervalMillSeconds = frameIntervalMillSeconds;
}

int32_t TabBox::GetFadeSwitchFrameIntervalMillSeconds() const
{
    return m_frameIntervalMillSeconds;
}

void TabBox::SetFadeSwitchTotalMillSeconds(int32_t totalMillSeconds)
{
    m_totalMillSeconds = totalMillSeconds;
}

int32_t TabBox::GetFadeSwitchTotalMillSeconds() const
{
    return m_totalMillSeconds;
}

void TabBox::SetFadeSwitchEasingFunctionType(EasingFunctionType easingFunctionType)
{
    m_easingFunctionType = easingFunctionType;
}

EasingFunctionType TabBox::GetFadeSwitchEasingFunctionType() const
{
    return m_easingFunctionType;
}

bool TabBox::StartSwitchItemAnimation(Control* pNewItemControl, Control* pOldItemControl)
{
    switch (GetFadeSwitchType()) {
    case FadeSwitchType::kFadeInOutX:
        return StartSwitchItemAnimationFadeInOutX(pNewItemControl, pOldItemControl);
    case FadeSwitchType::kFadeInOut:
        return StartSwitchItemAnimationFadeInOut(pNewItemControl, pOldItemControl);
    default:
        break;
    }
    return false;
}

bool TabBox::StartSwitchItemAnimationFadeInOutX(Control* pNewItemControl, Control* pOldItemControl)
{
    //Dynamically switch the X coordinate, the content area slides horizontally
    if ((pNewItemControl == nullptr) || (pOldItemControl == nullptr)) {
        return false;
    }
    size_t nNewSelIndex = GetItemIndex(pNewItemControl);
    size_t nOldSelIndex = GetItemIndex(pOldItemControl);
    if ((nNewSelIndex == Box::InvalidIndex) || (nOldSelIndex == Box::InvalidIndex)) {
        return false;
    }

    //Stop the old animation playback first
    if ((m_pAnimationPlayer != nullptr) && m_pAnimationPlayer->IsPlaying()) {
        AnimationCompleteCallback completeCallback = m_pAnimationPlayer->GetCompleteCallback();
        if (completeCallback != nullptr) {
            completeCallback();
        }
        m_pAnimationPlayer->SetCompleteCallback(nullptr);
        m_pAnimationPlayer->Stop();        
    }
    AnimationPlayer* pAnimationPlayer = new AnimationPlayer;
    m_pAnimationPlayer.reset(pAnimationPlayer);

    pAnimationPlayer->SetAnimationType(AnimationType::kAnimationNone);
    pAnimationPlayer->SetTotalMillSeconds(GetFadeSwitchTotalMillSeconds());
    pAnimationPlayer->SetFrameIntervalMillSeconds(GetFadeSwitchFrameIntervalMillSeconds());
    pAnimationPlayer->SetEasingFunctionType(GetFadeSwitchEasingFunctionType());

    //Start value and end value
    const int32_t nMaxValue = 255;
    pAnimationPlayer->SetStartValue(0);
    pAnimationPlayer->SetEndValue(nMaxValue);

    ControlPtrT<TabBox> pTabBox(this);
    ControlPtr pNewItem(pNewItemControl);
    ControlPtr pOldItem(pOldItemControl);
    bool bRightToLeft = nNewSelIndex > nOldSelIndex; //Whether to slide from right to left

    AnimationPlayCallback playCallback = [pTabBox, pNewItem, pOldItem, nMaxValue, bRightToLeft](int32_t nNewValue) {
            if ((pTabBox == nullptr) || (pNewItem == nullptr) || (pOldItem == nullptr)) {
                return;
            }
            //Make sure both pages are visible
            if (!pOldItem->IsVisible()) {
                pOldItem->SetVisible(true);
            }
            if (!pNewItem->IsVisible()) {
                pNewItem->SetVisible(true);
            }

            //Calculate the current offset and update it
            const int32_t nBoxWidth = pTabBox->GetPos().Width(); //Total width
            const int32_t nBoxItemOffset = static_cast<int32_t>((int64_t)nBoxWidth * nNewValue / nMaxValue);
            if (bRightToLeft) {
                pNewItem->SetRenderOffsetX(-(nBoxWidth - nBoxItemOffset));
                pOldItem->SetRenderOffsetX(nBoxItemOffset);
            }
            else {
                pNewItem->SetRenderOffsetX(nBoxWidth - nBoxItemOffset);
                pOldItem->SetRenderOffsetX(-nBoxItemOffset);
            }
        };
    pAnimationPlayer->SetPlayCallback(playCallback);

    AnimationCompleteCallback compelteCallback = [pTabBox, pNewItem, pOldItem]() {
            //Animation ended
            if ((pTabBox != nullptr) && (pOldItem != nullptr)) {
                size_t itemIndex = pTabBox->GetItemIndex(pOldItem.get());
                if (itemIndex < pTabBox->GetItemCount()) {
                    pOldItem->SetRenderOffsetX(0);
                    pOldItem->SetRenderOffsetY(0);
                    if (pTabBox->GetCurSel() != itemIndex) {
                        pOldItem->SetVisible(false);
                    }
                }
            }
            if ((pTabBox != nullptr) && (pNewItem != nullptr)) {
                size_t itemIndex = pTabBox->GetItemIndex(pNewItem.get());
                if (itemIndex < pTabBox->GetItemCount()) {
                    pNewItem->SetRenderOffsetX(0);
                    pNewItem->SetRenderOffsetY(0);
                    if (pTabBox->GetCurSel() == itemIndex) {
                        pNewItem->SetVisible(true);
                    }
                }
            }
        };
    pAnimationPlayer->SetCompleteCallback(compelteCallback);    
    pAnimationPlayer->Start();
    return true;
}

bool TabBox::StartSwitchItemAnimationFadeInOut(Control* pNewItemControl, Control* pOldItemControl)
{
    //The content area fades in and out
    if ((pNewItemControl == nullptr) || (pOldItemControl == nullptr)) {
        return false;
    }
    size_t nNewSelIndex = GetItemIndex(pNewItemControl);
    size_t nOldSelIndex = GetItemIndex(pOldItemControl);
    if ((nNewSelIndex == Box::InvalidIndex) || (nOldSelIndex == Box::InvalidIndex)) {
        return false;
    }

    //Stop the old animation playback first
    if ((m_pAnimationPlayer != nullptr) && m_pAnimationPlayer->IsPlaying()) {
        AnimationCompleteCallback completeCallback = m_pAnimationPlayer->GetCompleteCallback();
        if (completeCallback != nullptr) {
            completeCallback();
        }
        m_pAnimationPlayer->SetCompleteCallback(nullptr);
        m_pAnimationPlayer->Stop();
    }
    AnimationPlayer* pAnimationPlayer = new AnimationPlayer;
    m_pAnimationPlayer.reset(pAnimationPlayer);

    pAnimationPlayer->SetAnimationType(AnimationType::kAnimationNone);
    pAnimationPlayer->SetTotalMillSeconds(GetFadeSwitchTotalMillSeconds());
    pAnimationPlayer->SetFrameIntervalMillSeconds(GetFadeSwitchFrameIntervalMillSeconds());
    pAnimationPlayer->SetEasingFunctionType(GetFadeSwitchEasingFunctionType());

    //Start value and end value
    const int32_t nMaxValue = 255;
    pAnimationPlayer->SetStartValue(0);
    pAnimationPlayer->SetEndValue(nMaxValue);

    ControlPtrT<TabBox> pTabBox(this);
    ControlPtr pNewItem(pNewItemControl);
    ControlPtr pOldItem(pOldItemControl);

    uint8_t nNewItemAlpha = pNewItem->GetAlpha();
    uint8_t nOldItemAlpha = pOldItem->GetAlpha();

    //The newly selected tab page is painted last, to avoid being covered by the old page
    pNewItem->SetPaintOrder(1);

    AnimationPlayCallback playCallback = [pTabBox, pNewItem, pOldItem, nMaxValue, nNewItemAlpha, nOldItemAlpha](int32_t nNewValue) {
            if ((pTabBox == nullptr) || (pNewItem == nullptr) || (pOldItem == nullptr)) {
                return;
            }
            //Make sure both pages are visible
            if (!pOldItem->IsVisible()) {
                pOldItem->SetVisible(true);
            }
            if (!pNewItem->IsVisible()) {
                pNewItem->SetVisible(true);
            }
            if (nNewValue < 0) {
                nNewValue = 0;
            }
            if (nNewValue > nMaxValue) {
                nNewValue = nMaxValue;
            }

            //Calculate the current Alpha value and update it
            uint8_t nCurrentNewItemAlpha = static_cast<uint8_t>(nNewItemAlpha * nNewValue / nMaxValue);
            uint8_t nCurrentOldItemAlpha = static_cast<uint8_t>(nOldItemAlpha * (nMaxValue - nNewValue) / nMaxValue);

            pNewItem->SetAlpha(nCurrentNewItemAlpha);
            pOldItem->SetAlpha(nCurrentOldItemAlpha);
        };
    pAnimationPlayer->SetPlayCallback(playCallback);

    AnimationCompleteCallback compelteCallback = [pTabBox, pNewItem, pOldItem, nNewItemAlpha, nOldItemAlpha]() {
            //Animation ended
            if ((pTabBox != nullptr) && (pOldItem != nullptr)) {
                size_t itemIndex = pTabBox->GetItemIndex(pOldItem.get());
                if (itemIndex < pTabBox->GetItemCount()) {
                    pOldItem->SetRenderOffsetX(0);
                    pOldItem->SetRenderOffsetY(0);
                    pOldItem->SetAlpha(nOldItemAlpha);
                    if (pTabBox->GetCurSel() != itemIndex) {
                        pOldItem->SetVisible(false);
                    }
                }
            }
            if ((pTabBox != nullptr) && (pNewItem != nullptr)) {
                size_t itemIndex = pTabBox->GetItemIndex(pNewItem.get());
                if (itemIndex < pTabBox->GetItemCount()) {
                    pNewItem->SetRenderOffsetX(0);
                    pNewItem->SetRenderOffsetY(0);
                    pNewItem->SetAlpha(nNewItemAlpha);
                    pNewItem->SetPaintOrder(0); //Restore the paint order to normal painting
                    if (pTabBox->GetCurSel() == itemIndex) {
                        pNewItem->SetVisible(true);
                    }
                }
            }
        };
    pAnimationPlayer->SetCompleteCallback(compelteCallback);
    pAnimationPlayer->Start();
    return true;
}

} //namespace ui
