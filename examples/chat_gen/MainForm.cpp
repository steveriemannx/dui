#include "MainForm.h"
#include "dui/Utils/UiBuilder.h"
#include "generated_ui.inc"  // Build-time generated pure C++ UI code (from wechat.xml/login.xml)

MainForm::MainForm(LayoutType layoutType):
    m_layoutType(layoutType)
{
}

MainForm::~MainForm()
{
}

void MainForm::BuildUI()
{
    if (m_layoutType == kLogin) {
        InitLogin(this);
    }
    else {
        InitWechat(this);
    }
}

void MainForm::OnInitWindow()
{
    BuildUI();

    if (m_layoutType == kLogin) {
        // Let the generated root determine its natural height. A fixed height
        // leaves an empty transparent area below the login panel on X11.
        if (ui::Box* root = GetRoot()) {
            const ui::UiEstSize size = root->EstimateSize(ui::UiSize(999999, 999999));
            SetWindowSize(size.cx.GetInt32(), size.cy.GetInt32());
        }
        CenterWindow();
        // Also show the WeChat window behind the login window.
        ShowCustomWindow(kWechat);
    }

    BindEvents();
    BaseClass::OnInitWindow();
}

void MainForm::BindEvents()
{
}

void MainForm::ShowCustomWindow(LayoutType layoutType)
{
    MainForm* window = new MainForm(layoutType);
    ui::WindowCreateParam createParam("chat (Generated Code)", true);
    if (layoutType == kWechat) {
        //Match wechat.xml: size="1024,768".
        createParam.m_nWidth = 1024;
        createParam.m_nHeight = 768;
    }
    //login.xml has no explicit size; the root auto-resizes to its content.
    window->CreateWnd(nullptr, createParam);
    window->PostQuitMsgWhenClosed(true);
    window->ShowWindow(ui::kSW_SHOW_NA);
}
