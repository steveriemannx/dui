#include "MainForm.h"
#include "ChildWindowPaint.h"
#include "MyChildWindowEvents.h"

MainForm::MainForm():
    m_pChildWindow(nullptr)
{
}

MainForm::~MainForm()
{
    if (!m_childWindowEvents.empty()) {
        CloseChildWindows();
    }    
}

DString MainForm::GetSkinFolder()
{
    return _T("child_window");
}

DString MainForm::GetSkinFile()
{
    return _T("child_window.xml");
}

void MainForm::OnInitWindow()
{
    BaseClass::OnInitWindow();
    // Create the child window and associate the handling interface
    CreateChildWindows();
}

void MainForm::OnPreCloseWindow()
{
    // Destroy the child window and remove the association
    CloseChildWindows();
    BaseClass::OnPreCloseWindow();
}

void MainForm::OnLayeredWindowChanged()
{
    BaseClass::OnLayeredWindowChanged();
    // Synchronize the layered window attributes to the child window
    for (MyChildWindowEvents* pChildWindowEvents : m_childWindowEvents) {
        if (pChildWindowEvents != nullptr) {
            ui::ChildWindow* pChildWindow = pChildWindowEvents->GetChildWindow();
            if (pChildWindow != nullptr) {
                pChildWindow->SetChildWindowLayered(IsLayeredWindow());
            }
        }
    }
}

void MainForm::CreateChildWindows()
{
    ui::GridBox* pChildWindowBox = dynamic_cast<ui::GridBox*>(FindControl(_T("child_window_box")));
    if (pChildWindowBox != nullptr) {
        size_t nCount = pChildWindowBox->GetItemCount();
        for (size_t nItem = 0; nItem < nCount; ++nItem) {
            ui::ChildWindow* pChildWindow = dynamic_cast<ui::ChildWindow*>(pChildWindowBox->GetItemAt(nItem));
            if (pChildWindow != nullptr) {
                MyChildWindowEvents* pMyChildWindowEvents = new MyChildWindowEvents(pChildWindow, nItem, this);
                pChildWindow->CreateChildWindow(pMyChildWindowEvents);
                m_childWindowEvents.push_back(pMyChildWindowEvents);
            }
        }
    }
}

void MainForm::CloseChildWindows()
{
    std::vector<MyChildWindowEvents*> childWindowEvents;
    childWindowEvents.swap(m_childWindowEvents);
    for (MyChildWindowEvents* pChildWindowEvents : childWindowEvents) {
        if (pChildWindowEvents != nullptr) {
            ui::ChildWindow* pChildWindow = pChildWindowEvents->GetChildWindow();
            if (pChildWindow != nullptr) {
                // Close the child window (synchronously)
                pChildWindow->SetChildWindowEvents(nullptr);
                pChildWindow->CloseChildWindow();
            }
            delete pChildWindowEvents;
            pChildWindowEvents = nullptr;
        }
    }
}

bool MainForm::PaintChildWindow(ui::ChildWindow* pChildWindow)
{
    if (pChildWindow != nullptr) {
        pChildWindow->InvalidateChildWindow();
        return true;
    }
    return false;
}

bool MainForm::PaintNextChildWindow(ui::ChildWindow* pChildWindow)
{
    // Continuous drawing must be triggered in the idle function, otherwise the UI will freeze
    m_pChildWindow = pChildWindow;
    return true;
}

bool MainForm::PaintNextChildWindow()
{
    return DoPaintNextChildWindow(m_pChildWindow);
}

bool MainForm::DoPaintNextChildWindow(ui::ChildWindow * pChildWindow)
{
    if (pChildWindow == nullptr) {
        return false;
    }
    if ((pChildWindow == nullptr) || m_childWindowEvents.empty()) {
        return false;
    }
    size_t nStartItemIndex = 0;
    const size_t nItemCount = m_childWindowEvents.size();
    for (size_t nItemIndex = 0; nItemIndex < nItemCount; ++nItemIndex) {
        MyChildWindowEvents* pChildWindowEvents = m_childWindowEvents[nItemIndex];
        if ((pChildWindowEvents != nullptr) && (pChildWindowEvents->GetChildWindow() == pChildWindow)) {
            nStartItemIndex = nItemIndex;
            break;
        }
    }
    for (size_t nItemIndex = nStartItemIndex + 1; nItemIndex < nItemCount; ++nItemIndex) {
        MyChildWindowEvents* pChildWindowEvents = m_childWindowEvents[nItemIndex];
        if ((pChildWindowEvents != nullptr) &&
            (pChildWindowEvents->GetChildWindow() != nullptr) &&
            pChildWindowEvents->GetChildWindow()->IsVisible() &&
            pChildWindowEvents->IsPaintFps()) {
            // Determine the window to draw
            return PaintChildWindow(pChildWindowEvents->GetChildWindow());
        }
    }
    if (nStartItemIndex >= m_childWindowEvents.size()) {
        nStartItemIndex = m_childWindowEvents.size() - 1;
    }
    for (size_t nItemIndex = 0; nItemIndex <= nStartItemIndex; ++nItemIndex) {
        MyChildWindowEvents* pChildWindowEvents = m_childWindowEvents[nItemIndex];
        if ((pChildWindowEvents != nullptr) &&
            (pChildWindowEvents->GetChildWindow() != nullptr) &&
            pChildWindowEvents->GetChildWindow()->IsVisible() &&
            pChildWindowEvents->IsPaintFps()) {
            // Determine the window to draw
            return PaintChildWindow(pChildWindowEvents->GetChildWindow());
        }
    }
    return PaintChildWindow(pChildWindow);
}
