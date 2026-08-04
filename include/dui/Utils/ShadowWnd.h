#ifndef UI_UTILS_SHADOW_WND_H__
#define UI_UTILS_SHADOW_WND_H__

#include "dui/Utils/WinImplBase.h"

namespace ui {

/** Base class using an attached shadow window; implements creating and attaching the window, and can be inherited by windows without the WS_EX_LAYERED property
 * This class encapsulates 3 window modes:
 * (1) Normal mode, a window without an attached shadow: it can be configured in the XML file <Window> as follows (example):
 *             shadow_attached="false" layered_window="false" , the alpha property is not required and not supported;
 * (2) Attached shadow window mode, which requires a layered window (with the WS_EX_LAYERED property): it can be configured in the XML file <Window> as follows (example):
 *             shadow_attached="true" layered_window="true" , alpha is optional, sets the window transparency; if not set, the default is 255.
 * (3) Attached shadow window mode, which does not require a layered window (without the WS_EX_LAYERED property): it can be configured in the XML file <Window> as follows (example):
 *             shadow_attached="true" layered_window="false" , the alpha property is not required and not supported;
 *             This is equivalent to: layered_window="false" with the shadow_attached property not set.
 */
class ShadowWndBase;
class DUI_API ShadowWnd: public WindowImplBase
{
    typedef WindowImplBase BaseClass;
public:
    ShadowWnd();
    virtual ~ShadowWnd() override;

protected:
    /** Attach the window shadow
    */
    virtual Box* AttachShadow(Box* pRoot) override;

private:
    /** Determine whether a following shadow of a transparent window needs to be created
    */
    bool NeedCreateShadowWnd() const;

    /** Initialize the shadow
    */
    void InitShadow();

private:
    //The shadow window, transparent
    ShadowWndBase* m_pShadowWnd;
};
} // namespace ui

#endif // UI_UTILS_SHADOW_WND_H__
