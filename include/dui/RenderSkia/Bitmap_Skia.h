#ifndef UI_RENDER_SKIA_BITMAP_H_
#define UI_RENDER_SKIA_BITMAP_H_

#include "dui/Render/IRender.h"

//Forward declarations of Skia-related classes
class SkBitmap;

namespace ui
{
/** Bitmap implementation: Skia drawing engine
*/
class Bitmap_Skia: public IBitmap
{
public:
    Bitmap_Skia();
    virtual ~Bitmap_Skia() override;

public:
    /** Initialize from data (ARGB format)
    @param [in] nWidth Width
    @param [in] nHeight Height
    @param [in] pPixelBits Bitmap data; nullptr means an empty bitmap for the window, otherwise the data length is: nHeight * nWidth * 4
    @param [in] fImageSizeScale The image scale factor; 1.0f means the original value
    @param [in] alphaType The Alpha type of the bitmap; only the Skia engine needs this parameter
    */
    virtual bool Init(uint32_t nWidth, uint32_t nHeight,
                      const void* pPixelBits, float fImageSizeScale = 1.0f,
                      BitmapAlphaType alphaType = BitmapAlphaType::kPremul_SkAlphaType) override;

    /** Get the image width
    */
    virtual uint32_t GetWidth() const override;

    /** Get the image height
    */
    virtual uint32_t GetHeight() const override;

    /** Get the image size
    @return The image size
    */
    virtual UiSize GetSize() const override;

    /** Lock the bitmap data; the data length is GetWidth() * GetHeight() * 4
    */
    virtual void* LockPixelBits() override;

    /** Release the bitmap data
    */
    virtual void UnLockPixelBits() override;

    /** Clone to create a new bitmap
    *@return Returns the newly created bitmap interface; the caller releases the resources
    */
    virtual IBitmap* Clone() override;

public:
    /** Get the Skia bitmap
    */
    const SkBitmap& GetSkBitmap() const;

private:
    /** Update the transparent channel flag of the image
    */
    void UpdateAlphaFlag(uint8_t* pPixelBits);

    /** Perform data initialization
    */
    bool InitImage(uint32_t nWidth, uint32_t nHeight, const void* pPixelBits, BitmapAlphaType alphaType);

private:
    /** The Skia bitmap
    */
    std::unique_ptr<SkBitmap> m_pSkBitmap;
};

} // namespace ui

#endif // UI_RENDER_SKIA_BITMAP_H_
