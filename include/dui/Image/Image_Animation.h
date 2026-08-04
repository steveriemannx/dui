#ifndef UI_IMAGE_IMAGE_ANIMATION_H_
#define UI_IMAGE_IMAGE_ANIMATION_H_

#include "dui/Image/ImageDecoder.h"

namespace ui
{
/** Image data in animation format
*/
class DUI_API Image_Animation: public IImage
{
public:
    Image_Animation(const std::shared_ptr<IAnimationImage>& pAnimationImage);
    virtual ~Image_Animation() override;

    /** Get the image width
    */
    virtual int32_t GetWidth() const override;

    /** Get the image height
    */
    virtual int32_t GetHeight() const override;

    /** The width and height scaling ratio of the original image when loaded (1.0f means no scaling)
    */
    virtual float GetImageSizeScale() const override;

    /** Get the type of the image
    */
    virtual ImageType GetImageType() const override;

    /** Get the image data
    * @return Returns the image data only when ImageType==ImageType::kImageAnimation
    */
    virtual std::shared_ptr<IAnimationImage> GetImageAnimation() const override;

private:
    /** Whether asynchronous decoding of the image data is needed
    * @return Returns true if decoding is needed, false if decoding is not needed
    */
    virtual bool IsAsyncDecodeEnabled() const override;

    /** Whether the asynchronous decoding of the image data is finished
    * @return The asynchronous decoding operation of the image data has been completed
    */
    virtual bool IsAsyncDecodeFinished() const override;

    /** Get the image frame index currently finished by asynchronous decoding (numbered from 0)
    */
    virtual uint32_t GetDecodedFrameIndex() const override;

    /** Set the task ID of the asynchronous decoding
    * @param [in] nTaskId The task ID in the worker thread
    */
    virtual void SetAsyncDecodeTaskId(size_t nTaskId) override;

    /** Get the task ID of the asynchronous decoding
    */
    virtual size_t GetAsyncDecodeTaskId() const override;

    /** Decode the image data asynchronously (can be called from multiple threads)
    * @param [in] nMinFrameIndex The minimum frame index to decode up to (frame index, numbered from 0)
    * @param [in] IsAborted The decode termination test function; returns true to abort, otherwise indicates normal operation
    * @param [out] bDecodeError Returns true if an image decoding error is encountered
    * @return Returns true on success, false if decoding fails or is externally aborted
    */
    virtual bool AsyncDecode(uint32_t nMinFrameIndex,
                             std::function<bool(void)> IsAborted,
                             bool* bDecodeError) override;

    /** Merge the results of the asynchronous decoding of the image data
    */
    virtual bool MergeAsyncDecodeData() override;

private:
    /** The animation image
    */
    std::shared_ptr<IAnimationImage> m_pAnimationImage;

    /** The asynchronous decoding task ID
    */
    size_t m_nAsyncDecodeTaskId;
};

} //namespace ui

#endif //UI_IMAGE_IMAGE_ANIMATION_H_
