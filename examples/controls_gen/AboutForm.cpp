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
    // The layout is generated from about.xml at build time.
    return DUI_T("");
}

extern void InitAbout(ui::Window* pWindow);

void AboutForm::OnInitWindow()
{
    ::InitAbout(this);
    ui::WindowImplBase::OnInitWindow();

    ui::Label* link = ui::Find<ui::Label>(this, DUI_T("link"));
    if (link != nullptr) {
        link->AttachButtonUp([link](const ui::EventArgs& args) {
            ui::SystemUtil::OpenUrl(link->GetText());
            return true;
        });
    }
}
