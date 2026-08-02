#ifndef UI_RENDER_SKIA_DRAW_SKIA_IMAGE_H_
#define UI_RENDER_SKIA_DRAW_SKIA_IMAGE_H_

#include "duilib/Core/UiRect.h"

#include "SkiaHeaderBegin.h"
#include "include/core/SkImage.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "SkiaHeaderEnd.h"

namespace ui 
{

/** Draw a Skia image by calling Canvas
*/
class DrawSkiaImage
{
public:

    /** Draw a Skia image by calling Canvas; if the image is scaled, it is optimized (because Skia's own image scaling is slow)
    * @param [in] pSkCanvas The canvas interface
    * @param [in] rcDest The destination rectangle area for drawing the image
    * @param [in] skPointOrg The view offset coordinates of the target area in the canvas (top-left corner)
    * @param [in] skSrcImage The image interface to be drawn
    * @param [in] rcSrc The rectangle area of the image content to be drawn in the source image
    * @param [in] skPaint The paint attributes
    */
    static void DrawImage(SkCanvas* pSkCanvas,
                          const UiRect& rcDest,
                          const SkPoint& skPointOrg,
                          const sk_sp<SkImage>& skSrcImage,
                          const UiRect& rcSrc,
                          const SkPaint& skPaint);

};

} // namespace ui

#endif // UI_RENDER_SKIA_DRAW_SKIA_IMAGE_H_
