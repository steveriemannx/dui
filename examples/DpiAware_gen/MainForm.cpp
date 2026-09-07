//MainForm.cpp
#include "MainForm.h"
#include "dui/Utils/UiBuilder.h"
#include "generated_ui.inc"  // Build-time generated pure C++ UI code (from DpiAware.xml)

void MainForm::OnInitWindow()
{
    BuildUI();
    BindEvents();
    BaseClass::OnInitWindow();
    UpdateUI();
}

void MainForm::BuildUI()
{
    InitDpiAware(this);
}

void MainForm::BindEvents()
{
    if (auto* pButton = ui::Find<ui::Button>(this, DUI_T("NewWindow"))) {
        pButton->AttachClick([this](const ui::EventArgs& /*args*/) {
            // Pop up a new window
            MainForm* window = new MainForm();
            auto createParam = ui::WindowCreateParam();
            createParam.m_dwStyle = ui::kWS_POPUP | ui::kWS_VISIBLE;
            createParam.m_dwExStyle = ui::kWS_EX_LAYERED;
            createParam.m_windowTitle = DUI_T("DpiAware");
            createParam.m_bCenterWindow = true;
            window->CreateWnd(this, createParam);
            window->ShowWindow(ui::kSW_SHOW_NORMAL);
            return true;
            });
    }

    if (auto* pButton = ui::Find<ui::Button>(this, DUI_T("set_display_scale_factor"))) {
        pButton->AttachClick([this](const ui::EventArgs& /*args*/) {
            // Change the UI display scale
            if (auto* pRichEdit = ui::Find<ui::RichEdit>(this, DUI_T("display_scale_factor"))) {
            auto nNewDisplayScaleFactor = ui::StringUtil::StringToInt32(pRichEdit->GetText());
                if (nNewDisplayScaleFactor > 0) {
                    this->ChangeDisplayScale((uint32_t)nNewDisplayScaleFactor);
                    UpdateUI();
                }
            }
            return true;
            });
    }

    if (auto* pGroupTest = ui::Find<ui::Control>(this, DUI_T("group_box_test"))) {
        pGroupTest->AttachPosChanged([this](const ui::EventArgs& /*args*/) {
            UpdateUI();
            return true;
            });
    }
}

void MainForm::OnWindowDisplayScaleChanged(uint32_t nOldScaleFactor, uint32_t nNewScaleFactor)
{
    ui::WindowImplBase::OnWindowDisplayScaleChanged(nOldScaleFactor, nNewScaleFactor);
    UpdateUI();
}

LRESULT MainForm::OnSizeMsg(ui::WindowSizeType sizeType, const ui::UiSize& newWindowSize, const ui::NativeMsg& nativeMsg, bool& bHandled)
{
    auto lResult = ui::WindowImplBase::OnSizeMsg(sizeType, newWindowSize, nativeMsg, bHandled);
    UpdateUI();
    return lResult;
}

void MainForm::UpdateUI()
{
    if (auto* pLabel = ui::Find<ui::Label>(this, DUI_T("dpi_awareness"))) {
        DString text;
        auto mode = ui::GlobalManager::Instance().Dpi().GetDpiAwareness();
        if (mode == ui::DpiAwarenessMode::kDpiUnaware) {
            text = DUI_T("kDpiUnaware");
        }
        else if (mode == ui::DpiAwarenessMode::kSystemDpiAware) {
            text = DUI_T("kSystemDpiAware");
        }
        else if (mode == ui::DpiAwarenessMode::kPerMonitorDpiAware) {
            text = DUI_T("kPerMonitorDpiAware");
        }
        else if (mode == ui::DpiAwarenessMode::kPerMonitorDpiAware_V2) {
            text = DUI_T("kPerMonitorDpiAware_V2");
        }
        pLabel->SetText(text);
    }

    const auto* pGroupTest = ui::Find<ui::Control>(this, DUI_T("group_box_test"));
    if (auto* pLabel = ui::Find<ui::Label>(this, DUI_T("group_box_pos"))) {
        if (pGroupTest != nullptr) {
            auto text = ui::StringUtil::Printf(DUI_T("[left: %d, top: %d]"), pGroupTest->GetRect().left, pGroupTest->GetRect().top);
            pLabel->SetText(text);
        }
    }

    if (auto* pLabel = ui::Find<ui::Label>(this, DUI_T("primary_monitor_display_scale"))) {
        auto nScaleFactor = ui::GlobalManager::Instance().Dpi().GetDisplayScaleFactor();
        float fScale = ui::GlobalManager::Instance().Dpi().GetDisplayScale();
        auto text = ui::StringUtil::Printf(DUI_T("%d%% (DisplayScale: %.02f)"), nScaleFactor, fScale);
        pLabel->SetText(text);
    }
    if (auto* pLabel = ui::Find<ui::Label>(this, DUI_T("window_display_scale"))) {
        auto nScaleFactor = Dpi().GetDisplayScaleFactor();
        float fScale = Dpi().GetDisplayScale();
        auto text = ui::StringUtil::Printf(DUI_T("%d%% (DisplayScale: %.02f)"), nScaleFactor, fScale);
        pLabel->SetText(text);
    }
    if (auto* pRichEdit = ui::Find<ui::RichEdit>(this, DUI_T("display_scale_factor"))) {
        uint32_t nScaleFactor = Dpi().GetDisplayScaleFactor();
        pRichEdit->SetTextNumber((int64_t)nScaleFactor);
    }
    if (auto* pLabel = ui::Find<ui::Label>(this, DUI_T("window_size"))) {
        ui::UiRect rcWindow;
        GetWindowRect(rcWindow);
        auto text = ui::StringUtil::Printf(DUI_T("W:%d, H:%d [Left:%d, Top:%d]"), rcWindow.Width(), rcWindow.Height(), rcWindow.left, rcWindow.top);
        pLabel->SetText(text);
    }
    if (auto* pLabel = ui::Find<ui::Label>(this, DUI_T("window_client_size"))) {
        ui::UiRect rcClient;
        GetClientRect(rcClient);
        auto text = ui::StringUtil::Printf(DUI_T("W:%d, H:%d [Left:%d, Top:%d]"), rcClient.Width(), rcClient.Height(), rcClient.left, rcClient.top);
        pLabel->SetText(text);
    }
#ifdef DUI_BUILD_FOR_WAYLAND
    if (auto* pLabel = ui::Find<ui::Label>(this, DUI_T("Native_GetWindowSize"))) {
        int32_t w = 0;
        int32_t h = 0;
        NativeWnd()->GetWindowSize(&w, &h);
        auto text = ui::StringUtil::Printf(DUI_T("W:%d, H:%d"), w, h);
        pLabel->SetText(text);
    }
    if (auto* pLabel = ui::Find<ui::Label>(this, DUI_T("Native_GetWindowSizeInPixels"))) {
        int32_t w = 0;
        int32_t h = 0;
        NativeWnd()->GetWindowSizeInPixels(&w, &h);
        auto text = ui::StringUtil::Printf(DUI_T("W:%d, H:%d"), w, h);
        pLabel->SetText(text);
    }
    if (auto* pLabel = ui::Find<ui::Label>(this, DUI_T("Native_GetDisplayContentScale"))) {
        float scale = NativeWnd()->GetDisplayContentScale();
        auto text = ui::StringUtil::Printf(DUI_T("%.02f"), scale);
        pLabel->SetText(text);
    }
    if (auto* pLabel = ui::Find<ui::Label>(this, DUI_T("Native_GetWindowDisplayScale"))) {
        float scale = NativeWnd()->GetWindowDisplayScale();
        auto text = ui::StringUtil::Printf(DUI_T("%.02f"), scale);
        pLabel->SetText(text);
    }
    if (auto* pLabel = ui::Find<ui::Label>(this, DUI_T("Native_GetWindowPixelDensity"))) {
        float scale = NativeWnd()->GetWindowPixelDensity();
        auto text = ui::StringUtil::Printf(DUI_T("%.02f"), scale);
        pLabel->SetText(text);
    }
#else
    if (auto* pnative_backend = ui::Find<ui::Control>(this, DUI_T("native backend"))) {
        pnative_backend->SetVisible(false);
    }
#endif
}
