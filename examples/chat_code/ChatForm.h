#ifndef EXAMPLES_CHAT_CODE_FORM_H_
#define EXAMPLES_CHAT_CODE_FORM_H_

// dui
#include "dui/dui.h"

class ChatForm : public ui::WindowImplBase
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

    explicit ChatForm(LayoutType layoutType);
    virtual ~ChatForm() override;

    /** Resource-related interfaces
     * The GetSkinFolder interface sets the skin resource path of the window to be drawn
     * The GetSkinFile interface sets the xml description file of the window to be drawn
     */
    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;
    virtual void GetCreateWindowAttributes(ui::WindowCreateAttributes& attrs) override;

    /** Called after the window is created, for subclasses to do some initialization work
    */
    virtual void OnInitWindow() override;

public:
    static void ShowCustomWindow(LayoutType layoutType);

private:
    /** Build the UI purely in code (corresponds to the wechat.xml layout)
    */
    void BuildWechatUI();

    /** Build the UI purely in code (corresponds to the login.xml layout)
    */
    void BuildLoginUI();

private:
    LayoutType m_layoutType;
};

#endif //EXAMPLES_CHAT_CODE_FORM_H_
