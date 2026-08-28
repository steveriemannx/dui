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
    // Keep the same resource path as the XML/gen versions so layout images
    // and resources are resolved identically.
    return _T("layout");
}

DString MainForm::GetSkinFile()
{
    // Pure code mode: no layout XML is loaded
    return _T("");
}

void MainForm::OnInitWindow()
{
    // Use the OS-provided system shadow on all platforms.
    SetShadowAttached(true);
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    SetLayeredWindow(false, false);
    SetEnableShadowSnap(true);
    SetShadowBorderSize(0);


    SetSizeBox(ui::UiRect(4, 4, 4, 4), false);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), false);

    //Pure-code-built UI (corresponding to the layout.xml layout)
    BuildUIFromXml(this);
    BaseClass::OnInitWindow();
}

void MainForm::OnCloseWindow()
{
    //After the window is closed, exit the main thread message loop and shut down the program
    PostQuitMsg(0L);
}
