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
    auto* pRoot = ui::Create<ui::VBox>(this, {{_T("width"), _T("100%")}, {_T("height"), _T("100%")}});
    pRoot->SetBkColor(_T("bk_wnd_darkcolor"));

    auto* pCaption = ui::Create<ui::HBox>(this, {{_T("name"), _T("window_caption_bar")}, {_T("width"), _T("stretch")}, {_T("height"), _T("35")}});
    pCaption->SetBkColor(_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pCaption);

    auto* pLogo = ui::Create<ui::Control>(this, {{_T("width"), _T("18")}, {_T("height"), _T("18")}, {_T("valign"), _T("center")}, {_T("margin"), _T("8")}});
    pLogo->SetBkImage(_T("public/caption/logo.svg"));
    pCaption->AddItem(pLogo);

    auto* pTitle = ui::Create<ui::Label>(this, {{_T("valign"), _T("center")}, {_T("margin"), _T("8")}, {_T("mouse_enabled"), _T("false")}});
    pTitle->SetText(_T("Controls"));
    pCaption->AddItem(pTitle);

    auto* pSpacer = ui::Create<ui::Control>(this, {{_T("mouse_enabled"), _T("false")}});
    pCaption->AddItem(pSpacer);

    auto* pCloseBtn = ui::Create<ui::Button>(this, {{_T("width"), _T("40")}, {_T("height"), _T("32")}, {_T("margin"), _T("4,0,0,0")}});
    pCloseBtn->SetClass(_T("btn_wnd_close_11"));
    pCloseBtn->SetName(_T("closebtn"));
    pCaption->AddItem(pCloseBtn);

    auto* pContent = ui::Create<ui::Box>(this, {});
    pRoot->AddItem(pContent);

    auto* pCenter = ui::Create<ui::VBox>(this, {{_T("margin"), _T("0,0,0,0")}, {_T("valign"), _T("center")}, {_T("halign"), _T("center")}, {_T("width"), _T("100%")}, {_T("height"), _T("auto")}});
    pContent->AddItem(pCenter);

    auto* pTooltip = ui::Create<ui::Label>(this, {{_T("width"), _T("stretch")}, {_T("text_align"), _T("center")}});
    pTooltip->SetName(_T("tooltip"));
    pTooltip->SetText(_T("dui controls example."));
    pCenter->AddItem(pTooltip);

    auto* pLink = ui::Create<ui::Label>(this, {{_T("width"), _T("stretch")}, {_T("normal_text_color"), _T("blue")}, {_T("text_align"), _T("center")}, {_T("cursor_type"), _T("hand")}, {_T("margin"), _T("0,8")}});
    pLink->SetName(_T("link"));
    pLink->SetText(_T("https://github.com/steveriemannx/dui"));
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

