#ifndef EXAMPLES_CHAT_GEN_MAIN_FORM_H_
#define EXAMPLES_CHAT_GEN_MAIN_FORM_H_

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

    explicit MainForm(LayoutType layoutType = kLogin);
    virtual ~MainForm() override;

    /** Resource-related interfaces
     * The GetSkinFolder interface sets the skin resource path of the window to be drawn
     * The GetSkinFile interface sets the xml description file of the window to be drawn
     */
    virtual std::string GetSkinFolder() override { return "chat"; }
    virtual std::string GetSkinFile() override { return ""; }
    /** Called after the window is created, for subclasses to do some initialization work
     */
    virtual void OnInitWindow() override;

public:
    static void ShowCustomWindow(LayoutType layoutType);

private:
    void BuildUI();
    void BindEvents();

private:
    LayoutType m_layoutType;
};

#endif //EXAMPLES_CHAT_GEN_MAIN_FORM_H_
