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
    SetLayeredWindow(false, false);
    SetEnableShadowSnap(true);
    SetShadowBorderSize(0);
    SetSizeBox(ui::UiRect(4, 4, 4, 4), true);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), true);
    SetWindowIcon(DUI_T("public/caption/logo.ico"));
    CenterWindow();
}

void MainForm::BuildUI()
{
    auto* pRoot = ui::Create<ui::VBox>(this, {{DUI_T("bkcolor"), DUI_T("bk_wnd_darkcolor")}, {DUI_T("visible"), DUI_T("true")}});

    auto* pCaption = ui::Create<ui::HBox>(this, {{DUI_T("name"), DUI_T("window_caption_bar")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("36")}, {DUI_T("bkcolor"), DUI_T("bk_wnd_lightcolor")}});
    ui::Attach(pRoot, pCaption);

    auto* pCaptionLeft = ui::Create<ui::HBox>(this, {{DUI_T("margin"), DUI_T("0,0,30,0")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("width"), DUI_T("auto")}, {DUI_T("height"), DUI_T("auto")}, {DUI_T("mouse_enabled"), DUI_T("false")}});
    ui::Attach(pCaption, pCaptionLeft);

    auto* pLogo = ui::Create<ui::Control>(this, {{DUI_T("width"), DUI_T("18")}, {DUI_T("height"), DUI_T("18")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("bkimage"), DUI_T("public/caption/logo.svg")}});
    ui::Attach(pCaptionLeft, pLogo);

    auto* pTitle = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("ChildWindow Control Test Program")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("mouse_enabled"), DUI_T("false")}});
    ui::Attach(pCaptionLeft, pTitle);

    auto* pSpacer = ui::Create<ui::Control>(this, {{DUI_T("mouse_enabled"), DUI_T("false")}});
    ui::Attach(pCaption, pSpacer);

    auto* pCaptionRight = ui::Create<ui::HBox>(this, {{DUI_T("margin"), DUI_T("0,0,0,0")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("width"), DUI_T("auto")}, {DUI_T("height"), DUI_T("36")}});
    ui::Attach(pCaption, pCaptionRight);

    auto* pFullscreenBtn = ui::Create<ui::Button>(this, {{DUI_T("class"), DUI_T("btn_wnd_fullscreen_11")}, {DUI_T("name"), DUI_T("fullscreenbtn")}, {DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("margin"), DUI_T("0,2,0,2")}, {DUI_T("tooltip_text"), DUI_T("Fullscreen, press ESC to exit fullscreen")}});
    ui::Attach(pCaptionRight, pFullscreenBtn);

    auto* pMinBtn = ui::Create<ui::Button>(this, {{DUI_T("class"), DUI_T("btn_wnd_min_11")}, {DUI_T("name"), DUI_T("minbtn")}, {DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("margin"), DUI_T("0,2,0,2")}, {DUI_T("tooltip_text"), DUI_T("Minimize")}});
    ui::Attach(pCaptionRight, pMinBtn);

    auto* pMaxBox = ui::Create<ui::Box>(this, {{DUI_T("height"), DUI_T("stretch")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("margin"), DUI_T("0,2,0,2")}});
    ui::Attach(pCaptionRight, pMaxBox);

    auto* pMaxBtn = ui::Create<ui::Button>(this, {{DUI_T("class"), DUI_T("btn_wnd_max_11")}, {DUI_T("name"), DUI_T("maxbtn")}, {DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("tooltip_text"), DUI_T("Maximize")}});
    ui::Attach(pMaxBox, pMaxBtn);

    auto* pRestoreBtn = ui::Create<ui::Button>(this, {{DUI_T("class"), DUI_T("btn_wnd_restore_11")}, {DUI_T("name"), DUI_T("restorebtn")}, {DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("visible"), DUI_T("false")}, {DUI_T("tooltip_text"), DUI_T("Restore")}});
    ui::Attach(pMaxBox, pRestoreBtn);

    auto* pCloseBtn = ui::Create<ui::Button>(this, {{DUI_T("class"), DUI_T("btn_wnd_close_11")}, {DUI_T("name"), DUI_T("closebtn")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("margin"), DUI_T("0,0,0,2")}, {DUI_T("tooltip_text"), DUI_T("Close")}});
    ui::Attach(pCaptionRight, pCloseBtn);

    auto* pContent = ui::Create<ui::Box>(this, {});
    ui::Attach(pRoot, pContent);

    auto* pGridBox = ui::Create<ui::GridBox>(this, {{DUI_T("name"), DUI_T("child_window_box")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("rows"), DUI_T("3")}, {DUI_T("columns"), DUI_T("3")}, {DUI_T("bkcolor"), DUI_T("#FFF0F0F0")}});
    ui::Attach(pContent, pGridBox);

    for (int32_t i = 0; i < 9; ++i) {
        auto* pChild = ui::Create<ui::ChildWindow>(this, {{DUI_T("valign"), DUI_T("center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("child_window_margin"), DUI_T("12,36,12,12")}});
        auto* pChildCaption = ui::Create<ui::HBox>(this, {{DUI_T("padding"), DUI_T("12,0,0,0")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("height"), DUI_T("36")}});
        ui::Attach(pChild, pChildCaption);

        auto* pChildName = ui::Create<ui::Label>(this, {{DUI_T("name"), DUI_T("child_window_name")}, {DUI_T("text"), ui::StringUtil::Printf(DUI_T("ChildWindow%d"), i + 1)}, {DUI_T("margin"), DUI_T("2,0,2,0")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("mouse_enabled"), DUI_T("false")}});
        ui::Attach(pChildCaption, pChildName);

        auto* pFpsText = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Frame Rate FPS:")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("mouse_enabled"), DUI_T("false")}});
        ui::Attach(pChildCaption, pFpsText);

        auto* pFpsValue = ui::Create<ui::Label>(this, {{DUI_T("name"), DUI_T("label_fps")}, {DUI_T("text"), DUI_T("0000")}, {DUI_T("width"), DUI_T("42")}, {DUI_T("margin"), DUI_T("4,0,4,0")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("mouse_enabled"), DUI_T("false")}});
        ui::Attach(pChildCaption, pFpsValue);

        auto* pFpsPaint = ui::Create<ui::CheckBox>(this, {{DUI_T("class"), DUI_T("checkbox_1")}, {DUI_T("name"), DUI_T("fps_paint")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("width"), DUI_T("auto")}, {DUI_T("text"), DUI_T("Draw")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("selected"), DUI_T("true")}});
        ui::Attach(pChildCaption, pFpsPaint);

        auto* pFullscreen = ui::Create<ui::CheckBox>(this, {{DUI_T("class"), DUI_T("checkbox_1")}, {DUI_T("name"), DUI_T("child_fullscreen")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("width"), DUI_T("auto")}, {DUI_T("text"), DUI_T("Fullscreen")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("false")}});
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
    ui::GridBox* pChildWindowBox = ui::Find<ui::GridBox>(this, DUI_T("child_window_box"));
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
