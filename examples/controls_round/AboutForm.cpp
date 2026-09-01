#include "AboutForm.h"

AboutForm::AboutForm()
{
}

AboutForm::~AboutForm()
{
}

DString AboutForm::GetSkinFolder()
{
    return DUI_T("controls_round");
}

DString AboutForm::GetSkinFile()
{
    return DUI_T("about.xml");
}

void AboutForm::OnInitWindow()
{
    ui::WindowImplBase::OnInitWindow();
    ui::Label* link = ui::Find<ui::Label>(this, DUI_T("link"));
    if (link != nullptr) {
        link->AttachButtonUp([link](const ui::EventArgs& args) {
            ui::SystemUtil::OpenUrl(link->GetText());
            return true;
        });
    }
}
