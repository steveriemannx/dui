#include "MainForm.h"
#include "ChildWindowPaint.h"
#include "MyChildWindowEvents.h"

MainForm::MainForm():
    m_pChildWindow(nullptr)
{
}

MainForm::~MainForm()
{
    if (!m_childWindowEvents.empty()) {
        CloseChildWindows();
    }    
}

DString MainForm::GetSkinFolder()
{
    return _T("");
}

DString MainForm::GetSkinFile()
{
    // Pure code mode: no layout XML is loaded
    return _T("");
}

void MainForm::GetCreateWindowAttributes(ui::WindowCreateAttributes& attrs)
{
    // Match the <Window> attributes in child_window.xml:
    // size="75%,85%", size_box/caption, shadow/layered settings.
    ui::UiRect rcWork;
    ui::WindowBase::GetPrimaryMonitorWorkRect(rcWork);
    attrs.m_bInitSizeDefined = true;
    attrs.m_szInitSize.cx = (int32_t)(rcWork.Width() * 0.85f);
    attrs.m_szInitSize.cy = (int32_t)(rcWork.Height() * 0.90f);

    attrs.m_rcSizeBox = ui::UiRect(4, 4, 4, 4);
    attrs.m_bSizeBoxDefined = true;
    attrs.m_rcCaption = ui::UiRect(0, 0, 0, 36);
    attrs.m_bCaptionDefined = true;

    attrs.m_bShadowAttached = true;
    attrs.m_bShadowAttachedDefined = true;
    attrs.m_bIsLayeredWindow = false;
    attrs.m_bIsLayeredWindowDefined = true;

    // Shadow nine-patch parameters, corresponding to shadow_type="default" in
    // the XML layout (WindowBuilder adds the shadow corner to the size).
    ui::Shadow::ShadowType nShadowType = ui::Shadow::ShadowType::kShadowDefault;
    ui::UiSize szBorderRound;
    ui::UiPadding rcShadowCorner;
    DString shadowImage;
    if (ui::Shadow::GetShadowParam(nShadowType, szBorderRound, rcShadowCorner, shadowImage)) {
        attrs.m_rcShadowCorner = rcShadowCorner;
        if (attrs.m_bInitSizeDefined) {
            attrs.m_szInitSize.cx += rcShadowCorner.left + rcShadowCorner.right;
            attrs.m_szInitSize.cy += rcShadowCorner.top + rcShadowCorner.bottom;
        }
    }

    BaseClass::GetCreateWindowAttributes(attrs);
}

void MainForm::BuildUI()
{
    // Corresponding to the child_window.xml layout
    ui::VBox* pRoot = new ui::VBox(this);
    pRoot->SetBkColor(_T("bk_wnd_darkcolor"));

    // Title bar area
    ui::HBox* pCaption = new ui::HBox(this);
    pCaption->SetAttribute(_T("name"), _T("window_caption_bar"));
    pCaption->SetAttribute(_T("width"), _T("stretch"));
    pCaption->SetAttribute(_T("height"), _T("36"));
    pCaption->SetBkColor(_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pCaption);

    ui::HBox* pCaptionLeft = new ui::HBox(this);
    pCaptionLeft->SetAttribute(_T("margin"), _T("0,0,30,0"));
    pCaptionLeft->SetAttribute(_T("valign"), _T("center"));
    pCaptionLeft->SetAttribute(_T("width"), _T("auto"));
    pCaptionLeft->SetAttribute(_T("height"), _T("auto"));
    pCaptionLeft->SetAttribute(_T("mouse_enabled"), _T("false"));
    pCaption->AddItem(pCaptionLeft);

    ui::Control* pLogo = new ui::Control(this);
    pLogo->SetAttribute(_T("width"), _T("18"));
    pLogo->SetAttribute(_T("height"), _T("18"));
    pLogo->SetBkImage(_T("public/caption/logo.svg"));
    pLogo->SetAttribute(_T("valign"), _T("center"));
    pLogo->SetAttribute(_T("margin"), _T("8,0,0,0"));
    pCaptionLeft->AddItem(pLogo);

    ui::Label* pTitle = new ui::Label(this);
    pTitle->SetText(_T("ChildWindow Control Test Program"));
    pTitle->SetAttribute(_T("valign"), _T("center"));
    pTitle->SetAttribute(_T("margin"), _T("8,0,0,0"));
    pTitle->SetAttribute(_T("mouse_enabled"), _T("false"));
    pCaptionLeft->AddItem(pTitle);

    ui::Control* pSpacer = new ui::Control(this);
    pSpacer->SetAttribute(_T("mouse_enabled"), _T("false"));
    pCaption->AddItem(pSpacer);

    ui::HBox* pCaptionRight = new ui::HBox(this);
    pCaptionRight->SetAttribute(_T("margin"), _T("0,0,0,0"));
    pCaptionRight->SetAttribute(_T("valign"), _T("center"));
    pCaptionRight->SetAttribute(_T("width"), _T("auto"));
    pCaptionRight->SetAttribute(_T("height"), _T("36"));
    pCaption->AddItem(pCaptionRight);

    ui::Button* pMinBtn = new ui::Button(this);
    pMinBtn->SetClass(_T("btn_wnd_min_11"));
    pMinBtn->SetAttribute(_T("height"), _T("32"));
    pMinBtn->SetAttribute(_T("width"), _T("40"));
    pMinBtn->SetName(_T("minbtn"));
    pMinBtn->SetAttribute(_T("margin"), _T("0,2,0,2"));
    pMinBtn->SetToolTipText(_T("Minimize"));
    pCaptionRight->AddItem(pMinBtn);

    ui::Box* pMaxBox = new ui::Box(this);
    pMaxBox->SetAttribute(_T("height"), _T("stretch"));
    pMaxBox->SetAttribute(_T("width"), _T("40"));
    pMaxBox->SetAttribute(_T("margin"), _T("0,2,0,2"));
    pCaptionRight->AddItem(pMaxBox);

    ui::Button* pMaxBtn = new ui::Button(this);
    pMaxBtn->SetClass(_T("btn_wnd_max_11"));
    pMaxBtn->SetAttribute(_T("height"), _T("32"));
    pMaxBtn->SetAttribute(_T("width"), _T("stretch"));
    pMaxBtn->SetName(_T("maxbtn"));
    pMaxBtn->SetToolTipText(_T("Maximize"));
    pMaxBox->AddItem(pMaxBtn);

    ui::Button* pRestoreBtn = new ui::Button(this);
    pRestoreBtn->SetClass(_T("btn_wnd_restore_11"));
    pRestoreBtn->SetAttribute(_T("height"), _T("32"));
    pRestoreBtn->SetAttribute(_T("width"), _T("stretch"));
    pRestoreBtn->SetName(_T("restorebtn"));
    pRestoreBtn->SetVisible(false);
    pRestoreBtn->SetToolTipText(_T("Restore"));
    pMaxBox->AddItem(pRestoreBtn);

    ui::Button* pCloseBtn = new ui::Button(this);
    pCloseBtn->SetClass(_T("btn_wnd_close_11"));
    pCloseBtn->SetAttribute(_T("height"), _T("stretch"));
    pCloseBtn->SetAttribute(_T("width"), _T("40"));
    pCloseBtn->SetName(_T("closebtn"));
    pCloseBtn->SetAttribute(_T("margin"), _T("0,0,0,2"));
    pCloseBtn->SetToolTipText(_T("Close"));
    pCaptionRight->AddItem(pCloseBtn);

    // Work area: a 3x3 GridBox holding 9 child windows (corresponding to <Include count="9"/>)
    ui::Box* pContent = new ui::Box(this);
    pRoot->AddItem(pContent);

    ui::GridBox* pGridBox = new ui::GridBox(this);
    pGridBox->SetName(_T("child_window_box"));
    pGridBox->SetAttribute(_T("valign"), _T("center"));
    pGridBox->SetAttribute(_T("rows"), _T("3"));
    pGridBox->SetAttribute(_T("columns"), _T("3"));
    pGridBox->SetBkColor(_T("#FFF0F0F0"));
    pContent->AddItem(pGridBox);

    for (int32_t i = 0; i < 9; ++i) {
        ui::ChildWindow* pChild = new ui::ChildWindow(this);
        pChild->SetAttribute(_T("valign"), _T("center"));
        pChild->SetAttribute(_T("halign"), _T("center"));
        pChild->SetAttribute(_T("child_window_margin"), _T("12,36,12,12"));

        ui::HBox* pChildCaption = new ui::HBox(this);
        pChildCaption->SetAttribute(_T("padding"), _T("12,0,0,0"));
        pChildCaption->SetAttribute(_T("valign"), _T("top"));
        pChildCaption->SetAttribute(_T("height"), _T("36"));
        pChild->AddItem(pChildCaption);

        ui::Label* pChildName = new ui::Label(this);
        pChildName->SetName(_T("child_window_name"));
        pChildName->SetText(ui::StringUtil::Printf(_T("ChildWindow%d"), i + 1));
        pChildName->SetAttribute(_T("margin"), _T("2,0,2,0"));
        pChildName->SetAttribute(_T("valign"), _T("center"));
        pChildName->SetAttribute(_T("mouse_enabled"), _T("false"));
        pChildCaption->AddItem(pChildName);

        ui::Label* pFpsText = new ui::Label(this);
        pFpsText->SetText(_T("Frame Rate FPS:"));
        pFpsText->SetAttribute(_T("valign"), _T("center"));
        pFpsText->SetAttribute(_T("mouse_enabled"), _T("false"));
        pChildCaption->AddItem(pFpsText);

        ui::Label* pFpsValue = new ui::Label(this);
        pFpsValue->SetName(_T("label_fps"));
        pFpsValue->SetText(_T("0000"));
        pFpsValue->SetAttribute(_T("width"), _T("42"));
        pFpsValue->SetAttribute(_T("margin"), _T("4,0,4,0"));
        pFpsValue->SetAttribute(_T("valign"), _T("center"));
        pFpsValue->SetAttribute(_T("mouse_enabled"), _T("false"));
        pChildCaption->AddItem(pFpsValue);

        ui::CheckBox* pFpsPaint = new ui::CheckBox(this);
        pFpsPaint->SetClass(_T("checkbox_1"));
        pFpsPaint->SetName(_T("fps_paint"));
        pFpsPaint->SetAttribute(_T("height"), _T("28"));
        pFpsPaint->SetAttribute(_T("width"), _T("auto"));
        pFpsPaint->SetText(_T("Draw"));
        pFpsPaint->SetAttribute(_T("valign"), _T("center"));
        pFpsPaint->Selected(true);
        pChildCaption->AddItem(pFpsPaint);

        ui::CheckBox* pFullscreen = new ui::CheckBox(this);
        pFullscreen->SetClass(_T("checkbox_1"));
        pFullscreen->SetName(_T("child_fullscreen"));
        pFullscreen->SetAttribute(_T("height"), _T("28"));
        pFullscreen->SetAttribute(_T("width"), _T("auto"));
        pFullscreen->SetText(_T("Fullscreen"));
        pFullscreen->SetAttribute(_T("valign"), _T("center"));
        pFullscreen->SetAttribute(_T("margin"), _T("8,0,0,0"));
        pChildCaption->AddItem(pFullscreen);

        pGridBox->AddItem(pChild);
    }

    AttachBox(pRoot);
}


void MainForm::PreInitWindow()
{
    BaseClass::PreInitWindow();
    //Use the GPU (OpenGL) render backend on macOS, matching the XML example.
    SetRenderBackendType(ui::RenderBackendType::kMetal_BackendType);
}

void MainForm::OnInitWindow()
{
    // Use the OS-provided system shadow on all platforms.
    SetShadowAttached(true);
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    SetLayeredWindow(false, false);
    SetEnableShadowSnap(true);
    SetShadowBorderSize(0);


    SetSizeBox(ui::UiRect(4, 4, 4, 4), false);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), false);

    BuildUI();

    BaseClass::OnInitWindow();
    // Create the child window and associate the handling interface
    CreateChildWindows();
}

void MainForm::OnPreCloseWindow()
{
    // Destroy the child window and remove the association
    CloseChildWindows();
    BaseClass::OnPreCloseWindow();
}

void MainForm::OnLayeredWindowChanged()
{
    BaseClass::OnLayeredWindowChanged();
    // Synchronize the layered window attributes to the child window
    for (MyChildWindowEvents* pChildWindowEvents : m_childWindowEvents) {
        if (pChildWindowEvents != nullptr) {
            ui::ChildWindow* pChildWindow = pChildWindowEvents->GetChildWindow();
            if (pChildWindow != nullptr) {
                pChildWindow->SetChildWindowLayered(IsLayeredWindow());
            }
        }
    }
}

void MainForm::CreateChildWindows()
{
    ui::GridBox* pChildWindowBox = dynamic_cast<ui::GridBox*>(FindControl(_T("child_window_box")));
    if (pChildWindowBox != nullptr) {
        size_t nCount = pChildWindowBox->GetItemCount();
        for (size_t nItem = 0; nItem < nCount; ++nItem) {
            ui::ChildWindow* pChildWindow = dynamic_cast<ui::ChildWindow*>(pChildWindowBox->GetItemAt(nItem));
            if (pChildWindow != nullptr) {
                MyChildWindowEvents* pMyChildWindowEvents = new MyChildWindowEvents(pChildWindow, nItem, this);
                pChildWindow->CreateChildWindow(pMyChildWindowEvents);
                m_childWindowEvents.push_back(pMyChildWindowEvents);
            }
        }
    }
}

void MainForm::CloseChildWindows()
{
    std::vector<MyChildWindowEvents*> childWindowEvents;
    childWindowEvents.swap(m_childWindowEvents);
    for (MyChildWindowEvents* pChildWindowEvents : childWindowEvents) {
        if (pChildWindowEvents != nullptr) {
            ui::ChildWindow* pChildWindow = pChildWindowEvents->GetChildWindow();
            if (pChildWindow != nullptr) {
                // Close the child window (synchronously)
                pChildWindow->SetChildWindowEvents(nullptr);
                pChildWindow->CloseChildWindow();
            }
            delete pChildWindowEvents;
            pChildWindowEvents = nullptr;
        }
    }
}

bool MainForm::PaintChildWindow(ui::ChildWindow* pChildWindow)
{
    if (pChildWindow != nullptr) {
        pChildWindow->InvalidateChildWindow();
        return true;
    }
    return false;
}

bool MainForm::PaintNextChildWindow(ui::ChildWindow* pChildWindow)
{
    // Continuous drawing must be triggered in the idle function, otherwise the UI will freeze
    m_pChildWindow = pChildWindow;
    return true;
}

bool MainForm::PaintNextChildWindow()
{
    // Paint every visible child window on each idle pass so the FPS demo can
    // reach high rates on macOS as well; a strict round-robin makes each child
    // only get a fraction of the idle-loop frequency.
    for (MyChildWindowEvents* pEvents : m_childWindowEvents) {
        if ((pEvents != nullptr) && (pEvents->GetChildWindow() != nullptr) &&
            pEvents->GetChildWindow()->IsVisible()) {
            PaintChildWindow(pEvents->GetChildWindow());
        }
    }
    return true;
}

bool MainForm::DoPaintNextChildWindow(ui::ChildWindow * pChildWindow)
{
    if (pChildWindow == nullptr) {
        return false;
    }
    if ((pChildWindow == nullptr) || m_childWindowEvents.empty()) {
        return false;
    }
    size_t nStartItemIndex = 0;
    const size_t nItemCount = m_childWindowEvents.size();
    for (size_t nItemIndex = 0; nItemIndex < nItemCount; ++nItemIndex) {
        MyChildWindowEvents* pChildWindowEvents = m_childWindowEvents[nItemIndex];
        if ((pChildWindowEvents != nullptr) && (pChildWindowEvents->GetChildWindow() == pChildWindow)) {
            nStartItemIndex = nItemIndex;
            break;
        }
    }
    for (size_t nItemIndex = nStartItemIndex + 1; nItemIndex < nItemCount; ++nItemIndex) {
        MyChildWindowEvents* pChildWindowEvents = m_childWindowEvents[nItemIndex];
        if ((pChildWindowEvents != nullptr) &&
            (pChildWindowEvents->GetChildWindow() != nullptr) &&
            pChildWindowEvents->GetChildWindow()->IsVisible() &&
            pChildWindowEvents->IsPaintFps()) {
            // Determine the window to draw
            return PaintChildWindow(pChildWindowEvents->GetChildWindow());
        }
    }
    if (nStartItemIndex >= m_childWindowEvents.size()) {
        nStartItemIndex = m_childWindowEvents.size() - 1;
    }
    for (size_t nItemIndex = 0; nItemIndex <= nStartItemIndex; ++nItemIndex) {
        MyChildWindowEvents* pChildWindowEvents = m_childWindowEvents[nItemIndex];
        if ((pChildWindowEvents != nullptr) &&
            (pChildWindowEvents->GetChildWindow() != nullptr) &&
            pChildWindowEvents->GetChildWindow()->IsVisible() &&
            pChildWindowEvents->IsPaintFps()) {
            // Determine the window to draw
            return PaintChildWindow(pChildWindowEvents->GetChildWindow());
        }
    }
    return PaintChildWindow(pChildWindow);
}
