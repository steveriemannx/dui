#include "dui/Core/DragWindow.h"
#include "dui/Core/GlobalManager.h"

namespace ui
{
/** The bitmap display control
*/
class DragWindowBitmap: public Control
{
    typedef Control BaseClass;
public:
    explicit DragWindowBitmap(Window* pWindow):
        Control(pWindow)
    {
    }

    /** The draw function
    */
    virtual void Paint(ui::IRender* pRender, const ui::UiRect& rcPaint) override
    {
        BaseClass::Paint(pRender, rcPaint);
        if ((pRender != nullptr) && (m_pBitmap != nullptr)) {
            UiRect rcCorners;
            UiRect destRect = GetRect();
            UiRect srcRect;
            srcRect.right = m_pBitmap->GetWidth();
            srcRect.bottom = m_pBitmap->GetHeight();
            //Align to the center
            if (srcRect.Height() < destRect.Height()) {
                destRect.top = destRect.CenterY() - srcRect.Height() / 2;
                destRect.bottom = destRect.top + srcRect.Height();
            }
            if (srcRect.Width() < destRect.Width()) {
                destRect.left = destRect.CenterX() - srcRect.Width() / 2;
                destRect.right = destRect.left + srcRect.Width();
            }
            pRender->DrawImage(rcPaint, m_pBitmap.get(), destRect, rcCorners, srcRect, rcCorners);
        }
    }

    /** Set the bitmap to be drawn
    */
    void SetBitmap(const std::shared_ptr<IBitmap>& pBitmap)
    {
        m_pBitmap = pBitmap;
    }

private:
    /** The bitmap to be displayed
    */
    std::shared_ptr<IBitmap> m_pBitmap;
};

DragWindow::DragWindow():
    m_nRefCount(0)
{
}

DragWindow::~DragWindow()
{
}

void DragWindow::AddRef()
{
    ASSERT(m_nRefCount >= 0);
    ++m_nRefCount;
}

void DragWindow::Release()
{
    ASSERT(m_nRefCount > 0);
    --m_nRefCount;
    if (m_nRefCount == 0) {
        delete this;
    }
}

DString DragWindow::GetSkinFolder()
{
    return DUI_T("");
}

DString DragWindow::GetSkinFile()
{
    return DUI_T("<?xml version = \"1.0\" encoding=\"utf-8\"?>")
           DUI_T("<Window size=\"90,90\" shadow_snap=\"false\">")
           DUI_T("    <VBox width=\"stretch\" height=\"stretch\" visible=\"true\" bkcolor=\"white\"/>")
           DUI_T("</Window>");
}

void DragWindow::OnFinalMessage()
{
    Release();
}

void DragWindow::SetDragImage(const std::shared_ptr<IBitmap>& pBitmap)
{
    Box* pBox = GetXmlRoot();
    ASSERT(pBox != nullptr);
    if (pBox == nullptr) {
        return;
    }

    int32_t nExtraHeight = 0;
    if (pBitmap == nullptr) {
        const size_t nCount = pBox->GetItemCount();
        if (nCount > 0) {
            DragWindowBitmap* pBitmapControl = dynamic_cast<DragWindowBitmap*>(pBox->GetItemAt(nCount - 1));
            if (pBitmapControl != nullptr) {
                pBitmapControl->SetBitmap(nullptr);
            }
        }
        return;
    }
    else {
        const size_t nCount = pBox->GetItemCount();
        for (size_t nItem = 0; nItem < nCount; ++nItem) {
            Control* pControl = pBox->GetItemAt(nItem);
            if (pControl != nullptr) {
                if (pControl->GetFixedHeight().IsAuto()) {
                    UiRect rcClient;
                    GetClientRect(rcClient);
                    nExtraHeight = pControl->EstimateSize(UiSize(rcClient.Width(), rcClient.Height())).cy.GetInt32();
                }
                else {
                    nExtraHeight = pControl->GetFixedHeight().GetInt32();
                }
            }
        }
    }
    const int32_t nImageWidth = pBitmap->GetWidth();
    const int32_t nImageHeight = pBitmap->GetHeight();

    DragWindowBitmap* pBitmapControl = new DragWindowBitmap(this);
    pBitmapControl->SetBitmap(pBitmap);
    pBitmapControl->SetAttribute(DUI_T("width"), DUI_T("100%"));
    pBitmapControl->SetAttribute(DUI_T("height"), DUI_T("100%"));
    pBitmapControl->SetAttribute(DUI_T("bkcolor"), DUI_T("white"));
    pBox->AddItem(pBitmapControl);

    //Adjust the window size according to the size of the bitmap
    int32_t cx = nImageWidth;
    int32_t cy = nImageHeight + nExtraHeight;
    Dpi().ClientSizeToWindowSize(cx);
    Dpi().ClientSizeToWindowSize(cy);
    Resize(cx, cy, false, false);
}

void DragWindow::AdjustPos()
{
    UiPadding rcCorner = GetCurrentShadowCorner();
    Dpi().ClientSizeToWindowSize(rcCorner);
    UiPoint ptCursor;
    GetCursorPos(ptCursor);
    ptCursor.x -= rcCorner.left;
    ptCursor.y -= rcCorner.top;
    UiRect rc;
    GetWindowRect(rc);
    ptCursor.x -= (rc.right - rc.left - rcCorner.left - rcCorner.right) / 2;
   
    rc.left = ptCursor.x;
    rc.top = ptCursor.y;
    rc.right = rc.left;
    rc.bottom = rc.top;
    SetWindowPos(InsertAfterWnd(), rc.left, rc.top, rc.Width(), rc.Height(), kSWP_NOSIZE | kSWP_SHOWWINDOW | kSWP_NOACTIVATE);
}

}
