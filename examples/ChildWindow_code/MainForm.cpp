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

    BaseClass::GetCreateWindowAttributes(attrs);
}

void MainForm::PreInitWindow()
{
    BaseClass::PreInitWindow();
    //Use the GPU (Metal) render backend on macOS, matching the XML example.
    SetRenderBackendType(ui::RenderBackendType::kMetal_BackendType);
}

void MainForm::BuildUI()
{
    // Corresponding to the child_window.xml layout
    auto* pRoot = ui::Create<ui::VBox>(this, {});
    pRoot->SetBkColor(_T("bk_wnd_darkcolor"));

    // Title bar area
    auto* pCaption = ui::Create<ui::HBox>(this, {{_T("name"), _T("window_caption_bar")}, {_T("width"), _T("stretch")}, {_T("height"), _T("36")}});
    pCaption->SetBkColor(_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pCaption);

    auto* pCaptionLeft = ui::Create<ui::HBox>(this, {{_T("margin"), _T("0,0,30,0")}, {_T("valign"), _T("center")}, {_T("width"), _T("auto")}, {_T("height"), _T("auto")}, {_T("mouse_enabled"), _T("false")}});
    pCaption->AddItem(pCaptionLeft);

    auto* pLogo = ui::Create<ui::Control>(this, {{_T("width"), _T("18")}, {_T("height"), _T("18")}, {_T("valign"), _T("center")}, {_T("margin"), _T("8,0,0,0")}});
    pLogo->SetBkImage(_T("public/caption/logo.svg"));
    pCaptionLeft->AddItem(pLogo);

    auto* pTitle = ui::Create<ui::Label>(this, {{_T("valign"), _T("center")}, {_T("margin"), _T("8,0,0,0")}, {_T("mouse_enabled"), _T("false")}});
    pTitle->SetText(_T("ChildWindow Control Test Program"));
    pCaptionLeft->AddItem(pTitle);

    auto* pSpacer = ui::Create<ui::Control>(this, {{_T("mouse_enabled"), _T("false")}});
    pCaption->AddItem(pSpacer);

    auto* pCaptionRight = ui::Create<ui::HBox>(this, {{_T("margin"), _T("0,0,0,0")}, {_T("valign"), _T("center")}, {_T("width"), _T("auto")}, {_T("height"), _T("36")}});
    pCaption->AddItem(pCaptionRight);

    auto* pMinBtn = ui::Create<ui::Button>(this, {{_T("height"), _T("32")}, {_T("width"), _T("40")}, {_T("margin"), _T("0,2,0,2")}});
    pMinBtn->SetClass(_T("btn_wnd_min_11"));
    pMinBtn->SetName(_T("minbtn"));
    pMinBtn->SetToolTipText(_T("Minimize"));
    pCaptionRight->AddItem(pMinBtn);

    auto* pMaxBox = ui::Create<ui::Box>(this, {{_T("height"), _T("stretch")}, {_T("width"), _T("40")}, {_T("margin"), _T("0,2,0,2")}});
    pCaptionRight->AddItem(pMaxBox);

    auto* pMaxBtn = ui::Create<ui::Button>(this, {{_T("height"), _T("32")}, {_T("width"), _T("stretch")}});
    pMaxBtn->SetClass(_T("btn_wnd_max_11"));
    pMaxBtn->SetName(_T("maxbtn"));
    pMaxBtn->SetToolTipText(_T("Maximize"));
    pMaxBox->AddItem(pMaxBtn);

    auto* pRestoreBtn = ui::Create<ui::Button>(this, {{_T("height"), _T("32")}, {_T("width"), _T("stretch")}});
    pRestoreBtn->SetClass(_T("btn_wnd_restore_11"));
    pRestoreBtn->SetName(_T("restorebtn"));
    pRestoreBtn->SetVisible(false);
    pRestoreBtn->SetToolTipText(_T("Restore"));
    pMaxBox->AddItem(pRestoreBtn);

    auto* pCloseBtn = ui::Create<ui::Button>(this, {{_T("height"), _T("stretch")}, {_T("width"), _T("40")}, {_T("margin"), _T("0,0,0,2")}});
    pCloseBtn->SetClass(_T("btn_wnd_close_11"));
    pCloseBtn->SetName(_T("closebtn"));
    pCloseBtn->SetToolTipText(_T("Close"));
    pCaptionRight->AddItem(pCloseBtn);

    // Work area: a 3x3 GridBox holding 9 child windows (corresponding to <Include count="9"/>)
    auto* pContent = ui::Create<ui::Box>(this, {});
    pRoot->AddItem(pContent);

    auto* pGridBox = ui::Create<ui::GridBox>(this, {{_T("valign"), _T("center")}, {_T("rows"), _T("3")}, {_T("columns"), _T("3")}});
    pGridBox->SetName(_T("child_window_box"));
    pGridBox->SetBkColor(_T("#FFF0F0F0"));
    pContent->AddItem(pGridBox);

    for (int32_t i = 0; i < 9; ++i) {
    auto* pChild = ui::Create<ui::ChildWindow>(this, {{_T("valign"), _T("center")}, {_T("halign"), _T("center")}, {_T("child_window_margin"), _T("12,36,12,12")}});

    auto* pChildCaption = ui::Create<ui::HBox>(this, {{_T("padding"), _T("12,0,0,0")}, {_T("valign"), _T("top")}, {_T("height"), _T("36")}});
        pChild->AddItem(pChildCaption);

    auto* pChildName = ui::Create<ui::Label>(this, {{_T("margin"), _T("2,0,2,0")}, {_T("valign"), _T("center")}, {_T("mouse_enabled"), _T("false")}});
        pChildName->SetName(_T("child_window_name"));
        pChildName->SetText(ui::StringUtil::Printf(_T("ChildWindow%d"), i + 1));
        pChildCaption->AddItem(pChildName);

    auto* pFpsText = ui::Create<ui::Label>(this, {{_T("valign"), _T("center")}, {_T("mouse_enabled"), _T("false")}});
        pFpsText->SetText(_T("Frame Rate FPS:"));
        pChildCaption->AddItem(pFpsText);

    auto* pFpsValue = ui::Create<ui::Label>(this, {{_T("width"), _T("42")}, {_T("margin"), _T("4,0,4,0")}, {_T("valign"), _T("center")}, {_T("mouse_enabled"), _T("false")}});
        pFpsValue->SetName(_T("label_fps"));
        pFpsValue->SetText(_T("0000"));
        pChildCaption->AddItem(pFpsValue);

    auto* pFpsPaint = ui::Create<ui::CheckBox>(this, {{_T("height"), _T("28")}, {_T("width"), _T("auto")}, {_T("valign"), _T("center")}});
        pFpsPaint->SetClass(_T("checkbox_1"));
        pFpsPaint->SetName(_T("fps_paint"));
        pFpsPaint->SetText(_T("Draw"));
        pFpsPaint->Selected(true);
        pChildCaption->AddItem(pFpsPaint);

    auto* pFullscreen = ui::Create<ui::CheckBox>(this, {{_T("height"), _T("28")}, {_T("width"), _T("auto")}, {_T("valign"), _T("center")}, {_T("margin"), _T("8,0,0,0")}});
        pFullscreen->SetClass(_T("checkbox_1"));
        pFullscreen->SetName(_T("child_fullscreen"));
        pFullscreen->SetText(_T("Fullscreen"));
        pChildCaption->AddItem(pFullscreen);

        pGridBox->AddItem(pChild);
    }

    AttachBox(pRoot);
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
    return DoPaintNextChildWindow(m_pChildWindow);
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
