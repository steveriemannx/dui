#include "duilib/Box/ListBoxHelper.h"
#include "duilib/Box/ListBox.h"
#include "duilib/Render/IRender.h"
#include "duilib/Core/GlobalManager.h"

namespace ui 
{
ListBoxHelper::ListBoxHelper(ListBox* pListBox) :
    m_pListBox(pListBox),
    m_bMouseDown(false),
    m_bRMouseDown(false),
    m_bInMouseMove(false),
    m_pMouseSender(nullptr),
    m_bMouseDownInView(false),
    m_bEnableFrameSelection(false),
    m_frameSelectionBorderSize(1),
    m_frameSelectionBorderColor(_T("#FF0078D7")),
    m_frameSelectionColor(_T("#FFAACCEE")),
    m_frameSelectionAlpha(128),
    m_nNormalItemTop(-1)
{
}

ListBoxHelper::~ListBoxHelper()
{
}

void ListBoxHelper::SetEnableFrameSelection(bool bEnable)
{
    m_bEnableFrameSelection = bEnable;
}

bool ListBoxHelper::IsEnableFrameSelection() const
{
    return m_bEnableFrameSelection;
}

void ListBoxHelper::SetFrameSelectionColor(const DString& frameSelectionColor)
{
    m_frameSelectionColor = frameSelectionColor;
}

DString ListBoxHelper::GetFrameSelectionColor() const
{
    return m_frameSelectionColor.c_str();
}

void ListBoxHelper::SetframeSelectionAlpha(uint8_t frameSelectionAlpha)
{
    m_frameSelectionAlpha = frameSelectionAlpha;
}

uint8_t ListBoxHelper::GetFrameSelectionAlpha() const
{
    return m_frameSelectionAlpha;
}

void ListBoxHelper::SetFrameSelectionBorderColor(const DString& frameSelectionBorderColor)
{
    m_frameSelectionBorderColor = frameSelectionBorderColor;
}

DString ListBoxHelper::GetFrameSelectionBorderColor() const
{
    return m_frameSelectionBorderColor.c_str();
}

void ListBoxHelper::SetFrameSelectionBorderSize(int32_t nBorderSize)
{
    if (nBorderSize < 0) {
        nBorderSize = 0;
    }
    m_frameSelectionBorderSize = (uint8_t)nBorderSize;
}

int32_t ListBoxHelper::GetFrameSelectionBorderSize() const
{
    return m_frameSelectionBorderSize;
}

void ListBoxHelper::SetNormalItemTop(int32_t nNormalItemTop)
{
    m_nNormalItemTop = nNormalItemTop;
}

int32_t ListBoxHelper::GetNormalItemTop() const
{
    return m_nNormalItemTop;
}

void ListBoxHelper::PaintFrameSelection(IRender* pRender)
{
    if (!m_bInMouseMove || (pRender == nullptr)) {
        return;
    }
    UiSize64 scrollPos = m_pListBox->GetScrollPos();
    int64_t left = std::min(m_ptMouseDown.cx, m_ptMouseMove.cx) - scrollPos.cx;
    int64_t right = std::max(m_ptMouseDown.cx, m_ptMouseMove.cx) - scrollPos.cx;
    int64_t top = std::min(m_ptMouseDown.cy, m_ptMouseMove.cy) - scrollPos.cy;
    int64_t bottom = std::max(m_ptMouseDown.cy, m_ptMouseMove.cy) - scrollPos.cy;
    if (m_nNormalItemTop > 0) {
        if (top < m_nNormalItemTop) {
            top = (int64_t)m_nNormalItemTop - m_pListBox->Dpi().GetScaleInt(4);
        }
        if (bottom < m_nNormalItemTop) {
            bottom = m_nNormalItemTop;
        }
    }

    UiRectF rect(TruncateToInt32(left), TruncateToInt32(top),
                 TruncateToInt32(right), TruncateToInt32(bottom));

    int32_t frameSelectionBorderSize = GetFrameSelectionBorderSize();    
    if ((frameSelectionBorderSize > 0) && !m_frameSelectionBorderColor.empty()) {
        float fSelectionBorderSize = m_pListBox->Dpi().GetScaleFloat(frameSelectionBorderSize);
        pRender->DrawRect(rect, m_pListBox->GetUiColor(m_frameSelectionBorderColor.c_str()), fSelectionBorderSize);
    }
    if (!m_frameSelectionColor.empty()) {
        pRender->FillRect(rect, m_pListBox->GetUiColor(m_frameSelectionColor.c_str()), m_frameSelectionAlpha);
    }
}

void ListBoxHelper::OnButtonDown(const UiPoint& ptMouse, Control* pSender)
{
    if (m_bInMouseMove) {
        m_bInMouseMove = false;
        m_pListBox->Invalidate();
    }
    m_bMouseDownInView = (pSender == m_pListBox) ? true : false;
    m_bMouseDown = true;
    m_pMouseSender = pSender;
    UiSize64 scrollPos = m_pListBox->GetScrollPos();
    m_ptMouseDown.cx = ptMouse.x + scrollPos.cx;
    m_ptMouseDown.cy = ptMouse.y + scrollPos.cy;
}

void ListBoxHelper::OnButtonUp(const UiPoint& /*ptMouse*/, Control* pSender)
{
    bool bClickedBlank = false;
    if (m_bMouseDownInView && !m_bInMouseMove && (pSender == m_pListBox)) {
        bClickedBlank = true;
    }
    if (m_bInMouseMove) {
        m_bInMouseMove = false;
        m_pListBox->Invalidate();
    }
    m_bMouseDownInView = false;
    m_bMouseDown = false;
    m_pMouseSender = nullptr;
    if (bClickedBlank) {
        if (m_pListBox->OnLButtonClickedBlank()) {
            m_pListBox->SendEvent(kEventSelChanged);
        }
    }
}

void ListBoxHelper::OnRButtonDown(const UiPoint& ptMouse, Control* pSender)
{
    if (m_bInMouseMove) {
        m_bInMouseMove = false;
        m_pListBox->Invalidate();
    }
    m_bMouseDownInView = (pSender == m_pListBox) ? true : false;
    m_bRMouseDown = true;
    m_pMouseSender = pSender;
    UiSize64 scrollPos = m_pListBox->GetScrollPos();
    m_ptMouseDown.cx = ptMouse.x + scrollPos.cx;
    m_ptMouseDown.cy = ptMouse.y + scrollPos.cy;
}

void ListBoxHelper::OnRButtonUp(const UiPoint& /*ptMouse*/, Control* pSender)
{
    bool bClickedBlank = false;
    if (m_bMouseDownInView && !m_bInMouseMove && (pSender == m_pListBox)) {
        bClickedBlank = true;
    }
    if (m_bInMouseMove) {
        m_bInMouseMove = false;
        m_pListBox->Invalidate();
    }
    m_bMouseDownInView = false;
    m_bRMouseDown = false;
    m_pMouseSender = nullptr;
    if (bClickedBlank) {
        if (m_pListBox->OnRButtonClickedBlank()) {
            m_pListBox->SendEvent(kEventSelChanged);
        }
    }
}

void ListBoxHelper::OnMouseMove(const UiPoint& ptMouse, Control* pSender)
{
    if (!IsEnableFrameSelection() || !m_pListBox->IsMultiSelect()) {
        //Feature disabled, or single-selection mode
        return;
    }
    if ((m_bMouseDown || m_bRMouseDown) &&
        (pSender != nullptr) &&
        (m_pMouseSender == pSender) && pSender->IsMouseFocused()) {
        UiSize64 scrollPos = m_pListBox->GetScrollPos();
        m_ptMouseMove.cx = ptMouse.x + scrollPos.cx;
        m_ptMouseMove.cy = ptMouse.y + scrollPos.cy;

        //Only when the mouse moves beyond the specified number of pixels does the operation start to be treated as a drag, to avoid a normal click being recognized as a frame selection
        const int32_t minPt = m_pListBox->Dpi().GetScaleInt(8);
        if (!m_bInMouseMove) {
            if ((std::abs(m_ptMouseMove.cx - m_ptMouseDown.cx) > minPt) ||
                (std::abs(m_ptMouseMove.cy - m_ptMouseDown.cy) > minPt)) {
                //Start the frame selection operation
                m_bInMouseMove = true;
                OnCheckScrollView();
            }
        }
        else {
            //Scroll the view as needed, and update the mouse position after scrolling
            OnCheckScrollView();
        }
    }
    else if (m_bInMouseMove) {
        m_bInMouseMove = false;
        m_pListBox->Invalidate();
    }
}

void ListBoxHelper::OnWindowKillFocus()
{
    if (m_bInMouseMove) {
        m_pListBox->Invalidate();
    }
    m_bMouseDownInView = false;
    m_bMouseDown = false;
    m_bRMouseDown = false;
    m_bInMouseMove = false;
    m_pMouseSender = nullptr;
}

void ListBoxHelper::OnCheckScrollView()
{
    if (!m_bInMouseMove || !IsEnableFrameSelection()) {
        //Cancel the timer
        m_scrollViewFlag.Cancel();
        return;
    }
    bool bScrollView = false;
    const UiSize64 scrollPos = m_pListBox->GetScrollPos();
    UiSize64 pt = m_ptMouseMove;
    pt.cx -= scrollPos.cx;
    pt.cy -= scrollPos.cy;
    const UiSize64 ptMouseMove = pt; //Record the original value

    if (m_bInMouseMove) {
        int32_t nHScrollValue = DUI_NOSET_VALUE;
        int32_t nVScrollValue = DUI_NOSET_VALUE;
        m_pListBox->GetScrollDeltaValue(nHScrollValue, nVScrollValue);
        UiRect viewRect = m_pListBox->GetRect();
        if (m_nNormalItemTop > 0) {
            viewRect.top = m_nNormalItemTop;
            ASSERT(viewRect.top <= viewRect.bottom);
        }
        if (pt.cx <= viewRect.left) {
            //Scroll the view left
            m_pListBox->LineLeft(nHScrollValue);
            bScrollView = true;
        }
        else if (pt.cx >= viewRect.right) {
            //Scroll the view right
            m_pListBox->LineRight(nHScrollValue);
            bScrollView = true;
        }
        if (pt.cy <= viewRect.top) {
            //Scroll the view up
            m_pListBox->LineUp(nVScrollValue);
            bScrollView = true;
        }
        else if (pt.cy >= viewRect.bottom) {
            //Scroll the view down
            m_pListBox->LineDown(nVScrollValue);
            bScrollView = true;
        }
    }

    if (bScrollView) {
        UiSize64 scrollPosNew = m_pListBox->GetScrollPos();
        if (scrollPos != scrollPosNew) {
            //Update the mouse position
            m_ptMouseMove.cx = ptMouseMove.cx + scrollPosNew.cx;
            m_ptMouseMove.cy = ptMouseMove.cy + scrollPosNew.cy;
        }

        //Start the timer
        m_scrollViewFlag.Cancel();
        GlobalManager::Instance().Timer().AddTimer(m_scrollViewFlag.GetWeakFlag(),
                                                   UiBind(&ListBoxHelper::OnCheckScrollView, this),
                                                   50, 1); //Execute only once
    }
    else {
        //Cancel the timer
        m_scrollViewFlag.Cancel();
    }

    int64_t top = std::min(m_ptMouseDown.cy, m_ptMouseMove.cy);
    int64_t bottom = std::max(m_ptMouseDown.cy, m_ptMouseMove.cy);
    int64_t left = std::min(m_ptMouseDown.cx, m_ptMouseMove.cx);
    int64_t right = std::max(m_ptMouseDown.cx, m_ptMouseMove.cx);
    int32_t offsetTop = m_pListBox->GetRect().top;//The top coordinate of the top-left corner of the current control
    top -= offsetTop;
    bottom -= offsetTop;

    int32_t offsetLeft = m_pListBox->GetRect().left;//The left coordinate of the top-left corner of the current control
    left -= offsetLeft;
    right -= offsetLeft;
    bool bRet = m_pListBox->OnFrameSelection(left, right, top, bottom);
    m_pListBox->Invalidate();
    if (bRet) {
        m_pListBox->SendEvent(kEventSelChanged);
    }
}

} // namespace ui
