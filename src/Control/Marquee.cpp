#include "dui/Control/Marquee.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Core/Window.h"
#include "dui/Render/IRender.h"
#include "dui/Render/AutoClip.h"
#include "dui/Utils/StringUtil.h"

namespace ui
{

Marquee::Marquee(Window* pWindow) :
    BaseClass(pWindow),
    m_direction(MarqueeDirection::kLeft),
    m_nScrollSpeed(50),
    m_nScrollInterval(30),
    m_bHoverPause(true),
    m_bPaused(false),
    m_bHovering(false),
    m_bScrolling(false),
    m_bTimerStarted(false),
    m_nTimerId(0),
    m_fOffset(0.0f),
    m_fPeriod(0.0f),
    m_fTextMainSize(0.0f),
    m_fTextCrossSize(0.0f)
{
}

Marquee::~Marquee()
{
    StopTimer();
}

std::string Marquee::GetType() const
{
    return DUI_CTR_MARQUEE;
}

void Marquee::SetAttribute(const std::string& strName, const std::string& strValue)
{
    if ((strName == "scroll_direction") || (strName == "scrolldirection")) {
        if (strValue == "right") {
            SetScrollDirection(MarqueeDirection::kRight);
        }
        else if (strValue == "up") {
            SetScrollDirection(MarqueeDirection::kUp);
        }
        else if (strValue == "down") {
            SetScrollDirection(MarqueeDirection::kDown);
        }
        else {
            SetScrollDirection(MarqueeDirection::kLeft);
        }
    }
    else if ((strName == "scroll_speed") || (strName == "scrollspeed")) {
        SetScrollSpeed(StringUtil::StringToInt32(strValue));
    }
    else if ((strName == "scroll_interval") || (strName == "scrollinterval")) {
        SetScrollInterval(StringUtil::StringToInt32(strValue));
    }
    else if ((strName == "hover_pause") || (strName == "hoverpause")) {
        //dui has no StringUtil::IsValueTrue; booleans are compared against "true", as in the other dui controls
        SetHoverPause(strValue == "true");
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

void Marquee::SetScrollDirection(MarqueeDirection direction)
{
    if (m_direction != direction) {
        m_direction = direction;
        //The scroll direction decides the text layout: up/down scrolling uses vertical text, left/right scrolling uses horizontal text
        bool bVertical = (direction == MarqueeDirection::kUp) || (direction == MarqueeDirection::kDown);
        SetVerticalText(bVertical);
        ResetScroll();
        Invalidate();
    }
}

void Marquee::SetScrollSpeed(int32_t nSpeed)
{
    if (nSpeed < 0) {
        nSpeed = 0;
    }
    if (m_nScrollSpeed != nSpeed) {
        m_nScrollSpeed = nSpeed;
    }
}

void Marquee::SetScrollInterval(int32_t nIntervalMs)
{
    if (nIntervalMs < 5) {
        nIntervalMs = 5;
    }
    if (nIntervalMs > 1000) {
        nIntervalMs = 1000;
    }
    if (m_nScrollInterval != nIntervalMs) {
        m_nScrollInterval = nIntervalMs;
        //Restart the timer so that the new interval takes effect
        if (m_bTimerStarted) {
            StopTimer();
            StartTimer();
        }
    }
}

void Marquee::SetHoverPause(bool bHoverPause)
{
    if (m_bHoverPause != bHoverPause) {
        m_bHoverPause = bHoverPause;
    }
}

void Marquee::Pause()
{
    if (!m_bPaused) {
        m_bPaused = true;
    }
}

void Marquee::Resume()
{
    if (m_bPaused) {
        m_bPaused = false;
    }
}

void Marquee::SetPaused(bool bPaused)
{
    m_bPaused = bPaused;
}

void Marquee::SetText(const std::string& strText)
{
    BaseClass::SetText(strText);
    ResetScroll();
}

void Marquee::SetTextId(const std::string& strTextId)
{
    BaseClass::SetTextId(strTextId);
    ResetScroll();
}

void Marquee::ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale)
{
    BaseClass::ChangeDpiScale(nOldDpiScale, nNewDpiScale);
    //The pixel size of the text changes with the DPI, so clear the measurement cache
    ResetScroll();
    m_strMeasuredText.clear();
    m_fTextMainSize = 0.0f;
    m_fTextCrossSize = 0.0f;
}

void Marquee::SetWindow(Window* pWindow)
{
    if (GetWindow() != pWindow) {
        StopTimer();
    }
    BaseClass::SetWindow(pWindow);
}

bool Marquee::MouseEnter(const EventArgs& msg)
{
    m_bHovering = true;
    return BaseClass::MouseEnter(msg);
}

bool Marquee::MouseLeave(const EventArgs& msg)
{
    m_bHovering = false;
    return BaseClass::MouseLeave(msg);
}

bool Marquee::IsPausedEffective() const
{
    return m_bPaused || (m_bHoverPause && m_bHovering);
}

void Marquee::StartTimer()
{
    if (m_bTimerStarted || !IsInited()) {
        return;
    }
    m_bTimerStarted = true;
    m_timerFlag.Cancel(); //Cancel the old weakFlag first to avoid leftovers
    std::function<void()> callback = UiBind(&Marquee::OnTimerTick, this);
    TimerManager& timer = GlobalManager::Instance().Timer();
    m_nTimerId = timer.AddTimer(m_timerFlag.GetWeakFlag(), callback,
                                static_cast<uint32_t>(m_nScrollInterval), -1);
}

void Marquee::StopTimer()
{
    if (m_bTimerStarted) {
        if (m_nTimerId > 0) {
            GlobalManager::Instance().Timer().RemoveTimer(m_nTimerId);
            m_nTimerId = 0;
        }
        m_bTimerStarted = false;
    }
    m_timerFlag.Cancel();
}

void Marquee::ResetScroll()
{
    m_fOffset = 0.0f;
    m_fPeriod = 0.0f;
    //The text content changed, so the measurement cache is no longer valid (it is rebuilt on the next paint)
    m_strMeasuredText.clear();
    m_fTextMainSize = 0.0f;
    m_fTextCrossSize = 0.0f;
    //After a text change the scrolling state has to be decided again on the next paint (the text may switch between scrolling and static)
}

void Marquee::OnTimerTick()
{
    if (IsPausedEffective()) {
        return;
    }
    if (m_nScrollSpeed <= 0) {
        return;
    }
    const float fDelta = static_cast<float>(Dpi().GetScaleInt(m_nScrollSpeed)) *
                         static_cast<float>(m_nScrollInterval) / 1000.0f;
    m_fOffset += fDelta;
    //Cycle by the period (the period is measured and cached by PaintText)
    if ((m_fPeriod > 0.0f) && (m_fOffset >= m_fPeriod)) {
        m_fOffset -= m_fPeriod;
    }
    Invalidate();
}

void Marquee::PaintText(IRender* pRender)
{
    if (pRender == nullptr) {
        BaseClass::PaintText(pRender);
        return;
    }
    const std::string text = GetText();
    if (text.empty()) {
        BaseClass::PaintText(pRender);
        return;
    }

    UiRect rc = GetRect();
    rc.Deflate(GetControlPadding());
    rc.Deflate(GetTextPadding());
    if ((rc.Width() <= 0) || (rc.Height() <= 0)) {
        return;
    }

    const bool bVertical = IsVerticalText();
    //Measure the text size (cached: the previous result is reused while the text is unchanged, avoiding a MeasureString call on every frame)
    if (text != m_strMeasuredText) {
        MeasureStringParam measureParam = GetMeasureParam();
        UiRect rcTextSize = pRender->MeasureString(text, measureParam);
        m_strMeasuredText = text;
        m_fTextMainSize = bVertical ? static_cast<float>(rcTextSize.Height())
                                    : static_cast<float>(rcTextSize.Width());
        m_fTextCrossSize = bVertical ? static_cast<float>(rcTextSize.Width())
                                     : static_cast<float>(rcTextSize.Height());
    }
    const float fTextMainSize = m_fTextMainSize;
    const float fViewMainSize = bVertical ? static_cast<float>(rc.Height())
                                          : static_cast<float>(rc.Width());
    const bool bNeedScroll = (fTextMainSize > fViewMainSize);

    //Scrolling state changed: start/stop the timer
    if (bNeedScroll != m_bScrolling) {
        m_bScrolling = bNeedScroll;
        if (bNeedScroll) {
            StartTimer();
        }
        else {
            StopTimer();
            m_fOffset = 0.0f;
        }
    }

    //Text fits: show it statically (using the default Label alignment painting)
    if (!bNeedScroll) {
        BaseClass::PaintText(pRender);
        return;
    }

    //Scroll painting: two copies of the text, end to end, for a seamless loop
    const float fGap = fViewMainSize / 3.0f;
    m_fPeriod = fTextMainSize + fGap;
    //Normalize the offset into [0, period)
    if (m_fPeriod > 0.0f) {
        while (m_fOffset >= m_fPeriod) {
            m_fOffset -= m_fPeriod;
        }
        while (m_fOffset < 0.0f) {
            m_fOffset += m_fPeriod;
        }
    }

    //Text color
    ControlStateType stateType = kControlStateNormal;
    std::string clrName = GetPaintStateTextColor(GetState(), stateType);
    UiColor textColor = GetUiColor(clrName);
    if (textColor.IsEmpty()) {
        textColor = UiColor(0xFF000000);
    }

    DrawStringParam drawParam = GetDrawParam();
    drawParam.dwTextColor = textColor;
    drawParam.uFade = 255;

    const float fOff = m_fOffset;
    //Clip to the display area so the scrolling text is not painted outside the control
    std::unique_ptr<AutoClip> spClip = CreateRectClip(pRender, rc, true);

    if (bVertical) {
        //Vertical text: the column width is the text width, centered horizontally in the display area
        const float fColW = m_fTextCrossSize;
        const float fX = static_cast<float>(rc.left) + (static_cast<float>(rc.Width()) - fColW) / 2.0f;
        const float fX2 = fX + fColW;
        float fY1;
        if (m_direction == MarqueeDirection::kUp) {
            fY1 = static_cast<float>(rc.top) - fOff;
        }
        else { //kDown
            fY1 = static_cast<float>(rc.top) - m_fPeriod + fOff;
        }
        const float fY2 = fY1 + m_fPeriod;
        drawParam.uFormat = TEXT_LEFT | TEXT_TOP | TEXT_VERTICAL;
        drawParam.textRect = UiRect(static_cast<int32_t>(fX), static_cast<int32_t>(fY1),
                                    static_cast<int32_t>(fX2), static_cast<int32_t>(fY1 + fTextMainSize));
        pRender->DrawString(text, drawParam);
        drawParam.textRect = UiRect(static_cast<int32_t>(fX), static_cast<int32_t>(fY2),
                                    static_cast<int32_t>(fX2), static_cast<int32_t>(fY2 + fTextMainSize));
        pRender->DrawString(text, drawParam);
    }
    else {
        //Horizontal text: the row height is the text height, centered vertically in the display area
        const float fRowH = m_fTextCrossSize;
        const float fY = static_cast<float>(rc.top) + (static_cast<float>(rc.Height()) - fRowH) / 2.0f;
        const float fY2 = fY + fRowH;
        float fX1;
        if (m_direction == MarqueeDirection::kLeft) {
            fX1 = static_cast<float>(rc.left) - fOff;
        }
        else { //kRight
            fX1 = static_cast<float>(rc.left) - m_fPeriod + fOff;
        }
        const float fX2 = fX1 + m_fPeriod;
        drawParam.uFormat = TEXT_LEFT | TEXT_TOP | TEXT_SINGLELINE;
        drawParam.textRect = UiRect(static_cast<int32_t>(fX1), static_cast<int32_t>(fY),
                                    static_cast<int32_t>(fX1 + fTextMainSize), static_cast<int32_t>(fY2));
        pRender->DrawString(text, drawParam);
        drawParam.textRect = UiRect(static_cast<int32_t>(fX2), static_cast<int32_t>(fY),
                                    static_cast<int32_t>(fX2 + fTextMainSize), static_cast<int32_t>(fY2));
        pRender->DrawString(text, drawParam);
    }
    spClip.reset();
}

} // namespace ui
