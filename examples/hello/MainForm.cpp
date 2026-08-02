#include "MainForm.h"

MainForm::MainForm()
{
}

MainForm::~MainForm()
{
}

DString MainForm::GetSkinFolder()
{
    return _T("hello");
}

DString MainForm::GetSkinFile()
{
    return _T("hello.xml");
}

void MainForm::OnInitWindow()
{
    BaseClass::OnInitWindow();

    // Bind the click event of the "Click Me" button
    ui::Button* pBtn = dynamic_cast<ui::Button*>(FindControl(_T("hello_btn")));
    if (pBtn != nullptr) {
        pBtn->AttachClick([this](const ui::EventArgs& /*args*/) {
            ui::Label* pLabel = dynamic_cast<ui::Label*>(FindControl(_T("hello_label")));
            if (pLabel != nullptr) {
                pLabel->SetText(_T("Hello from XML mode!"));
            }
            return true;
        });
    }
}
