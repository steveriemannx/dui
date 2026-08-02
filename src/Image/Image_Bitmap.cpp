#include "duilib/Image/Image_Bitmap.h"
#include "duilib/Core/GlobalManager.h"

namespace ui
{

/** Implementation of the single-frame bitmap image interface
*/
class Image_Bitmap::BitmapImageImpl : public IBitmapImage
{
public:
    BitmapImageImpl():
        m_fImageSizeScale(IMAGE_SIZE_SCALE_NONE)
    {
    }

    virtual ~BitmapImageImpl() = default;

    /** Get the image width
    */
    virtual uint32_t GetWidth() const override
    {
        if (m_pBitmap != nullptr) {
            return m_pBitmap->GetWidth();
        }
        else if (m_pAnimationImage != nullptr) {
            return m_pAnimationImage->GetWidth();
        }
        return 0;
    }

    /** Get the image height
    */
    virtual uint32_t GetHeight() const override
    {
        if (m_pBitmap != nullptr) {
            return m_pBitmap->GetHeight();
        }
        else if (m_pAnimationImage != nullptr) {
            return m_pAnimationImage->GetHeight();
        }
        return 0;
    }

    /** The width and height scaling ratio of the original image when loaded (1.0f means no scaling)
    */
    virtual float GetImageSizeScale() const override
    {
        return m_fImageSizeScale;
    }

    /** Get the bitmap
    * @param [out] bDecodeError The return value indicates whether an image decoding error was encountered
    * @return Returns the interface pointer of the bitmap; if it returns nullptr and bDecodeError is false, the image decoding has not been completed yet (in the case of multi-threaded decoding)
    *                          if it returns nullptr and bDecodeError is true, an error occurred in image decoding
    */
    virtual std::shared_ptr<IBitmap> GetBitmap(bool* bDecodeError) override
    {
        if (m_pBitmap != nullptr) {
            return m_pBitmap;
        }
        //Delayed decoding
        if (m_pAnimationImage != nullptr) {
            //Single frame, load as a bitmap image
            IAnimationImage::AnimationFrame frame;
            if (m_pAnimationImage->ReadFrameData(0, UiSize(), &frame)) {
                if (bDecodeError != nullptr) {
                    *bDecodeError = frame.m_bDataError;
                }
                m_pBitmap = frame.m_pBitmap;
                if (m_pBitmap != nullptr) {
                    //After reading is finished, release the resources
                    m_pAnimationImage.reset();
                }
            }
            else {
                if (bDecodeError != nullptr) {
                    *bDecodeError = true;
                }
                //ASSERT(0);
            }
        }
        return m_pBitmap;
    }

public:
    /** Whether delayed decoding of the data is enabled
    * @return Returns true if decoding is needed, false if decoding is not needed
    */
    virtual bool IsDelayDecodeEnabled() const override
    {
        if (m_pAnimationImage != nullptr) {
            return m_pAnimationImage->IsDelayDecodeEnabled();
        }
        return false;
    }

    /** Whether the delayed decoding of the image data is finished
    * @return The delayed decoding operation of the image data has been completed
    */
    virtual bool IsDelayDecodeFinished() const override
    {
        if (m_pAnimationImage != nullptr) {
            return m_pAnimationImage->IsDelayDecodeFinished();
        }
        return true;
    }

    /** Get the image frame index currently finished by delayed decoding (numbered from 0)
    */
    virtual uint32_t GetDecodedFrameIndex() const override
    {
        if (m_pAnimationImage != nullptr) {
            return m_pAnimationImage->GetDecodedFrameIndex();
        }
        return 0;
    }

    /** Decode the image data with delay (can be called from multiple threads)
    * @param [in] nMinFrameIndex The minimum frame index to decode up to (frame index, numbered from 0)
    * @param [in] IsAborted The decode termination test function; returns true to abort, otherwise indicates normal operation
    * @param [out] bDecodeError Returns true if an image decoding error is encountered
    * @return Returns true on success, false if decoding fails or is externally aborted
    */
    virtual bool DelayDecode(uint32_t nMinFrameIndex,
                             std::function<bool(void)> IsAborted,
                             bool* bDecodeError) override
    {
        if (m_pAnimationImage != nullptr) {
            return m_pAnimationImage->DelayDecode(nMinFrameIndex, IsAborted, bDecodeError);
        }
        return false;
    }

    /** Merge the results of the delayed decoding of the image data
    */
    virtual bool MergeDelayDecodeData() override
    {
        if (m_pAnimationImage != nullptr) {
            return m_pAnimationImage->MergeDelayDecodeData();
        }
        return false;
    }

public:
    /** Bitmap data
    */
    std::shared_ptr<IBitmap> m_pBitmap;

    /** Animation data (only the first frame is taken)
    */
    std::shared_ptr<IAnimationImage> m_pAnimationImage;

    /** The scaling ratio when loading the original image
    */
    float m_fImageSizeScale;
};

Image_Bitmap::Image_Bitmap():
    m_nAsyncDecodeTaskId(0)
{
}

Image_Bitmap::~Image_Bitmap()
{
}

std::unique_ptr<IImage> Image_Bitmap::MakeImage(uint32_t nWidth, uint32_t nHeight,
                                                const void* pPixelBits,
                                                float fImageSizeScale,
                                                BitmapAlphaType alphaType)
{

    IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
    ASSERT(pRenderFactory != nullptr);
    if (pRenderFactory == nullptr) {
        return nullptr;
    }
    IBitmap* pBitmap = pRenderFactory->CreateBitmap();
    ASSERT(pBitmap != nullptr);
    if (pBitmap == nullptr) {
        return nullptr;
    }
    if (!pBitmap->Init(nWidth, nHeight, pPixelBits, fImageSizeScale, alphaType)) {
        delete pBitmap;
        return nullptr;
    }
    Image_Bitmap* pImageBitmap = new Image_Bitmap;
    std::unique_ptr<IImage> pImage(pImageBitmap);
    BitmapImageImpl* pImageBitmapImpl = new BitmapImageImpl;
    pImageBitmap->m_pBitmapImage.reset(pImageBitmapImpl);

    pImageBitmapImpl->m_pBitmap.reset(pBitmap);
    pImageBitmapImpl->m_fImageSizeScale = fImageSizeScale;
    return pImage;
}

std::unique_ptr<IImage> Image_Bitmap::MakeImage(const std::shared_ptr<IBitmap>& pBitmap, float fImageSizeScale)
{
    ASSERT(pBitmap != nullptr);
    if (pBitmap == nullptr) {
        return nullptr;
    }
    ASSERT((pBitmap->GetWidth() > 0) && (pBitmap->GetHeight() > 0));
    if ((pBitmap->GetWidth() <= 0) || (pBitmap->GetHeight() <= 0)) {
        return nullptr;
    }
    Image_Bitmap* pImageBitmap = new Image_Bitmap;
    std::unique_ptr<IImage> pImage(pImageBitmap);
    BitmapImageImpl* pImageBitmapImpl = new BitmapImageImpl;
    pImageBitmap->m_pBitmapImage.reset(pImageBitmapImpl);

    pImageBitmapImpl->m_pBitmap = pBitmap;
    pImageBitmapImpl->m_fImageSizeScale = fImageSizeScale;
    return pImage;
}

std::unique_ptr<IImage> Image_Bitmap::MakeImage(const std::shared_ptr<IBitmapImage>& pBitmap)
{
    ASSERT(pBitmap != nullptr);
    if (pBitmap == nullptr) {
        return nullptr;
    }
    ASSERT((pBitmap->GetWidth() > 0) && (pBitmap->GetHeight() > 0));
    if ((pBitmap->GetWidth() <= 0) || (pBitmap->GetHeight() <= 0)) {
        return nullptr;
    }

    Image_Bitmap* pImageBitmap = new Image_Bitmap;
    std::unique_ptr<IImage> pImage(pImageBitmap);
    pImageBitmap->m_pBitmapImage = pBitmap;
    return pImage;
}

std::unique_ptr<IImage> Image_Bitmap::MakeImage(const std::shared_ptr<IAnimationImage>& pAnimationImage)
{
    ASSERT(pAnimationImage != nullptr);
    if (pAnimationImage == nullptr) {
        return nullptr;
    }
    ASSERT((pAnimationImage->GetWidth() > 0) && (pAnimationImage->GetHeight() > 0));
    if ((pAnimationImage->GetWidth() <= 0) || (pAnimationImage->GetHeight() <= 0)) {
        return nullptr;
    }

    Image_Bitmap* pImageBitmap = new Image_Bitmap;
    std::unique_ptr<IImage> pImage(pImageBitmap);
    BitmapImageImpl* pImageBitmapImpl = new BitmapImageImpl;
    pImageBitmap->m_pBitmapImage.reset(pImageBitmapImpl);

    pImageBitmapImpl->m_pAnimationImage = pAnimationImage;
    pImageBitmapImpl->m_fImageSizeScale = pAnimationImage->GetImageSizeScale();
    return pImage;
}

int32_t Image_Bitmap::GetWidth() const
{
    if (m_pBitmapImage != nullptr) {
        return m_pBitmapImage->GetWidth();
    }
    return 0;
}

int32_t Image_Bitmap::GetHeight() const
{
    if (m_pBitmapImage != nullptr) {
        return m_pBitmapImage->GetHeight();
    }
    return 0;
}

float Image_Bitmap::GetImageSizeScale() const
{
    if (m_pBitmapImage != nullptr) {
        return m_pBitmapImage->GetImageSizeScale();
    }
    return 1.0f;
}

ImageType Image_Bitmap::GetImageType() const
{
    return ImageType::kImageBitmap;
}

std::shared_ptr<IBitmapImage> Image_Bitmap::GetImageBitmap() const
{
    ASSERT(m_pBitmapImage != nullptr);
    return m_pBitmapImage;
}

bool Image_Bitmap::IsAsyncDecodeEnabled() const
{
    ASSERT(m_pBitmapImage != nullptr);
    if (m_pBitmapImage != nullptr) {
        return m_pBitmapImage->IsDelayDecodeEnabled();
    }
    return false;
}

bool Image_Bitmap::IsAsyncDecodeFinished() const
{
    ASSERT(m_pBitmapImage != nullptr);
    if (m_pBitmapImage != nullptr) {
        return m_pBitmapImage->IsDelayDecodeFinished();
    }
    return false;
}

uint32_t Image_Bitmap::GetDecodedFrameIndex() const
{
    ASSERT(m_pBitmapImage != nullptr);
    if (m_pBitmapImage != nullptr) {
        return m_pBitmapImage->GetDecodedFrameIndex();
    }
    return 0;
}

void Image_Bitmap::SetAsyncDecodeTaskId(size_t nTaskId)
{
    m_nAsyncDecodeTaskId = nTaskId;
}

size_t Image_Bitmap::GetAsyncDecodeTaskId() const
{
    return m_nAsyncDecodeTaskId;
}

bool Image_Bitmap::AsyncDecode(uint32_t nMinFrameIndex, std::function<bool(void)> IsAborted, bool* bDecodeError)
{
    ASSERT(m_pBitmapImage != nullptr);
    if (m_pBitmapImage != nullptr) {
        return m_pBitmapImage->DelayDecode(nMinFrameIndex, IsAborted, bDecodeError);
    }
    return false;
}

bool Image_Bitmap::MergeAsyncDecodeData()
{
    ASSERT(m_pBitmapImage != nullptr);
    if (m_pBitmapImage != nullptr) {
        return m_pBitmapImage->MergeDelayDecodeData();
    }
    return false;
}

} //namespace ui
