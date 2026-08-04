#ifndef UI_RENDER_SKIA_RENDER_H_
#define UI_RENDER_SKIA_RENDER_H_

#include "dui/Render/IRender.h"
#include "dui/Core/Callback.h"

//Forward declarations of Skia-related classes
class SkSurface;
class SkCanvas;
struct SkPoint;
class SkPaint;
enum class SkTextEncoding;

namespace ui 
{

class Render_Skia : public IRender
{
public:
    Render_Skia();
    Render_Skia(const Render_Skia& r) = delete;
    Render_Skia& operator = (const Render_Skia& r) = delete;
    virtual ~Render_Skia() override;

    /** Get the render implementation type
    */
    virtual RenderType GetRenderType() const override;

    virtual void Clear(const UiColor& uiColor) override;
    virtual void ClearRect(const UiRect& rcDirty, const UiColor& uiColor) override;

    /** Snapshot the bitmap
    *@return Returns the bitmap interface; after it is returned, the caller manages the resources (including releasing them)
    */
    virtual IBitmap* MakeImageSnapshot() override;

    virtual void ClearAlpha(const UiRect& rcDirty, uint8_t alpha = 0) override;
    virtual void RestoreAlpha(const UiRect& rcDirty, const UiPadding& rcShadowPadding, uint8_t alpha) override;
    virtual void RestoreAlpha(const UiRect& rcDirty, const UiPadding& rcShadowPadding = UiPadding()) override;

    virtual UiPoint OffsetWindowOrg(UiPoint ptOffset) override;
    virtual UiPoint SetWindowOrg(UiPoint ptOffset) override;
    virtual UiPoint GetWindowOrg() const override;

    virtual void SaveClip(int32_t& nState) override;
    virtual void RestoreClip(int32_t nState) override;
    virtual void SetClip(const UiRect& rc, bool bIntersect = true) override;
    virtual void SetRoundClip(const UiRect& rc, float rx, float ry, bool bIntersect = true) override;
    virtual void ClearClip() override;

    virtual bool BitBlt(int32_t x, int32_t y, int32_t cx, int32_t cy, IRender* pSrcRender, int32_t xSrc, int32_t ySrc, RopMode rop) override;
    virtual bool StretchBlt(int32_t xDest, int32_t yDest, int32_t widthDest, int32_t heightDest, IRender* pSrcRender, int32_t xSrc, int32_t ySrc, int32_t widthSrc, int32_t heightSrc, RopMode rop) override;
    virtual bool AlphaBlend(int32_t xDest, int32_t yDest, int32_t widthDest, int32_t heightDest, IRender* pSrcRender, int32_t xSrc, int32_t ySrc, int32_t widthSrc, int32_t heightSrc, uint8_t alpha = 255) override;

    virtual void DrawImage(const UiRect& rcPaint, IBitmap* pBitmap, 
                           const UiRect& rcDest, const UiRect& rcDestCorners,
                           const UiRect& rcSource, const UiRect& rcSourceCorners,
                           uint8_t uFade = 255,
                           const TiledDrawParam* pTiledDrawParam = nullptr,
                           bool bWindowShadowMode = false) override;
    virtual void DrawImage(const UiRect& rcPaint, IBitmap* pBitmap, 
                           const UiRect& rcDest,  const UiRect& rcSource, 
                           uint8_t uFade = 255,
                           const TiledDrawParam* pTiledDrawParam = nullptr,
                           bool bWindowShadowMode = false) override;
    virtual void DrawImageRect(const UiRect& rcPaint, IBitmap* pBitmap,
                               const UiRect& rcDest, const UiRect& rcSource,
                               uint8_t uFade = 255, IMatrix* pMatrix = nullptr) override;

    virtual void DrawLine(const UiPointF& pt1, const UiPointF& pt2, UiColor penColor, float fWidth) override;
    virtual void DrawLine(const UiPointF& pt1, const UiPointF& pt2, IPen* pen) override;

    virtual void DrawRect(const UiRectF& rc, UiColor penColor, float fWidth, bool bLineInRect) override;
    virtual void DrawRect(const UiRectF& rc, IPen* pen, bool bLineInRect) override;

    virtual void FillRect(const UiRectF& rc, UiColor dwColor, uint8_t uFade = 255) override;
    virtual void FillRect(const UiRectF& rc, UiColor dwColor, UiColor dwColor2, int8_t nColor2Direction, uint8_t uFade = 255) override;

    virtual void DrawRoundRect(const UiRectF& rc, float rx, float ry, UiColor penColor, float fWidth) override;
    virtual void DrawRoundRect(const UiRectF& rc, float rx, float ry, IPen* pen) override;

    virtual void FillRoundRect(const UiRectF& rc, float rx, float ry, UiColor dwColor, uint8_t uFade = 255) override;
    virtual void FillRoundRect(const UiRectF& rc, float rx, float ry, UiColor dwColor, UiColor dwColor2, int8_t nColor2Direction, uint8_t uFade = 255) override;

    virtual void DrawCircle(const UiPointF& centerPt, float radius, UiColor penColor, float fWidth) override;
    virtual void DrawCircle(const UiPointF& centerPt, float radius, IPen* pen) override;
    virtual void FillCircle(const UiPointF& centerPt, float radius, UiColor dwColor, uint8_t uFade = 255) override;

    virtual void DrawArc(const UiRect& rc, float startAngle, float sweepAngle, bool useCenter, 
                         const IPen* pen, 
                         UiColor* gradientColor = nullptr, const UiRect* gradientRect = nullptr) override;

    virtual void DrawPath(const IPath* path, const IPen* pen) override;
    virtual void FillPath(const IPath* path, const IBrush* brush) override;
    virtual void FillPath(const IPath* path, const UiRectF& rc, UiColor dwColor, UiColor dwColor2, int8_t nColor2Direction) override;

    virtual UiRect MeasureString(const DString& strText, const MeasureStringParam& measureParam) override;
    virtual void DrawString(const DString& strText, const DrawStringParam& drawParam) override;

    virtual void MeasureRichText(const UiRect& textRect,
                                 const UiSize& szScrollOffset,
                                 IRenderFactory* pRenderFactory, 
                                 const std::vector<RichTextData>& richTextData,
                                 std::vector<std::vector<UiRect>>* pRichTextRects) override;

    virtual void MeasureRichText2(const UiRect& textRect,
                                  const UiSize& szScrollOffset,
                                  IRenderFactory* pRenderFactory, 
                                  const std::vector<RichTextData>& richTextData,
                                  RichTextLineInfoParam* pLineInfoParam,
                                  std::vector<std::vector<UiRect>>* pRichTextRects) override;

    virtual void MeasureRichText3(const UiRect& textRect,
                                  const UiSize& szScrollOffset,
                                  IRenderFactory* pRenderFactory, 
                                  const std::vector<RichTextData>& richTextData,
                                  RichTextLineInfoParam* pLineInfoParam,
                                  std::shared_ptr<DrawRichTextCache>& spDrawRichTextCache,
                                  std::vector<std::vector<UiRect>>* pRichTextRects) override;

    virtual void DrawRichText(const UiRect& textRect,
                              const UiSize& szScrollOffset,
                              IRenderFactory* pRenderFactory, 
                              const std::vector<RichTextData>& richTextData,
                              uint8_t uFade = 255,
                              std::vector<std::vector<UiRect>>* pRichTextRects = nullptr) override;

    virtual bool CreateDrawRichTextCache(const UiRect& textRect,
                                         const UiSize& szScrollOffset,
                                         IRenderFactory* pRenderFactory,
                                         const std::vector<RichTextData>& richTextData,
                                         std::shared_ptr<DrawRichTextCache>& spDrawRichTextCache) override;

    virtual bool IsValidDrawRichTextCache(const UiRect& textRect,
                                          const std::vector<RichTextData>& richTextData,
                                          const std::shared_ptr<DrawRichTextCache>& spDrawRichTextCache) override;

    virtual bool UpdateDrawRichTextCache(std::shared_ptr<DrawRichTextCache>& spOldDrawRichTextCache,
                                         const std::shared_ptr<DrawRichTextCache>& spUpdateDrawRichTextCache,
                                         std::vector<RichTextData>& richTextDataNew,
                                         size_t nStartLine,
                                         const std::vector<size_t>& modifiedLines,
                                         size_t nModifiedRows,
                                         const std::vector<size_t>& deletedLines,
                                         size_t nDeletedRows,
                                         const std::vector<int32_t>& rowRectTopList) override;

    virtual bool IsDrawRichTextCacheEqual(const DrawRichTextCache& first, const DrawRichTextCache& second) const override;

    virtual void DrawRichTextCacheData(const std::shared_ptr<DrawRichTextCache>& spDrawRichTextCache,                                       
                                       const UiRect& rcNewTextRect,
                                       const UiSize& szNewScrollOffset,
                                       const std::vector<int32_t>& rowXOffset,
                                       uint8_t uFade,
                                       std::vector<std::vector<UiRect>>* pRichTextRects = nullptr) override;

    virtual void DrawBoxShadow(const UiRect& rc, const UiSize& roundSize, const UiPoint& cpOffset, int32_t nBlurRadius, int32_t nSpreadRadius, UiColor dwColor, uint8_t uAlpha) override;

    virtual bool ReadPixels(const UiRect& rc, void* dstPixels, size_t dstPixelsLen) override;
    virtual bool WritePixels(void* srcPixels, size_t srcPixelsLen, const UiRect& rc) override;
    virtual bool WritePixels(void* srcPixels, size_t srcPixelsLen, const UiRect& rc, const UiRect& rcPaint) override;
    virtual RenderClipType GetClipInfo(std::vector<UiRect>& clipRects) override;
    virtual bool IsClipEmpty() const override;
    virtual bool IsEmpty() const override;
    virtual void SetRenderDpi(const IRenderDpiPtr& spRenderDpi) override;

public:
    /** Get the SkSurface interface
    */
    virtual SkSurface* GetSkSurface() const = 0;

    /** Get the SkCanvas interface
    */
    virtual SkCanvas* GetSkCanvas() const = 0;

protected:
    /** The origin coordinates of the view
    */
     SkPoint& GetPointOrg() const;

    /** Get the DPI conversion interface used by the Render
    */
    IRenderDpiPtr GetRenderDpi() const;

private:
    /** Get the GDI raster operation code
    */
    void SetRopMode(SkPaint& skPaint, RopMode rop) const;

    /** Initialize the Paint object based on the Pen interface
    */
    void SetPaintByPen(SkPaint& skPaint, const IPen* pen);

    /** Get the number of UTF16 characters occupied by the current glyph (1 or 2)
    */
    size_t GetUTF16CharCount(const DStringW::value_type* srcPtr, size_t textStartIndex) const;

    /** Set the drawing attributes for the color gradient
    */
    void InitGradientColor(SkPaint& skPaint, const UiRectF& rc, UiColor dwColor, UiColor dwColor2, int8_t nColor2Direction) const;

    /** Get the text encoding
    */
    SkTextEncoding GetTextEncoding() const;

    /** Get the bitmap data
    * @return Returns the address of the bitmap data; the data length is: height*width*sizeof(uint32_t)
    */
    void* GetPixelBits() const;

    /** DPI conversion of an integer
    */
    float GetScaleFloat(float fValue) const;

private:
    /** The state saved by Canval
    */
    int32_t m_saveCount;

    /** Drawing attributes
    */
    SkPaint* m_pSkPaint;

    /** The origin coordinates of the view
    */
    SkPoint* m_pSkPointOrg;

    /** DPI conversion helper interface
    */
    IRenderDpiPtr m_spRenderDpi;
};

} // namespace ui

#endif // UI_RENDER_SKIA_RENDER_H_
