#ifndef UI_CORE_WINDOW_CREATE_ATTRIBUTES_H_
#define UI_CORE_WINDOW_CREATE_ATTRIBUTES_H_

#include "dui/Core/UiTypes.h"

namespace ui {

/** Window attributes in XML
*/
class WindowCreateAttributes
{
public:
    /** Whether to use the system title bar
    */
    bool m_bUseSystemCaption = false;
    bool m_bUseSystemCaptionDefined = false;//Whether a valid value is included

    /** The stretchable range information of the four sides of the window
    */
    UiRect m_rcSizeBox;
    bool m_bSizeBoxDefined = false;//Whether a valid value is included

    /** Title bar area information
    */
    UiRect m_rcCaption;
    bool m_bCaptionDefined = false;//Whether a valid value is included

    /** Whether shadow effects are supported
    */
    bool m_bShadowAttached = true;
    bool m_bShadowAttachedDefined = false;//Whether a valid value is included

    /** Shadow nine-grid attributes (not DPI-scaled)
    */
    UiPadding m_rcShadowCorner;

    /** Whether it is a layered window
    */
    bool m_bIsLayeredWindow = true;
    bool m_bIsLayeredWindowDefined = false;//Whether a valid value is included

    /** Window transparency; on the Windows platform, this value is used as a parameter in the UpdateLayeredWindow function (BLENDFUNCTION.SourceConstantAlpha)
    */
    uint8_t m_nLayeredWindowAlpha = 255;
    bool m_bLayeredWindowAlphaDefined = false;//Whether a valid value is included

    /** Window opacity; on the Windows platform, this value is used as a parameter in the SetLayeredWindowAttributes function (bAlpha)
    */
    uint8_t m_nLayeredWindowOpacity = 255;
    bool m_bLayeredWindowOpacityDefined = false;//Whether a valid value is included

    /** The initial size of the window (already DPI-scaled)
    */
    UiSize m_szInitSize;
    bool m_bInitSizeDefined = false;//Whether a valid value is included

    /** The desired SDL Render name (optional parameter)
    */
    DString m_sdlRenderName;
};

} // namespace ui

#endif // UI_CORE_WINDOW_CREATE_ATTRIBUTES_H_

