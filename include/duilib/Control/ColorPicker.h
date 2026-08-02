#ifndef UI_CONTROL_COLORPICKER_H_
#define UI_CONTROL_COLORPICKER_H_

#include "duilib/Utils/WinImplBase.h"
#include "duilib/Control/Label.h"

namespace ui
{

/** Color picker, an independent window
*/
class Control;
class ColorPickerRegular;
class ColorPickerStatard;
class ColorPickerStatardGray;
class ColorPickerCustom;
class DUILIB_API ColorPicker : public WindowImplBase
{
    typedef WindowImplBase BaseClass;
public:
    ColorPicker();
    virtual ~ColorPicker() override;

    /** The following three interfaces must be overridden; the parent class calls these interfaces to build the window
     * GetSkinFolder        The interface sets the window skin resource path to be drawn
     * GetSkinFile            The interface sets the xml description file of the window to be drawn
     */
    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;

public:
    /** Set the selected color
    */
    void SetSelectedColor(const UiColor& color);

    /** Get the finally selected color when the window is closed
    */
    UiColor GetSelectedColor() const;

    /** Listen to the color selection event
    * @param[in] callback The callback function triggered when the selected color changes
    *            Parameter description:
                        wParam: The currently newly selected color value; a color can be generated with UiColor((uint32_t)wParam)
                        lParam: The previously selected old color value; a color can be generated with UiColor((uint32_t)lParam)
    */
    void AttachSelectColor(const EventCallback& callback);

    /** Listen to the window close event
     * @param[in] callback Specifies the callback function after closing; in this callback, the GetSelectedColor() function can be called to get the selected color value
                           The wParam of the parameter indicates how the window close was triggered:
     *                      0 - means closing this window via "OK"
                           1 - means closing this window by clicking the "Close" button of the window (default)
                           2 - means closing this window via "Cancel"
     */
    void AttachWindowClose(const EventCallback& callback);

protected:
    /** Called when the control to be created is not a standard control name
    * @param [in] strClass The control name
    * @return Returns a custom control pointer; in general, create a custom control according to the strClass parameter
    */
    virtual Control* CreateControl(const DString& strClass) override;

private:
    /** Called after the window is created, for subclasses to do some initialization work
    */
    virtual void OnInitWindow() override;

    /** Window close message (WM_CLOSE)
    * @param [in] wParam The wParam parameter of the message
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnWindowCloseMsg(uint32_t wParam, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** A new color was selected internally
    */
    void OnSelectColor(const UiColor& newColor);

    /** Pick a color from the screen
    */
    void OnPickColorFromScreen();

private:
    /** The interface of the newly selected color control
    */
    Label* m_pNewColor;

    /** The interface of the original color control
    */
    Label* m_pOldColor;

    /** The interface of the regular color control
    */
    ColorPickerRegular* m_pRegularPicker;

    /** The interface of the standard color control
    */
    ColorPickerStatard* m_pStatardPicker;

    /** The interface of the standard color control (gray)
    */
    ColorPickerStatardGray* m_pStatardGrayPicker;

    /** The interface of the custom color control
    */
    ColorPickerCustom* m_pCustomPicker;

    /** The selected color
    */
    UiColor m_selectedColor;

    /** The listening event of the color selection
    */
    EventCallback m_colorCallback;
};

} // namespace ui

#endif //UI_CONTROL_COLORPICKER_H_
