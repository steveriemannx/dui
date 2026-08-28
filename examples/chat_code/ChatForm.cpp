#include "ChatForm.h"

ChatForm::ChatForm(LayoutType layoutType):
    m_layoutType(layoutType)
{
}

ChatForm::~ChatForm()
{
}

DString ChatForm::GetSkinFolder()
{
    return _T("chat");
}

DString ChatForm::GetSkinFile()
{
    // Pure code mode: no layout XML is loaded
    return _T("");
}

void ChatForm::GetCreateWindowAttributes(ui::WindowCreateAttributes& attrs)
{
    if (m_layoutType == kWechat) {
        //Corresponds to the <Window> attributes of wechat.xml
        attrs.m_bInitSizeDefined = true;
        attrs.m_szInitSize.cx = 1024;
        attrs.m_szInitSize.cy = 768;
        attrs.m_bShadowAttached = true;
        attrs.m_bShadowAttachedDefined = true;
        attrs.m_bIsLayeredWindow = true;
        attrs.m_bIsLayeredWindowDefined = true;
        attrs.m_rcCaption = ui::UiRect(0, 0, 0, 36);
        attrs.m_bCaptionDefined = true;
    }
    else {
        //Corresponds to the <Window> attributes of login.xml (no explicit size:
        //the root box is width=304/height=auto, so AutoResizeWindow fits the
        //window to its content)
        attrs.m_bShadowAttached = true;
        attrs.m_bShadowAttachedDefined = true;
        attrs.m_bIsLayeredWindow = true;
        attrs.m_bIsLayeredWindowDefined = true;
        attrs.m_rcCaption = ui::UiRect(0, 0, 0, 36);
        attrs.m_bCaptionDefined = true;
    }

    BaseClass::GetCreateWindowAttributes(attrs);
}

void ChatForm::PreInitWindow()
{
    BaseClass::PreInitWindow();

    //No layout XML is loaded, so Window::ParseWindowXml cannot establish the
    //window resource sub-path; set it explicitly so image paths resolve from
    //the "chat" folder.
    SetResourcePath(ui::FilePath(_T("chat")));
}

void ChatForm::BuildWechatUI()
{
    // Corresponds to the wechat.xml layout
    auto* pRoot = ui::Create<ui::HBox>(this, {{_T("width"), _T("304")}, {_T("height"), _T("auto")}});

    // Left menu bar
    auto* pLeftMenu = ui::Create<ui::VBox>(this, {{_T("width"), _T("60")}});
    pLeftMenu->SetBkColor(_T("darkcolor"));
    pRoot->AddItem(pLeftMenu);

    auto* pHead = ui::Create<ui::Control>(this, {{_T("width"), _T("40")}, {_T("height"), _T("40")}, {_T("margin"), _T("10,10")}});
    pHead->SetBkImage(_T("head.png"));
    pLeftMenu->AddItem(pHead);

    // Friend list
    auto* pFriendList = ui::Create<ui::VBox>(this, {{_T("width"), _T("270")}});
    pFriendList->SetBkColor(_T("light_gray"));
    pRoot->AddItem(pFriendList);

    auto* pSearchBox = ui::Create<ui::Box>(this, {{_T("padding"), _T("10,8,10,8")}, {_T("height"), _T("auto")}});
    pFriendList->AddItem(pSearchBox);

    auto* pNickname = ui::Create<ui::RichEdit>(this, {{_T("width"), _T("stretch")}, {_T("height"), _T("35")}, {_T("font"), _T("system_14")}, {_T("text_align"), _T("vcenter")}, {_T("text_padding"), _T("25,8,8,8")}, {_T("borderround"), _T("5,5")}, {_T("prompttext"), _T("Search")}, {_T("width"), _T("stretch")}, {_T("height"), _T("auto")}, {_T("margin"), _T("10,0,10,0")}, {_T("valign"), _T("center")}, {_T("font"), _T("system_14")}, {_T("prompttext"), _T("Nickname")}});
    pNickname->SetClass(_T("simple simple_border prompt"));
    pNickname->SetName(_T("nickname"));
    pSearchBox->AddItem(pNickname);

    auto* pSearchIcon = ui::Create<ui::Control>(this, {{_T("width"), _T("auto")}, {_T("height"), _T("auto")}, {_T("margin"), _T("6,9")}});
    pSearchIcon->SetBkImage(_T("search_icon.png"));
    pSearchBox->AddItem(pSearchIcon);

    auto* pFriendContent = ui::Create<ui::VBox>(this, {});
    pFriendList->AddItem(pFriendContent);

    // Chat area
    auto* pChatArea = ui::Create<ui::VBox>(this, {});
    pRoot->AddItem(pChatArea);

    auto* pChatTitle = ui::Create<ui::HBox>(this, {{_T("height"), _T("80")}});
    pChatTitle->SetBkColor(_T("bk_wnd_darkcolor"));
    pChatArea->AddItem(pChatTitle);

    auto* pSplitLine = ui::Create<ui::Control>(this, {});
    pSplitLine->SetClass(_T("splitline_hor_level1"));
    pChatArea->AddItem(pSplitLine);

    auto* pChatContent = ui::Create<ui::VBox>(this, {});
    pChatContent->SetBkColor(_T("bk_wnd_darkcolor"));
    pChatArea->AddItem(pChatContent);

    auto* pChatInput = ui::Create<ui::VBox>(this, {{_T("height"), _T("150")}});
    pChatInput->SetBkColor(_T("bk_wnd_lightcolor"));
    pChatArea->AddItem(pChatInput);

    AttachBox(pRoot);
}

void ChatForm::BuildLoginUI()
{
    // Corresponds to the login.xml layout
    auto* pRoot = ui::Create<ui::Box>(this, {{_T("width"), _T("304")}, {_T("height"), _T("auto")}});

    auto* pHeadBox = ui::Create<ui::Box>(this, {{_T("width"), _T("130")}, {_T("height"), _T("130")}, {_T("margin"), _T("0,10,0,0")}, {_T("halign"), _T("center")}});
    pRoot->AddItem(pHeadBox);

    auto* pHeadShadow = ui::Create<ui::Control>(this, {{_T("width"), _T("auto")}, {_T("height"), _T("auto")}, {_T("halign"), _T("center")}, {_T("valign"), _T("center")}, {_T("mouse_enabled"), _T("false")}});
    pHeadShadow->SetBkImage(_T("head_shadow.png"));
    pHeadBox->AddItem(pHeadShadow);

    // Login panel
    auto* pLoginPanel = ui::Create<ui::Box>(this, {{_T("height"), _T("auto")}, {_T("margin"), _T("0,50,0,0")}, {_T("padding"), _T("14,14,14,14")}});
    pLoginPanel->SetBkImage(_T("file='bk_shadow.png' corner='30,30,30,30'"));
    pRoot->AddItem(pLoginPanel);

    auto* pLoginContent = ui::Create<ui::VBox>(this, {{_T("height"), _T("auto")}, {_T("borderround"), _T("3,3,3,3")}});
    pLoginContent->SetBkColor(_T("bk_wnd_lightcolor"));
    pLoginPanel->AddItem(pLoginContent);

    auto* pLoginVBox = ui::Create<ui::VBox>(this, {{_T("height"), _T("auto")}});
    pLoginContent->AddItem(pLoginVBox);

    // Title bar (minimize/close)
    auto* pCaption = ui::Create<ui::HBox>(this, {{_T("height"), _T("40")}, {_T("margin"), _T("0,6,5,40")}});
    pLoginVBox->AddItem(pCaption);

    auto* pSpacer = ui::Create<ui::Control>(this, {});
    pCaption->AddItem(pSpacer);

    auto* pMinBtn = ui::Create<ui::Button>(this, {{_T("height"), _T("32")}, {_T("width"), _T("32")}, {_T("margin"), _T("4,0,4,0")}});
    pMinBtn->SetClass(_T("btn_wnd_min_11"));
    pMinBtn->SetName(_T("minbtn"));
    pCaption->AddItem(pMinBtn);

    auto* pCloseBtn = ui::Create<ui::Button>(this, {{_T("height"), _T("32")}, {_T("width"), _T("32")}});
    pCloseBtn->SetClass(_T("btn_wnd_close_11"));
    pCloseBtn->SetName(_T("closebtn"));
    pCaption->AddItem(pCloseBtn);

    // Register/Login toggle
    auto* pRegBox = ui::Create<ui::HBox>(this, {{_T("height"), _T("auto")}});
    pLoginVBox->AddItem(pRegBox);

    auto* pSpacer2 = ui::Create<ui::Control>(this, {});
    pRegBox->AddItem(pSpacer2);

    auto* pRegisterBtn = ui::Create<ui::Button>(this, {{_T("margin"), _T("0,5,15,10")}, {_T("halign"), _T("right")}, {_T("cursortype"), _T("hand")}, {_T("font"), _T("system_underline_12")}, {_T("normaltextcolor"), _T("light_green")}});
    pRegisterBtn->SetName(_T("register_account"));
    pRegisterBtn->SetText(_T("Register"));
    pRegBox->AddItem(pRegisterBtn);

    auto* pEnterLoginBtn = ui::Create<ui::Button>(this, {{_T("margin"), _T("0,5,15,10")}, {_T("halign"), _T("right")}, {_T("cursortype"), _T("hand")}, {_T("font"), _T("system_underline_12")}, {_T("normaltextcolor"), _T("light_green")}});
    pEnterLoginBtn->SetName(_T("enter_login"));
    pEnterLoginBtn->SetText(_T("Login Directly"));
    pEnterLoginBtn->SetVisible(false);
    pRegBox->AddItem(pEnterLoginBtn);

    // Account and password
    auto* pEnterPanel = ui::Create<ui::VBox>(this, {{_T("width"), _T("240")}, {_T("height"), _T("auto")}, {_T("margin"), _T("20,0,20,0")}});
    pEnterPanel->SetName(_T("enter_panel"));
    pEnterPanel->SetBkImage(_T("user_password.png"));
    pLoginVBox->AddItem(pEnterPanel);

    auto* pUserRow = ui::Create<ui::HBox>(this, {{_T("height"), _T("41")}, {_T("padding"), _T("14,1,10,0")}});
    pEnterPanel->AddItem(pUserRow);

    auto* pUserIcon = ui::Create<ui::Control>(this, {{_T("width"), _T("16")}, {_T("height"), _T("16")}, {_T("valign"), _T("center")}, {_T("normalimage"), _T("user_1.png")}, {_T("disabledimage"), _T("user_2.png")}});
    pUserIcon->SetName(_T("usericon"));
    pUserRow->AddItem(pUserIcon);

    auto* pUsername = ui::Create<ui::RichEdit>(this, {{_T("width"), _T("stretch")}, {_T("height"), _T("auto")}, {_T("margin"), _T("10,0,10,0")}, {_T("valign"), _T("center")}, {_T("font"), _T("system_14")}, {_T("prompttext"), _T("Account")}});
    pUsername->SetClass(_T("simple prompt"));
    pUsername->SetName(_T("username"));
    pUserRow->AddItem(pUsername);

    // Nickname row (shown during registration)
    auto* pNickRow = ui::Create<ui::HBox>(this, {{_T("height"), _T("40")}, {_T("padding"), _T("14,1,0,0")}});
    pNickRow->SetName(_T("nick_name_panel"));
    pNickRow->SetVisible(false);
    pEnterPanel->AddItem(pNickRow);

    auto* pNickIcon = ui::Create<ui::Control>(this, {{_T("width"), _T("16")}, {_T("height"), _T("16")}, {_T("valign"), _T("center")}, {_T("normalimage"), _T("nickname.png")}, {_T("disabledimage"), _T("nickname.png")}});
    pNickIcon->SetName(_T("nick_name_icon"));
    pNickRow->AddItem(pNickIcon);

    auto* pNickname = ui::Create<ui::RichEdit>(this, {{_T("width"), _T("stretch")}, {_T("height"), _T("35")}, {_T("font"), _T("system_14")}, {_T("text_align"), _T("vcenter")}, {_T("text_padding"), _T("25,8,8,8")}, {_T("borderround"), _T("5,5")}, {_T("prompttext"), _T("Search")}, {_T("width"), _T("stretch")}, {_T("height"), _T("auto")}, {_T("margin"), _T("10,0,10,0")}, {_T("valign"), _T("center")}, {_T("font"), _T("system_14")}, {_T("prompttext"), _T("Nickname")}});
    pNickname->SetClass(_T("simple prompt"));
    pNickname->SetName(_T("nickname"));
    pNickRow->AddItem(pNickname);

    // Password row
    auto* pPwdRow = ui::Create<ui::HBox>(this, {{_T("height"), _T("41")}, {_T("padding"), _T("14,0,10,1")}, {_T("margin"), _T("0,2,0,0")}});
    pEnterPanel->AddItem(pPwdRow);

    auto* pPwdIcon = ui::Create<ui::Control>(this, {{_T("width"), _T("16")}, {_T("height"), _T("16")}, {_T("valign"), _T("center")}, {_T("normalimage"), _T("password_1.png")}, {_T("disabledimage"), _T("password_2.png")}});
    pPwdIcon->SetName(_T("passwordicon"));
    pPwdRow->AddItem(pPwdIcon);

    auto* pPassword = ui::Create<ui::RichEdit>(this, {{_T("width"), _T("stretch")}, {_T("height"), _T("auto")}, {_T("margin"), _T("10,0,10,0")}, {_T("valign"), _T("center")}, {_T("font"), _T("system_14")}, {_T("password"), _T("true")}, {_T("prompttext"), _T("Password")}});
    pPassword->SetClass(_T("simple prompt"));
    pPassword->SetName(_T("password"));
    pPwdRow->AddItem(pPassword);

    // Login hint
    auto* pLoginIngTip = ui::Create<ui::Label>(this, {{_T("margin"), _T("20,20,20,0")}, {_T("font"), _T("system_12")}});
    pLoginIngTip->SetName(_T("login_ing_tip"));
    pLoginIngTip->SetText(_T("Logging in..."));
    pLoginIngTip->SetVisible(false);
    pLoginContent->AddItem(pLoginIngTip);

    auto* pLoginErrorTip = ui::Create<ui::Label>(this, {{_T("margin"), _T("20,20,20,0")}, {_T("font"), _T("system_12")}, {_T("normaltextcolor"), _T("obvious_tip")}});
    pLoginErrorTip->SetName(_T("login_error_tip"));
    pLoginErrorTip->SetVisible(false);
    pLoginContent->AddItem(pLoginErrorTip);

    // Login/Register/Cancel buttons
    auto* pBtnBox = ui::Create<ui::Box>(this, {{_T("width"), _T("240")}, {_T("height"), _T("40")}, {_T("margin"), _T("20,20,20,20")}});
    pLoginContent->AddItem(pBtnBox);

    auto* pLoginBtn = ui::Create<ui::Button>(this, {{_T("width"), _T("240")}, {_T("height"), _T("40")}, {_T("font"), _T("system_bold_16")}});
    pLoginBtn->SetClass(_T("btn_global_blue_80x30"));
    pLoginBtn->SetName(_T("btn_login"));
    pLoginBtn->SetText(_T("Login"));
    pBtnBox->AddItem(pLoginBtn);

    auto* pRegisterMainBtn = ui::Create<ui::Button>(this, {{_T("width"), _T("240")}, {_T("height"), _T("40")}, {_T("font"), _T("system_bold_16")}});
    pRegisterMainBtn->SetClass(_T("btn_global_blue_80x30"));
    pRegisterMainBtn->SetName(_T("btn_register"));
    pRegisterMainBtn->SetText(_T("Register"));
    pRegisterMainBtn->SetVisible(false);
    pBtnBox->AddItem(pRegisterMainBtn);

    auto* pCancelBtn = ui::Create<ui::Button>(this, {{_T("width"), _T("240")}, {_T("height"), _T("40")}, {_T("font"), _T("system_bold_16")}});
    pCancelBtn->SetClass(_T("btn_global_red_80x30"));
    pCancelBtn->SetName(_T("btn_cancel"));
    pCancelBtn->SetText(_T("Cancel Login"));
    pCancelBtn->SetVisible(false);
    pBtnBox->AddItem(pCancelBtn);

    // Avatar
    auto* pAvatarBox = ui::Create<ui::Box>(this, {{_T("margin"), _T("0,10,0,0")}, {_T("mouse_enabled"), _T("false")}, {_T("width"), _T("130")}, {_T("height"), _T("130")}, {_T("halign"), _T("center")}});
    pRoot->AddItem(pAvatarBox);

    auto* pLogo = ui::Create<ui::Control>(this, {{_T("width"), _T("auto")}, {_T("height"), _T("auto")}, {_T("halign"), _T("center")}, {_T("valign"), _T("center")}});
    pLogo->SetBkImage(_T("logo.png"));
    pAvatarBox->AddItem(pLogo);

    auto* pHeadIcon = ui::Create<ui::Button>(this, {{_T("width"), _T("106")}, {_T("height"), _T("106")}, {_T("border_round"), _T("106,106,106,106")}, {_T("halign"), _T("center")}, {_T("valign"), _T("center")}, {_T("mouse_enabled"), _T("false")}, {_T("fade_alpha"), _T("true")}});
    pHeadIcon->SetName(_T("headicon"));
    pAvatarBox->AddItem(pHeadIcon);

    auto* pMask = ui::Create<ui::Control>(this, {{_T("width"), _T("auto")}, {_T("height"), _T("auto")}, {_T("halign"), _T("center")}, {_T("valign"), _T("center")}, {_T("mouse_enabled"), _T("false")}});
    pMask->SetBkImage(_T("mask.png"));
    pAvatarBox->AddItem(pMask);

    AttachBox(pRoot);
}

void ChatForm::OnInitWindow()
{
    // Use the OS-provided system shadow on all platforms.
    SetShadowAttached(true);
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    SetLayeredWindow(false, false);
    SetEnableShadowSnap(true);
    SetShadowBorderSize(0);

    if (m_layoutType == kWechat) {
        SetCaptionRect(ui::UiRect(0, 0, 0, 35), false);
        BuildWechatUI();
    }
    else {
        BuildLoginUI();
    }

    BaseClass::OnInitWindow();
}

void ChatForm::ShowCustomWindow(LayoutType layoutType)
{
    ChatForm* window = new ChatForm(layoutType);
    window->CreateWnd(nullptr, ui::WindowCreateParam(_T("chat (Pure Code)"), true));
    window->PostQuitMsgWhenClosed(true);
    window->ShowWindow(ui::kSW_SHOW_NORMAL);
}
