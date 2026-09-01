#ifndef EXAMPLES_LISTBOX_CODE_FORM_H_
#define EXAMPLES_LISTBOX_CODE_FORM_H_

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
    virtual DString GetSkinFolder() override { return DUI_T("list_box"); }
    virtual DString GetSkinFile() override { return DUI_T(""); }

    /** Called after the window is created, for subclasses to do some initialization work
    */
    virtual void OnInitWindow() override;

private:
    void SetupWindow();
    void BuildUI();

    void BindEvents();

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
    ui::ListBox* m_pListBox = nullptr;
};

#endif //EXAMPLES_LISTBOX_CODE_FORM_H_
