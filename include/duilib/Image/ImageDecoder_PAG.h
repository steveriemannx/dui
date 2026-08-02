#ifndef UI_IMAGE_IMAGE_DECODER_PAG_H_
#define UI_IMAGE_IMAGE_DECODER_PAG_H_

#include "duilib/Image/ImageDecoder.h"

#ifdef DUILIB_IMAGE_SUPPORT_LIB_PAG

namespace ui
{
/** Image decoder interface for the PAG format
*/
class ImageDecoder_PAG: public IImageDecoder
{
public:
    ImageDecoder_PAG();
    virtual ~ImageDecoder_PAG() override;

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

#endif //DUILIB_IMAGE_SUPPORT_LIB_PAG

#endif //UI_IMAGE_IMAGE_DECODER_PAG_H_
