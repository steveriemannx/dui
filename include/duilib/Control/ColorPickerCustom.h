#ifndef UI_CONTROL_COLORPICKER_CUSTOM_H_
#define UI_CONTROL_COLORPICKER_CUSTOM_H_

#include "duilib/Core/Box.h"
#include "duilib/Control/ColorControl.h"
#include "duilib/Control/ColorSlider.h"
#include "duilib/Control/RichEdit.h"

namespace ui
{
/** Color picker: custom colors
*/
class RichEdit;
class ColorPickerRegular;
class DUILIB_API ColorPickerCustom : public Box
{
    typedef Box BaseClass;
public:
    explicit ColorPickerCustom(Window* pWindow);

    /** Get the control type
    */
    virtual DString GetType() const override;

    /** Select a color
    */
    void SelectColor(const UiColor& color);

    /** Set the control position
    */
    virtual void SetPos(UiRect rc) override;

    /** Listen to the color selection event
    * @param [in] callback The callback function triggered when the selected color changes
    * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
    * Parameter description:
    *    wParam: The currently newly selected color value; a color can be generated with UiColor((uint32_t)wParam)
    *    lParam: The previously selected old color value; a color can be generated with UiColor((uint32_t)lParam)
    */
    void AttachSelectColor(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventSelectColor, callback, callbackID); }

private:
    /** The reason for the color change
    */
    enum class ChangeReason
    {
        ColorUpdate,    //The color value is updated from another page
        ColorSpectrum,    //The color spectrum
        ColorRegular,    //The standard color table
        NewColorEdit,    //The edit box of the new color

        ColorARGB_A,    //ARGB color change
        ColorARGB_R,    //ARGB color change
        ColorARGB_G,    //ARGB color change
        ColorARGB_B,    //ARGB color change

        ColorHSV_H,        //HSV color change
        ColorHSV_S,        //HSV color change
        ColorHSV_V,        //HSV color change

        ColorHSL_H,        //HSL color change
        ColorHSL_S,        //HSL color change
        ColorHSL_L,        //HSL color change
    };

    /** Initialize
    */
    void InitPicker();

    /** The color changed
    */
    void OnColorChanged(WPARAM wParam, LPARAM lParam, ChangeReason reason);

    /** Judge whether the input is a legal color string, e.g. "#FF123456"
    */
    bool IsValidColorString(const DString& colorText) const;

private:
    /** UI control for one color channel
    */
    struct ColorUI
    {
        //The color value display control
        RichEdit* m_pColorEdit = nullptr;

        //The color value adjustment control
        ColorSlider* m_pColorSlider = nullptr;
    };

    /** Initialize an RGB channel
    */
    void InitRGB(const ColorUI& colorUI, ChangeReason reason);

    /** Initialize an HSV channel
    */
    void InitHSV(const ColorUI& colorUI, int32_t maxValue, ChangeReason reason);

    /** Initialize an HSL channel
    */
    void InitHSL(const ColorUI& colorUI, int32_t maxValue, ChangeReason reason);

    /** Update RGB
    * @param [in] flag: 0 - A, 1 - R, 2 - G, 3 - B
    */
    void UpdateRGB(const ColorUI& colorUI, const UiColor& color, int32_t flag);

    /** Update HSV
    */
    void UpdateHSV(const ColorUI& colorUIH, const ColorUI& colorUIS, const ColorUI& colorUIV, const UiColor& color, ChangeReason reason);

    /** Update HSL
    */
    void UpdateHSL(const ColorUI& colorUIH, const ColorUI& colorUIS, const ColorUI& colorUIL, const UiColor& color, ChangeReason reason);

    /** The RGB color changed
    */
    void OnRGBChanged(ChangeReason reason);

    /** The HSV color changed
    */
    void OnHSVChanged(ChangeReason reason);

    /** The HSL color changed
    */
    void OnHSLChanged(ChangeReason reason);

private:
    /** Whether the initialization has been completed
    */
    bool m_bPickerInited;

    /** The old color
    */
    UiColor m_oldColor;

    /** The interface of the standard color control
    */
    ColorPickerRegular* m_pRegularPicker;

    /** The spectrum control
    */
    ColorControl* m_pSpectrumControl;

    /** The text box interface of the newly selected color control
    */
    RichEdit* m_pNewColorEdit;

private:
    /** The RGB color adjustment control
    */
    ColorUI m_rgbA;
    ColorUI m_rgbR;
    ColorUI m_rgbG;
    ColorUI m_rgbB;

    /** The HSV color adjustment control
    */
    ColorUI m_hsvH;
    ColorUI m_hsvS;
    ColorUI m_hsvV;

    /** The HSL color adjustment control
    */
    ColorUI m_hslH;
    ColorUI m_hslS;
    ColorUI m_hslL;
};

}//namespace ui

#endif //UI_CONTROL_COLORPICKER_CUSTOM_H_
