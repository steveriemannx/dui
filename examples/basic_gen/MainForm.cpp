#include "MainForm.h"
#include "generated_ui.inc"  // Build-time generated pure C++ UI code (from ../basic/basic.xml)
#include "dui/Utils/UiBuilder.h"

void MainForm::OnInitWindow()
{
    BuildUI();
    BindEvents();

    BaseClass::OnInitWindow();
}

void MainForm::BuildUI()
{
    InitBasic(this);
}

void MainForm::BindEvents()
{
    // Window caption buttons (closebtn/minbtn/maxbtn/restorebtn/fullscreenbtn)
    // are wired automatically by the framework via their names.
}
