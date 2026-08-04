#ifndef EXAMPLES_MAIN_FORM_H_
#define EXAMPLES_MAIN_FORM_H_

// dui
#include "dui/dui.h"

class MainForm : public ui::WindowImplBase
{
public:
    MainForm();
    virtual ~MainForm() override;

    /** Resource-related interface
     * The GetSkinFolder interface sets the skin resource path of the window you are drawing
     * The GetSkinFile interface sets the xml description file of the window you are drawing
     */
    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;

    /** Called after the window is created, allowing subclasses to do some initialization work
    */
    virtual void OnInitWindow() override;

private:
    /** Pure code UI construction (corresponds to the list_ctrl.xml layout)
    */
    void BuildUI();

    /** Layout initialization is complete; the position, size, and other layout info of each control is initialized, for subclasses to adjust the layout after the UI starts
    */
    virtual void OnInitLayout() override;

private:
    /** Fill data
    */
    void InsertItemData(int32_t nRows, int32_t nColumns, int32_t nImageId);

    /** Initialize the UI events related to this program's test features
    */
    void InitListCtrlEvents(ui::ListCtrl* pListCtrl);

    /** Run some functional tests
    */
    void RunListCtrlTest();

    /** Control this column
    */
    void OnColumnChanged(size_t nColumnId);

    /** Test the loading feature of ListCtrl
    */
    void TestListCtrlLoading(ui::ListCtrl* pListCtrl);

    /** Test the Loading feature
    */
    void OnTestLoadingProgress();

    /** Test the event callback interface of ListCtrl
    */
    void TestListCtrlEvents(ui::ListCtrl* pListCtrl);

    /** Get the basic message information (for displaying logs)
    */
    DString GetEventDisplayInfo(const ui::EventArgs& args);

    /** Get the basic message information (for displaying logs)
    */
    DString GetItemFilledEventDisplayInfo(const ui::EventArgs& args);

    /** Output test logs
    */
    void OutputDebugLog(const DString& logMsg);

private:
    // Test feature: show the child control on mouse enter, hide it on mouse leave
    void OnReportViewSubItemFilled(const ui::EventArgs& args);
    void OnReportViewSubItemMouseEnter(const ui::EventArgs& args);
    void OnReportViewSubItemMouseLeave(const ui::EventArgs& args);

private:
    /** Loading progress of the loading state (simulated value; real applications can compute the progress as appropriate)
    */
    float m_fLoadingPercent;
};

#endif //EXAMPLES_MAIN_FORM_H_
