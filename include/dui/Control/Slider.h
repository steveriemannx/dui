#ifndef UI_CONTROL_SLIDER_H_
#define UI_CONTROL_SLIDER_H_

#include "dui/Control/Progress.h"
#include <string>

namespace ui
{

/** Slider control
*/
class DUI_API Slider : public Progress
{
    typedef Progress BaseClass;
public:
    explicit Slider(Window* pWindow);

    /// Override the parent class method to provide personalized functionality; please refer to the parent class declaration
    virtual DString GetType() const override;
    virtual UiRect GetProgressPos() override;
    virtual void HandleEvent(const EventArgs& msg) override;
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;
    virtual void PaintStateImages(IRender* pRender) override;
    virtual void PaintBkColor(IRender* pRender) override;
    virtual void ClearImageCache() override;

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
    DString GetThumbStateImage(ControlStateType stateType) const;

    /** Set the image of the thumb in the specified state
     * @param[in] stateType The state identifier to set; refer to the ControlStateType enumeration
     * @param[in] pStrImage The image location to set
     */
    void SetThumbStateImage(ControlStateType stateType, const DString& pStrImage);

    /** Get the progress bar padding
     */
    const UiPadding& GetProgressBarPadding() const;

    /** Set the progress bar padding
     * @param [in] padding The padding information to set
     * @param [in] bNeedDpiScale Whether DPI scaling is needed
     */
    void SetProgressBarPadding(UiPadding padding, bool bNeedDpiScale);

    /** Listen for the progress change event of the progress bar
     * @param [in] callback The callback function called after the progress of the progress bar changes
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachValueChanged(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventValueChanged, callback, callbackID);    }

protected:
    int m_nStep;
    UiSize m_szThumb;
    StateImage m_thumbStateImage;
    UiPadding m_rcProgressBarPadding;
    UiString m_sImageModify;
};

}

#endif // UI_CONTROL_SLIDER_H_
