#include "duilib/Core/FullscreenBox.h"
#include "duilib/Core/Window.h"
#include "duilib/Control/Button.h"

namespace ui 
{
FullscreenBox::FullscreenBox(Window* pWindow) :
    Box(pWindow),
    m_nOldItemIndex(0),
    m_bWindowMaximized(false),
    m_bWindowFullscreen(false)
{
    //Disable the control's own padding
    SetEnableControlPadding(false);

    //The background color defaults to white (if no background color is set, the window may appear transparent in some cases, e.g. when the WebView2 control's web page is fullscreen)
    SetBkColor(_T("white"));

    //Save the original state of the window
    if (pWindow != nullptr) {
        m_bWindowMaximized = pWindow->IsWindowMaximized();
        m_bWindowFullscreen = pWindow->IsWindowFullscreen();
    }
}

FullscreenBox::~FullscreenBox()
{
    //When exiting the window directly from fullscreen, the original root needs to be released manually
    Box* pOldRoot = m_pOldRoot.get();
    if (pOldRoot != nullptr) {
        delete pOldRoot;
        pOldRoot = nullptr;
    }
}

DString FullscreenBox::GetType() const { return _T("FullscreenBox"); }

bool FullscreenBox::EnterControlFullscreen(Box* pOldRoot, Control* pFullscreenControl, const DString& exitButtonClass)
{
    ASSERT((pOldRoot != nullptr) && (pFullscreenControl != nullptr));
    if ((pOldRoot == nullptr) || (pFullscreenControl == nullptr)) {
        return false;
    }

    ASSERT(m_pFullscreenControl == nullptr);
    if (m_pFullscreenControl != nullptr) {
        return false;
    }
    ASSERT(m_pOldParent == nullptr);
    if (m_pOldParent != nullptr) {
        return false;
    }
    ASSERT(m_pOldRoot == nullptr);
    if (m_pOldRoot != nullptr) {
        return false;
    }

    //Extract the control from its original container
    RemoveControlFromBox(pFullscreenControl);

    //Hide the original control
    pOldRoot->SetVisible(false);
    m_pOldRoot = pOldRoot;

    //Add it to the fullscreen container
    AddItem(pFullscreenControl);
    pFullscreenControl->SetVisible(true);

    //Set the "exit fullscreen" button
    UpdateExitFullscreenBtn(exitButtonClass);
    return true;
}

bool FullscreenBox::UpdateControlFullscreen(Control* pFullscreenControl, const DString& exitButtonClass)
{
    if (pFullscreenControl == nullptr) {
        return false;
    }
    //Restore the old fullscreen control
    RestoreControlToBox();

    //Extract the control from its original container
    RemoveControlFromBox(pFullscreenControl);

    //Add it to the fullscreen container
    AddItem(pFullscreenControl);
    pFullscreenControl->SetVisible(true);

    //Set the "exit fullscreen" button
    UpdateExitFullscreenBtn(exitButtonClass);
    return true;
}

void FullscreenBox::ExitControlFullscreen()
{
    //Restore the old fullscreen control
    RestoreControlToBox();

    m_pOldRoot.reset();
    m_pExitFullscreenBtn.reset();
    m_exitButtonClass.clear();
}

void FullscreenBox::RemoveControlFromBox(Control* pFullscreenControl)
{
    if (pFullscreenControl == nullptr) {
        return;
    }
    m_pOldParent = pFullscreenControl->GetParent();
    if (m_pOldParent != nullptr) {
        //Record the index in the parent container
        m_nOldItemIndex = m_pOldParent->GetItemIndex(pFullscreenControl);

        //Remove it from the original parent container
        bool bOldAutoDestroyChild = m_pOldParent->IsAutoDestroyChild();
        m_pOldParent->SetAutoDestroyChild(false);
        m_pOldParent->Box::RemoveItem(pFullscreenControl);
        m_pOldParent->SetAutoDestroyChild(bOldAutoDestroyChild);

        //Save the outer margin
        m_rcOldMargin = pFullscreenControl->GetMargin();
    }
    m_pFullscreenControl = pFullscreenControl;
}

void FullscreenBox::RestoreControlToBox()
{
    if ((m_pOldParent != nullptr) && (m_pFullscreenControl != nullptr)) {
        SetAutoDestroyChild(false);
        RemoveItem(m_pFullscreenControl.get());
        SetAutoDestroyChild(true);

        //Restore the original outer margin
        m_pFullscreenControl->SetMargin(m_rcOldMargin, false);

        //Restore it to the original container
        m_pOldParent->Box::AddItemAt(m_pFullscreenControl.get(), m_nOldItemIndex);
    }
    m_pFullscreenControl.reset();
    m_pOldParent.reset();
    m_nOldItemIndex = 0;
    m_rcOldMargin.Clear();
}

void FullscreenBox::UpdateExitFullscreenBtn(const DString& exitButtonClass)
{
    if (m_exitButtonClass == exitButtonClass) {
        if (m_exitButtonClass.empty()) {
            if (m_pExitFullscreenBtn != nullptr) {
                RemoveItem(m_pExitFullscreenBtn.get());
                m_pExitFullscreenBtn.reset();
            }
            return;
        }
        else {
            if (m_pExitFullscreenBtn != nullptr) {
                return;
            }
        }
    }
    if (m_pExitFullscreenBtn != nullptr) {
        RemoveItem(m_pExitFullscreenBtn.get());
        m_pExitFullscreenBtn.reset();
    }

    m_exitButtonClass = exitButtonClass;
    if (!exitButtonClass.empty()) {
        Button* pButton = new Button(GetWindow());
        m_pExitFullscreenBtn = pButton;
        AddItem(pButton);
        pButton->SetClass(exitButtonClass);

        pButton->AttachClick([this](const EventArgs&) {
            //Exit fullscreen
            if (GetWindow()) {
                GetWindow()->ExitControlFullscreen();
            }
            return true;
            });
    }
}

void FullscreenBox::ProcessFullscreenButtonMouseMove(const UiPoint& pt)
{
    if (m_pExitFullscreenBtn != nullptr) {
        Control* pExitFullscreenBtn = m_pExitFullscreenBtn.get();
        if (pExitFullscreenBtn->GetRect().ContainsPt(pt)) {
            pExitFullscreenBtn->SetFadeVisible(true);
        }
        else if (pExitFullscreenBtn->GetAlpha() > 0) {
            pExitFullscreenBtn->SetFadeVisible(false);
        }
    }
}

Control* FullscreenBox::GetFullscreenControl() const
{
    return m_pFullscreenControl.get();
}

Box* FullscreenBox::GetOldRoot() const
{
    return m_pOldRoot.get();
}

bool FullscreenBox::IsWindowOldMaximized() const
{
    return m_bWindowMaximized;
}

bool FullscreenBox::IsWindowOldFullscreen() const
{
    return m_bWindowFullscreen;
}

} // namespace ui
