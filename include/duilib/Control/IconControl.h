#ifndef UI_CONTROL_ICON_CONTROL_H_
#define UI_CONTROL_ICON_CONTROL_H_

#include "duilib/Core/Control.h"

namespace ui
{
class IBitmap;

/** A control for displaying icons, suitable for small icon image resources; its functionality is relatively simple
 *  The bitmap data format is a 32-bit bitmap (processed externally):
 *    Windows platform: BGRA: struct BGRA { uint8_t b, g, r, a; };
 *    Non-Windows platforms: RGBA: struct RGBA { uint8_t r, g, b, a; };
 *  The bitmap alpha type is kPremul_SkAlphaType (externally premultiplied)
 *  Thread model: single-threaded, can only be used on the UI thread
 */
class DUILIB_API IconControl : public Control
{
    typedef Control BaseClass;
public:
    explicit IconControl(Window* pWindow);
    virtual ~IconControl() override;
    IconControl(const IconControl&) = delete;
    IconControl& operator=(const IconControl&) = delete;

    /** Get the control type
    */
    virtual DString GetType() const override;

public:
    /** Set the icon bitmap data
    * @param [in] nWidth The width
    * @param [in] nHeight The height
    * @param [in] pPixelBits The bitmap data
    * @param [in] nPixelBitsSize The length of the bitmap data (in bytes)
    */
    bool SetIconData(int32_t nWidth, int32_t nHeight, const uint8_t* pPixelBits, int32_t nPixelBitsSize);

    /** Clear the icon data
    */
    void ClearIconData();

    /** Get whether icon data is currently available
    */
    bool HasIconData() const;

protected:
    /** Override the parent control's draw function
    */
    virtual void Paint(IRender* pRender, const UiRect& rcPaint) override;

private:
    /** The bitmap data used for drawing
    */
    std::unique_ptr<IBitmap> m_pBitmap;
};

}//namespace ui

#endif //UI_CONTROL_ICON_CONTROL_H_
