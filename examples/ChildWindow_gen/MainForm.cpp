#include "MainForm.h"
#include "generated_ui.inc"  // Build-time generated pure C++ UI code
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
    // No XML file - UI is generated at build time
    return _T("");
}

void MainForm::GetCreateWindowAttributes(ui::WindowCreateAttributes& attrs)
{
    // Match the <Window> attributes in child_window.xml:
    // size="75%,85%", size_box/caption, shadow/layered settings.
    ui::UiRect rcWork;
    ui::WindowBase::GetPrimaryMonitorWorkRect(rcWork);
    attrs.m_bInitSizeDefined = true;
    attrs.m_szInitSize.cx = (int32_t)(rcWork.Width() * 0.85f);
    attrs.m_szInitSize.cy = (int32_t)(rcWork.Height() * 0.90f);

    attrs.m_rcSizeBox = ui::UiRect(4, 4, 4, 4);
    attrs.m_bSizeBoxDefined = true;
    attrs.m_rcCaption = ui::UiRect(0, 0, 0, 36);
    attrs.m_bCaptionDefined = true;

    attrs.m_bShadowAttached = true;
    attrs.m_bShadowAttachedDefined = true;
    attrs.m_bIsLayeredWindow = false;
    attrs.m_bIsLayeredWindowDefined = true;

    // Shadow nine-patch parameters, corresponding to shadow_type="default" in
    // the XML layout (WindowBuilder adds the shadow corner to the size).
    ui::Shadow::ShadowType nShadowType = ui::Shadow::ShadowType::kShadowDefault;
    ui::UiSize szBorderRound;
    ui::UiPadding rcShadowCorner;
    DString shadowImage;
    if (ui::Shadow::GetShadowParam(nShadowType, szBorderRound, rcShadowCorner, shadowImage)) {
        attrs.m_rcShadowCorner = rcShadowCorner;
        if (attrs.m_bInitSizeDefined) {
            attrs.m_szInitSize.cx += rcShadowCorner.left + rcShadowCorner.right;
            attrs.m_szInitSize.cy += rcShadowCorner.top + rcShadowCorner.bottom;
        }
    }

    BaseClass::GetCreateWindowAttributes(attrs);
}

void MainForm::PreInitWindow()
{
    BaseClass::PreInitWindow();
    //Use the GPU (Metal) render backend on macOS, matching the XML example.
    SetRenderBackendType(ui::RenderBackendType::kMetal_BackendType);
}

void MainForm::OnInitWindow()
{
    // Use the OS-provided system shadow on all platforms.
    SetShadowAttached(true);
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    SetLayeredWindow(false, false);
    SetEnableShadowSnap(true);
    SetShadowBorderSize(0);

    SetSizeBox(ui::UiRect(4, 4, 4, 4), false);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), false);

    // Build-time generated from child_window.xml
    InitChild_window(this);

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
    // Paint every visible child window on each idle pass so the FPS demo can
    // reach high rates on macOS as well; a strict round-robin makes each child
    // only get a fraction of the idle-loop frequency.
    for (MyChildWindowEvents* pEvents : m_childWindowEvents) {
        if ((pEvents != nullptr) && (pEvents->GetChildWindow() != nullptr) &&
            pEvents->GetChildWindow()->IsVisible()) {
            PaintChildWindow(pEvents->GetChildWindow());
        }
    }
    return true;
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
