#include "dui/Image/ImageAttribute.h"
#include "dui/Core/DpiManager.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Utils/AttributeUtil.h"

namespace ui 
{
ImageAttribute::ImageAttribute():
    m_rcDest(nullptr),
    m_rcMargin(nullptr),
    m_rcSource(nullptr),
    m_rcCorner(nullptr)
{
    Init();
}

ImageAttribute::ImageAttribute(const ImageAttribute& r) :
    m_rcDest(nullptr),
    m_rcMargin(nullptr),
    m_rcSource(nullptr),
    m_rcCorner(nullptr)
{
    Init();
    *this = r;
}

ImageAttribute& ImageAttribute::operator=(const ImageAttribute& r)
{
    if (&r == this) {
        return *this;
    }

    m_sImageString = r.m_sImageString;
    m_sImagePath = r.m_sImagePath;
    m_sImageName = r.m_sImageName;
    m_srcWidth = r.m_srcWidth;
    m_srcHeight = r.m_srcHeight;

    m_bImageDpiScaleEnabled = r.m_bImageDpiScaleEnabled;
    m_bDestDpiScaleEnabled = r.m_bDestDpiScaleEnabled;
    m_rcMarginScale = r.m_rcMarginScale;

    m_hAlign = r.m_hAlign;
    m_vAlign = r.m_vAlign;

    m_bFade = r.m_bFade;
    m_bWindowShadowMode = r.m_bWindowShadowMode;
    m_bAutoPlay = r.m_bAutoPlay;
    m_bAsyncLoad = r.m_bAsyncLoad;
    m_nPlayCount = r.m_nPlayCount;
    m_nIconSize = r.m_nIconSize;
    m_bIconAsAnimation = r.m_bIconAsAnimation;
    m_nIconFrameDelayMs = r.m_nIconFrameDelayMs;
    m_fPagMaxFrameRate = r.m_fPagMaxFrameRate;
    m_bPaintEnabled = r.m_bPaintEnabled;
    m_bAssertEnabled = r.m_bAssertEnabled;
    m_bAdaptiveDestRect = r.m_bAdaptiveDestRect;

    if (r.m_pTiledDrawParam != nullptr) {
        if (m_pTiledDrawParam == nullptr) {
            m_pTiledDrawParam = std::make_unique<TiledDrawParam>();
        }
        *m_pTiledDrawParam = *r.m_pTiledDrawParam;
    }
    else {
        m_pTiledDrawParam.reset();
    }

    if (r.m_rcDest != nullptr) {
        if (m_rcDest == nullptr) {
            m_rcDest = new UiRect;
        }
        *m_rcDest = *r.m_rcDest;
    }
    else {
        if (m_rcDest != nullptr) {
            delete m_rcDest;
            m_rcDest = nullptr;
        }
    }

    if (r.m_rcMargin != nullptr) {
        if (m_rcMargin == nullptr) {
            m_rcMargin = new UiMargin16;
        }
        *m_rcMargin = *r.m_rcMargin;
    }
    else {
        if (m_rcMargin != nullptr) {
            delete m_rcMargin;
            m_rcMargin = nullptr;
        }
    }

    if (r.m_rcSource != nullptr) {
        if (m_rcSource == nullptr) {
            m_rcSource = new UiRect;
        }
        *m_rcSource = *r.m_rcSource;
    }
    else {
        if (m_rcSource != nullptr) {
            delete m_rcSource;
            m_rcSource = nullptr;
        }
    }

    if (r.m_rcCorner != nullptr) {
        if (m_rcCorner == nullptr) {
            m_rcCorner = new UiRect;
        }
        *m_rcCorner = *r.m_rcCorner;
    }
    else {
        if (m_rcCorner != nullptr) {
            delete m_rcCorner;
            m_rcCorner = nullptr;
        }
    }

    return *this;
}

void ImageAttribute::Init()
{
    m_sImageString.clear();
    m_sImagePath.clear();
    m_sImageName.clear();
    m_srcWidth.clear();
    m_srcHeight.clear();

    m_bImageDpiScaleEnabled = true;
    m_bDestDpiScaleEnabled = true;
    m_rcMarginScale = 0;

    m_hAlign.clear();
    m_vAlign.clear();

    m_bFade = 0xFF;
    m_bWindowShadowMode = false;
    m_bAutoPlay = true;
    m_bAsyncLoad = GlobalManager::Instance().Image().IsImageAsyncLoad();
    m_nPlayCount = -1;    
    m_nIconSize = 0;
    m_bIconAsAnimation = false;
    m_nIconFrameDelayMs = 1000;
    m_fPagMaxFrameRate = 30.0f;
    m_bPaintEnabled = true;
    m_bAssertEnabled = true;
    m_bAdaptiveDestRect = false;
    m_pTiledDrawParam.reset();

    if (m_rcDest != nullptr) {
        delete m_rcDest;
        m_rcDest = nullptr;
    }
    if (m_rcSource != nullptr) {
        delete m_rcSource;
        m_rcSource = nullptr;
    }
    if (m_rcMargin != nullptr) {
        delete m_rcMargin;
        m_rcMargin = nullptr;
    }
    if (m_rcCorner != nullptr) {
        delete m_rcCorner;
        m_rcCorner = nullptr;
    }
    m_rcMarginScale = 0;
}

ImageAttribute::~ImageAttribute()
{
    if (m_rcDest != nullptr) {
        delete m_rcDest;
        m_rcDest = nullptr;
    }
    if (m_rcSource != nullptr) {
        delete m_rcSource;
        m_rcSource = nullptr;
    }
    if (m_rcMargin != nullptr) {
        delete m_rcMargin;
        m_rcMargin = nullptr;
    }
    if (m_rcCorner != nullptr) {
        delete m_rcCorner;
        m_rcCorner = nullptr;
    }
    m_pTiledDrawParam.reset();
}

void ImageAttribute::InitByImageString(const DString& strImageString, const DpiManager& dpi)
{
    Init();
    m_sImageString = strImageString;
    m_sImagePath = strImageString;
    ModifyAttribute(strImageString, dpi);
}

void ImageAttribute::ModifyAttribute(const DString& strImageString, const DpiManager& dpi)
{
    // Note: the image attribute documentation (docs/Global.md) contains detailed descriptions of each attribute
    if (strImageString.find(_T('=')) == DString::npos) {
        //No equals sign, which means there are no attributes, return directly
        return;
    }
    std::vector<std::pair<DString, DString>> attributeList;
    AttributeUtil::ParseAttributeList(strImageString, _T('\''), attributeList);

    ImageAttribute& imageAttribute = *this;
    imageAttribute.m_bImageDpiScaleEnabled = true;
    imageAttribute.m_bDestDpiScaleEnabled = true;
    for (const auto& attribute : attributeList) {
        const DString& name = attribute.first;
        const DString& value = attribute.second;
        if (name.empty() || value.empty()) {
            continue;
        }
        if (name == _T("file") || name == _T("res")) {
            //Image resource file name, used to load the image resource according to this setting
            imageAttribute.m_sImagePath = value;
        }
        else if (name == _T("name")) {
            //Image resource name
            imageAttribute.m_sImageName = value;
        }
        else if (name == _T("width")) {
            //Set the image width, can enlarge or shrink the image: pixels or percentage %, e.g., 300, or 30%
            imageAttribute.m_srcWidth = value;
        }
        else if (name == _T("height")) {
            //Set the image height, can enlarge or shrink the image: pixels or percentage %, e.g., 200, or 30%
            imageAttribute.m_srcHeight = value;
        }
        else if ((name == _T("src")) || (name == _T("source"))) {
            //Image source area setting: can be used to include only part of the source image content (for example, through this mechanism, the state images of a button can be combined into one large image, making it convenient to manage image resources)
            if (imageAttribute.m_rcSource == nullptr) {
                imageAttribute.m_rcSource = new UiRect;
            }
            AttributeUtil::ParseRectValue(value.c_str(), *imageAttribute.m_rcSource);
            imageAttribute.m_rcSource->left = std::max(imageAttribute.m_rcSource->left, 0);
            imageAttribute.m_rcSource->top = std::max(imageAttribute.m_rcSource->top, 0);
        }
        else if (name == _T("corner")) {
            //The rounded corner attributes of the image; if this attribute is set, the image is drawn in nine-patch mode when drawing:
            //    The four corners are not stretched, the four edges are stretched, and the middle part can be stretched or tiled according to the xtiled and ytiled attributes
            if (imageAttribute.m_rcCorner == nullptr) {
                imageAttribute.m_rcCorner = new UiRect;
            }
            AttributeUtil::ParseRectValue(value.c_str(), *imageAttribute.m_rcCorner);
            imageAttribute.m_rcCorner->left = std::max(imageAttribute.m_rcCorner->left, 0);
            imageAttribute.m_rcCorner->top = std::max(imageAttribute.m_rcCorner->top, 0);
            imageAttribute.m_rcCorner->right = std::max(imageAttribute.m_rcCorner->right, 0);
            imageAttribute.m_rcCorner->bottom = std::max(imageAttribute.m_rcCorner->bottom, 0);
        }
        else if (name == _T("window_shadow_mode")) {
            //When drawing in nine-patch mode, the middle part is not drawn (e.g., for window shadows, only the border needs to be drawn, not the middle part)
            imageAttribute.m_bWindowShadowMode = (value == _T("true"));
        }
        else if ((name == _T("dpi_scale")) || (name == _T("dpiscale"))) {
            //When loading the image, scale the image size according to the DPI
            imageAttribute.m_bImageDpiScaleEnabled = (value == _T("true"));
        }
        else if ((name == _T("dest_scale")) || (name == _T("destscale"))) {
            //When loading, scale the dest attribute according to the DPI, only valid when the dest attribute is set (it will affect the dest attribute)
            //When drawing (used internally), controls whether DPI scaling is performed on the dest attribute
            imageAttribute.m_bDestDpiScaleEnabled = (value == _T("true"));
        }
        else if (name == _T("dest")) {
            //Set the destination area, which is relative to the Rect area of the owning control
            if (!value.empty()) {
                if (imageAttribute.m_rcDest == nullptr) {
                    imageAttribute.m_rcDest = new UiRect;
                }
                UiRect& rect = *imageAttribute.m_rcDest;
                DString::value_type* pstr = nullptr;
                rect.left = StringUtil::StringToInt32(value.c_str(), &pstr, 10); ASSERT(pstr);
                AttributeUtil::SkipSepChar(pstr);
                if (*pstr != _T('\0')) {
                    rect.top = StringUtil::StringToInt32(pstr, &pstr, 10); ASSERT(pstr);
                    AttributeUtil::SkipSepChar(pstr);
                }
                if (*pstr != _T('\0')) {
                    rect.right = StringUtil::StringToInt32(pstr, &pstr, 10); ASSERT(pstr);
                    AttributeUtil::SkipSepChar(pstr);
                }
                if (*pstr != _T('\0')) {
                    rect.bottom = StringUtil::StringToInt32(pstr, &pstr, 10); ASSERT(pstr);
                }
            }
        }
        else if ((name == _T("margin") || (name == _T("padding")))) {
            //Set the outer margin of the image in the destination area (the old name "padding" is kept for compatibility)
            UiMargin margin;
            AttributeUtil::ParseMarginValue(value.c_str(), margin);
            imageAttribute.SetImageMargin(margin, true, dpi);
        }
        else if (name == _T("halign")) {
            //Set the horizontal alignment in the destination area
            ASSERT((value == _T("left")) || (value == _T("center")) || (value == _T("right")));
            if ((value == _T("left")) || (value == _T("center")) || (value == _T("right"))) {
                imageAttribute.m_hAlign = value;
            }
        }
        else if (name == _T("valign")) {
            //Set the vertical alignment in the destination area
            ASSERT((value == _T("top")) || (value == _T("center")) || (value == _T("bottom")));
            if ((value == _T("top")) || (value == _T("center")) || (value == _T("bottom"))) {
                imageAttribute.m_vAlign = value;
            }
        }
        else if (name == _T("fade")) {
            //The opacity of the image
            imageAttribute.m_bFade = (uint8_t)StringUtil::StringToInt32(value);
        }
        else if (name == _T("xtiled")) {
            //Horizontal tiling
            if (m_pTiledDrawParam == nullptr) {
                m_pTiledDrawParam = std::make_unique<TiledDrawParam>();
            }
            imageAttribute.m_pTiledDrawParam->m_bTiledX = (value == _T("true"));
        }
        else if ((name == _T("full_xtiled")) || (name == _T("fullxtiled"))) {
            //When tiling horizontally, ensure the whole image is drawn
            if (m_pTiledDrawParam == nullptr) {
                m_pTiledDrawParam = std::make_unique<TiledDrawParam>();
            }
            imageAttribute.m_pTiledDrawParam->m_bFullTiledX = (value == _T("true"));
        }
        else if (name == _T("ytiled")) {
            //Vertical tiling
            if (m_pTiledDrawParam == nullptr) {
                m_pTiledDrawParam = std::make_unique<TiledDrawParam>();
            }
            imageAttribute.m_pTiledDrawParam->m_bTiledY = (value == _T("true"));
        }
        else if ((name == _T("full_ytiled")) || (name == _T("fullytiled"))) {
            //When tiling vertically, ensure the whole image is drawn
            if (m_pTiledDrawParam == nullptr) {
                m_pTiledDrawParam = std::make_unique<TiledDrawParam>();
            }
            imageAttribute.m_pTiledDrawParam->m_bFullTiledY = (value == _T("true"));
        }
        else if ((name == _T("tiled_margin")) || (name == _T("tiledmargin"))) {
            //When drawing tiled, the interval between each tiled image, including horizontal tiling and vertical tiling
            if (m_pTiledDrawParam == nullptr) {
                m_pTiledDrawParam = std::make_unique<TiledDrawParam>();
            }
            imageAttribute.m_pTiledDrawParam->m_nTiledMarginX = StringUtil::StringToInt32(value);
            imageAttribute.m_pTiledDrawParam->m_nTiledMarginY = imageAttribute.m_pTiledDrawParam->m_nTiledMarginX;
        }
        else if (name == _T("tiled_margin_x")) {
            //When drawing tiled, the interval between each tiled image, horizontal tiling
            if (m_pTiledDrawParam == nullptr) {
                m_pTiledDrawParam = std::make_unique<TiledDrawParam>();
            }
            imageAttribute.m_pTiledDrawParam->m_nTiledMarginX = StringUtil::StringToInt32(value);
        }
        else if (name == _T("tiled_margin_y")) {
            //When drawing tiled, the interval between each tiled image, vertical tiling
            if (m_pTiledDrawParam == nullptr) {
                m_pTiledDrawParam = std::make_unique<TiledDrawParam>();
            }
            imageAttribute.m_pTiledDrawParam->m_nTiledMarginY = StringUtil::StringToInt32(value);
        }
        else if (name == _T("tiled_padding")) {
            if (m_pTiledDrawParam == nullptr) {
                m_pTiledDrawParam = std::make_unique<TiledDrawParam>();
            }
            UiPadding rcPadding;
            AttributeUtil::ParsePaddingValue(value.c_str(), rcPadding);
            m_pTiledDrawParam->m_rcTiledPadding = rcPadding;
        }
        else if ((name == _T("icon_size")) || (name == _T("iconsize"))) {
            //Specify the image size of the ICO file to load (only valid when the image file is an ICO file)
            imageAttribute.m_nIconSize = (uint32_t)StringUtil::StringToInt32(value);
        }
        else if (name == _T("icon_as_animation")) {
            //If it is an ICO file, specify whether to load as a multi-frame image (displayed as an animation image)
            imageAttribute.m_bIconAsAnimation = (value == _T("true"));
        }
        else if (name == _T("icon_frame_delay")) {
            //If it is an ICO file, when displayed as a multi-frame image, the playback time interval of each frame, milliseconds
            imageAttribute.m_nIconFrameDelayMs = StringUtil::StringToInt32(value);
            if (imageAttribute.m_nIconFrameDelayMs <= 0) {
                imageAttribute.m_nIconFrameDelayMs = 1000;
            }
        }
        else if (name == _T("pag_max_frame_rate")) {
            //If it is a PAG file, used to specify the frame rate of the animation, the default is 30.0f
            imageAttribute.m_fPagMaxFrameRate = (float)StringUtil::StringToInt32(value);
        }
        else if ((name == _T("play_count")) || (name == _T("playcount"))) {
            //If it is an animation image, the meaning of the value
            //  -1: Play forever
            //  0 : No valid play count; use the default value of the image
            // > 0: A specific number of plays; stop playing after reaching the play count
            imageAttribute.m_nPlayCount = StringUtil::StringToInt32(value);
            if (imageAttribute.m_nPlayCount < 0) {
                imageAttribute.m_nPlayCount = -1;
            }
        }
        else if (name == _T("auto_play")) {
            //If it is an animation image, whether to auto-play
            imageAttribute.m_bAutoPlay = (value == _T("true"));
        }
        else if (name == _T("async_load")) {
            //Whether the image supports asynchronous loading (i.e., load the image data in a worker thread to avoid the main interface stuttering)
            imageAttribute.m_bAsyncLoad = (value == _T("true"));
        }
        else if (name == _T("adaptive_dest_rect")) {
            //Automatically adapt to the destination area (scale the image proportionally)
            imageAttribute.m_bAdaptiveDestRect = (value == _T("true"));
        }
        else if (name == _T("assert")) {
            //The code assertion setting when image loading fails (enabled in debug builds, used to diagnose errors during image loading, especially failures caused by incorrect image data)
            imageAttribute.m_bAssertEnabled = (value == _T("true"));
        }
        else {
            ASSERT(!"ImageAttribute::ModifyAttribute: fount unknown attribute!");
        }
    }
}

bool ImageAttribute::IsAssertEnabled() const
{
    return m_bAssertEnabled;
}

DString ImageAttribute::GetImageName() const
{
    return m_sImageName.c_str();
}

bool ImageAttribute::HasValidImageRect(const UiRect& rcDest)
{
    if (rcDest.IsZero() || rcDest.IsEmpty()) {
        return false;
    }
    if ((rcDest.Width() > 0) && (rcDest.Height() > 0)) {
        return true;
    }
    return false;
}

UiRect ImageAttribute::GetImageSourceRect() const
{
    UiRect rc;
    if (m_rcSource != nullptr) {
        rc = *m_rcSource;
    }
    return rc;
}

UiRect ImageAttribute::GetImageDestRect(int32_t imageWidth, int32_t imageHeight, const DpiManager& dpi) const
{
    UiRect rc;
    if (m_rcDest != nullptr) {
        rc = *m_rcDest;
        if (m_bDestDpiScaleEnabled) {
            //rcDest should be scaled according to the DPI
            dpi.ScaleRect(rc);
        }
        //If the complete area is not specified, calculate the area automatically (only left and top may be set, without right and bottom)
        if (rc.right <= rc.left) {
            if (imageWidth > 0) {
                rc.right = rc.left + imageWidth;
            }
        }
        if (rc.bottom <= rc.top) {
            if (imageHeight > 0) {
                rc.bottom = rc.top + imageHeight;
            }
        }
    }
    return rc;
}

bool ImageAttribute::HasDestRect() const
{
    return m_rcDest != nullptr;
}

UiMargin ImageAttribute::GetImageMargin(const DpiManager& dpi) const
{
    UiMargin rc;
    if (m_rcMargin != nullptr) {
        rc = UiMargin(m_rcMargin->left, m_rcMargin->top, m_rcMargin->right, m_rcMargin->bottom);
        if (m_rcMarginScale != dpi.GetDisplayScaleFactor()) {
            rc = dpi.GetScaleMargin(rc, m_rcMarginScale);
        }
    }
    return rc;
}

void ImageAttribute::SetImageMargin(const UiMargin& newMargin, bool bNeedDpiScale, const DpiManager& dpi)
{
    UiMargin rcMarginDpi = newMargin;
    if (bNeedDpiScale) {
        dpi.ScaleMargin(rcMarginDpi);
    }
    if (m_rcMargin == nullptr) {
        m_rcMargin = new UiMargin16;
    }
    m_rcMargin->left = TruncateToUInt16(rcMarginDpi.left);
    m_rcMargin->top = TruncateToUInt16(rcMarginDpi.top);
    m_rcMargin->right = TruncateToUInt16(rcMarginDpi.right);
    m_rcMargin->bottom = TruncateToUInt16(rcMarginDpi.bottom);
    m_rcMarginScale = TruncateToUInt16(dpi.GetDisplayScaleFactor());
}

bool ImageAttribute::IsTiledDraw() const
{
    if (m_pTiledDrawParam != nullptr) {
        return m_pTiledDrawParam->m_bTiledX || m_pTiledDrawParam->m_bTiledY;
    }
    return false;
}

TiledDrawParam ImageAttribute::GetTiledDrawParam(const DpiManager& dpi) const
{
    TiledDrawParam tiledDrawParam;
    if (m_pTiledDrawParam != nullptr) {
        tiledDrawParam = *m_pTiledDrawParam;
        //Perform DPI scaling on some values
        dpi.ScaleInt(tiledDrawParam.m_nTiledMarginX);
        dpi.ScaleInt(tiledDrawParam.m_nTiledMarginY);
        dpi.ScalePadding(tiledDrawParam.m_rcTiledPadding);
    }
    return tiledDrawParam;
}

UiRect ImageAttribute::GetImageCorner() const
{
    UiRect rc;
    if (m_rcCorner != nullptr) {
        rc = *m_rcCorner;
    }
    return rc;
}

bool ImageAttribute::HasImageCorner() const
{
    UiRect rcCorner = GetImageCorner();
    return (rcCorner.left > 0) || (rcCorner.top > 0) || (rcCorner.right > 0) || (rcCorner.bottom > 0);
}

/** Calculate the adaptive destination area size that keeps the aspect ratio
 * @param nImageWidth The original image width
 * @param nImageHeight The original image height
 * @param targetSize The destination area size (width, height)
 * @return The new size after adaptation (width, height)
 */
static UiSize CalculateAdaptiveSize(int32_t nImageWidth, int32_t nImageHeight, const UiSize& targetSize)
{
    if ((nImageWidth <= 0) || (nImageHeight <= 0) ||
        (targetSize.cx <= 0) || (targetSize.cy <= 0)) {
        return UiSize();
    }

    float imageRatio = static_cast<float>(nImageWidth) / nImageHeight;
    float targetRatio = static_cast<float>(targetSize.cx) / targetSize.cy;

    int32_t newWidth = targetSize.cx;
    int32_t newHeight = targetSize.cy;

    if (imageRatio > targetRatio) {
        // Take the width as the reference, scale the height proportionally
        newHeight = static_cast<int32_t>(targetSize.cx / imageRatio);
    }
    else {
        // Take the height as the reference, scale the width proportionally
        newWidth = static_cast<int32_t>(targetSize.cy * imageRatio);
    }

    return UiSize(newWidth, newHeight);
}

UiRect ImageAttribute::CalculateAdaptiveRect(int32_t nImageWidth, int32_t nImageHeight,
                                             const UiRect& targetRect,
                                             const DString& hAlign,
                                             const DString& vAlign)
{
    int32_t targetWidth = targetRect.Width();
    int32_t targetHeight = targetRect.Height();

    UiSize newSize = CalculateAdaptiveSize(nImageWidth, nImageHeight, UiSize(targetWidth, targetHeight));

    // Calculate the horizontal position
    int32_t newLeft = targetRect.left;
    if (hAlign == _T("center")) {
        newLeft = targetRect.left + (targetWidth - newSize.cx) / 2;
    }
    else if (hAlign == _T("right")) {
        newLeft = targetRect.left + targetWidth - newSize.cx;
    }
    // LEFT alignment needs no adjustment

    // Calculate the vertical position
    int32_t newTop = targetRect.top;
    if (vAlign == _T("center")) {
        newTop = targetRect.top + (targetHeight - newSize.cy) / 2;
    }
    else if (vAlign == _T("bottom")) {
        newTop = targetRect.top + targetHeight - newSize.cy;
    }
    // TOP alignment needs no adjustment

    int32_t newRight = newLeft + newSize.cx;
    int32_t newBottom = newTop + newSize.cy;
    return UiRect(newLeft, newTop, newRight, newBottom);
}

}
