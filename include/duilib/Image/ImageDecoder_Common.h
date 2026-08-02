#ifndef UI_IMAGE_IMAGE_DECODER_COMMON_H_
#define UI_IMAGE_IMAGE_DECODER_COMMON_H_

#include "duilib/Image/ImageDecoder.h"

namespace ui
{
/** General image decoder interface, supports decoding of multiple image formats, such as BMP format
*/
class ImageDecoder_Common: public IImageDecoder
{
public:
    ImageDecoder_Common();
    virtual ~ImageDecoder_Common() override;

    /** Get the format name supported by this decoder
    */
    virtual DString GetFormatName() const override;

    /** Check whether this decoder supports the given file name
    * @param [in] imageFilePath Physical file name (e.g.: "File.jpg", can include a path), or virtual file name (e.g.: "icon:1")
    */
    virtual bool CanDecode(const DString& imageFilePath) const override;

    /** Check whether this decoder supports the given data stream
    * @param [in] data The start address of the data
    * @param [in] dataLen The length of the data
    */
    virtual bool CanDecode(const uint8_t* data, size_t dataLen) const override;

    /** Load and decode the image data, return the decoded image data
    @param [in] decodeParam The parameters related to image decoding
    */
    virtual std::unique_ptr<IImage> LoadImageData(const ImageDecodeParam& decodeParam) override;
};

} //namespace ui

#endif //UI_IMAGE_IMAGE_DECODER_COMMON_H_
