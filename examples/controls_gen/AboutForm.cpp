#include "AboutForm.h"

AboutForm::AboutForm()
{
}

AboutForm::~AboutForm()
{
}

DString AboutForm::GetSkinFolder()
{
    return "";
}

DString AboutForm::GetSkinFile()
{
    // The layout is generated from about.xml at build time.
    return "";
}

extern void InitAbout(ui::Window* pWindow);

void AboutForm::OnInitWindow()
{
    ::InitAbout(this);
    ui::WindowImplBase::OnInitWindow();

    ui::Label* link = ui::Find<ui::Label>(this, "link");
    if (link != nullptr) {
        link->AttachButtonUp([link](const ui::EventArgs& args) {
            ui::SystemUtil::OpenUrl(link->GetText());
            return true;
        });
    }
}
