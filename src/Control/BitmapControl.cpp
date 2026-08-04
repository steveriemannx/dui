#include "dui/Control/BitmapControl.h"
#include "dui/Render/IRender.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Utils/AttributeUtil.h"
#include "dui/Utils/PerformanceUtil.h"
#include "dui/Utils/FileUtil.h"
#include "dui/Image/ImageAttribute.h"

namespace ui
{

BitmapControl::BitmapControl(Window* pWindow):
    Box(pWindow),
    m_hAlignType(HorAlignType::kAlignLeft),
    m_vAlignType(VerAlignType::kAlignTop),
    m_nBitmapAlpha(255),
    m_bAdaptiveDestRect(false),
    m_bStretchedDrawing(false),
    m_bSupportMultiThread(true)
{
}

BitmapControl::~BitmapControl()
{
    m_pBitmap.reset();
}

DString BitmapControl::GetType() const { return DUI_CTR_BITMAP_CONTROL; }

void BitmapControl::SetAttribute(const DString& strName, const DString& strValue)
{
    if (strName == _T("bitmap_halign")) {
        ASSERT((strValue == _T("left")) || (strValue == _T("center")) || (strValue == _T("right")));
        if (strValue == _T("center")) {
            SetBitmapHAlignType(HorAlignType::kAlignCenter);
        }
        else if (strValue == _T("right")) {
            SetBitmapHAlignType(HorAlignType::kAlignRight);
        }
        else {
            SetBitmapHAlignType(HorAlignType::kAlignLeft);
        }
    }
    else if (strName == _T("bitmap_valign")) {
        ASSERT((strValue == _T("top")) || (strValue == _T("center")) || (strValue == _T("bottom")));
        if (strValue == _T("center")) {
            SetBitmapVAlignType(VerAlignType::kAlignCenter);
        }
        else if (strValue == _T("bottom")) {
            SetBitmapVAlignType(VerAlignType::kAlignBottom);
        }
        else {
            SetBitmapVAlignType(VerAlignType::kAlignTop);
        }
    }
    else if (strName == _T("bitmap_alpha")) {
        SetBitmapAlpha((uint8_t)StringUtil::StringToInt32(strValue));
    }
    else if (strName == _T("bitmap_dest")) {
        UiRect rcDest;
        DString::value_type* pstr = nullptr;
        rcDest.left = StringUtil::StringToInt32(strValue.c_str(), &pstr, 10); ASSERT(pstr);
        AttributeUtil::SkipSepChar(pstr);
        if (*pstr != _T('\0')) {
            rcDest.top = StringUtil::StringToInt32(pstr, &pstr, 10); ASSERT(pstr);
            AttributeUtil::SkipSepChar(pstr);
        }
        if (*pstr != _T('\0')) {
            rcDest.right = StringUtil::StringToInt32(pstr, &pstr, 10); ASSERT(pstr);
            AttributeUtil::SkipSepChar(pstr);
        }
        if (*pstr != _T('\0')) {
            rcDest.bottom = StringUtil::StringToInt32(pstr, &pstr, 10); ASSERT(pstr);
        }
        SetBitmapDest(rcDest, true);
    }
    else if (strName == _T("bitmap_src")) {
        UiRect rcSource;
        AttributeUtil::ParseRectValue(strValue.c_str(), rcSource);
        rcSource.left = std::max(rcSource.left, 0);
        rcSource.top = std::max(rcSource.top, 0);
        SetBitmapSource(rcSource, true);
    }
    else if (strName == _T("bitmap_margin")) {
        UiMargin rcMargin;
        AttributeUtil::ParseMarginValue(strValue.c_str(), rcMargin);
        SetBitmapMargin(rcMargin, true);
    }
    else if (strName == _T("bitmap_adaptive_dest_rect")) {
        SetAdaptiveDestRect(strValue == _T("true"));
    }
    else if (strName == _T("bitmap_stretch")) {
        SetStretchedDrawing(strValue == _T("true"));
    }
    else if (strName == _T("bitmap_multi_thread")) {
        SetSupportMultiThread(strValue == _T("true"));
    }
    else if (strName == _T("bitmap_file")) {
        //Set the associated image file: mainly used for testing
        m_bitmapFile = strValue;
        if (m_pBitmap != nullptr) {
            m_pBitmap.reset();
            CheckLoadBitmapFile();
        }
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

UiSize BitmapControl::EstimateImage(UiSize szAvailable, EstimateImageType estImageType)
{
    UiSize estSize = BaseClass::EstimateImage(szAvailable, estImageType);

    //Load the specified image on demand
    CheckLoadBitmapFile();

    int32_t nImageInfoWidth = 0;
    int32_t nImageInfoHeight = 0;
    GetBitmapSize(nImageInfoWidth, nImageInfoHeight);
    if ((nImageInfoWidth <= 0) || (nImageInfoHeight <= 0)) {
        //No image data
        return estSize;
    }

    //The padding of the control itself
    const UiPadding rcControlPadding = GetControlPadding();
    UiRect rcDest;
    bool hasDestAttr = false;
    if (m_rcDest != nullptr) {
        //Use the destination area specified in the configuration (already DPI-adapted per the configuration): take priority as the basis of the image size
        rcDest = *m_rcDest;
        if (rcDest.left < 0) {
            rcDest.left = 0;
        }
        if (rcDest.top < 0) {
            rcDest.top = 0;
        }
        if (rcDest.right <= rcDest.left) {
            rcDest.right = rcDest.left + nImageInfoWidth;
        }
        if (rcDest.bottom <= rcDest.top) {
            rcDest.bottom = rcDest.top + nImageInfoHeight;
        }
        hasDestAttr = true;
    }
    UiRect rcSource;
    if (m_rcSource != nullptr) {
        rcSource = *m_rcSource;
    }
    if (rcSource.right > (int32_t)nImageInfoWidth) {
        rcSource.right = (int32_t)nImageInfoWidth;
    }
    if (rcSource.bottom > (int32_t)nImageInfoHeight) {
        rcSource.bottom = (int32_t)nImageInfoHeight;
    }

    UiSize imageSize;
    if (rcDest.Width() > 0) {
        //Take 0 as the base point and right as the boundary
        imageSize.cx = rcDest.right;
    }
    else if (rcSource.Width() > 0) {
        imageSize.cx = rcSource.Width();
    }
    else {
        imageSize.cx = nImageInfoWidth;
    }

    if (rcDest.Height() > 0) {
        //Take 0 as the base point and bottom as the boundary
        imageSize.cy = rcDest.bottom;
    }
    else if (rcSource.Height() > 0) {
        imageSize.cy = rcSource.Height();
    }
    else {
        imageSize.cy = nImageInfoHeight;
    }

    if (!hasDestAttr) {
        //If there is no rcDest attribute, the image margin needs to be added (the margin attribute of the image itself)
        UiMargin rcImageMargin;
        if (m_rcMargin != nullptr) {
            rcImageMargin = *m_rcMargin;
        }
        imageSize.cx += (rcImageMargin.left + rcImageMargin.right);
        imageSize.cy += (rcImageMargin.top + rcImageMargin.bottom);
    }
    if (m_bAdaptiveDestRect) {
        //Automatically adapt to the destination area (scale the image proportionally): adjust the drawing area based on the image size
        const int32_t nImageWidth = rcSource.Width();
        const int32_t nImageHeight = rcSource.Height();
        UiRect rcControlDest = UiRect(0, 0,
                                      szAvailable.cx - rcControlPadding.left - rcControlPadding.right,
                                      szAvailable.cy - rcControlPadding.top - rcControlPadding.bottom);
        rcControlDest.Validate();
        if (rcControlDest.Width() > 0 && rcControlDest.Height() > 0) {
            DString hAlign = _T("left");
            if (m_hAlignType == HorAlignType::kAlignCenter) {
                hAlign = _T("center");
            }
            else if (m_hAlignType == HorAlignType::kAlignRight) {
                hAlign = _T("right");
            }
            DString vAlign = _T("top");
            if (m_vAlignType == VerAlignType::kAlignCenter) {
                vAlign = _T("center");
            }
            else if (m_vAlignType == VerAlignType::kAlignBottom) {
                vAlign = _T("bottom");
            }
            rcControlDest = ImageAttribute::CalculateAdaptiveRect(nImageWidth, nImageHeight, rcControlDest, hAlign, vAlign);
            imageSize.cx = rcControlDest.Width();
            imageSize.cy = rcControlDest.Height();
        }
    }

    //For the image size, the padding of the control needs to be added
    if (imageSize.cx > 0) {
        imageSize.cx += (rcControlPadding.left + rcControlPadding.right);
    }
    if (imageSize.cy > 0) {
        imageSize.cy += (rcControlPadding.top + rcControlPadding.bottom);
    }
    if ((estImageType == EstimateImageType::kBoth) || (estImageType == EstimateImageType::kWidthOnly)) {
        //Width is calculated automatically
        estSize.cx = std::max(estSize.cx, imageSize.cx);
    }
    if ((estImageType == EstimateImageType::kBoth) || (estImageType == EstimateImageType::kHeightOnly)) {
        //Height is calculated automatically
        estSize.cy = std::max(estSize.cy, imageSize.cy);
    }
    return estSize;
}

void BitmapControl::ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale)
{
    BaseClass::ChangeDpiScale(nOldDpiScale, nNewDpiScale);
    if (!Dpi().CheckDisplayScaleFactor(nNewDpiScale)) {
        return;
    }
    if (HasBitmapDest()) {
        UiRect rcDest = GetBitmapDest();
        rcDest = Dpi().GetScaleRect(rcDest, nOldDpiScale);
        SetBitmapDest(rcDest, false);
    }
    if (HasBitmapSource()) {
        UiRect rcSource = GetBitmapSource();
        rcSource = Dpi().GetScaleRect(rcSource, nOldDpiScale);
        SetBitmapSource(rcSource, false);
    }
    if (HasBitmapMargin()) {
        UiMargin rcMargin = GetBitmapMargin();
        rcMargin = Dpi().GetScaleMargin(rcMargin, nOldDpiScale);
        SetBitmapMargin(rcMargin, false);
    }
}

void BitmapControl::SetBitmapHAlignType(HorAlignType hAlignType)
{
    if (m_hAlignType != hAlignType) {
        m_hAlignType = hAlignType;
        //Redraw the image
        Invalidate();
    }
}

HorAlignType BitmapControl::GetBitmapHAlignType() const
{
    return m_hAlignType;
}

void BitmapControl::SetBitmapVAlignType(VerAlignType vAlignType)
{
    if (m_vAlignType != vAlignType) {
        m_vAlignType = vAlignType;
        //Redraw the image
        Invalidate();
    }
}

VerAlignType BitmapControl::GetBitmapVAlignType() const
{
    return m_vAlignType;
}

void BitmapControl::SetBitmapAlpha(uint8_t nBitmapAlpha)
{
    if (m_nBitmapAlpha != nBitmapAlpha) {
        m_nBitmapAlpha = nBitmapAlpha;
        //Redraw the image
        Invalidate();
    }
}

uint8_t BitmapControl::GetBitmapAlpha() const
{
    return m_nBitmapAlpha;
}

void BitmapControl::SetBitmapDest(UiRect rcDest, bool bNeedDpiScale)
{
    if (bNeedDpiScale) {
        Dpi().ScaleRect(rcDest);
    }
    rcDest.Validate();
    if (m_rcDest == nullptr) {
        m_rcDest = std::make_unique<UiRect>(rcDest);
        //Redraw the image
        Invalidate();
    }
    else {
        if (*m_rcDest != rcDest) {
            *m_rcDest = rcDest;
            //Redraw the image
            Invalidate();
        }
    }
}

UiRect BitmapControl::GetBitmapDest() const
{
    if (m_rcDest != nullptr) {
        return *m_rcDest;
    }
    return UiRect();
}

bool BitmapControl::HasBitmapDest() const
{
    return (m_rcDest != nullptr);
}

void BitmapControl::SetBitmapSource(UiRect rcSource, bool bNeedDpiScale)
{
    if (bNeedDpiScale) {
        Dpi().ScaleRect(rcSource);
    }
    rcSource.Validate();
    if (m_rcSource == nullptr) {
        m_rcSource = std::make_unique<UiRect>(rcSource);
        //Redraw the image
        Invalidate();
    }
    else {
        if (*m_rcSource != rcSource) {
            *m_rcSource = rcSource;
            //Redraw the image
            Invalidate();
        }
    }
}

UiRect BitmapControl::GetBitmapSource() const
{
    if (m_rcSource != nullptr) {
        return *m_rcSource;
    }
    return UiRect();
}

bool BitmapControl::HasBitmapSource() const
{
    return (m_rcSource != nullptr);
}

void BitmapControl::SetBitmapMargin(UiMargin rcMargin, bool bNeedDpiScale)
{
    if (bNeedDpiScale) {
        Dpi().ScaleMargin(rcMargin);
    }
    if (m_rcMargin == nullptr) {
        m_rcMargin = std::make_unique<UiMargin>(rcMargin);
        //Redraw the image
        Invalidate();
    }
    else {
        if (*m_rcMargin != rcMargin) {
            *m_rcMargin = rcMargin;
            //Redraw the image
            Invalidate();
        }
    }
}

UiMargin BitmapControl::GetBitmapMargin() const
{
    if (m_rcMargin != nullptr) {
        return *m_rcMargin;
    }
    return UiMargin();
}

bool BitmapControl::HasBitmapMargin() const
{
    return (m_rcMargin != nullptr);
}

void BitmapControl::SetAdaptiveDestRect(bool bAdaptiveDestRect)
{
    if (m_bAdaptiveDestRect != bAdaptiveDestRect) {
        m_bAdaptiveDestRect = bAdaptiveDestRect;
        //Redraw the image
        Invalidate();
    }
}

bool BitmapControl::IsAdaptiveDestRect() const
{
    return m_bAdaptiveDestRect;
}

void BitmapControl::SetStretchedDrawing(bool bStretchedDrawing)
{
    if (m_bStretchedDrawing != bStretchedDrawing) {
        m_bStretchedDrawing = bStretchedDrawing;
        //Redraw the image
        Invalidate();
    }
}

bool BitmapControl::IsStretchedDrawing() const
{
    return m_bStretchedDrawing;
}

void BitmapControl::SetSupportMultiThread(bool bSupportMultiThread)
{
    m_bSupportMultiThread = bSupportMultiThread;
}

bool BitmapControl::IsSupportMultiThread() const
{
    return m_bSupportMultiThread;
}

void BitmapControl::Paint(IRender* pRender, const UiRect& rcPaint)
{
    BaseClass::Paint(pRender, rcPaint);

    //Draw the image
    PaintBitmap(pRender, rcPaint);
}

void BitmapControl::CheckLoadBitmapFile()
{
    if (!m_bitmapFile.empty() && (m_pBitmap == nullptr)) {
        //Load the specified image: no optimization when loading image data, only for testing functionality
        ImageDecodeParam decodeParam;
        FilePath resPath = ui::GlobalManager::Instance().GetResourcePath();
        resPath += m_bitmapFile.c_str();
        decodeParam.m_imageFilePath = resPath;
        decodeParam.m_pFileData = std::make_shared<std::vector<uint8_t>>();
        decodeParam.m_fImageSizeScale = Dpi().GetDisplayScale();
        FileUtil::ReadFileData(decodeParam.m_imageFilePath, *decodeParam.m_pFileData);
        std::shared_ptr<IBitmap> pBitmap = GlobalManager::Instance().ImageDecoders().DecodeImageData(decodeParam);
        ASSERT(pBitmap != nullptr);
        if (pBitmap != nullptr) {
            SetBitmapDataWithCopy(pBitmap.get());
        }
    }
}

bool BitmapControl::SetBitmapData(int32_t nWidth, int32_t nHeight, const uint8_t* pPixelBits, int32_t nPixelBitsSize)
{
    ASSERT((pPixelBits != nullptr) && (nPixelBitsSize > 0) && (nWidth > 0) && (nHeight > 0));
    if ((pPixelBits == nullptr) || (nPixelBitsSize <= 0) || (nWidth <= 0) || (nHeight <= 0)) {
        return false;
    }
    ASSERT(nPixelBitsSize == nHeight * nWidth * (int32_t)sizeof(uint32_t));
    if (nPixelBitsSize != nHeight * nWidth * (int32_t)sizeof(uint32_t)) {
        return false;
    }

    //When multi-threading is supported, lock before operating on m_pBitmap
    std::unique_ptr<std::unique_lock<std::mutex>> spMutexLock;
    if (IsSupportMultiThread()) {
        spMutexLock = std::make_unique<std::unique_lock<std::mutex>>(m_bitmapMutex);
    }

    if (m_pBitmap == nullptr) {
        IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
        ASSERT(pRenderFactory != nullptr);
        if (pRenderFactory != nullptr) {
            m_pBitmap.reset(pRenderFactory->CreateBitmap());
        }
    }
    ASSERT(m_pBitmap != nullptr);
    if (m_pBitmap == nullptr) {
        return false;
    }
    bool bRet = false;
    if (((int32_t)m_pBitmap->GetWidth() == nWidth) && ((int32_t)m_pBitmap->GetHeight() == nHeight)) {
        void* pBits = m_pBitmap->LockPixelBits();
        if (pBits != nullptr) {
            //Copy the image data to the bitmap
            ::memcpy(pBits, pPixelBits, nWidth * nHeight * sizeof(uint32_t));
            m_pBitmap->UnLockPixelBits();
            bRet = true;
        }
    }
    else {
        bRet = m_pBitmap->Init(nWidth, nHeight, pPixelBits);
    }
    if (bRet) {
        //Redraw the image
        Invalidate();
    }
    return bRet;
}

bool BitmapControl::SetBitmapDataWithCopy(IBitmap* pBitmap)
{
    if (pBitmap == nullptr) {
        return false;
    }
    int32_t nWidth = pBitmap->GetWidth();
    int32_t nHeight = pBitmap->GetHeight();
    const uint8_t* pPixelBits = (const uint8_t*)pBitmap->LockPixelBits();
    int32_t nPixelBitsSize = nHeight * nWidth * sizeof(uint32_t);
    return SetBitmapData(nWidth, nHeight, pPixelBits, nPixelBitsSize);
}

void BitmapControl::ClearBitmapData()
{
    //When multi-threading is supported, lock before operating on m_pBitmap
    std::unique_ptr<std::unique_lock<std::mutex>> spMutexLock;
    if (IsSupportMultiThread()) {
        spMutexLock = std::make_unique<std::unique_lock<std::mutex>>(m_bitmapMutex);
    }
    if (m_pBitmap != nullptr) {
        m_pBitmap.reset();
        //Redraw the image
        Invalidate();
    }    
}

bool BitmapControl::HasBitmapData()
{
    //When multi-threading is supported, lock before operating on m_pBitmap
    std::unique_ptr<std::unique_lock<std::mutex>> spMutexLock;
    if (IsSupportMultiThread()) {
        spMutexLock = std::make_unique<std::unique_lock<std::mutex>>(m_bitmapMutex);
    }
    return (m_pBitmap != nullptr) && (m_pBitmap->GetWidth() > 0) && (m_pBitmap->GetHeight() > 0);
}

void BitmapControl::GetBitmapSize(int32_t& nImageWidth, int32_t& nImageHeight)
{
    //When multi-threading is supported, lock before operating on m_pBitmap
    std::unique_ptr<std::unique_lock<std::mutex>> spMutexLock;
    if (IsSupportMultiThread()) {
        spMutexLock = std::make_unique<std::unique_lock<std::mutex>>(m_bitmapMutex);
    }
    nImageWidth = 0;
    nImageHeight = 0;
    IBitmap* pBitmap = m_pBitmap.get();
    if (pBitmap != nullptr) {
        nImageWidth = pBitmap->GetWidth();
        nImageHeight = pBitmap->GetHeight();
    }
}

void BitmapControl::PaintBitmap(IRender* pRender, const UiRect& rcPaint)
{
    GlobalManager::Instance().AssertUIThread();

    //Load the specified image on demand
    CheckLoadBitmapFile();

    //Measure the performance of drawing the image
    PerformanceStat statPerformance(_T("BitmapControl::Paint"));

    //When multi-threading is supported, lock before operating on m_pBitmap
    std::unique_ptr<std::unique_lock<std::mutex>> spMutexLock;
    if (IsSupportMultiThread()) {
        spMutexLock = std::make_unique<std::unique_lock<std::mutex>>(m_bitmapMutex);
    }

    int32_t nImageInfoWidth = 0;
    int32_t nImageInfoHeight = 0;
    IBitmap* pBitmap = m_pBitmap.get();
    if (pBitmap != nullptr) {
        nImageInfoWidth = pBitmap->GetWidth();
        nImageInfoHeight = pBitmap->GetHeight();
    }
    if ((nImageInfoWidth <= 0) || (nImageInfoHeight <= 0)) {
        //No image data
        return;
    }

    UiRect rcDest;
    if (m_rcDest != nullptr) {
        //Use the destination area specified in the configuration
        rcDest = *m_rcDest;
        if (rcDest.left < 0) {
            rcDest.left = 0;
        }
        if (rcDest.top < 0) {
            rcDest.top = 0;
        }
        if (rcDest.right <= rcDest.left) {
            rcDest.right = rcDest.left + nImageInfoWidth;
        }
        if (rcDest.bottom <= rcDest.top) {
            rcDest.bottom = rcDest.top + nImageInfoHeight;
        }
        rcDest.Offset(GetRect().left, GetRect().top);
    }
    else {
        rcDest = GetRect();
        rcDest.Deflate(GetControlPadding());
    }

    //The margin of the image (subtracted)
    if (m_rcMargin != nullptr) {
        rcDest.Deflate(*m_rcMargin);
    }

    UiRect rcTemp;
    if (!UiRect::Intersect(rcTemp, rcDest, GetRect())) {
        return;//rcDest has no intersection with the target area, cannot draw
    }

    UiRect rcSource;
    if (m_rcSource != nullptr) {
        rcSource = *m_rcSource;        
        if (rcSource.right > (int32_t)m_pBitmap->GetWidth()) {
            rcSource.right = (int32_t)m_pBitmap->GetWidth();
        }
        if (rcSource.bottom > (int32_t)m_pBitmap->GetHeight()) {
            rcSource.bottom = (int32_t)m_pBitmap->GetHeight();
        }
    }
    else {
        rcSource.left = 0;
        rcSource.top = 0;
        rcSource.right = rcSource.left + m_pBitmap->GetWidth();
        rcSource.bottom = rcSource.top + m_pBitmap->GetHeight();
    }

    rcSource.Validate();
    if (rcSource.IsEmpty()) {
        return;//No valid data area
    }

    const int32_t nImageWidth = rcSource.Width();
    const int32_t nImageHeight = rcSource.Height();

    bool bAdaptiveDestRect = m_bAdaptiveDestRect; //Automatically adapt to the destination area (draw with the specified alignment after proportional scaling)
    bool bStretchedDrawing = m_bStretchedDrawing; //Stretch drawing, its priority is lower than the bAdaptiveDestRect option
    if (!bAdaptiveDestRect && !bStretchedDrawing) {
        //When the image size is larger than the destination area size, always set to automatically adapt to the destination area
        if ((nImageWidth > rcDest.Width()) || (nImageHeight > rcDest.Height())) {
            bAdaptiveDestRect = true;
        }
    }
    if (bAdaptiveDestRect) {
        //Automatically adapt to the destination area (scale the image proportionally): adjust the drawing area based on the image size
        DString hAlign = _T("left");
        if (m_hAlignType == HorAlignType::kAlignCenter) {
            hAlign = _T("center");
        }
        else if (m_hAlignType == HorAlignType::kAlignRight) {
            hAlign = _T("right");
        }
        DString vAlign = _T("top");
        if (m_vAlignType == VerAlignType::kAlignCenter) {
            vAlign = _T("center");
        }
        else if (m_vAlignType == VerAlignType::kAlignBottom) {
            vAlign = _T("bottom");
        }
        rcDest = ImageAttribute::CalculateAdaptiveRect(nImageWidth, nImageHeight, rcDest, hAlign, vAlign);
    }
    else if (!bStretchedDrawing) {
        //Not stretch drawing, handle the alignment
        if (m_hAlignType == HorAlignType::kAlignLeft) {
            rcDest.right = rcDest.left + nImageWidth;
        }
        else if (m_hAlignType == HorAlignType::kAlignCenter) {
            rcDest.left = rcDest.CenterX() - nImageWidth / 2;
            rcDest.right = rcDest.left + nImageWidth;
        }
        else if (m_hAlignType == HorAlignType::kAlignRight) {
            rcDest.left = rcDest.right - nImageWidth;
        }

        if (m_vAlignType == VerAlignType::kAlignTop) {
            rcDest.bottom = rcDest.top + nImageHeight;
        }
        else if (m_vAlignType == VerAlignType::kAlignCenter) {
            rcDest.top = rcDest.CenterY() - nImageHeight / 2;
            rcDest.bottom = rcDest.top + nImageHeight;
        }
        else if (m_vAlignType == VerAlignType::kAlignBottom) {
            rcDest.top = rcDest.bottom - nImageHeight;
        }
    }
    pRender->DrawImage(rcPaint, m_pBitmap.get(), rcDest, rcSource, m_nBitmapAlpha);
}

}//namespace ui
