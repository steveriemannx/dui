#ifndef UI_IMAGE_IMAGE_DECODER_UTIL_H_
#define UI_IMAGE_IMAGE_DECODER_UTIL_H_

#include "duilib/Core/UiTypes.h"
#include "duilib/Utils/FilePath.h"

namespace ui
{
class DUILIB_API ImageDecoderUtil
{
public:
    /** Query which types of images are supported (by extension; the extension does not contain the '.' character, e.g., "BMP;JPG", etc.)
    */
    static DString GetSupportedFileExtentions();

    /** Check whether this decoder supports the given data stream
    * @param [in] data The start address of the data
    * @param [in] dataLen The length of the data
    */
    static bool CanDecode(const uint8_t* data, size_t dataLen);

    /** Load an image (general purpose, supports multiple file formats, but does not support loading multiple frames)
    * @param [in] filePath The file path
    * @param [out] imageData Returns the loaded bitmap data
    */
    static bool LoadImageFromFile(const FilePath& filePath,
                                  UiImageData& imageData);

    /** Load an image (general purpose, supports multiple file formats, but does not support loading multiple frames)
    * @param [in] fileData The file data
    * @param [out] imageData Returns the loaded bitmap data
    */
    static bool LoadImageFromMemory(const std::vector<uint8_t>& fileData,
                                    UiImageData& imageData);

public:
    /** Load an ICO image (this function only supports the ICO format)
    * @param [in] filePath The ICO file path
    * @param [in] bLoadAllFrames For multi-frame images, whether to load all frames (true to load all frames, false to load only the first frame)
    * @param [in] iconSize Only valid when bLoadAllFrames is false, the size of the ICO icon to load
    *                      Because the ICO file contains icons of various sizes, only one icon is loaded when loading
    * @param [out] imageData Returns the loaded bitmap data
    */
    static bool LoadIcoFromFile(const FilePath& filePath,
                                bool bLoadAllFrames,
                                uint32_t iconSize /*Only valid when bLoadAllFrames is false*/,
                                std::vector<UiImageData>& imageData);

    /** Load an ICO image (this function only supports the ICO format)
    * @param [in] fileData The ICO file data
    * @param [in] bLoadAllFrames For multi-frame images, whether to load all frames (true to load all frames, false to load only the first frame)
    * @param [in] iconSize Only valid when bLoadAllFrames is false, the size of the ICO icon to load
    *                      Because the ICO file contains icons of various sizes, only one icon is loaded when loading
    * @param [out] imageData Returns the loaded bitmap data
    */
    static bool LoadIcoFromMemory(const std::vector<uint8_t>& fileData,
                                  bool bLoadAllFrames,
                                  uint32_t iconSize /*Only valid when bLoadAllFrames is false*/,
                                  std::vector<UiImageData>& imageData);
};

} // namespace ui

#endif // UI_IMAGE_IMAGE_DECODER_UTIL_H_
