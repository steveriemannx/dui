#include "ChatForm.h"
#include "generated_ui.inc"  // Build-time generated pure C++ UI code (from wechat.xml/login.xml)

ChatForm::ChatForm(LayoutType layoutType):
    m_layoutType(layoutType)
{
}

ChatForm::~ChatForm()
{
}

DString ChatForm::GetSkinFolder()
{
    return _T("chat");
}

DString ChatForm::GetSkinFile()
{
    // No XML file - UI is generated at build time from wechat.xml/login.xml
    return _T("");
}

void ChatForm::GetCreateWindowAttributes(ui::WindowCreateAttributes& attrs)
{
    if (m_layoutType == kWechat) {
        //Corresponds to the <Window> attributes of wechat.xml
        attrs.m_bInitSizeDefined = true;
        attrs.m_szInitSize.cx = 1024;
        attrs.m_szInitSize.cy = 768;
        attrs.m_bShadowAttached = true;
        attrs.m_bShadowAttachedDefined = true;
        attrs.m_bIsLayeredWindow = true;
        attrs.m_bIsLayeredWindowDefined = true;
        attrs.m_rcCaption = ui::UiRect(0, 0, 0, 35);
        attrs.m_bCaptionDefined = true;
    }
    else {
        //Corresponds to the <Window> attributes of login.xml
        attrs.m_bInitSizeDefined = true;
        attrs.m_szInitSize.cx = 304;
        attrs.m_szInitSize.cy = 520;
        attrs.m_bShadowAttached = false;
        attrs.m_bIsLayeredWindow = true;
        attrs.m_bIsLayeredWindowDefined = true;
        attrs.m_rcCaption = ui::UiRect(0, 0, 0, 160);
        attrs.m_bCaptionDefined = true;
    }
    BaseClass::GetCreateWindowAttributes(attrs);
}

void ChatForm::OnInitWindow()
{
    if (m_layoutType == kWechat) {
        SetCaptionRect(ui::UiRect(0, 0, 0, 35), false);
        // Build-time generated from wechat.xml
        InitWechat(this);
    }
    else {
        // Build-time generated from login.xml
        InitLogin(this);
    }

    BaseClass::OnInitWindow();
}

void ChatForm::ShowCustomWindow(LayoutType layoutType)
{
    ChatForm* window = new ChatForm(layoutType);
    window->CreateWnd(nullptr, ui::WindowCreateParam(_T("chat (Generated Code)"), true));
    window->PostQuitMsgWhenClosed(true);
    window->ShowWindow(ui::kSW_SHOW_NORMAL);
}
