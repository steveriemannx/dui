#include "dui/Control/RangeSlider.h"
#include <algorithm>
#include <cmath>
#include "dui/Utils/AttributeUtil.h"
#include "dui/Core/GlobalManager.h"

namespace ui
{

RangeSlider::RangeSlider(Window* pWindow) :
    Progress(pWindow),
    m_fLowValue(0),
    m_fMinRange(0),
    m_szThumb(0, 0),
    m_thumbStateImage(),
    m_highThumbStateImage(),
    m_strRangeColor(),
    m_nRangeRadius(0),
    m_bClickToJump(true),
    m_bMuteRangeEvent(false),
    m_bHighValueSet(false),
    m_dragThumb(kThumbNone),
    m_hoverThumb(kThumbNone),
    m_sImageModify()
{
    SetTextStyle(TEXT_SINGLELINE | TEXT_HCENTER, false);
}

std::string RangeSlider::GetType() const { return DUI_CTR_RANGE_SLIDER; }

void RangeSlider::SetMinRange(double fMinRange)
{
    if (fMinRange < 0) {
        fMinRange = 0;
    }
    if (m_fMinRange == fMinRange) {
        return;
    }
    m_fMinRange = fMinRange;

    //After the minimum distance grows the low value may be out of range and has to be clamped again
    const double fOldLow = m_fLowValue;
    ClampLowValue();
    Invalidate();
    if (m_fLowValue != fOldLow) {
        EmitRangeChanged();
    }
}

void RangeSlider::SetLowValue(double fValue)
{
    const double fOldLow = m_fLowValue;

    const int32_t nMin = GetMinValue();
    const int32_t nMax = GetMaxValue();
    if (fValue < (double)nMin) {
        fValue = (double)nMin;
    }
    if (fValue > (double)nMax) {
        fValue = (double)nMax;
    }
    //The low value must not exceed the high value (the minimum distance is preserved)
    //While the high value has not been set yet, the low value is only constrained by [min, max], so the initialization is not blocked by the default high value
    double fLowMax = (double)nMax;
    if (m_bHighValueSet) {
        fLowMax = GetHighValue() - m_fMinRange;
        if (fLowMax < (double)nMin) {
            fLowMax = (double)nMin;
        }
    }
    if (fValue > fLowMax) {
        fValue = fLowMax;
    }

    if (fValue == fOldLow) {
        return;
    }
    m_fLowValue = fValue;
    Invalidate();
    EmitRangeChanged();
}

void RangeSlider::SetLowValueNoEvent(double fValue)
{
    const bool bOldMute = m_bMuteRangeEvent;
    m_bMuteRangeEvent = true;
    SetLowValue(fValue);
    m_bMuteRangeEvent = bOldMute;
}

void RangeSlider::SetHighValue(double fValue)
{
    const double fOldHigh = GetHighValue();

    const int32_t nMin = GetMinValue();
    const int32_t nMax = GetMaxValue();
    double fHighMin = GetLowValue() + m_fMinRange;
    if (fHighMin < (double)nMin) {
        fHighMin = (double)nMin;
    }
    if (fValue < fHighMin) {
        fValue = fHighMin;
    }
    if (fValue > (double)nMax) {
        fValue = (double)nMax;
    }

    m_bHighValueSet = true;
    if (fValue == fOldHigh) {
        return;
    }
    //Use the silent setter of the base class to suppress its own kEventValueChanged; this control dispatches the range event itself
    Progress::SetValueNoEvent(fValue);
    Invalidate();
    EmitRangeChanged();
}

void RangeSlider::SetHighValueNoEvent(double fValue)
{
    const bool bOldMute = m_bMuteRangeEvent;
    m_bMuteRangeEvent = true;
    SetHighValue(fValue);
    m_bMuteRangeEvent = bOldMute;
}

void RangeSlider::SetRange(double fLowValue, double fHighValue)
{
    const double fOldLow = GetLowValue();
    const double fOldHigh = GetHighValue();

    //Set the low value first and then the high value (the low value is ready then and can constrain the high value correctly)
    const bool bOldMute = m_bMuteRangeEvent;
    m_bMuteRangeEvent = true;
    SetLowValue(fLowValue);
    SetHighValue(fHighValue);
    ClampLowValue();
    m_bMuteRangeEvent = bOldMute;

    if ((GetLowValue() != fOldLow) || (GetHighValue() != fOldHigh)) {
        EmitRangeChanged();
    }
}

void RangeSlider::SetRangeNoEvent(double fLowValue, double fHighValue)
{
    const bool bOldMute = m_bMuteRangeEvent;
    m_bMuteRangeEvent = true;
    SetRange(fLowValue, fHighValue);
    m_bMuteRangeEvent = bOldMute;
}

void RangeSlider::ClampLowValue()
{
    const double fMin = (double)GetMinValue();
    const double fMax = (double)GetMaxValue();

    double fLowMax = fMax;
    if (m_bHighValueSet) {
        fLowMax = GetHighValue() - m_fMinRange;
        if (fLowMax < fMin) {
            fLowMax = fMin;
        }
    }

    double fLow = m_fLowValue;
    if (fLow < fMin) {
        fLow = fMin;
    }
    if (fLow > fLowMax) {
        fLow = fLowMax;
    }
    if (fLow > fMax) {
        fLow = fMax;
    }
    m_fLowValue = fLow;
}

void RangeSlider::EmitRangeChanged()
{
    if (m_bMuteRangeEvent) {
        return;
    }
    //The range value is carried through pEventData (a temporary object on the stack, valid only while this synchronous dispatch runs)
    RangeValue range;
    range.lowValue = GetLowValue();
    range.highValue = GetHighValue();
    SendEvent(kEventValueChanged, (WPARAM)1, 0, static_cast<void*>(&range));
}

void RangeSlider::SetThumbSize(UiSize szXY, bool bNeedDpiScale)
{
    if (bNeedDpiScale) {
        Dpi().ScaleSize(szXY);
    }
    m_szThumb = szXY;
}

UiSize RangeSlider::GetThumbSize() const
{
    UiSize szThumb = m_szThumb;
    if (szThumb.IsEmpty()) {
        szThumb = { 10, 10 };
        Dpi().ScaleSize(szThumb);
    }
    return szThumb;
}

std::string RangeSlider::GetThumbStateImage(ControlStateType stateType) const
{
    return m_thumbStateImage.GetImageString(stateType);
}

void RangeSlider::SetThumbStateImage(ControlStateType stateType, const std::string& strImage)
{
    m_thumbStateImage.SetImageString(stateType, strImage, Dpi());
    Invalidate();
}

std::string RangeSlider::GetHighThumbStateImage(ControlStateType stateType) const
{
    return m_highThumbStateImage.GetImageString(stateType);
}

void RangeSlider::SetHighThumbStateImage(ControlStateType stateType, const std::string& strImage)
{
    m_highThumbStateImage.SetImageString(stateType, strImage, Dpi());
    Invalidate();
}

void RangeSlider::SetRangeColor(const std::string& strColor)
{
    if (m_strRangeColor != strColor) {
        m_strRangeColor = strColor;
        Invalidate();
    }
}

void RangeSlider::SetRangeRadius(int32_t nRadius)
{
    if (nRadius < 0) {
        nRadius = 0;
    }
    if (m_nRangeRadius != nRadius) {
        m_nRangeRadius = nRadius;
        Invalidate();
    }
}

void RangeSlider::SetClickToJump(bool bEnable)
{
    m_bClickToJump = bEnable;
}

UiRect RangeSlider::ThumbRectFromValue(double fValue) const
{
    int32_t nMax = GetMaxValue();
    int32_t nMin = GetMinValue();
    if (nMax <= nMin) {
        nMax = nMin + 1;
    }
    if (fValue > (double)nMax) {
        fValue = (double)nMax;
    }
    if (fValue < (double)nMin) {
        fValue = (double)nMin;
    }

    const UiSize szThumb = GetThumbSize();
    const UiRect& rc = GetRect();
    if (IsHorizontal()) {
        int32_t left = (int32_t)(rc.left + (double)(rc.right - rc.left - szThumb.cx) * (fValue - nMin) / (nMax - nMin) + 0.5);
        int32_t top = (rc.bottom + rc.top - szThumb.cy) / 2;
        return UiRect(left, top, left + szThumb.cx, top + szThumb.cy);
    }
    else {
        int32_t left = (rc.right + rc.left - szThumb.cx) / 2;
        int32_t top = (int32_t)(rc.bottom - szThumb.cy - (double)(rc.bottom - rc.top - szThumb.cy) * (fValue - nMin) / (nMax - nMin) + 0.5);
        return UiRect(left, top, left + szThumb.cx, top + szThumb.cy);
    }
}

UiRect RangeSlider::CalcRangeRect(double fValue) const
{
    int32_t nMax = GetMaxValue();
    int32_t nMin = GetMinValue();
    if (nMax <= nMin) {
        nMax = nMin + 1;
    }
    if (fValue > (double)nMax) {
        fValue = (double)nMax;
    }
    if (fValue < (double)nMin) {
        fValue = (double)nMin;
    }
    double fRatio = (fValue - nMin) / (double)(nMax - nMin);
    fRatio = std::clamp(fRatio, 0.0, 1.0);

    UiRect rc;
    if (IsHorizontal()) {
        rc.right = (int32_t)std::ceil(fRatio * (GetRect().right - GetRect().left));
        rc.bottom = GetRect().bottom - GetRect().top;
    }
    else {
        rc.top = (int32_t)std::ceil((1.0 - fRatio) * (GetRect().bottom - GetRect().top));
        rc.bottom = GetRect().bottom - GetRect().top;
        rc.right = GetRect().right - GetRect().left;
    }
    return rc;
}

double RangeSlider::ValueFromMousePt(const UiPoint& ptMouse) const
{
    const int32_t nMin = GetMinValue();
    const int32_t nMax = GetMaxValue();
    const UiSize szThumb = GetThumbSize();
    const UiRect& rc = GetRect();
    if (IsHorizontal()) {
        if (ptMouse.x >= rc.right - szThumb.cx / 2) {
            return (double)nMax;
        }
        if (ptMouse.x <= rc.left + szThumb.cx / 2) {
            return (double)nMin;
        }
        return nMin + (double)((nMax - nMin) * (ptMouse.x - rc.left - szThumb.cx / 2)) /
            (double)(rc.right - rc.left - szThumb.cx);
    }
    else {
        if (ptMouse.y >= rc.bottom - szThumb.cy / 2) {
            return (double)nMin;
        }
        if (ptMouse.y <= rc.top + szThumb.cy / 2) {
            return (double)nMax;
        }
        return nMin + (double)((nMax - nMin) * (rc.bottom - ptMouse.y - szThumb.cy / 2)) /
            (double)(rc.bottom - rc.top - szThumb.cy);
    }
}

void RangeSlider::HandleEvent(const EventArgs& msg)
{
    if (IsDisabledEvents(msg)) {
        //For mouse/keyboard messages, a disabled control forwards them to the parent control
        Box* pParent = GetParent();
        if (pParent != nullptr) {
            pParent->SendEventMsg(msg);
        }
        else {
            BaseClass::HandleEvent(msg);
        }
        return;
    }

    if ((msg.eventType == kEventMouseButtonDown) || (msg.eventType == kEventMouseDoubleClick)) {
        UiPoint newPtMouse(msg.ptMouse);
        newPtMouse.Offset(GetScrollOffsetInScrollBox());
        if (GetLowThumbRect().ContainsPt(newPtMouse)) {
            m_dragThumb = kThumbLow;
            SetMouseFocused(true);
            return;
        }
        if (GetHighThumbRect().ContainsPt(newPtMouse)) {
            m_dragThumb = kThumbHigh;
            SetMouseFocused(true);
            return;
        }
        //Click on the track: by default the value jumps immediately and a drag starts (moving whichever thumb is closer to the click position)
        if (m_bClickToJump && IsEnabled() && IsMouseEnabled()) {
            const double fValue = ValueFromMousePt(newPtMouse);
            const double fDistLow = std::fabs(fValue - GetLowValue());
            const double fDistHigh = std::fabs(fValue - GetHighValue());
            if (fDistLow <= fDistHigh) {
                m_dragThumb = kThumbLow;
                SetLowValue(fValue);
            }
            else {
                m_dragThumb = kThumbHigh;
                SetHighValue(fValue);
            }
            SetMouseFocused(true);
        }
        return;
    }

    if (msg.eventType == kEventMouseButtonUp) {
        if (IsMouseFocused()) {
            //When click-to-jump is disabled, the value jumps only after the mouse button is released
            if (!m_bClickToJump) {
                UiPoint ptMouse(msg.ptMouse);
                ptMouse.Offset(GetScrollOffsetInScrollBox());
                const double fValue = ValueFromMousePt(ptMouse);
                if (m_dragThumb == kThumbLow) {
                    SetLowValue(fValue);
                }
                else if (m_dragThumb == kThumbHigh) {
                    SetHighValue(fValue);
                }
            }
            m_dragThumb = kThumbNone;
            SetMouseFocused(false);
        }
        return;
    }

    if (msg.eventType == kEventMouseMove) {
        UiPoint ptMouse(msg.ptMouse);
        ptMouse.Offset(GetScrollOffsetInScrollBox());

        //Update the thumb the mouse hovers over (used to paint the hovered state image)
        const ThumbPart oldHover = m_hoverThumb;
        if (GetLowThumbRect().ContainsPt(ptMouse)) {
            m_hoverThumb = kThumbLow;
        }
        else if (GetHighThumbRect().ContainsPt(ptMouse)) {
            m_hoverThumb = kThumbHigh;
        }
        else {
            m_hoverThumb = kThumbNone;
        }
        if (oldHover != m_hoverThumb) {
            Invalidate();
        }

        if (IsMouseFocused()) {
            const double fValue = ValueFromMousePt(ptMouse);
            if (m_dragThumb == kThumbLow) {
                SetLowValue(fValue);
            }
            else if (m_dragThumb == kThumbHigh) {
                SetHighValue(fValue);
            }
            return;
        }
        return;
    }

    if (msg.eventType == kEventMouseLeave) {
        if (m_hoverThumb != kThumbNone) {
            m_hoverThumb = kThumbNone;
            Invalidate();
        }
    }

    Progress::HandleEvent(msg);
}

void RangeSlider::SetAttribute(const std::string& strName, const std::string& strValue2)
{
    //dui does not support variable expansion in attribute values, so the value is used as is
    const std::string& strValue = strValue2;
    if ((strName == "low_value") || (strName == "lowvalue")) {
        SetLowValue(StringUtil::StringToDouble(strValue));
    }
    else if ((strName == "high_value") || (strName == "highvalue")) {
        SetHighValue(StringUtil::StringToDouble(strValue));
    }
    else if (strName == "value") {
        //High value (same semantics as the single-thumb slider controls)
        SetHighValue(StringUtil::StringToDouble(strValue));
    }
    else if ((strName == "min_range") || (strName == "minrange")) {
        SetMinRange(StringUtil::StringToDouble(strValue));
    }
    else if ((strName == "range_color") || (strName == "rangecolor")) {
        SetRangeColor(strValue);
    }
    else if ((strName == "range_radius") || (strName == "rangeradius")) {
        SetRangeRadius(StringUtil::StringToInt32(strValue));
    }
    else if ((strName == "click_to_jump") || (strName == "clicktojump")) {
        //dui has no StringUtil::IsValueTrue; booleans are compared against "true", as in the other dui controls
        SetClickToJump(strValue == "true");
    }
    else if ((strName == "thumb_size") || (strName == "thumbsize")) {
        UiSize szXY;
        AttributeUtil::ParseSizeValue(strValue.c_str(), szXY);
        SetThumbSize(szXY, true);
    }
    else if ((strName == "thumb_normal_image") || (strName == "thumbnormalimage")) {
        SetThumbStateImage(kControlStateNormal, strValue);
    }
    else if ((strName == "thumb_hovered_image") || (strName == "thumb_hot_image") || (strName == "thumbhotimage")) {
        SetThumbStateImage(kControlStateHot, strValue);
    }
    else if ((strName == "thumb_pressed_image") || (strName == "thumb_pushed_image") || (strName == "thumbpushedimage")) {
        SetThumbStateImage(kControlStatePushed, strValue);
    }
    else if ((strName == "thumb_disabled_image") || (strName == "thumbdisabledimage")) {
        SetThumbStateImage(kControlStateDisabled, strValue);
    }
    else if ((strName == "high_thumb_normal_image") || (strName == "highthumbnormalimage")) {
        SetHighThumbStateImage(kControlStateNormal, strValue);
    }
    else if ((strName == "high_thumb_hovered_image") || (strName == "high_thumb_hot_image") || (strName == "highthumbhotimage")) {
        SetHighThumbStateImage(kControlStateHot, strValue);
    }
    else if ((strName == "high_thumb_pressed_image") || (strName == "high_thumb_pushed_image") || (strName == "highthumbpushedimage")) {
        SetHighThumbStateImage(kControlStatePushed, strValue);
    }
    else if ((strName == "high_thumb_disabled_image") || (strName == "highthumbdisabledimage")) {
        SetHighThumbStateImage(kControlStateDisabled, strValue);
    }
    else if ((strName == "min") || (strName == "max")) {
        BaseClass::SetAttribute(strName, strValue);
        //After a boundary change the low value may be out of range and has to be clamped again
        ClampLowValue();
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

void RangeSlider::ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale)
{
    if (!Dpi().CheckDisplayScaleFactor(nNewDpiScale)) {
        return;
    }
    UiSize szThumbSize = GetThumbSize();
    szThumbSize = Dpi().GetScaleSize(szThumbSize, nOldDpiScale);
    SetThumbSize(szThumbSize, false);

    BaseClass::ChangeDpiScale(nOldDpiScale, nNewDpiScale);
}

void RangeSlider::PaintStateImages(IRender* pRender)
{
    ASSERT(pRender != nullptr);
    if (pRender == nullptr) {
        return;
    }

    //Highlighted selected interval (falls back to progress_color when no color is configured)
    const std::string strRangeColor = m_strRangeColor.empty() ? GetProgressColor() : m_strRangeColor;
    const UiColor rangeColor = GetUiColor(strRangeColor);
    if (rangeColor.GetARGB() != 0) {
        const UiRect rcLow = CalcRangeRect(GetLowValue());
        const UiRect rcHigh = CalcRangeRect(GetHighValue());
        const UiRect& rc = GetRect();
        UiRect rcBand;
        if (IsHorizontal()) {
            rcBand.left = rc.left + rcLow.right;
            rcBand.right = rc.left + rcHigh.right;
            rcBand.top = rc.top;
            rcBand.bottom = rc.bottom;
        }
        else {
            rcBand.left = rc.left;
            rcBand.right = rc.right;
            rcBand.top = rc.top + rcHigh.top;
            rcBand.bottom = rc.top + rcLow.top;
        }
        if (!rcBand.IsEmpty()) {
            int32_t nRadius = Dpi().GetScaleInt(m_nRangeRadius);
            if (nRadius > 0) {
                const int32_t nMaxRadius = std::min(rcBand.Width(), rcBand.Height()) / 2;
                if (nRadius > nMaxRadius) {
                    nRadius = nMaxRadius;
                }
            }
            if (nRadius > 0) {
                pRender->FillRoundRect(UiRectF::MakeFromRect(rcBand), (float)nRadius, (float)nRadius, rangeColor, GetAlpha());
            }
            else {
                pRender->FillRect(UiRectF::MakeFromRect(rcBand), rangeColor, GetAlpha());
            }
        }
    }

    //The two thumbs
    PaintThumb(pRender, kThumbLow);
    PaintThumb(pRender, kThumbHigh);
}

void RangeSlider::PaintThumb(IRender* pRender, ThumbPart part)
{
    UiRect rcThumb = (part == kThumbLow) ? GetLowThumbRect() : GetHighThumbRect();
    rcThumb.left -= GetRect().left;
    rcThumb.top -= GetRect().top;
    rcThumb.right -= GetRect().left;
    rcThumb.bottom -= GetRect().top;

    //Choose the image set: use the images configured for the high thumb when present, otherwise fall back to the low thumb images
    const StateImage* pStateImage = &m_thumbStateImage;
    if ((part == kThumbHigh) && m_highThumbStateImage.HasImage()) {
        pStateImage = &m_highThumbStateImage;
    }

    //State priority: disabled > dragging (pressed) > hovered > normal
    ControlStateType stateType = kControlStateNormal;
    if (!IsEnabled()) {
        stateType = kControlStateDisabled;
    }
    else if ((m_dragThumb == part) && IsMouseFocused()) {
        stateType = kControlStatePushed;
    }
    else if ((GetState() == kControlStateHot) && (m_hoverThumb == part)) {
        stateType = kControlStateHot;
    }

    Image* pThumbImage = pStateImage->GetStateImage(stateType);
    if ((pThumbImage == nullptr) && (stateType != kControlStateNormal)) {
        //Fall back to the normal state image
        pThumbImage = pStateImage->GetStateImage(kControlStateNormal);
    }
    if (pThumbImage == nullptr) {
        return;
    }

    m_sImageModify.clear();
    m_sImageModify = StringUtil::Printf("destscale='false' dest='%d,%d,%d,%d'", rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom);
    PaintImage(pRender, pThumbImage, m_sImageModify.c_str());
}

void RangeSlider::ClearImageCache()
{
    BaseClass::ClearImageCache();
    m_thumbStateImage.ClearImageCache();
    m_highThumbStateImage.ClearImageCache();
}

} // namespace ui
