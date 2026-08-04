#ifndef UI_CONTROL_COLOR_SLIDER_H_
#define UI_CONTROL_COLOR_SLIDER_H_

#include "dui/Control/Slider.h"

namespace ui
{
/** Color type
*/
enum class ColorMode
{
    kMode_ARGB,    //ARGB
    kMode_HSV,    //HSV
    kMode_HSL    //HSL
};

/** Color adjustment mode
*/
enum class ColorAdjustMode
{
    kMode_ARGB_A, //ARGB mode, adjust A
    kMode_ARGB_R, //ARGB mode, adjust R
    kMode_ARGB_G, //ARGB mode, adjust G
    kMode_ARGB_B, //ARGB mode, adjust B
    kMode_HSV_H,  //HSV mode, adjust H
    kMode_HSV_S,  //HSV mode, adjust S
    kMode_HSV_V,  //HSV mode, adjust V
    kMode_HSL_H,  //HSL mode, adjust H
    kMode_HSL_S,  //HSL mode, adjust S
    kMode_HSL_L   //HSL mode, adjust L
};

/** HSV color
*/
class ColorHSV
{
public:
    uint16_t H = 0; //Range: [0, 359)
    uint8_t  S = 0; //Range: [0, 100]
    uint8_t  V = 0; //Range: [0, 100]
};

/** HSL color
*/
class ColorHSL
{
public:
    uint16_t H = 0; //Range: [0, 359)
    uint8_t  S = 0; //Range: [0, 100]
    uint8_t  L = 0; //Range: [0, 100]
};

/** Slider control with a custom color background
*/
class DUI_API ColorSlider: public Slider
{
    typedef Slider BaseClass;
public:
    explicit ColorSlider(Window* pWindow);

    /** Get the control type
    */
    virtual DString GetType() const override;

    /** Set the color information (color in ARGB format)
    */
    void SetColorInfo(const UiColor& color, ColorAdjustMode adjustMode);

    /** Set the color information (color in HSV format)
    */
    void SetColorInfo(const ColorHSV& color, ColorAdjustMode adjustMode);

    /** Set the color information (color in HSL format)
    */
    void SetColorInfo(const ColorHSL& color, ColorAdjustMode adjustMode);

    /** The entry function for drawing the background image
    * @param[in] pRender Specifies the drawing area
    */
    virtual void PaintBkImage(IRender* pRender) override;

private:

    /** Get the interface of the drawn color bitmap
    * @param [in] rect The display area size information
    */
    IBitmap* GetColorBitmap(const UiRect& rect);

    /** Get the gradient color in ARGB format
    */
    void GetARGB(uint32_t* buffer, int32_t samples,
                 const UiColor& start,
                 const UiColor& end) const;

private:
    /** The color bitmap
    */
    std::unique_ptr<IBitmap> m_spBitmap;

    /** The color mode
    */
    ColorMode m_colorMode;

    /** The ARGB color value
    */
    UiColor m_argbColor;

    /** The HSV color value
    */
    ColorHSV m_hsvColor;

    /** The HSL color value
    */
    ColorHSL m_hslColor;

    /** The color adjustment mode
    */
    ColorAdjustMode m_adjustMode;
};

}//namespace ui

#endif //UI_CONTROL_COLOR_SLIDER_H_
