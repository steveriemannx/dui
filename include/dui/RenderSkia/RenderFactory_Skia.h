#ifndef UI_RENDER_SKIA_RENDER_FACTORY_H_
#define UI_RENDER_SKIA_RENDER_FACTORY_H_

#include "dui/Render/IRender.h"

class SkFontMgr;

namespace ui 
{

class RenderFactory_Skia : public IRenderFactory
{
public:
    RenderFactory_Skia();
    RenderFactory_Skia(const RenderFactory_Skia& r) = delete;
    RenderFactory_Skia& operator = (const RenderFactory_Skia& r) = delete;
    virtual ~RenderFactory_Skia() override;

    virtual IFont* CreateIFont() override;
    virtual IPen* CreatePen(UiColor color, float fWidth = 1.0f) override;
    virtual IBrush* CreateBrush(UiColor corlor) override;
    virtual IPath* CreatePath() override;
    virtual IMatrix* CreateMatrix() override;
    virtual IBitmap* CreateBitmap() override;

    /** Create a Render object
    * @param [in] spRenderDpi The associated DPI conversion interface
    * @param [in] platformData Platform-specific data; on Windows this value is the window handle
    * @parma [in] backendType The backend drawing type
    */
    virtual IRender* CreateRender(const IRenderDpiPtr& spRenderDpi,
                                  void* platformData = nullptr,
                                  RenderBackendType backendType = RenderBackendType::kRaster_BackendType) override;

    /** Get the font manager interface (each factory shares one object)
    */
    virtual IFontMgr* GetFontMgr() const override;

private:
    /** Internal implementation class
    */
    class TImpl;
    TImpl* m_impl;
};

} // namespace ui

#endif // UI_RENDER_SKIA_RENDER_FACTORY_H_
