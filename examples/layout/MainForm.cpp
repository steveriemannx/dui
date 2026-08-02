#include "MainForm.h"

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
    return _T("layout.xml");
}

void MainForm::OnInitWindow()
{
}

void MainForm::OnCloseWindow()
{
    //After the window is closed, exit the main thread message loop and shut down the program
    PostQuitMsg(0L);
}
