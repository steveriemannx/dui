#ifndef EXAMPLES_LAYOUTS_FORM_H_
#define EXAMPLES_LAYOUTS_FORM_H_

// dui
#include "dui/dui.h"

class ChatForm : public ui::WindowImplBase
{
public:
    ChatForm(const DString& class_name, const DString& theme_directory, const DString& layout_xml);
    virtual ~ChatForm() override;

    /** Resource-related interfaces
     * The GetSkinFolder interface sets the skin resource path of the window to be drawn
     * The GetSkinFile interface sets the xml description file of the window to be drawn
     */
    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;

    /** Called after the window is created, for subclasses to do some initialization work
    */
    virtual void OnInitWindow() override;

public:
    static void ShowCustomWindow(const DString& class_name, const DString& theme_directory, const DString& layout_xml);

private:
    DString m_themeDirectory;
    DString m_layoutXml;
};

#endif //EXAMPLES_LAYOUTS_FORM_H_
