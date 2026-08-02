#include "duilib/Image/ImageDecoderUtil.h"
#include "duilib/Image/Image.h"
#include "duilib/Core/GlobalManager.h"
#include "duilib/Utils/StringUtil.h"
#include "duilib/Utils/FileUtil.h"
#include "duilib/Utils/PerformanceUtil.h"
#include "duilib/Image/ImageUtil.h"

#pragma warning (push)
#pragma warning (disable: 4244 4505)
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_TGA
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM
#define STBI_NO_GIF
#define STBI_NO_PNG
#if DUILIB_IMAGE_SUPPORT_JPEG_TURBO
    #define STBI_NO_JPEG
#endif
#include "third_party/stb_image/stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STBIR_DEFAULT_FILTER_UPSAMPLE STBIR_FILTER_TRIANGLE
#pragma warning (disable: 4505)
    #include "third_party/stb_image/stb_image_resize2.h"
#pragma warning (pop)

#pragma warning (push)
#pragma warning (disable: 4996)
    #include "third_party/cximage/ximage.h"
#pragma warning (pop)

#include <set>

namespace ui
{

/** Load images using stb_image
*/
namespace STBImageLoader
{
    /** Load an image from memory data
    */
    static bool LoadImageFromMemoryOrFile(const std::vector<uint8_t>& fileData, const FilePath& filePath, UiImageData& imageData)
    {
#ifdef DUILIB_BUILD_FOR_WIN
        imageData.m_imageDataType = UiImageDataType::kBGRA;
#else
        imageData.m_imageDataType = UiImageDataType::kRGBA;
#endif
        int nWidth = 0;
        int nHeight = 0;
        std::vector<uint8_t>& argbData = imageData.m_imageData;
        argbData.clear();
        constexpr const int desired_channels = 4; //The returned image data format is fixed: RGBA, each image element is 4 bytes
        int channels_in_file = 4;
        uint8_t* rgbaData = nullptr;
        if (!fileData.empty()) {
            const uint8_t* buffer = fileData.data();
            int len = (int)fileData.size();
            rgbaData = stbi_load_from_memory(buffer, len, &nWidth, &nHeight, &channels_in_file, desired_channels);
        }
        else if (!filePath.IsEmpty()) {
            std::string filePathA = filePath.NativePathA();
            rgbaData = stbi_load(filePathA.c_str(), &nWidth, &nHeight, &channels_in_file, desired_channels);
        }
        else {
            ASSERT(0);
        }

        if (rgbaData == nullptr) {
            return false;
        }

        ASSERT( (channels_in_file == 3) || (channels_in_file == 4));
        //ASSERT((nWidth > 0) && (nHeight > 0));
        if (((channels_in_file == 3) || (channels_in_file == 4)) &&
            (nWidth > 0) && (nHeight > 0)) {
            argbData.resize((size_t)nHeight * nWidth * desired_channels);
            const size_t colorCount = (size_t)nHeight * nWidth;

            //Data format conversion: ABGR[alpha, blue, green, red] -> ARGB[alpha, red, green, blue]
            for (size_t i = 0; i < colorCount; ++i) {
                size_t colorIndex = i * 4;
                if (channels_in_file == 4) {
                    argbData[colorIndex + 3] = rgbaData[colorIndex + 3]; //A, alpha
                }
                else {
                    argbData[colorIndex + 3] = 255; //A, alpha
                }
#ifdef DUILIB_BUILD_FOR_WIN
                //Data format: BGRA on Windows, RGBA on other platforms
                argbData[colorIndex + 0] = rgbaData[colorIndex + 2]; //B, blue
                argbData[colorIndex + 1] = rgbaData[colorIndex + 1]; //G, green
                argbData[colorIndex + 2] = rgbaData[colorIndex + 0]; //R, red
#else
                argbData[colorIndex + 0] = rgbaData[colorIndex + 0]; //R, red
                argbData[colorIndex + 1] = rgbaData[colorIndex + 1]; //G, green
                argbData[colorIndex + 2] = rgbaData[colorIndex + 2]; //B, blue
#endif
            }

            imageData.m_imageWidth = nWidth;
            imageData.m_imageHeight = nHeight;
        }
        stbi_image_free(rgbaData);
        return !argbData.empty();
    }
}//STBImageLoader

/** Load images using cximage (only supports the GIF and ICO formats)
* @param [in] isIconFile If true, it is an ICO file; otherwise, it is a GIF file
* @param [in] iconSize The size of the ICO icon to load, because the ICO file contains icons of various sizes, only one icon is loaded when loading
* @param [in] bLoadAllFrames For multi-frame images, whether to load all frames (true to load all frames, false to load only the first frame)
* @param [out] nFrameCount Returns the total frame count of the image
*/
namespace CxImageLoader
{
    // Find the icon size that best matches nIconSize from widthList
    static uint32_t FindBestIconSize(const std::vector<uint32_t>& widthList, uint32_t nIconSize)
    {
        if (widthList.empty()) {
            // Handle the empty container case; return 0 here as the default value
            return 0;
        }

        // Check whether there is an exactly matching value
        auto exactMatch = std::find(widthList.begin(), widthList.end(), nIconSize);
        if (exactMatch != widthList.end()) {
            return *exactMatch;
        }

        // Find the first value greater than nIconSize
        auto greaterIt = std::upper_bound(widthList.begin(), widthList.end(), nIconSize);

        if (greaterIt != widthList.end()) {
            // If an element greater than the target value exists, compare which of it and the previous element is closer
            if (greaterIt == widthList.begin()) {
                return *greaterIt;
            }

            uint32_t prev = *(greaterIt - 1);
            uint32_t curr = *greaterIt;

            // Prefer the larger value if the two distances are equal
            if (nIconSize - prev < curr - nIconSize) {
                return prev;
            }
            else {
                return curr;
            }
        }
        else {
            // All elements are smaller than the target value, return the largest element
            return widthList.back();
        }
    }

    static bool LoadImageFromMemory(const std::vector<uint8_t>& fileData,
                                    bool bLoadAllFrames,
                                    uint32_t iconSize,
                                    std::vector<UiImageData>& imageData)
    {
        ASSERT(!fileData.empty());
        if (fileData.empty()) {
            return false;
        }
        uint32_t imagetype = CXIMAGE_FORMAT_ICO;
        CxMemFile stream((uint8_t*)fileData.data(), (uint32_t)fileData.size());
        CxImage cxImage(imagetype);
        cxImage.SetRetreiveAllFrames(true);
        bool isLoaded = cxImage.Decode(&stream, imagetype);
        int32_t frameCount = cxImage.GetNumFrames();
        //ASSERT(isLoaded && cxImage.IsValid() && (frameCount > 0));
        if (!isLoaded || !cxImage.IsValid() || (frameCount < 1)) {
            return false;
        }

        //ICO
        std::vector<uint32_t> frameNumColors;  //Used to record the number of colors of each Frame in the ICO file
        std::unique_ptr<CxImage> cxIcoImage;   //The extraction interface of the ICO file of each Frame
        std::unique_ptr<CxMemFile> cxIcoStream;//The ICO file data stream of each Frame
        //
        imageData.clear();
        imageData.resize(frameCount);
        frameNumColors.resize(frameCount);
        for (int32_t index = 0; index < frameCount; ++index) {
            CxImage* cxFrame = nullptr;
            cxIcoStream = std::make_unique<CxMemFile>((uint8_t*)fileData.data(), (uint32_t)fileData.size());
            cxIcoImage = std::make_unique<CxImage>(imagetype);
            cxIcoImage->SetFrame(index);
            if (cxIcoImage->Decode(cxIcoStream.get(), imagetype) && cxIcoImage->IsValid()) {
                cxFrame = cxIcoImage.get();
            }

            //ASSERT(cxFrame != nullptr);
            if (cxFrame == nullptr) {
                imageData.clear();
                return false;
            }
            frameNumColors[index] = cxFrame->GetNumColors();////2, 16, 256; 0 for RGB images.

            uint32_t nWidth = cxFrame->GetWidth();
            uint32_t nHeight = cxFrame->GetHeight();
            ASSERT((nWidth > 0) && (nHeight > 0));
            if ((nWidth == 0) || (nHeight == 0)) {
                imageData.clear();
                return false;
            }

            int32_t lPx = 0;
            int32_t lPy = 0;
            UiImageData& bitmapData = imageData[index];
            bitmapData.m_imageData.resize((size_t)nHeight * nWidth * 4);
            RGBQUAD* pBit = (RGBQUAD*)bitmapData.m_imageData.data();
            for (lPy = 0; lPy < (int32_t)nHeight; ++lPy) {
                for (lPx = 0; lPx < (int32_t)nWidth; ++lPx) {
                    *pBit = cxFrame->GetPixelColor(lPx, lPy, true);
                    if (!cxFrame->AlphaIsValid() && !cxFrame->IsTransparent() && !cxFrame->AlphaPaletteIsEnabled()) {
                        //If there is no Alpha channel, fill the A value with a fixed value
                        pBit->rgbReserved = 255;
                    }
                    else {
                        //The image contains an Alpha channel
                        uint8_t a = pBit->rgbReserved;
                        if (!cxFrame->AlphaIsValid()) {
                            a = 255;
                        }

                        int32_t transIndex = cxFrame->GetTransIndex();//Gets the index used for transparency. Returns -1 for no transparancy.
                        int32_t bitCount = cxFrame->GetBpp();//1, 4, 8, 24.
                        int32_t numColors = cxFrame->GetNumColors();//2, 16, 256; 0 for RGB images.
                        if ((transIndex >= 0) && (bitCount < 24) && (numColors != 0) && (cxFrame->GetDIB() != nullptr)) {
                            RGBQUAD transColor = cxFrame->GetTransColor();
                            if ((transColor.rgbRed == pBit->rgbRed) &&
                                (transColor.rgbGreen == pBit->rgbGreen) &&
                                (transColor.rgbBlue == pBit->rgbBlue)) {
                                //Transparent color, mark the Alpha channel as fully transparent
                                a = 0;
                            }
                        }
                        pBit->rgbReserved = a;

                        if ((a > 0) && (a < 255)) {
                            pBit->rgbRed = pBit->rgbRed * a / 255;
                            pBit->rgbGreen = pBit->rgbGreen * a / 255;
                            pBit->rgbBlue = pBit->rgbBlue * a / 255;
                        }
                    }
#ifdef DUILIB_BUILD_FOR_WIN
                    //Data format: BGRA on Windows, RGBA on other platforms
                    bitmapData.m_imageDataType = UiImageDataType::kBGRA;
#else
                    //Other platforms, swap the R and B values
                    bitmapData.m_imageDataType = UiImageDataType::kRGBA;
                    uint8_t r = pBit->rgbRed;
                    pBit->rgbRed = pBit->rgbBlue;
                    pBit->rgbBlue = r;
#endif
                    ++pBit;
                }
            }
            bitmapData.m_imageWidth = nWidth;
            bitmapData.m_imageHeight = nHeight;
        }

        if (!bLoadAllFrames && !imageData.empty()) {
            //Only load one ICO image; expand later according to the actual application scenarios (prefer 32-bit true-color images, then 256-color, then 16-color)
            std::set<uint32_t> widthSet;
            const size_t imageCount = imageData.size();
            for (size_t i = 0; i < imageCount; ++i) {
                const UiImageData& icoData = imageData[i];
                widthSet.insert(icoData.m_imageWidth);
            }
            std::vector<uint32_t> widthList;
            for (auto v : widthSet) {
                widthList.push_back(v);
            }
            uint32_t nBestIconSize = FindBestIconSize(widthList, iconSize);
            if (nBestIconSize == 0) {
                nBestIconSize = iconSize;
            }
            bool isIconSizeValid = false;
            for (size_t i = 0; i < imageCount; ++i) {
                const UiImageData& icoData = imageData[i];
                if (icoData.m_imageWidth == nBestIconSize) {
                    isIconSizeValid = true;
                    break;
                }
            }

            std::vector<uint32_t> colors = {0, 256, 16, 2};
            bool isFound = false;
            for (auto color : colors) {
                for (size_t i = 0; i < imageCount; ++i) {
                    const UiImageData& icoData = imageData[i];
                    uint32_t numColors = frameNumColors[i];
                    if ((!isIconSizeValid || (icoData.m_imageWidth == nBestIconSize)) && (numColors == color)) {
                        UiImageData oneData = icoData;
                        imageData.resize(1);
                        imageData[0] = oneData;
                        isFound = true;
                        break;
                    }
                }
                if (isFound) {
                    break;
                }
            }
            if (imageData.size() > 1) {
                UiImageData oneData = imageData.front();
                imageData.resize(1);
                imageData[0] = oneData;
            }
        }

        for (UiImageData& bitmapData : imageData) {
            //The data loaded by CxImage needs to be flipped, with the top-left corner of the screen as the origin
            if (!bitmapData.m_imageData.empty()) {
                ImageUtil::FlipPixelBits(bitmapData.m_imageData.data(), bitmapData.m_imageData.size(), bitmapData.m_imageWidth, bitmapData.m_imageHeight);
            }
        }
        return !imageData.empty();
    }
}//CxImageLoader

DString ImageDecoderUtil::GetSupportedFileExtentions()
{
    //Bitmap format extensions: BMP;DIB
    return DString(_T("BMP;DIB"));
}

bool ImageDecoderUtil::CanDecode(const uint8_t* data, size_t dataLen)
{
    if ((data == nullptr) || (dataLen == 0)) {
        return false;
    }

    return true;
}

bool ImageDecoderUtil::LoadImageFromFile(const FilePath& filePath,
                                         UiImageData& imageData)
{
    std::vector<uint8_t> fileData;
    return STBImageLoader::LoadImageFromMemoryOrFile(fileData, filePath, imageData);
}

bool ImageDecoderUtil::LoadImageFromMemory(const std::vector<uint8_t>& fileData,
                                           UiImageData& imageData)
{
    FilePath filePath;
    return STBImageLoader::LoadImageFromMemoryOrFile(fileData, filePath, imageData);
}

bool ImageDecoderUtil::LoadIcoFromFile(const FilePath& filePath,
                                       bool bLoadAllFrames,
                                       uint32_t iconSize /*Only valid when bLoadAllFrames is false*/,
                                       std::vector<UiImageData>& imageData)
{
    //The ICO format only supports loading from a memory stream
    std::vector<uint8_t> fileData;
    FileUtil::ReadFileData(filePath, fileData);
    return CxImageLoader::LoadImageFromMemory(fileData, bLoadAllFrames, iconSize, imageData);
}

bool ImageDecoderUtil::LoadIcoFromMemory(const std::vector<uint8_t>& fileData,
                                         bool bLoadAllFrames,
                                         uint32_t iconSize /*Only valid when bLoadAllFrames is false*/,
                                         std::vector<UiImageData>& imageData)
{
    return CxImageLoader::LoadImageFromMemory(fileData, bLoadAllFrames, iconSize, imageData);
}

} // namespace ui
