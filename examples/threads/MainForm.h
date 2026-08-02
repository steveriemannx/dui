#ifndef EXAMPLES_MAIN_FORM_H_
#define EXAMPLES_MAIN_FORM_H_

// duilib
#include "duilib/duilib.h"
#include <chrono>

class MainThread;
class MainForm : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
public:
    explicit MainForm(MainThread* pMainThread);
    virtual ~MainForm() override;

    /** Resource-related interfaces
     * GetSkinFolder sets the skin resource path of the window to be drawn
     * GetSkinFile sets the XML description file of the window to be drawn
     */
    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;

    /** Called after the window is created, for subclasses to do some initialization work
    */
    virtual void OnInitWindow() override;

public:
    /** Update the UI state (can be called from the worker thread)
    */
    void UpdateUI();

    /** Output a log (can be called from the worker thread)
    */
    void PrintLog(const DString& log);

private:
    /** Execute a task in the worker thread
    * @param [in] nThreadIdentifier Thread identifier
    */
    bool RunTaskInThread(int32_t nThreadIdentifier);

    /** Execute a specific task in the worker thread
    */
    void ExecuteTaskInThread();

    /** Update the running time
    */
    void UpdateRunningTime();

private:
    /** Log display control
    */
    ui::RichEdit* m_pLogEdit;

    /** Running time display
    */
    ui::Label* m_pRunningTimeLabel;

    /** Start time
    */
    std::chrono::steady_clock::time_point m_startTime;

    /** Log sequence number
    */
    int32_t m_nLogLineNumber;

    /** Thread management interface
    */
    MainThread* m_pMainThread;
};

#endif //EXAMPLES_MAIN_FORM_H_
