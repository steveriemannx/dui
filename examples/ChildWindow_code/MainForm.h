#ifndef EXAMPLES_CHILD_WINDOW_CODE_MAIN_FORM_H_
#define EXAMPLES_CHILD_WINDOW_CODE_MAIN_FORM_H_

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

    virtual std::string GetSkinFolder() override { return "child_window"; }
    virtual std::string GetSkinFile() override { return ""; }

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
    /** Called before the Render is created; select the GPU backend.
    */
    virtual void PreInitWindow() override;

    /** Called after the window is created, for subclasses to do some initialization work
    */
    virtual void OnInitWindow() override;
    virtual void OnInitLayout() override;

private:
    void SetupWindow();
    void BuildUI();
    void BindEvents();

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

#endif // EXAMPLES_CHILD_WINDOW_CODE_MAIN_FORM_H_
