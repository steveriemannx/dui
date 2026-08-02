#ifndef UI_IMAGE_IMAGE_DECODER_H_
#define UI_IMAGE_IMAGE_DECODER_H_

#include "duilib/Core/UiTypes.h"
#include "duilib/Core/Callback.h"
#include "duilib/Utils/FilePath.h"

namespace ui
{
/** Bitmap interface
*/
class IBitmap;

/** Interface that supports multi-threaded decoding (used at the low-level decoding layer, supports delayed decoding, can decode in multiple threads, avoiding image decoding on the UI thread which causes stuttering)
*/
class DUILIB_API IImageDelayDecode
{
public:
    virtual ~IImageDelayDecode() = default;

    /** Whether delayed decoding of the data is enabled
    * @return Returns true if decoding is needed, false if decoding is not needed
    */
    virtual bool IsDelayDecodeEnabled() const = 0;

    /** Whether the delayed decoding of the image data is finished
    * @return The delayed decoding operation of the image data has been completed
    */
    virtual bool IsDelayDecodeFinished() const = 0;

    /** Get the image frame index currently finished by delayed decoding (numbered from 0)
    */
    virtual uint32_t GetDecodedFrameIndex() const = 0;

    /** Decode the image data with delay (can be called from multiple threads)
    * @param [in] nMinFrameIndex The minimum frame index to decode up to (frame index, numbered from 0)
    * @param [in] IsAborted The decode termination test function; returns true to abort, otherwise indicates normal operation
    * @param [out] bDecodeError Returns true if an image decoding error is encountered
    * @return Returns true on success, false if decoding fails or is externally aborted
    */
    virtual bool DelayDecode(uint32_t nMinFrameIndex,
                             std::function<bool(void)> IsAborted,
                             bool* bDecodeError) = 0;

    /** Merge the results of the delayed decoding of the image data
    */
    virtual bool MergeDelayDecodeData() = 0;
};

/** Interface that supports multi-threaded decoding (used at the application layer, supports delayed decoding, can decode in multiple threads, avoiding image decoding on the UI thread which causes stuttering)
*/
class DUILIB_API IImageAsyncDecode
{
public:
    virtual ~IImageAsyncDecode() = default;

    /** Whether asynchronous decoding of the image data is needed
    * @return Returns true if decoding is needed, false if decoding is not needed
    */
    virtual bool IsAsyncDecodeEnabled() const = 0;

    /** Whether the asynchronous decoding of the image data is finished
    * @return The asynchronous decoding operation of the image data has been completed
    */
    virtual bool IsAsyncDecodeFinished() const = 0;

    /** Get the image frame index currently finished by asynchronous decoding (numbered from 0)
    */
    virtual uint32_t GetDecodedFrameIndex() const = 0;

    /** Set the task ID of the asynchronous decoding
    * @param [in] nTaskId The task ID in the worker thread
    */
    virtual void SetAsyncDecodeTaskId(size_t nTaskId) = 0;

    /** Get the task ID of the asynchronous decoding
    */
    virtual size_t GetAsyncDecodeTaskId() const = 0;

    /** Decode the image data asynchronously (can be called from multiple threads)
    * @param [in] nMinFrameIndex The minimum frame index to decode up to (frame index, numbered from 0)
    * @param [in] IsAborted The decode termination test function; returns true to abort, otherwise indicates normal operation
    * @param [out] bDecodeError Returns true if an image decoding error is encountered
    * @return Returns true on success, false if decoding fails or is externally aborted
    */
    virtual bool AsyncDecode(uint32_t nMinFrameIndex,
                             std::function<bool(void)> IsAborted,
                             bool* bDecodeError) = 0;

    /** Merge the results of the asynchronous decoding of the image data
    */
    virtual bool MergeAsyncDecodeData() = 0;
};

/** SVG vector image interface
*/
class DUILIB_API ISvgImage
{
public:
    virtual ~ISvgImage() = default;

    /** Get the image width
    */
    virtual uint32_t GetWidth() const = 0;

    /** Get the image height
    */
    virtual uint32_t GetHeight() const = 0;

    /** The width and height scaling ratio of the original image when loaded (1.0f means no scaling)
    */
    virtual float GetImageSizeScale() const = 0;

    /** Get a bitmap of the specified size, vector-scaled
    * @param [in] szImageSize Represents the width (cx) and height (cy) of the image to get
    */
    virtual std::shared_ptr<IBitmap> GetBitmap(const UiSize& szImageSize) = 0;
};

/** Single-frame bitmap image interface
*/
class DUILIB_API IBitmapImage : public IImageDelayDecode
{
public:
    virtual ~IBitmapImage() = default;

    /** Get the image width
    */
    virtual uint32_t GetWidth() const = 0;

    /** Get the image height
    */
    virtual uint32_t GetHeight() const = 0;

    /** The width and height scaling ratio of the original image when loaded (1.0f means no scaling)
    */
    virtual float GetImageSizeScale() const = 0;

    /** Get the bitmap
    * @param [out] bDecodeError The return value indicates whether an image decoding error was encountered
    * @return Returns the interface pointer of the bitmap; if it returns nullptr and bDecodeError is false, the image decoding has not been completed yet (in the case of multi-threaded decoding)
    *                          if it returns nullptr and bDecodeError is true, an error occurred in image decoding
    */
    virtual std::shared_ptr<IBitmap> GetBitmap(bool* bDecodeError) = 0;
};

/** Default playback time interval of animation images (milliseconds)
*/
#define IMAGE_ANIMATION_DELAY_MS        (100)

/** Minimum playback time interval of animation images (milliseconds)
*/
#define IMAGE_ANIMATION_DELAY_MS_MIN    (20)

/** Animation image interface
*/
class DUILIB_API IAnimationImage: public IImageDelayDecode
{
public:
    virtual ~IAnimationImage() = default;

    /** One frame of image data of a multi-frame image
    */
    class DUILIB_API AnimationFrame
    {
    public:
        bool m_bDataPending = false;        //Whether the data is in the pending-decode state: true means pending, waiting for decoding to complete before use
        bool m_bDataError = false;          //Whether a decoding error occurred in the data
        int32_t m_nFrameIndex = -1;         //The index number of the image frame
        int32_t m_nOffsetX = 0;             //The X-axis offset value of this frame in the drawing area, in pixels
        int32_t m_nOffsetY = 0;             //The Y-axis offset value of this frame in the drawing area, in pixels
        std::shared_ptr<IBitmap> m_pBitmap; //The bitmap data of this frame, used for drawing

        /** Set the playback duration of the frame, in milliseconds
        */
        void SetDelayMs(int32_t nDelayMs)
        {
            if (nDelayMs <= 0) {
                //When not set, set to the default value
                nDelayMs = IMAGE_ANIMATION_DELAY_MS;
            }
            else if (nDelayMs < IMAGE_ANIMATION_DELAY_MS_MIN) {
                //When below the minimum value, set to the minimum value
                nDelayMs = IMAGE_ANIMATION_DELAY_MS_MIN;
            }
            m_nDelayMs = nDelayMs;
        }

        /** Get the playback duration of the frame, in milliseconds
        */
        int32_t GetDelayMs() const
        {
            return m_nDelayMs;
        }

    private:
        int32_t m_nDelayMs = IMAGE_ANIMATION_DELAY_MS;  //The playback duration of the image frame, in milliseconds
    };
public:
    /** Get the image width
    */
    virtual uint32_t GetWidth() const = 0;

    /** Get the image height
    */
    virtual uint32_t GetHeight() const = 0;

    /** The width and height scaling ratio of the original image when loaded (1.0f means no scaling)
    */
    virtual float GetImageSizeScale() const = 0;

    /** Get the frame count of the image
    */
    virtual int32_t GetFrameCount() const = 0;

    /** Get the number of loop plays
    * @return Returns the number of loop plays, -1 means play forever
    */
    virtual int32_t GetLoopCount() const = 0;

    /** Query whether the image data of a certain frame is ready (in multi-threaded decoding, the frame data is decoded in a background thread)
    * @param [in] nFrameIndex The index number of the image frame, a subscript value numbered from 0; value range: [0, GetFrameCount())
    */
    virtual bool IsFrameDataReady(uint32_t nFrameIndex) = 0;

    /** Get the playback duration of an image frame, in milliseconds
    * @param [in] nFrameIndex The index number of the image frame, a subscript value numbered from 0; value range: [0, GetFrameCount())
    */
    virtual int32_t GetFrameDelayMs(uint32_t nFrameIndex) = 0;

    /** Read one frame of data
    * @param [in] nFrameIndex The index number of the image frame, a subscript value numbered from 0; value range: [0, GetFrameCount())
    * @param [in] szDestRectSize The size of the destination area, used for scaling vector images
    * @param [out] pAnimationFrame Returns the bitmap data of the image of this frame
    * @return Returns true on success, false on failure
    */
    virtual bool ReadFrameData(int32_t nFrameIndex, const UiSize& szDestRectSize, AnimationFrame* pAnimationFrame) = 0;
};

/** Smart pointer of AnimationFrame
*/
typedef std::shared_ptr<IAnimationImage::AnimationFrame> AnimationFramePtr;

/** Image type
*/
enum class DUILIB_API ImageType
{
    kImageBitmap,       //Bitmap type, single frame; when the image is scaled, it is lossy scaling and the display effect becomes worse
    kImageSvg,          //SVG vector image, single frame; when the image is scaled, it is vector scaling and the display effect is better
    kImageAnimation     //Animation image, multiple frames
};

/** The width and height scaling ratio of the original image when loaded: the value for no scaling
*/
#define IMAGE_SIZE_SCALE_NONE (1.0f)

/** Image interface
*/
class DUILIB_API IImage: public IImageAsyncDecode
{
public:
    virtual ~IImage() = default;

public:
    /** Get the image width
    */
    virtual int32_t GetWidth() const = 0;

    /** Get the image height
    */
    virtual int32_t GetHeight() const = 0;

    /** The width and height scaling ratio of the original image when loaded (1.0f means no scaling)
    */
    virtual float GetImageSizeScale() const = 0;

    /** Get the type of the image
    */
    virtual ImageType GetImageType() const = 0;

    /** Get the image data
    * @return Returns the image data only when ImageType==ImageType::kImageBitmap
    */
    virtual std::shared_ptr<IBitmapImage> GetImageBitmap() const { return nullptr; }

    /** Get the image data
    * @return Returns the image data only when ImageType==ImageType::kImageSvg
    */
    virtual std::shared_ptr<ISvgImage> GetImageSvg() const { return nullptr; }

    /** Get the image data
    * @return Returns the image data only when ImageType==ImageType::kImageAnimation
    */
    virtual std::shared_ptr<IAnimationImage> GetImageAnimation() const { return nullptr; }
};

/** Input parameters for image decoding
*/
struct ImageDecodeParam
{
public:
    //File path
    FilePath m_imageFilePath;

    //File header data (1KB of data, used to select the image decoder)
    std::vector<uint8_t> m_fileHeaderData;

    //File data (if empty, the file data has not been loaded, and the file data needs to be read according to the file path)
    std::shared_ptr<std::vector<uint8_t>> m_pFileData;

    //Size of the destination area, used to optimize loading performance
    UiSize m_rcMaxDestRectSize;

    //The scaling ratio requested for loading
    float m_fImageSizeScale = 1.0f;

    //Whether asynchronous thread decoding of the image data is supported
    bool m_bAsyncDecode = false;

    //The code assertion setting when image loading fails (enabled in debug builds, used to diagnose errors during image loading, especially failures caused by incorrect image data)
    bool m_bAssertEnabled = true;

public:
    //For multi-frame images, whether to load all frames (true to load all frames; false to load only the first frame, loaded as a single-frame image)
    bool m_bLoadAllFrames = true;

    //For the ICO format, whether to load multiple frames for display as an animation (by default, the ICO format is displayed as a single frame)
    bool m_bIconAsAnimation = false;

    //For the ICO format, the icon size value to load
    uint32_t m_nIconSize = 0;

    //For the ICO format, the playback time interval of each frame, in milliseconds (only valid when m_bIconAsAnimation && m_bLoadAllFrames are true)
    uint32_t m_nIconFrameDelayMs = 1000;

    //For the PAG format, the frame rate of decoding the animation
    float m_fPagMaxFrameRate = 30.0f;
};

/** Image decoder interface
*/
class DUILIB_API IImageDecoder
{
public:
    virtual ~IImageDecoder() = default;

    /** Get the format name supported by this decoder
    */
    virtual DString GetFormatName() const = 0;

    /** Check whether this decoder supports the given file name
    * @param [in] imageFilePath Physical file name (e.g.: "File.jpg", can include a path), or virtual file name (e.g.: "icon:1")
    */
    virtual bool CanDecode(const DString& imageFilePath) const = 0;

    /** Check whether this decoder supports the given data stream
    * @param [in] data The start address of the data
    * @param [in] dataLen The length of the data
    */
    virtual bool CanDecode(const uint8_t* data, size_t dataLen) const = 0;

    /** Load and decode the image data, return the decoded image data
    @param [in] decodeParam The parameters related to image decoding
    */
    virtual std::unique_ptr<IImage> LoadImageData(const ImageDecodeParam& decodeParam) = 0;
};

} //namespace ui

#endif //UI_IMAGE_IMAGE_DECODER_H_
