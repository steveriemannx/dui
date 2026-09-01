#ifndef EXAMPLES_CHAT_CODE_MAIN_FORM_H_
#define EXAMPLES_CHAT_CODE_MAIN_FORM_H_

// dui
#include "dui/dui.h"

class MainForm : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
public:
    /** Layout type (the original example's dynamic m_layoutXml switching is replaced by an enum)
     */
    enum LayoutType
    {
        kWechat = 0,    //Mimics the WeChat layout (originally wechat.xml)
        kLogin,         //Mimics the login window layout (originally login.xml)
    };

    MainForm() : m_layoutType(kLogin) {}
    explicit MainForm(LayoutType layoutType) : m_layoutType(layoutType) {}
    virtual ~MainForm() override = default;

    /** Resource-related interfaces
     * GetSkinFolder sets the skin resource path of the window to be drawn
     * GetSkinFile sets the xml description file of the window to be drawn
     */
    virtual DString GetSkinFolder() override { return DUI_T("chat"); }
    virtual DString GetSkinFile() override { return DUI_T(""); }

    /** Called after the window is created, for subclasses to do some initialization work
     */
    virtual void OnInitWindow() override;

public:
    static void ShowCustomWindow(LayoutType layoutType);

private:
    /** Build the UI purely in code (corresponds to the wechat.xml layout)
     */
    void SetupWindow();
    void BuildUI();
    void BuildWechatUI();

    /** Build the UI purely in code (corresponds to the login.xml layout)
     */
    void BuildLoginUI();

    void BindEvents();

private:
    LayoutType m_layoutType;
};

#endif //EXAMPLES_CHAT_CODE_MAIN_FORM_H_
