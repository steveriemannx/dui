#include "duilib/RenderSkia/DrawSkiaImage.h"

#include "duilib/Utils/PerformanceUtil.h"

#pragma warning (push)
#pragma warning (disable: 4505)
    #define STBIR_DEFAULT_FILTER_UPSAMPLE STBIR_FILTER_TRIANGLE
    #include "third_party/stb_image/stb_image_resize2.h"
#pragma warning (pop)

#include "duilib/RenderSkia/SkiaHeaderBegin.h"
#include "include/core/SkBitmap.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkData.h"
#include "duilib/RenderSkia/SkiaHeaderEnd.h"

//Define the DUILIB_HAVE_OPENCV macro to enable OpenCV; using OpenCV for image scaling is the fastest
//If enabled, the following steps are required:
//1. In the duilib project, add the root of the OpenCV library installation path to the include directory of the VC project
//2. In the executable project, the following libs need to be added when linking: opencv_core4110.lib;opencv_imgproc4110.lib (these are the libs of OpenCV 4.11; other versions have different lib names)

//#define DUILIB_HAVE_OPENCV 1

#ifdef DUILIB_HAVE_OPENCV
    #pragma warning (push)
    #pragma warning (disable: 4127)
        #include "opencv2/opencv.hpp"
    #pragma warning (pop)
#endif

//#ifdef DUILIB_HAVE_OPENCV
//    #ifdef _DEBUG
//        #pragma comment (lib, "opencv_core4110d.lib")
//        #pragma comment (lib, "opencv_imgproc4110d.lib")
//    #else
//        #pragma comment (lib, "opencv_core4110.lib")
//        #pragma comment (lib, "opencv_imgproc4110.lib")
//    #endif
//#endif

namespace ui 
{

#ifdef DUILIB_HAVE_OPENCV
//OpenCV

// Direct memory mapping (zero copy)
static cv::Mat SkImageToCvMat_Opt(const sk_sp<SkImage>& skImage)
{
    // Get the pixel information
    SkPixmap pixmap;
    if (!skImage->peekPixels(&pixmap)) {
        return cv::Mat();
    }

    // Create a Mat that shares memory
    cv::Mat viewMat(pixmap.height(), pixmap.width(), CV_8UC4, pixmap.writable_addr(), pixmap.rowBytes());

    // Channel rearrangement (RGBA -> BGRA)
    if (skImage->colorType() == SkColorType::kRGBA_8888_SkColorType) {
        cv::mixChannels(viewMat, viewMat, { 2,0, 1,1, 0,2, 3,3 }); // Swap the R and B channels in place (OpenCV handles BGRA format by default)
    }    
    return viewMat; // Note: the lifetime of Mat must be shorter than that of SkImage
}

// Optimized scaling function
static sk_sp<SkImage> SkiaResizeWithOpenCV_Opt(const sk_sp<SkImage>& srcImage, int newWidth, int newHeight)
{
    // Get the memory view of the source image
    cv::Mat srcMat = SkImageToCvMat_Opt(srcImage);
    if (srcMat.empty()) {
        return nullptr;
    }

    // Perform scaling using pre-allocated memory
    SkImageInfo dstInfo = SkImageInfo::MakeN32Premul(newWidth, newHeight);
    sk_sp<SkData> dstData = SkData::MakeUninitialized(dstInfo.computeMinByteSize());
    cv::Mat dstMat(newHeight, newWidth, CV_8UC4, dstData->writable_data(), dstInfo.minRowBytes());

    // Perform scaling and convert the color space
    cv::resize(srcMat, dstMat, dstMat.size(), 0, 0, cv::INTER_LINEAR);

    // Channel rearrangement (BGRA->RGBA)
    if (dstInfo.colorType() == SkColorType::kRGBA_8888_SkColorType) {
        cv::mixChannels(dstMat, dstMat, { 2,0, 1,1, 0,2, 3,3 });
    }

    // Create an SkImage that shares memory
    return SkImages::RasterFromData(dstInfo, dstData, dstInfo.minRowBytes());
}

/** Resize a Skia image to fit the drawing target size, to avoid slowdown caused by scaling during drawing (OpenCV implementation)
*/
static bool ResizeSkiaImageByOpenCV(const sk_sp<SkImage>& skImage, const UiRect& rcSrc, const UiRect& rcDest, sk_sp<SkImage>& skNewImage)
{
    if ((skImage == nullptr) || (rcSrc.left != 0) || (rcSrc.top != 0) || (rcSrc.Width() < 10) || (rcSrc.Height() < 10) || (rcDest.Width() < 10) || (rcDest.Height() < 10)) {
        return false;
    }
    if ((rcSrc.Width() == rcDest.Width()) && (rcSrc.Height() == rcDest.Height())) {
        //Same area, no scaling
        return false;
    }
    if ((skImage->width() != rcSrc.Width()) || (skImage->height() != rcSrc.Height())) {
        //Not drawing the whole image
        return false;
    }
    SkPixmap srcPixmap;
    if (!skImage->peekPixels(&srcPixmap)) {
        return false;
    }
    if ((srcPixmap.width() != rcSrc.Width()) || (srcPixmap.height() != rcSrc.Height())) {
        //Has an error
        return false;
    }

    PerformanceStat statPerformance(_T("Render_Skia::DrawSkiaImage::ResizeSkiaImageByOpenCV"));
    skNewImage = SkiaResizeWithOpenCV_Opt(skImage, rcDest.Width(), rcDest.Height());
    return skNewImage != nullptr;
}
#else
/** Resize a Skia image to fit the drawing target size, to avoid slowdown caused by scaling during drawing (stb_image implementation)
*/
static bool ResizeSkiaImageByStbImage(const sk_sp<SkImage>& /*skImage*/, const UiRect& /*rcSrc*/, const UiRect& /*rcDest*/, sk_sp<SkImage>& /*skNewImage*/)
{
    //Disabled: testing showed that with the current code, enabling this noticeably degrades drawing performance, mainly because this function's scaling takes much more time than Skia's own scaling (newer Skia versions may have improved scaling performance).
    return false;
    //if ((skImage == nullptr) || (rcSrc.left != 0) || (rcSrc.top != 0) || (rcSrc.Width() < 10) || (rcSrc.Height() < 10) || (rcDest.Width() < 10) || (rcDest.Height() < 10)) {
    //    return false;
    //}
    //if ((rcSrc.Width() == rcDest.Width()) && (rcSrc.Height() == rcDest.Height())) {
    //    //Same area, no scaling
    //    return false;
    //}
    //if ((skImage->width() != rcSrc.Width()) || (skImage->height() != rcSrc.Height())) {
    //    //Not drawing the whole image
    //    return false;
    //}
    //SkPixmap srcPixmap;
    //if (!skImage->peekPixels(&srcPixmap)) {
    //    return false;
    //}
    //if ((srcPixmap.width() != rcSrc.Width()) || (srcPixmap.height() != rcSrc.Height())) {
    //    //Has an error
    //    return false;
    //}
    //PerformanceStat statPerformance(_T("Render_Skia::DrawSkiaImage::ResizeSkiaImageByStbImage"));

    //sk_sp<SkData> skData = SkData::MakeUninitialized(rcDest.Height() * rcDest.Width() * sizeof(uint32_t));
    //const unsigned char* input_pixels = (const unsigned char*)srcPixmap.addr();
    //int input_w = srcPixmap.width();
    //int input_h = srcPixmap.height();
    //int input_stride_in_bytes = 0;
    //unsigned char* output_pixels = (unsigned char*)skData->writable_data();
    //int output_w = rcDest.Width();
    //int output_h = rcDest.Height();
    //int output_stride_in_bytes = 0;
    //stbir_pixel_layout num_channels = STBIR_RGBA;
    //unsigned char* result = stbir_resize_uint8_linear(input_pixels, input_w, input_h, input_stride_in_bytes,
    //                                                  output_pixels, output_w, output_h, output_stride_in_bytes,
    //                                                  num_channels);
    //if (result != nullptr) {
    //    skNewImage = SkImages::RasterFromData(SkImageInfo::Make(SkISize::Make(output_w, output_h), skImage->imageInfo().colorInfo()), skData, rcDest.Width() * sizeof(uint32_t));
    //    if ((skNewImage->height() == rcDest.Height()) && (skNewImage->width() == rcDest.Width())) {
    //        return (skNewImage != nullptr);
    //    }
    //}
    //return false;
}
#endif //end of OpenCV


void DrawSkiaImage::DrawImage(SkCanvas* pSkCanvas,
                              const UiRect& rcDest,
                              const SkPoint& skPointOrg,
                              const sk_sp<SkImage>& skSrcImage,
                              const UiRect& rcSrc,
                              const SkPaint& skPaint)
{
    if ((pSkCanvas == nullptr) || (skSrcImage == nullptr)) {
        return;
    }
    SkIRect rcSkDestI = { rcDest.left, rcDest.top, rcDest.right, rcDest.bottom };
    SkRect rcSkDest = SkRect::Make(rcSkDestI);
    rcSkDest.offset(skPointOrg);

    SkIRect rcSkSrcI = { rcSrc.left, rcSrc.top, rcSrc.right, rcSrc.bottom };
    SkRect rcSkSrc = SkRect::Make(rcSkSrcI);

#ifdef DUILIB_HAVE_OPENCV
    sk_sp<SkImage> skNewImage;
    if (ResizeSkiaImageByOpenCV(skSrcImage, rcSrc, rcDest, skNewImage)) {
        PerformanceStat statPerformance(_T("Render_Skia::DrawSkiaImage::DrawImage drawImageRect(OpenCV)"));
        rcSkSrc.fRight = rcSkSrc.fLeft + skNewImage->width();
        rcSkSrc.fBottom = rcSkSrc.fTop + skNewImage->height();
        pSkCanvas->drawImageRect(skNewImage, rcSkSrc, rcSkDest, SkSamplingOptions(), &skPaint, SkCanvas::kStrict_SrcRectConstraint);
    }
#else
    sk_sp<SkImage> skNewImage;
    if (ResizeSkiaImageByStbImage(skSrcImage, rcSrc, rcDest, skNewImage)) {
        PerformanceStat statPerformance(_T("Render_Skia::DrawSkiaImage::DrawImage drawImageRect(StbImage)"));
        rcSkSrc.fRight = rcSkSrc.fLeft + skNewImage->width();
        rcSkSrc.fBottom = rcSkSrc.fTop + skNewImage->height();
        pSkCanvas->drawImageRect(skNewImage, rcSkSrc, rcSkDest, SkSamplingOptions(), &skPaint, SkCanvas::kStrict_SrcRectConstraint);
    }
#endif
    else {
        PerformanceStat statPerformance(_T("Render_Skia::DrawSkiaImage::DrawImage drawImageRect(Skia Only)"));
        pSkCanvas->drawImageRect(skSrcImage, rcSkSrc, rcSkDest, SkSamplingOptions(), &skPaint, SkCanvas::kStrict_SrcRectConstraint);
    }
}

} // namespace ui

