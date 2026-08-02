#include "MainForm.h"
#include "generated_ui.inc"  // Build-time generated pure C++ UI code (from layout.xml)

MainForm::MainForm()
{
}

MainForm::~MainForm()
{
}

DString MainForm::GetSkinFolder()
{
    return _T("layout");
}

DString MainForm::GetSkinFile()
{
    // No XML file - UI is generated at build time from layout.xml
    return _T("");
}

void MainForm::OnInitWindow()
{
    SetSizeBox(ui::UiRect(4, 4, 4, 4), false);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), false);

    // Build-time generated from layout.xml
    InitLayout(this);
}

void MainForm::OnCloseWindow()
{
    //After the window is closed, exit the main thread message loop and shut down the program
    PostQuitMsg(0L);
}
