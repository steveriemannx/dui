//MainForm.h
#ifndef EXAMPLES_MAIN_FORM_H_
#define EXAMPLES_MAIN_FORM_H_

// duilib
#include "duilib/duilib.h"
#include "duilib/duilib_webview2.h"

/** Main window implementation of the application
*/
class MainForm : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
public:
    MainForm();
    virtual ~MainForm() override;

    /**  Called when the window is created; implemented by subclasses to get the window skin folder
    * @return subclasses need to implement and return the window skin folder
    */
    virtual DString GetSkinFolder() override;

    /**  Called when the window is created; implemented by subclasses to get the window skin XML description file
    * @return subclasses need to implement and return the window skin XML description file
    *         The returned content can be the XML file content (a string starting with the character '<'),
    *         or a file path (a string not starting with the character '<'); the file must be findable in the GetSkinFolder() path
    */
    virtual DString GetSkinFile() override;

protected:
    /** Called after the window is created, so that subclasses can do some initialization work
    */
    virtual void OnInitWindow() override;

    /** Key pressed (WM_KEYDOWN or WM_SYSKEYDOWN)
    * @param [in] vkCode virtual key code
    * @param [in] modifierKey modifier key flags, valid values: ModifierKey::kFirstPress, ModifierKey::kAlt
    * @param [in] nativeMsg the raw message content received from the system
    * @param [out] bHandled whether the message has been handled; returning true means the message was processed successfully and need not be passed to the window procedure; returning false means the message will continue to be passed to the window procedure for processing
    * @return the message processing result; if the application handles this message, it should return zero
    */
    virtual LRESULT OnKeyDownMsg(ui::VirtualKeyCode vkCode, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled) override;

private:
    bool OnClicked(const ui::EventArgs& msg);
    bool OnNavigate(const ui::EventArgs& msg);

private:
    ui::WebView2Control* m_pWebView2Control;
    ui::RichEdit* m_pEditUrl;
};

#endif //EXAMPLES_MAIN_FORM_H_
