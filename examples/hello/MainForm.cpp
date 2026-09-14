#include "MainForm.h"
#include "dui/Utils/UiBuilder.h"

void MainForm::OnInitWindow()
{
    BaseClass::OnInitWindow();
    BindEvents();
}

void MainForm::BindEvents()
{
    if (auto* pButton = ui::Find<ui::Button>(this, "hello_btn")) {
        pButton->AttachClick([this](const ui::EventArgs&) {
            if (auto* pLabel = ui::Find<ui::Label>(this, "hello_label")) {
                pLabel->SetText("Hello from XML mode!");
            }
            return true;
        });
    }
}
