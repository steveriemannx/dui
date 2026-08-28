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
        attrs.m_rcCaption = ui::UiRect(0, 0, 0, 36);
        attrs.m_bCaptionDefined = true;
    }
    else {
        //Corresponds to the <Window> attributes of login.xml (no explicit size:
        //the generated root box is width=304/height=auto, so AutoResizeWindow
        //fits the window to its content)
        attrs.m_bShadowAttached = true;
        attrs.m_bShadowAttachedDefined = true;
        attrs.m_bIsLayeredWindow = true;
        attrs.m_bIsLayeredWindowDefined = true;
        attrs.m_rcCaption = ui::UiRect(0, 0, 0, 36);
        attrs.m_bCaptionDefined = true;
    }

    BaseClass::GetCreateWindowAttributes(attrs);
}

void ChatForm::PreInitWindow()
{
    BaseClass::PreInitWindow();

    //No layout XML is loaded, so Window::ParseWindowXml failed and reset the
    //window resource path; restore it now so image paths resolve from the
    //"chat" folder.
    SetResourcePath(ui::FilePath(_T("chat")));
}
void ChatForm::OnInitWindow()
{
    // Use the OS-provided system shadow on all platforms.
    SetShadowAttached(true);
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    SetLayeredWindow(false, false);
    SetEnableShadowSnap(true);
    SetShadowBorderSize(0);

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
    ui::WindowCreateParam createParam(_T("chat (Generated Code)"), true);
    if (layoutType == kWechat) {
        //Match wechat.xml: size="1024,768".
        createParam.m_nWidth = 1024;
        createParam.m_nHeight = 768;
    }
    //login.xml has no explicit size; the root auto-resizes to its content.
    window->CreateWnd(nullptr, createParam);
    window->PostQuitMsgWhenClosed(true);
    window->ShowWindow(ui::kSW_SHOW_NORMAL);
}
