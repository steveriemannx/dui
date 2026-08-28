#include "CodeForm.h"

CodeForm::CodeForm()
{
}

CodeForm::~CodeForm()
{
}

DString CodeForm::GetSkinFolder()
{
    return _T("");
}

DString CodeForm::GetSkinFile()
{
    return _T("");
}

void CodeForm::GetCreateWindowAttributes(ui::WindowCreateAttributes& createAttributes)
{
    createAttributes.m_bInitSizeDefined = true;
    createAttributes.m_szInitSize.cx = 520;
    createAttributes.m_szInitSize.cy = 380;
    createAttributes.m_bShadowAttached = false;
    createAttributes.m_bShadowAttachedDefined = true;
    createAttributes.m_bIsLayeredWindow = false;
    createAttributes.m_bIsLayeredWindowDefined = true;
    createAttributes.m_rcSizeBox = ui::UiRect(4, 4, 4, 4);
    createAttributes.m_bSizeBoxDefined = true;
    createAttributes.m_rcCaption = ui::UiRect(0, 0, 0, 36);
    createAttributes.m_bCaptionDefined = true;

    BaseClass::GetCreateWindowAttributes(createAttributes);
}

void CodeForm::OnInitWindow()
{
    SetSizeBox(ui::UiRect(4, 4, 4, 4), false);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), false);

    // Define inline button classes using pre-registered font IDs (from ThemeInit.h)
    AddClass(_T("codeui_close"), _T(" text=\"X\" font=\"system_bold_12\" text_align=\"hcenter,vcenter\"")
        _T(" textcolor=\"#FFFFFFFF\" hottextcolor=\"#FFFFFFFF\" pushedtextcolor=\"#FFFFFFFF\"")
        _T(" bkcolor=\"#FFC42B1C\" hotbkcolor=\"#FFE03A2A\" pushedbkcolor=\"#FF991B10\"")
        _T(" bordersize=\"1\" bordercolor=\"#FF991B10\" width=\"36\" height=\"28\""));

    AddClass(_T("codeui_min"), _T(" text=\"_\" font=\"system_bold_12\" text_align=\"hcenter,vcenter\"")
        _T(" textcolor=\"#FFFFFFFF\" hottextcolor=\"#FFFFFFFF\" pushedtextcolor=\"#FFFFFFFF\"")
        _T(" bkcolor=\"#FF3A3A3A\" hotbkcolor=\"#FF4A4A4A\" pushedbkcolor=\"#FF555555\"")
        _T(" bordersize=\"1\" bordercolor=\"#FF555555\" width=\"36\" height=\"28\""));

    AddClass(_T("codeui_max"), _T(" text=\"[]\" font=\"system_bold_12\" text_align=\"hcenter,vcenter\"")
        _T(" textcolor=\"#FFFFFFFF\" hottextcolor=\"#FFFFFFFF\" pushedtextcolor=\"#FFFFFFFF\"")
        _T(" bkcolor=\"#FF3A3A3A\" hotbkcolor=\"#FF4A4A4A\" pushedbkcolor=\"#FF555555\"")
        _T(" bordersize=\"1\" bordercolor=\"#FF555555\" width=\"36\" height=\"28\""));

    AddClass(_T("codeui_restore"), _T(" text=\"[]\" font=\"system_bold_12\" text_align=\"hcenter,vcenter\"")
        _T(" textcolor=\"#FFFFFFFF\" hottextcolor=\"#FFFFFFFF\" pushedtextcolor=\"#FFFFFFFF\"")
        _T(" bkcolor=\"#FF3A3A3A\" hotbkcolor=\"#FF4A4A4A\" pushedbkcolor=\"#FF555555\"")
        _T(" bordersize=\"1\" bordercolor=\"#FF555555\" width=\"36\" height=\"28\""));

    // Root VBox
    ui::VBox* pRoot = new ui::VBox(this);
    pRoot->SetBkColor(_T("bk_wnd_darkcolor"));

    // Caption bar
    ui::HBox* pCaption = new ui::HBox(this);
    pCaption->SetAttribute(_T("name"), _T("window_caption_bar"));
    pCaption->SetAttribute(_T("height"), _T("36"));
    pCaption->SetBkColor(_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pCaption);

    // Title
    ui::Label* pTitle = new ui::Label(this);
    pTitle->SetAttribute(_T("text"), _T("  Code UI Demo"));
    pTitle->SetAttribute(_T("font"), _T("system_12"));
    pTitle->SetAttribute(_T("text_align"), _T("vcenter"));
    pTitle->SetAttribute(_T("textcolor"), _T("darkcolor"));
    pTitle->SetAttribute(_T("mouse_enabled"), _T("false"));
    pCaption->AddItem(pTitle);

    // Spacer
    ui::Control* pSpacer = new ui::Control(this);
    pSpacer->SetAttribute(_T("mouse_enabled"), _T("false"));
    pCaption->AddItem(pSpacer);

    // Minimize button
    ui::Button* pMinBtn = new ui::Button(this);
    pMinBtn->SetAttribute(_T("name"), _T("minbtn"));
    pMinBtn->SetAttribute(_T("class"), _T("codeui_min"));
    pMinBtn->SetAttribute(_T("margin"), _T("2,3,2,3"));
    pMinBtn->SetAttribute(_T("tooltip_text"), _T("minimize"));
    pMinBtn->AttachClick(ui::UiBind(&CodeForm::OnButtonClick, this, std::placeholders::_1));
    pCaption->AddItem(pMinBtn);

    // Maximize button
    ui::Button* pMaxBtn = new ui::Button(this);
    pMaxBtn->SetAttribute(_T("name"), _T("maxbtn"));
    pMaxBtn->SetAttribute(_T("class"), _T("codeui_max"));
    pMaxBtn->SetAttribute(_T("margin"), _T("2,3,2,3"));
    pMaxBtn->SetAttribute(_T("tooltip_text"), _T("maximize"));
    pMaxBtn->AttachClick(ui::UiBind(&CodeForm::OnButtonClick, this, std::placeholders::_1));
    pCaption->AddItem(pMaxBtn);

    // Restore button
    ui::Button* pRestoreBtn = new ui::Button(this);
    pRestoreBtn->SetAttribute(_T("name"), _T("restorebtn"));
    pRestoreBtn->SetAttribute(_T("class"), _T("codeui_restore"));
    pRestoreBtn->SetAttribute(_T("margin"), _T("2,3,2,3"));
    pRestoreBtn->SetAttribute(_T("visible"), _T("false"));
    pRestoreBtn->SetAttribute(_T("tooltip_text"), _T("restore"));
    pRestoreBtn->AttachClick(ui::UiBind(&CodeForm::OnButtonClick, this, std::placeholders::_1));
    pCaption->AddItem(pRestoreBtn);

    // Close button
    ui::Button* pCloseBtn = new ui::Button(this);
    pCloseBtn->SetAttribute(_T("name"), _T("closebtn"));
    pCloseBtn->SetAttribute(_T("class"), _T("codeui_close"));
    pCloseBtn->SetAttribute(_T("margin"), _T("2,3,2,3"));
    pCloseBtn->SetAttribute(_T("tooltip_text"), _T("close"));
    pCloseBtn->AttachClick(ui::UiBind(&CodeForm::OnButtonClick, this, std::placeholders::_1));
    pCaption->AddItem(pCloseBtn);

    // Content area
    ui::VBox* pContent = new ui::VBox(this);
    pContent->SetBkColor(_T("bk_wnd_darkcolor"));
    pContent->SetAttribute(_T("valign"), _T("center"));
    pContent->SetAttribute(_T("halign"), _T("center"));
    pRoot->AddItem(pContent);

    ui::Label* pMain = new ui::Label(this);
    pMain->SetAttribute(_T("text"), _T("Standalone Pure C++ UI"));
    pMain->SetAttribute(_T("font"), _T("system_bold_16"));
    pMain->SetAttribute(_T("text_align"), _T("hcenter,vcenter"));
    pMain->SetAttribute(_T("text_padding"), _T("0,0,0,10"));
    pMain->SetAttribute(_T("textcolor"), _T("darkcolor"));
    pContent->AddItem(pMain);

    ui::Label* pSub = new ui::Label(this);
    pSub->SetAttribute(_T("text"), _T("Zero external dependencies.\nAll theme data and layout defined in C++.\nCopy to any machine and run directly."));
    pSub->SetAttribute(_T("font"), _T("system_12"));
    pSub->SetAttribute(_T("text_align"), _T("hcenter,vcenter"));
    pSub->SetAttribute(_T("width"), _T("stretch"));
    pSub->SetAttribute(_T("textcolor"), _T("gray"));
    pContent->AddItem(pSub);

    AttachBox(pRoot);
    BaseClass::OnInitWindow();
}

void CodeForm::OnCloseWindow()
{
    BaseClass::OnCloseWindow();
}

bool CodeForm::OnButtonClick(const ui::EventArgs& msg)
{
    ui::Control* pSender = msg.GetSender();
    if (pSender == nullptr) return false;
    DString sName = pSender->GetName();
    if (sName == DUI_CTR_BUTTON_CLOSE) {
        CloseWnd();
    } else if (sName == DUI_CTR_BUTTON_MIN) {
        ShowWindow(ui::kSW_MINIMIZE);
    } else if (sName == DUI_CTR_BUTTON_MAX) {
        ShowWindow(ui::kSW_SHOW_MAXIMIZED);
    } else if (sName == DUI_CTR_BUTTON_RESTORE) {
        ShowWindow(ui::kSW_RESTORE);
    }
    return true;
}
