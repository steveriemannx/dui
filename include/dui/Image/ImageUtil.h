#ifndef UI_IMAGE_IMAGE_UTIL_H_
#define UI_IMAGE_IMAGE_UTIL_H_

#include "dui/Core/UiTypes.h"

namespace ui
{
/** Bitmap interface
*/
class IBitmap;

/** SVG vector image interface
*/
class DUI_API ImageUtil
{
public:
    /** Determine whether the image needs to be resized at this image scaling ratio
    * @param [in] fImageSizeScale The scaling ratio of the image, 1.0f means the original value
    */
    static bool NeedResizeImage(float fImageSizeScale);

    /** Determine whether the scaling ratios of two images are the same
    */
    static bool IsSameImageScale(float fImageSizeScale1, float fImageSizeScale2);

    /** Determine whether a scaling ratio is valid
    */
    static bool IsValidImageScale(float fImageSizeScale);

    /** Calculate the image size after proportional scaling (scaled proportionally)
    * @param [in] nImageSize The size of the original image, width or height
    * @param [in] fImageSizeScale The scaling ratio of the image, 1.0f means the original value
    */
    static uint32_t GetScaledImageSize(uint32_t nImageSize, float fImageSizeScale);

    /** Calculate the offset after proportional scaling (scaled proportionally)
    * @param [in] nImageOffset The position offset of the original image
    * @param [in] fImageSizeScale The scaling ratio of the image, 1.0f means the original value
    */
    static int32_t GetScaledImageOffset(int32_t nImageOffset, float fImageSizeScale);

    /** Calculate the image size after proportional scaling (scaled according to the DPI display percentage)
    * @param [in] nImageSize The size of the original image, width or height
    * @param [in] nNewDpiScale The new DPI scaling percentage, 100 is the original value, 200 means scaling to 200%
    * @param [in] nOrgDpiScale The DPI scaling percentage corresponding to the nImageSize value
    */
    static uint32_t GetScaledImageSize(uint32_t nImageSize, uint32_t nNewDpiScale, uint32_t nOrgDpiScale = 100);

    /** Get the best loading scaling ratio
    * @param [in] rcMaxDestRectSize The drawing destination size
    * @param [in] nImageWidth The width of the original image
    * @param [in] nImageHeight The height of the original image
    * @param [in] fMaxScale The maximum ratio (to avoid occupying too much memory)
    * @param [out] fScale Returns the best loading scaling ratio
    */
    static bool GetBestImageScale(const UiSize& rcMaxDestRectSize,
                                  int32_t nImageWidth,
                                  int32_t nImageHeight,
                                  float fMaxScale,
                                  float& fScale);

    /** Flip the image vertically (upside down) for a 32-bit pixel format image (RGBA/ARGB, etc., 4 bytes/pixel)
    * @param [in] pPixelBits The start address of the image data (must be writable)
    * @param [in] nPixelBitsLen The total number of bytes of the pixel data (must equal nHeight * nWidth * 4)
    * @param [in] nWidth The width of the image data
    * @param [in] nHeight The height of the image data
    * @return Returns true on success, false if the parameters are invalid
    */
    static bool FlipPixelBits(uint8_t* pPixelBits, size_t nPixelBitsLen, uint32_t nWidth, uint32_t nHeight);

    /** Adjust the size of the image
    * @param [in] pPixelBits The start address of the image data
    * @param [in] nPixelBitsLen The total number of bytes of the pixel data (must equal nHeight * nWidth * 4)
    * @param [in] nWidth The width of the image data
    * @param [in] nHeight The height of the image data
    * @param [in] fImageSizeScale The scaling ratio of the image, 1.0f means the original value
    * @param [out] outPixelBits The image data after resizing (the data length equals nImageHeight * nNewWidth * 4)
    * @param [out] nNewWidth The image width after resizing
    * @param [out] nNewHeight The image height after resizing
    */
    static bool ResizeImageData(const uint8_t* pPixelBits, size_t nPixelBitsLen, uint32_t nWidth, uint32_t nHeight,
                                float fImageSizeScale,
                                std::vector<uint8_t>& outPixelBits, uint32_t& nNewWidth, uint32_t& nNewHeight);

    /** Adjust the size of the image
    * @param [in] pPixelBits The start address of the image data
    * @param [in] nPixelBitsLen The total number of bytes of the pixel data (must equal nHeight * nWidth * 4)
    * @param [in] nWidth The width of the image data
    * @param [in] nHeight The height of the image data
    * @param [in] pOutPixelBits Output the image data after resizing (writable, the data length equals nImageHeight * nNewWidth * 4)
    * @param [in] nNewWidth The image width after resizing
    * @param [in] nNewHeight The image height after resizing
    */
    static bool ResizeImageData(const uint8_t* pPixelBits, size_t nPixelBitsLen, uint32_t nWidth, uint32_t nHeight,
                                uint8_t* pOutPixelBits, size_t nOutPixelBitsLen, uint32_t nNewWidth, uint32_t nNewHeight);

    /** Resize a bitmap
    * @param [in] pBitmap The bitmap interface to resize
    * @param [in] nNewWidth The image width after resizing
    * @param [in] nNewHeight The image height after resizing
    */
    static std::unique_ptr<IBitmap> ResizeImageBitmap(IBitmap* pBitmap, int32_t nNewWidth, int32_t nNewHeight);
};

} //namespace ui

#endif //UI_IMAGE_IMAGE_UTIL_H_
