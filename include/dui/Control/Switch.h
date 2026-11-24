#ifndef UI_CONTROL_SWITCH_H_
#define UI_CONTROL_SWITCH_H_

#include "dui/Control/CheckBox.h"
#include "dui/Animation/AnimationPlayer.h"
#include "dui/Image/Image.h"

namespace ui {

/** Switch toggle control (derived from CheckBox)
 *  Rounded track plus a round thumb; switching the checked state animates the thumb slide and a color/image cross-fade.
 *  Both the track and the thumb support SVG image skins: when images are configured, the off/on images cross-fade
 *  according to the animation progress while the thumb image slides along the track; when no images are configured
 *  the control paints itself with semantic colors (FillRoundRect plus color interpolation).
 *  Default skin classes: switch (iOS style), switch_fluent (Windows 11 Fluent style) and
 *  switch_material (Material Design 3 style).
 *  The checked state and the events are the same as CheckBox (kEventSelect / kEventUnSelect).
 */
class DUI_API Switch : public CheckBox
{
    typedef CheckBox BaseClass;
public:
    explicit Switch(Window* pWindow);
    Switch(const Switch& r) = delete;
    Switch& operator=(const Switch& r) = delete;
    ~Switch() override;

    virtual std::string GetType() const override;
    virtual void SetAttribute(const std::string& strName, const std::string& strValue) override;
    virtual void PaintStateColors(IRender* pRender) override;
    virtual void PaintStateImages(IRender* pRender) override;
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;

public:
    /** Sets the switch animation duration in milliseconds; 0 disables the animation
    */
    void SetAnimationDuration(int32_t nMs);

    /** Gets the switch animation duration in milliseconds
    */
    int32_t GetAnimationDuration() const { return m_nAnimationMs; }

    /** Sets/gets the track color (unchecked state)
    */
    void SetTrackOffColor(const std::string& strColor);
    std::string GetTrackOffColor() const { return m_strTrackOffColor; }

    /** Sets/gets the track color (checked state)
    */
    void SetTrackOnColor(const std::string& strColor);
    std::string GetTrackOnColor() const { return m_strTrackOnColor; }

    /** Sets/gets the thumb color (unchecked state)
    */
    void SetThumbOffColor(const std::string& strColor);
    std::string GetThumbOffColor() const { return m_strThumbOffColor; }

    /** Sets/gets the thumb color (checked state)
    */
    void SetThumbOnColor(const std::string& strColor);
    std::string GetThumbOnColor() const { return m_strThumbOnColor; }

    /** Sets the track image (unchecked state); an empty strImage clears it and falls back to the painted color
    */
    void SetTrackOffImage(const std::string& strImage);
    std::string GetTrackOffImage() const { return m_strTrackOffImage; }

    /** Sets the track image (checked state); an empty strImage clears it and falls back to the painted color
    */
    void SetTrackOnImage(const std::string& strImage);
    std::string GetTrackOnImage() const { return m_strTrackOnImage; }

    /** Sets the thumb image (unchecked state); an empty strImage clears it and falls back to the painted color
    */
    void SetThumbOffImage(const std::string& strImage);
    std::string GetThumbOffImage() const { return m_strThumbOffImage; }

    /** Sets the thumb image (checked state); an empty strImage clears it and falls back to the painted color
    */
    void SetThumbOnImage(const std::string& strImage);
    std::string GetThumbOnImage() const { return m_strThumbOnImage; }

    /** Sets the distance between the square thumb image box and the track edges (DPI aware); default 0
    *   Note: the side length of the square thumb image box equals the track height; draw shadows and other padding inside the image
    */
    void SetThumbPadding(int32_t nPadding, bool bNeedDpiScale = true);
    int32_t GetThumbPadding() const { return m_nThumbPadding; }

protected:
    /** Called when the checked state changes (from CheckBox); starts the switch animation
    */
    virtual void OnPrivateSetSelected() override;

private:
    /** Computes the square thumb image box (translated along the track by the current animation progress, vertically centered)
    */
    UiRect GetThumbRect() const;

    /** Interpolates two colors by progress
    * @param [in] t Progress value, in the range [0, 1]
    */
    static UiColor LerpColor(UiColor colorFrom, UiColor colorTo, double t);

    /** Sets/updates one image (released when strImage is empty)
    */
    void UpdateSwitchImage(std::unique_ptr<Image>& pImage, const std::string& strImage);

    /** Draws an image into the destination rectangle with the given alpha
    * @param [in] nAlpha Image alpha, in the range [0, 255]
    */
    void DrawSwitchImage(IRender* pRender, Image* pImage, const UiRect& rcDest, uint8_t nAlpha) const;

    /** Whether the track has a usable pair of images (off/on must both be configured)
    */
    bool HasTrackImages() const;

    /** Whether the thumb has a usable pair of images (off/on must both be configured)
    */
    bool HasThumbImages() const;

private:
    /** Animation duration in milliseconds
    */
    int32_t m_nAnimationMs;

    /** Current animation progress (0: unchecked, 100: checked)
    */
    int32_t m_nAnimValue;

    std::string m_strTrackOffColor;     //Track color (unchecked)
    std::string m_strTrackOnColor;      //Track color (checked)
    std::string m_strThumbOffColor;     //Thumb color (unchecked)
    std::string m_strThumbOnColor;      //Thumb color (checked)

    std::string m_strTrackOffImage;     //Track image attribute (unchecked)
    std::string m_strTrackOnImage;      //Track image attribute (checked)
    std::string m_strThumbOffImage;     //Thumb image attribute (unchecked)
    std::string m_strThumbOnImage;      //Thumb image attribute (checked)

    /** Distance between the square thumb image box and the track edges (physical pixels after DPI scaling)
    */
    int32_t m_nThumbPadding;

    std::unique_ptr<Image> m_pTrackOffImage;
    std::unique_ptr<Image> m_pTrackOnImage;
    std::unique_ptr<Image> m_pThumbOffImage;
    std::unique_ptr<Image> m_pThumbOnImage;

    /** Switch animation player
    */
    std::unique_ptr<AnimationPlayer> m_pAnimationPlayer;
};

} // namespace ui

#endif // UI_CONTROL_SWITCH_H_
