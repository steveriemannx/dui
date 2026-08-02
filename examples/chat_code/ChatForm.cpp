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
    return _T("");
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
        attrs.m_rcCaption = ui::UiRect(0, 0, 0, 35);
        attrs.m_bCaptionDefined = true;
    }
    else {
        //Corresponds to the <Window> attributes of login.xml
        attrs.m_bInitSizeDefined = true;
        attrs.m_szInitSize.cx = 304;
        attrs.m_szInitSize.cy = 520;
        attrs.m_bShadowAttached = false;
        attrs.m_bIsLayeredWindow = true;
        attrs.m_bIsLayeredWindowDefined = true;
        attrs.m_rcCaption = ui::UiRect(0, 0, 0, 160);
        attrs.m_bCaptionDefined = true;
    }
    BaseClass::GetCreateWindowAttributes(attrs);
}

void ChatForm::BuildWechatUI()
{
    // Corresponds to the wechat.xml layout
    ui::HBox* pRoot = new ui::HBox(this);

    // Left menu bar
    ui::VBox* pLeftMenu = new ui::VBox(this);
    pLeftMenu->SetAttribute(_T("width"), _T("60"));
    pLeftMenu->SetBkColor(_T("darkcolor"));
    pRoot->AddItem(pLeftMenu);

    ui::Control* pHead = new ui::Control(this);
    pHead->SetBkImage(_T("head.png"));
    pHead->SetAttribute(_T("width"), _T("40"));
    pHead->SetAttribute(_T("height"), _T("40"));
    pHead->SetAttribute(_T("margin"), _T("10,10"));
    pLeftMenu->AddItem(pHead);

    // Friend list
    ui::VBox* pFriendList = new ui::VBox(this);
    pFriendList->SetAttribute(_T("width"), _T("270"));
    pFriendList->SetBkColor(_T("light_gray"));
    pRoot->AddItem(pFriendList);

    ui::Box* pSearchBox = new ui::Box(this);
    pSearchBox->SetAttribute(_T("padding"), _T("10,8,10,8"));
    pSearchBox->SetAttribute(_T("height"), _T("auto"));
    pFriendList->AddItem(pSearchBox);

    ui::RichEdit* pNickname = new ui::RichEdit(this);
    pNickname->SetClass(_T("simple simple_border prompt"));
    pNickname->SetName(_T("nickname"));
    pNickname->SetAttribute(_T("width"), _T("stretch"));
    pNickname->SetAttribute(_T("height"), _T("35"));
    pNickname->SetAttribute(_T("font"), _T("system_14"));
    pNickname->SetAttribute(_T("text_align"), _T("vcenter"));
    pNickname->SetAttribute(_T("text_padding"), _T("25,8,8,8"));
    pNickname->SetAttribute(_T("borderround"), _T("5,5"));
    pNickname->SetAttribute(_T("prompttext"), _T("Search"));
    pSearchBox->AddItem(pNickname);

    ui::Control* pSearchIcon = new ui::Control(this);
    pSearchIcon->SetAttribute(_T("width"), _T("auto"));
    pSearchIcon->SetAttribute(_T("height"), _T("auto"));
    pSearchIcon->SetAttribute(_T("margin"), _T("6,9"));
    pSearchIcon->SetBkImage(_T("search_icon.png"));
    pSearchBox->AddItem(pSearchIcon);

    ui::VBox* pFriendContent = new ui::VBox(this);
    pFriendList->AddItem(pFriendContent);

    // Chat area
    ui::VBox* pChatArea = new ui::VBox(this);
    pRoot->AddItem(pChatArea);

    ui::HBox* pChatTitle = new ui::HBox(this);
    pChatTitle->SetAttribute(_T("height"), _T("80"));
    pChatTitle->SetBkColor(_T("bk_wnd_darkcolor"));
    pChatArea->AddItem(pChatTitle);

    ui::Control* pSplitLine = new ui::Control(this);
    pSplitLine->SetClass(_T("splitline_hor_level1"));
    pChatArea->AddItem(pSplitLine);

    ui::VBox* pChatContent = new ui::VBox(this);
    pChatContent->SetBkColor(_T("bk_wnd_darkcolor"));
    pChatArea->AddItem(pChatContent);

    ui::VBox* pChatInput = new ui::VBox(this);
    pChatInput->SetAttribute(_T("height"), _T("150"));
    pChatInput->SetBkColor(_T("bk_wnd_lightcolor"));
    pChatArea->AddItem(pChatInput);

    AttachBox(pRoot);
}

void ChatForm::BuildLoginUI()
{
    // Corresponds to the login.xml layout
    ui::Box* pRoot = new ui::Box(this);
    pRoot->SetAttribute(_T("width"), _T("304"));
    pRoot->SetAttribute(_T("height"), _T("auto"));

    ui::Box* pHeadBox = new ui::Box(this);
    pHeadBox->SetAttribute(_T("width"), _T("130"));
    pHeadBox->SetAttribute(_T("height"), _T("130"));
    pHeadBox->SetAttribute(_T("margin"), _T("0,10,0,0"));
    pHeadBox->SetAttribute(_T("halign"), _T("center"));
    pRoot->AddItem(pHeadBox);

    ui::Control* pHeadShadow = new ui::Control(this);
    pHeadShadow->SetAttribute(_T("width"), _T("auto"));
    pHeadShadow->SetAttribute(_T("height"), _T("auto"));
    pHeadShadow->SetAttribute(_T("halign"), _T("center"));
    pHeadShadow->SetAttribute(_T("valign"), _T("center"));
    pHeadShadow->SetBkImage(_T("head_shadow.png"));
    pHeadShadow->SetAttribute(_T("mouse_enabled"), _T("false"));
    pHeadBox->AddItem(pHeadShadow);

    // Login panel
    ui::Box* pLoginPanel = new ui::Box(this);
    pLoginPanel->SetAttribute(_T("height"), _T("auto"));
    pLoginPanel->SetAttribute(_T("margin"), _T("0,50,0,0"));
    pLoginPanel->SetAttribute(_T("padding"), _T("14,14,14,14"));
    pLoginPanel->SetBkImage(_T("file='bk_shadow.png' corner='30,30,30,30'"));
    pRoot->AddItem(pLoginPanel);

    ui::VBox* pLoginContent = new ui::VBox(this);
    pLoginContent->SetAttribute(_T("height"), _T("auto"));
    pLoginContent->SetBkColor(_T("bk_wnd_lightcolor"));
    pLoginContent->SetAttribute(_T("borderround"), _T("3,3,3,3"));
    pLoginPanel->AddItem(pLoginContent);

    ui::VBox* pLoginVBox = new ui::VBox(this);
    pLoginVBox->SetAttribute(_T("height"), _T("auto"));
    pLoginContent->AddItem(pLoginVBox);

    // Title bar (minimize/close)
    ui::HBox* pCaption = new ui::HBox(this);
    pCaption->SetAttribute(_T("height"), _T("40"));
    pCaption->SetAttribute(_T("margin"), _T("0,6,5,40"));
    pLoginVBox->AddItem(pCaption);

    ui::Control* pSpacer = new ui::Control(this);
    pCaption->AddItem(pSpacer);

    ui::Button* pMinBtn = new ui::Button(this);
    pMinBtn->SetClass(_T("btn_wnd_min_11"));
    pMinBtn->SetName(_T("minbtn"));
    pMinBtn->SetAttribute(_T("height"), _T("32"));
    pMinBtn->SetAttribute(_T("width"), _T("32"));
    pMinBtn->SetAttribute(_T("margin"), _T("4,0,4,0"));
    pCaption->AddItem(pMinBtn);

    ui::Button* pCloseBtn = new ui::Button(this);
    pCloseBtn->SetClass(_T("btn_wnd_close_11"));
    pCloseBtn->SetAttribute(_T("height"), _T("32"));
    pCloseBtn->SetAttribute(_T("width"), _T("32"));
    pCloseBtn->SetName(_T("closebtn"));
    pCaption->AddItem(pCloseBtn);

    // Register/Login toggle
    ui::HBox* pRegBox = new ui::HBox(this);
    pRegBox->SetAttribute(_T("height"), _T("auto"));
    pLoginVBox->AddItem(pRegBox);

    ui::Control* pSpacer2 = new ui::Control(this);
    pRegBox->AddItem(pSpacer2);

    ui::Button* pRegisterBtn = new ui::Button(this);
    pRegisterBtn->SetName(_T("register_account"));
    pRegisterBtn->SetAttribute(_T("margin"), _T("0,5,15,10"));
    pRegisterBtn->SetAttribute(_T("halign"), _T("right"));
    pRegisterBtn->SetText(_T("Register"));
    pRegisterBtn->SetAttribute(_T("cursortype"), _T("hand"));
    pRegisterBtn->SetAttribute(_T("font"), _T("system_underline_12"));
    pRegisterBtn->SetAttribute(_T("normaltextcolor"), _T("light_green"));
    pRegBox->AddItem(pRegisterBtn);

    ui::Button* pEnterLoginBtn = new ui::Button(this);
    pEnterLoginBtn->SetName(_T("enter_login"));
    pEnterLoginBtn->SetAttribute(_T("margin"), _T("0,5,15,10"));
    pEnterLoginBtn->SetAttribute(_T("halign"), _T("right"));
    pEnterLoginBtn->SetText(_T("Login Directly"));
    pEnterLoginBtn->SetAttribute(_T("cursortype"), _T("hand"));
    pEnterLoginBtn->SetAttribute(_T("font"), _T("system_underline_12"));
    pEnterLoginBtn->SetAttribute(_T("normaltextcolor"), _T("light_green"));
    pEnterLoginBtn->SetVisible(false);
    pRegBox->AddItem(pEnterLoginBtn);

    // Account and password
    ui::VBox* pEnterPanel = new ui::VBox(this);
    pEnterPanel->SetName(_T("enter_panel"));
    pEnterPanel->SetAttribute(_T("width"), _T("240"));
    pEnterPanel->SetAttribute(_T("height"), _T("auto"));
    pEnterPanel->SetAttribute(_T("margin"), _T("20,0,20,0"));
    pEnterPanel->SetBkImage(_T("user_password.png"));
    pLoginVBox->AddItem(pEnterPanel);

    ui::HBox* pUserRow = new ui::HBox(this);
    pUserRow->SetAttribute(_T("height"), _T("41"));
    pUserRow->SetAttribute(_T("padding"), _T("14,1,10,0"));
    pEnterPanel->AddItem(pUserRow);

    ui::Control* pUserIcon = new ui::Control(this);
    pUserIcon->SetName(_T("usericon"));
    pUserIcon->SetAttribute(_T("width"), _T("16"));
    pUserIcon->SetAttribute(_T("height"), _T("16"));
    pUserIcon->SetAttribute(_T("valign"), _T("center"));
    pUserIcon->SetAttribute(_T("normalimage"), _T("user_1.png"));
    pUserIcon->SetAttribute(_T("disabledimage"), _T("user_2.png"));
    pUserRow->AddItem(pUserIcon);

    ui::RichEdit* pUsername = new ui::RichEdit(this);
    pUsername->SetClass(_T("simple prompt"));
    pUsername->SetName(_T("username"));
    pUsername->SetAttribute(_T("width"), _T("stretch"));
    pUsername->SetAttribute(_T("height"), _T("auto"));
    pUsername->SetAttribute(_T("margin"), _T("10,0,10,0"));
    pUsername->SetAttribute(_T("valign"), _T("center"));
    pUsername->SetAttribute(_T("font"), _T("system_14"));
    pUsername->SetAttribute(_T("prompttext"), _T("Account"));
    pUserRow->AddItem(pUsername);

    // Nickname row (shown during registration)
    ui::HBox* pNickRow = new ui::HBox(this);
    pNickRow->SetName(_T("nick_name_panel"));
    pNickRow->SetAttribute(_T("height"), _T("40"));
    pNickRow->SetAttribute(_T("padding"), _T("14,1,0,0"));
    pNickRow->SetVisible(false);
    pEnterPanel->AddItem(pNickRow);

    ui::Control* pNickIcon = new ui::Control(this);
    pNickIcon->SetName(_T("nick_name_icon"));
    pNickIcon->SetAttribute(_T("width"), _T("16"));
    pNickIcon->SetAttribute(_T("height"), _T("16"));
    pNickIcon->SetAttribute(_T("valign"), _T("center"));
    pNickIcon->SetAttribute(_T("normalimage"), _T("nickname.png"));
    pNickIcon->SetAttribute(_T("disabledimage"), _T("nickname.png"));
    pNickRow->AddItem(pNickIcon);

    ui::RichEdit* pNickname = new ui::RichEdit(this);
    pNickname->SetClass(_T("simple prompt"));
    pNickname->SetName(_T("nickname"));
    pNickname->SetAttribute(_T("width"), _T("stretch"));
    pNickname->SetAttribute(_T("height"), _T("auto"));
    pNickname->SetAttribute(_T("margin"), _T("10,0,10,0"));
    pNickname->SetAttribute(_T("valign"), _T("center"));
    pNickname->SetAttribute(_T("font"), _T("system_14"));
    pNickname->SetAttribute(_T("prompttext"), _T("Nickname"));
    pNickRow->AddItem(pNickname);

    // Password row
    ui::HBox* pPwdRow = new ui::HBox(this);
    pPwdRow->SetAttribute(_T("height"), _T("41"));
    pPwdRow->SetAttribute(_T("padding"), _T("14,0,10,1"));
    pPwdRow->SetAttribute(_T("margin"), _T("0,2,0,0"));
    pEnterPanel->AddItem(pPwdRow);

    ui::Control* pPwdIcon = new ui::Control(this);
    pPwdIcon->SetName(_T("passwordicon"));
    pPwdIcon->SetAttribute(_T("width"), _T("16"));
    pPwdIcon->SetAttribute(_T("height"), _T("16"));
    pPwdIcon->SetAttribute(_T("valign"), _T("center"));
    pPwdIcon->SetAttribute(_T("normalimage"), _T("password_1.png"));
    pPwdIcon->SetAttribute(_T("disabledimage"), _T("password_2.png"));
    pPwdRow->AddItem(pPwdIcon);

    ui::RichEdit* pPassword = new ui::RichEdit(this);
    pPassword->SetClass(_T("simple prompt"));
    pPassword->SetName(_T("password"));
    pPassword->SetAttribute(_T("width"), _T("stretch"));
    pPassword->SetAttribute(_T("height"), _T("auto"));
    pPassword->SetAttribute(_T("margin"), _T("10,0,10,0"));
    pPassword->SetAttribute(_T("valign"), _T("center"));
    pPassword->SetAttribute(_T("font"), _T("system_14"));
    pPassword->SetAttribute(_T("password"), _T("true"));
    pPassword->SetAttribute(_T("prompttext"), _T("Password"));
    pPwdRow->AddItem(pPassword);

    // Login hint
    ui::Label* pLoginIngTip = new ui::Label(this);
    pLoginIngTip->SetName(_T("login_ing_tip"));
    pLoginIngTip->SetAttribute(_T("margin"), _T("20,20,20,0"));
    pLoginIngTip->SetAttribute(_T("font"), _T("system_12"));
    pLoginIngTip->SetText(_T("Logging in..."));
    pLoginIngTip->SetVisible(false);
    pLoginContent->AddItem(pLoginIngTip);

    ui::Label* pLoginErrorTip = new ui::Label(this);
    pLoginErrorTip->SetName(_T("login_error_tip"));
    pLoginErrorTip->SetAttribute(_T("margin"), _T("20,20,20,0"));
    pLoginErrorTip->SetAttribute(_T("font"), _T("system_12"));
    pLoginErrorTip->SetAttribute(_T("normaltextcolor"), _T("obvious_tip"));
    pLoginErrorTip->SetVisible(false);
    pLoginContent->AddItem(pLoginErrorTip);

    // Login/Register/Cancel buttons
    ui::Box* pBtnBox = new ui::Box(this);
    pBtnBox->SetAttribute(_T("width"), _T("240"));
    pBtnBox->SetAttribute(_T("height"), _T("40"));
    pBtnBox->SetAttribute(_T("margin"), _T("20,20,20,20"));
    pLoginContent->AddItem(pBtnBox);

    ui::Button* pLoginBtn = new ui::Button(this);
    pLoginBtn->SetClass(_T("btn_global_blue_80x30"));
    pLoginBtn->SetName(_T("btn_login"));
    pLoginBtn->SetAttribute(_T("width"), _T("240"));
    pLoginBtn->SetAttribute(_T("height"), _T("40"));
    pLoginBtn->SetAttribute(_T("font"), _T("system_bold_16"));
    pLoginBtn->SetText(_T("Login"));
    pBtnBox->AddItem(pLoginBtn);

    ui::Button* pRegisterMainBtn = new ui::Button(this);
    pRegisterMainBtn->SetClass(_T("btn_global_blue_80x30"));
    pRegisterMainBtn->SetName(_T("btn_register"));
    pRegisterMainBtn->SetAttribute(_T("width"), _T("240"));
    pRegisterMainBtn->SetAttribute(_T("height"), _T("40"));
    pRegisterMainBtn->SetAttribute(_T("font"), _T("system_bold_16"));
    pRegisterMainBtn->SetText(_T("Register"));
    pRegisterMainBtn->SetVisible(false);
    pBtnBox->AddItem(pRegisterMainBtn);

    ui::Button* pCancelBtn = new ui::Button(this);
    pCancelBtn->SetClass(_T("btn_global_red_80x30"));
    pCancelBtn->SetName(_T("btn_cancel"));
    pCancelBtn->SetAttribute(_T("width"), _T("240"));
    pCancelBtn->SetAttribute(_T("height"), _T("40"));
    pCancelBtn->SetAttribute(_T("font"), _T("system_bold_16"));
    pCancelBtn->SetText(_T("Cancel Login"));
    pCancelBtn->SetVisible(false);
    pBtnBox->AddItem(pCancelBtn);

    // Avatar
    ui::Box* pAvatarBox = new ui::Box(this);
    pAvatarBox->SetAttribute(_T("margin"), _T("0,10,0,0"));
    pAvatarBox->SetAttribute(_T("mouse_enabled"), _T("false"));
    pAvatarBox->SetAttribute(_T("width"), _T("130"));
    pAvatarBox->SetAttribute(_T("height"), _T("130"));
    pAvatarBox->SetAttribute(_T("halign"), _T("center"));
    pRoot->AddItem(pAvatarBox);

    ui::Control* pLogo = new ui::Control(this);
    pLogo->SetAttribute(_T("width"), _T("auto"));
    pLogo->SetAttribute(_T("height"), _T("auto"));
    pLogo->SetAttribute(_T("halign"), _T("center"));
    pLogo->SetAttribute(_T("valign"), _T("center"));
    pLogo->SetBkImage(_T("logo.png"));
    pAvatarBox->AddItem(pLogo);

    ui::Button* pHeadIcon = new ui::Button(this);
    pHeadIcon->SetName(_T("headicon"));
    pHeadIcon->SetAttribute(_T("width"), _T("106"));
    pHeadIcon->SetAttribute(_T("height"), _T("106"));
    pHeadIcon->SetAttribute(_T("border_round"), _T("106,106,106,106"));
    pHeadIcon->SetAttribute(_T("halign"), _T("center"));
    pHeadIcon->SetAttribute(_T("valign"), _T("center"));
    pHeadIcon->SetAttribute(_T("mouse_enabled"), _T("false"));
    pHeadIcon->SetAttribute(_T("fade_alpha"), _T("true"));
    pAvatarBox->AddItem(pHeadIcon);

    ui::Control* pMask = new ui::Control(this);
    pMask->SetAttribute(_T("width"), _T("auto"));
    pMask->SetAttribute(_T("height"), _T("auto"));
    pMask->SetBkImage(_T("mask.png"));
    pMask->SetAttribute(_T("halign"), _T("center"));
    pMask->SetAttribute(_T("valign"), _T("center"));
    pMask->SetAttribute(_T("mouse_enabled"), _T("false"));
    pAvatarBox->AddItem(pMask);

    AttachBox(pRoot);
}

void ChatForm::OnInitWindow()
{
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
