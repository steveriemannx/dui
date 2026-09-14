#ifndef EXAMPLES_LISTBOX_FORM_H_
#define EXAMPLES_LISTBOX_FORM_H_

// dui
#include "dui/dui.h"

class ListBoxForm : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
public:
    ListBoxForm() = default;
    virtual ~ListBoxForm() override = default;

    /** Resource-related interfaces
     * The GetSkinFolder interface sets the skin resource path for the window to be drawn
     * The GetSkinFile interface sets the xml description file for the window to be drawn
     */
    virtual std::string GetSkinFolder() override { return "list_box"; }
    virtual std::string GetSkinFile() override { return "list_box.xml"; }

    /** Called after the window is created, for subclasses to do some initialization work
    */
    virtual void OnInitWindow() override;

private:
    void BindEvents();

    /** Test the list events
    */
    void TestListBoxEvents(ui::ListBox* pListBox);

    /** Get the basic information of the message (used to display the log)
    */
    std::string GetEventDisplayInfo(const ui::EventArgs& args, ui::ListBox* pListBox);

    /** Output the test log
    */
    void OutputDebugLog(const std::string& logMsg);

private:
    ui::ListBox* m_pListBox = nullptr;
};

#endif //EXAMPLES_LISTBOX_FORM_H_
