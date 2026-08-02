#ifndef UI_IMAGE_IMAGE_LOTTIE_H_
#define UI_IMAGE_IMAGE_LOTTIE_H_

#include "duilib/Image/ImageDecoder.h"

namespace ui
{
/** Image data in LOTTIE format
*/
class Image_LOTTIE : public IAnimationImage
{
public:
    Image_LOTTIE();
    virtual ~Image_LOTTIE() override;

public:
    /** Load the image data
    * @param [in] fileData The image file data (if not empty, the file data is used first)
    * @param [in] imageFilePath The image file path
    * @param [in] fImageSizeScale The image scaling percentage
    * @param [in] rcMaxDestRectSize The size of the destination area, used to optimize loading performance
    * @param [in] bAssertEnabled Whether assertions are allowed when image data errors are encountered
    */
    bool LoadImageFile(std::vector<uint8_t>& fileData,
                       const FilePath& imageFilePath,
                       float fImageSizeScale,
                       const UiSize& rcMaxDestRectSize,
                       bool bAssertEnabled);

public:
    /** Get the image width
    */
    virtual uint32_t GetWidth() const override;

    /** Get the image height
    */
    virtual uint32_t GetHeight() const override;

    /** The width and height scaling ratio of the original image when loaded (1.0f means no scaling)
    */
    virtual float GetImageSizeScale() const override;

    /** Get the frame count of the image
    */
    virtual int32_t GetFrameCount() const override;

    /** Get the number of loop plays
    * @return Returns the number of loop plays, -1 means play forever
    */
    virtual int32_t GetLoopCount() const override;

    /** Query whether the image data of a certain frame is ready (in multi-threaded decoding, the frame data is decoded in a background thread)
    * @param [in] nFrameIndex The index number of the image frame, a subscript value numbered from 0; value range: [0, GetFrameCount())
    */
    virtual bool IsFrameDataReady(uint32_t nFrameIndex) override;

    /** Get the playback duration of an image frame, in milliseconds
    * @param [in] nFrameIndex The index number of the image frame, a subscript value numbered from 0; value range: [0, GetFrameCount())
    */
    virtual int32_t GetFrameDelayMs(uint32_t nFrameIndex) override;

    /** Read one frame of data
    * @param [in] nFrameIndex The index number of the image frame, a subscript value numbered from 0; value range: [0, GetFrameCount())
    * @param [in] szDestRectSize The size of the destination area, used for scaling vector images
    * @param [out] pAnimationFrame Returns the bitmap data of the image of this frame
    * @return Returns true on success, false on failure
    */
    virtual bool ReadFrameData(int32_t nFrameIndex, const UiSize& szDestRectSize, AnimationFrame* pAnimationFrame) override;

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
    /** Private implementation data
    */
    struct TImpl;
    std::unique_ptr<TImpl> m_impl;
};

} //namespace ui

#endif //UI_IMAGE_IMAGE_LOTTIE_H_
