#ifndef EXAMPLES_MAIN_FORM_H_
#define EXAMPLES_MAIN_FORM_H_

// dui
#include "dui/dui.h"

#include "DataProvider.h"

class MainForm : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
public:
    MainForm();
    virtual ~MainForm() override;

    /** Resource-related interfaces
     * The GetSkinFolder interface sets the skin resource path for the window to be drawn
     * The GetSkinFile interface sets the xml description file for the window to be drawn
     */
    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;
    virtual void GetCreateWindowAttributes(ui::WindowCreateAttributes& attrs) override;

    /** Called after the window is created, for subclasses to do some initialization work
    */
    virtual void OnInitWindow() override;

private:
    /** Pure-code-built UI (corresponding to the main.xml layout)
    */
    void BuildUI();

    bool OnClicked(const ui::EventArgs& args);

    /** Test the virtual list events
    */
    void TestVirtualListBoxEvents(ui::VirtualListBox* pListBox);

    /** Get the basic information of the message (used to display the log)
    */
    DString GetEventDisplayInfo(const ui::EventArgs& args, ui::VirtualListBox* pListBox);

    /** Output the test log
    */
    void OutputDebugLog(const DString& logMsg);

private:
    ui::VirtualListBox* m_pTileList;
    DataProvider* m_pDataProvider;

private:
    ui::RichEdit* m_pEditColumn;
    ui::RichEdit* m_pEditTotal;
    ui::Option* m_pOptionColumnFix;
    ui::RichEdit* m_pEditUpdate;
    ui::RichEdit* m_pEditTaskName;
    ui::RichEdit* m_pEditDelete;
    ui::RichEdit* m_pEditChildMarginX;
    ui::RichEdit* m_pEditChildMarginY;
};

#endif //EXAMPLES_MAIN_FORM_H_
