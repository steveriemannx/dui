#ifndef UI_CORE_WINDOW_H_
#define UI_CORE_WINDOW_H_

#include "duilib/Core/WindowBase.h"
#include "duilib/Core/Shadow.h"
#include "duilib/Core/ControlFinder.h"
#include "duilib/Core/ColorManager.h"
#include "duilib/Core/ControlPtrT.h"
#include "duilib/Render/IRender.h"
#include "duilib/Utils/FilePath.h"

namespace ui
{

class Box;
class Control;
class ToolTip;
class WindowBuilder;

/** Window class
*  //The basic initialization flow required by external callers:
*  1. Call Window::CreateWnd to create the window;
*  //The following can be implemented in the PreInitWindow function:
*  2. Window::SetResourcePath;
*  3. WindowBuilder::Create, get Box* pRoot;
*  4. Window::AttachShadow(pRoot), get the shadowed Box* pRoot to support the window shadow effect;
*  5. Window::AttachBox(pRoot);
*/
class DUILIB_API Window : public WindowBase
{
public:
    Window();
    Window(const Window& r) = delete;
    Window& operator=(const Window& r) = delete;
    virtual ~Window() override;

public:
    /** Sets the window resource path (relative to the resource root directory)
    * @param [in] strPath The path to set
    */
    void SetResourcePath(const FilePath& strPath);

    /** Gets the window resource path
    */
    const FilePath& GetResourcePath() const;

    /** Sets the path of the XML file associated with the window (relative to GetResourcePath())
    * @param [in] xmlPath The path to set
    */
    void SetXmlPath(const FilePath& xmlPath);

    /** Gets the path of the XML file associated with the window
    * @return Returns the subdirectory of the XML file; the actual XML path is GetResourcePath() + GetXmlPath()
    */
    const FilePath& GetXmlPath() const;

    /** Binds the top-level container of the window
    * @param [in] pRoot The container pointer, usually the outermost container configured in the XML
    */
    bool AttachBox(Box* pRoot);

    /** Gets the top-level container of the window
    @return Returns the top-level container of the window, which may be the shadowed Box container (when AttachBox is called) or the Box container configured in the XML
    */
    Box* GetRoot() const;

    /** Gets the top-level container configured in the XML
    @return Returns the Box container configured in the XML
    */
    Box* GetXmlRoot() const;

    /** Gets the parent window
    */
    Window* GetParentWindow() const;

    /** Sends a request to exit the message loop when the window closes
    * @param [in] bPostQuitMsg If true, a request to exit the message loop is sent when the window closes
    */
    void PostQuitMsgWhenClosed(bool bPostQuitMsg);

    /** Sets the rendering engine draw backend type of this window
    *   This attribute can only be set before creating the window, or specified in the XML; it cannot be modified after application
    * @param [in] backendType The rendering engine draw backend type
    * @return Returns true on success, otherwise false
    */
    bool SetRenderBackendType(RenderBackendType backendType);

    /** Gets the rendering engine draw backend type
    */
    RenderBackendType GetRenderBackendType() const;

    /** Sets the window icon (supports *.ico; other formats are also supported, but ICO is recommended)
    *  @param [in] iconFilePath The path of the icon file (relative path within the resource root)
    */
    bool SetWindowIcon(const DString& iconFilePath);

public:
    /** @name Window shadow related interfaces
    * @{
    */
    /** Attaches the window shadow
    * @param pXmlRoot The top-level container configured in the XML file
    * @return If IsShadowAttached() is true, returns the shadow container pointer; if false, returns pXmlRoot
    */
    virtual Box* AttachShadow(Box* pXmlRoot);

    /** Sets whether the shadow effect is attached to the window
    * @param [in] bShadowAttached Attach the shadow when true, do not attach when false
    */
    void SetShadowAttached(bool bShadowAttached);

    /** Gets whether the shadow effect is attached
    */
    bool IsShadowAttached() const;

    /** Whether the current shadow effect value is the default value
    */
    bool IsUseDefaultShadowAttached() const;

    /** Sets whether the current shadow effect value is the default value
    */
    void SetUseDefaultShadowAttached(bool bDefault);

    /** Sets the shadow type
    */
    void SetShadowType(Shadow::ShadowType nShadowType);

    /** Gets the shadow type
    */
    Shadow::ShadowType GetShadowType() const;

    /** Gets the shadow image
    */
    DString GetShadowImage() const;

    /** Sets the window shadow image
    * @param [in] shadowImage The image location
    */
    void SetShadowImage(const DString& shadowImage);

    /** Sets the shadow border size (not DPI-scaled)
    */
    void SetShadowBorderSize(int32_t nShadowBorderSize);

    /** Gets the shadow border size (not DPI-scaled)
    */
    int32_t GetShadowBorderSize() const;

    /** Sets the shadow border color
    */
    void SetShadowBorderColor(const DString& shadowBorderColor);

    /** Gets the shadow border color
    */
    DString GetShadowBorderColor() const;

    /** Gets the current shadow nine-patch attribute (already DPI-scaled)
     *@return If the shadow is not attached or the window is maximized, returns UiPadding(0, 0, 0, 0); otherwise returns the set nine-patch attribute (already DPI-scaled)
     */
    UiPadding GetCurrentShadowCorner() const;

    /** Gets the set shadow nine-patch attribute
     *@return Returns the nine-patch attribute set via SetShadowCorner, not DPI-scaled
     */
    UiPadding GetShadowCorner() const;

    /** Sets the nine-patch description of the shadow image
    * @param [in] rcShadowCorner The nine-patch attribute of the shadow image, not DPI-scaled
    */
    void SetShadowCorner(const UiPadding& rcShadowCorner);

    /** Gets the shadow corner radius
    * @return Returns the shadow corner radius, not DPI-scaled
    */
    UiSize GetShadowBorderRound() const;

    /** Sets the shadow corner radius
    * @param [in] szBorderRound The corner radius of the shadow, not DPI-scaled
    */
    void SetShadowBorderRound(UiSize szBorderRound);

    /** Sets whether the shadow supports window edge-snap operations
    */
    void SetEnableShadowSnap(bool bEnable);

    /** Gets whether the shadow supports window edge-snap operations
    */
    bool IsEnableShadowSnap() const;

    /** @}*/

public:
    /** Gets the control currently holding the focus
    */
    Control* GetFocusControl() const;

    /** Gets the control of the current mouse event
    */
    Control* GetEventClick() const;

    /** Sets the focus to the specified control (makes the window the focus window and the control the focus control)
    * @param [in] pControl The control pointer
    */
    void SetFocusControl(Control* pControl);

    /** Removes the focus from the control (does not affect the window focus)
    */
    void KillFocusControl();

    /** Sets whether the window focus is also set when the control focus is set (i.e. when SetFocusControl is called)
    */
    void SetCheckSetWindowFocus(bool bCheckSetWindowFocus);

    /** When setting the control focus (i.e. calling SetFocusControl), whether the window focus is also set
    */
    bool IsCheckSetWindowFocus() const;

    /** Gets which control the mouse is currently over
    */
    Control* GetHoverControl() const;

    /** Moves the control focus to the next (or previous) control
    * @param [in] bForward true for the previous control, otherwise false; defaults to true
    */
    bool SetNextTabControl(bool bForward = true);

public:
    /** @name Window drawing related interfaces
    * @{
    */
    /** Sets whether the control has been laid out
    * @param [in] bArrange true if already arranged, otherwise false
    */
    void SetArrange(bool bArrange);

    /** Clears the image cache
    */
    void ClearImageCache();

    /** Invalidates the window region
    */
    void InvalidateAll();

    /** @} */

public:
    /**@name Control lookup related interfaces
    * @{
    */
    /** Finds the specified control at the given coordinates using the default flags: UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST
    * @param [in] pt The specified coordinates
    */
    Control* FindControl(const UiPoint& pt) const;

    /** Finds the control that supports ToolTip at the given coordinates
    */
    Control* FindToolTipControl(const UiPoint& pt) const;

    /** Finds the control that can respond to WM_CONTEXTMENU at the given coordinates
    * @param [in] pt The specified coordinates
    */
    Control* FindContextMenuControl(const UiPoint* pt) const;

    /** Finds the Box container that supports drag-and-drop
    * @param [in] pt The specified coordinates
    * @param [in] nDropInId The drop-in ID value (each control can set an ID to receive drops)
    */
    Box* FindDroppableBox(const UiPoint& pt, uint8_t nDropInId) const;

    /** Finds a control by its name
    * @param [in] strName The control name (note: case sensitive)
    */
    Control* FindControl(const DString& strName) const;

    /** Finds a child control at the given coordinates
    * @param [in] pParent The control to search
    * @param [in] pt The coordinates to search
    */
    Control* FindSubControlByPoint(Control* pParent, const UiPoint& pt) const;

    /** Finds a child control by name
    * @param [in] pParent The control to search
    * @param [in] strName The name to search for (note: case sensitive)
    */
    Control* FindSubControlByName(Control* pParent, const DString& strName) const;

    /** @} */

public:
    /** Gets the window interface at a point
    * @param [in] pt A point in screen coordinates
    * @param [in] bIgnoreChildWindow true to ignore child windows, false to not ignore them
    */
    Window* WindowFromPoint(const UiPoint& pt, bool bIgnoreChildWindow = false);

    /** Updates the ToolTip info (the ToolTip info has already changed at this point)
    */
    void UpdateToolTip();

public:
    /** Gets the nine-patch description of the alpha channel repair range, corresponding to the alphafixcorner attribute in XML
    */
    const UiRect& GetAlphaFixCorner() const;

    /** Sets the nine-patch description of the alpha channel repair range
    * @param [in] rc The repair range to set
    * @param [in] bNeedDpiScale Whether to apply DPI scaling
    */
    void SetAlphaFixCorner(const UiRect& rc, bool bNeedDpiScale);

    /** Sets the initial window size, corresponding to the size attribute in the XML file
    * @param [in] cx The width, already DPI-scaled
    * @param [in] cy The height, already DPI-scaled
    */
    void SetInitSize(int cx, int cy);

    /** Initializes the control; called when the control is added to a container (used to cache the control name)
    * @param [in] pControl The control pointer
    */
    bool InitControls(Control* pControl);

    /** Recycles the control
    * @param [in] pControl The control pointer
    */
    void ReapObjects(Control* pControl);

    /** Adds a common style
    * @param [in] strClassName The common style class name
    * @param [in] strControlAttrList The common style attribute list in XML-escaped format
    */
    void AddClass(const DString& strClassName, const DString& strControlAttrList);

    /** Gets the content of the specified common style
    * @param [in] strClassName The common style class name
    * @return Returns the common style content of the specified name, in XML-escaped format
    */
    DString GetClassAttributes(const DString& strClassName) const;

    /** Removes a common style
    * @param [in] strClassName The common style class name to remove
    */
    bool RemoveClass(const DString& strClassName);

    /** Removes all common styles
    */
    void RemoveAllClass();

    /** Adds a color value for use within the window
    * @param [in] strName The color name (e.g. white)
    * @param [in] strValue The concrete color value (e.g. #FFFFFFFF)
    */
    void AddTextColor(const DString& strName, const DString& strValue);

    /** Adds a color value for use within the window
    * @param [in] strName The color name (e.g. white)
    * @param [in] argb The concrete color value, in ARGB format
    */
    void AddTextColor(const DString& strName, UiColor argb);

    /** Gets the concrete value of a color by name
    * @param [in] strName The color name to get
    * @return Returns the color description value in DWORD format
    */
    UiColor GetTextColor(const DString& strName) const;

    /** Removes the color attribute with the specified name
    * @param [in] strName The color name to remove
    */
    void RemoveTextColor(const DString& strName);

    /** Adds an option group
    * @param [in] strGroupName The group name
    * @param [in] pControl The control pointer
    */
    bool AddOptionGroup(const DString& strGroupName, Control* pControl);

    /** Gets the control list in the specified option group
    * @param [in] strGroupName The group name
    * @return Returns the list of all controls under this group
    */
    std::vector<Control*>* GetOptionGroup(const DString& strGroupName);

    /** Removes an option group
    * @param [in] strGroupName The group name
    * @param [in] pControl The control name
    */
    void RemoveOptionGroup(const DString& strGroupName, Control* pControl);

    /** Removes all option groups
    */
    void RemoveAllOptionGroups();

public:
    /** Whether the specified key is pressed in this message
    * @param [in] msg The message currently being processed
    * @param [in] modifierKey The keyboard state to check
    */
    bool IsKeyDown(const EventArgs& msg, ModifierKey modifierKey) const;

    /** Gets the drawing engine object
    */
    virtual IRender* GetRender() const override;

    /** Gets the control interface at the specified coordinates
    * @param [in] pt A point in client coordinates
    */
    virtual Control* OnFindControl(const UiPoint& pt) const override;

    /** Gets the DPI conversion object associated with this window, used by Render
    */
    std::shared_ptr<IRenderDpi> GetRenderDpi();

    /** Sets whether the window attributes have been fully set (to avoid setting them repeatedly)
    */
    void SetWindowAttributesApplied(bool bApplied);

    /** Gets whether the window attributes have been fully set
    */
    bool IsWindowAttributesApplied() const;

    /** Initializes the skin configuration file
    * @param [in] skinFolder The window skin directory, a relative path
    * @param [in] skinFile The window skin XML description file
    */
    void InitSkin(const DString& skinFolder, const DString& skinFile);

public:
    /**   Called when creating the window; implemented by subclasses to get the window skin directory
    * @return Subclasses must implement and return the window skin directory, a relative path
    */
    virtual DString GetSkinFolder();

    /**   Called when creating the window; implemented by subclasses to get the window skin XML description file
    * @return Subclasses must implement and return the window skin XML description file
    *         The returned content can be XML file content (a string starting with the character '<'), 
    *         Or a file path (a string not starting with the character '<'); the file must be found under the GetSkinFolder() path
    */
    virtual DString GetSkinFile();

    /** Called when the control to create is not a standard control name
    * @param [in] strClass The control name
    * @return Returns a custom control pointer; normally the custom control is created from the strClass parameter
    */
    virtual Control* CreateControl(const DString& strClass);

public:
    // Window attribute setting
    /** Sets a specified attribute of the window
     * @param[in] strName The attribute name to set (e.g. width)
     * @param[in] strValue The attribute value to set (e.g. 100)
     */
    virtual void SetAttribute(const DString& strName, const DString& strValue);

    /** Sets the global class attribute of the control
     * @param[in] strClass The class name to set; it must exist in global.xml
     */
    void SetClass(const DString& strClass);

    /** Applies an attribute list
     * @param[in] strList The string representation of the attribute list, e.g. `width="800" height="600"`
     */
    void ApplyAttributeList(const DString& strList);

    /** Sets whether drag-and-drop is allowed (drop text and drop file operations)
    * @param [in] bEnable true to allow drag-and-drop, false to disallow it
    */
    void SetEnableDragDrop(bool bEnable);

    /** Unregisters a drag-and-drop interface
    */
    bool IsEnableDragDrop() const;

public:
    /** Sets the control to be displayed fullscreen; after entering fullscreen, use ExitFullscreen() to leave it
    * @param [in] pFullscreenControl The control to be displayed fullscreen
    * @param [in] exitButtonClass The Class name of the exit-fullscreen button; empty means the "Exit Fullscreen" button is not shown
    *             The default exit-fullscreen Class name is "btn_exit_fullscreen", defined in globlal.xml
    */
    bool SetFullscreenControl(Control* pFullscreenControl,
                              const DString& exitButtonClass = _T("btn_exit_fullscreen"));

    /** Gets the fullscreen control
    * @return Returns the interface of the fullscreen control, or nullptr if there is none
    */
    Control* GetFullscreenControl() const;

    /** Exits the control fullscreen state, restoring the control to its state before fullscreen
    */
    void ExitControlFullscreen();

protected:
    /** Initializing window data (internal function; subclasses overriding it must call the base class function)
    */
    virtual void PreInitWindow() override;

    /** Initializes window data (called after the window is created), for subclasses to do initialization work
    */
    virtual void OnInitWindow() override {};

    /** Completes the window data initialization
    */
    virtual void PostInitWindow() override;

    /** Completes the UI layout initialization; the position and size of each control are initialized
    */
    virtual void OnInitLayout() override {};

    /** Called when the window is about to be closed, for subclasses to do cleanup work
    */
    virtual void OnPreCloseWindow() {};

    /** Called after the window has been closed, for subclasses to do cleanup work
    */
    virtual void OnCloseWindow() {};

    /** Called when the window is destroyed; this is the last message for the window (the default implementation calls delete on itself; override to avoid self-destruction)
    */
    virtual void OnFinalMessage();

    /** The shadow type of the window changed
    */
    virtual void OnWindowShadowTypeChanged() {};

protected:
    /** Switches between the system title bar and the custom-drawn title bar
    */
    virtual void OnUseSystemCaptionBarChanged() override;

    /** Prepares for drawing
    * @return Returns true to continue drawing, false to stop drawing
    */
    virtual bool OnPreparePaint() override;

    /** The layered window attribute of the window changed
    */
    virtual void OnLayeredWindowChanged() override;

    /** The opacity of the window changed
    */
    virtual void OnWindowAlphaChanged() override;

    /** Enters the fullscreen state
    */
    virtual void OnWindowEnterFullscreen() override;

    /** Exits the fullscreen state
    */
    virtual void OnWindowExitFullscreen() override;

    /** The window DPI scale changed; updates control sizes and layout (for subclass use)
    * @param [in] nOldScaleFactor The old DPI scale percentage
    * @param [in] nNewScaleFactor The new DPI scale percentage, consistent with Dpi().GetDisplayScaleFactor()
    */
    virtual void OnWindowDisplayScaleChanged(uint32_t nOldScaleFactor, uint32_t nNewScaleFactor) override;

    /** Gets the set window shadow size
    * @param [out] rcShadow Returns the window shadow size, not DPI-scaled
    */
    virtual void GetShadowCorner(UiPadding& rcShadow) const override;

    /** Gets the current shadow nine-patch attribute (already DPI-scaled)
     *@param [out] rcShadow If the shadow is not attached or the window is maximized, returns UiPadding(0, 0, 0, 0); otherwise returns the set nine-patch attribute (already DPI-scaled)
     */
    virtual void GetCurrentShadowCorner(UiPadding& rcShadow) const override;

    /** Determines whether a point is on a control placed on the title bar
    */
    virtual bool IsPtInCaptionBarControl(const UiPoint& pt) const override;

    /** Determines whether the window has maximize and minimize buttons
    * @param [out] bMinimizeBox Returns true if the window has a minimize button
    * @param [out] bMaximizeBox Returns true if the window has a maximize button
    */
    virtual bool HasMinMaxBox(bool& bMinimizeBox, bool& bMaximizeBox) const override;

    /** Determines whether a point is on the maximize or restore button
    */
    virtual bool IsPtInMaximizeRestoreButton(const UiPoint& pt) const override;

    /** Gets the window creation attributes (read from the Window tag of the XML file)
    * @param [out] createAttributes Returns the window creation attributes read from the Window tag of the XML file
    */
    virtual void GetCreateWindowAttributes(WindowCreateAttributes& createAttributes) override;

    /** @name Window message handling related
        * @{
    */
    /** The window message dispatcher
    * @param [in] uMsg The message
    * @param [in] wParam The message extra parameter
    * @param [in] lParam The message extra parameter
    * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return Returns the result of processing the message
    */
    virtual LRESULT OnWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) override;

    /** The window creation success event (WM_CREATE/WM_INITDIALOG)
    * @param [in] bDoModal Whether the window is currently a modal dialog shown via DoModal
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return Returns the result of processing the message; if the application handled the message it should return zero
    */
    virtual void OnWindowCreateMsg(bool bDoModal, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Window close message (WM_CLOSE)
     * @param [in] wParam The wParam of the message
     * @param [in] nativeMsg The raw message content received from the system
     * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
     * @return Returns the result of processing the message; if the application handled the message it should return zero
     */
    virtual LRESULT OnWindowCloseMsg(uint32_t wParam, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** The window is shown or hidden (WM_SHOWWINDOW)
    * @param [in] bShow true means the window is being shown, false means it is being hidden
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return Returns the result of processing the message; if the application handled the message it should return zero
    */
    virtual LRESULT OnShowWindowMsg(bool bShow, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Window painting (SDL_EVENT_WINDOW_EXPOSED/WM_PAINT)
    * @param [in] rcPaint The rectangle to repaint in this draw pass
    * @param [in] nativeMsg The raw message content received from the system
    *             SDL implementation: nativeMsg.uMsg is SDL_EVENT_WINDOW_EXPOSED, nativeMsg.wParam is an SDL_Window* pointer
    *             Windows implementation: nativeMsg.uMsg is WM_PAINT, nativeMsg.wParam is the HWND of the window
    * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return Returns the result of processing the message; if the application handled the message it should return zero
    */
    virtual LRESULT OnPaintMsg(const UiRect& rcPaint, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** The window position/size changed (WM_WINDOWPOSCHANGED)
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return Returns the result of processing the message; if the application handled the message it should return zero
    */
    virtual LRESULT OnWindowPosChangedMsg(const NativeMsg& nativeMsg, bool& bHandled) override;

    /** The window size changed (WM_SIZE)
    * @param [in] sizeType The type that triggered the window size change
    * @param [in] newWindowSize The new window size (width and height)
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return Returns the result of processing the message; if the application handled the message it should return zero
    */
    virtual LRESULT OnSizeMsg(WindowSizeType sizeType, const UiSize& newWindowSize, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Window moved (WM_MOVE)
    * @param [in] ptTopLeft The x and y coordinates of the window client area top-left corner (screen coordinates)
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return Returns the result of processing the message; if the application handled the message it should return zero
    */
    virtual LRESULT OnMoveMsg(const UiPoint& ptTopLeft, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** The window gained focus (WM_SETFOCUS)
    * @param [in] pLostFocusWindow The window that lost the keyboard focus (can be nullptr)
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return Returns the result of processing the message; if the application handled the message it should return zero
    */
    virtual LRESULT OnSetFocusMsg(WindowBase* pLostFocusWindow, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** The window lost focus (WM_KILLFOCUS)
    * @param [in] pSetFocusWindow The window that receives the keyboard focus (can be nullptr)
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return Returns the result of processing the message; if the application handled the message it should return zero
    */
    virtual LRESULT OnKillFocusMsg(WindowBase* pSetFocusWindow, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Notifies the application of input focus changes (WM_IME_SETCONTEXT)
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return Returns the result of processing the message; if the application handled the message it should return zero
    */
    virtual LRESULT OnImeSetContextMsg(const NativeMsg& nativeMsg, bool& bHandled) override;

    /** The input method starts composing a string (WM_IME_STARTCOMPOSITION)
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return Returns the result of processing the message; if the application handled the message it should return zero
    */
    virtual LRESULT OnImeStartCompositionMsg(const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Changes the key composition state (WM_IME_COMPOSITION)
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return Returns the result of processing the message; if the application handled the message it should return zero
    */
    virtual LRESULT OnImeCompositionMsg(const NativeMsg& nativeMsg, bool& bHandled) override;

    /** The input method ends composition (WM_IME_ENDCOMPOSITION)
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return Returns the result of processing the message; if the application handled the message it should return zero
    */
    virtual LRESULT OnImeEndCompositionMsg(const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Sets the cursor (WM_SETCURSOR)
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return If the application handled this message, it should return TRUE to stop further processing or FALSE to continue
    */
    virtual LRESULT OnSetCursorMsg(const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Notifies the window that the user wants to display a context menu (WM_CONTEXTMENU). The user may have clicked the right mouse button (right-clicked in the window), pressed Shift+F10, or pressed the application key (context menu key) on some keyboards.
    * @param [in] pt The mouse position in client coordinates; (-1,-1) means the user pressed SHIFT+F10
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return Returns the result of processing the message; if the application handled the message it should return zero
    */
    virtual LRESULT OnContextMenuMsg(const UiPoint& pt, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Key down (WM_KEYDOWN or WM_SYSKEYDOWN)
    * @param [in] vkCode The virtual key code
    * @param [in] modifierKey Key modifier flags. Valid values: ModifierKey::kFirstPress, ModifierKey::kAlt
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return Returns the result of processing the message; if the application handled the message it should return zero
    */
    virtual LRESULT OnKeyDownMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Key up (WM_KEYUP or WM_SYSKEYUP)
    * @param [in] vkCode The virtual key code
    * @param [in] modifierKey Key modifier flags. Valid values: ModifierKey::kAlt
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return Returns the result of processing the message; if the application handled the message it should return zero
    */
    virtual LRESULT OnKeyUpMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Character key (WM_CHAR)
    * @param [in] vkCode The virtual key code
    * @param [in] modifierKey Key modifier flags. Valid values: ModifierKey::kFirstPress, ModifierKey::kAlt
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return Returns the result of processing the message; if the application handled the message it should return zero
    */
    virtual LRESULT OnCharMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Hotkey message (WM_HOTKEY)
    * @param [in] hotkeyId The hotkey ID
    * @param [in] vkCode The virtual key code
    * @param [in] modifierKey Key modifier flags. Valid values: ModifierKey::kAlt, ModifierKey::kControl, ModifierKey::kShift, ModifierKey::kWin
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return Returns the result of processing the message; if the application handled the message it should return zero
    */
    virtual LRESULT OnHotKeyMsg(int32_t hotkeyId, VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Mouse wheel rotation (WM_MOUSEWHEEL)
    * @param [in] wheelDelta The wheel rotation distance, expressed as a multiple or fraction of WHEEL_DELTA (120). Positive values mean the wheel rotated forward (away from the user); negative values mean backward (toward the user)
    * @param [in] pt The mouse position, in client coordinates
    * @param [in] modifierKey Key modifier flags. Valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return Returns the result of processing the message; if the application handled the message it should return zero
    */
    virtual LRESULT OnMouseWheelMsg(int32_t wheelDelta, const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Mouse move message (WM_MOUSEMOVE)
    * @param [in] pt The mouse position, in client coordinates
    * @param [in] modifierKey Key modifier flags. Valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] bFromNC true means this is an NC message (WM_NCMOUSEMOVE); false means it is a WM_MOUSEMOVE message
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return Returns the result of processing the message; if the application handled the message it should return zero
    */
    virtual LRESULT OnMouseMoveMsg(const UiPoint& pt, uint32_t modifierKey, bool bFromNC, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Mouse hover message (WM_MOUSEHOVER)
    * @param [in] pt The mouse position, in client coordinates
    * @param [in] modifierKey Key modifier flags. Valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return Returns the result of processing the message; if the application handled the message it should return zero
    */
    virtual LRESULT OnMouseHoverMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Mouse leave message (WM_MOUSELEAVE)
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return Returns the result of processing the message; if the application handled the message it should return zero
    */
    virtual LRESULT OnMouseLeaveMsg(const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Mouse left button down (WM_LBUTTONDOWN)
    * @param [in] pt The mouse position, in client coordinates
    * @param [in] modifierKey Key modifier flags. Valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return Returns the result of processing the message; if the application handled the message it should return zero
    */
    virtual LRESULT OnMouseLButtonDownMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Mouse left button up (WM_LBUTTONUP)
    * @param [in] pt The mouse position, in client coordinates
    * @param [in] modifierKey Key modifier flags. Valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return Returns the result of processing the message; if the application handled the message it should return zero
    */
    virtual LRESULT OnMouseLButtonUpMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Mouse left button double-click (WM_LBUTTONDBLCLK)
    * @param [in] pt The mouse position, in client coordinates
    * @param [in] modifierKey Key modifier flags. Valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return Returns the result of processing the message; if the application handled the message it should return zero
    */
    virtual LRESULT OnMouseLButtonDbClickMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Mouse right button down (WM_RBUTTONDOWN)
    * @param [in] pt The mouse position, in client coordinates
    * @param [in] modifierKey Key modifier flags. Valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return Returns the result of processing the message; if the application handled the message it should return zero
    */
    virtual LRESULT OnMouseRButtonDownMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Mouse right button up (WM_RBUTTONUP)
    * @param [in] pt The mouse position, in client coordinates
    * @param [in] modifierKey Key modifier flags. Valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return Returns the result of processing the message; if the application handled the message it should return zero
    */
    virtual LRESULT OnMouseRButtonUpMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Mouse right button double-click (WM_RBUTTONDBLCLK)
    * @param [in] pt The mouse position, in client coordinates
    * @param [in] modifierKey Key modifier flags. Valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return Returns the result of processing the message; if the application handled the message it should return zero
    */
    virtual LRESULT OnMouseRButtonDbClickMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Mouse middle button down (WM_MBUTTONDOWN)
    * @param [in] pt The mouse position, in client coordinates
    * @param [in] modifierKey Key modifier flags. Valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return Returns the result of processing the message; if the application handled the message it should return zero
    */
    virtual LRESULT OnMouseMButtonDownMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Mouse middle button up (WM_MBUTTONUP)
    * @param [in] pt The mouse position, in client coordinates
    * @param [in] modifierKey Key modifier flags. Valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return Returns the result of processing the message; if the application handled the message it should return zero
    */
    virtual LRESULT OnMouseMButtonUpMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Mouse middle button double-click (WM_MBUTTONDBLCLK)
    * @param [in] pt The mouse position, in client coordinates
    * @param [in] modifierKey Key modifier flags. Valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return Returns the result of processing the message; if the application handled the message it should return zero
    */
    virtual LRESULT OnMouseMButtonDbClickMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** The window lost mouse capture (WM_CAPTURECHANGED)
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return Returns the result of processing the message; if the application handled the message it should return zero
    */
    virtual LRESULT OnCaptureChangedMsg(const NativeMsg& nativeMsg, bool& bHandled) override;

    /** The window edge-snap operation
    * @param [in] bLeftSnap Snap the window to the left edge
    * @param [in] bRightSnap Snap the window to the right edge
    * @param [in] bTopSnap Snap the window to the top edge
    * @param [in] bBottomSnap Snap the window to the bottom edge
    */
    virtual void OnWindowPosSnapped(bool bLeftSnap, bool bRightSnap, bool bTopSnap, bool bBottomSnap) override;

    /** Window drag-and-drop operation interface (the interface parameters depend on the implementation)
    * @param [in] dropType The source type of the drag-and-drop operation
    * @param [in,out] pDropData The concrete type depends on dropType:
    *                 When dropType is kControlDropTypeWindows (the Windows platform SDK implementation), pDropData is of type ControlDropData_Windows*
    *                 When dropType is kControlDropTypeSDL (the SDL implementation), pDropData is of type ControlDropData_SDL*
    *                 pDropData->m_bHandled is the message handling flag; returning true means the event has been handled and is not forwarded to other UI controls, effectively intercepting the message
    *                 pDropData->m_hResult is the return value after handling the message, returned to the OS; on Windows, success returns S_OK
    */
    virtual void OnDropEnterMsg(ui::ControlDropType dropType, void* pDropData) override;
    virtual void OnDropOverMsg(ui::ControlDropType dropType, void* pDropData) override;
    virtual void OnDropMsg(ui::ControlDropType dropType, void* pDropData) override;
    virtual void OnDropLeaveMsg() override;

    /** Handles the system notification message for screen resolution changes (WM_DISPLAYCHANGE)
    * @param [in] nColorDepth The new display color depth, in bits per pixel
    * @param [in] nScreenWidth The horizontal resolution of the screen
    * @param [in] nScreenHeight The vertical resolution of the screen
    */
    virtual void OnDisplayResolutionChangedMsg(int32_t nColorDepth, int32_t nScreenWidth, int32_t nScreenHeight) override;

    /** Handles the system notification message for DPI changes (WM_DPICHANGED)
    * @param [in] fNewDisplayScale The new window UI display scale; 1.0f means no scaling
    * @param [in] fNewPixelDensity The new pixel density of the window (only used in the SDL implementation)
    */
    virtual void OnDisplayScaleChangedMsg(float fNewDisplayScale, float fNewPixelDensity) override;

    /** @}*/

private:
    /** The window is closing; handles internal state (internal function)
    */
    virtual void PreCloseWindow() override final;

    /** The window has closed; handles internal state (internal function)
    */
    virtual void PostCloseWindow() override final;

    /** Called when the window is destroyed; this is the last message for the window
    */
    virtual void FinalMessage() override final;

    /** The window DPI scale changed; updates control sizes and layout (this function cannot be overridden; to handle this event, override OnWindowDisplayScaleChanged)
    * @param [in] nOldScaleFactor The old DPI scale percentage
    * @param [in] nNewScaleFactor The new DPI scale percentage, consistent with Dpi().GetDisplayScaleFactor()
    */
    virtual void OnDisplayScaleChanged(uint32_t nOldScaleFactor, uint32_t nNewScaleFactor) override final;

    /** Enters the fullscreen state
    */
    virtual void NotifyWindowEnterFullscreen() override final;

    /** Exits the fullscreen state
    */
    virtual void NotifyWindowExitFullscreen() override final;

private:
    //Mouse button down message handlers
    void OnButtonDown(EventType eventType, const UiPoint& pt, const NativeMsg& nativeMsg, uint32_t modifierKey);

    //Mouse button up message handlers
    void OnButtonUp(EventType eventType, const UiPoint& pt, const NativeMsg& nativeMsg, uint32_t modifierKey);

    //Clears the mouse and keyboard operation states
    void ClearStatus();

    /** Determines whether the mouse enter/leave message should be sent
    * @param [in] pt The current mouse position
    * @param [in] modifierKey The key modifier flags
    * @param [in] bHideToolTip Whether the ToolTip needs to be hidden
    * @return Returns true if the mouse enter/leave message should be sent, false if not
    */
    bool HandleMouseEnterLeave(const UiPoint& pt, uint32_t modifierKey, bool bHideToolTip);

private:
    /**@name Animation effect related interfaces
    * @{
    */
    /** Sets the drawing offset
    * @param [in] renderOffset The offset value
    */
    void SetRenderOffset(UiPoint renderOffset);

    /** Sets the drawing X offset
    * @param [in] renderOffsetX The X offset
    */
    void SetRenderOffsetX(int renderOffsetX);

    /** Sets the drawing Y offset
    * @param [in] renderOffsetY The Y offset
    */
    void SetRenderOffsetY(int renderOffsetY);

    /** @} */

private:
    /** Automatically adjusts the window size based on the root
    * @return Returns true if the window size was adjusted, otherwise false
    */
    bool AutoResizeWindow(bool bRepaint);

    /** Adjusts the control layout
    */
    void ArrangeRoot();

    /** Cleans up the window resources
    */
    void ClearWindow();

    /** Initializes the layout
    */
    void OnFirstLayout();

    /** Shows or hides the window
    */
    void OnShowWindow(bool bShow);

    /** Updates the window state and creates the Render, etc.
    */
    bool PreparePaint(bool bArrange);

    /** The drawing function body
    * @param [in] rcPaint The rectangle to repaint in this draw pass
    * @return Returns true if drawing was performed, otherwise false
    */
    bool Paint(const UiRect& rcPaint);

    /** Adjusts the Render size to match the current client area
    */
    bool ResizeRenderToClientSize() const;

    /** Parses the XML file associated with the window
    */
    void ParseWindowXml();

    /** The focus control changed
    */
    void OnFocusControlChanged();

    /** Gets the shadow operation interface
    */
    Shadow* GetShadow() const;

private:
    /** Handles the window maximize event
    */
    void ProcessWindowMaximized();

    /** Handles the window restore event
    */
    void ProcessWindowRestored();

    /** Handles the window enter fullscreen event
    */
    void ProcessWindowEnterFullscreen();

    /** Handles the window exit fullscreen event
    */
    void ProcessWindowExitFullscreen();

    /** Handles the dynamic display of the fullscreen button
    */
    void ProcessFullscreenButtonMouseMove(const UiPoint& pt);

    /** Saves and sets the container outer margin in fullscreen state
    */
    void SetWindowMaximizedMargin();

    /** Restores the container outer margin in fullscreen state
    */
    void RestoreWindowMaximizedMargin();

private:
    //The focus control
    ControlPtr m_pFocus;

    //The hover control under the mouse
    ControlPtr m_pEventHover;

    //The ToolTip control under the mouse
    ControlPtr m_pEventToolTip;

    /** The clicked control: 
        Assigned in WM_LBUTTONDOWN/WM_RBUTTONDOWN/WM_LBUTTONDBLCLK
        Cleared in WM_LBUTTONUP/WM_KILLFOCUS
    */
    ControlPtr m_pEventClick;

    /** The control where a key was pressed
    *   Assigned in WM_KEYDOWN / WM_SYSKEYDOWN
    *   Cleared in WM_KEYUP / WM_SYSKEYUP
    */
    ControlPtr m_pEventKey;

    /** The control lookup helper class
    */
    ControlFinder m_controlFinder;

    /** The container associated with the window, the root node
    */
    BoxPtr m_pRoot;

    /** Window shadow
    */
    std::unique_ptr<Shadow> m_shadow;

    /** Whether the control is currently in fullscreen state
    */
    bool m_bControlFullscreen;

private:
    //The nine-patch description of the alpha channel repair range
    UiRect m_rcAlphaFix;

    //Whether the layout changed; if it changed (true), the layout must be recalculated
    bool m_bIsArranged;

    //Whether the layout has been initialized
    bool m_bFirstLayout;

    //Whether the layout initialization callback has been called
    bool m_bInitLayout;

    //Whether the window focus is also set when setting the control focus
    bool m_bCheckSetWindowFocus;

    //The drawing offset (for animation)
    UiPoint m_renderOffset;

    //The outer margin in the maximized state (on Windows, the maximized window area overflows the screen, so an outer margin is needed to keep the window content from overflowing)
    UiMargin m_rcWindowMaximizedMargin;

    //The drawing engine
    std::unique_ptr<IRender> m_render;

private:
    /** The resource path of each window (relative to the resource root directory)
    */
    FilePath m_resourcePath;

    /** The path of the XML file associated with the window (relative to m_resourcePath); the actual XML directory is m_resourcePath + m_xmlPath
    */
    FilePath m_xmlPath;

    /** The skin path
    */
    DString m_skinFolder;

    /** The skin configuration file
    */
    DString m_skinFile;

    /** XML parsing and control creation
    */
    std::unique_ptr<WindowBuilder> m_windowBuilder;

private:
    /** The mapping between class names and attributes in the window configuration
    */
    std::map<DString, DString> m_defaultAttrHash;

    /** The mapping between window color strings and color values (ARGB)
    */
    ColorMap m_colorMap;

    /** The controls under each Option group of this window (i.e. radio controls are grouped)
    */
    std::map<DString, std::vector<Control*>> m_mOptionGroup;

    /** Tooltip
    */
    std::unique_ptr<ToolTip> m_toolTip;

    /** Sends a request to exit the message loop when the window closes
    */
    bool m_bPostQuitMsgWhenClosed;

    /** The rendering engine backend drawing method (CPU, OpenGL, etc.)
    */
    RenderBackendType m_renderBackendType;

    /** The initial window size
    */
    UiSize m_szInitSize;

    /** Whether the window attributes have been fully set
    */
    bool m_bWindowAttributesApplied;
};

} // namespace ui

#endif // UI_CORE_WINDOW_H_
