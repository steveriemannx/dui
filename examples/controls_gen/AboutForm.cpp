#include "AboutForm.h"

AboutForm::AboutForm()
{
}

AboutForm::~AboutForm()
{
}

DString AboutForm::GetSkinFolder()
{
    return _T("");
}

DString AboutForm::GetSkinFile()
{
    // Pure code mode: no layout XML is loaded
    return _T("");
}

void AboutForm::BuildUI()
{
    // Corresponds to the about.xml layout
    ui::VBox* pRoot = new ui::VBox(this);
    pRoot->SetBkColor(_T("bk_wnd_darkcolor"));
    pRoot->SetAttribute(_T("width"), _T("100%"));
    pRoot->SetAttribute(_T("height"), _T("100%"));

    ui::HBox* pCaption = new ui::HBox(this);
    pCaption->SetAttribute(_T("name"), _T("window_caption_bar"));
    pCaption->SetAttribute(_T("width"), _T("stretch"));
    pCaption->SetAttribute(_T("height"), _T("35"));
    pCaption->SetBkColor(_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pCaption);

    ui::Control* pLogo = new ui::Control(this);
    pLogo->SetAttribute(_T("width"), _T("18"));
    pLogo->SetAttribute(_T("height"), _T("18"));
    pLogo->SetBkImage(_T("public/caption/logo.svg"));
    pLogo->SetAttribute(_T("valign"), _T("center"));
    pLogo->SetAttribute(_T("margin"), _T("8"));
    pCaption->AddItem(pLogo);

    ui::Label* pTitle = new ui::Label(this);
    pTitle->SetText(_T("Controls"));
    pTitle->SetAttribute(_T("valign"), _T("center"));
    pTitle->SetAttribute(_T("margin"), _T("8"));
    pTitle->SetAttribute(_T("mouse_enabled"), _T("false"));
    pCaption->AddItem(pTitle);

    ui::Control* pSpacer = new ui::Control(this);
    pSpacer->SetAttribute(_T("mouse_enabled"), _T("false"));
    pCaption->AddItem(pSpacer);

    ui::Button* pCloseBtn = new ui::Button(this);
    pCloseBtn->SetClass(_T("btn_wnd_close_11"));
    pCloseBtn->SetName(_T("closebtn"));
    pCloseBtn->SetAttribute(_T("width"), _T("40"));
    pCloseBtn->SetAttribute(_T("height"), _T("32"));
    pCloseBtn->SetAttribute(_T("margin"), _T("4,0,0,0"));
    pCaption->AddItem(pCloseBtn);

    ui::Box* pContent = new ui::Box(this);
    pRoot->AddItem(pContent);

    ui::VBox* pCenter = new ui::VBox(this);
    pCenter->SetAttribute(_T("margin"), _T("0,0,0,0"));
    pCenter->SetAttribute(_T("valign"), _T("center"));
    pCenter->SetAttribute(_T("halign"), _T("center"));
    pCenter->SetAttribute(_T("width"), _T("100%"));
    pCenter->SetAttribute(_T("height"), _T("auto"));
    pContent->AddItem(pCenter);

    ui::Label* pTooltip = new ui::Label(this);
    pTooltip->SetName(_T("tooltip"));
    pTooltip->SetText(_T("nim_duilib controls example."));
    pTooltip->SetAttribute(_T("width"), _T("stretch"));
    pTooltip->SetAttribute(_T("text_align"), _T("center"));
    pCenter->AddItem(pTooltip);

    ui::Label* pLink = new ui::Label(this);
    pLink->SetName(_T("link"));
    pLink->SetAttribute(_T("width"), _T("stretch"));
    pLink->SetText(_T("https://github.com/rhett-lee/nim_duilib"));
    pLink->SetAttribute(_T("normal_text_color"), _T("blue"));
    pLink->SetAttribute(_T("text_align"), _T("center"));
    pLink->SetAttribute(_T("cursor_type"), _T("hand"));
    pLink->SetAttribute(_T("margin"), _T("0,8"));
    pCenter->AddItem(pLink);

    AttachBox(pRoot);
}

void AboutForm::OnInitWindow()
{
    BuildUI();

    ui::Label* link = static_cast<ui::Label*>(FindControl(_T("link")));
    if (link != nullptr) {
        link->AttachButtonUp([link](const ui::EventArgs& args) {
            ui::SystemUtil::OpenUrl(link->GetText());
            return true;
        });
    }
}

