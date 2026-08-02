/** @file CircleProgress.h
* @brief Circular progress bar control; text can be placed in the middle of the ring (e.g. 85%)
* @copyright (c) 2019-2022, NetEase Inc. All rights reserved
* @author Xuhuajie
* @date 2019/8/14
*/

#ifndef UI_CONTROL_CIRCLEPROGRESS_H_
#define UI_CONTROL_CIRCLEPROGRESS_H_

#include "duilib/Control/Progress.h"

namespace ui
{

class DUILIB_API CircleProgress : public Progress
{
    typedef Progress BaseClass;
public:
    explicit CircleProgress(Window* pWindow);
    virtual ~CircleProgress() override;

    /// Override the parent class methods to provide personalized features; please refer to the parent class declarations
    virtual DString GetType() const override;
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;
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
    void SetBackgroudColor(const DString& strColor);

    /** Set the foreground color of the progress bar
    * @param [in] strColor The foreground color string to set; this string must exist in global.xml
    */
    void SetForegroudColor(const DString& strColor);

    /** Set the foreground gradient color of the progress bar; used together with SetForegroudColor; it can be left unset, in which case there is no gradient effect
    * @param [in] strColor The foreground gradient color string to set; this string must exist in global.xml
    */
    void SetCircleGradientColor(const DString& strColor);

    /** Set the progress indicator moving icon
    * @param [in] sIndicatorImage The image to set
    */
    void SetIndicator(const DString& sIndicatorImage);

protected:
    bool m_bCircular;
    bool m_bClockwise;
    float m_fCircleWidth;
    UiColor m_dwBackgroundColor;
    UiColor m_dwForegroundColor;
    UiColor m_dwGradientColor;
    Image* m_pIndicatorImage;
};

} // namespace ui

#endif // UI_CONTROL_CIRCLEPROGRESS_H_
