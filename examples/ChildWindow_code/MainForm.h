#ifndef EXAMPLES_MAIN_FORM_H_
#define EXAMPLES_MAIN_FORM_H_

// dui
#include "dui/dui.h"
#include "ChildWindowPaintScheduler.h"

class MyChildWindowEvents;

class MainForm : public ui::WindowImplBase, public ChildWindowPaintScheduler
{
    typedef ui::WindowImplBase BaseClass;
public:
    MainForm();
    virtual ~MainForm() override;

    /** Resource-related interfaces
     * GetSkinFolder sets the skin resource path of the window to be drawn
     * GetSkinFile sets the XML description file of the window to be drawn
     */
    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;

public:
    /** Draw this child window immediately
    * @param [in] pChildWindow The child window pointer
    */
    virtual bool PaintChildWindow(ui::ChildWindow* pChildWindow) override;

    /** Draw the next child window after this one immediately (draw each child window in sequence)
    * @param [in] pChildWindow The child window pointer
    */
    virtual bool PaintNextChildWindow(ui::ChildWindow* pChildWindow) override;

public:
    /** Draw the next child window after this one immediately (draw each child window in sequence)
    */
    bool PaintNextChildWindow();

protected:
    /** Called after the window is created, for subclasses to do some initialization work
    */
    virtual void OnInitWindow() override;

private:
    /** Build the UI in pure code (corresponding to the child_window.xml layout)
    */
    void BuildUI();

    /** Called when the window is about to be closed, for subclasses to do some cleanup work
    */
    virtual void OnPreCloseWindow() override;

    /** The layered window attributes of the window have changed
    */
    virtual void OnLayeredWindowChanged() override;

private:
    /** Create a child window and save the association
    */
    void CreateChildWindows();

    /** Close the child window and remove the association
    */
    void CloseChildWindows();

    /** Draw the next child window after this one immediately (draw each child window in sequence)
    * @param [in] pChildWindow The child window pointer
    */
    bool DoPaintNextChildWindow(ui::ChildWindow* pChildWindow);

private:
    /** Events associated with the child window
    */
    std::vector<MyChildWindowEvents*> m_childWindowEvents;

    /** The child window currently being drawn
    */
    ui::ChildWindow* m_pChildWindow;
};

#endif //EXAMPLES_MAIN_FORM_H_
