#include "MainForm.h"
#include "ChildWindowPaint.h"
#include "MyChildWindowEvents.h"
#include "dui/Utils/UiBuilder.h"

MainForm::MainForm() :
    m_pChildWindow(nullptr)
{
}

MainForm::~MainForm()
{
    if (!m_childWindowEvents.empty()) {
        CloseChildWindows();
    }
}

void MainForm::PreInitWindow()
{
    BaseClass::PreInitWindow();
    SetRenderBackendType(ui::RenderBackendType::kMetal_BackendType);
}

void MainForm::SetupWindow()
{
    ui::UiRect rcWork;
    ui::WindowBase::GetPrimaryMonitorWorkRect(rcWork);
#if defined(DUI_BUILD_FOR_MACOS)
    SetWindowSize(static_cast<int32_t>(rcWork.Width() * 0.85f),
                  static_cast<int32_t>(rcWork.Height() * 0.90f));
#else
    SetWindowSize(static_cast<int32_t>(rcWork.Width() * 0.75f),
                  static_cast<int32_t>(rcWork.Height() * 0.85f));
#endif

    SetShadowAttached(true);
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
#if defined(DUI_BUILD_FOR_MACOS)
    SetLayeredWindow(true, false);
#else
    SetLayeredWindow(false, false);
#endif
    SetEnableShadowSnap(true);
    SetShadowBorderSize(0);
    SetSizeBox(ui::UiRect(4, 4, 4, 4), true);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), true);
    SetWindowIcon("public/caption/logo.ico");
    CenterWindow();
}

void MainForm::BuildUI()
{
    auto* pRoot = ui::Create<ui::VBox>(this, {{"bkcolor", "bk_wnd_darkcolor"}, {"visible", "true"}});

    auto* pCaption = ui::Create<ui::HBox>(this, {{"name", "window_caption_bar"}, {"width", "stretch"}, {"height", "36"}, {"bkcolor", "bk_wnd_lightcolor"}});
    ui::Attach(pRoot, pCaption);

    auto* pCaptionLeft = ui::Create<ui::HBox>(this, {{"margin", "0,0,30,0"}, {"valign", "center"}, {"width", "auto"}, {"height", "auto"}, {"mouse_enabled", "false"}});
    ui::Attach(pCaption, pCaptionLeft);

    auto* pLogo = ui::Create<ui::Control>(this, {{"width", "18"}, {"height", "18"}, {"valign", "center"}, {"margin", "8,0,0,0"}, {"bkimage", "public/caption/logo.svg"}});
    ui::Attach(pCaptionLeft, pLogo);

    auto* pTitle = ui::Create<ui::Label>(this, {{"text", "ChildWindow Control Test Program"}, {"valign", "center"}, {"margin", "8,0,0,0"}, {"mouse_enabled", "false"}});
    ui::Attach(pCaptionLeft, pTitle);

    auto* pSpacer = ui::Create<ui::Control>(this, {{"mouse_enabled", "false"}});
    ui::Attach(pCaption, pSpacer);

    auto* pCaptionRight = ui::Create<ui::HBox>(this, {{"margin", "0,0,0,0"}, {"valign", "center"}, {"width", "auto"}, {"height", "36"}});
    ui::Attach(pCaption, pCaptionRight);

    auto* pFullscreenBtn = ui::Create<ui::Button>(this, {{"class", "btn_wnd_fullscreen_11"}, {"name", "fullscreenbtn"}, {"height", "32"}, {"width", "40"}, {"margin", "0,2,0,2"}, {"tooltip_text", "Fullscreen, press ESC to exit fullscreen"}});
    ui::Attach(pCaptionRight, pFullscreenBtn);

    auto* pMinBtn = ui::Create<ui::Button>(this, {{"class", "btn_wnd_min_11"}, {"name", "minbtn"}, {"height", "32"}, {"width", "40"}, {"margin", "0,2,0,2"}, {"tooltip_text", "Minimize"}});
    ui::Attach(pCaptionRight, pMinBtn);

    auto* pMaxBox = ui::Create<ui::Box>(this, {{"height", "stretch"}, {"width", "40"}, {"margin", "0,2,0,2"}});
    ui::Attach(pCaptionRight, pMaxBox);

    auto* pMaxBtn = ui::Create<ui::Button>(this, {{"class", "btn_wnd_max_11"}, {"name", "maxbtn"}, {"height", "32"}, {"width", "stretch"}, {"tooltip_text", "Maximize"}});
    ui::Attach(pMaxBox, pMaxBtn);

    auto* pRestoreBtn = ui::Create<ui::Button>(this, {{"class", "btn_wnd_restore_11"}, {"name", "restorebtn"}, {"height", "32"}, {"width", "stretch"}, {"visible", "false"}, {"tooltip_text", "Restore"}});
    ui::Attach(pMaxBox, pRestoreBtn);

    auto* pCloseBtn = ui::Create<ui::Button>(this, {{"class", "btn_wnd_close_11"}, {"name", "closebtn"}, {"height", "stretch"}, {"width", "40"}, {"margin", "0,0,0,2"}, {"tooltip_text", "Close"}});
    ui::Attach(pCaptionRight, pCloseBtn);

    auto* pContent = ui::Create<ui::Box>(this, {});
    ui::Attach(pRoot, pContent);

    auto* pGridBox = ui::Create<ui::GridBox>(this, {{"name", "child_window_box"}, {"valign", "center"}, {"rows", "3"}, {"columns", "3"}, {"bkcolor", "#FFF0F0F0"}});
    ui::Attach(pContent, pGridBox);

    for (int32_t i = 0; i < 9; ++i) {
        auto* pChild = ui::Create<ui::ChildWindow>(this, {{"valign", "center"}, {"halign", "center"}, {"child_window_margin", "12,36,12,12"}});
        auto* pChildCaption = ui::Create<ui::HBox>(this, {{"padding", "12,0,0,0"}, {"valign", "top"}, {"height", "36"}});
        ui::Attach(pChild, pChildCaption);

        auto* pChildName = ui::Create<ui::Label>(this, {{"name", "child_window_name"}, {"text", ui::StringUtil::Printf("ChildWindow%d", i + 1)}, {"margin", "2,0,2,0"}, {"valign", "center"}, {"mouse_enabled", "false"}});
        ui::Attach(pChildCaption, pChildName);

        auto* pFpsText = ui::Create<ui::Label>(this, {{"text", "Frame Rate FPS:"}, {"valign", "center"}, {"mouse_enabled", "false"}});
        ui::Attach(pChildCaption, pFpsText);

        auto* pFpsValue = ui::Create<ui::Label>(this, {{"name", "label_fps"}, {"text", "0000"}, {"width", "42"}, {"margin", "4,0,4,0"}, {"valign", "center"}, {"mouse_enabled", "false"}});
        ui::Attach(pChildCaption, pFpsValue);

        auto* pFpsPaint = ui::Create<ui::CheckBox>(this, {{"class", "checkbox_1"}, {"name", "fps_paint"}, {"height", "28"}, {"width", "auto"}, {"text", "Draw"}, {"valign", "center"}, {"selected", "true"}});
        ui::Attach(pChildCaption, pFpsPaint);

        auto* pFullscreen = ui::Create<ui::CheckBox>(this, {{"class", "checkbox_1"}, {"name", "child_fullscreen"}, {"height", "28"}, {"width", "auto"}, {"text", "Fullscreen"}, {"valign", "center"}, {"margin", "8,0,0,0"}, {"selected", "false"}});
        ui::Attach(pChildCaption, pFullscreen);
        ui::Attach(pGridBox, pChild);
    }

    ui::Attach(this, pRoot);
}

void MainForm::BindEvents()
{
}

void MainForm::OnInitWindow()
{
    SetupWindow();
    BuildUI();
    BindEvents();
    BaseClass::OnInitWindow();
}

void MainForm::OnInitLayout()
{
    BaseClass::OnInitLayout();
    if (m_childWindowEvents.empty()) {
        CreateChildWindows();
    }
}

void MainForm::OnPreCloseWindow()
{
    CloseChildWindows();
    BaseClass::OnPreCloseWindow();
}

void MainForm::OnLayeredWindowChanged()
{
    BaseClass::OnLayeredWindowChanged();
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
    ui::GridBox* pChildWindowBox = ui::Find<ui::GridBox>(this, "child_window_box");
    if (pChildWindowBox != nullptr) {
        size_t nCount = pChildWindowBox->GetItemCount();
        for (size_t nItem = 0; nItem < nCount; ++nItem) {
            ui::ChildWindow* pChildWindow = dynamic_cast<ui::ChildWindow*>(pChildWindowBox->GetItemAt(nItem));
            if (pChildWindow != nullptr) {
                MyChildWindowEvents* pMyChildWindowEvents = new MyChildWindowEvents(pChildWindow, nItem, this);
                if (pChildWindow->CreateChildWindow(pMyChildWindowEvents)) {
                    m_childWindowEvents.push_back(pMyChildWindowEvents);
                    pChildWindow->InvalidateChildWindow();
                }
                else {
                    delete pMyChildWindowEvents;
                }
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
    m_pChildWindow = pChildWindow;
    return true;
}

bool MainForm::PaintNextChildWindow()
{
    return DoPaintNextChildWindow(m_pChildWindow);
}

bool MainForm::DoPaintNextChildWindow(ui::ChildWindow* pChildWindow)
{
    if (pChildWindow == nullptr || m_childWindowEvents.empty()) {
        return false;
    }
    size_t nStartItemIndex = 0;
    const size_t nItemCount = m_childWindowEvents.size();
    for (size_t nItemIndex = 0; nItemIndex < nItemCount; ++nItemIndex) {
        MyChildWindowEvents* pChildWindowEvents = m_childWindowEvents[nItemIndex];
        if (pChildWindowEvents != nullptr && pChildWindowEvents->GetChildWindow() == pChildWindow) {
            nStartItemIndex = nItemIndex;
            break;
        }
    }
    for (size_t nItemIndex = nStartItemIndex + 1; nItemIndex < nItemCount; ++nItemIndex) {
        MyChildWindowEvents* pChildWindowEvents = m_childWindowEvents[nItemIndex];
        if (pChildWindowEvents != nullptr && pChildWindowEvents->GetChildWindow() != nullptr &&
            pChildWindowEvents->GetChildWindow()->IsVisible() && pChildWindowEvents->IsPaintFps()) {
            return PaintChildWindow(pChildWindowEvents->GetChildWindow());
        }
    }
    if (nStartItemIndex >= m_childWindowEvents.size()) {
        nStartItemIndex = m_childWindowEvents.size() - 1;
    }
    for (size_t nItemIndex = 0; nItemIndex <= nStartItemIndex; ++nItemIndex) {
        MyChildWindowEvents* pChildWindowEvents = m_childWindowEvents[nItemIndex];
        if (pChildWindowEvents != nullptr && pChildWindowEvents->GetChildWindow() != nullptr &&
            pChildWindowEvents->GetChildWindow()->IsVisible() && pChildWindowEvents->IsPaintFps()) {
            return PaintChildWindow(pChildWindowEvents->GetChildWindow());
        }
    }
    return PaintChildWindow(pChildWindow);
}
