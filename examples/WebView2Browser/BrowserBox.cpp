#include "BrowserBox.h"
#include "BrowserForm.h"
#include "BrowserManager.h"

using namespace ui;

BrowserBox::BrowserBox(ui::Window* pWindow, std::string browserId):
    ui::VBox(pWindow)
{
    m_pBrowserForm = nullptr;
    m_pWebView2Control = nullptr;
    m_browserId = browserId;
}

BrowserForm* BrowserBox::GetBrowserForm() const
{
    ASSERT(nullptr != m_pBrowserForm);
    ASSERT(m_pBrowserForm->IsWindow());
    return m_pBrowserForm;
}

ui::WebView2Control* BrowserBox::GetWebView2Control()
{
    return m_pWebView2Control;
}

const DString& BrowserBox::GetTitle() const
{
    return m_title;
}

void BrowserBox::InitBrowserBox(const DString& url)
{
    m_pWebView2Control = static_cast<ui::WebView2Control*>(FindSubControl(_T("webview2_control")));
    ASSERT(m_pWebView2Control != nullptr);
    if (m_pWebView2Control == nullptr) {
        return;
    }
    //Attach events
    m_pWebView2Control->SetSourceChangedCallback([this](const DString& url) {
        ui::GlobalManager::Instance().AssertUIThread();
        m_url = url;
        m_pBrowserForm->SetURL(m_browserId, url);
        });

    m_pWebView2Control->SetDocumentTitleChangedCallback([this](const DString& title) {
        ui::GlobalManager::Instance().AssertUIThread();
        m_title = title;
        m_pBrowserForm->SetTabItemName(ui::StringConvert::UTF8ToT(m_browserId), title);
        });

    m_pWebView2Control->SetNavigationStateChangedCallback([this](WebView2Control::NavigationState state, HRESULT /*errorCode*/) {
        ui::GlobalManager::Instance().AssertUIThread();
        if (m_pBrowserForm != nullptr) {
            m_pBrowserForm->OnLoadingStateChange(this);
        }
        if (m_pWebView2Control != nullptr) {
            //Test code
            if (state == WebView2Control::NavigationState::Completed) {
               // m_pWebView2Control->PostWebMessageAsString(_T("hello world!"));               
            }
        }
        });

    m_pWebView2Control->SetHistoryChangedCallback([this]() {
        ui::GlobalManager::Instance().AssertUIThread();
        if (m_pBrowserForm != nullptr) {
            m_pBrowserForm->OnLoadingStateChange(this);
        }
        });

    m_pWebView2Control->SetFavIconChangedCallback([this](int32_t nWidth, int32_t nHeight, const std::vector<uint8_t>& imageData) {
        ui::GlobalManager::Instance().AssertUIThread();
        if (m_pBrowserForm != nullptr) {
            m_pBrowserForm->NotifyFavicon(this, nWidth, nHeight, imageData);
        }
        });
    m_pWebView2Control->SetZoomFactorChangedCallback([this](double zoomFactor) {
        //Test code
        ui::GlobalManager::Instance().AssertUIThread();
        });
    m_pWebView2Control->SetWebMessageReceivedCallback([this](const DString& url,
                                                             const DString& webMessageAsJson,
                                                             const DString& webMessageAsString) {
        //Test code
        ui::GlobalManager::Instance().AssertUIThread();
        //Send a reply to the HTML page
        m_pWebView2Control->PostWebMessageAsString(_T("Hello from C++!"));
        });

    //New window request callback function
    m_pWebView2Control->SetNewWindowRequestedCallback([this](const DString& sourceUrl, const DString& sourceFrame,
                                                             const DString& targetUrl, const DString& targetFrame,
                                                             bool bUserInitiated) {
            // Returning true allows creating the popup page, but the new page navigates in the current page and no new window pops up;
            // Returning false blocks the popup page, and the display logic of the new page is managed inside the callback function
            ui::GlobalManager::Instance().AssertUIThread();
            if (!bUserInitiated) {
                //If it is not a manually triggered popup page, block it directly
                return false;
            }
            if (targetUrl.empty()) {
                //The target URL is empty, block it directly
                return false;
            }
            //Create a new tab
            if (m_pBrowserForm != nullptr) {
                if (m_pBrowserForm->IsWindowFullscreen() &&
                    (m_pBrowserForm->GetFullscreenControl() != nullptr) &&
                    (dynamic_cast<ui::WebView2Control*>(m_pBrowserForm->GetFullscreenControl()) != nullptr)) {
                    //In fullscreen mode, do not open a new tab, open directly in the current page
                    return true;
                }

                //Open in a new tab
                m_pBrowserForm->OpenLinkUrl(targetUrl, false);
            }
            return false;
        });

    m_pWebView2Control->InitializeAsync(_T(""), [this](HRESULT result) {
        //Test code
        ui::GlobalManager::Instance().AssertUIThread();
        });

    //Navigate to the URL
    DString navigateUrl = url;
    if (navigateUrl.empty()) {
        navigateUrl = _T("www.baidu.com");

        ////Test JS-C++ communication
        //ui::FilePath webViewHtml = GlobalManager::GetDefaultResourcePath(true);
        //webViewHtml.NormalizeDirectoryPath();
        //webViewHtml += _T("themes/default/webview2_browser/WebView2Demo.html");
        //webViewHtml.NormalizeFilePath();
        //navigateUrl = _T("file:///");
        //navigateUrl += webViewHtml.ToString();
        //StringUtil::ReplaceAll(_T("\\"), _T("/"), navigateUrl);
    }
    m_pWebView2Control->Navigate(navigateUrl);
}

void BrowserBox::UninitBrowserBox()
{
    BrowserManager::GetInstance()->RemoveBorwserBox(m_browserId, this);
}

void BrowserBox::SetWindow(Window* pWindow)
{
    m_pBrowserForm = dynamic_cast<BrowserForm*>(pWindow);
    ASSERT(m_pBrowserForm != nullptr);

    BaseClass::SetWindow(pWindow);
}

bool BrowserBox::OnSetFocus(const ui::EventArgs& msg)
{
    // When the Box gets focus, transfer the focus to the web page control
    if (m_pWebView2Control) {
        m_pWebView2Control->SetFocus();
    }

    //Do not call the base class method, to avoid overriding the input method management logic (the base class closes the input method)
    if (GetState() == kControlStateNormal) {
        SetState(kControlStateHot);
        Invalidate();
    }
    return true;
}
