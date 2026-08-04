#include "RenderTest1.h"

#if defined (DUI_BUILD_FOR_WIN)
    #include "dui/Utils/BitmapHelper_Windows.h"
#elif defined(DUI_BUILD_FOR_SDL) || defined(DUI_BUILD_FOR_WAYLAND)
    #include "dui/Utils/BitmapHelper_SDL.h"
#endif

namespace ui {

RenderTest1::RenderTest1(ui::Window* pWindow):
    ui::Control(pWindow)
{
}

RenderTest1::~RenderTest1()
{
}

void RenderTest1::AlphaPaint(IRender* pRender, const UiRect& rcPaint)
{
    BaseClass::AlphaPaint(pRender, rcPaint);
}

void RenderTest1::Paint(IRender* pRender, const UiRect& rcPaint)
{
    BaseClass::Paint(pRender, rcPaint);
    int32_t marginLeft = 8;
    int32_t marginTop = 4;
    Dpi().ScaleInt(marginLeft);
    Dpi().ScaleInt(marginTop);

    UiRect rect = GetRect();
    UiRect textRect = rect;
    rect.left += marginLeft;
    rect.top += marginTop;

    int32_t nSize = 110;
    int32_t nTextLineHeight = 40;
    Dpi().ScaleInt(nSize);
    Dpi().ScaleInt(nTextLineHeight);

    rect.right = rect.left + nSize;
    rect.bottom = rect.top + nSize;
    int32_t currentBottom = rect.bottom;//Record the current bottom value
    
    //Draw with the same interfaces
    if (m_pImage == nullptr) {
        //Load the image on first draw
        m_pImage = std::make_unique<Image>();
        m_pImage->SetImageString(_T("file='autumn.png' async_load='false'"), Dpi());
        LoadImageInfo(*m_pImage);
    }
    const Image& image = *m_pImage;
    
    rect.right = rect.left + image.GetImageInfo()->GetWidth();
    rect.bottom = rect.top + image.GetImageInfo()->GetHeight();

    UiRect rcSourceCorner;
    UiRect rcImageSource(0, 0, image.GetImageInfo()->GetWidth(), image.GetImageInfo()->GetHeight());

    std::shared_ptr<IBitmap> pBitmap = image.GetCurrentBitmap(false, rect, rcImageSource, rcSourceCorner, nullptr);
    pRender->DrawImage(rcPaint, pBitmap.get(), rect, UiRect(), rcImageSource, UiRect());

    //Draw the image semi-transparently
    rect.left = rect.right + marginLeft;
    rect.right = rect.left + image.GetImageInfo()->GetWidth();
    pBitmap = image.GetCurrentBitmap(false, rect, rcImageSource, rcSourceCorner, nullptr);
    pRender->DrawImage(rcPaint, pBitmap.get(), rect, UiRect(), rcImageSource, UiRect(), 100);

    //Draw scaled down
    rect.left = rect.right + marginLeft;
    rect.right = rect.left + image.GetImageInfo()->GetWidth() / 2;
    rect.bottom = rect.top + image.GetImageInfo()->GetHeight() / 2;
    pBitmap = image.GetCurrentBitmap(false, rect, rcImageSource, rcSourceCorner, nullptr);
    pRender->DrawImage(rcPaint, pBitmap.get(), rect, UiRect(), rcImageSource, UiRect());

    //The three drawing functions: BitBlt/StretchBlt/AlphaBlend
    IRender* pSrcRender = BitmapHelper::CreateRenderObject(pBitmap.get());
    ASSERT(pSrcRender != nullptr);

    rect.left = rect.right + marginLeft;
    rect.right = rect.left + image.GetImageInfo()->GetWidth();
    rect.bottom = rect.top + image.GetImageInfo()->GetHeight();
    pRender->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), pSrcRender, 0, 0, RopMode::kSrcCopy);

    rect.left = rect.right + marginLeft;
    rect.right = rect.left + image.GetImageInfo()->GetWidth() / 2;
    rect.bottom = rect.top + image.GetImageInfo()->GetHeight() / 2;
    pRender->StretchBlt(rect.left, rect.top, rect.Width(), rect.Height(), pSrcRender, 0, 0, pSrcRender->GetWidth(), pSrcRender->GetHeight(), RopMode::kSrcCopy);

    rect.left = rect.right + marginLeft;
    rect.right = rect.left + image.GetImageInfo()->GetWidth() ;
    rect.bottom = rect.top + image.GetImageInfo()->GetHeight() ;
    pRender->AlphaBlend(rect.left, rect.top, rect.Width() , rect.Height() , pSrcRender, 0, 0, pSrcRender->GetWidth(), pSrcRender->GetHeight(), 96);

    delete pSrcRender;
    pSrcRender = nullptr;

    currentBottom = rect.bottom;//Record the current bottom value

    //New line
    rect = GetRect();
    rect.left += marginLeft;
    rect.top = currentBottom + marginTop;

    //Tile drawing, using small images
    int32_t nTileSize = Dpi().GetScaleInt(32);
    std::unique_ptr<IBitmap> pTiledBitmap = ui::ImageUtil::ResizeImageBitmap(pBitmap.get(), nTileSize, nTileSize);
    if (pTiledBitmap != nullptr) {
        UiRect rect1 = rect;
        UiRect rect2 = rect;
        UiRect rect3 = rect;
        UiRect rect4 = rect;
        rect1.right = rect1.left + rect.Width() / 2;
        rect1.bottom = rect1.top + rect.Height() / 2;

        rect2.left = rect1.right + Dpi().GetScaleInt(2);
        rect2.right = rect.right - Dpi().GetScaleInt(4);
        rect2.top = rect1.top;
        rect2.bottom = rect1.bottom;

        rect3.left = rect1.left;
        rect3.right = rect1.right;
        rect3.top = rect1.bottom + Dpi().GetScaleInt(2);
        rect3.bottom = rect.bottom - Dpi().GetScaleInt(4);

        rect4.left = rect3.right + Dpi().GetScaleInt(2);
        rect4.top = rect3.top;
        rect4.right = rect.right - Dpi().GetScaleInt(4);
        rect4.bottom = rect3.bottom;

        rcImageSource.left = 0;
        rcImageSource.top = 0;
        rcImageSource.right = pTiledBitmap->GetWidth();
        rcImageSource.bottom = pTiledBitmap->GetHeight();

        UiRect rcCorners;
        TiledDrawParam tiledParam;

        //Tile drawing
        tiledParam.m_bTiledX = true;
        tiledParam.m_bTiledY = true;
        pRender->DrawImage(rcPaint, pTiledBitmap.get(), rect1, rcCorners, rcImageSource, rcCorners, 255, &tiledParam);

        tiledParam.m_nTiledMarginX = Dpi().GetScaleInt(2);
        tiledParam.m_nTiledMarginY = Dpi().GetScaleInt(2);
        pRender->DrawImage(rcPaint, pTiledBitmap.get(), rect2, rcCorners, rcImageSource, rcCorners, 255, &tiledParam);

        //Nine-patch (9-slice) drawing
        tiledParam = TiledDrawParam();
        tiledParam.m_bTiledX = true;
        tiledParam.m_bTiledY = true;
        int32_t nCornerSize = Dpi().GetScaleInt(6);
        rcCorners = UiRect(nCornerSize, nCornerSize, nCornerSize, nCornerSize);
        pRender->DrawImage(rcPaint, pTiledBitmap.get(), rect3, rcCorners, rcImageSource, rcCorners, 255, &tiledParam);

        int32_t nPaddingSize = Dpi().GetScaleInt(2);
        tiledParam.m_nTiledMarginX = Dpi().GetScaleInt(2);
        tiledParam.m_nTiledMarginY = Dpi().GetScaleInt(2);
        tiledParam.m_rcTiledPadding = UiPadding(nPaddingSize, nPaddingSize, nPaddingSize, nPaddingSize);
        pRender->DrawImage(rcPaint, pTiledBitmap.get(), rect4, rcCorners, rcImageSource, rcCorners, 255, &tiledParam);
    }
}

void RenderTest1::PaintChild(IRender* pRender, const UiRect& rcPaint)
{
    BaseClass::PaintChild(pRender, rcPaint);
}

} //end of namespace ui
