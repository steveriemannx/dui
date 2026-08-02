#include "CefForm.h"

#ifdef DUILIB_BUILD_FOR_SDL
    #include <iostream>
#endif

CefForm::CefForm()
{
}

CefForm::~CefForm()
{
}

DString CefForm::GetSkinFolder()
{
    return _T("cef");
}

DString CefForm::GetSkinFile()
{
    return _T("cef.xml");
}

void CefForm::OnInitWindow()
{
#if !defined (DUILIB_BUILD_FOR_WIN)
    //Linux platform: in non-off-screen rendering mode, use the system caption bar
    if (!kEnableOffScreenRendering) {
        SetUseSystemCaption(true);
    }
#endif

    if (!IsUseSystemCaption()) {
        if (ui::CefManager::GetInstance()->IsEnableOffScreenRendering()) {
            //Off-screen rendering: enable the layered window attribute
            SetLayeredWindow(true, true);
        }
        else {
            //Window mode: disable the layered window attribute
            SetLayeredWindow(false, true);
        }
    }

    // Listen to the mouse click event
    GetRoot()->AttachBubbledEvent(ui::kEventClick, UiBind(&CefForm::OnClicked, this, std::placeholders::_1), 0);

    // Find the specified control from the XML
    m_pCefControl = dynamic_cast<ui::CefControl*>(FindControl(_T("cef_control")));
    m_pCefControlDev = dynamic_cast<ui::CefControl*>(FindControl(_T("cef_control_dev")));
    m_pDevToolBtn = dynamic_cast<ui::Button*>(FindControl(_T("btn_dev_tool")));
    m_pEditUrl = dynamic_cast<ui::RichEdit*>(FindControl(_T("edit_url")));
    ASSERT(m_pDevToolBtn != nullptr);
    //ASSERT(m_pEditUrl != nullptr);

    // Set the input box style
    if (m_pEditUrl != nullptr) {
        m_pEditUrl->SetSelAllOnFocus(true);
        m_pEditUrl->AttachReturn(UiBind(&CefForm::OnNavigate, this, std::placeholders::_1));
    }

    ui::Control* pControl = FindControl(_T("btn_back"));
    if (pControl != nullptr) {
        pControl->SetEnabled(false);
    }

    pControl = FindControl(_T("btn_forward"));
    if (pControl != nullptr) {
        pControl->SetEnabled(false);
    }

    if (m_pCefControl != nullptr) {
        m_pCefControl->SetCefEventHandler(this);
        if (m_pCefControlDev != nullptr) {
            //The developer tools of m_pCefControl are displayed in the m_pCefControlDev control
            m_pCefControl->SetDevToolsView(m_pCefControlDev);
        }

        //URL change event
        m_pCefControl->AttachMainUrlChange(UiBind(&CefForm::OnMainUrlChange, this, std::placeholders::_1, std::placeholders::_2));
    }

    if (m_pCefControl != nullptr) {
        m_pCefControl->AttachDevToolAttachedStateChange(UiBind(&CefForm::OnDevToolVisibleStateChanged, this, std::placeholders::_1, std::placeholders::_2));
    }
    if (m_pCefControlDev != nullptr) {
        m_pCefControlDev->SetFadeVisible(false);
    }

    //Set the callback function that controls the main process singleton
    ui::CefManager::GetInstance()->SetAlreadyRunningAppRelaunch(UiBind(&CefForm::OnAlreadyRunningAppRelaunch, this, std::placeholders::_1));

    if (!ui::CefManager::GetInstance()->IsEnableOffScreenRendering()) {
        //Handle the multi-focus problem of controls (since the cef control is in child window mode, duilib cannot do this by itself)
        AttachWindowKillFocusMsg([this](const ui::EventArgs& args) {
            //When the window loses focus, let the controls in the interface lose focus too, to avoid the problem of the web page and interface controls being focused at the same time
            KillFocusControl();
            return true;
            });
    }

    //Fullscreen page
    ui::Button* pFullscreenBtn = dynamic_cast<ui::Button*>(FindControl(_T("cef_full_screen_btn")));
    if (pFullscreenBtn != nullptr) {
        pFullscreenBtn->AttachClick([this](const ui::EventArgs&) {
            ui::Control* pCefControl = FindControl(_T("cef_control"));
            if (pCefControl != nullptr) {
                this->SetFullscreenControl(pCefControl);
            }
            return true;
            });
    }

#ifdef DUILIB_BUILD_FOR_SDL
    //Show basic SDL information
    DString driverName = GetVideoDriverName();
    DString renderName = GetWindowRenderName();
    DString logMsg = ui::StringUtil::Printf(_T("[SDL: VideoDriver:\"%s\", RenderName:\"%s\"]"), driverName.c_str(), renderName.c_str());
    std::cout << logMsg << std::endl;
#endif
}

void CefForm::OnPreCloseWindow()
{
    //When closing the window, first close all Browser objects associated with this window
    ui::CefManager::GetInstance()->ProcessWindowCloseEvent(this);
}

void CefForm::OnCloseWindow()
{   
    //After the window is closed, exit the message loop of the main thread and close the program
    ui::CefManager::GetInstance()->PostQuitMessage(0L);
}

LRESULT CefForm::OnKeyDownMsg(ui::VirtualKeyCode vkCode, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled)
{
    if (vkCode == ui::kVK_F11) {
        if (ui::CefManager::GetInstance()->IsEnableF11()) {
            //Enter fullscreen or exit fullscreen for the page
            if (IsWindowFullscreen() && (GetFullscreenControl() != nullptr)) {
                bHandled = true;
                ExitControlFullscreen();
            }
            else {
                //Display the current page in fullscreen
                if (m_pCefControl != nullptr) {
                    bHandled = true;
                    SetFullscreenControl(m_pCefControl);
                }
            }
        }
    }
    else if (vkCode == ui::kVK_F12) {
        if (ui::CefManager::GetInstance()->IsEnableF12()) {
            //Show or hide the developer tools
            bHandled = true;
            SwitchShowDevTools();
        }
    }
    if (bHandled) {
        return 0;
    }
    return BaseClass::OnKeyDownMsg(vkCode, modifierKey, nativeMsg, bHandled);
}

void CefForm::SwitchShowDevTools()
{
    if (m_pCefControl != nullptr) {
        if (m_pCefControl->IsAttachedDevTools()) {
            m_pCefControl->DettachDevTools();
            if (m_pCefControlDev != nullptr) {
                m_pCefControlDev->SetFadeVisible(false);
            }
        }
        else {
            m_pCefControl->AttachDevTools();
        }
    }
}

void CefForm::OnAlreadyRunningAppRelaunch(const std::vector<DString>& argumentList)
{
    if (ui::GlobalManager::Instance().IsInUIThread()) {
        //CEF 133 calls this interface
        SetWindowForeground();
        if (!argumentList.empty()) {
            //Only process the first argument
            DString url = argumentList[0];
            if (m_pCefControl != nullptr) {                
                m_pCefControl->LoadURL(url);
                m_pCefControl->SetFocus();
            }
        }
    }
    else {
        //Forward to the UI thread for processing
        ui::GlobalManager::Instance().Thread().PostTask(ui::kThreadUI, UiBind(&CefForm::OnAlreadyRunningAppRelaunch, this, argumentList));
    }
}

bool CefForm::OnClicked(const ui::EventArgs& msg)
{
    DString name = msg.GetSender()->GetName();

    if (name == _T("btn_dev_tool")) {
        SwitchShowDevTools();
    }
    else if (name == _T("btn_back")) {
        if (m_pCefControl != nullptr) {
            m_pCefControl->GoBack();
        }
    }
    else if (name == _T("btn_forward")) {
        if (m_pCefControl != nullptr) {
            m_pCefControl->GoForward();
        }
    }
    else if (name == _T("btn_navigate")) {
        ui::EventArgs emptyMsg;
        OnNavigate(emptyMsg);
    }
    else if (name == _T("btn_refresh")) {
        if (m_pCefControl != nullptr) {
            m_pCefControl->Refresh();
        }
    }
    return true;
}

bool CefForm::OnNavigate(const ui::EventArgs& /*msg*/)
{
    if ((m_pEditUrl != nullptr) && !m_pEditUrl->GetText().empty()) {
        if (m_pCefControl != nullptr) {
            m_pCefControl->LoadURL(m_pEditUrl->GetText());
            m_pCefControl->SetFocus();
        }
    }
    return true;
}

void CefForm::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    ui::GlobalManager::Instance().AssertUIThread();
}

void CefForm::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
    ui::GlobalManager::Instance().AssertUIThread();
}

void CefForm::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  CefRefPtr<CefContextMenuParams> params,
                                  CefRefPtr<CefMenuModel> model)
{
    ASSERT(CefCurrentlyOn(TID_UI));
}

bool CefForm::OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefContextMenuParams> params,
                                   int command_id,
                                   cef_event_flags_t event_flags)
{
    ASSERT(CefCurrentlyOn(TID_UI));
    return false;
}
    
void CefForm::OnContextMenuDismissed(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame)
{
    ASSERT(CefCurrentlyOn(TID_UI));
}

void CefForm::OnTitleChange(CefRefPtr<CefBrowser> browser, const DString& title)
{
    ui::GlobalManager::Instance().AssertUIThread();
    ui::Label* pLabelTitle = dynamic_cast<ui::Label*>(FindControl(_T("page_title")));
    if (pLabelTitle != nullptr) {
        pLabelTitle->SetText(title);
    }
}
    
void CefForm::OnUrlChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const DString& url)
{
    ui::GlobalManager::Instance().AssertUIThread();
}
    
void CefForm::OnMainUrlChange(const DString& oldUrl, const DString& newUrl)
{
    ui::GlobalManager::Instance().AssertUIThread();
    ui::RichEdit* pEditUrl = dynamic_cast<ui::RichEdit*>(FindControl(_T("edit_url")));
    if (pEditUrl != nullptr) {
        pEditUrl->SetText(newUrl);
    }
}
    
void CefForm::OnFaviconURLChange(CefRefPtr<CefBrowser> browser, const std::vector<CefString>& icon_urls)
{
    ui::GlobalManager::Instance().AssertUIThread();
}
        
void CefForm::OnFullscreenModeChange(CefRefPtr<CefBrowser> browser, bool bFullscreen)
{
    ui::GlobalManager::Instance().AssertUIThread();
}
    
void CefForm::OnStatusMessage(CefRefPtr<CefBrowser> browser, const DString& value)
{
    ui::GlobalManager::Instance().AssertUIThread();
}
    
void CefForm::OnLoadingProgressChange(CefRefPtr<CefBrowser> browser, double progress)
{
    ui::GlobalManager::Instance().AssertUIThread();
}
    
void CefForm::OnMediaAccessChange(CefRefPtr<CefBrowser> browser, bool has_video_access, bool has_audio_access)
{
    ui::GlobalManager::Instance().AssertUIThread();
}

bool CefForm::OnDragEnter(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDragData> dragData, CefDragHandler::DragOperationsMask mask)
{
    m_dropFileList.clear();
    if ((dragData != nullptr) && dragData->IsFile()){
        //File drop operation
#if CEF_VERSION_MAJOR > 109
        //When CEF is higher than version 109, getting the full paths of files is supported
        dragData->GetFilePaths(m_dropFileList);
#else
        //With CEF 109, only file names can be obtained, not the full paths of files
        dragData->GetFileNames(m_dropFileList);
#endif
    }
    return false;
}

void CefForm::OnDraggableRegionsChanged(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const std::vector<CefDraggableRegion>& regions)
{
    ui::GlobalManager::Instance().AssertUIThread();
}

bool CefForm::OnBeforePopup(CefRefPtr<CefBrowser> browser,
                            CefRefPtr<CefFrame> frame,
                            int popup_id,
                            const CefString& target_url,
                            const CefString& target_frame_name,
                            CefLifeSpanHandler::WindowOpenDisposition target_disposition,
                            bool user_gesture,
                            const CefPopupFeatures& popupFeatures,
                            CefWindowInfo& windowInfo,
                            CefRefPtr<CefClient>& client,
                            CefBrowserSettings& settings,
                            CefRefPtr<CefDictionaryValue>& extra_info,
                            bool* no_javascript_access)
{
    ASSERT(CefCurrentlyOn(TID_UI));
    if (!user_gesture) {
        //Automatic popup, block it directly
        return true;
    }
#if CEF_VERSION_MAJOR > 109
    if (target_disposition == CEF_WOD_NEW_POPUP) {
#else
    if (target_disposition == WOD_NEW_POPUP) {
#endif
        //Open a new popup window (this will make browser->IsPopup() return true)
        Dpi().ScaleInt(windowInfo.bounds.height);
        Dpi().ScaleInt(windowInfo.bounds.width);
        return false;
    }
    else if ((browser != nullptr) && (browser->GetMainFrame() != nullptr) && !target_url.empty()) {
        //Navigate to the popup URL
        browser->GetMainFrame()->LoadURL(target_url);
    }
    return true;
}

void CefForm::OnBeforePopupAborted(CefRefPtr<CefBrowser> browser, int popup_id)
{
    ASSERT(CefCurrentlyOn(TID_UI));
}

bool CefForm::OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             CefRefPtr<CefRequest> request,
                             bool user_gesture,
                             bool is_redirect)
{
    ASSERT(CefCurrentlyOn(TID_UI));
    return false;
}

cef_return_value_t CefForm::OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
                                                 CefRefPtr<CefFrame> frame,
                                                 CefRefPtr<CefRequest> request,
                                                 CefRefPtr<CefCallback> callback)
{
    ASSERT(CefCurrentlyOn(TID_IO));
    return RV_CONTINUE;
}

void CefForm::OnResourceRedirect(CefRefPtr<CefBrowser> browser,
                                 CefRefPtr<CefFrame> frame,
                                 CefRefPtr<CefRequest> request,
                                 CefRefPtr<CefResponse> response,
                                 CefString& new_url)
{
    ASSERT(CefCurrentlyOn(TID_IO));
}
    
bool CefForm::OnResourceResponse(CefRefPtr<CefBrowser> browser,
                                 CefRefPtr<CefFrame> frame,
                                 CefRefPtr<CefRequest> request,
                                 CefRefPtr<CefResponse> response)
{
    ASSERT(CefCurrentlyOn(TID_IO));
    return false;
}

void CefForm::OnResourceLoadComplete(CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefFrame> frame,
                                     CefRefPtr<CefRequest> request,
                                     CefRefPtr<CefResponse> response,
                                     cef_urlrequest_status_t status,
                                     int64_t received_content_length)
{
    ASSERT(CefCurrentlyOn(TID_IO));
}

void CefForm::OnProtocolExecution(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  CefRefPtr<CefRequest> request,
                                  bool& allow_os_execution)
{
    ASSERT(CefCurrentlyOn(TID_IO));
}

void CefForm::OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward)
{
    ui::GlobalManager::Instance().AssertUIThread();
    ui::Control* pControl = FindControl(_T("btn_back"));
    if ((pControl != nullptr) && (m_pCefControl != nullptr)) {
        pControl->SetEnabled(m_pCefControl->CanGoBack());
    }

    pControl = FindControl(_T("btn_forward"));
    if ((pControl != nullptr) && (m_pCefControl != nullptr)) {
        pControl->SetEnabled(m_pCefControl->CanGoForward());
    }
}
    
void CefForm::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, cef_transition_type_t transition_type)
{
    ui::GlobalManager::Instance().AssertUIThread();
}
    
void CefForm::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
    // Register methods for the frontend to call
    if (m_pCefControl != nullptr) {
        //Show a MessageBox
        m_pCefControl->RegisterCppFunc(_T("ShowMessageBox"), ToWeakCallback([this](const std::string& params, ui::ReportResultFunction callback) {
            DString value = ui::StringConvert::UTF8ToT(params);
            ui::SystemUtil::ShowMessageBox(this, value.c_str(), _T("C++ received a message from JavaScript"));
            callback(false, R"({ "message": "Success." })");
            }));

        //Receive the file drop operation from the web page
        m_pCefControl->RegisterCppFunc(_T("OnDropFilesToBrowser"), ToWeakCallback([this](const std::string& params, ui::ReportResultFunction callback) {
            DString jsonDropFileList = ui::StringConvert::UTF8ToT(params);            
            callback(false, R"({ "message": "Success." })");
            OnDropFiles(jsonDropFileList);
            }));
    }
}

void CefForm::OnDropFiles(const DString& jsonDropFileList)
{
    ui::SystemUtil::ShowMessageBox(this, jsonDropFileList.c_str(), _T("CefForm::OnDropFiles: C++ received a message from JavaScript"));
    //Business logic
    //1. Parse the json and extract the file names and file sizes from jsonDropFileList (the web page cannot get the local paths of files)
    //2. Compare whether the files in the two file lists (m_dropFileList, jsonDropFileList) are the same (file count, file names, file sizes)
    //3. Process the business logic according to the file paths in the m_dropFileList list
}
    
void CefForm::OnLoadError(CefRefPtr<CefBrowser> browser,
                          CefRefPtr<CefFrame> frame,
                          cef_errorcode_t errorCode,
                          const DString& errorText,
                          const DString& failedUrl)
{
    ui::GlobalManager::Instance().AssertUIThread();
}

void CefForm::OnDevToolAttachedStateChange(bool bVisible)
{
    ui::GlobalManager::Instance().AssertUIThread();
}

bool CefForm::OnCanDownload(CefRefPtr<CefBrowser> browser,
                            const CefString& url,
                            const CefString& request_method)
{
    ASSERT(CefCurrentlyOn(TID_UI));
    return true;
}

bool CefForm::OnBeforeDownload(CefRefPtr<CefBrowser> browser,
                               CefRefPtr<CefDownloadItem> download_item,
                               const CefString& suggested_name,
                               CefRefPtr<CefBeforeDownloadCallback> callback)
{
    ASSERT(CefCurrentlyOn(TID_UI));
    return true;
}

void CefForm::OnDownloadUpdated(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefDownloadItem> download_item,
                                CefRefPtr<CefDownloadItemCallback> callback)
{
    ASSERT(CefCurrentlyOn(TID_UI));
}

bool CefForm::OnFileDialog(CefRefPtr<CefBrowser> browser,
                           cef_file_dialog_mode_t mode,
                           const CefString& title,
                           const CefString& default_file_path,
                           const std::vector<CefString>& accept_filters,
                           const std::vector<CefString>& accept_extensions,
                           const std::vector<CefString>& accept_descriptions,
                           CefRefPtr<CefFileDialogCallback> callback)
{
    ASSERT(CefCurrentlyOn(TID_UI));
    return false;
}

void CefForm::OnDocumentAvailableInMainFrame(CefRefPtr<CefBrowser> browser)
{
    ui::GlobalManager::Instance().AssertUIThread();
}

void CefForm::OnDownloadFavIconFinished(CefRefPtr<CefBrowser> browser,
                                        const CefString& image_url,
                                        int http_status_code,
                                        CefRefPtr<CefImage> image)
{
    ui::GlobalManager::Instance().AssertUIThread();
}

void CefForm::OnDevToolVisibleStateChanged(bool bVisible, bool bPopup)
{
    ui::GlobalManager::Instance().AssertUIThread();
    if (bPopup || !bVisible) {
        if (m_pCefControlDev != nullptr) {
            m_pCefControlDev->SetFadeVisible(false);
        }
    }
    else if (bVisible && !bPopup) {
        if (m_pCefControlDev != nullptr) {
            m_pCefControlDev->SetFadeVisible(true);
        }
    }
}
