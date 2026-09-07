#ifndef EXAMPLES_CHAT_MAIN_FORM_H_
#define EXAMPLES_CHAT_MAIN_FORM_H_

// dui
#include "dui/dui.h"

class MainForm : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
public:
    // The default (entry) window is the login window; it spawns the WeChat
    // window behind it so the login interface stays in front.
    MainForm() : m_layoutXml(DUI_T("login.xml")), m_isLogin(true) {}
    explicit MainForm(const DString& layoutXml, bool isLogin = false)
        : m_layoutXml(layoutXml), m_isLogin(isLogin) {}
    virtual ~MainForm() override = default;

    /** Resource-related interfaces
     * GetSkinFolder sets the skin resource path of the window to be drawn
     * GetSkinFile sets the XML description file of the window to be drawn
     */
    virtual DString GetSkinFolder() override { return DUI_T("chat"); }
    virtual DString GetSkinFile() override { return m_layoutXml; }

    /** Called after the window is created, for subclasses to do some initialization work
     */
    virtual void OnInitWindow() override;
    virtual void OnInitLayout() override;

private:
    void BindEvents();

    DString m_layoutXml;
    bool m_isLogin = false;
};

#endif //EXAMPLES_CHAT_MAIN_FORM_H_
