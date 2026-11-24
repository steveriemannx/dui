#ifndef UI_CONTROL_SLIDER_H_
#define UI_CONTROL_SLIDER_H_

#include "dui/Control/Progress.h"
#include "dui/Core/ControlFinder.h"
#include <string>

namespace ui
{

/** Slider tick mark position
*/
enum class SliderTickPos : uint8_t
{
    kBottom,    //Horizontal slider: below the track (default)
    kTop,       //Horizontal slider: above the track
    kRight,     //Vertical slider: right side of the track (default)
    kLeft       //Vertical slider: left side of the track
};

/** Slider control
*/
class DUI_API Slider : public Progress
{
    typedef Progress BaseClass;
public:
    explicit Slider(Window* pWindow);

    /// Override the parent class method to provide personalized functionality; please refer to the parent class declaration
    virtual std::string GetType() const override;
    virtual UiRect GetProgressPos() const override;
    virtual void HandleEvent(const EventArgs& msg) override;
    virtual void SetAttribute(const std::string& strName, const std::string& strValue) override;
    virtual void PaintStateImages(IRender* pRender) override;
    virtual void PaintBkColor(IRender* pRender) override;
    virtual void ClearImageCache() override;

    /** Get the control flags; required for keyboard accessibility (so that the control can be reached with Tab)
    */
    virtual uint32_t GetControlFlags() const override;

    /** Whether the value jumps immediately to the clicked position when the left mouse button is pressed on the track
    * @param [in] bEnable true jumps immediately (default), false keeps the old behaviour of jumping only when the mouse button is released
    */
    void SetClickToJump(bool bEnable);

    /** Get whether the value jumps immediately when the left mouse button is pressed on the track
    */
    bool IsClickToJump() const { return m_bClickToJump; }

    /** Set the big step of PageUp/PageDown and Shift + an arrow key
    * @param [in] nPageStep The big step; <=0 means (max - min) / 10 is used automatically
    */
    void SetPageStep(int32_t nPageStep);

    /** Get the big step (<=0 means automatic mode; the value actually computed is returned)
    */
    int32_t GetPageStep() const;

    /** Set whether the tick marks are shown
    */
    void SetShowTicks(bool bShow);

    /** Get whether the tick marks are shown
    */
    bool IsShowTicks() const { return m_bShowTicks; }

    /** Set the number of tick marks (at least 2, including both ends)
    */
    void SetTickCount(int32_t nCount);

    /** Get the number of tick marks
    */
    int32_t GetTickCount() const { return m_nTickCount; }

    /** Set the tick mark length (in pixels; it is scaled by the current DPI internally)
    */
    void SetTickLength(int32_t nLength);

    /** Get the tick mark length
    */
    int32_t GetTickLength() const;

    /** Set the tick mark color (a semantic color name or a color value; text_muted is used when empty)
    */
    void SetTickColor(const std::string& strColor);

    /** Get the tick mark color
    */
    const std::string& GetTickColor() const { return m_strTickColor; }

    /** Set the tick mark position (use Bottom/Top for a horizontal slider and Right/Left for a vertical one)
    */
    void SetTickPos(SliderTickPos pos);

    /** Get the tick mark position
    */
    SliderTickPos GetTickPos() const { return m_tickPos; }

    /** Set whether the tick value texts are shown (show_ticks must be true)
    */
    void SetShowTickLabels(bool bShow);

    /** Get whether the tick value texts are shown
    */
    bool IsShowTickLabels() const { return m_bShowTickLabels; }

    /** Set the interval, in ticks, at which a tick value text is shown (minimum 1)
    */
    void SetTickLabelInterval(int32_t nInterval);

    /** Get the display interval of the tick value texts
    */
    int32_t GetTickLabelInterval() const { return m_nTickLabelInterval; }

    /** Set the font ID of the tick value texts
    */
    void SetTickFontId(const std::string& strFontId);

    /** Get the font ID of the tick value texts
    */
    const std::string& GetTickFontId() const { return m_strTickFontId; }

    /** Set the color of the tick value texts (a semantic color name or a color value; text_muted is used when empty)
    */
    void SetTickLabelColor(const std::string& strColor);

    /** Get the color of the tick value texts
    */
    const std::string& GetTickLabelColor() const { return m_strTickLabelColor; }

    /** Tick value text formatting callback: customizes the text shown on a tick label
    *   @param value     The progress value of that tick
    *   @param index     The tick index (starting from 0)
    *   @return The text to display (such as "5", "50%", "03:20")
    */
    typedef std::function<std::string(double value, int32_t index)> TickLabelCallback;

    /** Set the tick value text formatting callback
    * @param [in] callback The formatting callback; set it to null to restore the default value formatting
    */
    void SetTickLabelCallback(const TickLabelCallback& callback);

    /** DPI changed, update the control size and layout
    * @param [in] nOldDpiScale The old DPI scale percentage
    * @param [in] nNewDpiScale The new DPI scale percentage, consistent with the value of Dpi().GetScale()
    */
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;

    /** Get the step value
     * @return Returns the step value
     */
    int GetChangeStep();

    /** Set the step value
     * @param[in] step The step value to set
     */
    void SetChangeStep(int step);

    /** Set the size of the thumb
     * @param [in] szXY The size to set
     * @param [in] bNeedDpiScale Whether DPI scaling is needed
     */
    void SetThumbSize(UiSize szXY, bool bNeedDpiScale);

    /** Get the size of the thumb
    */
    UiSize GetThumbSize() const;

    /** Get the rect information of the thumb
     * @return Returns the rect information of the thumb
     */
    UiRect GetThumbRect() const;

    /** Get the image of the thumb in the specified state
     * @param[in] stateType The state identifier to get; refer to the ControlStateType enumeration
     * @return Returns the image path
     */
    std::string GetThumbStateImage(ControlStateType stateType) const;

    /** Set the image of the thumb in the specified state
     * @param[in] stateType The state identifier to set; refer to the ControlStateType enumeration
     * @param[in] pStrImage The image location to set
     */
    void SetThumbStateImage(ControlStateType stateType, const std::string& pStrImage);

    /** Get the progress bar padding
     */
    const UiPadding& GetProgressBarPadding() const;

    /** Set the progress bar padding
     * @param [in] padding The padding information to set
     * @param [in] bNeedDpiScale Whether DPI scaling is needed
     */
    void SetProgressBarPadding(UiPadding padding, bool bNeedDpiScale);

protected:
    /** Calculate the progress value corresponding to a mouse position (clamped)
    * @param [in] ptMouse The position in control coordinates
    * @return Returns the progress value for that position
    */
    double ValueFromMousePt(const UiPoint& ptMouse) const;

    /** Adjust the current value according to a keyboard key
    * @param [in] vkCode The virtual key code
    * @param [in] bShiftDown Whether the Shift key is held down (Shift + an arrow key = a big step)
    * @return Returns true when the key was handled
    */
    bool HandleKeyDown(uint32_t vkCode, bool bShiftDown);

    /** Adjust the current value by steps (clamped to [min, max] automatically)
    * @param [in] fDelta The increment (may be negative)
    * @param [in] nStepCount The number of steps (a negative value means the opposite direction)
    */
    void StepValue(double fDelta, int32_t nStepCount = 1);

    /** Paint the tick marks and the tick value texts
    */
    void PaintTicks(IRender* pRender);

    /** Paint a tick value text
    * @param [in] uAlign The alignment (a combination of TEXT_LEFT/TEXT_HCENTER/TEXT_RIGHT and
    *                    TEXT_TOP/TEXT_VCENTER/TEXT_BOTTOM)
    */
    void DrawTickText(IRender* pRender, double fValue, int32_t nIndex, const UiRect& rcLabel, uint32_t uAlign);

    /** Calculate the progress value of the tick with index nIndex
    */
    double GetTickValue(int32_t nIndex) const;

    int m_nStep;
    UiSize m_szThumb;
    StateImage m_thumbStateImage;
    UiPadding m_rcProgressBarPadding;
    UiString m_sImageModify;

private:
    //Whether the value jumps immediately when the track is pressed (true by default)
    bool m_bClickToJump;

    //Big step of PageUp/PageDown; <=0 means automatic
    int32_t m_nPageStep;

    //Tick mark related settings
    bool m_bShowTicks;
    int32_t m_nTickCount;
    int32_t m_nTickLength;      //Already DPI scaled
    std::string m_strTickColor;
    SliderTickPos m_tickPos;
    bool m_bShowTickLabels;
    int32_t m_nTickLabelInterval;
    std::string m_strTickFontId;
    std::string m_strTickLabelColor;
    TickLabelCallback m_tickLabelCallback;
};

}

#endif // UI_CONTROL_SLIDER_H_
