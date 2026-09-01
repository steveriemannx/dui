#include "MainForm.h"
#include "generated_ui.inc"  // Build-time generated pure C++ UI code (from ../hello/hello.xml)
#include "dui/Utils/UiBuilder.h"

void MainForm::OnInitWindow()
{
    BuildUI();
    BindEvents();

    BaseClass::OnInitWindow();
}

void MainForm::BuildUI()
{
    InitHello(this);
}

void MainForm::BindEvents()
{
    if (auto* pButton = ui::Find<ui::Button>(this, DUI_T("hello_btn"))) {
        pButton->AttachClick([this](const ui::EventArgs&) {
            if (auto* pLabel = ui::Find<ui::Label>(this, DUI_T("hello_label"))) {
                pLabel->SetText(DUI_T("Hello from codegen mode!"));
            }
            return true;
        });
    }
}
