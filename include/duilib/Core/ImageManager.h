#ifndef UI_CORE_IMAGEMANAGER_H_
#define UI_CORE_IMAGEMANAGER_H_

#include "duilib/Core/Callback.h"
#include "duilib/Core/ControlPtrT.h"
#include "duilib/Image/ImageDecoder.h"
#include <string>
#include <vector>
#include <list>
#include <unordered_map>
#include <memory>
#include <chrono>

namespace ui 
{
class ImageInfo;
class ImageLoadParam;
class DpiManager;
class Window;
class Control;
class Image;

/** Type of the delayed image release callback
 * @param [in] pImageData Image data interface of the original image
 * @param [in] imageFullPath The full path of the image
 * @return Returns true to allow adding to the delayed release queue, returns false to prevent adding to the delayed release queue
 */
using ReleaseImageCallback = std::function<bool (const std::shared_ptr<ui::IImage>& pImageData,
                                                 const DString& imageFullPath)>;

/** Image manager (for the release of image resources: delayed release; internally there is an original image queue. If an image needs to be released immediately, the ReleaseImageCallback callback can prevent it from being added to the delayed release queue)
 */
class DUILIB_API ImageManager
{
public:
    ImageManager();
    ~ImageManager();
    ImageManager(const ImageManager&) = delete;
    ImageManager& operator = (const ImageManager&) = delete;

public:
    /** Load the image ImageInfo object
     * @param [in] loadParam The loading attributes of the image, including the image path and other information
     * @param [out] bImageDataFromCache Returns true if the ImageData sharing the original image data was obtained from the cache, otherwise it was reloaded
     * @return Returns the smart pointer of the image ImageInfo object
     */
    std::shared_ptr<ImageInfo> GetImage(const ImageLoadParam& loadParam, bool& bImageDataFromCache);

    /** Delete all images from the cache
     */
    void RemoveAllImages();

    /** Release an original image from the cache (delayed release)
    * @param [in] pImageData Image data interface of the original image
    * @param [in] imageFullPath The full path of the image
    */
    void ReleaseImage(const std::shared_ptr<IImage>& pImageData, const DString& imageFullPath);

    /** Cancel the release of the original image
    */
    void CancelReleaseImage(const std::shared_ptr<IImage>& pImageData);

    /** Set the callback function for the delayed release of images, which can be used to prevent image resources from being added to the delayed release queue and release them immediately
     *   Note: if the image resource is used in the sub-items of a virtual list, releasing the original image resource immediately will degrade performance, because the elements of the virtual list are refilled on every refresh
     * @param [in] callback Callback function for the delayed release of images
     */
    void SetReleaseImageCallback(ReleaseImageCallback callback);

public:
    /** Set whether to smart match the image of the nearest scale percentage
    *   For example, when dpiScale is 120, if no image matches but an image with a scale percentage of 125 exists, it will be matched automatically
    *   This feature can reduce the number of images for each DPI and reduce the DPI adaptation workload
    */
    void SetAutoMatchScaleImage(bool bAutoMatchScaleImage);

    /** Get whether to smart match the image of the nearest scale percentage
    */
    bool IsAutoMatchScaleImage() const;

    /** Set whether the multi-threaded asynchronous loading of image data is enabled by default
    */
    void SetImageAsyncLoad(bool bImageAsyncLoad);

    /** Get whether the multi-threaded asynchronous loading of image data is enabled by default
    */
    bool IsImageAsyncLoad() const;

public:
    /** Add to the delayed paint list
    * @param [in] pControl The control associated with the image
    * @param [in] pImage The image interface
    * @param [in] imageKey The KEY of the image resource
    */
    void AddDelayPaintData(Control* pControl, Image* pImage, const DString& imageKey);

    /** Remove the image-related data from the delayed paint list
    * @param [in] pControl The control associated with the image
    * @param [in] pImage The image interface
    */
    void RemoveDelayPaintData(Control* pControl);
    void RemoveDelayPaintData(Image* pImage);

    /** Perform the delayed paint (called when the image resource finishes loading in a child thread)
    * @param [in] imageKey The KEY of the image resource
    */
    void DelayPaintImage(const DString& imageKey);

private:
    /** Callback function called when the image info is destroyed, used to release the image resource
    * @param[in] pImageInfo The ImageInfo object corresponding to the image
    */
    static void CallImageInfoDestroy(ImageInfo* pImageInfo);

    /** Callback function called when the image data is destroyed, used to release the data of the image resource
    * @param[in] pImage The image data interface
    */
    static void CallImageDataDestroy(IImage* pImage);

private:
    /** Callback function called when the image info is created
    * @param[in] pImageInfo The ImageInfo object corresponding to the image
    */
    void OnImageInfoCreate(std::shared_ptr<ImageInfo>& pImageInfo);

    /** Callback function called when the image info is destroyed, used to release the image resource
     * @param[in] pImageInfo The ImageInfo object corresponding to the image
     */
    void OnImageInfoDestroy(ImageInfo* pImageInfo);

    /** Callback function called when the image data is created
     * @param[in] imageKey The KEY of the image
     * @param[in] pImage The image data interface
     * @param[in] fImageSizeScale The scale ratio of the image
     */
    void OnImageDataCreate(const DString& imageKey, std::shared_ptr<IImage>& pImage, float fImageSizeScale);

    /** Callback function called when the image data is destroyed, used to release the data of the image resource
     * @param[in] pImage The image data interface
     */
    void OnImageDataDestroy(IImage* pImage);

private:
    /** Find the image for the specified DPI scale percentage; one image can be set per DPI to improve image quality at different DPIs
    *   For example, an image with a DPI scale of 120 (i.e., scaled up to 120%): "image.png" corresponds to "image@120.png"
    * @param [in] dpiScale The DPI scale percentage to look up
    * @param [in] bIsUseZip Whether to use zip archive resources
    * @param [in] imageFullPath The full path of the image resource
    * @param [out] dpiImageFullPath Returns the image resource path for the specified DPI, or an empty string if not found
    * @param [out] nImageFileDpiScale The DPI scale percentage corresponding to the image
    */
    bool GetDpiScaleImageFullPath(uint32_t dpiScale,
                                  bool bIsUseZip,
                                  const DString& imageFullPath,
                                  DString& dpiImageFullPath,
                                  uint32_t& nImageFileDpiScale) const;

    /** Find the image for the specified DPI scale percentage; one image can be set per DPI to improve image quality at different DPIs
    *   For example, an image with a DPI scale of 120 (i.e., scaled up to 120%): "image.png" corresponds to "image@120.png"
    * @param [in] dpiScale The DPI scale percentage to look up
    * @param [in] bIsUseZip Whether to use zip archive resources
    * @param [in] imageFullPath The full path of the image resource
    * @param [out] dpiImageFullPath Returns the image resource path for the specified DPI, or an empty string if not found
    */
    bool FindDpiScaleImageFullPath(uint32_t dpiScale,
                                  bool bIsUseZip,
                                  const DString& imageFullPath,
                                  DString& dpiImageFullPath) const;

    /** Get the image resource path for the specified DPI scale percentage
    *   For example, an image with a DPI scale of 120 (i.e., scaled up to 120%): "image.png" corresponds to "image@120.png"
    * @param [in] dpiScale The DPI scale percentage to look up
    * @param [in] imageFullPath The full path of the image resource
    * @return Returns the image resource path for the specified DPI, or an empty string on failure
    */
    DString GetDpiScaledPath(uint32_t dpiScale, const DString& imageFullPath) const;

private:
    /** Whether to smart match the image of the nearest scale percentage
    */
    bool m_bAutoMatchScaleImage;

    /** Whether the multi-threaded asynchronous loading of image data is enabled by default
    */
    bool m_bImageAsyncLoad;

    /** Image resource mapping table (mapping between the image loading key and the image UI data)
    *   KEY: obtained from the ImageLoadParam::GetLoadKey function
    */
    std::unordered_map<DString, std::weak_ptr<ImageInfo>> m_imageInfoMap;

    /** Original image data of the image
    */
    struct TImageData
    {
        //Constructor
        TImageData() :
            m_fImageSizeScale(1.0f)
        {
        }
        TImageData(const std::shared_ptr<IImage>& pImage, float fImageSizeScale) :
            m_pImage(pImage),
            m_fImageSizeScale(fImageSizeScale)
        {
        }

        //Released image interface
        std::weak_ptr<IImage> m_pImage;

        //Image scale ratio passed in at load time
        float m_fImageSizeScale;
    };

    /** Image resource mapping table (mapping between the original image data key and the image data)
    *   KEY: obtained from the ImageManager::GetDpiScaleImageFullPath function, parameter: dpiImageFullPath
    */
    std::unordered_map<DString, TImageData> m_imageDataMap;

    /** Original image data waiting to be released
    */
    struct TReleaseImageData
    {
        //Released image interface
        std::shared_ptr<IImage> m_pImage;

        //Time when the data was released
        std::chrono::steady_clock::time_point m_releaseTime;
    };
    std::vector<TReleaseImageData> m_delayReleaseImageList;

    /** Callback function for the delayed release of images
    */
    ReleaseImageCallback m_releaseImageCallback;

private:
    /** Data related to the delayed paint of images (after the image resource finishes loading in a child thread, the UI needs to be notified to repaint the image)
    */
    struct TImageDelayPaintData
    {
        ControlPtr m_pControl;          //The associated control interface
        ControlPtrT<Image> m_pImage;    //The associated image interface
        DString m_imageKey;             //The KEY of the image resource
    };
    std::list<TImageDelayPaintData> m_delayPaintImageList;
};

}
#endif //UI_CORE_IMAGEMANAGER_H_
