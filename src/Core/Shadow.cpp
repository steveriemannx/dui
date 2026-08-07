#include "dui/Core/Shadow.h"
#include "dui/Core/Box.h"
#include "dui/Core/Window.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Core/ClickThrough.h"
#include "dui/Render/IRender.h"
#include "dui/Render/AutoClip.h"

namespace ui 
{

class ShadowBox : public Box
{
    typedef Box BaseClass;
public:
    ShadowBox(Window* pWindow, Shadow* pShadow):
        Box(pWindow),
        m_pShadow(pShadow)
    {
        // Disable the control's own padding, otherwise the shadow cannot be drawn
        SetEnableControlPadding(false);
    }
    virtual DString GetType() const override { return _T("ShadowBox"); }

    // Draw the child controls inside the container
    virtual void PaintChild(IRender* pRender, const UiRect& rcPaint) override
    {
        UiRect rcTemp;
        if (!UiRect::Intersect(rcTemp, rcPaint, GetRect())) {
            return;
        }
        UiPadding rcPadding;
        if (m_pShadow != nullptr) {
            rcPadding = m_pShadow->GetCurrentShadowCorner();
        }
        UiRect rcRect = GetRect();
        rcRect.Deflate(rcPadding);
        // Set the client area clip region to avoid covering the shadow
        AutoClip rectClip(pRender, rcRect, true);

        UiSize borderRound;
        if (m_pShadow != nullptr) {
            borderRound = m_pShadow->GetShadowBorderRound();
        }
        if (GetWindow() != nullptr) {
            GetWindow()->Dpi().ScaleSize(borderRound);
        }
        float fRoundWidth = (float)borderRound.cx;
        float fRoundHeight = (float)borderRound.cy;
        bool bRoundClip = (borderRound.cx > 0) && (borderRound.cy > 0);
        if (rcPadding.IsEmpty()) {
            bRoundClip = false;
        }
        // Set the rounded client area clip region to avoid covering the shadow
        AutoClip roundClip(pRender, rcRect, fRoundWidth, fRoundHeight, bRoundClip);

        // Draw the child controls
        BaseClass::PaintChild(pRender, rcPaint);
    }

    /** Calculate the control size (width and height)
        If an image is set and either width or height is set to auto, the final size will be calculated based on the image size and the text size
     *  @param [in] szAvailable The available size, excluding the inner padding and the outer margin
     *  @return The estimated size of the control, including the inner padding (Box), excluding the outer margin
     */
    virtual UiEstSize EstimateSize(UiSize szAvailable) override
    {
        UiFixedSize fixedSize;
        UiEstSize returnEstSize;
        if (!PreEstimateSize(szAvailable, fixedSize, returnEstSize)) {
            return returnEstSize;
        }

        // The size of the child controls, including the inner padding, but excluding the outer margin; it includes the size of the shadow itself (i.e. the inner padding of the Box)
        UiSize64 layoutSize = GetLayout()->EstimateLayoutSize(m_items, szAvailable);
        UiSize sizeByChild(ui::TruncateToInt32(layoutSize.cx), ui::TruncateToInt32(layoutSize.cy));

        SetReEstimateSize(false);
        for (auto pControl : m_items) {
            ASSERT(pControl != nullptr);
            if ((pControl == nullptr) || !pControl->IsVisible() || pControl->IsFloat()) {
                continue;
            }
            if ((pControl->GetFixedWidth().IsAuto()) ||
                (pControl->GetFixedHeight().IsAuto())) {
                if (pControl->IsReEstimateSize(szAvailable)) {
                    SetReEstimateSize(true);
                    break;
                }
            }
        }
        if (fixedSize.cx.IsAuto()) {
            fixedSize.cx.SetInt32(sizeByChild.cx);
        }
        if (fixedSize.cy.IsAuto()) {
            fixedSize.cy.SetInt32(sizeByChild.cy);
        }

        UiEstSize estSize = MakeEstSize(fixedSize);
        SetEstimateSize(estSize, szAvailable);
        return estSize;
    }

    virtual void PaintBkImage(IRender* pRender) override
    {
        if (pRender == nullptr) {
            return;
        }
        if ((m_pShadow == nullptr) || !m_pShadow->IsShadowAttached()) {
            BaseClass::PaintBkImage(pRender);
        }
        else {
            Image* pBkImage = GetBkImagePtr();
            Window* pWindow = GetWindow();
            if ((pBkImage != nullptr) && (pWindow != nullptr)) {
                UiRect destRect = GetRect();
                if (m_pShadow->IsEnableShadowSnap()) {
                    UiPadding rcShadowCorner = pWindow->GetCurrentShadowCorner();
                    UiPadding rcRealCorner = m_pShadow->GetShadowCorner();
                    pWindow->Dpi().ScalePadding(rcRealCorner);

                    // When the window is snapped to the edge, the shadow needs to be stretched to the window edge
                    if (rcShadowCorner.top == 0) {
                        destRect.top -= rcRealCorner.top;
                    }
                    if (rcShadowCorner.left == 0) {
                        destRect.left -= rcRealCorner.left;
                    }
                    if (rcShadowCorner.right == 0) {
                        destRect.right += rcRealCorner.right;
                    }
                    if (rcShadowCorner.bottom == 0) {
                        destRect.bottom += rcRealCorner.bottom;
                    }
                }
                PaintImage(pRender, pBkImage, _T(""), DUI_NOSET_VALUE, nullptr, &destRect);
            }

            // Draw the border
            if (m_pShadow != nullptr) {
                Box* pXmlRoot = m_pShadow->GetAttachedXmlRoot();
                int32_t nShadowBorderSize = m_pShadow->GetShadowBorderSize();  // Border size (with the rectangle of the XmlRoot Box as the center line, one pixel on each side of the center line)
                DString shadowBorderColor = m_pShadow->GetShadowBorderColor(); // Border color (close to the default window border color of Win11)
                UiColor dwBorderColor;
                if (!shadowBorderColor.empty() && (nShadowBorderSize > 0)) {
                    dwBorderColor = GetUiColor(shadowBorderColor);
                }
                if ((pXmlRoot != nullptr) && (nShadowBorderSize > 0) && !dwBorderColor.IsEmpty()) {                 
                    float fBorderSize = Dpi().GetScaleFloat(nShadowBorderSize);
                    const UiSize borderRound = m_pShadow->GetShadowBorderRound();   // Corner radius size

                    float rx = Dpi().GetScaleFloat(borderRound.cx);
                    float ry = Dpi().GetScaleFloat(borderRound.cy);
                    
                    UiRect rcPos = pXmlRoot->GetPos();
                    if (!rcPos.IsEmpty()) {
                        UiRectF rcRoot((float)rcPos.left, (float)rcPos.top, (float)rcPos.right, (float)rcPos.bottom);

                        if ((borderRound.cy > 0) && (borderRound.cy > 0)) {
                            pRender->DrawRoundRect(rcRoot, rx, ry, dwBorderColor, fBorderSize);
                        }
                        else {
                            pRender->DrawRect(rcRoot, dwBorderColor, fBorderSize, false);
                        }
                    }
                }
            }
        }
    }

    /** Mouse click event, implementing the mouse click-through functionality
    */
    void OnMouseDown(UiPoint ptMouse)
    {
        UiPoint pt = ptMouse;
        // Determine whether the click is on the shadow
        if ((m_pShadow == nullptr) || !m_pShadow->IsShadowAttached()) {
            return;
        }
        Box* pXmlRoot = m_pShadow->GetAttachedXmlRoot();
        if (pXmlRoot == nullptr) {
            return;
        }
        UiRect rcRoot = pXmlRoot->GetPos();
        if (rcRoot.ContainsPt(pt)) {
            // The mouse is not on the shadow
            return;
        }

        ClientToScreen(pt);
        OnMouseClickShadow(pt);
    }

    /** Mouse click on the shadow
    * @param [in] ptMouse The point clicked by the mouse (screen coordinates)
    */
    void OnMouseClickShadow(UiPoint ptMouse) const
    {
        if (!m_pShadow->IsEnableClickThroughWindow()) {
            return;
        }
        ClickThrough shadowClick;
        shadowClick.ClickThroughWindow(GetWindow(), ptMouse);
    }

private:
    // The associated shadow control
    Shadow* m_pShadow;
};

Shadow::Shadow(Window* pWindow):
    m_bShadowAttached(true),
    m_bUseDefaultShadowAttached(true),
    m_isMaximized(false),
    m_pShadowBox(nullptr),
    m_pWindow(pWindow),
    m_bEnableClickThroughWindow(true),
    m_bEnableShadowSnap(true),
    m_bLeftSnap(false),
    m_bTopSnap(false),
    m_bRightSnap(false),
    m_bBottomSnap(false),
    m_nShadowBorderSize(2),
    m_shadowBorderColor(_T("#FFA3A3A3"))
{
    SetShadowType(Shadow::ShadowType::kShadowDefault);
}

bool Shadow::IsUseDefaultShadowAttached() const
{
    return m_bUseDefaultShadowAttached;
}

void Shadow::SetUseDefaultShadowAttached(bool bDefault)
{
    m_bUseDefaultShadowAttached = bDefault;
}

Box* Shadow::AttachShadow(Box* pXmlRoot)
{
    if (!m_bShadowAttached) {
        return pXmlRoot;
    }
    //System shadows are provided by the OS (macOS NSWindow / Windows DWM):
    //no self-drawn ShadowBox is needed (matches the reference nim_duilib).
    if (IsSystemShadowEnabled()) {
        return pXmlRoot;
    }
    ASSERT(m_pShadowBox == nullptr);
    if (m_pShadowBox != nullptr) {
        return pXmlRoot;
    }

    if (pXmlRoot == nullptr) {
        return nullptr;
    }

    m_pShadowBox = new ShadowBox(pXmlRoot->GetWindow(), this);
    m_pShadowBox->SetMouseEnabled(false);    // The shadow container does not receive mouse messages
    m_pShadowBox->SetNoFocus();              // The shadow container does not get the focus
    m_pShadowBox->SetKeyboardEnabled(false); // The shadow container does not receive keyboard messages
    m_pShadowBox->AddItem(pXmlRoot);
    DoAttachShadow(m_pShadowBox, pXmlRoot, true, m_isMaximized);
    return m_pShadowBox;
}

Box* Shadow::GetShadowBox() const
{
    return m_pShadowBox;
}

Box* Shadow::GetAttachedXmlRoot() const
{
    Box* pShadowRoot = GetShadowBox();
    if (pShadowRoot == nullptr) {
        return nullptr;
    }
    Box* pXmlRoot = nullptr;
    if (pShadowRoot->GetItemCount() > 0) {
        pXmlRoot = dynamic_cast<Box*>(pShadowRoot->GetItemAt(0));
    }
    return pXmlRoot;
}

bool Shadow::HasShadowBox() const
{
    return m_pShadowBox != nullptr;
}

void Shadow::DoAttachShadow(Box* pNewRoot, Box* pOrgRoot, bool bNewAttach, bool isMaximized) const
{
    // Implementation logic: update as needed
    ASSERT((pNewRoot != nullptr) && (pOrgRoot != nullptr));
    if ((pNewRoot == nullptr) || (pOrgRoot == nullptr)) {
        return;
    }
    const UiPadding rcShadowCorner = GetCurrentShadowCorner();
    if (bNewAttach && !isMaximized) {
        // Attach and the window is not in the maximized state
        pNewRoot->SetPadding(rcShadowCorner, false);
    }
    else {
        // When detaching or when the window is in the maximized state
        pNewRoot->SetPadding(UiPadding(0, 0, 0, 0), false);
    }
    if (pOrgRoot->GetFixedWidth().IsInt32()) {
        int32_t rootWidth = pOrgRoot->GetFixedWidth().GetInt32();
        if (bNewAttach) {
            rootWidth += (rcShadowCorner.left + rcShadowCorner.right);
        }
        pNewRoot->SetFixedWidth(UiFixedInt(rootWidth), true, false);
    }
    else {
        pNewRoot->SetFixedWidth(pOrgRoot->GetFixedWidth(), true, false);
    }
    if (pOrgRoot->GetFixedHeight().IsInt32()) {
        int32_t rootHeight = pOrgRoot->GetFixedHeight().GetInt32();
        if (bNewAttach) {
            rootHeight += (rcShadowCorner.top + rcShadowCorner.bottom);
        }       
        pNewRoot->SetFixedHeight(UiFixedInt(rootHeight), true, false);
    }
    else {
        pNewRoot->SetFixedHeight(pOrgRoot->GetFixedHeight(), true, false);
    }
    pNewRoot->SetBkImage(bNewAttach ? m_shadowImage : DString());
}

void Shadow::SetShadowAttached(bool bShadowAttached)
{
    m_bShadowAttached = bShadowAttached;
    // After being set externally, update to a non-default value
    m_bUseDefaultShadowAttached = false;
    OnShadowAttached(GetShadowType());
}

bool Shadow::IsShadowAttached() const
{ 
    return m_bShadowAttached;
}

bool Shadow::IsSystemShadowType(ShadowType nShadowType)
{
    return (nShadowType == ShadowType::kShadowSystemDefault) ||
           (nShadowType == ShadowType::kShadowSystemDoNotRound) ||
           (nShadowType == ShadowType::kShadowSystemRound) ||
           (nShadowType == ShadowType::kShadowSystemSmallRound);
}

bool Shadow::IsShadowTypeNeedLayeredWindow(ShadowType nShadowType)
{
    if (IsSystemShadowType(nShadowType)) {
        return false;
    }
    if (nShadowType == ShadowType::kShadowNone) {
        return false;
    }
    return true;
}

bool Shadow::IsSystemShadowEnabled() const
{
    return IsSystemShadowEnabled(m_nShadowType);
}

bool Shadow::IsSystemShadowEnabled(ShadowType nShadowType) const
{
    return IsSystemShadowType(nShadowType);
}

Shadow::ShadowType Shadow::GetDefaultShadowType(const Window* pWindow)
{
    if (pWindow != nullptr) {
        if (pWindow->IsLayeredWindow()) {
            //Layered window: self-drawn shadow with rounded corners
            return ShadowType::kShadowBigRound;
        }
        else if (pWindow->NativeWnd()->IsSystemShadowSupported()) {
            //The platform supports OS-provided shadows
            return ShadowType::kShadowSystemDefault;
        }
    }
#ifdef DUI_BUILD_FOR_WIN
    if (pWindow != nullptr && pWindow->NativeWnd()->IsSystemShadowSupported()) {
        return ShadowType::kShadowSystemDefault;
    }
    return ShadowType::kShadowBigRound;
#elif defined(DUI_BUILD_FOR_MACOS)
    //macOS supports OS-provided shadows
    return ShadowType::kShadowSystemDefault;
#else
    //Other platforms: self-drawn shadow with rounded corners
    return ShadowType::kShadowBigRound;
#endif
}

Shadow::ShadowType Shadow::GetSupportedShadowType(const Window* pWindow, ShadowType nShadowType)
{
    if (nShadowType == ShadowType::kShadowDefault) {
        nShadowType = GetDefaultShadowType(pWindow);
    }
    if (pWindow == nullptr) {
        return nShadowType;
    }
    if (!pWindow->NativeWnd()->IsSystemShadowSupported()) {
        //OS shadows unsupported: fall back to self-drawn shadows
        if (IsSystemShadowType(nShadowType)) {
            if (nShadowType == ShadowType::kShadowSystemDefault) {
                nShadowType = ShadowType::kShadowBigRound;
            }
            else if (nShadowType == ShadowType::kShadowSystemDoNotRound) {
                nShadowType = ShadowType::kShadowNone;
            }
            else if (nShadowType == ShadowType::kShadowSystemRound) {
                nShadowType = ShadowType::kShadowBigRound;
            }
            else if (nShadowType == ShadowType::kShadowSystemSmallRound) {
                nShadowType = ShadowType::kShadowSmallRound;
            }
        }
    }
    return nShadowType;
}

void Shadow::SetShadowType(Shadow::ShadowType nShadowType)
{
    ASSERT(nShadowType >= Shadow::ShadowType::kShadowFirst);
    ASSERT(nShadowType < Shadow::ShadowType::kShadowCount);
    if ((nShadowType >= Shadow::ShadowType::kShadowFirst) &&
        (nShadowType < Shadow::ShadowType::kShadowCount)) {
        m_nShadowType = nShadowType;
    }
    else {
        return;
    }

    // Enable the shadow
    m_bShadowAttached = true;

    // After being set externally, update to a non-default value
    m_bUseDefaultShadowAttached = false;

    OnShadowAttached(GetShadowType());
}

Shadow::ShadowType Shadow::GetShadowType() const
{
    return m_nShadowType;
}

bool Shadow::GetShadowType(const DString& typeString, ShadowType& nShadowType)
{
    if (typeString == _T("big")) {
        nShadowType = Shadow::ShadowType::kShadowBig;
    }
    else if (typeString == _T("big_round")) {
        nShadowType = Shadow::ShadowType::kShadowBigRound;
    }
    else if (typeString == _T("small")) {
        nShadowType = Shadow::ShadowType::kShadowSmall;
    }
    else if (typeString == _T("small_round")) {
        nShadowType = Shadow::ShadowType::kShadowSmallRound;
    }
    else if (typeString == _T("menu")) {
        nShadowType = Shadow::ShadowType::kShadowMenu;
    }
    else if (typeString == _T("menu_round")) {
        nShadowType = Shadow::ShadowType::kShadowMenuRound;
    }
    else if (typeString == _T("none")) {
        nShadowType = Shadow::ShadowType::kShadowNone;
    }
    else if (typeString == _T("none_round")) {
        nShadowType = Shadow::ShadowType::kShadowNoneRound;
    }
    else if (typeString == _T("custom")) {
        nShadowType = Shadow::ShadowType::kShadowCustom;
    }
    else if (typeString == _T("default")) {
        nShadowType = Shadow::ShadowType::kShadowDefault;
    }
    else if (typeString == _T("system_default")) {
        nShadowType = Shadow::ShadowType::kShadowSystemDefault;
    }
    else if (typeString == _T("system_not_round")) {
        nShadowType = Shadow::ShadowType::kShadowSystemDoNotRound;
    }
    else if (typeString == _T("system_round")) {
        nShadowType = Shadow::ShadowType::kShadowSystemRound;
    }
    else if (typeString == _T("system_small_round")) {
        nShadowType = Shadow::ShadowType::kShadowSystemSmallRound;
    }
    else {
        ASSERT(0);
        return false;
    }
    return true;
}

bool Shadow::GetShadowParam(ShadowType nShadowType,
                            UiSize& szBorderRound,
                            UiPadding& rcShadowCorner,
                            DString& shadowImage,
                            Shadow* pShadowObj)
{
    bool bRet = false;
    if (nShadowType == Shadow::ShadowType::kShadowBig) {
        bRet = true;
        szBorderRound = UiSize(0, 0);
        rcShadowCorner = UiPadding(30, 30, 34, 36);
        shadowImage = StringUtil::Printf(_T("file='public/shadow/shadow_big.svg' window_shadow_mode='true' corner='%d,%d,%d,%d'"),
                                             rcShadowCorner.left + szBorderRound.cx,
                                             rcShadowCorner.top + szBorderRound.cx,
                                             rcShadowCorner.right + szBorderRound.cx,
                                             rcShadowCorner.bottom + szBorderRound.cx);
    }
    else if (nShadowType == Shadow::ShadowType::kShadowBigRound) {
        bRet = true;
        szBorderRound = UiSize(6, 6);
        rcShadowCorner = UiPadding(30, 30, 34, 36);
        shadowImage = StringUtil::Printf(_T("file='public/shadow/shadow_big_round.svg' window_shadow_mode='true' corner='%d,%d,%d,%d'"),
                                         rcShadowCorner.left + szBorderRound.cx,
                                         rcShadowCorner.top + szBorderRound.cx,
                                         rcShadowCorner.right + szBorderRound.cx,
                                         rcShadowCorner.bottom + szBorderRound.cx);
    }
    else if (nShadowType == Shadow::ShadowType::kShadowSmall) {
        bRet = true;
        szBorderRound = UiSize(0, 0);
        rcShadowCorner = UiPadding(24, 24, 28, 30);
        shadowImage = StringUtil::Printf(_T("file='public/shadow/shadow_small.svg' window_shadow_mode='true' corner='%d,%d,%d,%d'"),
                                             rcShadowCorner.left + szBorderRound.cx,
                                             rcShadowCorner.top + szBorderRound.cx,
                                             rcShadowCorner.right + szBorderRound.cx,
                                             rcShadowCorner.bottom + szBorderRound.cx);
    }
    else if (nShadowType == Shadow::ShadowType::kShadowSmallRound) {
        bRet = true;
        szBorderRound = UiSize(6, 6);
        rcShadowCorner = UiPadding(24, 24, 28, 30);
        shadowImage = StringUtil::Printf(_T("file='public/shadow/shadow_small_round.svg' window_shadow_mode='true' corner='%d,%d,%d,%d'"),
                                         rcShadowCorner.left + szBorderRound.cx,
                                         rcShadowCorner.top + szBorderRound.cx,
                                         rcShadowCorner.right + szBorderRound.cx,
                                         rcShadowCorner.bottom + szBorderRound.cx);
    }
    else if (nShadowType == Shadow::ShadowType::kShadowMenu) {
        bRet = true;
        szBorderRound = UiSize(0, 0);
        rcShadowCorner = UiPadding(24, 24, 28, 30);
        shadowImage = StringUtil::Printf(_T("file='public/shadow/shadow_menu.svg' window_shadow_mode='true' corner='%d,%d,%d,%d'"),
                                             rcShadowCorner.left + szBorderRound.cx,
                                             rcShadowCorner.top + szBorderRound.cx,
                                             rcShadowCorner.right + szBorderRound.cx,
                                             rcShadowCorner.bottom + szBorderRound.cx);
    }
    else if (nShadowType == Shadow::ShadowType::kShadowMenuRound) {
        bRet = true;
        szBorderRound = UiSize(6, 6);
        rcShadowCorner = UiPadding(24, 24, 28, 30);
        shadowImage = StringUtil::Printf(_T("file='public/shadow/shadow_menu_round.svg' window_shadow_mode='true' corner='%d,%d,%d,%d'"),
                                         rcShadowCorner.left + szBorderRound.cx,
                                         rcShadowCorner.top + szBorderRound.cx,
                                         rcShadowCorner.right + szBorderRound.cx,
                                         rcShadowCorner.bottom + szBorderRound.cx);
    }
    else if (nShadowType == Shadow::ShadowType::kShadowNone) {
        bRet = true;
        szBorderRound = UiSize(0, 0);
        rcShadowCorner = UiPadding(0, 0, 0, 0);// Set one pixel to accommodate the border line (see the following code)
        shadowImage.clear();
    }
    else if (nShadowType == Shadow::ShadowType::kShadowNoneRound) {
        bRet = true;
        szBorderRound = UiSize(6, 6);
        rcShadowCorner = UiPadding(0, 0, 0, 0);// Set one pixel to accommodate the border line (see the following code)
        shadowImage.clear();
    }
    else if (nShadowType == Shadow::ShadowType::kShadowCustom) {
        bRet = true;
        szBorderRound = UiSize(0, 0);
        rcShadowCorner = UiPadding(0, 0, 0, 0);
        shadowImage.clear();
    }
    else {
        szBorderRound = UiSize(0, 0);
        rcShadowCorner = UiPadding(0, 0, 0, 0);
        shadowImage.clear();
    }

    if ((pShadowObj != nullptr) && ((nShadowType == Shadow::ShadowType::kShadowNone) ||
                                    (nShadowType == Shadow::ShadowType::kShadowNoneRound))) {
        int32_t nShadowBorderSize = pShadowObj->GetShadowBorderSize();
        if (pShadowObj->GetShadowBorderColor().empty()) {
            nShadowBorderSize = 0;
        }
        if (nShadowBorderSize > 0) {
            nShadowBorderSize /= 2; // Take half of the border line
            if (nShadowBorderSize < 1) {
                nShadowBorderSize = 1;
            }
            rcShadowCorner = UiPadding(nShadowBorderSize, nShadowBorderSize, nShadowBorderSize, nShadowBorderSize);// Border line width
        }
        else {
            rcShadowCorner = UiPadding(0, 0, 0, 0);// Disable the border line
        }
    }
    return bRet;
}

void Shadow::OnShadowAttached(Shadow::ShadowType nShadowType)
{
    UiSize szBorderRound;
    UiPadding rcShadowCorner;
    DString shadowImage;
    if (GetShadowParam(nShadowType, szBorderRound, rcShadowCorner, shadowImage, this)) {
        // User-defined type: do not overwrite the original values; the user-set values take precedence
        if (nShadowType != Shadow::ShadowType::kShadowCustom) {
            SetShadowCorner(rcShadowCorner);
            SetShadowBorderRound(szBorderRound);
            SetShadowImage(shadowImage);
        }
    }
    UpdateShadow();

    //OS-provided shadows: forward the type to the native window (macOS
    //NSWindow / Windows DWM). Self-drawn types disable the OS shadow.
    if ((m_pWindow != nullptr) && (m_pWindow->NativeWnd() != nullptr) &&
        m_pWindow->NativeWnd()->IsSystemShadowSupported()) {
        if (IsShadowAttached() && IsSystemShadowEnabled(nShadowType)) {
            ui::NativeWindowShadowType nativeShadowType =
                ui::NativeWindowShadowType::kShadowSystemDefault;
            if (nShadowType == Shadow::ShadowType::kShadowSystemDefault) {
                nativeShadowType = ui::NativeWindowShadowType::kShadowSystemDefault;
            }
            else if (nShadowType == Shadow::ShadowType::kShadowSystemDoNotRound) {
                nativeShadowType = ui::NativeWindowShadowType::kShadowSystemDoNotRound;
            }
            else if (nShadowType == Shadow::ShadowType::kShadowSystemRound) {
                nativeShadowType = ui::NativeWindowShadowType::kShadowSystemRound;
            }
            else if (nShadowType == Shadow::ShadowType::kShadowSystemSmallRound) {
                nativeShadowType = ui::NativeWindowShadowType::kShadowSystemSmallRound;
            }
            m_pWindow->NativeWnd()->SetSystemShadowType(nativeShadowType);
        }
        else {
            m_pWindow->NativeWnd()->SetSystemShadowType(
                ui::NativeWindowShadowType::kShadowSystemDisabled);
        }
    }
}

void Shadow::UpdateShadow()
{
    // If AttachShadow has already been called, some processing is needed
    if (m_pShadowBox != nullptr) {
        Box* pOrgRoot = nullptr;
        if (m_pShadowBox->GetItemCount() > 0) {
            pOrgRoot = dynamic_cast<Box*>(m_pShadowBox->GetItemAt(0));
        }
        DoAttachShadow(m_pShadowBox, pOrgRoot, m_bShadowAttached, m_isMaximized);

        // Refresh and redraw
        m_pShadowBox->ArrangeAncestor();
        UiRect rcShadow = m_pShadowBox->GetPos();
        if (!rcShadow.IsEmpty()) {
            m_pShadowBox->SetPos(rcShadow);
        }        
    }
}

void Shadow::SetShadowImage(const DString& shadowImage)
{
    if (shadowImage != m_shadowImage) {
        // The shadow image has changed
        m_shadowImage = shadowImage;
        UpdateShadow();
    }
}

const DString& Shadow::GetShadowImage() const
{
    return m_shadowImage;
}

void Shadow::SetShadowBorderSize(int32_t nShadowBorderSize)
{
    ASSERT(nShadowBorderSize >= 0);
    if (nShadowBorderSize >= 0) {
        m_nShadowBorderSize = nShadowBorderSize;
    }    
}

int32_t Shadow::GetShadowBorderSize() const
{
    return m_nShadowBorderSize;
}

void Shadow::SetShadowBorderColor(const DString& shadowBorderColor)
{
    m_shadowBorderColor = shadowBorderColor;
}

const DString& Shadow::GetShadowBorderColor() const
{
    return m_shadowBorderColor;
}

void Shadow::SetShadowCorner(const UiPadding& rcShadowCorner)
{
    ASSERT((rcShadowCorner.left >= 0) && (rcShadowCorner.top >= 0) && (rcShadowCorner.right >= 0) && (rcShadowCorner.bottom >= 0));
    if ((rcShadowCorner.left >= 0) && (rcShadowCorner.top >= 0) && (rcShadowCorner.right >= 0) && (rcShadowCorner.bottom >= 0)) {
        m_rcShadowCorner = rcShadowCorner;
        UpdateShadow();
    }    
}

UiPadding Shadow::GetShadowCorner() const
{
    return m_rcShadowCorner;
}

UiPadding Shadow::GetCurrentShadowCorner() const
{
    if (m_bShadowAttached && !m_isMaximized) {
        UiPadding rcShadowCorner = m_rcShadowCorner;
        ASSERT(m_pWindow != nullptr);
        if (m_pWindow != nullptr) {
            m_pWindow->Dpi().ScalePadding(rcShadowCorner);
        }

        if (m_bLeftSnap) {
            rcShadowCorner.left = 0;
        }
        if (m_bTopSnap) {
            rcShadowCorner.top = 0;
        }
        if (m_bRightSnap) {
            rcShadowCorner.right = 0;
        }
        if (m_bBottomSnap) {
            rcShadowCorner.bottom = 0;
        }
        return rcShadowCorner;
    }
    else {
        return UiPadding(0, 0, 0, 0);
    }
}

void Shadow::SetShadowBorderRound(UiSize szBorderRound)
{
    m_szBorderRound = szBorderRound;
    if (m_pShadowBox != nullptr) {
        m_pShadowBox->Invalidate();
    }
}

UiSize Shadow::GetShadowBorderRound() const
{
    return m_szBorderRound;
}

void Shadow::MaximizedOrRestored(bool isMaximized)
{
    m_isMaximized = isMaximized;
    if (!m_bShadowAttached) {
        return;
    }
    if (m_pShadowBox != nullptr) {
        UiPadding rcShadowCorner = GetCurrentShadowCorner();
        m_pShadowBox->SetPadding(rcShadowCorner, false);
    }
}

void Shadow::ClearImageCache()
{
    if (m_pShadowBox) {
        m_pShadowBox->ClearImageCache();
    }    
}

void Shadow::ChangeDpiScale(const DpiManager& dpi, uint32_t /*nOldDpiScale*/, uint32_t nNewDpiScale)
{
    if (!dpi.CheckDisplayScaleFactor(nNewDpiScale)) {
        return;
    }
    // Update the shadow image (trigger image reloading, adapting to the image responsive to the DPI value according to the DPI)
    DString shadowImage = GetShadowImage();
    if (!shadowImage.empty()) {
        SetShadowImage(_T(""));
        SetShadowImage(shadowImage);
    }
}

void Shadow::SetWindowPosSnap(bool bLeftSnap, bool bRightSnap, bool bTopSnap, bool bBottomSnap)
{
    if (IsEnableShadowSnap() && IsShadowAttached() && !GetShadowImage().empty()) {
        if ((m_bLeftSnap != bLeftSnap) || (m_bTopSnap != bTopSnap) ||
            (m_bRightSnap != bRightSnap) || (m_bBottomSnap != bBottomSnap)) {
            m_bLeftSnap = bLeftSnap;
            m_bTopSnap = bTopSnap;
            m_bRightSnap = bRightSnap;
            m_bBottomSnap = bBottomSnap;

            // Update the attributes
            UpdateWindowPosSnap();
        }
    }
    else {
        if ((m_bLeftSnap != 0) || (m_bTopSnap != 0) ||
            (m_bRightSnap != 0) || (m_bBottomSnap != 0)) {
            m_bLeftSnap = 0;
            m_bTopSnap = 0;
            m_bRightSnap = 0;
            m_bBottomSnap = 0;

            // Update the attributes
            UpdateWindowPosSnap();
        }
    }
}

void Shadow::SetEnableShadowSnap(bool bEnable)
{
    m_bEnableShadowSnap = bEnable;
}

bool Shadow::IsEnableShadowSnap() const
{
    return m_bEnableShadowSnap;
}

void Shadow::UpdateWindowPosSnap()
{
    if (m_pShadowBox != nullptr) {
        UiPadding rcShadowCorner = GetCurrentShadowCorner();
        if(!rcShadowCorner.Equals(m_pShadowBox->GetPadding())) {
            m_pShadowBox->SetPadding(rcShadowCorner, false);
            if (m_pWindow != nullptr) {
                m_pWindow->InvalidateAll();
            }
        }        
    }
}

void Shadow::CheckMouseClickOnShadow(EventType eventType, const UiPoint& pt)
{
    if ((eventType != kEventMouseButtonDown) && (eventType != kEventMouseRButtonDown)) {
        // Only handle the mouse left button down and right button down events
        return;
    }
    if (!IsEnableClickThroughWindow()) {
        return;
    }
    Shadow::ShadowType shadowType = GetShadowType();
    if ((shadowType == Shadow::ShadowType::kShadowNone) || (shadowType == Shadow::ShadowType::kShadowNoneRound)) {
        // No-shadow mode
        return;
    }

    ShadowBox* pShadowBox = nullptr;
    Box* pBox = GetShadowBox();
    if (pBox != nullptr) {
        pShadowBox = dynamic_cast<ShadowBox*>(pBox);
    }
    if (pShadowBox == nullptr) {
        return;
    }
    UiRect rcShadowBox = pShadowBox->GetRect();
    if (!rcShadowBox.ContainsPt(pt)) {
        return;
    }

    UiPadding rcShadowCorner = GetShadowCorner();
    if ((rcShadowCorner.left <= 1) && (rcShadowCorner.top <= 1) && (rcShadowCorner.right <= 1) && (rcShadowCorner.bottom <= 1)) {
        // No shadow currently
        return;
    }
    pShadowBox->Dpi().ScalePadding(rcShadowCorner);
    rcShadowBox.Deflate(rcShadowCorner);
    if (rcShadowBox.ContainsPt(pt)) {
        // The mouse is not within the shadow area
        return;
    }

    // The mouse is confirmed to have clicked on the shadow; handle the shadow click-through logic
    pShadowBox->OnMouseDown(pt);
}

void Shadow::SetEnableClickThroughWindow(bool bEnable)
{
    m_bEnableClickThroughWindow = bEnable;
}

bool Shadow::IsEnableClickThroughWindow() const
{
    return m_bEnableClickThroughWindow;
}

} //namespace ui
