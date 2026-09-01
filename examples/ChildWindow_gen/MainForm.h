#ifndef EXAMPLES_CHILD_WINDOW_GEN_MAIN_FORM_H_
#define EXAMPLES_CHILD_WINDOW_GEN_MAIN_FORM_H_

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

    virtual DString GetSkinFolder() override { return DUI_T("child_window"); }
    virtual DString GetSkinFile() override { return DUI_T(""); }

public:
    virtual bool PaintChildWindow(ui::ChildWindow* pChildWindow) override;

    virtual bool PaintNextChildWindow(ui::ChildWindow* pChildWindow) override;

public:
    bool PaintNextChildWindow();

protected:
    virtual void PreInitWindow() override;

    virtual void OnInitWindow() override;
    virtual void OnInitLayout() override;

    virtual void OnPreCloseWindow() override;

    virtual void OnLayeredWindowChanged() override;

private:
    void BuildUI();
    void BindEvents();

    void CreateChildWindows();

    void CloseChildWindows();

    bool DoPaintNextChildWindow(ui::ChildWindow* pChildWindow);

private:
    std::vector<MyChildWindowEvents*> m_childWindowEvents;

    ui::ChildWindow* m_pChildWindow;
};

#endif // EXAMPLES_CHILD_WINDOW_GEN_MAIN_FORM_H_
