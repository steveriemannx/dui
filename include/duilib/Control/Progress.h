#ifndef UI_CONTROL_PROGRESS_H_
#define UI_CONTROL_PROGRESS_H_

#include "duilib/Control/Label.h"
#include "duilib/Image/Image.h"

namespace ui
{

/** Progress bar control
*/
class DUILIB_API Progress : public LabelTemplate<Control>
{
    typedef LabelTemplate<Control> BaseClass;
public:
    explicit Progress(Window* pWindow);
    virtual ~Progress() override;

    /// Override parent class methods to provide personalized functionality; please refer to the parent class declaration
    virtual DString GetType() const override;
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;
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
    DString GetProgressImage() const;

    /** Set the progress bar background image
     * @param [in] strImage The image address
     */
    void SetProgressImage(const DString& strImage);

    /** Get the progress bar background color
     * @return Returns the string value of the background color, corresponding to the specified color value in global.xml
     */
    DString GetProgressColor() const;

    /** Set the progress bar background color
     * @param [in] The background color string to set; the string must exist in global.xml
     */
    void SetProgressColor(const DString& strProgressColor);

    /** Get the progress bar position
    * @return Returns the progress bar area coordinates (relative coordinates with GetRect() as the whole area, with the top-left corner being <0,0>)
    */
    virtual UiRect GetProgressPos();

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

private:
    //Whether to count down (progress from 100 to 0)
    bool m_bReverse;

    //Timer cancellation mechanism
    WeakCallbackFlag m_timer;
};

} // namespace ui

#endif // UI_CONTROL_PROGRESS_H_
