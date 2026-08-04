#include "dui/Core/ImageManager.h"
#include "dui/Image/Image.h"
#include "dui/Image/ImageLoadParam.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Core/DpiManager.h"
#include "dui/Core/Window.h"
#include "dui/Core/Control.h"
#include "dui/Utils/StringUtil.h"
#include "dui/Utils/StringConvert.h"
#include "dui/Utils/FileUtil.h"
#include "dui/Utils/FilePathUtil.h"

#ifdef DUI_BUILD_FOR_WIN
    //#define OUTPUT_IMAGE_LOG 1
#endif

namespace ui 
{
ImageManager::ImageManager():
    m_bAutoMatchScaleImage(true),
    m_bImageAsyncLoad(true),
    m_releaseImageCallback(nullptr)
{
}

ImageManager::~ImageManager()
{
}

std::shared_ptr<ImageInfo> ImageManager::GetImage(const ImageLoadParam& loadParam, bool& bImageDataFromCache)
{
    ASSERT(ui::GlobalManager::Instance().IsInUIThread());
    bImageDataFromCache = false;
    const DString loadKey = loadParam.GetLoadKey(loadParam.GetLoadDpiScale());
    auto iter = m_imageInfoMap.find(loadKey);
    if (iter != m_imageInfoMap.end()) {
        std::shared_ptr<ImageInfo> spImageInfo = iter->second.lock();
        if (spImageInfo != nullptr) {
            //A valid image resource was found in the cache, return it directly
            bImageDataFromCache = true;
            return spImageInfo;
        }
    }

    //Reload the resource
    const ImageLoadPath& imageLoadPath = loadParam.GetImageLoadPath();
    DString imageFullPath = imageLoadPath.m_imageFullPath.ToString();   //Image path (local path or relative path in the archive)
    uint32_t nImageFileDpiScale = 100;                                  //DPI scale of the original image is 100 when it is not DPI scaled
    const bool isUseZip = GlobalManager::Instance().Zip().IsUseZip();   //Whether to use a Zip archive
    const bool bImageDpiScaleEnabled = loadParam.IsImageDpiScaleEnabled();//Image attribute: load_scale="false", use only the original image, no scaling needed
    if (bImageDpiScaleEnabled && 
        ((imageLoadPath.m_pathType == ImageLoadPathType::kLocalResPath) ||
         (imageLoadPath.m_pathType == ImageLoadPathType::kZipResPath))) {
        //Only files in the resource directory get the DPI-adaptive image lookup
        DString dpiImageFullPath;
        uint32_t dpiImageDpiScale = nImageFileDpiScale;
        if (GetDpiScaleImageFullPath(loadParam.GetLoadDpiScale(), isUseZip, imageFullPath, dpiImageFullPath, dpiImageDpiScale)) {
            //Mark the DPI-adaptive image attribute; if the path differs, a file for the corresponding DPI has been selected
            ASSERT((dpiImageDpiScale != 0) && !dpiImageFullPath.empty());
            if ((dpiImageDpiScale != 0) && !dpiImageFullPath.empty()) {
                imageFullPath = dpiImageFullPath;
                nImageFileDpiScale = dpiImageDpiScale;
                ASSERT(!imageFullPath.empty());
            }
        }
    }

    float fImageSizeScale = 1.0f;
    //Calculate the set scale, affecting the scaling percentage on load (image attributes set via width='300' or width='300%')
    const bool bHasFixedSize = loadParam.HasImageFixedSize();
    if (bImageDpiScaleEnabled && !bHasFixedSize) {
        //Load scale (calculated relative to the original image, so that all DPI-adaptive images display identically)
        //1. If the image width and height are used to evaluate the display space: scale by the DPI ratio so the UI looks the same at different DPIs
        //2. If the image width and height are not needed to evaluate the display space, this load scale only affects the image display, not the layout
        //3. If the width or height attribute of the image is set, use only the original image, no scaling (because scaling is performed after loading)
        fImageSizeScale = static_cast<float>(loadParam.GetLoadDpiScale()) / static_cast<float>(nImageFileDpiScale);
    }

    std::shared_ptr<IImage> spImageData;
    //Query the cache; if a cached entry exists, the image resource can be shared without reloading
    const DString imageKey = imageFullPath;
    auto iterImageData = m_imageDataMap.find(imageKey);
    if (iterImageData != m_imageDataMap.end()) {
        spImageData = iterImageData->second.m_pImage.lock();
#ifdef OUTPUT_IMAGE_LOG
        DString log = _T("Lock ImageData(reuse): ") + imageKey + _T("\n");
        ::OutputDebugString(log.c_str());
#endif
        if (spImageData != nullptr) {
            if (!ImageUtil::IsSameImageScale(iterImageData->second.m_fImageSizeScale, fImageSizeScale)) {
                //After dynamically switching DPI, the scale changes and the image must be reloaded; the previously loaded image cannot be shared
                m_imageDataMap.erase(iterImageData);
                spImageData.reset();
            }
        }
    }
    bImageDataFromCache = spImageData != nullptr ? true : false; //Whether the shared ImageData resource was obtained from the cache
    if (spImageData == nullptr) {
        //Load the image from memory data
        ImageDecoderFactory& ImageDecoders = GlobalManager::Instance().ImageDecoders();
        std::vector<uint8_t> fileData;
        std::vector<uint8_t> fileHeaderData;
        if (imageLoadPath.m_pathType != ImageLoadPathType::kVirtualPath) {
            //A physical image file must have image data for decoding
            FilePath imageFilePath(imageFullPath);
            if (isUseZip && !imageFilePath.IsAbsolutePath()) {
                GlobalManager::Instance().Zip().GetZipData(imageFilePath, fileData);
                ASSERT(!fileData.empty());
                if (fileData.empty()) {
                    //Load failed
                    return nullptr;
                }
            }
            else {
                bool bReadFileData = true;//Whether to read the complete file content into memory (by default, all image file data is read into memory, then loaded and decoded)
                if (imageLoadPath.m_pathType == ImageLoadPathType::kLocalPath) {
                    //Local file (not in the program's resources directory; may be a large file, e.g., several MB or more)
                    uint64_t nFileSize = imageFilePath.GetFileSize();
                    if (nFileSize > 128 * 1024) {//128KB
                        //Large file
                        bReadFileData = false;
                    }
                }
                if (bReadFileData) {
                    //Small files/files in the program's resources directory, etc.: read all file data
                    FileUtil::ReadFileData(imageFilePath, fileData);
                    if (loadParam.IsAssertEnabled()) {
                        ASSERT(!fileData.empty());
                    }                    
                    if (fileData.empty()) {
                        //Load failed
                        return nullptr;
                    }
                }
                else {
                    //Large file: read only part of the file header data for signature validation (read 4KB of data)
                    FileUtil::ReadFileHeaderData(imageFilePath, 4 * 1024, fileHeaderData);
                    if (loadParam.IsAssertEnabled()) {
                        ASSERT(!fileHeaderData.empty());
                    }
                    if (fileHeaderData.empty()) {
                        //Load failed
                        return nullptr;
                    }
                }
            }           
        }
        ImageDecodeParam decodeParam;
        decodeParam.m_imageFilePath = imageFullPath;//The steps above have already ensured the file exists when it is a local file
        if (!fileData.empty()) {
            decodeParam.m_pFileData = std::make_shared<std::vector<uint8_t>>();
            decodeParam.m_pFileData->swap(fileData);
        }
        else if (!fileHeaderData.empty()) {
            decodeParam.m_fileHeaderData.swap(fileHeaderData);
        }
        if (nImageFileDpiScale == 100) {//For the DPI-adaptive original image, do not enable this optimization to avoid abnormal original image size calculations
            decodeParam.m_rcMaxDestRectSize = loadParam.GetMaxDestRectSize();
        }
        decodeParam.m_fImageSizeScale = fImageSizeScale;

        decodeParam.m_bAsyncDecode = loadParam.IsAsyncDecodeEnabled();    //Whether multi-threaded image decoding is supported
        decodeParam.m_bIconAsAnimation = loadParam.IsIconAsAnimation();   //ICO format related parameter
        decodeParam.m_nIconSize = loadParam.GetIconSize();                //ICO format related parameter
        decodeParam.m_nIconFrameDelayMs = loadParam.GetIconFrameDelayMs();//ICO format related parameter
        decodeParam.m_fPagMaxFrameRate = loadParam.GetPagMaxFrameRate();  //PAG format related parameter
        decodeParam.m_bLoadAllFrames = true; //Parameter related to all multi-frame images
        decodeParam.m_bAssertEnabled = loadParam.IsAssertEnabled();       //Whether assertion is allowed when image loading fails (generally only affects problems caused by image data errors)

        //Load the image
        std::unique_ptr<IImage> pImageData = ImageDecoders.LoadImageData(decodeParam);
        bool bEnableAssert = true;
#ifndef DUI_IMAGE_SUPPORT_LIB_PAG        
        if (pImageData == nullptr) {
            DString fileExt = FilePathUtil::GetFileExtension(decodeParam.m_imageFilePath.ToString());
            StringUtil::MakeUpperString(fileExt);
            if (fileExt == _T("PAG")) {
                //When PAG is not supported, disable the assertion error
                bEnableAssert = false;
            }
        }
#endif
        if (loadParam.IsAssertEnabled() && bEnableAssert) {
            ASSERT(pImageData != nullptr); //Assert when image loading fails
        }        
        if (pImageData == nullptr) {
            //Load failed
            return nullptr;
        }

        ASSERT((pImageData->GetWidth() > 0) && (pImageData->GetHeight() > 0));
        if ((pImageData->GetWidth() <= 0) || (pImageData->GetHeight() <= 0)) {
            //Load failed
            return nullptr;
        }
        //Assign, and add to the container (replace the delete function)
        ASSERT(imageKey == imageFullPath);
        spImageData.reset(pImageData.release(), ImageManager::CallImageDataDestroy);//TODO: to be verified, may have platform compatibility issues
        OnImageDataCreate(imageKey, spImageData, fImageSizeScale);        
    }
    if (spImageData != nullptr) {
        std::shared_ptr<ImageInfo> imageInfo(new ImageInfo, &ImageManager::CallImageInfoDestroy);
        imageInfo->SetImageKey(imageKey);
        bool bRet = imageInfo->SetImageData(loadParam, spImageData, bImageDpiScaleEnabled, nImageFileDpiScale);
        ASSERT(bRet);
        if (bRet) {
            ASSERT(loadKey == imageInfo->GetLoadKey());
            OnImageInfoCreate(imageInfo);

            if (bImageDataFromCache) {
                //If the ImageData from the cache is being reused, remove the data from the pending-release queue
                CancelReleaseImage(spImageData);
            }
            return imageInfo;
        }
    }
    return nullptr;
}

void ImageManager::CallImageInfoDestroy(ImageInfo* pImageInfo)
{
    ImageManager& imageManager = GlobalManager::Instance().Image();
    imageManager.OnImageInfoDestroy(pImageInfo);
}

void ImageManager::CallImageDataDestroy(IImage* pImage)
{
    ImageManager& imageManager = GlobalManager::Instance().Image();
    imageManager.OnImageDataDestroy(pImage);
}

void ImageManager::OnImageInfoCreate(std::shared_ptr<ImageInfo>& pImageInfo)
{
    ASSERT(pImageInfo != nullptr);
    if (pImageInfo != nullptr) {
        DString loadKey = pImageInfo->GetLoadKey();
        ASSERT(!loadKey.empty());
        if (!loadKey.empty()) {
            m_imageInfoMap[loadKey] = pImageInfo;
#ifdef OUTPUT_IMAGE_LOG
            DString log = _T("Created ImageInfo: ") + loadKey + _T("\n");
            ::OutputDebugString(log.c_str());
#endif
        }
    }
}

void ImageManager::OnImageInfoDestroy(ImageInfo* pImageInfo)
{
    ASSERT(ui::GlobalManager::Instance().IsInUIThread());
    ASSERT(pImageInfo != nullptr);
    if (pImageInfo != nullptr) {
        DString loadKey = pImageInfo->GetLoadKey();
        ASSERT(!loadKey.empty());
        if (!loadKey.empty()) {            
            auto iter = m_imageInfoMap.find(loadKey);
            if (iter != m_imageInfoMap.end()) {
                m_imageInfoMap.erase(iter);
            }
        }
        delete pImageInfo;
#ifdef OUTPUT_IMAGE_LOG
        DString log = _T("Removed ImageInfo: ") + loadKey + _T("\n");
        ::OutputDebugString(log.c_str());
#endif
    }
}

void ImageManager::OnImageDataCreate(const DString& imageKey, std::shared_ptr<IImage>& pImage, float fImageSizeScale)
{
    ASSERT(!imageKey.empty() && (pImage != nullptr));
    if (!imageKey.empty() && (pImage != nullptr)) {
        m_imageDataMap[imageKey] = TImageData(pImage, fImageSizeScale);
#ifdef OUTPUT_IMAGE_LOG
        DString log = _T("Created ImageData: ") + imageKey + _T("\n");
        ::OutputDebugString(log.c_str());
#endif
    }
}

void ImageManager::OnImageDataDestroy(IImage* pImage)
{
    ASSERT(ui::GlobalManager::Instance().IsInUIThread());
    ASSERT(pImage != nullptr);
    if (pImage != nullptr) {
        auto iter = m_imageDataMap.begin();
        while (iter != m_imageDataMap.end()) {
            if (iter->second.m_pImage.expired()) {
#ifdef OUTPUT_IMAGE_LOG
                DString log = _T("Removed ImageData: ") + iter->first + _T("\n");
                ::OutputDebugString(log.c_str());
#endif
                iter = m_imageDataMap.erase(iter);
            }
            else {
                ++iter;
            }
        }
        delete pImage;
    }
}

void ImageManager::RemoveAllImages()
{
    m_imageDataMap.clear();
    m_delayReleaseImageList.clear();
    m_imageInfoMap.clear();
}

void ImageManager::ReleaseImage(const std::shared_ptr<IImage>& pImageData, const DString& imageFullPath)
{
    //First remove the element from the queue, so that only one element is in the queue
    CancelReleaseImage(pImageData);

    //Through the callback function, adding to the delayed release queue can be avoided
    if (m_releaseImageCallback != nullptr) {
        bool bAllowRelease = m_releaseImageCallback(pImageData, imageFullPath);
        if (!bAllowRelease) {
            return;
        }
    }

    if (pImageData != nullptr) {
        TReleaseImageData imageData;
        imageData.m_pImage = pImageData;
        imageData.m_releaseTime = std::chrono::steady_clock::now();
        m_delayReleaseImageList.push_back(imageData);
    }

    const int32_t nDelaySeconds = 35;
    auto delayReleaseImage = [nDelaySeconds]() {
        ImageManager& imageManager = GlobalManager::Instance().Image();
        auto nowTime = std::chrono::steady_clock::now();
        auto iter = imageManager.m_delayReleaseImageList.begin();
        while (iter != imageManager.m_delayReleaseImageList.end()) {
            const TReleaseImageData& imageData = *iter;
            //Check and release the image resource (interval: 30 seconds, release the original image to avoid affecting image sharing)
            auto seconds = std::chrono::duration_cast<std::chrono::seconds>(nowTime - imageData.m_releaseTime);
            if (seconds.count() > (nDelaySeconds - 5)) {
                iter = imageManager.m_delayReleaseImageList.erase(iter);
            }
            else {
                ++iter;
            }
        }
    };
    GlobalManager::Instance().Thread().PostDelayedTask(ui::kThreadUI, delayReleaseImage, nDelaySeconds * 1000);
}

void ImageManager::CancelReleaseImage(const std::shared_ptr<IImage>& pImageData)
{
    if ((pImageData != nullptr) && !m_delayReleaseImageList.empty()) {
        auto iter = m_delayReleaseImageList.begin();
        while (iter != m_delayReleaseImageList.end()) {
            const TReleaseImageData& imageData = *iter;
            if (imageData.m_pImage == pImageData) {
                iter = m_delayReleaseImageList.erase(iter);
                break;
            }
            else {
                ++iter;
            }
        }
    }
}

void ImageManager::SetReleaseImageCallback(ReleaseImageCallback callback)
{
    m_releaseImageCallback = callback;
}

void ImageManager::SetAutoMatchScaleImage(bool bAutoMatchScaleImage)
{
    m_bAutoMatchScaleImage = bAutoMatchScaleImage;
}

bool ImageManager::IsAutoMatchScaleImage() const
{
    return m_bAutoMatchScaleImage;
}

void ImageManager::SetImageAsyncLoad(bool bImageAsyncLoad)
{
    m_bImageAsyncLoad = bImageAsyncLoad;
}

bool ImageManager::IsImageAsyncLoad() const
{
    return m_bImageAsyncLoad;
}

bool ImageManager::GetDpiScaleImageFullPath(uint32_t dpiScale,
                                            bool bIsUseZip,
                                            const DString& imageFullPath,
                                            DString& dpiImageFullPath,
                                            uint32_t& nImageFileDpiScale) const
{
    nImageFileDpiScale = 0;
    if (FindDpiScaleImageFullPath(dpiScale, bIsUseZip, imageFullPath, dpiImageFullPath)) {
        nImageFileDpiScale = dpiScale;
        return true;
    }
    dpiImageFullPath.clear();
    if (!IsAutoMatchScaleImage() || (dpiScale < 115)) {
        //The smart match feature is disabled, or the current DPI does not need smart matching of the scale percentage
        return false;
    }

    DString dpiFullPath;
    std::vector<uint32_t> allScales = {125, 150, 175, 200, 225, 250, 300};
    std::vector<std::pair<uint32_t, DString>> allDpiImagePath;
    for (auto scale : allScales) {
        if (FindDpiScaleImageFullPath(scale, bIsUseZip, imageFullPath, dpiFullPath)) {
            allDpiImagePath.push_back({ scale, dpiFullPath });
        }
    }

    dpiImageFullPath.clear();
    size_t nCount = allDpiImagePath.size();
    for (size_t index = 0; index < nCount; ++index) {
        uint32_t nScale = allDpiImagePath[index].first;
        const DString& sPath = allDpiImagePath[index].second;
        if (nScale > dpiScale) {
            if (index == 0) {
                //The first one
                dpiImageFullPath = sPath;
                nImageFileDpiScale = nScale;
                break;
            }
            else {
                //Between the two, choose the closest one
                uint32_t nLastScale = allDpiImagePath[index - 1].first;
                ASSERT(nLastScale <= dpiScale);
                float diffScaleLast = ((float)dpiScale - (float)nLastScale) / (float)nLastScale;
                float diffScale = ((float)nScale - (float)dpiScale) / (float)nScale;
                if (diffScaleLast < diffScale) {
                    dpiImageFullPath = allDpiImagePath[index - 1].second;
                    nImageFileDpiScale = allDpiImagePath[index - 1].first;
                }
                else {
                    dpiImageFullPath = sPath;
                    nImageFileDpiScale = nScale;
                }
                break;
            }
        }
        else if (index == (nCount - 1)) {
            //The last one
            dpiImageFullPath = sPath;
            nImageFileDpiScale = nScale;
        }
    }
    return !dpiImageFullPath.empty();
}

bool ImageManager::FindDpiScaleImageFullPath(uint32_t dpiScale,
                                             bool bIsUseZip,
                                             const DString& imageFullPath,
                                             DString& dpiImageFullPath) const
{
    dpiImageFullPath.clear();
    if ((dpiScale == 100) || (dpiScale == 0)) {
        //The current DPI does not need scaling
        return false;
    }
    dpiImageFullPath = GetDpiScaledPath(dpiScale, imageFullPath);
    if (dpiImageFullPath.empty()) {
        return false;
    }

    bool bExists = false;
    if (bIsUseZip) {
        bExists = GlobalManager::Instance().Zip().IsZipResExist(FilePath(dpiImageFullPath));
    }
    else {
        bExists = FilePath(dpiImageFullPath).IsExistsPath();
    }
    if (!bExists) {
        dpiImageFullPath.clear();
    }
    return bExists;
}

DString ImageManager::GetDpiScaledPath(uint32_t dpiScale, const DString& imageFullPath) const
{
    DString strPathDir;
    DString strPathFileName;
    std::list<DString> strPathList = StringUtil::Split(imageFullPath, _T("\\"));
    for (auto it = strPathList.begin(); it != strPathList.end(); ++it) {
        auto itTemp = it;
        if (++itTemp == strPathList.end()) {
            strPathFileName = *it;
        }
        else {
            strPathDir += *it + _T("\\");
        }
    }

    size_t iPointPos = strPathFileName.rfind('.');
    ASSERT(iPointPos != DString::npos);
    if (iPointPos == DString::npos) {
        return DString();
    }
    DString strFileExtension = strPathFileName.substr(iPointPos, strPathFileName.size() - iPointPos);
    DString strFile = strPathFileName.substr(0, iPointPos);
    //Return the image for the specified DPI; for example, an image with a DPI scale of 120 (i.e., scaled up to 120%): "image.png" corresponds to "image@120.png"
    strPathFileName = StringUtil::Printf(_T("%s%s%d%s"), strFile.c_str(), _T("@"), dpiScale, strFileExtension.c_str());
    DString strNewFilePath = strPathDir + strPathFileName;
    return strNewFilePath;
}

void ImageManager::AddDelayPaintData(Control* pControl, Image* pImage, const DString& imageKey)
{
    GlobalManager::Instance().AssertUIThread();
    ASSERT((pControl != nullptr) && (pImage != nullptr) && !imageKey.empty());
    if ((pControl == nullptr) || (pImage == nullptr) || imageKey.empty()) {
        return;
    }
    RemoveDelayPaintData(pImage); //Only keep one record per Image resource

    TImageDelayPaintData delayPaint;
    delayPaint.m_pControl = pControl;
    delayPaint.m_pImage = pImage;
    delayPaint.m_imageKey = imageKey;
    m_delayPaintImageList.push_back(delayPaint);
}

void ImageManager::RemoveDelayPaintData(Control* pControl)
{
    GlobalManager::Instance().AssertUIThread();
    GlobalManager::Instance().AssertUIThread();
    ASSERT(pControl != nullptr);
    if (pControl == nullptr) {
        return;
    }
    auto iter = m_delayPaintImageList.begin();
    while (iter != m_delayPaintImageList.end()) {
        if ((iter->m_pControl == pControl) || (iter->m_pControl == nullptr)) {
            iter = m_delayPaintImageList.erase(iter);
        }
        else {
            ++iter;
        }
    }
}

void ImageManager::RemoveDelayPaintData(Image* pImage)
{
    GlobalManager::Instance().AssertUIThread();
    GlobalManager::Instance().AssertUIThread();
    ASSERT((pImage != nullptr));
    if (pImage == nullptr) {
        return;
    }
    auto iter = m_delayPaintImageList.begin();
    while (iter != m_delayPaintImageList.end()) {
        if ((iter->m_pImage == pImage) || (iter->m_pImage == nullptr)) {
            iter = m_delayPaintImageList.erase(iter);
        }
        else {
            ++iter;
        }
    }
}

void ImageManager::DelayPaintImage(const DString& imageKey)
{
    GlobalManager::Instance().AssertUIThread();
    GlobalManager::Instance().AssertUIThread();
    ASSERT(!imageKey.empty());
    if (imageKey.empty()) {
        return;
    }
    auto iter = m_delayPaintImageList.begin();
    while (iter != m_delayPaintImageList.end()) {
        if ((iter->m_pControl == nullptr) || (iter->m_pImage == nullptr) || (iter->m_imageKey == imageKey)) {
            ControlPtrT<Image> pImage = iter->m_pImage;
            iter = m_delayPaintImageList.erase(iter);
            if (pImage != nullptr) {
                pImage->RedrawImage();
            }
        }
        else {
            ++iter;
        }
    }
}

} //namespace ui
