#include "MainForm.h"
#include "dui/Utils/UiBuilder.h"

namespace
{
/** Show the WeChat window behind the login window (does not take focus).
 */
void OpenWindow(const DString& title, const DString& layoutXml)
{
    MainForm* window = new MainForm(layoutXml, false);
    window->CreateWnd(nullptr, ui::WindowCreateParam(title, true));
    window->PostQuitMsgWhenClosed(true);
    window->ShowWindow(ui::kSW_SHOW_NA);
}
}

void MainForm::OnInitWindow()
{
    BindEvents();

    // The login window is the entry window; it spawns the WeChat window
    // behind it. The WeChat window does not spawn anything (avoids recursion).
    if (m_isLogin) {
        OpenWindow("wechat", "wechat.xml");
    }

    BaseClass::OnInitWindow();
}

void MainForm::BindEvents()
{
}
