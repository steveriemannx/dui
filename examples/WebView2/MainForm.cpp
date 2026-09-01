//MainForm.cpp
#include "MainForm.h"
#include "dui/Utils/UiBuilder.h"

void MainForm::OnInitWindow()
{
    //Window initialization finished, this Form can now be initialized

    m_pWebView2Control = ui::Find<ui::WebView2Control>(this, "webview2_control");
    m_pEditUrl = ui::Find<ui::RichEdit>(this, "edit_url");

    // Set the input box style
    if (m_pEditUrl != nullptr) {
        m_pEditUrl->SetSelAllOnFocus(true);
        m_pEditUrl->AttachReturn(UiBind(&MainForm::OnNavigate, this, std::placeholders::_1));
    }

    ui::Control* pControl = ui::Find<ui::Control>(this, "btn_back");
    if (pControl != nullptr) {
        pControl->SetEnabled(false);
    }

    pControl = ui::Find<ui::Control>(this, "btn_forward");
    if (pControl != nullptr) {
        pControl->SetEnabled(false);
    }

    if (m_pWebView2Control != nullptr) {
        //Update the state of the back and forward buttons
        m_pWebView2Control->SetHistoryChangedCallback([this]() {
            if (m_pWebView2Control != nullptr) {
                ui::Control* pControl = ui::Find<ui::Control>(this, "btn_back");
                if (pControl != nullptr) {
                    pControl->SetEnabled(m_pWebView2Control->CanGoBack());
                }
                pControl = ui::Find<ui::Control>(this, "btn_forward");
                if (pControl != nullptr) {
                    pControl->SetEnabled(m_pWebView2Control->CanGoForward());
                }
            }
            });

        //Update the URL
        m_pWebView2Control->SetSourceChangedCallback([this](const DString& url) {
            ui::GlobalManager::Instance().AssertUIThread();
            ui::RichEdit* pEditUrl = ui::Find<ui::RichEdit>(this, "edit_url");
            if (pEditUrl != nullptr) {
                pEditUrl->SetText(url);
            }
            });

        //Update the title
        m_pWebView2Control->SetDocumentTitleChangedCallback([this](const DString& title) {
            ui::GlobalManager::Instance().AssertUIThread();
            ui::Label* pLabelTitle = ui::Find<ui::Label>(this, "page_title");
            if (pLabelTitle != nullptr) {
                pLabelTitle->SetText(title);
            }
            });
    }

    BindEvents();
    BaseClass::OnInitWindow();
}

void MainForm::BindEvents()
{
    GetRoot()->AttachBubbledEvent(ui::kEventClick, UiBind(&MainForm::OnClicked, this, std::placeholders::_1), 0);

    //Fullscreen page
    if (auto* pFullscreenBtn = ui::Find<ui::Button>(this, "webview2_full_screen_btn")) {
        pFullscreenBtn->AttachClick([this](const ui::EventArgs&) {
            ui::Control* pWebView2Control = ui::Find<ui::Control>(this, "webview2_control");
            if (pWebView2Control != nullptr) {
                this->SetFullscreenControl(pWebView2Control);
            }
            return true;
            });
    }
}

bool MainForm::OnClicked(const ui::EventArgs& msg)
{
    DString name = msg.GetSender()->GetName();

    if (name == _T("btn_dev_tool")) {
        if (m_pWebView2Control != nullptr) {
            m_pWebView2Control->OpenDevToolsWindow();
        }
    }
    else if (name == _T("btn_back")) {
        if (m_pWebView2Control != nullptr) {
            m_pWebView2Control->NavigateBack();
        }
    }
    else if (name == _T("btn_forward")) {
        if (m_pWebView2Control != nullptr) {
            m_pWebView2Control->NavigateForward();
        }
    }
    else if (name == _T("btn_navigate")) {
        ui::EventArgs emptyMsg;
        OnNavigate(emptyMsg);
    }
    else if (name == _T("btn_refresh")) {
        if (m_pWebView2Control != nullptr) {
            m_pWebView2Control->Refresh();
        }
    }
    return true;
}

bool MainForm::OnNavigate(const ui::EventArgs& /*msg*/)
{
    if ((m_pEditUrl != nullptr) && !m_pEditUrl->GetText().empty()) {
        if (m_pWebView2Control != nullptr) {
            m_pWebView2Control->Navigate(m_pEditUrl->GetText());
            m_pWebView2Control->SetFocus();
        }
    }
    return true;
}


LRESULT MainForm::OnKeyDownMsg(ui::VirtualKeyCode vkCode, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled)
{
    if (vkCode == ui::kVK_F11) {
        if (ui::WebView2Manager::GetInstance().IsEnableF11()) {
            //Enter fullscreen or exit fullscreen for the page
            if (IsWindowFullscreen() && (GetFullscreenControl() != nullptr)) {
                bHandled = true;
                ExitControlFullscreen();
            }
            else {
                //Display the current page in fullscreen
                if (m_pWebView2Control != nullptr) {
                    bHandled = true;
                    SetFullscreenControl(m_pWebView2Control);
                }
            }
        }
    }
    else if (vkCode == ui::kVK_F12) {
        if (ui::WebView2Manager::GetInstance().IsEnableF12()) {
            //Show or hide the developer tools
            bHandled = true;
            if (m_pWebView2Control != nullptr) {
                //There is only an open function, no close function
                if (m_pWebView2Control->AreDevToolsEnabled()) {
                    m_pWebView2Control->OpenDevToolsWindow();
                }
            }
        }
    }
    if (bHandled) {
        return 0;
    }
    return BaseClass::OnKeyDownMsg(vkCode, modifierKey, nativeMsg, bHandled);
}
