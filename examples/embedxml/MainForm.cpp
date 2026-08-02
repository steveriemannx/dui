#include "MainForm.h"
#include "embedded_resources.h"

MainForm::MainForm()
{
}

MainForm::~MainForm()
{
}

DString MainForm::GetSkinFolder()
{
    // Resource path for images/fonts (still loaded from disk)
    return _T("basic");
}

DString MainForm::GetSkinFile()
{
    // Compiled-in XML - no file read at runtime
    // The string starts with '<' so the framework treats it as XML content
    return ui_resources::k_basic;
}

void MainForm::OnInitWindow()
{
    BaseClass::OnInitWindow();
}
