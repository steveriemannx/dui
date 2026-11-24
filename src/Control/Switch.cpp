#include "dui/Control/Switch.h"
#include "dui/Core/GlobalManager.h"

namespace ui
{

Switch::Switch(Window* pWindow) :
    BaseClass(pWindow),
    m_nAnimationMs(160),
    m_nAnimValue(0),
    m_strTrackOffColor("bg_switch_track_off"),
    m_strTrackOnColor("bg_switch_track_on"),
    m_strThumbOffColor("bg_switch_thumb_off"),
    m_strThumbOnColor("bg_switch_thumb_on"),
    m_nThumbPadding(0)
{
}

Switch::~Switch()
{
}

std::string Switch::GetType() const
{
    return DUI_CTR_SWITCH;
}

void Switch::SetAttribute(const std::string& strName, const std::string& strValue)
{
    if ((strName == "switch_animation_ms") || (strName == "animation_ms")) {
        SetAnimationDuration(StringUtil::StringToInt32(strValue));
    }
    else if ((strName == "track_off_color") || (strName == "trackoffcolor")) {
        SetTrackOffColor(strValue);
    }
    else if ((strName == "track_on_color") || (strName == "trackoncolor")) {
        SetTrackOnColor(strValue);
    }
    else if ((strName == "thumb_off_color") || (strName == "thumboffcolor")) {
        SetThumbOffColor(strValue);
    }
    else if ((strName == "thumb_on_color") || (strName == "thumboncolor")) {
        SetThumbOnColor(strValue);
    }
    else if ((strName == "track_off_image") || (strName == "trackoffimage")) {
        SetTrackOffImage(strValue);
    }
    else if ((strName == "track_on_image") || (strName == "trackonimage")) {
        SetTrackOnImage(strValue);
    }
    else if ((strName == "thumb_off_image") || (strName == "thumboffimage")) {
        SetThumbOffImage(strValue);
    }
    else if ((strName == "thumb_on_image") || (strName == "thumbonimage")) {
        SetThumbOnImage(strValue);
    }
    else if (strName == "thumb_padding") {
        SetThumbPadding(StringUtil::StringToInt32(strValue));
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

void Switch::ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale)
{
    if (!Dpi().CheckDisplayScaleFactor(nNewDpiScale)) {
        return;
    }
    BaseClass::ChangeDpiScale(nOldDpiScale, nNewDpiScale);
    m_nThumbPadding = Dpi().GetScaleInt(m_nThumbPadding, nOldDpiScale);

    //The image attributes have to be re-parsed with the new DPI
    UpdateSwitchImage(m_pTrackOffImage, m_strTrackOffImage);
    UpdateSwitchImage(m_pTrackOnImage, m_strTrackOnImage);
    UpdateSwitchImage(m_pThumbOffImage, m_strThumbOffImage);
    UpdateSwitchImage(m_pThumbOnImage, m_strThumbOnImage);
}

void Switch::SetAnimationDuration(int32_t nMs)
{
    m_nAnimationMs = nMs;
    if (m_nAnimationMs < 0) {
        m_nAnimationMs = 0;
    }
}

void Switch::SetTrackOffColor(const std::string& strColor)
{
    m_strTrackOffColor = strColor;
    Invalidate();
}

void Switch::SetTrackOnColor(const std::string& strColor)
{
    m_strTrackOnColor = strColor;
    Invalidate();
}

void Switch::SetThumbOffColor(const std::string& strColor)
{
    m_strThumbOffColor = strColor;
    Invalidate();
}

void Switch::SetThumbOnColor(const std::string& strColor)
{
    m_strThumbOnColor = strColor;
    Invalidate();
}

void Switch::UpdateSwitchImage(std::unique_ptr<Image>& pImage, const std::string& strImage)
{
    if (strImage.empty()) {
        pImage.reset();
        return;
    }
    if (pImage == nullptr) {
        pImage = std::make_unique<Image>();
        pImage->SetControl(this);
    }
    pImage->SetImageString(strImage, Dpi());
}

void Switch::SetTrackOffImage(const std::string& strImage)
{
    m_strTrackOffImage = strImage;
    UpdateSwitchImage(m_pTrackOffImage, strImage);
    Invalidate();
}

void Switch::SetTrackOnImage(const std::string& strImage)
{
    m_strTrackOnImage = strImage;
    UpdateSwitchImage(m_pTrackOnImage, strImage);
    Invalidate();
}

void Switch::SetThumbOffImage(const std::string& strImage)
{
    m_strThumbOffImage = strImage;
    UpdateSwitchImage(m_pThumbOffImage, strImage);
    Invalidate();
}

void Switch::SetThumbOnImage(const std::string& strImage)
{
    m_strThumbOnImage = strImage;
    UpdateSwitchImage(m_pThumbOnImage, strImage);
    Invalidate();
}

void Switch::SetThumbPadding(int32_t nPadding, bool bNeedDpiScale)
{
    if (bNeedDpiScale) {
        nPadding = Dpi().GetScaleInt(nPadding);
    }
    if (nPadding < 0) {
        nPadding = 0;
    }
    m_nThumbPadding = nPadding;
    Invalidate();
}

bool Switch::HasTrackImages() const
{
    return (m_pTrackOffImage != nullptr) && !m_pTrackOffImage->GetImagePath().empty() &&
           (m_pTrackOnImage != nullptr) && !m_pTrackOnImage->GetImagePath().empty();
}

bool Switch::HasThumbImages() const
{
    return (m_pThumbOffImage != nullptr) && !m_pThumbOffImage->GetImagePath().empty() &&
           (m_pThumbOnImage != nullptr) && !m_pThumbOnImage->GetImagePath().empty();
}

void Switch::DrawSwitchImage(IRender* pRender, Image* pImage, const UiRect& rcDest, uint8_t nAlpha) const
{
    if ((pRender == nullptr) || (pImage == nullptr) || (nAlpha == 0)) {
        return;
    }
    PaintImage(pRender, pImage, "", static_cast<int32_t>(nAlpha), nullptr, &rcDest, nullptr);
}

UiColor Switch::LerpColor(UiColor colorFrom, UiColor colorTo, double t)
{
    auto lerp = [t](uint8_t from, uint8_t to) -> uint8_t {
        return (uint8_t)(from + (to - from) * t);
    };
    return UiColor(UiColor::MakeARGB(lerp(colorFrom.GetA(), colorTo.GetA()),
                                     lerp(colorFrom.GetR(), colorTo.GetR()),
                                     lerp(colorFrom.GetG(), colorTo.GetG()),
                                     lerp(colorFrom.GetB(), colorTo.GetB())));
}

UiRect Switch::GetThumbRect() const
{
    UiRect rc = GetRect();
    UiPadding rcPadding = GetControlPadding();
    rc.Deflate(rcPadding);

    //The side length of the square thumb image box equals the track height (shadows and other padding are drawn inside the image)
    const int32_t nSize = rc.Height();
    const int32_t nCenterY = rc.CenterY();

    //The center x of the thumb square box slides with the animation progress
    const int32_t nMinX = rc.left + m_nThumbPadding + nSize / 2;
    const int32_t nMaxX = rc.right - m_nThumbPadding - nSize / 2;
    int32_t nCenterX = nMinX;
    if (nMaxX > nMinX) {
        nCenterX = nMinX + (nMaxX - nMinX) * m_nAnimValue / 100;
    }

    UiRect thumbRect;
    thumbRect.left = nCenterX - nSize / 2;
    thumbRect.right = nCenterX + nSize / 2;
    thumbRect.top = nCenterY - nSize / 2;
    thumbRect.bottom = nCenterY + nSize / 2;
    return thumbRect;
}

void Switch::PaintStateColors(IRender* pRender)
{
    if (pRender == nullptr) {
        return;
    }
    UiRect rc = GetRect();
    UiPadding rcPadding = GetControlPadding();
    rc.Deflate(rcPadding);
    if ((rc.Width() <= 0) || (rc.Height() <= 0)) {
        return;
    }

    const double t = m_nAnimValue / 100.0;

    //Track image: the off/on images cross-fade according to the progress
    if (HasTrackImages()) {
        DrawSwitchImage(pRender, m_pTrackOffImage.get(), rc,
                        static_cast<uint8_t>((1.0 - t) * 255.0));
        DrawSwitchImage(pRender, m_pTrackOnImage.get(), rc,
                        static_cast<uint8_t>(t * 255.0));
        return;
    }

    //Without images, paint the track with the semantic color (corner radius = half of the height), interpolating the color by progress
    const float fRound = (float)rc.Height() / 2.0f;
    UiColor trackColor = LerpColor(GetUiColor(m_strTrackOffColor), GetUiColor(m_strTrackOnColor), t);
    pRender->FillRoundRect(UiRectF((float)rc.left, (float)rc.top, (float)rc.right, (float)rc.bottom), fRound, fRound, trackColor, GetAlpha());
}

void Switch::PaintStateImages(IRender* pRender)
{
    if (pRender == nullptr) {
        return;
    }

    const double t = m_nAnimValue / 100.0;

    //Thumb image: the square box slides along the track and the off/on images cross-fade according to the progress
    if (HasThumbImages()) {
        UiRect thumbRect = GetThumbRect();
        if ((thumbRect.Width() > 0) && (thumbRect.Height() > 0)) {
            DrawSwitchImage(pRender, m_pThumbOffImage.get(), thumbRect,
                            static_cast<uint8_t>((1.0 - t) * 255.0));
            DrawSwitchImage(pRender, m_pThumbOnImage.get(), thumbRect,
                            static_cast<uint8_t>(t * 255.0));
        }
        return;
    }

    //Without images, paint the round thumb with the semantic color (diameter = track height - 2 * padding), interpolating the color by progress
    UiRect rc = GetRect();
    UiPadding rcPadding = GetControlPadding();
    rc.Deflate(rcPadding);
    if ((rc.Width() <= 0) || (rc.Height() <= 0)) {
        return;
    }
    const int32_t nThumbMargin = Dpi().GetScaleInt(2);
    const int32_t nThumbSize = rc.Height() - nThumbMargin * 2;
    if (nThumbSize <= 0) {
        return;
    }
    const int32_t nMinX = rc.left + nThumbMargin + nThumbSize / 2;
    const int32_t nMaxX = rc.right - nThumbMargin - nThumbSize / 2;
    int32_t nCenterX = nMinX;
    if (nMaxX > nMinX) {
        nCenterX = nMinX + (nMaxX - nMinX) * m_nAnimValue / 100;
    }
    UiRect thumbRect;
    thumbRect.left = nCenterX - nThumbSize / 2;
    thumbRect.right = nCenterX + nThumbSize / 2;
    thumbRect.top = rc.CenterY() - nThumbSize / 2;
    thumbRect.bottom = rc.CenterY() + nThumbSize / 2;

    const float fRound = (float)thumbRect.Height() / 2.0f;
    UiColor thumbColor = LerpColor(GetUiColor(m_strThumbOffColor), GetUiColor(m_strThumbOnColor), t);
    pRender->FillRoundRect(UiRectF((float)thumbRect.left, (float)thumbRect.top, (float)thumbRect.right, (float)thumbRect.bottom), fRound, fRound, thumbColor, GetAlpha());
}

void Switch::OnPrivateSetSelected()
{
    //Update the animation target value
    const int32_t nTargetValue = IsSelected() ? 100 : 0;

    //If the control is not initialized yet (for example during XML loading) or the animation duration is 0, jump to the target value
    if (!IsInited() || (m_nAnimationMs <= 0)) {
        m_nAnimValue = nTargetValue;
        return;
    }

    //Nothing to do when the current value already is the target value
    if (m_nAnimValue == nTargetValue) {
        return;
    }

    //Stop the previous animation first
    if ((m_pAnimationPlayer != nullptr) && m_pAnimationPlayer->IsPlaying()) {
        m_pAnimationPlayer->SetCompleteCallback(nullptr);
        m_pAnimationPlayer->Stop();
    }

    m_pAnimationPlayer.reset(new AnimationPlayer);
    m_pAnimationPlayer->SetAnimationType(AnimationType::kAnimationNone);
    m_pAnimationPlayer->SetTotalMillSeconds(m_nAnimationMs);
    m_pAnimationPlayer->SetFrameIntervalMillSeconds(16);
    m_pAnimationPlayer->SetEasingFunctionType(EaseInOutQuad);
    m_pAnimationPlayer->SetStartValue(m_nAnimValue);
    m_pAnimationPlayer->SetEndValue(nTargetValue);

    std::weak_ptr<WeakFlag> weakFlag = GetWeakFlag();
    m_pAnimationPlayer->SetPlayCallback([this, weakFlag](int32_t nValue) {
        if (weakFlag.expired()) {
            return;
        }
        m_nAnimValue = nValue;
        Invalidate();
    });
    m_pAnimationPlayer->SetCompleteCallback([this, weakFlag]() {
        if (weakFlag.expired()) {
            return;
        }
        //The animation has finished: make sure it lands on the final value
        m_nAnimValue = IsSelected() ? 100 : 0;
        Invalidate();
    });
    m_pAnimationPlayer->Start();
}

} // namespace ui
