#include "MainForm.h"
#include "generated_ui.inc"  // Build-time generated pure C++ UI code (from layout.xml)
#include "dui/Utils/UiBuilder.h"

void MainForm::OnInitWindow()
{
    BuildUI();
    BindEvents();

    BaseClass::OnInitWindow();
}

void MainForm::BuildUI()
{
    InitLayout(this);
}

void MainForm::BindEvents()
{
}
