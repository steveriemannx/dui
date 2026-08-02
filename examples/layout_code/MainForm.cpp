#include "MainForm.h"
#include "LayoutBuildUI.inc"  // Pure-code-built UI (corresponding to the layout.xml layout)

MainForm::MainForm()
{
}

MainForm::~MainForm()
{
}

DString MainForm::GetSkinFolder()
{
    return _T("");
}

DString MainForm::GetSkinFile()
{
    // Pure code mode: no layout XML is loaded
    return _T("");
}

void MainForm::OnInitWindow()
{
    SetSizeBox(ui::UiRect(4, 4, 4, 4), false);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), false);

    //Pure-code-built UI (corresponding to the layout.xml layout)
    BuildUIFromXml(this);
}

void MainForm::OnCloseWindow()
{
    //After the window is closed, exit the main thread message loop and shut down the program
    PostQuitMsg(0L);
}
