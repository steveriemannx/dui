#include "dui/Control/Progress.h"
#include "dui/Core/GlobalManager.h"
#include <algorithm>
#include <cmath>

namespace ui
{

Progress::Progress(Window* pWindow) :
    LabelTemplate<Control>(pWindow),
    m_bHorizontal(true),
    m_bStretchForeImage(true),
    m_nMaxValue(100),
    m_nMinValue(0),
    m_fCurrentValue(0),
    m_sProgressColor(),
    m_pProgressImage(nullptr),
    m_sProgressImageModify(),
    m_bMarquee(false),
    m_nMarqueeWidth(0),
    m_nMarqueeStep(0),
    m_nMarqueeElapsed(50), // for 1s 25fps,will use 20fps default
    m_nMarqueePos(0),
    m_fDisplayValue(0),
    m_bAnimationEnabled(false),
    m_nAnimationDuration(300),
    m_easingAnimation(EaseOutQuad),
    m_strAnimationEasing("EaseOutQuad"),
    m_pAnimPlayer(nullptr),
    m_bShowPercent(false),
    m_bDisableValueChangeEvent(false),
    m_nProgressRadius(0),
    m_fBufferValue(-1),
    m_strBufferColor(),
    m_textFormatCallback(),
    m_bReverse(false)
{
    SetTextStyle(TEXT_SINGLELINE | TEXT_HCENTER | TEXT_VCENTER, false);
    SetFixedHeight(UiFixedInt(12), true, true);
    SetMarqueeWidth(10, true);
    SetMarqueeStep(4, true);
}

Progress::~Progress()
{
    if (m_pProgressImage != nullptr) {
        delete m_pProgressImage;
        m_pProgressImage = nullptr;
    }
}

std::string Progress::GetType() const { return DUI_CTR_PROGRESS; }

void Progress::SetAttribute(const std::string& srName, const std::string& strValue)
{
    if ((srName == "horizontal") || (srName == "hor")) {
        SetHorizontal(strValue == "true");
    }
    else if (srName == "min") {
        SetMinValue(StringUtil::StringToInt32(strValue));
    }
    else if (srName == "max") {
        SetMaxValue(StringUtil::StringToInt32(strValue));
    }
    else if (srName == "value") {
        SetValue(StringUtil::StringToDouble(strValue));
    }
    else if ((srName == "progress_image") || (srName == "progressimage")) {
        SetProgressImage(strValue);
    }
    else if ((srName == "stretch_fore_image") || (srName == "is_stretch_fore") || (srName == "isstretchfore")) {
        SetStretchForeImage(strValue == "true");
    }
    else if ((srName == "progress_color") || (srName == "progresscolor")) {
        SetProgressColor(strValue);
    }
    else if ((srName == "progress_radius") || (srName == "progressradius")) {
        SetProgressRadius(StringUtil::StringToInt32(strValue));
    }
    else if ((srName == "buffer_value") || (srName == "buffervalue")) {
        SetBufferValue(StringUtil::StringToDouble(strValue));
    }
    else if ((srName == "buffer_color") || (srName == "buffercolor")) {
        SetBufferColor(strValue);
    }
    else if ((srName == "show_percent") || (srName == "showpercent")) {
        SetShowPercent(strValue == "true");
    }
    else if ((srName == "animation_enabled") || (srName == "animationenabled")) {
        SetAnimationEnabled(strValue == "true");
    }
    else if ((srName == "animation_duration") || (srName == "animationduration")) {
        SetAnimationDuration(StringUtil::StringToInt32(strValue));
    }
    else if ((srName == "animation_easing") || (srName == "animationeasing")) {
        SetAnimationEasingName(strValue);
    }
    else if (srName == "marquee") {
        SetMarquee(strValue == "true");
    }
    else if ((srName == "marquee_width") || (srName == "marqueewidth")) {
        SetMarqueeWidth(StringUtil::StringToInt32(strValue), true);
    }
    else if ((srName == "marquee_step") || (srName == "marqueestep")) {
        SetMarqueeStep(StringUtil::StringToInt32(strValue), true);
    }
    else if (srName == "reverse") {
        SetReverse(strValue == "true");
    }
    else {
        Label::SetAttribute(srName, strValue);
    }
}

bool Progress::IsHorizontal() const
{
    return m_bHorizontal;
}

void Progress::SetHorizontal(bool bHorizontal)
{
    if (m_bHorizontal != bHorizontal) {
        m_bHorizontal = bHorizontal;
        Invalidate();
    }    
}

int32_t Progress::GetMinValue() const
{
    return m_nMinValue;
}

void Progress::SetMinValue(int32_t nMin)
{
    if (m_nMinValue != nMin) {
        m_nMinValue = nMin;
        //After min changes, pull the current value back into the new range so that GetValue()
        //never returns an out-of-range value
        ClampCurrentValue();
        Invalidate();
    }
}

int32_t Progress::GetMaxValue() const
{
    return m_nMaxValue;
}

void Progress::SetMaxValue(int32_t nMax)
{
    if (m_nMaxValue != nMax) {
        m_nMaxValue = nMax;
        //After max changes, pull the current value back into the new range so that GetValue()
        //never returns an out-of-range value
        ClampCurrentValue();
        Invalidate();
    }    
}

void Progress::ClampCurrentValue()
{
    //The range may be invalid (such as min > max); in that case leave the current value untouched
    //and let the existing painting/reading logic handle it
    if (m_nMaxValue < m_nMinValue) {
        return;
    }
    //When the buffer is not set it is -1 (the painting condition requires it to be > min), so it
    //takes no part in the clamping here and is not treated as an out-of-range value
    const bool bBufferSet = (m_fBufferValue > m_nMinValue);
    if ((m_fCurrentValue >= m_nMinValue) && (m_fCurrentValue <= m_nMaxValue) &&
        (!bBufferSet || (m_fBufferValue <= m_nMaxValue)) &&
        (m_fDisplayValue >= m_nMinValue) && (m_fDisplayValue <= m_nMaxValue)) {
        return;
    }

    double fNewValue = m_fCurrentValue;
    if (fNewValue > m_nMaxValue) {
        fNewValue = m_nMaxValue;
    }
    if (fNewValue < m_nMinValue) {
        fNewValue = m_nMinValue;
    }
    //Secondary progress (buffer): clamp it only when it has been set, keeping the original meaning of
    //a negative value as "not set"
    double fNewBuffer = m_fBufferValue;
    if (bBufferSet) {
        if (fNewBuffer > m_nMaxValue) {
            fNewBuffer = m_nMaxValue;
        }
        if (fNewBuffer < m_nMinValue) {
            fNewBuffer = m_nMinValue;
        }
    }
    //Display value: converge it as well while the animation is playing, so that it does not jump back
    //once the animation is over
    double fNewDisplay = m_fDisplayValue;
    if (fNewDisplay > m_nMaxValue) {
        fNewDisplay = m_nMaxValue;
    }
    if (fNewDisplay < m_nMinValue) {
        fNewDisplay = m_nMinValue;
    }

    const bool bValueClamped = (fNewValue != m_fCurrentValue);
    m_fCurrentValue = fNewValue;
    m_fDisplayValue = fNewDisplay;
    if (fNewBuffer != m_fBufferValue) {
        m_fBufferValue = fNewBuffer;
    }

    if (bValueClamped) {
        //The value was clamped because the range changed externally: update the percentage text too
        //(the display value of the animation has already been converged)
        UpdatePercentText();
        //A running animation still targets the old value from before the clamping and would push the
        //display value out of range again, so it must be stopped
        if ((m_pAnimPlayer != nullptr) && m_pAnimPlayer->IsPlaying()) {
            m_pAnimPlayer->SetCompleteCallback(nullptr);
            m_pAnimPlayer->Stop();
        }
    }
}

double Progress::GetValue() const
{
    return m_fCurrentValue;
}

void Progress::SetValue(double fValue)
{
    if (fValue > m_nMaxValue) {
        fValue = m_nMaxValue;
    }
    if (fValue < m_nMinValue) {
        fValue = m_nMinValue;
    }
    if (fValue == m_fCurrentValue) {
        return;
    }
    const double fOldValue = m_fCurrentValue;
    m_fCurrentValue = fValue;

    //Value change animation (when it is not yet initialized, or the animation is disabled,
    //jump directly to the target value)
    if (m_bAnimationEnabled && IsInited() && (m_nAnimationDuration > 0)) {
        StartValueAnimation(m_fDisplayValue, fValue);
    }
    else {
        m_fDisplayValue = fValue;
        UpdatePercentText();
    }

    //Send the value changed event
    SendValueChangedEvent(fValue, fOldValue);

    //Completion event: fired once when crossing from "not completed" to the "completion end point"
    //(max when increasing, min when counting down)
    const bool bOldCompleted = m_bReverse ? (fOldValue <= m_nMinValue) : (fOldValue >= m_nMaxValue);
    const bool bCompleted = m_bReverse ? (fValue <= m_nMinValue) : (fValue >= m_nMaxValue);
    if (bCompleted && !bOldCompleted) {
        SendEvent(kEventProgressCompleted, 0, 0);
    }

    Invalidate();
}

void Progress::SetValueNoEvent(double fValue)
{
    //Set the value without triggering the kEventValueChanged event (the same semantics as
    //SetTextNoEvent of the text controls)
    bool bOldValue = m_bDisableValueChangeEvent;
    m_bDisableValueChangeEvent = true;
    SetValue(fValue);
    m_bDisableValueChangeEvent = bOldValue;
}

bool Progress::IsStretchForeImage()
{
    return m_bStretchForeImage;
}

void Progress::SetStretchForeImage(bool bStretchForeImage /*= true*/)
{
    if (m_bStretchForeImage == bStretchForeImage) {
        return;
    }
    m_bStretchForeImage = bStretchForeImage;
    Invalidate();
}

std::string Progress::GetProgressImage() const
{
    if (m_pProgressImage != nullptr) {
        return m_pProgressImage->GetImageString();
    }
    return std::string();
}

void Progress::SetProgressImage(const std::string& strImage)
{
    if (m_pProgressImage == nullptr) {
        m_pProgressImage = new Image;
    }
    m_pProgressImage->SetImageString(strImage, Dpi());
    Invalidate();
}

std::string Progress::GetProgressColor() const
{
    return m_sProgressColor.c_str();
}

void Progress::SetProgressColor(const std::string& strProgressColor)
{
    //The color is not validated here: if it cannot be resolved while painting, the fill is skipped
    //(a lenient approach that avoids an assertion failure)
    if (m_sProgressColor == strProgressColor) {
        return;
    }
    m_sProgressColor = strProgressColor;
    Invalidate();
}

void Progress::ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale)
{
    if (!Dpi().CheckDisplayScaleFactor(nNewDpiScale)) {
        return;
    }
    int32_t iValue = GetMarqueeWidth();
    iValue = Dpi().GetScaleInt(iValue, nOldDpiScale);
    SetMarqueeWidth(iValue, false);

    iValue = GetMarqueeStep();
    iValue = Dpi().GetScaleInt(iValue, nOldDpiScale);
    SetMarqueeStep(iValue, false);

    BaseClass::ChangeDpiScale(nOldDpiScale, nNewDpiScale);
}

void Progress::PaintStateImages(IRender* pRender)
{
    if (pRender == nullptr) {
        return;
    }
    if (m_bMarquee && !m_sProgressColor.empty()) {
        PaintMarquee(pRender);
        return;
    }

    UiRect rc = GetProgressPos();

    //Buffer (secondary progress), painted first and covered by the main progress: the
    //"buffered + played" double progress of a video or a download
    if ((m_fBufferValue > m_fCurrentValue) && (m_fBufferValue > m_nMinValue)) {
        UiColor bufferColor = GetUiColor(m_strBufferColor);
        if (bufferColor.GetARGB() == 0) {
            //No buffer color specified: use a lighter version of the foreground color
            bufferColor = GetUiColor(m_sProgressColor.c_str());
        }
        if (bufferColor.GetARGB() != 0) {
            UiRect rcBuffer = CalcProgressRect(m_fBufferValue);
            UiRect rcBufferColor = GetRect();
            if (IsHorizontal()) {
                rcBufferColor.right = rcBufferColor.left + rcBuffer.right;
            }
            else {
                rcBufferColor.top = rcBufferColor.top + rcBuffer.top;
            }
            if (!rcBufferColor.IsEmpty()) {
                //The buffer is lighter (about 40% opacity)
                uint8_t bufferAlpha = static_cast<uint8_t>(GetAlpha() * 40 / 100);
                DrawSolidProgressRect(pRender, rcBufferColor, bufferColor, bufferAlpha);
            }
        }
    }

    //Solid color of the main progress
    if (!m_sProgressColor.empty()) {
        UiColor dwProgressColor = this->GetUiColor(m_sProgressColor.c_str());
        if (dwProgressColor.GetARGB() != 0) {
            UiRect rcProgressColor = GetRect();
            if (IsHorizontal()) {
                rcProgressColor.right = rcProgressColor.left + rc.right;
            }
            else {
                rcProgressColor.top = rcProgressColor.top + rc.top;
            }
            if (rcProgressColor.IsEmpty()) {
                return;
            }
            DrawSolidProgressRect(pRender, rcProgressColor, dwProgressColor, GetAlpha());
        }
    }
    if (rc.IsEmpty() || (m_pProgressImage == nullptr) || (m_pProgressImage->GetImageString().empty())) {
        return;
    }
    //Load image resources
    LoadImageInfo(*m_pProgressImage);

    m_sProgressImageModify.clear();
    if (m_bStretchForeImage) {
        m_sProgressImageModify = StringUtil::Printf("destscale='false' dest='%d,%d,%d,%d'", rc.left, rc.top, rc.right, rc.bottom);
    }
    else {
        ui::UiRect m_rcSrc = rc;
        std::shared_ptr<ImageInfo> pProgressImageCache = m_pProgressImage->GetImageInfo();
        if (pProgressImageCache != nullptr) {
            if (m_rcSrc.right > pProgressImageCache->GetWidth()) {
                m_rcSrc.right = pProgressImageCache->GetWidth();
            }
            if (m_rcSrc.bottom > pProgressImageCache->GetHeight()) {
                m_rcSrc.bottom = pProgressImageCache->GetHeight();
            }
        }
        m_sProgressImageModify = StringUtil::Printf("destscale='false' dest='%d,%d,%d,%d' source='%d,%d,%d,%d'"
            , rc.left, rc.top, rc.right, rc.bottom
            , m_rcSrc.left, m_rcSrc.top, m_rcSrc.right, m_rcSrc.bottom);
    }

    // Make the corner value not exceed the drawable range
    const UiRect corner = m_pProgressImage->GetImageAttribute().GetImageCorner();
    if (IsHorizontal()) {
        if (corner.left != 0 && corner.left >= rc.right) {
            std::string imageModify = m_sProgressImageModify.c_str();
            imageModify += StringUtil::Printf(" corner='%d,%d,%d,%d'",
                rc.right,
                corner.top,
                0,
                corner.bottom);
            m_sProgressImageModify = imageModify;
        }
    }
    else {
        if (corner.top != 0 && corner.top >= rc.bottom) {
            std::string imageModify = m_sProgressImageModify.c_str();
            imageModify += StringUtil::Printf(" corner='%d,%d,%d,%d'",
                corner.left,
                corner.bottom,
                corner.right,
                0);
            m_sProgressImageModify = imageModify;
        }
    }
    PaintImage(pRender, m_pProgressImage, m_sProgressImageModify.c_str());
}

UiRect Progress::GetProgressPos() const
{
    return CalcProgressRect(GetDisplayValue());
}

UiRect Progress::CalcProgressRect(double value) const
{
    int32_t nMax = GetMaxValue();
    int32_t nMin = GetMinValue();
    double fValue = value;

    if (nMax <= nMin) {
        nMax = nMin + 1;
    }
    if (fValue > nMax) {
        fValue = nMax;
    }
    if (fValue < nMin) {
        fValue = nMin;
    }
    //reverse (count down) is implemented below by inverting the ratio; the absolute value flip
    //(nMax - nMin) - fValue is no longer done here, because that form computes an out-of-range
    //ratio when min != 0

    UiRect rc;
    if (IsHorizontal()) {
        //Normalize to a ratio first and then invert it for reverse: the progress is empty at
        //value = min and full at value = max
        double fRatio = (fValue - nMin) / static_cast<double>(nMax - nMin);
        fRatio = std::clamp(fRatio, 0.0, 1.0);
        if (IsReverse()) {
            fRatio = 1.0 - fRatio;
        }
        rc.right = static_cast<int>(std::ceil(fRatio * (GetRect().right - GetRect().left)));
        rc.bottom = GetRect().bottom - GetRect().top;
    }
    else {
        double fRatio = (fValue - nMin) / static_cast<double>(nMax - nMin);
        fRatio = std::clamp(fRatio, 0.0, 1.0);
        if (IsReverse()) {
            fRatio = 1.0 - fRatio;
        }
        //A vertical progress bar fills from bottom to top, so top is determined by the remaining height
        rc.top = static_cast<int>(std::ceil((1.0 - fRatio) * (GetRect().bottom - GetRect().top)));
        rc.bottom = GetRect().bottom - GetRect().top;
        rc.right = GetRect().right - GetRect().left;
    }
    return rc;
}

void Progress::ClearImageCache()
{
    BaseClass::ClearImageCache();
    if (m_pProgressImage != nullptr) {
        m_pProgressImage->ClearImageCache();
    }    
}

void Progress::Play()
{
    if (!m_bMarquee || !IsVisible() || !IsEnabled()) {
        m_timer.Cancel();
        return;
    }
    m_nMarqueePos = m_nMarqueePos + GetMarqueeStep();

    //Track length and marquee block width: when the track length is <= 0 there is no room to scroll,
    //so reset directly to avoid an endless accumulation
    ui::UiRect rc = GetRect();
    const int32_t nTrackLen = IsHorizontal() ? (rc.right - rc.left) : (rc.bottom - rc.top);
    if (nTrackLen <= 0) {
        m_nMarqueePos = 0;
        Invalidate();
        return;
    }
    //Cycle length = track length + marquee block width (after the block has crossed the whole track
    //it reappears from the head)
    const int32_t nCycle = nTrackLen + GetMarqueeWidth();
    if (nCycle > 0) {
        m_nMarqueePos = m_nMarqueePos % nCycle;
        if (m_nMarqueePos < 0) {
            m_nMarqueePos += nCycle;
        }
    }

    Invalidate();
}

void Progress::PaintMarquee(IRender* pRender) 
{
    if (pRender == nullptr) {
        return;
    }
    if (!m_sProgressColor.empty()) {
        UiColor dwProgressColor = GlobalManager::Instance().Color().GetColor(m_sProgressColor.c_str());
        if (dwProgressColor.GetARGB() != 0) {
            ui::UiRect rc = GetRect();
            if (IsHorizontal()) {
                rc.left = std::max(m_nMarqueePos, 0) + rc.left;
                rc.right = rc.left + (m_nMarqueePos >= 0 ? GetMarqueeWidth() : (GetMarqueeWidth() + m_nMarqueePos));
            }
            else {
                rc.top = std::max(m_nMarqueePos, 0) + rc.top;
                rc.bottom = rc.top + (m_nMarqueePos >= 0 ? GetMarqueeWidth() : (GetMarqueeWidth() + m_nMarqueePos));
            }
            if (!rc.IsEmpty()) {
                //Uses the same progress_radius as the main progress, so that a configured corner radius
                //does not leave the marquee with square corners
                DrawSolidProgressRect(pRender, rc, dwProgressColor, GetAlpha());
            }
        }
    }
}

bool Progress::IsMarquee() const
{
    return m_bMarquee;
}

void Progress::SetMarquee(bool bMarquee)
{
    if (m_bMarquee == bMarquee) {
        return;
    }

    m_bMarquee = bMarquee;
    m_nMarqueePos = 0;

    if (m_bMarquee) {
        auto playCallback = UiBind(&Progress::Play, this);
        GlobalManager::Instance().Timer().AddTimer(m_timer.GetWeakFlag(), playCallback, m_nMarqueeElapsed);
    }
    else {
        m_timer.Cancel();
    }
    Invalidate();
}

int32_t Progress::GetMarqueeWidth() const
{
    return m_nMarqueeWidth;
}

void Progress::SetMarqueeWidth(int32_t nMarqueeWidth, bool bNeedDpiScale)
{
    if (nMarqueeWidth < 0) {
        nMarqueeWidth = 10;
    }
    if (bNeedDpiScale) {
        Dpi().ScaleInt(nMarqueeWidth);
    }
    if (m_nMarqueeWidth != nMarqueeWidth) {
        m_nMarqueeWidth = nMarqueeWidth;
        Invalidate();
    }
}

int32_t Progress::GetMarqueeStep() const
{
    return m_nMarqueeStep;
}

void Progress::SetMarqueeStep(int32_t nMarqueeStep, bool bNeedDpiScale)
{
    if (nMarqueeStep < 0) {
        nMarqueeStep = 4;
    }
    if (bNeedDpiScale) {
        Dpi().ScaleInt(nMarqueeStep);
    }
    if (m_nMarqueeStep != nMarqueeStep) {
        m_nMarqueeStep = nMarqueeStep;
        Invalidate();
    }    
}

int32_t Progress::GetMarqueeElapsed() const
{
    return m_nMarqueeElapsed;
}

void Progress::SetMarqueeElapsed(int32_t nMarqueeElapsed)
{
    if (!IsMarquee() || nMarqueeElapsed == m_nMarqueeElapsed) {
        return;
    }

    m_nMarqueeElapsed = nMarqueeElapsed;
    m_timer.Cancel();

    auto playCallback = UiBind(&Progress::Play, this);
    GlobalManager::Instance().Timer().AddTimer(m_timer.GetWeakFlag(), playCallback, m_nMarqueeElapsed);

    Invalidate();
}

void Progress::SetReverse(bool bReverse)
{
    m_bReverse = bReverse;
}

bool Progress::IsReverse() const
{
    return m_bReverse;
}

void Progress::SetShowPercent(bool bShowPercent)
{
    if (m_bShowPercent != bShowPercent) {
        m_bShowPercent = bShowPercent;
        if (m_bShowPercent) {
            UpdatePercentText();
        }
        Invalidate();
    }
}

void Progress::SetAnimationEnabled(bool bEnabled)
{
    m_bAnimationEnabled = bEnabled;
    if (!bEnabled) {
        //Animation disabled: stop the old animation and jump directly to the target value
        if ((m_pAnimPlayer != nullptr) && m_pAnimPlayer->IsPlaying()) {
            m_pAnimPlayer->SetCompleteCallback(nullptr);
            m_pAnimPlayer->Stop();
        }
        m_pAnimPlayer.reset();
        m_fDisplayValue = m_fCurrentValue;
    }
}

void Progress::SetAnimationDuration(int32_t nMs)
{
    if (nMs < 0) {
        nMs = 0;
    }
    m_nAnimationDuration = nMs;
}

void Progress::SetAnimationEasing(EasingFunctionType easing)
{
    m_easingAnimation = easing;
}

void Progress::SetAnimationEasingName(const std::string& strEasing)
{
    if (strEasing.empty()) {
        //An empty value falls back to the default curve, consistent with the constructor
        m_easingAnimation = EaseOutQuad;
        m_strAnimationEasing = "EaseOutQuad";
        return;
    }
    //The keys of GetEasingFunctionType are exactly the EasingFunctionType enum names (either case is
    //accepted for the first letter);
    //it returns EaseLinear for an unrecognized name. The original string is recorded here to make
    //diagnostics easier
    const EasingFunctionType easing = EasingFunctions::GetEasingFunctionType(strEasing);
    m_easingAnimation = easing;
    m_strAnimationEasing = strEasing;
}

void Progress::SetProgressRadius(int32_t nRadius)
{
    if (nRadius < 0) {
        nRadius = 0;
    }
    if (m_nProgressRadius != nRadius) {
        m_nProgressRadius = nRadius;
        Invalidate();
    }
}

void Progress::SetBufferValue(double fValue)
{
    if (fValue != m_fBufferValue) {
        m_fBufferValue = fValue;
        Invalidate();
    }
}

void Progress::SetBufferColor(const std::string& strColor)
{
    if (m_strBufferColor != strColor) {
        m_strBufferColor = strColor;
        Invalidate();
    }
}

void Progress::SetTextFormatCallback(const TextFormatCallback& callback)
{
    m_textFormatCallback = callback;
    if (m_bShowPercent) {
        UpdatePercentText();
    }
    Invalidate();
}

void Progress::DrawSolidProgressRect(IRender* pRender, const UiRect& rcProgressColor, UiColor color, uint8_t alpha)
{
    //Rounded fill (configured by progress_radius; the default of 0 means square corners)
    int32_t nRadius = Dpi().GetScaleInt(m_nProgressRadius);
    if (nRadius > 0) {
        int32_t nMaxRadius = std::min(rcProgressColor.Width(), rcProgressColor.Height()) / 2;
        nRadius = std::min(nRadius, nMaxRadius);
        if (nRadius > 0) {
            pRender->FillRoundRect(UiRectF::MakeFromRect(rcProgressColor), static_cast<float>(nRadius), static_cast<float>(nRadius), color, alpha);
            return;
        }
    }
    pRender->FillRect(UiRectF::MakeFromRect(rcProgressColor), color, alpha);
}

void Progress::SendValueChangedEvent(double newValue, double oldValue)
{
    if (m_bDisableValueChangeEvent) {
        //Sending the value changed event is disabled (during SetValueNoEvent)
        return;
    }
    //The value is passed truncated to an integer (the same as Slider); the exact floating point value
    //can be read with GetValue()
    SendEvent(kEventValueChanged, (WPARAM)newValue, (LPARAM)oldValue);
}

void Progress::UpdatePercentText()
{
    if (!m_bShowPercent) {
        return;
    }
    if (m_textFormatCallback) {
        //Custom format: the text is generated by the callback
        std::string strText = m_textFormatCallback(GetDisplayValue(), static_cast<double>(m_nMinValue), static_cast<double>(m_nMaxValue));
        SetText(strText);
        return;
    }
    int32_t nMin = m_nMinValue;
    int32_t nMax = m_nMaxValue;
    if (nMax <= nMin) {
        nMax = nMin + 1;
    }
    double fPercent = (GetDisplayValue() - nMin) / static_cast<double>(nMax - nMin) * 100.0;
    if (fPercent < 0.0) {
        fPercent = 0.0;
    }
    if (fPercent > 100.0) {
        fPercent = 100.0;
    }
    int32_t nPercent = static_cast<int32_t>(std::lround(fPercent));
    SetText(StringUtil::Printf("%d%%", nPercent));
}

void Progress::StartValueAnimation(double fFrom, double fTo)
{
    //Stop the old animation
    if ((m_pAnimPlayer != nullptr) && m_pAnimPlayer->IsPlaying()) {
        m_pAnimPlayer->SetCompleteCallback(nullptr);
        m_pAnimPlayer->Stop();
    }
    m_pAnimPlayer.reset(new AnimationPlayer);
    m_pAnimPlayer->SetAnimationType(AnimationType::kAnimationNone);
    m_pAnimPlayer->SetTotalMillSeconds(m_nAnimationDuration);
    m_pAnimPlayer->SetFrameIntervalMillSeconds(16);
    m_pAnimPlayer->SetEasingFunctionType(m_easingAnimation);
    m_pAnimPlayer->SetStartValue(0);
    m_pAnimPlayer->SetEndValue(1000);

    std::weak_ptr<WeakFlag> weakFlag = GetWeakFlag();
    m_pAnimPlayer->SetPlayCallback([this, weakFlag, fFrom, fTo](int32_t nValue) {
        if (weakFlag.expired()) {
            return;
        }
        m_fDisplayValue = fFrom + (fTo - fFrom) * static_cast<double>(nValue) / 1000.0;
        UpdatePercentText();
        Invalidate();
    });
    m_pAnimPlayer->SetCompleteCallback([this, weakFlag, fTo]() {
        if (weakFlag.expired()) {
            return;
        }
        m_fDisplayValue = fTo;
        UpdatePercentText();
        Invalidate();
    });
    m_pAnimPlayer->Start();
}

}
