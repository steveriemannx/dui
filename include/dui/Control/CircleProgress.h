/** @file CircleProgress.h
* @brief Circular progress bar control; text can be placed in the middle of the ring (e.g. 85%)
* @copyright (c) 2019-2022, NetEase Inc. All rights reserved
* @author Xuhuajie
* @date 2019/8/14
*/

#ifndef UI_CONTROL_CIRCLEPROGRESS_H_
#define UI_CONTROL_CIRCLEPROGRESS_H_

#include "dui/Control/Progress.h"
#include <functional>

namespace ui
{

/** Callback used to paint the center content area
*   @param pRender The render interface
*   @param rcCenter The rectangle of the center content area (relative to the control, already shrunk
*                   according to center_ratio)
*   @param fValue The current progress value
*   @return Returns true when the callback painted the content itself (the default text painting is skipped)
*/
typedef std::function<bool(IRender* pRender, const UiRect& rcCenter, double fValue)> CircleCenterCallback;

class DUI_API CircleProgress : public Progress
{
    typedef Progress BaseClass;
public:
    explicit CircleProgress(Window* pWindow);
    virtual ~CircleProgress() override;

    /// Override the parent class methods to provide personalized features; please refer to the parent class declarations
    virtual std::string GetType() const override;
    virtual void SetAttribute(const std::string& strName, const std::string& strValue) override;
    virtual void PaintStateImages(IRender* pRender) override;
    virtual void ClearImageCache() override;

    /** DPI change: update the control size and layout
    * @param [in] nOldDpiScale The old DPI scale percentage
    * @param [in] nNewDpiScale The new DPI scale percentage, consistent with the value of Dpi().GetScale()
    */
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;

    /** Set the circular progress bar; the default is the normal progress bar
    * @param [in] bCircular true sets the circular progress bar, false sets the parent progress bar style; the default is true
    */
    void SetCircular(bool bCircular = true);

    /** Set the direction of increase
    * @param [in] bClockwise true sets clockwise, false sets counterclockwise; the default is true
    */
    void SetClockwiseRotation(bool bClockwise = true);

    /** Set the ring width
    * @param [in] fCircleWidth The width value
    * @param [in] bNeedDpiScale Whether DPI scaling is supported
    */
    void SetCircleWidth(float fCircleWidth, bool bNeedDpiScale);

    /** Get the ring width
    */
    float GetCircleWidth() const;

    /** Set the background color of the progress bar
    * @param [in] strColor The background color string to set; this string must exist in global.xml
    */
    void SetBackgroundColor(const std::string& strColor);

    /** Set the foreground color of the progress bar
    * @param [in] strColor The foreground color string to set; this string must exist in global.xml
    */
    void SetForegroundColor(const std::string& strColor);

    /** Set the foreground gradient color of the progress bar; used together with SetForegroundColor; it can be left unset, in which case there is no gradient effect
     * @param [in] strColor The foreground gradient color string to set; this string must exist in global.xml
    */
    void SetCircleGradientColor(const std::string& strColor);

    /** Set the progress indicator moving icon
    * @param [in] sIndicatorImage The image to set
    */
    void SetIndicator(const std::string& sIndicatorImage);

    /** Set whether this is an indeterminate progress (indeterminate: the value is ignored and a
    * segment of the ring keeps rotating)
    * @param [in] bIndeterminate true enables the indeterminate progress mode
    */
    void SetIndeterminate(bool bIndeterminate);

    /** Get whether this is an indeterminate progress
    */
    bool IsIndeterminate() const { return m_bIndeterminate; }

    /** Set the start angle of the arc (in degrees)
    * @param [in] fAngle The start angle; 0 means the 3 o'clock direction and it increases clockwise;
    *                    the default of 270 means the 12 o'clock direction
    */
    void SetStartAngle(float fAngle);

    /** Get the start angle of the arc (in degrees)
    */
    float GetStartAngle() const { return m_fStartAngle; }

    /** Set the total span angle of the arc (in degrees), i.e. the angle swept between value = min and
    * value = max
    * @param [in] fSpan The span angle, in the range (0, 360]; 360 is a full circle and 180 is a half-circle dial
    */
    void SetSpanAngle(float fSpan);

    /** Get the total span angle of the arc (in degrees)
    */
    float GetSpanAngle() const { return m_fSpanAngle; }

    /** Set the angle of the rotating arc in indeterminate progress mode
    * @param [in] fArc The arc length (in degrees), in the range (0, 360]; 90 by default
    */
    void SetIndeterminateArc(float fArc);

    /** Get the angle of the rotating arc in indeterminate progress mode
    */
    float GetIndeterminateArc() const { return m_fIndeterminateArc; }

    /** Set the rotation angle per frame in indeterminate progress mode
    * @param [in] fSpeed The angle per frame (in degrees); 6 by default
    */
    void SetIndeterminateSpeed(float fSpeed);

    /** Get the rotation angle per frame in indeterminate progress mode
    */
    float GetIndeterminateSpeed() const { return m_fIndeterminateSpeed; }

    /** Set the refresh interval of the indeterminate progress animation (milliseconds, 16 by default)
    */
    void SetIndeterminateInterval(int32_t nInterval);

    /** Get the refresh interval of the indeterminate progress animation (milliseconds)
    */
    int32_t GetIndeterminateInterval() const { return m_nIndeterminateInterval; }

    /** Set the ratio of the center content area to the control size (0~1; the default of 1.0 means the
    * inscribed circle of the whole control)
    * @param [in] fRatio The ratio, used to leave a margin for the ring or to place a dashboard needle
    */
    void SetCenterRatio(float fRatio);

    /** Get the ratio of the center content area to the control size
    */
    float GetCenterRatio() const { return m_fCenterRatio; }

    /** Set the padding of the center content area (in pixels, DPI scaled); the format is
    * "left,top,right,bottom"
    */
    void SetCenterPadding(const UiPadding& padding, bool bNeedDpiScale = true);

    /** Get the padding of the center content area
    */
    const UiPadding& GetCenterPadding() const { return m_rcCenterPadding; }

    /** Set a custom paint callback for the center content area (for an icon, an image, a needle, ...)
    * @param [in] callback The paint callback; set it to null to restore the default text painting
    */
    void SetCenterCallback(const CircleCenterCallback& callback);

    /** Get the rectangle of the center content area (relative to the control)
    */
    UiRect GetCenterRect() const;

private:
    /** Timer callback of the indeterminate progress (rotating arc)
    */
    void OnIndeterminateTimer();

    /** Paint the center content (the text or the result of the custom callback)
    */
    void PaintCenterContent(IRender* pRender, double fValue);

protected:
    bool m_bCircular;
    bool m_bClockwise;
    float m_fCircleWidth;
    UiColor m_dwBackgroundColor;
    UiColor m_dwForegroundColor;
    UiColor m_dwGradientColor;
    Image* m_pIndicatorImage;

    /** Whether this is an indeterminate progress
    */
    bool m_bIndeterminate;

    /** Current rotation angle of the indeterminate progress (in degrees)
    */
    float m_fIndeterminateAngle;

    /** Cancellation mechanism of the indeterminate progress timer
    */
    WeakCallbackFlag m_indeterminateTimer;

    /** Start angle of the arc (in degrees); the default of 270 is the 12 o'clock direction
    */
    float m_fStartAngle;

    /** Total span angle of the arc (in degrees); the default of 360 is a full circle
    */
    float m_fSpanAngle;

    /** Arc length of the indeterminate progress (in degrees); 90 by default
    */
    float m_fIndeterminateArc;

    /** Rotation angle per frame of the indeterminate progress (in degrees); 6 by default
    */
    float m_fIndeterminateSpeed;

    /** Refresh interval of the indeterminate progress (milliseconds); 16 by default
    */
    int32_t m_nIndeterminateInterval;

    /** Ratio of the center content area to the control size
    */
    float m_fCenterRatio;

    /** Padding of the center content area
    */
    UiPadding m_rcCenterPadding;

    /** Custom paint callback of the center content area
    */
    CircleCenterCallback m_centerCallback;
};

} // namespace ui

#endif // UI_CONTROL_CIRCLEPROGRESS_H_
