#include "MainForm.h"
#include "dui/Utils/UiBuilder.h"

void MainForm::OnInitWindow()
{
    BindEvents();
    BaseClass::OnInitWindow();
}

void MainForm::BindEvents()
{
    if (auto* pButton = ui::Find<ui::Button>(this, DUI_T("hello_btn"))) {
        pButton->AttachClick([this](const ui::EventArgs&) {
            if (auto* pLabel = ui::Find<ui::Label>(this, DUI_T("hello_label"))) {
                pLabel->SetText(DUI_T("Hello from hello2 (self-contained)!"));
            }
            return true;
        });
    }
}
