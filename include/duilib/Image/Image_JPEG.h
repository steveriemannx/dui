#ifndef UI_IMAGE_IMAGE_JPEG_H_
#define UI_IMAGE_IMAGE_JPEG_H_

#include "duilib/Image/ImageDecoder.h"
#include "duilib/Render/IRender.h"

#ifdef DUILIB_IMAGE_SUPPORT_JPEG_TURBO

namespace ui
{
/** Image data in JPEG format (the image data is not decoded when loading; the image data is decoded when getting the bitmap)
*/
class Image_JPEG: public IBitmapImage
{
public:
    /** Load the image data
     * @param [in] fileData The image file data (if not empty, the file data is used first)
     * @param [in] imageFilePath The image file path
     * @param [in] fImageSizeScale The scaling ratio of the image, 1.0f means the original value
     * @param [in] bAsyncDecode Whether asynchronous thread decoding of the image data is supported
     * @param [in] rcMaxDestRectSize The size of the destination area, used to optimize loading performance
     * @param [in] bAssertEnabled Whether assertions are allowed when image data errors are encountered
     */
    bool LoadImageFile(std::vector<uint8_t>& fileData,
                       const FilePath& imageFilePath,
                       float fImageSizeScale,
                       bool bAsyncDecode,
                       const UiSize& rcMaxDestRectSize,
                       bool bAssertEnabled);

public:
    Image_JPEG();
    virtual ~Image_JPEG() override;

    /** Get the image width
    */
    virtual uint32_t GetWidth() const override;

    /** Get the image height
    */
    virtual uint32_t GetHeight() const override;

    /** The width and height scaling ratio of the original image when loaded (1.0f means no scaling)
    */
    virtual float GetImageSizeScale() const override;

    /** Get the bitmap
    * @param [out] bDecodeError The return value indicates whether an image decoding error was encountered
    * @return Returns the interface pointer of the bitmap; if it returns nullptr and bDecodeError is false, the image decoding has not been completed yet (in the case of multi-threaded decoding)
    *                          if it returns nullptr and bDecodeError is true, an error occurred in image decoding
    */
    virtual std::shared_ptr<IBitmap> GetBitmap(bool* bDecodeError) override;

public:
    /** Whether delayed decoding of the data is enabled
    * @return Returns true if decoding is needed, false if decoding is not needed
    */
    virtual bool IsDelayDecodeEnabled() const override;

    /** Whether the delayed decoding of the image data is finished
    * @return The delayed decoding operation of the image data has been completed
    */
    virtual bool IsDelayDecodeFinished() const override;

    /** Get the image frame index currently finished by delayed decoding (numbered from 0)
    */
    virtual uint32_t GetDecodedFrameIndex() const override;

    /** Decode the image data with delay (can be called from multiple threads)
    * @param [in] nMinFrameIndex The minimum frame index to decode up to (frame index, numbered from 0)
    * @param [in] IsAborted The decode termination test function; returns true to abort, otherwise indicates normal operation
    * @param [out] bDecodeError Returns true if an image decoding error is encountered
    * @return Returns true on success, false if decoding fails or is externally aborted
    */
    virtual bool DelayDecode(uint32_t nMinFrameIndex,
                             std::function<bool(void)> IsAborted,
                             bool* bDecodeError) override;

    /** Merge the results of the delayed decoding of the image data
    */
    virtual bool MergeDelayDecodeData() override;

private:
    /** Decode the image data
    */
    std::shared_ptr<IBitmap> DecodeBitmap() const;

private:
    /** Private implementation data
    */
    struct TImpl;
    std::unique_ptr<TImpl> m_impl;
};

} //namespace ui

#endif //DUILIB_IMAGE_SUPPORT_JPEG_TURBO
#endif //UI_IMAGE_IMAGE_JPEG_H_
