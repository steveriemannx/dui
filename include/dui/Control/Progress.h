#ifndef UI_CONTROL_PROGRESS_H_
#define UI_CONTROL_PROGRESS_H_

#include "dui/Control/Label.h"
#include "dui/Image/Image.h"
#include "dui/Animation/AnimationPlayer.h"
#include <functional>

namespace ui
{

/** Progress bar control
*/
class DUI_API Progress : public LabelTemplate<Control>
{
    typedef LabelTemplate<Control> BaseClass;
public:
    /** Progress text formatting callback: customizes the text displayed by show_percent
    *   @param value     The current progress value (the display value during the animation)
    *   @param minValue  The minimum progress value
    *   @param maxValue  The maximum progress value
    *   @return The text to display (such as "5/10", "3.2 GB", "3 seconds left")
    */
    typedef std::function<std::string(double value, double minValue, double maxValue)> TextFormatCallback;

public:
    explicit Progress(Window* pWindow);
    virtual ~Progress() override;

    /// Override parent class methods to provide personalized functionality; please refer to the parent class declaration
    virtual std::string GetType() const override;
    virtual void SetAttribute(const std::string& strName, const std::string& strValue) override;
    virtual void PaintStateImages(IRender* pRender) override;
    virtual void ClearImageCache() override;

    /** When the DPI changes, update the control size and layout
    * @param [in] nOldDpiScale The old DPI scale percentage
    * @param [in] nNewDpiScale The new DPI scale percentage, consistent with the value of Dpi().GetScale()
    */
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;

    /** Determine whether it is a horizontal progress bar
     * @return Returns true if it is a horizontal progress bar, false if it is a vertical progress bar
     */
    bool IsHorizontal() const;

    /** Set horizontal or vertical progress bar
     * @param [in] bHorizontal Set to a horizontal progress bar when true, a vertical progress bar when false, the default is true
     */
    void SetHorizontal(bool bHorizontal = true);

    /** Get the minimum value of the progress bar
     * @return Returns the minimum value of the progress bar
     */
    int32_t GetMinValue() const;

    /** Set the minimum value of the progress bar
     * @param [in] nMin The minimum value
     */
    void SetMinValue(int32_t nMin);

    /** Get the maximum value of the progress bar
     * @return Returns the maximum value of the progress bar
     */
    int32_t GetMaxValue() const;

    /** Set the maximum value of the progress bar
     * @param [in] nMax The maximum value to set
     */
    void SetMaxValue(int32_t nMax);

    /** Get the current progress percentage
     * @return Returns the current progress percentage
     */
    double GetValue() const;

    /** Set the current progress percentage
     * @param[in] fValue The percentage value to set
     */
    void SetValue(double fValue);

    /** Set the current progress percentage without triggering the kEventValueChanged event
     * Used to synchronize a value internally (for example, one event source driving several
     * controls to display the same value), so that the value-changed callback is not triggered
     * again, which would lead to a recursive loop
     * @param[in] fValue The percentage value to set
     */
    void SetValueNoEvent(double fValue);

    /** Whether the progress bar foreground image is scaled for display
     * @return Returns true for scaled display, false for unscaled display
     */
    bool IsStretchForeImage();

    /** Set whether the progress bar foreground image is scaled for display
     * @param [in] bStretchForeImage true for scaled display, false for unscaled display
     */
    void SetStretchForeImage(bool bStretchForeImage = true);

    /** Get the progress bar background image
     * @return Returns the background image location
     */
    std::string GetProgressImage() const;

    /** Set the progress bar background image
     * @param [in] strImage The image address
     */
    void SetProgressImage(const std::string& strImage);

    /** Get the progress bar background color
     * @return Returns the string value of the background color, corresponding to the specified color value in global.xml
     */
    std::string GetProgressColor() const;

    /** Set the progress bar background color
     * @param [in] The background color string to set; the string must exist in global.xml
     */
    void SetProgressColor(const std::string& strProgressColor);

    /** Get the progress bar position
    * @return Returns the progress bar area coordinates (relative coordinates with GetRect() as the whole area, with the top-left corner being <0,0>)
    */
    virtual UiRect GetProgressPos() const;

    /** Get the current display value (the interpolated value while the value animation plays;
    *   otherwise it is equal to GetValue())
    * @return Returns the progress value currently used for painting
    */
    double GetDisplayValue() const { return m_fDisplayValue; }

    /** Listen for the progress value changed event
    * @param [in] callback The callback invoked after the progress value changed (EventArgs::wParam is
    *                      the new value and lParam is the old value, both truncated to integers)
    * @param [in] callbackID The ID of this callback function (used to remove the callback function)
    */
    void AttachValueChanged(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventValueChanged, callback, callbackID); }

    /** Set whether to display the percentage text automatically (it overrides the control text and is
    *  updated to "xx%" whenever the progress changes)
    */
    void SetShowPercent(bool bShowPercent);

    /** Get whether the percentage text is displayed automatically
    */
    bool IsShowPercent() const { return m_bShowPercent; }

    /** Set whether the value change animation is enabled (disabled by default)
    */
    void SetAnimationEnabled(bool bEnabled);

    /** Get whether the value change animation is enabled
    */
    bool IsAnimationEnabled() const { return m_bAnimationEnabled; }

    /** Set the value change animation duration (milliseconds, 300 by default)
    */
    void SetAnimationDuration(int32_t nMs);

    /** Get the value change animation duration
    */
    int32_t GetAnimationDuration() const { return m_nAnimationDuration; }

    /** Set the easing curve of the value change animation (EaseOutQuad by default)
    * @param [in] easing The easing function type; see enum EasingFunctionType
    */
    void SetAnimationEasing(EasingFunctionType easing);

    /** Get the easing curve of the value change animation
    */
    EasingFunctionType GetAnimationEasing() const { return m_easingAnimation; }

    /** Set the easing curve of the value change animation by name; the name is exactly the same as
    *  the EasingFunctionType enum value
    * @param [in] strEasing The easing function name, such as "EaseOutQuad", "EaseLinear", "EaseOutElastic";
    *                       an empty or unrecognized name falls back to the default EaseOutQuad
    */
    void SetAnimationEasingName(const std::string& strEasing);

    /** Get the easing curve name of the value change animation (the same as the EasingFunctionType enum value)
    */
    const std::string& GetAnimationEasingName() const { return m_strAnimationEasing; }

    /** Set the corner radius of the solid color foreground (in pixels, DPI-scaled automatically;
    *  0 by default means square corners)
    */
    void SetProgressRadius(int32_t nRadius);

    /** Get the corner radius of the solid color foreground
    */
    int32_t GetProgressRadius() const { return m_nProgressRadius; }

    /** Clamp the current value into the new [min, max] range immediately (called automatically when
    *  min/max change)
    */
    void ClampCurrentValue();

    /** Set the secondary progress value (buffer; usually slightly ahead of the main progress, such as
    *  the cached position of a video)
    * @param [in] fValue The secondary progress value; it is not painted when it is less than or equal
    *                    to min; the default of -1 means "not set"
    */
    void SetBufferValue(double fValue);

    /** Get the secondary progress value
    */
    double GetBufferValue() const { return m_fBufferValue; }

    /** Set the secondary progress color (a semantic color name or a color value; when empty, a
    *  semi-transparent version of the foreground color is used)
    */
    void SetBufferColor(const std::string& strColor);

    /** Get the secondary progress color
    */
    const std::string& GetBufferColor() const { return m_strBufferColor; }

    /** Set the progress text formatting callback (only used when show_percent is enabled; it customizes
    *  the displayed text)
    * @param [in] callback The formatting callback; set it to null to restore the default "xx%" format
    */
    void SetTextFormatCallback(const TextFormatCallback& callback);

    /** Listen for the progress completed event (triggered when the progress value reaches the completion
    *  end point: max when increasing, min when decreasing)
    */
    void AttachProgressCompleted(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventProgressCompleted, callback, callbackID); }

    /** Play the marquee
     */
    virtual void Play();

    /** Draw the marquee
     */
    void PaintMarquee(IRender* pRender);

    /** Whether it is a marquee
     */
    bool IsMarquee() const;

    /** Set marquee
     */
    void SetMarquee(bool bMarquee);

    /** Get the marquee width
     */
    int32_t GetMarqueeWidth() const;

    /** Set the marquee width
     */
    void SetMarqueeWidth(int32_t nMarqueeWidth, bool bNeedDpiScale);

    /** Get the marquee step
     */
    int32_t GetMarqueeStep() const;

    /** Set the marquee step
     */
    void SetMarqueeStep(int32_t nMarqueeStep, bool bNeedDpiScale);

    /** Get the marquee duration
     */
    int32_t GetMarqueeElapsed() const;

    /** Set the marquee duration
     */
    void SetMarqueeElapsed(int32_t nMarqueeElapsed);

public:
    /** Set whether to fill the progress bar in reverse
    */
    void SetReverse(bool bReverse);

    /** Whether to fill the progress bar in reverse
    */
    bool IsReverse() const;

private:
    /** Send the progress value changed event
    * @param newValue The new value
    * @param oldValue The old value
    */
    void SendValueChangedEvent(double newValue, double oldValue);

    /** Update the percentage display text (when show_percent is enabled)
    */
    void UpdatePercentText();

    /** Start the value change animation (easing from fFrom to fTo)
    */
    void StartValueAnimation(double fFrom, double fTo);

    /** Calculate the filled area from the progress value (including clamping and reverse handling;
    *  shared by GetProgressPos and the buffer painting)
    * @param value The progress value
    */
    UiRect CalcProgressRect(double value) const;

    /** Draw a solid progress fill (including the rounded corners; shared by the main progress and the buffer)
    */
    void DrawSolidProgressRect(IRender* pRender, const UiRect& rcProgressColor, UiColor color, uint8_t alpha);

private:
    //Whether the progress bar is horizontal or vertical: true for horizontal, false for vertical
    bool m_bHorizontal;

    //Specify whether the progress bar foreground image is scaled for display
    bool m_bStretchForeImage;

    //Maximum progress value (default is 100)
    int32_t m_nMaxValue;

    //Minimum progress value (default is 0)
    int32_t m_nMinValue;

    //Current progress value
    double m_fCurrentValue;

    //Secondary progress value (buffer); the default of -1 means "not set"
    double m_fBufferValue;

    //Secondary progress color (a semantic color name or a color value)
    std::string m_strBufferColor;

    //Callback used to format the progress text
    TextFormatCallback m_textFormatCallback;

    //Progress bar foreground image
    Image* m_pProgressImage;

    //Progress bar foreground image attributes
    UiString m_sProgressImageModify;

    //Progress bar foreground color; if not specified, the default color is used
    UiString m_sProgressColor;

    //Scrolling effect
    bool m_bMarquee;
    int32_t m_nMarqueeWidth;
    int32_t m_nMarqueeStep;
    int32_t m_nMarqueeElapsed;
    int32_t m_nMarqueePos;

    //Display value of the value change animation (the interpolated intermediate value)
    double m_fDisplayValue;

    //Whether the value change animation is enabled
    bool m_bAnimationEnabled;

    //Value change animation duration (milliseconds)
    int32_t m_nAnimationDuration;

    //Easing curve of the value change animation (EaseOutQuad by default)
    EasingFunctionType m_easingAnimation;

    //Easing curve name of the value change animation (the same as the EasingFunctionType enum value;
    //used for the XML round-trip and for logging)
    std::string m_strAnimationEasing;

    //Player of the value change animation
    std::unique_ptr<AnimationPlayer> m_pAnimPlayer;

    //Whether the percentage text is displayed automatically
    bool m_bShowPercent;

    //Whether sending the progress value changed event is disabled (true while SetValueNoEvent runs)
    bool m_bDisableValueChangeEvent;

    //Corner radius of the solid color foreground (the raw value before DPI scaling)
    int32_t m_nProgressRadius;

private:
    //Whether to count down (progress from 100 to 0)
    bool m_bReverse;

    //Timer cancellation mechanism
    WeakCallbackFlag m_timer;
};

} // namespace ui

#endif // UI_CONTROL_PROGRESS_H_
