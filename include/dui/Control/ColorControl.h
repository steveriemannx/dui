#ifndef UI_CONTROL_COLOR_CONTROL_H_
#define UI_CONTROL_COLOR_CONTROL_H_

#include "dui/Core/Control.h"
#include "dui/Render/IRender.h"

namespace ui
{
/** Custom color control
*/
class DUI_API ColorControl: public Control
{
    typedef Control BaseClass;
public:
    explicit ColorControl(Window* pWindow);

    /** Get the control type
    */
    virtual DString GetType() const override;

    /** Select a color
    */
    void SelectColor(const UiColor& selColor);

    /** Listen to the color selection event
    * @param [in] callback The callback function triggered when the selected color changes
    * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
    *            Parameter description:
    *                   wParam: The currently newly selected color value; a color can be generated with UiColor((uint32_t)wParam)
    *                   lParam: The previously selected old color value; a color can be generated with UiColor((uint32_t)lParam)
    */
    void AttachSelectColor(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventSelectColor, callback, callbackID); }

protected:
    /** The entry function for drawing the background image
    * @param[in] pRender Specifies the drawing area
    */
    virtual void PaintBkImage(IRender* pRender) override;

    /** Mouse left button down
    */
    virtual bool ButtonDown(const EventArgs& msg) override;

    /** Mouse move
    */
    virtual bool MouseMove(const EventArgs& msg) override;

    /** Mouse left button up
    */
    virtual bool ButtonUp(const EventArgs& msg) override;

private:
    /** Get the interface of the drawn color bitmap
    * @param [in] rect The display area size information
    */
    IBitmap* GetColorBitmap(const UiRect& rect);

    /** The selected position changed
    */
    void OnSelectPosChanged(const UiRect& rect, const UiPoint& pt);

    /** Set the mouse capture
    */
    void SetMouseCapture(bool bCapture);

private:
    /** The color bitmap
    */
    std::unique_ptr<IBitmap> m_spBitmap;

    /** The mouse click position
    */
    UiPoint m_lastPt;

    /** Whether the mouse is pressed down
    */
    bool m_bMouseDown;
};

}//namespace ui

#endif //UI_CONTROL_COLOR_CONTROL_H_
