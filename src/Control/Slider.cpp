#include "dui/Control/Slider.h"
#include <algorithm>
#include <cmath>
#include "dui/Utils/AttributeUtil.h"
#include "dui/Core/GlobalManager.h"

namespace ui
{

Slider::Slider(Window* pWindow) :
    Progress(pWindow),
    m_szThumb(0, 0),
    m_nStep(1),
    m_sImageModify(),
    m_thumbStateImage(),
    m_rcProgressBarPadding(),
    m_bClickToJump(true),
    m_nPageStep(0),
    m_bShowTicks(false),
    m_nTickCount(5),
    m_nTickLength(0),
    m_strTickColor(),
    m_tickPos(SliderTickPos::kBottom),
    m_bShowTickLabels(false),
    m_nTickLabelInterval(1),
    m_strTickFontId("system_regular_12"),
    m_strTickLabelColor(),
    m_tickLabelCallback()
{
    SetTextStyle(TEXT_SINGLELINE | TEXT_HCENTER, false);
    m_nTickLength = 4;   //Default tick mark length (the raw value before scaling)
}

std::string Slider::GetType() const { return DUI_CTR_SLIDER; }

uint32_t Slider::GetControlFlags() const
{
    //Keyboard accessibility: join the Tab key focus cycle when the control is enabled and
    //keyboard operation is allowed
    return (IsEnabled() && IsKeyboardEnabled() && IsAllowTabStop()) ? UIFLAG_TABSTOP : UIFLAG_DEFAULT;
}

void Slider::SetClickToJump(bool bEnable)
{
    m_bClickToJump = bEnable;
}

void Slider::SetPageStep(int32_t nPageStep)
{
    m_nPageStep = nPageStep;
}

int32_t Slider::GetPageStep() const
{
    if (m_nPageStep > 0) {
        return m_nPageStep;
    }
    //Automatic mode: one tenth of the total range, at least 1
    int32_t nRange = GetMaxValue() - GetMinValue();
    if (nRange <= 0) {
        return 1;
    }
    int32_t nPageStep = nRange / 10;
    if (nPageStep < 1) {
        nPageStep = 1;
    }
    if (nPageStep < m_nStep) {
        nPageStep = m_nStep;
    }
    return nPageStep;
}

void Slider::SetShowTicks(bool bShow)
{
    if (m_bShowTicks != bShow) {
        m_bShowTicks = bShow;
        Invalidate();
    }
}

void Slider::SetTickCount(int32_t nCount)
{
    if (nCount < 2) {
        nCount = 2;
    }
    if (m_nTickCount != nCount) {
        m_nTickCount = nCount;
        Invalidate();
    }
}

void Slider::SetTickLength(int32_t nLength)
{
    if (nLength < 0) {
        nLength = 0;
    }
    if (m_nTickLength != nLength) {
        //Store the raw, unscaled value; it is scaled by the current DPI while painting
        m_nTickLength = nLength;
        Invalidate();
    }
}

int32_t Slider::GetTickLength() const
{
    return m_nTickLength;
}

void Slider::SetTickColor(const std::string& strColor)
{
    if (m_strTickColor != strColor) {
        m_strTickColor = strColor;
        Invalidate();
    }
}

void Slider::SetTickPos(SliderTickPos pos)
{
    if (m_tickPos != pos) {
        m_tickPos = pos;
        Invalidate();
    }
}

void Slider::SetShowTickLabels(bool bShow)
{
    if (m_bShowTickLabels != bShow) {
        m_bShowTickLabels = bShow;
        Invalidate();
    }
}

void Slider::SetTickLabelInterval(int32_t nInterval)
{
    if (nInterval < 1) {
        nInterval = 1;
    }
    if (m_nTickLabelInterval != nInterval) {
        m_nTickLabelInterval = nInterval;
        Invalidate();
    }
}

void Slider::SetTickFontId(const std::string& strFontId)
{
    if (m_strTickFontId != strFontId) {
        m_strTickFontId = strFontId;
        Invalidate();
    }
}

void Slider::SetTickLabelColor(const std::string& strColor)
{
    if (m_strTickLabelColor != strColor) {
        m_strTickLabelColor = strColor;
        Invalidate();
    }
}

void Slider::SetTickLabelCallback(const TickLabelCallback& callback)
{
    m_tickLabelCallback = callback;
    Invalidate();
}

double Slider::GetTickValue(int32_t nIndex) const
{
    const int32_t nMin = GetMinValue();
    const int32_t nMax = GetMaxValue();
    const int32_t nCount = (m_nTickCount > 1) ? m_nTickCount : 1;
    if (nMax <= nMin) {
        return nMin;
    }
    return nMin + double(nMax - nMin) * (double(nIndex) / double(nCount - 1));
}

UiRect Slider::GetProgressPos() const
{
    int32_t nMax = GetMaxValue();
    int32_t nMin = GetMinValue();
    double fValue = GetValue();

    if (nMax <= nMin) {
        nMax = nMin + 1;
    }
    if (fValue > nMax) {
        fValue = nMax;
    }
    if (fValue < nMin) {
        fValue = nMin;
    }

    UiSize szThumb = GetThumbSize();
    UiRect rc;
    if (IsHorizontal()) {
        rc.right = int((fValue - nMin) * (GetRect().right - GetRect().left - szThumb.cx) / (nMax - nMin) + szThumb.cx / 2 + 0.5);
        rc.bottom = GetRect().bottom - GetRect().top;
    }
    else {
        rc.top = int((nMax - fValue) * (GetRect().bottom - GetRect().top - szThumb.cy) / (nMax - nMin) + szThumb.cy / 2 + 0.5);
        rc.right = GetRect().right - GetRect().left;
        rc.bottom = GetRect().bottom - GetRect().top;
    }

    return rc;
}

double Slider::ValueFromMousePt(const UiPoint& ptMouse) const
{
    const int32_t nMin = GetMinValue();
    const int32_t nMax = GetMaxValue();
    const UiSize szThumb = GetThumbSize();
    const UiRect& rc = GetRect();
    if (IsHorizontal()) {
        if (ptMouse.x >= rc.right - szThumb.cx / 2) {
            return nMax;
        }
        if (ptMouse.x <= rc.left + szThumb.cx / 2) {
            return nMin;
        }
        return nMin + double((nMax - nMin) * (ptMouse.x - rc.left - szThumb.cx / 2)) /
            double(rc.right - rc.left - szThumb.cx);
    }
    else {
        if (ptMouse.y >= rc.bottom - szThumb.cy / 2) {
            return nMin;
        }
        if (ptMouse.y <= rc.top + szThumb.cy / 2) {
            return nMax;
        }
        return nMin + double((nMax - nMin) * (rc.bottom - ptMouse.y - szThumb.cy / 2)) /
            double(rc.bottom - rc.top - szThumb.cy);
    }
}

void Slider::StepValue(double fDelta, int32_t nStepCount)
{
    const int32_t nMin = GetMinValue();
    const int32_t nMax = GetMaxValue();
    const int32_t nStep = (m_nStep > 0) ? m_nStep : 1;
    const int32_t nCount = (nStepCount > 0) ? nStepCount : ((nStepCount < 0) ? -nStepCount : 1);
    double fValue = GetValue() + fDelta * nStep * nCount;
    if (fValue < nMin) {
        fValue = nMin;
    }
    if (fValue > nMax) {
        fValue = nMax;
    }
    SetValue(fValue);
}

bool Slider::HandleKeyDown(uint32_t vkCode, bool bShiftDown)
{
    const bool bHorizontal = IsHorizontal();
    //Arrow keys: left/right for a horizontal slider, up/down for a vertical one;
    //PageUp/PageDown always mean increase/decrease
    //bBigStep means a big step (PageUp/PageDown, or Shift + an arrow key)
    bool bIncrease = false;
    bool bArrowKey = true;
    if ((int32_t)vkCode == (bHorizontal ? kVK_RIGHT : kVK_UP)) {
        bIncrease = true;
    }
    else if ((int32_t)vkCode == (bHorizontal ? kVK_LEFT : kVK_DOWN)) {
        bIncrease = false;
    }
    else if (vkCode == kVK_PRIOR) {
        bIncrease = true;
        bArrowKey = false;
    }
    else if (vkCode == kVK_NEXT) {
        bIncrease = false;
        bArrowKey = false;
    }
    else if (vkCode == kVK_END) {
        SetValue(GetMaxValue());
        return true;
    }
    else if (vkCode == kVK_HOME) {
        SetValue(GetMinValue());
        return true;
    }
    else {
        return false;
    }

    //Shift + an arrow key and PageUp/PageDown are big steps
    const bool bBigStep = !bArrowKey || bShiftDown;
    StepValue(bIncrease ? 1.0 : -1.0, bBigStep ? GetPageStep() : 1);
    return true;
}

void Slider::HandleEvent(const EventArgs& msg)
{
    if (IsDisabledEvents(msg)) {
        //If it is a mouse or keyboard message, and the control is disabled, forward it to the parent control
        Box* pParent = GetParent();
        if (pParent != nullptr) {
            pParent->SendEventMsg(msg);
        }
        else {
            BaseClass::HandleEvent(msg);
        }
        return;
    }
    if (msg.eventType == kEventKeyDown) {
        if (IsEnabled() && IsKeyboardEnabled() &&
            HandleKeyDown(msg.vkCode, (msg.modifierKey & kShift) != 0)) {
            return;
        }
    }
    if (msg.eventType == kEventMouseButtonDown || msg.eventType == kEventMouseDoubleClick) {
        UiPoint newPtMouse(msg.ptMouse);
        newPtMouse.Offset(GetScrollOffsetInScrollBox());
        UiRect rcThumb = GetThumbRect();
        if (rcThumb.ContainsPt(newPtMouse)) {
            SetMouseFocused(true);
            return;
        }
        //Clicking the track: by default the value jumps on mouse down and dragging starts
        //(the thumb keeps following the mouse while the button is held down)
        if (m_bClickToJump && IsEnabled() && IsMouseEnabled()) {
            SetValue(ValueFromMousePt(newPtMouse));
            SetMouseFocused(true);
        }
        return;
    }
    if (msg.eventType == kEventMouseButtonUp) {
        if(IsMouseFocused()) {
            SetMouseFocused(false);
        }
        //Final correction when the mouse capture ends (with click-to-jump disabled the value only
        //changes when the mouse button is released)
        if (!m_bClickToJump) {
            UiPoint ptMouse(msg.ptMouse);
            ptMouse.Offset(GetScrollOffsetInScrollBox());
            SetValue(ValueFromMousePt(ptMouse));
        }
        return;
    }
    if (msg.eventType == kEventMouseWheel) {
        int32_t detaValue = msg.eventData;
        if (detaValue > 0) {
            SetValue(GetValue() + GetChangeStep());
            return;
        }
        else {
            SetValue(GetValue() - GetChangeStep());
            return;
        }
    }
    if (msg.eventType == kEventMouseMove) {
        if (IsMouseFocused()) {
            UiPoint ptMouse(msg.ptMouse);
            ptMouse.Offset(GetScrollOffsetInScrollBox());
            SetValue(ValueFromMousePt(ptMouse));
        }
        return;
    }

    Progress::HandleEvent(msg);
}

void Slider::SetAttribute(const std::string& strName, const std::string& strValue)
{
    if (strName == "step") {
        SetChangeStep(StringUtil::StringToInt32(strValue));
    }
    else if ((strName == "thumb_normal_image") || (strName == "thumbnormalimage")) {
        SetThumbStateImage(kControlStateNormal, strValue);
    }
    else if ((strName == "thumb_hot_image") || (strName == "thumbhotimage")) {
        SetThumbStateImage(kControlStateHot, strValue);
    }
    else if ((strName == "thumb_pushed_image") || (strName == "thumbpushedimage")) {
        SetThumbStateImage(kControlStatePushed, strValue);
    }
    else if ((strName == "thumb_disabled_image") || (strName == "thumbdisabledimage")) {
        SetThumbStateImage(kControlStateDisabled, strValue);
    }
    else if ((strName == "thumb_size") || (strName == "thumbsize")) {
        UiSize szXY;
        AttributeUtil::ParseSizeValue(strValue.c_str(), szXY);
        SetThumbSize(szXY, true);
    }
    else if ((strName == "progress_bar_padding") || (strName == "progressbarpadding")) {
        UiPadding rcPadding;
        AttributeUtil::ParsePaddingValue(strValue.c_str(), rcPadding);
        SetProgressBarPadding(rcPadding, true);
    }
    else if ((strName == "click_to_jump") || (strName == "clicktojump")) {
        SetClickToJump(strValue == "true");
    }
    else if ((strName == "page_step") || (strName == "pagestep")) {
        SetPageStep(StringUtil::StringToInt32(strValue));
    }
    else if ((strName == "show_ticks") || (strName == "showticks")) {
        SetShowTicks(strValue == "true");
    }
    else if ((strName == "tick_count") || (strName == "tickcount")) {
        SetTickCount(StringUtil::StringToInt32(strValue));
    }
    else if ((strName == "tick_length") || (strName == "ticklength")) {
        SetTickLength(StringUtil::StringToInt32(strValue));
    }
    else if ((strName == "tick_color") || (strName == "tickcolor")) {
        SetTickColor(strValue);
    }
    else if ((strName == "tick_pos") || (strName == "tickpos")) {
        if (strValue == "bottom") {
            SetTickPos(SliderTickPos::kBottom);
        }
        else if (strValue == "top") {
            SetTickPos(SliderTickPos::kTop);
        }
        else if (strValue == "right") {
            SetTickPos(SliderTickPos::kRight);
        }
        else if (strValue == "left") {
            SetTickPos(SliderTickPos::kLeft);
        }
    }
    else if ((strName == "show_tick_labels") || (strName == "showticklabels")) {
        SetShowTickLabels(strValue == "true");
    }
    else if ((strName == "tick_label_interval") || (strName == "ticklabelinterval")) {
        SetTickLabelInterval(StringUtil::StringToInt32(strValue));
    }
    else if ((strName == "tick_font") || (strName == "tickfont")) {
        SetTickFontId(strValue);
    }
    else if ((strName == "tick_label_color") || (strName == "ticklabelcolor")) {
        SetTickLabelColor(strValue);
    }
    else {
        Progress::SetAttribute(strName, strValue);
    }
}

void Slider::ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale)
{
    if (!Dpi().CheckDisplayScaleFactor(nNewDpiScale)) {
        return;
    }
    UiSize szThumbSize = GetThumbSize();
    szThumbSize = Dpi().GetScaleSize(szThumbSize, nOldDpiScale);
    SetThumbSize(szThumbSize, false);

    BaseClass::ChangeDpiScale(nOldDpiScale, nNewDpiScale);
}

void Slider::PaintBkColor(IRender* pRender)
{
    const UiRect rcOldPaintRect = GetPaintRect();
    const UiRect rcOldRect = GetRect();

    UiRect rc = GetRect();
    const UiPadding& padding = m_rcProgressBarPadding;
    rc.Deflate(padding);
    SetRect(rc);

    UiRect paintRect = GetPaintRect();
    paintRect.Deflate(padding);
    SetPaintRect(paintRect);

    Control::PaintBkColor(pRender);

    paintRect = GetPaintRect();
    paintRect.Inflate(padding);
    SetPaintRect(paintRect);

    //Restore the original value
    SetRect(rcOldRect);
    SetPaintRect(rcOldPaintRect);
}

void Slider::PaintStateImages(IRender* pRender)
{
    const UiRect rcOldPaintRect = GetPaintRect();
    const UiRect rcOldRect = GetRect();

    UiRect rc = GetRect();
    const UiPadding& padding = m_rcProgressBarPadding;
    rc.Deflate(padding);
    SetRect(rc);

    Progress::PaintStateImages(pRender);

    //Restore the original value
    SetRect(rcOldRect);
    SetPaintRect(rcOldPaintRect);

    UiRect rcThumb = GetThumbRect();
    rcThumb.left -= GetRect().left;
    rcThumb.top -= GetRect().top;
    rcThumb.right -= GetRect().left;
    rcThumb.bottom -= GetRect().top;

    //Paint the thumb according to its state: pushed first, then hot, and finally normal.
    //Do not return early here, otherwise the tick painting at the end would be skipped (the tick
    //marks would disappear while the thumb is hot or pushed)
    bool bThumbPainted = false;
    if (IsMouseFocused()) {
        m_sImageModify.clear();
        m_sImageModify = StringUtil::Printf("destscale='false' dest='%d,%d,%d,%d'", rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom);
        bThumbPainted = PaintImage(pRender, m_thumbStateImage.GetStateImage(kControlStatePushed), m_sImageModify.c_str());
    }
    else if (GetState() == kControlStateHot) {
        m_sImageModify.clear();
        m_sImageModify = StringUtil::Printf("destscale='false' dest='%d,%d,%d,%d'", rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom);
        bThumbPainted = PaintImage(pRender, m_thumbStateImage.GetStateImage(kControlStateHot), m_sImageModify.c_str());
    }
    if (!bThumbPainted) {
        m_sImageModify.clear();
        m_sImageModify = StringUtil::Printf("destscale='false' dest='%d,%d,%d,%d'", rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom);
        PaintImage(pRender, m_thumbStateImage.GetStateImage(kControlStateNormal), m_sImageModify.c_str());
    }

    //Tick marks and tick value texts (drawn outside the track; they do not affect the painting of
    //the thumb and of the progress bar itself)
    PaintTicks(pRender);
}

void Slider::PaintTicks(IRender* pRender)
{
    if ((pRender == nullptr) || !m_bShowTicks || (m_nTickCount < 2)) {
        return;
    }

    //The track area is the control area with progress_bar_padding removed; the ticks are aligned with it
    UiRect rcTrack = GetRect();
    rcTrack.Deflate(m_rcProgressBarPadding);
    if (rcTrack.IsEmpty()) {
        return;
    }

    const UiSize szThumb = GetThumbSize();
    const bool bHorizontal = IsHorizontal();
    //Track center line (vertically centered for a horizontal slider, horizontally centered for a
    //vertical one)
    int32_t nTrackLength = 0;
    int32_t nThumbStart = 0;      //Start of the thumb's movable range
    int32_t nThumbSpan = 0;       //Length of the thumb's movable range
    if (bHorizontal) {
        nThumbStart = rcTrack.left + szThumb.cx / 2;
        nThumbSpan = rcTrack.right - rcTrack.left - szThumb.cx;
        nTrackLength = rcTrack.Width();
    }
    else {
        nThumbStart = rcTrack.bottom - szThumb.cy / 2;
        nThumbSpan = rcTrack.bottom - rcTrack.top - szThumb.cy;
        nTrackLength = rcTrack.Height();
    }
    if ((nThumbSpan <= 0) || (nTrackLength <= 0)) {
        return;
    }

    //Tick mark color: text_muted by default (the secondary text color, subdued so that it does not
    //compete with the main content)
    UiColor tickColor = GetUiColor(m_strTickColor.empty() ? "text_muted" : m_strTickColor);
    if (tickColor.IsEmpty()) {
        tickColor = GetUiColor("color_gray");
    }
    if (tickColor.IsEmpty()) {
        tickColor = UiColor(UiColors::Gray);
    }
    if (!IsEnabled()) {
        //Disabled state: fade the tick marks
        tickColor = UiColor(tickColor.GetAlpha() / 2, tickColor.GetRed(), tickColor.GetGreen(), tickColor.GetBlue());
    }

    const int32_t nTickLen = (m_nTickLength > 0) ? Dpi().GetScaleInt(m_nTickLength) : 0;
    if (nTickLen == 0) {
        return;
    }
    const int32_t nGap = Dpi().GetScaleInt(2);
    const int32_t nLabelHeight = Dpi().GetScaleInt(14);
    const bool bShowLabels = m_bShowTickLabels && !m_strTickFontId.empty();
    const int32_t nCount = m_nTickCount;

    const UiRect& rcControl = GetRect();

    //Center distance between adjacent ticks: a label half-width must not exceed half of it,
    //otherwise adjacent labels would overlap
    const int32_t nTickStep = (nCount > 1) ? (nThumbSpan / (nCount - 1)) : nThumbSpan;
    int32_t nLabelHalfSpan = Dpi().GetScaleInt(28);   //Label half-width (horizontal slider)
    int32_t nLabelLead = nLabelHalfSpan;              //Label area width (vertical slider)
    if (nTickStep > 0) {
        //Narrow it according to the actual spacing: keep a 2px gap and stay within half a tick step
        nLabelHalfSpan = std::max(Dpi().GetScaleInt(12), std::min(nLabelHalfSpan, nTickStep / 2 - Dpi().GetScaleInt(2)));
        nLabelLead = nLabelHalfSpan * 2;
    }
    //Vertical slider: the label area width must not exceed the control width either
    const int32_t nMaxLead = std::max(nLabelHeight, rcTrack.Width() - Dpi().GetScaleInt(8));
    if (nLabelLead > nMaxLead) {
        nLabelLead = nMaxLead;
    }

    //Main ticks (the labeled ones) are slightly longer and minor ticks slightly shorter, which gives
    //a visual hierarchy
    const int32_t nMainTickLen = nTickLen;
    const int32_t nSubTickLen = std::max(Dpi().GetScaleInt(2), nTickLen / 2);

    for (int32_t i = 0; i < nCount; ++i) {
        //Tick positions are computed over the thumb's movable range so that they align with the thumb center
        const int32_t nPos = nThumbStart + (int32_t)(double(nThumbSpan) * (double(i) / double(nCount - 1)) + 0.5);
        const bool bLabel = bShowLabels && ((i % m_nTickLabelInterval) == 0);
        const int32_t nThisTickLen = bLabel ? nMainTickLen : nSubTickLen;
        UiRect rcLabel;
        //The first and the last label are aligned to the edges (without changing the label box width)
        //so that they do not intrude into the space of a neighboring label
        uint32_t uAlign = TEXT_HCENTER | TEXT_VCENTER;
        if (bHorizontal) {
            const int32_t nThumbTop = (rcTrack.bottom + rcTrack.top - szThumb.cy) / 2;
            const bool bBelow = (m_tickPos == SliderTickPos::kBottom);
            const int32_t nThumbEdge = bBelow ? (nThumbTop + szThumb.cy) : nThumbTop;
            const int32_t nTickStart = bBelow ? (nThumbEdge + nGap) : (nThumbEdge - nGap - nThisTickLen);
            pRender->DrawLine(UiPointF(static_cast<float>(nPos), static_cast<float>(nTickStart)),
                              UiPointF(static_cast<float>(nPos), static_cast<float>(nTickStart + nThisTickLen)),
                              tickColor, 1.0f);
            if (bLabel) {
                //Vertical position of the label: outside the tick mark and fully inside the control
                int32_t nLabelTop = bBelow ? (nTickStart + nTickLen) : (nTickStart - nLabelHeight);
                nLabelTop = std::clamp(nLabelTop, rcControl.top, std::max(rcControl.top, rcControl.bottom - nLabelHeight));
                const int32_t nLabelH = nLabelHalfSpan * 2;
                if (i == 0) {
                    //First label: left aligned text, with the box extending to the right of the tick
                    uAlign = TEXT_LEFT | TEXT_VCENTER;
                    rcLabel = UiRect(nPos, nLabelTop, nPos + nLabelH, nLabelTop + nLabelHeight);
                }
                else if (i == nCount - 1) {
                    //Last label: right aligned text, with the box extending to the left of the tick
                    uAlign = TEXT_RIGHT | TEXT_VCENTER;
                    rcLabel = UiRect(nPos - nLabelH, nLabelTop, nPos, nLabelTop + nLabelHeight);
                }
                else {
                    rcLabel = UiRect(nPos - nLabelHalfSpan, nLabelTop, nPos + nLabelHalfSpan, nLabelTop + nLabelHeight);
                }
            }
        }
        else {
            const int32_t nThumbLeft = (rcTrack.right + rcTrack.left - szThumb.cx) / 2;
            const bool bRight = (m_tickPos == SliderTickPos::kRight);
            const int32_t nThumbEdge = bRight ? (nThumbLeft + szThumb.cx) : nThumbLeft;
            const int32_t nTickStart = bRight ? (nThumbEdge + nGap) : (nThumbEdge - nGap - nThisTickLen);
            pRender->DrawLine(UiPointF(static_cast<float>(nTickStart), static_cast<float>(nPos)),
                              UiPointF(static_cast<float>(nTickStart + nThisTickLen), static_cast<float>(nPos)),
                              tickColor, 1.0f);
            if (bLabel) {
                int32_t nLabelLeft = bRight ? (nTickStart + nTickLen) : (nTickStart - nTickLen - nLabelLead);
                nLabelLeft = std::clamp(nLabelLeft, rcControl.left, std::max(rcControl.left, rcControl.right - nLabelLead));
                const int32_t nLabelW = nLabelLead;
                if (i == 0) {
                    //First label: top aligned text
                    uAlign = TEXT_HCENTER | TEXT_TOP;
                    rcLabel = UiRect(nLabelLeft, nPos, nLabelLeft + nLabelW, nPos + nLabelHeight);
                }
                else if (i == nCount - 1) {
                    //Last label: bottom aligned text
                    uAlign = TEXT_HCENTER | TEXT_BOTTOM;
                    rcLabel = UiRect(nLabelLeft, nPos - nLabelHeight, nLabelLeft + nLabelW, nPos);
                }
                else {
                    rcLabel = UiRect(nLabelLeft, nPos - nLabelHeight / 2, nLabelLeft + nLabelW, nPos + nLabelHeight / 2);
                }
            }
        }
        if (bLabel) {
            DrawTickText(pRender, GetTickValue(i), i, rcLabel, uAlign);
        }
    }
}

void Slider::DrawTickText(IRender* pRender, double fValue, int32_t nIndex, const UiRect& rcLabel, uint32_t uAlign)
{
    if (pRender == nullptr) {
        return;
    }
    std::string strText;
    if (m_tickLabelCallback) {
        strText = m_tickLabelCallback(fValue, nIndex);
    }
    if (strText.empty()) {
        const double fFrac = fValue - floor(fValue);
        if (fabs(fFrac) < 0.0001) {
            //An integral tick value: avoid displaying it as 20.00 because of a floating point error
            strText = StringUtil::Printf("%I64d", (int64_t)llround(fValue));
        }
        else {
            strText = StringUtil::Printf("%.2f", fValue);
        }
    }
    if (strText.empty()) {
        return;
    }

    UiColor textColor = GetUiColor(m_strTickLabelColor.empty() ? "text_muted" : m_strTickLabelColor);
    if (textColor.IsEmpty()) {
        textColor = UiColor(0xFF000000);
    }
    DrawStringParam param;
    param.textRect = rcLabel;
    param.dwTextColor = textColor;
    param.pFont = GetIFontById(m_strTickFontId);
    param.uFormat = TEXT_SINGLELINE | uAlign;
    pRender->DrawString(strText, param);
}

void Slider::ClearImageCache()
{
    BaseClass::ClearImageCache();
    m_thumbStateImage.ClearImageCache();
}

int Slider::GetChangeStep()
{
    return m_nStep;
}

void Slider::SetChangeStep(int step)
{
    m_nStep = step;
}

void Slider::SetThumbSize(UiSize szXY, bool bNeedDpiScale)
{
    if (bNeedDpiScale) {
        Dpi().ScaleSize(szXY);
    }    
    m_szThumb = szXY;
}

UiSize Slider::GetThumbSize() const
{
    UiSize szThumb = m_szThumb;
    if (szThumb.IsEmpty()) {
        szThumb = {10, 10};
        Dpi().ScaleSize(szThumb);
    }
    return szThumb;
}

UiRect Slider::GetThumbRect() const
{
    int32_t nMax = GetMaxValue();
    int32_t nMin = GetMinValue();
    double fValue = GetValue();

    if (nMax <= nMin) {
        nMax = nMin + 1;
    }
    if (fValue > nMax) {
        fValue = nMax;
    }
    if (fValue < nMin) {
        fValue = nMin;
    }

    UiSize szThumb = GetThumbSize();
    if(IsHorizontal()) {
        int32_t left = int32_t(GetRect().left + (GetRect().right - GetRect().left - szThumb.cx) * (fValue - nMin) / (nMax - nMin));
        int32_t top = (GetRect().bottom + GetRect().top - szThumb.cy) / 2;
        return UiRect(left, top, left + szThumb.cx, top + szThumb.cy);
    }
    else {
        int32_t left = (GetRect().right + GetRect().left - szThumb.cx) / 2;
        int32_t top = int32_t(GetRect().bottom - szThumb.cy - (GetRect().bottom - GetRect().top - szThumb.cy) * (fValue - nMin) / (nMax - nMin));
        return UiRect(left, top, left + szThumb.cx, top + szThumb.cy);
    }
}

std::string Slider::GetThumbStateImage(ControlStateType stateType) const
{
    return m_thumbStateImage.GetImageString(stateType);
}

void Slider::SetThumbStateImage(ControlStateType stateType, const std::string& pStrImage)
{
    m_thumbStateImage.SetImageString(stateType, pStrImage, Dpi());
    Invalidate();
}

const UiPadding& Slider::GetProgressBarPadding() const
{
    return m_rcProgressBarPadding;
}

void Slider::SetProgressBarPadding(UiPadding padding, bool bNeedDpiScale)
{
    if (bNeedDpiScale) {
        Dpi().ScalePadding(padding);
    }
    if (!m_rcProgressBarPadding.Equals(padding)) {
        m_rcProgressBarPadding = padding;
        RelayoutOrRedraw();
    }    
}

}
