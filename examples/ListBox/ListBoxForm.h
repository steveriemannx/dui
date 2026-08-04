#ifndef EXAMPLES_LISTBOX_FORM_H_
#define EXAMPLES_LISTBOX_FORM_H_

// dui
#include "dui/dui.h"

class ListBoxForm : public ui::WindowImplBase
{
public:
    ListBoxForm();
    virtual ~ListBoxForm() override;

    /** Resource-related interfaces
     * The GetSkinFolder interface sets the skin resource path for the window to be drawn
     * The GetSkinFile interface sets the xml description file for the window to be drawn
     */
    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;

    /** Called after the window is created, for subclasses to do some initialization work
    */
    virtual void OnInitWindow() override;

private:
    /** Test the list events
    */
    void TestListBoxEvents(ui::ListBox* pListBox);

    /** Get the basic information of the message (used to display the log)
    */
    DString GetEventDisplayInfo(const ui::EventArgs& args, ui::ListBox* pListBox);

    /** Output the test log
    */
    void OutputDebugLog(const DString& logMsg);

private:
    ui::ListBox* m_pListBox;
};

#endif //EXAMPLES_LISTBOX_FORM_H_
