#include "duilib/Image/ImageDecoder_SVG.h"
#include "duilib/Image/ImageUtil.h"
#include "duilib/Image/Image_Svg.h"
#include "duilib/Utils/FilePathUtil.h"
#include "duilib/Utils/FileUtil.h"
#include "duilib/Core/GlobalManager.h"
#include <cmath>

#if defined(__GNUC__) && !defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

#pragma warning (push)
#pragma warning (disable: 4456 4244 4702)
    #define NANOSVG_IMPLEMENTATION
    #define NANOSVG_ALL_COLOR_KEYWORDS
    #include "third_party/svg/nanosvg.h"
    #define NANOSVGRAST_IMPLEMENTATION
    #include "third_party/svg/nanosvgrast.h"
#pragma warning (pop)

#if defined(__GNUC__) && !defined(__clang__)
    #pragma GCC diagnostic pop
#endif

#include "duilib/RenderSkia/SkiaHeaderBegin.h"
#include "modules/svg/include/SkSVGDOM.h"
#include "modules/svg/include/SkSVGRenderContext.h"
#include "include/core/SkStream.h"
#include "include/core/SkBitmap.h"
#include "include/core/SkCanvas.h"
#include "duilib/RenderSkia/SkiaHeaderEnd.h"

namespace ui
{
/** Load SVG images (NanoSvg)
*/
namespace NanoSvgDecoder
{
    class SvgDeleter
    {
    public:
        inline void operator()(NSVGimage* x) const { nsvgDelete(x); }
    };

    class RasterizerDeleter
    {
    public:
        inline void operator()(NSVGrasterizer* x) const { nsvgDeleteRasterizer(x); }
    };

    /** Get the width and height of the SVG image (only parses the xml, no rendering, fast)
    */
    bool ImageSizeFromMemory(const std::vector<uint8_t>& data, int32_t& nSvgImageWidth, int32_t& nSvgImageHeight)
    {
        std::vector<uint8_t> fileData = data;//The data needs to be copied here, because the original data will be corrupted during parsing
        ASSERT(!fileData.empty());
        if (fileData.empty()) {
            return false;
        }
        bool hasAppended = false;
        if (fileData.back() != '\0') {
            //Ensure it is a string with a trailing zero, to avoid out-of-bounds memory access
            fileData.push_back('\0');
            hasAppended = true;
        }
        char* pData = (char*)fileData.data();
        NSVGimage* svgData = nsvgParse(pData, "px", 96.0f);//When "px" is passed, the third parameter dpi has no effect.
        if (hasAppended) {
            fileData.pop_back();
        }

        std::unique_ptr<NSVGimage, SvgDeleter> svg((NSVGimage*)svgData);
        int width = (int)std::ceil(svg->width);
        int height = (int)std::ceil(svg->height);
        if (width <= 0 || height <= 0) {
            return false;
        }
        nSvgImageWidth = width;
        nSvgImageHeight = height;
        return true;
    }
}

/** Implementation of the SVG vector image interface
*/
class SvgImageImpl : public ISvgImage
{
public:
    virtual ~SvgImageImpl() override = default;

    /** Get the image width
    */
    virtual uint32_t GetWidth() const override { return m_nImageWidth; }

    /** Get the image height
    */
    virtual uint32_t GetHeight() const override { return m_nImageHeight; }

    /** The width and height scaling ratio of the original image when loaded (1.0f means no scaling)
    */
    virtual float GetImageSizeScale() const override { return m_fImageSizeScale; }

    /** Get a bitmap of the specified size, vector-scaled
    * @param [in] szImageSize Represents the width (cx) and height (cy) of the image to get
    */
    virtual std::shared_ptr<IBitmap> GetBitmap(const UiSize& szImageSize) override
    {
        const uint32_t nImageWidth = szImageSize.cx > 0 ? (uint32_t)szImageSize.cx : m_nImageWidth;
        const uint32_t nImageHeight = szImageSize.cy > 0 ? (uint32_t)szImageSize.cy : m_nImageHeight;
        ASSERT((nImageWidth > 0) && (nImageHeight > 0));
        if ((nImageWidth <= 0) || (nImageHeight <= 0)) {
            return nullptr;
        }

        if ((m_pBitmap != nullptr) &&
            (m_pBitmap->GetWidth() == nImageWidth) &&
            (m_pBitmap->GetHeight() == nImageHeight)) {
            //Use the cached bitmap
            return m_pBitmap;
        }

        //Generate the bitmap, vector-scaled
        IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
        ASSERT(pRenderFactory != nullptr);
        if (pRenderFactory == nullptr) {
            return nullptr;
        }
        std::shared_ptr<IBitmap> pBitmap(pRenderFactory->CreateBitmap());
        ASSERT(pBitmap != nullptr);
        if (pBitmap == nullptr) {
            return nullptr;
        }

        SkBitmap skBitmap;
#ifdef DUILIB_BUILD_FOR_WIN
        SkImageInfo info = SkImageInfo::Make((int32_t)nImageWidth, (int32_t)nImageHeight, SkColorType::kN32_SkColorType, SkAlphaType::kPremul_SkAlphaType);
#else
        SkImageInfo info = SkImageInfo::Make((int32_t)nImageWidth, (int32_t)nImageHeight, SkColorType::kRGBA_8888_SkColorType, SkAlphaType::kPremul_SkAlphaType);
#endif
        skBitmap.allocPixels(info);
        SkCanvas canvas(skBitmap);

        //Set the container size to be consistent with the image size (the image size is the size after DPI scaling)
        m_svgDom->getRoot()->setWidth(SkSVGLength((SkScalar)nImageWidth, SkSVGLength::Unit::kPX));
        m_svgDom->getRoot()->setHeight(SkSVGLength((SkScalar)nImageHeight, SkSVGLength::Unit::kPX));
        m_svgDom->setContainerSize(SkSize::Make(SkISize::Make((int32_t)nImageWidth, (int32_t)nImageHeight)));

        //Draw to the bitmap
        m_svgDom->render(&canvas);
        if (!pBitmap->Init(nImageWidth, nImageHeight, skBitmap.getPixels())) {
            pBitmap.reset();
        }
        //Record the cached bitmap to avoid regenerating the bitmap every time
        m_pBitmap = pBitmap;
        return pBitmap;
    }

public:
    //Width
    uint32_t m_nImageWidth = 0;

    //Height
    uint32_t m_nImageHeight = 0;

    //Scaling ratio
    float m_fImageSizeScale = IMAGE_SIZE_SCALE_NONE;

    //Memory data stream
    std::unique_ptr<SkMemoryStream> m_spMemStream;

    //SVG DOM
    sk_sp<SkSVGDOM> m_svgDom;

private:
    //Cached bitmap
    std::shared_ptr<IBitmap> m_pBitmap;
};

ImageDecoder_SVG::ImageDecoder_SVG()
{
}

ImageDecoder_SVG::~ImageDecoder_SVG()
{
}

DString ImageDecoder_SVG::GetFormatName() const
{
    return _T("SVG");
}

bool ImageDecoder_SVG::CanDecode(const DString& imageFilePath) const
{
    DString fileExt = FilePathUtil::GetFileExtension(imageFilePath);
    StringUtil::MakeUpperString(fileExt);
    if (fileExt == _T("SVG")) {
        return true;
    }
    return false;
}

bool ImageDecoder_SVG::CanDecode(const uint8_t* data, size_t dataLen) const
{
    // SVG is in XML format; check whether the beginning is <?xml or <svg
    if ((data == nullptr) || (dataLen < 8)) {
        return false;
    }

    // Convert to a string for checking
    std::string headerStr(reinterpret_cast<const char*>(data), dataLen);

    // Check the common SVG beginnings
    return (headerStr.substr(0, 5) == "<?xml") || (headerStr.substr(0, 4) == "<svg");
}

std::unique_ptr<IImage> ImageDecoder_SVG::LoadImageData(const ImageDecodeParam& decodeParam)
{
    std::vector<uint8_t> fileData;
    if ((decodeParam.m_pFileData != nullptr) && !decodeParam.m_pFileData->empty()) {
        fileData = *decodeParam.m_pFileData;
    }
    else if (!decodeParam.m_imageFilePath.IsEmpty()){
        FileUtil::ReadFileData(decodeParam.m_imageFilePath, fileData);
        if (decodeParam.m_bAssertEnabled) {
            ASSERT(!fileData.empty());
        }
        if (fileData.empty()) {
            return nullptr;
        }
    }
    else {
        ASSERT(0);
        return nullptr;
    }
    std::unique_ptr<SkMemoryStream> spMemStream = SkMemoryStream::MakeCopy(fileData.data(), fileData.size());
    ASSERT(spMemStream != nullptr);
    if (spMemStream == nullptr) {
        return nullptr;
    }
    sk_sp<SkSVGDOM> svgDom = SkSVGDOM::MakeFromStream(*spMemStream);
    if (decodeParam.m_bAssertEnabled) {
        ASSERT(svgDom != nullptr);
    }
    if (svgDom == nullptr) {
        return nullptr;
    }
    if (decodeParam.m_bAssertEnabled) {
        ASSERT(svgDom->getRoot() != nullptr);
    }
    if (svgDom->getRoot() == nullptr) {
        return nullptr;
    }
    spMemStream.reset();

    SkSize svgSize = svgDom->getRoot()->intrinsicSize(SkSVGLengthContext(SkSize::Make(0, 0)));
    int32_t nSvgImageWidth = int32_t(std::ceil(svgSize.width()));
    int32_t nSvgImageHeight = int32_t(std::ceil(svgSize.height()));
    if ((nSvgImageWidth < 1) || (nSvgImageHeight < 1)) {
        auto viewBox = svgDom->getRoot()->getViewBox();
        if (viewBox.has_value()) {
            nSvgImageWidth = int32_t(std::ceil(viewBox->width()));
            nSvgImageHeight = int32_t(std::ceil(viewBox->height()));
        }
    }
    else {
        //If the viewBox does not exist, set one, otherwise there will be an exception when scaling the image (this logic keeps consistent with NanoSvg)
        auto viewBox = svgDom->getRoot()->getViewBox();
        if (!viewBox.has_value()) {
            svgDom->getRoot()->setViewBox(SkRect::MakeIWH(nSvgImageWidth, nSvgImageHeight));
        }
    }
    if ((nSvgImageWidth < 1) || (nSvgImageHeight < 1)) {
        //If the width and height are not directly defined in the image, use NanoSvg to calculate the width and height of the image (the SVG wrapper of Skia does not provide this functionality)
        if (!NanoSvgDecoder::ImageSizeFromMemory(fileData, nSvgImageWidth, nSvgImageHeight)) {
            if (decodeParam.m_bAssertEnabled) {
                ASSERT(0);
            }
            return nullptr;
        }
    }

    ASSERT((nSvgImageWidth > 0) && (nSvgImageHeight > 0));
    if ((nSvgImageWidth < 1) || (nSvgImageHeight < 1)) {
        return nullptr;
    }

    //Calculate the size after scaling
    float fImageSizeScale = decodeParam.m_fImageSizeScale;
    uint32_t nImageWidth = ImageUtil::GetScaledImageSize((uint32_t)nSvgImageWidth, fImageSizeScale);
    uint32_t nImageHeight = ImageUtil::GetScaledImageSize((uint32_t)nSvgImageHeight, fImageSizeScale);
    ASSERT((nImageHeight > 0) && (nImageHeight > 0));
    if ((nImageWidth < 1) || (nImageHeight < 1)) {
        return nullptr;
    }

    SvgImageImpl* pSvgImageImpl = new SvgImageImpl;
    std::shared_ptr<ISvgImage> pSvgImage(pSvgImageImpl);
    pSvgImageImpl->m_nImageWidth = nImageWidth;
    pSvgImageImpl->m_nImageHeight = nImageHeight;
    pSvgImageImpl->m_fImageSizeScale = fImageSizeScale;
    pSvgImageImpl->m_spMemStream = std::move(spMemStream);
    pSvgImageImpl->m_svgDom = svgDom;

    std::unique_ptr<IImage> pImage = Image_Svg::MakeImage(pSvgImage);
    return pImage;
}

} //namespace ui
