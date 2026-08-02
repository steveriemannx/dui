#ifndef UI_IMAGE_IMAGE_DECODER_FACTORY_H_
#define UI_IMAGE_IMAGE_DECODER_FACTORY_H_

#include "duilib/Image/ImageDecoder.h"
#include "duilib/Image/ImageUtil.h"

namespace ui
{
/** Image format decoder factory class
*/
class DUILIB_API ImageDecoderFactory
{
public:
    ImageDecoderFactory();
    ~ImageDecoderFactory();

    /** Add an image format decoder
    * @param [in] pImageDecoder The image decoder to add
    */
    bool AddImageDecoder(const std::shared_ptr<IImageDecoder>& pImageDecoder);

    /** Remove an image format decoder
    * @param [in] pImageDecoder The image decoder to add
    */
    bool RemoveImageDecoder(const std::shared_ptr<IImageDecoder>& pImageDecoder);

    /** Clear all image format decoders
    */
    void Clear();

public:
    /** Load and decode the image data, return the decoded image data
    * @param [in] decodeParam The parameters related to image decoding
    */
    std::unique_ptr<IImage> LoadImageData(const ImageDecodeParam& decodeParam);

    /** Decode a file data into a bitmap (multi-frame images are not supported; if the image has multiple frames, only the first frame is decoded)
    * @param [in] decodeParam The parameters related to image decoding
    */
    std::shared_ptr<IBitmap> DecodeImageData(const ImageDecodeParam& decodeParam);

private:
    /** Image decoders
    */
    std::vector<std::shared_ptr<IImageDecoder>> m_imageDecoders;
};

} // namespace ui

#endif // UI_IMAGE_IMAGE_DECODER_FACTORY_H_
