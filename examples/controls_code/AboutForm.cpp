#include "AboutForm.h"

AboutForm::AboutForm()
{
}

AboutForm::~AboutForm()
{
}

std::string AboutForm::GetSkinFolder()
{
    return "";
}

std::string AboutForm::GetSkinFile()
{
    // Pure code mode: no layout XML is loaded
    return "";
}

void AboutForm::BuildUI()
{
    // Corresponds to the about.xml layout
    auto* pRoot = ui::Create<ui::VBox>(this, {{"width", "100%"}, {"height", "100%"}});
    pRoot->SetBkColor("bk_wnd_darkcolor");

    auto* pCaption = ui::Create<ui::HBox>(this, {{"name", "window_caption_bar"}, {"width", "stretch"}, {"height", "35"}});
    pCaption->SetBkColor("bk_wnd_lightcolor");
    pRoot->AddItem(pCaption);

    auto* pLogo = ui::Create<ui::Control>(this, {{"width", "18"}, {"height", "18"}, {"valign", "center"}, {"margin", "8"}});
    pLogo->SetBkImage("public/caption/logo.svg");
    pCaption->AddItem(pLogo);

    auto* pTitle = ui::Create<ui::Label>(this, {{"valign", "center"}, {"margin", "8"}, {"mouse_enabled", "false"}});
    pTitle->SetText("Controls");
    pCaption->AddItem(pTitle);

    auto* pSpacer = ui::Create<ui::Control>(this, {{"mouse_enabled", "false"}});
    pCaption->AddItem(pSpacer);

    auto* pContent = ui::Create<ui::Box>(this, {});
    pRoot->AddItem(pContent);

    auto* pCenter = ui::Create<ui::VBox>(this, {{"margin", "0,0,0,0"}, {"valign", "center"}, {"halign", "center"}, {"width", "100%"}, {"height", "auto"}});
    pContent->AddItem(pCenter);

    auto* pTooltip = ui::Create<ui::Label>(this, {{"width", "stretch"}, {"text_align", "center"}});
    pTooltip->SetName("tooltip");
    pTooltip->SetText("dui controls example.");
    pCenter->AddItem(pTooltip);

    auto* pLink = ui::Create<ui::Label>(this, {{"width", "stretch"}, {"normal_text_color", "blue"}, {"text_align", "center"}, {"cursor_type", "hand"}, {"margin", "0,8"}});
    pLink->SetName("link");
    pLink->SetText("https://github.com/steveriemannx/dui");
    pCenter->AddItem(pLink);

    AttachBox(pRoot);
}

void AboutForm::OnInitWindow()
{
    BuildUI();
    ui::WindowImplBase::OnInitWindow();

    ui::Label* link = ui::Find<ui::Label>(this, "link");
    if (link != nullptr) {
        link->AttachButtonUp([link](const ui::EventArgs& args) {
            ui::SystemUtil::OpenUrl(link->GetText());
            return true;
        });
    }
}
