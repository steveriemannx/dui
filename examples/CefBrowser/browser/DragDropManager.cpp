#include "DragDropManager.h"
#include "browser/DragForm.h"
#include "browser/BrowserForm.h"
#include "browser/BrowserManager.h"

DragDropManager::DragDropManager():
    m_pDragingBox(nullptr),
    m_pDragForm(nullptr)
{
}

DragDropManager::~DragDropManager()
{
}

DragDropManager* DragDropManager::GetInstance()
{
    static DragDropManager self;
    return &self;
}

bool DragDropManager::IsDragingBorwserBox() const
{
    return (m_pDragingBox != nullptr);
}

bool DragDropManager::StartDragBorwserBox(BrowserBox* browserBox, std::shared_ptr<ui::IBitmap> spIBitmap, const ui::UiPoint& ptOffset)
{
    ASSERT(browserBox != nullptr);
    if (browserBox == nullptr) {
        return false;
    }
    m_pDragingBox = browserBox;
    BrowserForm* dragBrowserForm = dynamic_cast<BrowserForm*>(m_pDragingBox->GetBrowserForm());
    ASSERT(dragBrowserForm != nullptr);
    if (dragBrowserForm == nullptr) {
        m_pDragingBox = nullptr;
        return false;
    }

    // Get the number of browser boxes in the dragged browser window
    int32_t box_count = dragBrowserForm->GetBoxCount();
    ASSERT(box_count > 0);

    if (!dragBrowserForm->OnBeforeDragBoxCallback(ui::StringConvert::UTF8ToT(m_pDragingBox->GetBrowserId()))) {
        m_pDragingBox = nullptr;
        return false;
    }

    if ((m_pDragForm != nullptr) && m_pDragForm->IsClosingWnd()) {
        m_pDragForm->Release();
        m_pDragForm = nullptr;
    }
    if (m_pDragForm == nullptr) {
        m_pDragForm = new DragForm;
        m_pDragForm->AddRef();        

        ui::WindowCreateParam createWndParam;
        createWndParam.m_dwStyle = ui::kWS_POPUP;
        createWndParam.m_dwExStyle = ui::kWS_EX_LAYERED | ui::kWS_EX_NOACTIVATE | ui::kWS_EX_TRANSPARENT;
        m_pDragForm->CreateWnd(nullptr, createWndParam);
        ASSERT(m_pDragForm->IsWindow());
        if (m_pDragForm->IsWindow()) {
            m_pDragForm->AddRef();
        }

        DString title = m_pDragingBox->GetTitle();
        ui::Box* pRootBox = m_pDragForm->GetXmlRoot();
        if (pRootBox != nullptr) {
            if (pRootBox->GetItemCount() > 0) {
                ui::Label* pTitle = dynamic_cast<ui::Label*>(pRootBox->GetItemAt(0));
                if (pTitle != nullptr) {
                    pTitle->SetText(title);
                }
            }
        }
        m_pDragForm->SetDragImage(spIBitmap);
        m_pDragForm->ShowWindow(ui::kSW_SHOW_NA);
        m_pDragForm->AdjustPos();
    }
    return true;
}

void DragDropManager::EndDragBorwserBox(bool bSuccess)
{
    if (m_pDragForm != nullptr) {
        if (!m_pDragForm->IsClosingWnd()) {
            m_pDragForm->SetDragImage(nullptr);
            m_pDragForm->CloseWnd();
        }
        m_pDragForm->Release();
        m_pDragForm = nullptr;
    }

    if (m_pDragingBox == nullptr) {
        return;
    }

    // Get the browser window that the currently dragged browser box belongs to
    BrowserForm* dragBrowserForm = dynamic_cast<BrowserForm*>(m_pDragingBox->GetBrowserForm());
    ASSERT(dragBrowserForm != nullptr);
    if (dragBrowserForm == nullptr) {
        m_pDragingBox = nullptr;
        return;
    }

    if (!bSuccess) {
        //Operation failed
        dragBrowserForm->OnAfterDragBoxCallback(false);
        m_pDragingBox = nullptr;
        return;
    }

    //Determine the target window
    BrowserForm* dropBrowserForm = nullptr;
    ui::UiPoint screenPt;
    dragBrowserForm->GetCursorPos(screenPt);
    ui::Window* pWindow = dragBrowserForm->WindowFromPoint(screenPt, true);
    if (pWindow != nullptr) {
        dropBrowserForm = dynamic_cast<BrowserForm*>(pWindow);
    }

    // Get the number of browser boxes in the dragged browser window
    int box_count = dragBrowserForm->GetBoxCount();
    ASSERT(box_count > 0);

    if (dragBrowserForm == dropBrowserForm) {
        dropBrowserForm = nullptr;
    }

    if (dropBrowserForm != nullptr) {
        //Put the dragged browser box into another browser window
        dragBrowserForm->OnAfterDragBoxCallback(true);
        if (dragBrowserForm->DetachBox(m_pDragingBox)) {
            dropBrowserForm->AttachBox(m_pDragingBox);
        }
    }
    else {
        // If there is only one browser box in the dragged browser window, the drag fails
        if (1 == box_count) {
            dragBrowserForm->OnAfterDragBoxCallback(false);
        }
        // If there are multiple browser boxes, detach the browser box from the original browser window and attach it to a new browser window; the drag succeeds
        else {
            dragBrowserForm->OnAfterDragBoxCallback(true);
            if (dragBrowserForm->DetachBox(m_pDragingBox)) {
                BrowserForm* newBrowserForm = BrowserManager::GetInstance()->CreateBrowserForm();
                if (newBrowserForm->CreateWnd(nullptr, ui::WindowCreateParam(_T("CefBrowser")))) {
                    if (newBrowserForm->AttachBox(m_pDragingBox)) {
                        // Set the position of the new browser window here, offset by 100,20 from the mouse coordinates
                        ui::UiPoint pt_mouse;
                        newBrowserForm->GetCursorPos(pt_mouse);

                        const int kDragFormXOffset = -100;   //The x offset relative to the mouse after dragging out a new browser window
                        const int kDragFormYOffset = -20;    //The y offset relative to the mouse after dragging out a new browser window
                        ui::UiRect rect(pt_mouse.x + newBrowserForm->Dpi().GetScaleInt(kDragFormXOffset),
                                        pt_mouse.y + newBrowserForm->Dpi().GetScaleInt(kDragFormYOffset),
                                        0, 0);
                        newBrowserForm->SetWindowPos(ui::InsertAfterWnd(), rect.left, rect.top, rect.Width(), rect.Height(), ui::kSWP_NOSIZE);
                        newBrowserForm->ShowWindow(ui::kSW_SHOW_NORMAL);
                    }
                }
            }
        }
    }
    m_pDragingBox = nullptr;
}

void DragDropManager::UpdateDragFormPos()
{
    if (m_pDragingBox != nullptr) {
        //Bring the target window to the front
        BrowserForm* dragBrowserForm = dynamic_cast<BrowserForm*>(m_pDragingBox->GetBrowserForm());
        ASSERT(dragBrowserForm != nullptr);
        if (dragBrowserForm != nullptr) {
            BrowserForm* dropBrowserForm = nullptr;
            ui::UiPoint screenPt;
            dragBrowserForm->GetCursorPos(screenPt);
            ui::Window* pWindow = dragBrowserForm->WindowFromPoint(screenPt, true);
            if (pWindow != nullptr) {
                dropBrowserForm = dynamic_cast<BrowserForm*>(pWindow);
            }
            if (dropBrowserForm != nullptr) {
                ui::InsertAfterWnd insertAfterWnd;
                if ((m_pDragForm != nullptr) && !m_pDragForm->IsClosingWnd()) {
                    insertAfterWnd.m_pWindow = m_pDragForm;
                }
                dropBrowserForm->SetWindowPos(insertAfterWnd, 0, 0, 0, 0, ui::kSWP_NOSIZE | ui::kSWP_NOMOVE | ui::kSWP_NOACTIVATE);
            }
        }
    }

    if (m_pDragForm != nullptr) {
        if (IsDragingBorwserBox()) {
            if (!m_pDragForm->IsClosingWnd()) {
                m_pDragForm->AdjustPos();
            }
        }
        else {
            if (!m_pDragForm->IsClosingWnd()) {
                m_pDragForm->SetDragImage(nullptr);
                m_pDragForm->CloseWnd();
            }
            m_pDragForm->Release();
            m_pDragForm = nullptr;
        }
    }
}
