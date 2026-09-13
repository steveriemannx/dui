#include "MainForm.h"
#include "dui/Utils/UiBuilder.h"

void MainForm::SetupWindow()
{
    if (m_layoutType == kWechat) {
        SetWindowSize(1024, 768);
        CenterWindow();
    }
    SetShadowAttached(true);
#if defined(DUI_BUILD_FOR_LINUX)
    SetShadowType(ui::Shadow::ShadowType::kShadowDrawDefault);
#else
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    SetLayeredWindow(false, false);
#endif
    SetEnableShadowSnap(true);
    SetShadowBorderSize(0);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), false);
}

void MainForm::BuildUI()
{
    if (m_layoutType == kLogin) {
        BuildLoginUI();
    }
    else {
        BuildWechatUI();
    }
}

void MainForm::BuildWechatUI()
{
    // Corresponds to the wechat.xml layout
    auto* pRoot = ui::Create<ui::VBox>(this, {});

    // Window caption bar (macOS traffic lights are inserted by the framework)
    auto* pCaptionBar = ui::Create<ui::HBox>(this, {{"name", "window_caption_bar"}, {"width", "stretch"}, {"height", "36"}, {"bkcolor", "bk_wnd_lightcolor"}});
    ui::Attach(pRoot, pCaptionBar);

    auto* pCaptionFiller = ui::Create<ui::Control>(this, {{"mouse_enabled", "false"}});
    ui::Attach(pCaptionBar, pCaptionFiller);

    auto* pFullscreenBtn = ui::Create<ui::Button>(this, {{"class", "btn_wnd_fullscreen_11"}, {"height", "32"}, {"width", "40"}, {"name", "fullscreenbtn"}, {"margin", "0,2,0,2"}, {"tooltip_text", "Fullscreen, press ESC to exit fullscreen"}});
    ui::Attach(pCaptionBar, pFullscreenBtn);

    auto* pMinBtn = ui::Create<ui::Button>(this, {{"class", "btn_wnd_min_11"}, {"height", "32"}, {"width", "40"}, {"name", "minbtn"}, {"margin", "0,2,0,2"}, {"tooltip_text", "Minimize"}});
    ui::Attach(pCaptionBar, pMinBtn);

    auto* pMaxBox = ui::Create<ui::Box>(this, {{"height", "stretch"}, {"width", "40"}, {"margin", "0,2,0,2"}});
    ui::Attach(pCaptionBar, pMaxBox);

    auto* pMaxBtn = ui::Create<ui::Button>(this, {{"class", "btn_wnd_max_11"}, {"height", "32"}, {"width", "stretch"}, {"name", "maxbtn"}, {"tooltip_text", "Maximize"}});
    ui::Attach(pMaxBox, pMaxBtn);

    auto* pRestoreBtn = ui::Create<ui::Button>(this, {{"class", "btn_wnd_restore_11"}, {"height", "32"}, {"width", "stretch"}, {"name", "restorebtn"}, {"visible", "false"}, {"tooltip_text", "Restore"}});
    ui::Attach(pMaxBox, pRestoreBtn);

    auto* pCloseBtn = ui::Create<ui::Button>(this, {{"class", "btn_wnd_close_11"}, {"height", "stretch"}, {"width", "40"}, {"name", "closebtn"}, {"margin", "0,0,0,2"}, {"tooltip_text", "Close"}});
    ui::Attach(pCaptionBar, pCloseBtn);

    // Main content area
    auto* pContent = ui::Create<ui::HBox>(this, {});
    ui::Attach(pRoot, pContent);

    // Left menu bar
    auto* pLeftMenu = ui::Create<ui::VBox>(this, {{"width", "60"}});
    pLeftMenu->SetBkColor("darkcolor");
    ui::Attach(pContent, pLeftMenu);

    auto* pHead = ui::Create<ui::Control>(this, {{"width", "40"}, {"height", "40"}, {"margin", "10,10"}});
    pHead->SetBkImage("head.png");
    ui::Attach(pLeftMenu, pHead);

    // Friend list
    auto* pFriendList = ui::Create<ui::VBox>(this, {{"width", "270"}});
    pFriendList->SetBkColor("light_gray");
    ui::Attach(pContent, pFriendList);

    auto* pSearchBox = ui::Create<ui::Box>(this, {{"padding", "10,8,10,8"}, {"height", "auto"}});
    ui::Attach(pFriendList, pSearchBox);

    auto* pNickname = ui::Create<ui::RichEdit>(this, {{"class", "simple simple_border prompt"}, {"name", "nickname"}, {"width", "stretch"}, {"height", "35"}, {"font", "system_14"}, {"text_align", "vcenter"}, {"text_padding", "25,8,8,8"}, {"borderround", "5,5"}, {"prompttext", "Search"}});
    ui::Attach(pSearchBox, pNickname);

    auto* pSearchIcon = ui::Create<ui::Control>(this, {{"width", "auto"}, {"height", "auto"}, {"margin", "6,9"}});
    pSearchIcon->SetBkImage("search_icon.png");
    ui::Attach(pSearchBox, pSearchIcon);

    auto* pFriendContent = ui::Create<ui::VBox>(this, {});
    ui::Attach(pFriendList, pFriendContent);

    // Chat area
    auto* pChatArea = ui::Create<ui::VBox>(this, {});
    ui::Attach(pContent, pChatArea);

    auto* pChatTitle = ui::Create<ui::HBox>(this, {{"height", "80"}});
    pChatTitle->SetBkColor("bk_wnd_darkcolor");
    ui::Attach(pChatArea, pChatTitle);

    auto* pSplitLine = ui::Create<ui::Control>(this, {});
    pSplitLine->SetClass("splitline_hor_level1");
    ui::Attach(pChatArea, pSplitLine);

    auto* pChatContent = ui::Create<ui::VBox>(this, {});
    pChatContent->SetBkColor("bk_wnd_darkcolor");
    ui::Attach(pChatArea, pChatContent);

    auto* pChatInput = ui::Create<ui::VBox>(this, {{"height", "150"}});
    pChatInput->SetBkColor("bk_wnd_lightcolor");
    ui::Attach(pChatArea, pChatInput);

    ui::Attach(this, pRoot);
}

void MainForm::BuildLoginUI()
{
    // Corresponds to the login.xml layout
    auto* pRoot = ui::Create<ui::VBox>(this, {{"width", "304"}, {"height", "auto"}, {"bkcolor", "bk_wnd_lightcolor"}});

    // macOS traffic lights are inserted into this caption bar by WindowImplBase.
#if defined(DUI_BUILD_FOR_MACOS)
    auto* pCaptionBar = ui::Create<ui::HBox>(this, {{"name", "window_caption_bar"}, {"width", "stretch"}, {"height", "36"}, {"bkcolor", "bk_wnd_lightcolor"}});
    auto* pCaptionFiller = ui::Create<ui::Control>(this, {{"mouse_enabled", "false"}});
    ui::Attach(pCaptionBar, pCaptionFiller);
    ui::Attach(pRoot, pCaptionBar);
#endif

    auto* pContent = ui::Create<ui::Box>(this, {{"width", "304"}, {"height", "auto"}});
    ui::Attach(pRoot, pContent);

    auto* pHeadBox = ui::Create<ui::Box>(this, {{"width", "130"}, {"height", "130"}, {"margin", "0,10,0,0"}, {"halign", "center"}});
    ui::Attach(pContent, pHeadBox);

    auto* pHeadShadow = ui::Create<ui::Control>(this, {{"width", "auto"}, {"height", "auto"}, {"halign", "center"}, {"valign", "center"}, {"mouse_enabled", "false"}});
    pHeadShadow->SetBkImage("head_shadow.png");
    ui::Attach(pHeadBox, pHeadShadow);

    // Login panel
    auto* pLoginPanel = ui::Create<ui::Box>(this, {{"height", "auto"}, {"margin", "0,50,0,0"}, {"padding", "14,14,14,14"}});
    pLoginPanel->SetBkImage("file='bk_shadow.png' corner='30,30,30,30'");
    ui::Attach(pContent, pLoginPanel);

    auto* pLoginContent = ui::Create<ui::VBox>(this, {{"height", "auto"}, {"borderround", "3,3,3,3"}});
    pLoginContent->SetBkColor("bk_wnd_lightcolor");
    ui::Attach(pLoginPanel, pLoginContent);

    auto* pLoginVBox = ui::Create<ui::VBox>(this, {{"height", "auto"}});
    ui::Attach(pLoginContent, pLoginVBox);

    // Title bar (minimize/close)
    auto* pCaption = ui::Create<ui::HBox>(this, {{"height", "40"}, {"margin", "0,6,5,40"}});
    ui::Attach(pLoginVBox, pCaption);

    auto* pSpacer = ui::Create<ui::Control>(this, {});
    ui::Attach(pCaption, pSpacer);

    auto* pMinBtn = ui::Create<ui::Button>(this, {{"height", "32"}, {"width", "32"}, {"margin", "4,0,4,0"}});
    pMinBtn->SetClass("btn_wnd_min_11");
    pMinBtn->SetName("minbtn");
    ui::Attach(pCaption, pMinBtn);

    auto* pCloseBtn = ui::Create<ui::Button>(this, {{"height", "32"}, {"width", "32"}});
    pCloseBtn->SetClass("btn_wnd_close_11");
    pCloseBtn->SetName("closebtn");
    ui::Attach(pCaption, pCloseBtn);

    // Register/Login toggle
    auto* pRegBox = ui::Create<ui::HBox>(this, {{"height", "auto"}});
    ui::Attach(pLoginVBox, pRegBox);

    auto* pSpacer2 = ui::Create<ui::Control>(this, {});
    ui::Attach(pRegBox, pSpacer2);

    auto* pRegisterBtn = ui::Create<ui::Button>(this, {{"margin", "0,5,15,10"}, {"halign", "right"}, {"cursortype", "hand"}, {"font", "system_underline_12"}, {"normaltextcolor", "light_green"}});
    pRegisterBtn->SetName("register_account");
    pRegisterBtn->SetText("Register");
    ui::Attach(pRegBox, pRegisterBtn);

    auto* pEnterLoginBtn = ui::Create<ui::Button>(this, {{"margin", "0,5,15,10"}, {"halign", "right"}, {"cursortype", "hand"}, {"font", "system_underline_12"}, {"normaltextcolor", "light_green"}});
    pEnterLoginBtn->SetName("enter_login");
    pEnterLoginBtn->SetText("Login Directly");
    pEnterLoginBtn->SetVisible(false);
    ui::Attach(pRegBox, pEnterLoginBtn);

    // Account and password
    auto* pEnterPanel = ui::Create<ui::VBox>(this, {{"width", "240"}, {"height", "auto"}, {"margin", "20,0,20,0"}});
    pEnterPanel->SetName("enter_panel");
    pEnterPanel->SetBkImage("user_password.png");
    ui::Attach(pLoginVBox, pEnterPanel);

    auto* pUserRow = ui::Create<ui::HBox>(this, {{"height", "41"}, {"padding", "14,1,10,0"}});
    ui::Attach(pEnterPanel, pUserRow);

    auto* pUserIcon = ui::Create<ui::Control>(this, {{"width", "16"}, {"height", "16"}, {"valign", "center"}, {"normalimage", "user_1.png"}, {"disabledimage", "user_2.png"}});
    pUserIcon->SetName("usericon");
    ui::Attach(pUserRow, pUserIcon);

    auto* pUsername = ui::Create<ui::RichEdit>(this, {{"width", "stretch"}, {"height", "auto"}, {"margin", "10,0,10,0"}, {"valign", "center"}, {"font", "system_14"}, {"prompttext", "Account"}});
    pUsername->SetClass("simple prompt");
    pUsername->SetName("username");
    ui::Attach(pUserRow, pUsername);

    // Nickname row (shown during registration)
    auto* pNickRow = ui::Create<ui::HBox>(this, {{"height", "40"}, {"padding", "14,1,0,0"}});
    pNickRow->SetName("nick_name_panel");
    pNickRow->SetVisible(false);
    ui::Attach(pEnterPanel, pNickRow);

    auto* pNickIcon = ui::Create<ui::Control>(this, {{"width", "16"}, {"height", "16"}, {"valign", "center"}, {"normalimage", "nickname.png"}, {"disabledimage", "nickname.png"}});
    pNickIcon->SetName("nick_name_icon");
    ui::Attach(pNickRow, pNickIcon);

    auto* pNickname = ui::Create<ui::RichEdit>(this, {{"class", "simple prompt"}, {"name", "nickname"}, {"width", "stretch"}, {"height", "auto"}, {"margin", "10,0,10,0"}, {"valign", "center"}, {"font", "system_14"}, {"prompttext", "Nickname"}});
    pNickname->SetClass("simple prompt");
    pNickname->SetName("nickname");
    ui::Attach(pNickRow, pNickname);

    // Password row
    auto* pPwdRow = ui::Create<ui::HBox>(this, {{"height", "41"}, {"padding", "14,0,10,1"}, {"margin", "0,2,0,0"}});
    ui::Attach(pEnterPanel, pPwdRow);

    auto* pPwdIcon = ui::Create<ui::Control>(this, {{"width", "16"}, {"height", "16"}, {"valign", "center"}, {"normalimage", "password_1.png"}, {"disabledimage", "password_2.png"}});
    pPwdIcon->SetName("passwordicon");
    ui::Attach(pPwdRow, pPwdIcon);

    auto* pPassword = ui::Create<ui::RichEdit>(this, {{"width", "stretch"}, {"height", "auto"}, {"margin", "10,0,10,0"}, {"valign", "center"}, {"font", "system_14"}, {"password", "true"}, {"prompttext", "Password"}});
    pPassword->SetClass("simple prompt");
    pPassword->SetName("password");
    ui::Attach(pPwdRow, pPassword);

    // Login hint
    auto* pLoginIngTip = ui::Create<ui::Label>(this, {{"margin", "20,20,20,0"}, {"font", "system_12"}});
    pLoginIngTip->SetName("login_ing_tip");
    pLoginIngTip->SetText("Logging in...");
    pLoginIngTip->SetVisible(false);
    ui::Attach(pLoginContent, pLoginIngTip);

    auto* pLoginErrorTip = ui::Create<ui::Label>(this, {{"margin", "20,20,20,0"}, {"font", "system_12"}, {"normaltextcolor", "obvious_tip"}});
    pLoginErrorTip->SetName("login_error_tip");
    pLoginErrorTip->SetVisible(false);
    ui::Attach(pLoginContent, pLoginErrorTip);

    // Login/Register/Cancel buttons
    auto* pBtnBox = ui::Create<ui::Box>(this, {{"width", "240"}, {"height", "40"}, {"margin", "20,20,20,20"}});
    ui::Attach(pLoginContent, pBtnBox);

    auto* pLoginBtn = ui::Create<ui::Button>(this, {{"width", "240"}, {"height", "40"}, {"font", "system_bold_16"}});
    pLoginBtn->SetClass("btn_global_blue_80x30");
    pLoginBtn->SetName("btn_login");
    pLoginBtn->SetText("Login");
    ui::Attach(pBtnBox, pLoginBtn);

    auto* pRegisterMainBtn = ui::Create<ui::Button>(this, {{"width", "240"}, {"height", "40"}, {"font", "system_bold_16"}});
    pRegisterMainBtn->SetClass("btn_global_blue_80x30");
    pRegisterMainBtn->SetName("btn_register");
    pRegisterMainBtn->SetText("Register");
    pRegisterMainBtn->SetVisible(false);
    ui::Attach(pBtnBox, pRegisterMainBtn);

    auto* pCancelBtn = ui::Create<ui::Button>(this, {{"width", "240"}, {"height", "40"}, {"font", "system_bold_16"}});
    pCancelBtn->SetClass("btn_global_red_80x30");
    pCancelBtn->SetName("btn_cancel");
    pCancelBtn->SetText("Cancel Login");
    pCancelBtn->SetVisible(false);
    ui::Attach(pBtnBox, pCancelBtn);

    // Avatar
    auto* pAvatarBox = ui::Create<ui::Box>(this, {{"margin", "0,10,0,0"}, {"mouse_enabled", "false"}, {"width", "130"}, {"height", "130"}, {"halign", "center"}});
    ui::Attach(pContent, pAvatarBox);

    auto* pLogo = ui::Create<ui::Control>(this, {{"width", "auto"}, {"height", "auto"}, {"halign", "center"}, {"valign", "center"}});
    pLogo->SetBkImage("logo.png");
    ui::Attach(pAvatarBox, pLogo);

    auto* pHeadIcon = ui::Create<ui::Button>(this, {{"width", "106"}, {"height", "106"}, {"border_round", "106,106,106,106"}, {"halign", "center"}, {"valign", "center"}, {"mouse_enabled", "false"}, {"fade_alpha", "true"}});
    pHeadIcon->SetName("headicon");
    ui::Attach(pAvatarBox, pHeadIcon);

    auto* pMask = ui::Create<ui::Control>(this, {{"width", "auto"}, {"height", "auto"}, {"halign", "center"}, {"valign", "center"}, {"mouse_enabled", "false"}});
    pMask->SetBkImage("mask.png");
    ui::Attach(pAvatarBox, pMask);

    ui::Attach(this, pRoot);
}

void MainForm::OnInitWindow()
{
    SetupWindow();
    BuildUI();

    if (m_layoutType == kLogin) {
        // Size the window from the actual pure-code tree before centering it.
        // The old fixed height left the visible login panel above center.
        if (ui::Box* root = GetRoot()) {
            const ui::UiEstSize size = root->EstimateSize(ui::UiSize(999999, 999999));
            SetWindowSize(size.cx.GetInt32(), size.cy.GetInt32());
        }
        CenterWindow();
        // A layout example mimicking the WeChat window (layout built by C++ code),
        // shown behind the login window.
        ShowCustomWindow(kWechat);
    }

    BindEvents();
    BaseClass::OnInitWindow();
}

void MainForm::OnInitLayout()
{
    BaseClass::OnInitLayout();
    if (m_layoutType == kLogin) {
        CenterWindow();
    }
}

void MainForm::BindEvents()
{
}

void MainForm::ShowCustomWindow(LayoutType layoutType)
{
    MainForm* window = new MainForm(layoutType);
    ui::WindowCreateParam createParam("chat (Pure Code)", true);
    if (layoutType == kWechat) {
        createParam.m_nWidth = 1024;
        createParam.m_nHeight = 768;
    }
    window->CreateWnd(nullptr, createParam);
    window->PostQuitMsgWhenClosed(true);
    window->ShowWindow(ui::kSW_SHOW_NA);
}
