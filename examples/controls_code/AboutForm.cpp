#include "AboutForm.h"

AboutForm::AboutForm()
{
}

AboutForm::~AboutForm()
{
}

DString AboutForm::GetSkinFolder()
{
    return DUI_T("");
}

DString AboutForm::GetSkinFile()
{
    // Pure code mode: no layout XML is loaded
    return DUI_T("");
}

void AboutForm::BuildUI()
{
    // Corresponds to the about.xml layout
    auto* pRoot = ui::Create<ui::VBox>(this, {{DUI_T("width"), DUI_T("100%")}, {DUI_T("height"), DUI_T("100%")}});
    pRoot->SetBkColor(DUI_T("bk_wnd_darkcolor"));

    auto* pCaption = ui::Create<ui::HBox>(this, {{DUI_T("name"), DUI_T("window_caption_bar")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("35")}});
    pCaption->SetBkColor(DUI_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pCaption);

    auto* pLogo = ui::Create<ui::Control>(this, {{DUI_T("width"), DUI_T("18")}, {DUI_T("height"), DUI_T("18")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8")}});
    pLogo->SetBkImage(DUI_T("public/caption/logo.svg"));
    pCaption->AddItem(pLogo);

    auto* pTitle = ui::Create<ui::Label>(this, {{DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8")}, {DUI_T("mouse_enabled"), DUI_T("false")}});
    pTitle->SetText(DUI_T("Controls"));
    pCaption->AddItem(pTitle);

    auto* pSpacer = ui::Create<ui::Control>(this, {{DUI_T("mouse_enabled"), DUI_T("false")}});
    pCaption->AddItem(pSpacer);

    auto* pContent = ui::Create<ui::Box>(this, {});
    pRoot->AddItem(pContent);

    auto* pCenter = ui::Create<ui::VBox>(this, {{DUI_T("margin"), DUI_T("0,0,0,0")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("width"), DUI_T("100%")}, {DUI_T("height"), DUI_T("auto")}});
    pContent->AddItem(pCenter);

    auto* pTooltip = ui::Create<ui::Label>(this, {{DUI_T("width"), DUI_T("stretch")}, {DUI_T("text_align"), DUI_T("center")}});
    pTooltip->SetName(DUI_T("tooltip"));
    pTooltip->SetText(DUI_T("dui controls example."));
    pCenter->AddItem(pTooltip);

    auto* pLink = ui::Create<ui::Label>(this, {{DUI_T("width"), DUI_T("stretch")}, {DUI_T("normal_text_color"), DUI_T("blue")}, {DUI_T("text_align"), DUI_T("center")}, {DUI_T("cursor_type"), DUI_T("hand")}, {DUI_T("margin"), DUI_T("0,8")}});
    pLink->SetName(DUI_T("link"));
    pLink->SetText(DUI_T("https://github.com/steveriemannx/dui"));
    pCenter->AddItem(pLink);

    AttachBox(pRoot);
}

void AboutForm::OnInitWindow()
{
    BuildUI();
    ui::WindowImplBase::OnInitWindow();

    ui::Label* link = ui::Find<ui::Label>(this, DUI_T("link"));
    if (link != nullptr) {
        link->AttachButtonUp([link](const ui::EventArgs& args) {
            ui::SystemUtil::OpenUrl(link->GetText());
            return true;
        });
    }
}
