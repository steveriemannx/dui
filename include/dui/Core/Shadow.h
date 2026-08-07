#ifndef UI_CORE_SHADOW_H_
#define UI_CORE_SHADOW_H_

#include "dui/Core/UiSize.h"
#include "dui/Core/UiRect.h"
#include <string>

namespace ui 
{

class Box;
class Control;
class Window;
class DpiManager;

/** Window shadow properties
*/
class DUI_API Shadow
{
public:
    /** Shadow type
    */
    enum ShadowType
    {
        kShadowFirst        = 0,            //The starting value of valid values

        kShadowBig          = 0,            //Large shadow, square corners (suitable for normal windows)
        kShadowBigRound     = 1,            //Large shadow, rounded corners (suitable for normal windows)
        kShadowSmall        = 2,            //Small shadow, square corners (suitable for normal windows)
        kShadowSmallRound   = 3,            //Small shadow, rounded corners (suitable for normal windows)
        kShadowMenu         = 4,            //Small shadow, square corners (suitable for popup windows, such as menus)
        kShadowMenuRound    = 5,            //Small shadow, rounded corners (suitable for popup windows, such as menus)
        kShadowNone         = 6,            //No shadow, with border, square corners
        kShadowNoneRound    = 7,            //No shadow, with border, rounded corners
        kShadowCustom       = 8,                //User-defined shadow (setting it clears the default shadow properties; subsequently call SetShadowImage, SetShadowCorner, SetShadowBorderRound to set the shadow properties)

        //System shadows (provided by the OS, e.g. macOS NSWindow / Windows DWM).
        //When one of these is selected the window must be non-layered
        //(see IsShadowTypeNeedLayeredWindow).
        kShadowSystemDefault    = 9,             //OS default shadow
        kShadowSystemDoNotRound = 10,            //OS shadow, square corners
        kShadowSystemRound      = 11,            //OS shadow, rounded corners
        kShadowSystemSmallRound = 12,            //OS shadow, small rounded corners
        kShadowCount,                           //The maximum value of valid values

        kShadowDefault      = kShadowBigRound   //Default shadow (used by default when not set)
    };

    /** Get the corresponding shadow type from a string
    */
    static bool GetShadowType(const DString& typeString, ShadowType& nShadowType);

    /** True when the type is one of the OS-provided shadow types
     * (kShadowSystemDefault / DoNotRound / Round / SmallRound).
    */
    static bool IsSystemShadowType(ShadowType nShadowType);

    /** Whether the type requires a layered window (self-drawn shadows do;
     *  system shadows and kShadowNone do not).
    */
    static bool IsShadowTypeNeedLayeredWindow(ShadowType nShadowType);

    /** Resolve kShadowDefault to a concrete type based on the window and the
     *  platform (layered window -> self-drawn; macOS -> system shadow).
    */
    static ShadowType GetDefaultShadowType(const Window* pWindow);

    /** Make the type usable on this platform: system types fall back to
     *  self-drawn shadows when the OS shadow is unsupported.
    */
    static ShadowType GetSupportedShadowType(const Window* pWindow, ShadowType nShadowType);

    /** Get the parameters corresponding to the default shadow type
    * @param [in] nShadowType The shadow type
    * @param [out] szBorderRound Returns the corner radius, not DPI-scaled
    * @param [out] rcShadowCorner Returns the nine-grid properties of the shadow asset, not DPI-scaled
    * @param [out] shadowImage Returns the shadow image properties, including the nine-grid properties of the shadow image
    * @param [in] pShadowObj The associated shadow object
    */
    static bool GetShadowParam(ShadowType nShadowType,
                               UiSize& szBorderRound,
                               UiPadding& rcShadowCorner,
                               DString& shadowImage,
                               Shadow* pShadowObj = nullptr);

public:
    /** Constructor
     @param [in] pWindow The associated window
    */
    explicit Shadow(Window* pWindow);

    /** Set whether the shadow effect is supported
     * @param[in] bShadowAttached Set true to support the shadow effect, false to not support the shadow effect
     */
    void SetShadowAttached(bool bShadowAttached);

    /** Determine whether the shadow effect is already supported
     */
    bool IsShadowAttached() const;

    /** Whether the current shadow effect value is the default value
    */
    bool IsUseDefaultShadowAttached() const;

    /** Set whether the current shadow effect value is the default value
    */
    void SetUseDefaultShadowAttached(bool bDefault);

    /** Set the shadow type
    */
    void SetShadowType(Shadow::ShadowType nShadowType);

    /** True when the current shadow type is an OS-provided shadow and it is
     *  attached (enabled).
    */
    bool IsSystemShadowEnabled() const;
    bool IsSystemShadowEnabled(ShadowType nShadowType) const;

    /** Get the shadow type
    */
    Shadow::ShadowType GetShadowType() const;

    /** Set the nine-grid description of the shadow asset
    * @param [in] rcShadowCorner The nine-grid properties of the shadow image, values not DPI-scaled
    */
    void SetShadowCorner(const UiPadding& rcShadowCorner);

    /** Get the set shadow nine-grid properties
     *@return Returns the nine-grid properties set via the SetShadowCorner function, values not DPI-scaled
     */
    UiPadding GetShadowCorner() const;

    /** Get the current shadow nine-grid properties (already DPI-scaled)
     *@return If the shadow is not attached or the window is maximized, returns UiPadding(0, 0, 0, 0); otherwise returns the set nine-grid properties (already DPI-scaled)
     */
    UiPadding GetCurrentShadowCorner() const;

    /** Set the corner radius of the shadow
    * @param [in] szBorderRound The corner radius of the shadow, values not DPI-scaled
    */
    void SetShadowBorderRound(UiSize szBorderRound);

    /** Get the corner radius of the shadow
    * @return Returns the corner radius of the shadow, values not DPI-scaled
    */
    UiSize GetShadowBorderRound() const;

    /** Set the shadow image properties
     */
    void SetShadowImage(const DString& shadowImage);

    /** Get the shadow image properties
     */
    const DString& GetShadowImage() const;

    /** Set the border size of the shadow (not DPI-scaled)
    */
    void SetShadowBorderSize(int32_t nShadowBorderSize);

    /** Get the border size of the shadow (not DPI-scaled)
    */
    int32_t GetShadowBorderSize() const;

    /** Set the border color of the shadow
    */
    void SetShadowBorderColor(const DString& shadowBorderColor);

    /** Get the border color of the shadow
    */
    const DString& GetShadowBorderColor() const;

public:
    /** Attach the shadow to the top-level container of the window
     * @param[in] pXmlRoot The top-level container of the window, the top-level container in the XML configuration
     */
    Box* AttachShadow(Box* pXmlRoot);

    /** Get the container pointer after attaching the shadow
    */
    Box* GetShadowBox() const;

    /** Determine whether there is an attached shadow Box
    */
    bool HasShadowBox() const;

    /** Get the top-level container in the XML configuration (i.e., the Box passed into AttachShadow)
    */
    Box* GetAttachedXmlRoot() const;

    /** Set whether the window is maximized or restored
     * @param[in] isMaximized Set to true for maximized, false to restore the initial state
     */
    void MaximizedOrRestored(bool isMaximized);

    /** Clear the image cache
     */
    void ClearImageCache();

    /** DPI changed; update the control size and layout
    * @param [in] dpi The DPI manager
    * @param [in] nOldDpiScale The old DPI scale percentage
    * @param [in] nNewDpiScale The new DPI scale percentage, consistent with the value of Dpi().GetScale()
    */
    void ChangeDpiScale(const DpiManager& dpi, uint32_t nOldDpiScale, uint32_t nNewDpiScale);

    /** Set the window snap properties
    */
    void SetWindowPosSnap(bool bLeftSnap, bool bRightSnap, bool bTopSnap, bool bBottomSnap);

    /** Set whether the shadow supports window snap operations
    */
    void SetEnableShadowSnap(bool bEnable);

    /** Get whether the shadow supports window snap operations
    */
    bool IsEnableShadowSnap() const;

    /** Check whether the click is on the shadow; if so, handle related logic (such as mouse passthrough)
    * @param [in] eventType The event type
    * @param [in] pt The mouse position
    */
    void CheckMouseClickOnShadow(EventType eventType, const UiPoint& pt);

    /** Set whether the shadow's click-through feature is supported
    */
    void SetEnableClickThroughWindow(bool bEnable);

    /** Get whether the shadow's click-through feature is supported
    */
    bool IsEnableClickThroughWindow() const;

private:
    /** Attach the shadow to the window
     */
    void DoAttachShadow(Box* pNewRoot, Box* pOrgRoot, bool bNewAttach, bool isMaximized) const;

    /** The shadow attach event
    */
    void OnShadowAttached(Shadow::ShadowType nShadowType);

    /** Update the shadow properties
    */
    void UpdateShadow();

    /** Update the window snap properties
    */
    void UpdateWindowPosSnap();

private:
    //Whether the shadow effect is supported
    bool m_bShadowAttached;

    //Whether the current shadow effect value is the default value
    bool m_bUseDefaultShadowAttached;

    //Whether the window is currently maximized
    bool m_isMaximized;

    /** Whether the shadow's click-through feature is supported
    */
    bool m_bEnableClickThroughWindow;

    /** Whether the shadow supports window snap operations
    */
    bool m_bEnableShadowSnap;

    /** Window snap properties (left, top, right, bottom)
    */
    bool m_bLeftSnap;
    bool m_bTopSnap;
    bool m_bRightSnap;
    bool m_bBottomSnap;

    //The shadow image properties
    DString m_shadowImage;

    //The shadow nine-grid properties (not DPI-scaled)
    UiPadding m_rcShadowCorner;

    //The corner radius of the shadow (not DPI-scaled)
    UiSize m_szBorderRound;

    /** The border size of the shadow (not DPI-scaled)
    */
    int32_t m_nShadowBorderSize;

    /** The border color of the shadow
    */
    DString m_shadowBorderColor;

    //The shadow container interface
    Box* m_pShadowBox;

    //The associated window
    Window* m_pWindow;

    //The shadow type
    Shadow::ShadowType m_nShadowType;
};

}

#endif // UI_CORE_SHADOW_H_
