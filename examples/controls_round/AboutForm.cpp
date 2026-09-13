#include "AboutForm.h"

AboutForm::AboutForm()
{
}

AboutForm::~AboutForm()
{
}

std::string AboutForm::GetSkinFolder()
{
    return "controls_round";
}

std::string AboutForm::GetSkinFile()
{
    return "about.xml";
}

void AboutForm::OnInitWindow()
{
    ui::WindowImplBase::OnInitWindow();
    ui::Label* link = ui::Find<ui::Label>(this, "link");
    if (link != nullptr) {
        link->AttachButtonUp([link](const ui::EventArgs& args) {
            ui::SystemUtil::OpenUrl(link->GetText());
            return true;
        });
    }
}
