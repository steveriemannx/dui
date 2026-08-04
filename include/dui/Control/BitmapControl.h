#ifndef UI_CONTROL_BITMAP_CONTROL_H_
#define UI_CONTROL_BITMAP_CONTROL_H_

#include "dui/Core/Box.h"
#include <mutex>

namespace ui
{
class IBitmap;

/** Control for displaying bitmap data, suitable for displaying memory-based bitmap data, and supports various image resource attributes
 *  The data format of the bitmap is a 32-bit bitmap (processed externally):
 *    Windows platform is BGRA: struct BGRA { uint8_t b, g, r, a; };
 *    Non-Windows platform is RGBA: struct RGBA { uint8_t r, g, b, a; };
 *  The alpha type of the bitmap is kPremul_SkAlphaType (pre-multiplied externally)
 *  Thread model:
 *     (1) For functions related to bitmap data operations: multi-threaded is supported, bitmap data can be operated on in worker threads, and bitmap drawing is performed on the UI thread
 *     (2) For functions not related to bitmap data operations: can only be called on the UI thread, multi-threading is not supported
 */

/** Control for displaying bitmap data, suitable for displaying memory-based bitmap data, and supports various image resource attributes
 *  The data format of the bitmap is a 32-bit bitmap (processed externally):
 *    Windows platform is BGRA: struct BGRA { uint8_t b, g, r, a; };
 *    Non-Windows platform is RGBA: struct RGBA { uint8_t r, g, b, a; };
 *  The alpha type of the bitmap is kPremul_SkAlphaType (pre-multiplied externally)
 *  Thread model:
 *     (1) For functions related to bitmap data operations: multi-threaded is supported, bitmap data can be operated on in worker threads, and bitmap drawing is performed on the UI thread
 *     (2) For functions not related to bitmap data operations: can only be called on the UI thread, multi-threading is not supported
 */
class DUI_API BitmapControl : public Box
{
    typedef Box BaseClass;
public:
    explicit BitmapControl(Window* pWindow);
    virtual ~BitmapControl() override;
    BitmapControl(const BitmapControl&) = delete;
    BitmapControl& operator=(const BitmapControl&) = delete;

    /** Get the control type
    */
    virtual DString GetType() const override;
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;

    /** Calculate the size of the image area (width and height)
     *  @param [in] szAvailable The available size, excluding padding and margin
     *  @param [in] estImageType The type of image estimation
     *  @return The estimated image size of the control, including padding (Box), excluding margin
     */
    virtual UiSize EstimateImage(UiSize szAvailable, EstimateImageType estImageType) override;

    /** DPI change: update the control size and layout
    * @param [in] nOldDpiScale The old DPI scale percentage
    * @param [in] nNewDpiScale The new DPI scale percentage, consistent with the value of Dpi().GetScale()
    */
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;

public:
    /** Set bitmap data; the data will be copied and saved
     * @param [in] nWidth Width
     * @param [in] nHeight Height
     * @param [in] pPixelBits Bitmap data
     * @param [in] nPixelBitsSize The length of the bitmap data (in bytes)
     */
    bool SetBitmapData(int32_t nWidth, int32_t nHeight, const uint8_t* pPixelBits, int32_t nPixelBitsSize);

    /** Set bitmap data; the data will be copied and saved
     * @param [in] pBitmap The bitmap data interface
     */
    bool SetBitmapDataWithCopy(IBitmap* pBitmap);

    /** Clear the bitmap data
    */
    void ClearBitmapData();

    /** Get whether bitmap data is currently present
    */
    bool HasBitmapData();

public:
    /** Set the horizontal alignment type when drawing the bitmap image
    */
    void SetBitmapHAlignType(HorAlignType hAlignType);

    /** Get the horizontal alignment type when drawing the bitmap image
    */
    HorAlignType GetBitmapHAlignType() const;

    /** Set the vertical alignment type when drawing the bitmap image
    */
    void SetBitmapVAlignType(VerAlignType vAlignType);

    /** Get the vertical alignment type when drawing the bitmap image
    */
    VerAlignType GetBitmapVAlignType() const;

    /** Set the alpha when drawing the image (0 - 255)
    */
    void SetBitmapAlpha(uint8_t nBitmapAlpha);

    /** Get the alpha when drawing the image (0 - 255)
    */
    uint8_t GetBitmapAlpha() const;

    /** Set the position and size of the drawing destination area (relative to the control area)
    * @param [in] rcDest The position and size of the drawing destination area
    * @param [in] bNeedDpiScale Whether DPI scaling is needed
    */
    void SetBitmapDest(UiRect rcDest, bool bNeedDpiScale);

    /** Get the position and size of the drawing destination area (relative to the control area)
    * @return Returns the position and size of the drawing destination area, already DPI scaled
    */
    UiRect GetBitmapDest() const;

    /** Whether the position and size of the drawing destination area are set (relative to the control area)
    */
    bool HasBitmapDest() const;

    /** Set the position and size of the drawing source area
    * @param [in] rcSource The position and size of the drawing source area
    * @param [in] bNeedDpiScale Whether DPI scaling is needed
    */
    void SetBitmapSource(UiRect rcSource, bool bNeedDpiScale);

    /** Get the position and size of the drawing source area
    * @return Returns the position and size of the drawing source area, already DPI scaled
    */
    UiRect GetBitmapSource() const;

    /** Whether the position and size of the drawing source area are set
    */
    bool HasBitmapSource() const;

    /** Set the margin in the drawing destination area (invalid if rcDest is specified)
    * @param [in] rcMargin The position and size of the drawing source area
    * @param [in] bNeedDpiScale Whether DPI scaling is needed
    */
    void SetBitmapMargin(UiMargin rcMargin, bool bNeedDpiScale);

    /** Get the margin in the drawing destination area (invalid if rcDest is specified)
    * @return Returns the position and size of the drawing destination area, already DPI scaled
    */
    UiMargin GetBitmapMargin() const;

    /** Whether the margin in the drawing destination area is set (invalid if rcDest is specified)
    */
    bool HasBitmapMargin() const;

    /** Set whether to automatically adapt to the destination area when drawing (scale the image proportionally)
    * @param [in] bAdaptiveDestRect true means automatically adapt to the destination area when drawing (scale the image proportionally)
    */
    void SetAdaptiveDestRect(bool bAdaptiveDestRect);

    /** Get whether to automatically adapt to the destination area when drawing (scale the image proportionally)
    */
    bool IsAdaptiveDestRect() const;

    /** Set whether to stretch the image when drawing (mutually exclusive with IsAdaptiveDestRect(), with lower priority than IsAdaptiveDestRect())
    * @param [in] bStretchedDrawing true means stretch the image when drawing, false means do not stretch the image when drawing
    */
    void SetStretchedDrawing(bool bStretchedDrawing);

    /** Get whether to stretch the image when drawing
    */
    bool IsStretchedDrawing() const;

    /** Set whether multi-threaded operations on bitmap data are supported (if not called, the default is true; multi-threaded bitmap data operations are supported by default)
    * @param [in] bSupportMultiThread true means multi-threaded bitmap data operations are supported, false means they are not supported
    */
    void SetSupportMultiThread(bool bSupportMultiThread);

    /** Get whether multi-threaded operations on bitmap data are supported
    */
    bool IsSupportMultiThread() const;

protected:
    /** Override the parent control drawing function
    */
    virtual void Paint(IRender* pRender, const UiRect& rcPaint) override;

private:
    /** Get the width and height of the bitmap image
    */
    void GetBitmapSize(int32_t& nImageWidth, int32_t& nImageHeight);

    /** Draw the image
    */
    void PaintBitmap(IRender* pRender, const UiRect& rcPaint);

    /** Load the specified image
    */
    void CheckLoadBitmapFile();

private:
    // Bitmap data lock, supports operating on bitmap data in worker threads
    std::mutex m_bitmapMutex;

    // Bitmap data used for drawing
    std::unique_ptr<IBitmap> m_pBitmap;

    // The associated image file: mainly used for testing
    UiString m_bitmapFile;

    // Position and size of the drawing destination area (relative to the control area)
    std::unique_ptr<UiRect> m_rcDest;

    // Position and size of the image source area
    std::unique_ptr<UiRect> m_rcSource;

    // Margin in the drawing destination area (invalid if rcDest is specified)
    std::unique_ptr<UiMargin> m_rcMargin;

    // Horizontal alignment type of the image
    HorAlignType m_hAlignType;

    // Vertical alignment type of the image
    VerAlignType m_vAlignType;

    // Alpha (0 - 255)
    uint8_t m_nBitmapAlpha;

    // Whether to automatically adapt to the destination area (scale the image proportionally)
    bool m_bAdaptiveDestRect;

    // Whether to stretch the image when drawing
    bool m_bStretchedDrawing;

    // Whether multi-threaded operations on bitmap data are supported
    bool m_bSupportMultiThread;
};

}//namespace ui

#endif //UI_CONTROL_BITMAP_CONTROL_H_
