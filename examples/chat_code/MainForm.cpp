#include "MainForm.h"
#include "dui/Utils/UiBuilder.h"

void MainForm::SetupWindow()
{
    if (m_layoutType == kWechat) {
        SetWindowSize(1024, 768);
        CenterWindow();
    }
    SetShadowAttached(true);
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    SetLayeredWindow(false, false);
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
    auto* pCaptionBar = ui::Create<ui::HBox>(this, {{DUI_T("name"), DUI_T("window_caption_bar")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("36")}, {DUI_T("bkcolor"), DUI_T("bk_wnd_lightcolor")}});
    ui::Attach(pRoot, pCaptionBar);

    auto* pCaptionFiller = ui::Create<ui::Control>(this, {{DUI_T("mouse_enabled"), DUI_T("false")}});
    ui::Attach(pCaptionBar, pCaptionFiller);

    auto* pFullscreenBtn = ui::Create<ui::Button>(this, {{DUI_T("class"), DUI_T("btn_wnd_fullscreen_11")}, {DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("name"), DUI_T("fullscreenbtn")}, {DUI_T("margin"), DUI_T("0,2,0,2")}, {DUI_T("tooltip_text"), DUI_T("Fullscreen, press ESC to exit fullscreen")}});
    ui::Attach(pCaptionBar, pFullscreenBtn);

    auto* pMinBtn = ui::Create<ui::Button>(this, {{DUI_T("class"), DUI_T("btn_wnd_min_11")}, {DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("name"), DUI_T("minbtn")}, {DUI_T("margin"), DUI_T("0,2,0,2")}, {DUI_T("tooltip_text"), DUI_T("Minimize")}});
    ui::Attach(pCaptionBar, pMinBtn);

    auto* pCloseBtn = ui::Create<ui::Button>(this, {{DUI_T("class"), DUI_T("btn_wnd_close_11")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("name"), DUI_T("closebtn")}, {DUI_T("margin"), DUI_T("0,0,0,2")}, {DUI_T("tooltip_text"), DUI_T("Close")}});
    ui::Attach(pCaptionBar, pCloseBtn);

    // Main content area
    auto* pContent = ui::Create<ui::HBox>(this, {});
    ui::Attach(pRoot, pContent);

    // Left menu bar
    auto* pLeftMenu = ui::Create<ui::VBox>(this, {{DUI_T("width"), DUI_T("60")}});
    pLeftMenu->SetBkColor(DUI_T("darkcolor"));
    ui::Attach(pContent, pLeftMenu);

    auto* pHead = ui::Create<ui::Control>(this, {{DUI_T("width"), DUI_T("40")}, {DUI_T("height"), DUI_T("40")}, {DUI_T("margin"), DUI_T("10,10")}});
    pHead->SetBkImage(DUI_T("head.png"));
    ui::Attach(pLeftMenu, pHead);

    // Friend list
    auto* pFriendList = ui::Create<ui::VBox>(this, {{DUI_T("width"), DUI_T("270")}});
    pFriendList->SetBkColor(DUI_T("light_gray"));
    ui::Attach(pContent, pFriendList);

    auto* pSearchBox = ui::Create<ui::Box>(this, {{DUI_T("padding"), DUI_T("10,8,10,8")}, {DUI_T("height"), DUI_T("auto")}});
    ui::Attach(pFriendList, pSearchBox);

    auto* pNickname = ui::Create<ui::RichEdit>(this, {{DUI_T("class"), DUI_T("simple simple_border prompt")}, {DUI_T("name"), DUI_T("nickname")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("35")}, {DUI_T("font"), DUI_T("system_14")}, {DUI_T("text_align"), DUI_T("vcenter")}, {DUI_T("text_padding"), DUI_T("25,8,8,8")}, {DUI_T("borderround"), DUI_T("5,5")}, {DUI_T("prompttext"), DUI_T("Search")}});
    ui::Attach(pSearchBox, pNickname);

    auto* pSearchIcon = ui::Create<ui::Control>(this, {{DUI_T("width"), DUI_T("auto")}, {DUI_T("height"), DUI_T("auto")}, {DUI_T("margin"), DUI_T("6,9")}});
    pSearchIcon->SetBkImage(DUI_T("search_icon.png"));
    ui::Attach(pSearchBox, pSearchIcon);

    auto* pFriendContent = ui::Create<ui::VBox>(this, {});
    ui::Attach(pFriendList, pFriendContent);

    // Chat area
    auto* pChatArea = ui::Create<ui::VBox>(this, {});
    ui::Attach(pContent, pChatArea);

    auto* pChatTitle = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("80")}});
    pChatTitle->SetBkColor(DUI_T("bk_wnd_darkcolor"));
    ui::Attach(pChatArea, pChatTitle);

    auto* pSplitLine = ui::Create<ui::Control>(this, {});
    pSplitLine->SetClass(DUI_T("splitline_hor_level1"));
    ui::Attach(pChatArea, pSplitLine);

    auto* pChatContent = ui::Create<ui::VBox>(this, {});
    pChatContent->SetBkColor(DUI_T("bk_wnd_darkcolor"));
    ui::Attach(pChatArea, pChatContent);

    auto* pChatInput = ui::Create<ui::VBox>(this, {{DUI_T("height"), DUI_T("150")}});
    pChatInput->SetBkColor(DUI_T("bk_wnd_lightcolor"));
    ui::Attach(pChatArea, pChatInput);

    ui::Attach(this, pRoot);
}

void MainForm::BuildLoginUI()
{
    // Corresponds to the login.xml layout
    auto* pRoot = ui::Create<ui::VBox>(this, {{DUI_T("width"), DUI_T("304")}, {DUI_T("height"), DUI_T("auto")}, {DUI_T("bkcolor"), DUI_T("bk_wnd_lightcolor")}});

    // macOS traffic lights are inserted into this caption bar by WindowImplBase.
    auto* pCaptionBar = ui::Create<ui::HBox>(this, {{DUI_T("name"), DUI_T("window_caption_bar")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("36")}, {DUI_T("bkcolor"), DUI_T("bk_wnd_lightcolor")}});
    auto* pCaptionFiller = ui::Create<ui::Control>(this, {{DUI_T("mouse_enabled"), DUI_T("false")}});
    ui::Attach(pCaptionBar, pCaptionFiller);
    ui::Attach(pRoot, pCaptionBar);

    auto* pContent = ui::Create<ui::Box>(this, {{DUI_T("width"), DUI_T("304")}, {DUI_T("height"), DUI_T("auto")}});
    ui::Attach(pRoot, pContent);

    auto* pHeadBox = ui::Create<ui::Box>(this, {{DUI_T("width"), DUI_T("130")}, {DUI_T("height"), DUI_T("130")}, {DUI_T("margin"), DUI_T("0,10,0,0")}, {DUI_T("halign"), DUI_T("center")}});
    ui::Attach(pContent, pHeadBox);

    auto* pHeadShadow = ui::Create<ui::Control>(this, {{DUI_T("width"), DUI_T("auto")}, {DUI_T("height"), DUI_T("auto")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("mouse_enabled"), DUI_T("false")}});
    pHeadShadow->SetBkImage(DUI_T("head_shadow.png"));
    ui::Attach(pHeadBox, pHeadShadow);

    // Login panel
    auto* pLoginPanel = ui::Create<ui::Box>(this, {{DUI_T("height"), DUI_T("auto")}, {DUI_T("margin"), DUI_T("0,50,0,0")}, {DUI_T("padding"), DUI_T("14,14,14,14")}});
    pLoginPanel->SetBkImage(DUI_T("file='bk_shadow.png' corner='30,30,30,30'"));
    ui::Attach(pContent, pLoginPanel);

    auto* pLoginContent = ui::Create<ui::VBox>(this, {{DUI_T("height"), DUI_T("auto")}, {DUI_T("borderround"), DUI_T("3,3,3,3")}});
    pLoginContent->SetBkColor(DUI_T("bk_wnd_lightcolor"));
    ui::Attach(pLoginPanel, pLoginContent);

    auto* pLoginVBox = ui::Create<ui::VBox>(this, {{DUI_T("height"), DUI_T("auto")}});
    ui::Attach(pLoginContent, pLoginVBox);

    // Title bar (minimize/close)
    auto* pCaption = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("40")}, {DUI_T("margin"), DUI_T("0,6,5,40")}});
    ui::Attach(pLoginVBox, pCaption);

    auto* pSpacer = ui::Create<ui::Control>(this, {});
    ui::Attach(pCaption, pSpacer);

    auto* pMinBtn = ui::Create<ui::Button>(this, {{DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("32")}, {DUI_T("margin"), DUI_T("4,0,4,0")}});
    pMinBtn->SetClass(DUI_T("btn_wnd_min_11"));
    pMinBtn->SetName(DUI_T("minbtn"));
    ui::Attach(pCaption, pMinBtn);

    auto* pCloseBtn = ui::Create<ui::Button>(this, {{DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("32")}});
    pCloseBtn->SetClass(DUI_T("btn_wnd_close_11"));
    pCloseBtn->SetName(DUI_T("closebtn"));
    ui::Attach(pCaption, pCloseBtn);

    // Register/Login toggle
    auto* pRegBox = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("auto")}});
    ui::Attach(pLoginVBox, pRegBox);

    auto* pSpacer2 = ui::Create<ui::Control>(this, {});
    ui::Attach(pRegBox, pSpacer2);

    auto* pRegisterBtn = ui::Create<ui::Button>(this, {{DUI_T("margin"), DUI_T("0,5,15,10")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("cursortype"), DUI_T("hand")}, {DUI_T("font"), DUI_T("system_underline_12")}, {DUI_T("normaltextcolor"), DUI_T("light_green")}});
    pRegisterBtn->SetName(DUI_T("register_account"));
    pRegisterBtn->SetText(DUI_T("Register"));
    ui::Attach(pRegBox, pRegisterBtn);

    auto* pEnterLoginBtn = ui::Create<ui::Button>(this, {{DUI_T("margin"), DUI_T("0,5,15,10")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("cursortype"), DUI_T("hand")}, {DUI_T("font"), DUI_T("system_underline_12")}, {DUI_T("normaltextcolor"), DUI_T("light_green")}});
    pEnterLoginBtn->SetName(DUI_T("enter_login"));
    pEnterLoginBtn->SetText(DUI_T("Login Directly"));
    pEnterLoginBtn->SetVisible(false);
    ui::Attach(pRegBox, pEnterLoginBtn);

    // Account and password
    auto* pEnterPanel = ui::Create<ui::VBox>(this, {{DUI_T("width"), DUI_T("240")}, {DUI_T("height"), DUI_T("auto")}, {DUI_T("margin"), DUI_T("20,0,20,0")}});
    pEnterPanel->SetName(DUI_T("enter_panel"));
    pEnterPanel->SetBkImage(DUI_T("user_password.png"));
    ui::Attach(pLoginVBox, pEnterPanel);

    auto* pUserRow = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("41")}, {DUI_T("padding"), DUI_T("14,1,10,0")}});
    ui::Attach(pEnterPanel, pUserRow);

    auto* pUserIcon = ui::Create<ui::Control>(this, {{DUI_T("width"), DUI_T("16")}, {DUI_T("height"), DUI_T("16")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("normalimage"), DUI_T("user_1.png")}, {DUI_T("disabledimage"), DUI_T("user_2.png")}});
    pUserIcon->SetName(DUI_T("usericon"));
    ui::Attach(pUserRow, pUserIcon);

    auto* pUsername = ui::Create<ui::RichEdit>(this, {{DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("auto")}, {DUI_T("margin"), DUI_T("10,0,10,0")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("font"), DUI_T("system_14")}, {DUI_T("prompttext"), DUI_T("Account")}});
    pUsername->SetClass(DUI_T("simple prompt"));
    pUsername->SetName(DUI_T("username"));
    ui::Attach(pUserRow, pUsername);

    // Nickname row (shown during registration)
    auto* pNickRow = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("40")}, {DUI_T("padding"), DUI_T("14,1,0,0")}});
    pNickRow->SetName(DUI_T("nick_name_panel"));
    pNickRow->SetVisible(false);
    ui::Attach(pEnterPanel, pNickRow);

    auto* pNickIcon = ui::Create<ui::Control>(this, {{DUI_T("width"), DUI_T("16")}, {DUI_T("height"), DUI_T("16")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("normalimage"), DUI_T("nickname.png")}, {DUI_T("disabledimage"), DUI_T("nickname.png")}});
    pNickIcon->SetName(DUI_T("nick_name_icon"));
    ui::Attach(pNickRow, pNickIcon);

    auto* pNickname = ui::Create<ui::RichEdit>(this, {{DUI_T("class"), DUI_T("simple prompt")}, {DUI_T("name"), DUI_T("nickname")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("auto")}, {DUI_T("margin"), DUI_T("10,0,10,0")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("font"), DUI_T("system_14")}, {DUI_T("prompttext"), DUI_T("Nickname")}});
    pNickname->SetClass(DUI_T("simple prompt"));
    pNickname->SetName(DUI_T("nickname"));
    ui::Attach(pNickRow, pNickname);

    // Password row
    auto* pPwdRow = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("41")}, {DUI_T("padding"), DUI_T("14,0,10,1")}, {DUI_T("margin"), DUI_T("0,2,0,0")}});
    ui::Attach(pEnterPanel, pPwdRow);

    auto* pPwdIcon = ui::Create<ui::Control>(this, {{DUI_T("width"), DUI_T("16")}, {DUI_T("height"), DUI_T("16")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("normalimage"), DUI_T("password_1.png")}, {DUI_T("disabledimage"), DUI_T("password_2.png")}});
    pPwdIcon->SetName(DUI_T("passwordicon"));
    ui::Attach(pPwdRow, pPwdIcon);

    auto* pPassword = ui::Create<ui::RichEdit>(this, {{DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("auto")}, {DUI_T("margin"), DUI_T("10,0,10,0")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("font"), DUI_T("system_14")}, {DUI_T("password"), DUI_T("true")}, {DUI_T("prompttext"), DUI_T("Password")}});
    pPassword->SetClass(DUI_T("simple prompt"));
    pPassword->SetName(DUI_T("password"));
    ui::Attach(pPwdRow, pPassword);

    // Login hint
    auto* pLoginIngTip = ui::Create<ui::Label>(this, {{DUI_T("margin"), DUI_T("20,20,20,0")}, {DUI_T("font"), DUI_T("system_12")}});
    pLoginIngTip->SetName(DUI_T("login_ing_tip"));
    pLoginIngTip->SetText(DUI_T("Logging in..."));
    pLoginIngTip->SetVisible(false);
    ui::Attach(pLoginContent, pLoginIngTip);

    auto* pLoginErrorTip = ui::Create<ui::Label>(this, {{DUI_T("margin"), DUI_T("20,20,20,0")}, {DUI_T("font"), DUI_T("system_12")}, {DUI_T("normaltextcolor"), DUI_T("obvious_tip")}});
    pLoginErrorTip->SetName(DUI_T("login_error_tip"));
    pLoginErrorTip->SetVisible(false);
    ui::Attach(pLoginContent, pLoginErrorTip);

    // Login/Register/Cancel buttons
    auto* pBtnBox = ui::Create<ui::Box>(this, {{DUI_T("width"), DUI_T("240")}, {DUI_T("height"), DUI_T("40")}, {DUI_T("margin"), DUI_T("20,20,20,20")}});
    ui::Attach(pLoginContent, pBtnBox);

    auto* pLoginBtn = ui::Create<ui::Button>(this, {{DUI_T("width"), DUI_T("240")}, {DUI_T("height"), DUI_T("40")}, {DUI_T("font"), DUI_T("system_bold_16")}});
    pLoginBtn->SetClass(DUI_T("btn_global_blue_80x30"));
    pLoginBtn->SetName(DUI_T("btn_login"));
    pLoginBtn->SetText(DUI_T("Login"));
    ui::Attach(pBtnBox, pLoginBtn);

    auto* pRegisterMainBtn = ui::Create<ui::Button>(this, {{DUI_T("width"), DUI_T("240")}, {DUI_T("height"), DUI_T("40")}, {DUI_T("font"), DUI_T("system_bold_16")}});
    pRegisterMainBtn->SetClass(DUI_T("btn_global_blue_80x30"));
    pRegisterMainBtn->SetName(DUI_T("btn_register"));
    pRegisterMainBtn->SetText(DUI_T("Register"));
    pRegisterMainBtn->SetVisible(false);
    ui::Attach(pBtnBox, pRegisterMainBtn);

    auto* pCancelBtn = ui::Create<ui::Button>(this, {{DUI_T("width"), DUI_T("240")}, {DUI_T("height"), DUI_T("40")}, {DUI_T("font"), DUI_T("system_bold_16")}});
    pCancelBtn->SetClass(DUI_T("btn_global_red_80x30"));
    pCancelBtn->SetName(DUI_T("btn_cancel"));
    pCancelBtn->SetText(DUI_T("Cancel Login"));
    pCancelBtn->SetVisible(false);
    ui::Attach(pBtnBox, pCancelBtn);

    // Avatar
    auto* pAvatarBox = ui::Create<ui::Box>(this, {{DUI_T("margin"), DUI_T("0,10,0,0")}, {DUI_T("mouse_enabled"), DUI_T("false")}, {DUI_T("width"), DUI_T("130")}, {DUI_T("height"), DUI_T("130")}, {DUI_T("halign"), DUI_T("center")}});
    ui::Attach(pContent, pAvatarBox);

    auto* pLogo = ui::Create<ui::Control>(this, {{DUI_T("width"), DUI_T("auto")}, {DUI_T("height"), DUI_T("auto")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}});
    pLogo->SetBkImage(DUI_T("logo.png"));
    ui::Attach(pAvatarBox, pLogo);

    auto* pHeadIcon = ui::Create<ui::Button>(this, {{DUI_T("width"), DUI_T("106")}, {DUI_T("height"), DUI_T("106")}, {DUI_T("border_round"), DUI_T("106,106,106,106")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("mouse_enabled"), DUI_T("false")}, {DUI_T("fade_alpha"), DUI_T("true")}});
    pHeadIcon->SetName(DUI_T("headicon"));
    ui::Attach(pAvatarBox, pHeadIcon);

    auto* pMask = ui::Create<ui::Control>(this, {{DUI_T("width"), DUI_T("auto")}, {DUI_T("height"), DUI_T("auto")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("mouse_enabled"), DUI_T("false")}});
    pMask->SetBkImage(DUI_T("mask.png"));
    ui::Attach(pAvatarBox, pMask);

    ui::Attach(this, pRoot);
}

void MainForm::OnInitWindow()
{
    SetupWindow();
    BuildUI();

    if (m_layoutType == kLogin) {
        // The pure-code tree is created after SetupWindow; apply the full
        // content size once the root exists (matches login.xml).
        SetWindowSize(304, 696);
        CenterWindow();
        // A layout example mimicking the WeChat window (layout built by C++ code),
        // shown behind the login window.
        ShowCustomWindow(kWechat);
    }

    BindEvents();
    BaseClass::OnInitWindow();
}

void MainForm::BindEvents()
{
}

void MainForm::ShowCustomWindow(LayoutType layoutType)
{
    MainForm* window = new MainForm(layoutType);
    ui::WindowCreateParam createParam(DUI_T("chat (Pure Code)"), true);
    if (layoutType == kWechat) {
        createParam.m_nWidth = 1024;
        createParam.m_nHeight = 768;
    }
    window->CreateWnd(nullptr, createParam);
    window->PostQuitMsgWhenClosed(true);
    window->ShowWindow(ui::kSW_SHOW_NA);
}
