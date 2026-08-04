#ifndef UI_CORE_CONTROL_H_
#define UI_CORE_CONTROL_H_

#include "dui/Core/PlaceHolder.h"
#include "dui/Core/BoxShadow.h"
#include "dui/Core/Keyboard.h"
#include "dui/Core/EventArgs.h"
#include "dui/Animation/EasingFunctions.h"

namespace ui 
{
    class Control;
    class ControlLoading;
    class Image;
    class IMatrix;
    class StateColorMap;
    class StateColorMap2;
    class StateImageMap;
    class AnimationManager;
    class AnimationPlayer;
    class IRender;
    class IPath;
    class IFont;
    class AutoClip;
    class ControlDropTarget_Windows;
    class ControlDropTarget_SDL;
    class ControlDropTargetImpl_Windows;
    class ControlDropTargetImpl_SDL;

    typedef Control* (* FINDCONTROLPROC)(Control*, void*);

/** Control base class (equivalent to Widget)
*/
class DUI_API Control: public PlaceHolder
{
    typedef PlaceHolder BaseClass;
public:
    explicit Control(Window* pWindow);
    Control(const Control& r) = delete;
    Control& operator=(const Control& r) = delete;
    virtual ~Control() override;

    /** Get the control type
    */
    virtual DString GetType() const override;

    /// Graphics related
    /** Get the background color
     * @return Returns the background color string, defined in global.xml
     */
    DString GetBkColor() const;

    /** Set the background color
     * @param [in] strColor The background color value to set, must exist in global.xml
     */
    void SetBkColor(const DString& strColor);

    /** Set the background color
     * @param [in] color The background color value to set
     */
    void SetBkColor(const UiColor& color);

    /** Set the second background color (to implement the gradient background color)
     * @param [in] strColor The background color value to set, must exist in global.xml
     */
    void SetBkColor2(const DString& strColor);

    /** Set the second background color (to implement the gradient background color)
     * @param [in] color The background color value to set
     */
    void SetBkColor2(const UiColor& color);

    /** Get the second background color (to implement the gradient background color)
     */
    DString GetBkColor2() const;

    /** Set the direction of the second background color
     * @param [in] direction The direction of the second background color: "1": left->right, "2": top->bottom, "3": top-left->bottom-right, "4": top-right->bottom-left
     */
    void SetBkColor2Direction(const DString& direction);

    /** Get the direction of the second background color
    * @return Returns the direction of the second background color: "1": left->right, "2": top->bottom, "3": top-left->bottom-right, "4": top-right->bottom-left
     */
    DString GetBkColor2Direction() const;

    /** Get the foreground color (the foreground color is generally set to a semi-transparent color)
     * @return Returns the foreground color string, defined in global.xml
     */
    DString GetForeColor() const;

    /** Set the foreground color (the foreground color is generally set to a semi-transparent color)
     * @param [in] strColor The foreground color value to set, must exist in global.xml
     */
    void SetForeColor(const DString& strColor);

    /** Set the foreground color (the foreground color is generally set to a semi-transparent color)
     * @param [in] color The foreground color value to set
     */
    void SetForeColor(const UiColor& color);

    /** Get the control color in a specified state
     * @param [in] stateType The state to get the color for, refer to the ControlStateType enum
     * @return The color string set for the specified state, corresponding to the color value in global.xml
     */
    DString GetStateColor(ControlStateType stateType) const;

    /** Get the color rectangle outer margin of the control in a specified state
     * @param [in] stateType The state to get the color for, refer to the ControlStateType enum
     * @return The color rectangle outer margin set for the specified state (already DPI scaled)
     */
    UiMargin GetStateColorMargin(ControlStateType stateType) const;

    /** Get the color rectangle corner radius of the control in a specified state
     * @param [in] stateType The state to get the color for, refer to the ControlStateType enum
     * @return The color rectangle corner radius set for the specified state (already DPI scaled)
     */
    UiSize GetStateColorRound(ControlStateType stateType) const;

    /** Set the control color, color margin, and color rectangle corner radius in a specified state
     * @param [in] stateType The state to set the color for, refer to the ControlStateType enum
     * @param [in] strColor The color value to set, must exist in global.xml
     * @param [in] colorMargin The color rectangle outer margin to set; if not set, the color rectangle coincides with the control rectangle
     * @param [in] colorRound The color rectangle corner radius to set; if not set, the color rectangle follows the shape of the control rectangle
     * @param [in] bNeedDpiScale Whether DPI adaptation is needed
     */
    void SetStateColor(ControlStateType stateType, const DString& strColor);
    void SetStateColorMargin(ControlStateType stateType, UiMargin colorMargin, bool bNeedDpiScale);
    void SetStateColorRound(ControlStateType stateType, UiSize colorRound, bool bNeedDpiScale);

    /**
     * @brief Get the background image path
     * @return The background image path
     */
    DString GetBkImage() const;

    /**
     * @brief Get the background image path in UTF8 format
     * @return The background image path
     */
    std::string GetUTF8BkImage() const;

    /**
     * @brief Set the background image
     * @param[in] strImage The image path to set
     * @return None
     */
    void SetBkImage(const DString& strImage);

    /**
     * @brief Set the background image (UTF8 format string)
     * @param[in] strImage The image path to set
     * @return None
     */
    void SetUTF8BkImage(const std::string& strImage);

public:
    /**
     * @brief Get the image path in a specified state
     * @param[in] The state to get the image for, refer to the ControlStateType enum
     * @return The image path in the specified state
     */
    DString GetStateImage(ControlStateType stateType) const;

    /**
     * @brief Set the image in a specified state
     * @param[in] stateType The state to set the image for, refer to the ControlStateType enum
     * @param[in] strImage The image path to set
     * @return None
     */
    void SetStateImage(ControlStateType stateType, const DString& strImage);

    /**
     * @brief Get the foreground image in a specified state
     * @param[in] The state to get the image for, refer to the `ControlStateType` enum
     * @return The foreground image path in the specified state
     */
    DString GetForeStateImage(ControlStateType stateType) const;

    /**
     * @brief Set the foreground image in a specified state
     * @param[in] stateType The state to set the image for, refer to the `ControlStateType` enum
     * @param[in] strImage The foreground image path to set
     * @return None
     */
    void SetForeStateImage(ControlStateType stateType, const DString& strImage);

    /**@brief Get the control state
     * @return The state of the control, please refer to the `ControlStateType` enum
     */
    ControlStateType GetState() const;

    /**@brief Set the control state
     * @param[in] controlState The control state to set, please refer to the `ControlStateType` enum
     */
    void SetState(ControlStateType controlState);

    /** Whether currently in the Hot state (this state has more business logic)
    */
    bool IsHotState() const;

    /** Get the image interface used to estimate the control size (width and height)
     */
    virtual Image* GetEstimateImage();

    /// Border related
    /** Get the border color in a specified state
     * @param [in] stateType The control state
     * @return The border color string, corresponding to the concrete color value in global.xml
     */
    virtual DString GetBorderColor(ControlStateType stateType) const;

    /** Set the border color, applied to all states
     * @param [in] strBorderColor The border color string value to set, must exist in global.xml
     */
    void SetBorderColor(const DString& strBorderColor);

    /** Set the border color in a specified state
     * @param [in] stateType The control state
     * @param [in] strBorderColor The border color string value to set, must exist in global.xml
     */
    void SetBorderColor(ControlStateType stateType, const DString& strBorderColor);

    /** Set the border color in the focus state
    */
    void SetFocusBorderColor(const DString& strBorderColor);

    /** Get the border color in the focus state
    */
    DString GetFocusBorderColor() const;

    /** Set the border size (left, top, right, bottom correspond to the left, top, right, and bottom border sizes respectively)
     * @param [in] rc A set of border sizes in a `UiRectF` structure
     * @param [in] bNeedDpiScale Whether DPI adaptation is needed
     */
    void SetBorderSize(UiRectF rc, bool bNeedDpiScale);

    /** Get the border size
    */
    UiRectF GetBorderSize() const;

    /** Get the left border size
     * @return The size of the left border
     */
    float GetLeftBorderSize() const;

    /** Set the left border size
     * @param [in] fSize The left border size to set
     * @param [in] bNeedDpiScale Whether DPI adaptation is needed
     */
    void SetLeftBorderSize(float fSize, bool bNeedDpiScale);

    /** Get the top border size
     */
    float GetTopBorderSize() const;

    /** Set the top border size
     * @param [in] fSize The top border size to set
     * @param [in] bNeedDpiScale Whether DPI adaptation is needed
     */
    void SetTopBorderSize(float fSize, bool bNeedDpiScale);

    /** Get the right border size
     * @return The size of the right border
     */
    float GetRightBorderSize() const;

    /** Set the right border size
     * @param [in] fSize The right border size to set
     * @param [in] bNeedDpiScale Whether DPI adaptation is needed
     */
    void SetRightBorderSize(float fSize, bool bNeedDpiScale);

    /** Get the bottom border size
     * @return The size of the bottom border
     */
    float GetBottomBorderSize() const;

    /** Set the bottom border size
     * @param [in] fSize The bottom border size to set
     * @param [in] bNeedDpiScale Whether DPI adaptation is needed
     */
    void SetBottomBorderSize(float fSize, bool bNeedDpiScale);

    /** Set the line style of the four borders of the control (the line styles of the four borders must be the same, setting them separately is not supported)
    * @param [in] borderDashStyle The line style, see the IPen::DashStyle enum definition for the value range
    */
    void SetBorderDashStyle(int8_t borderDashStyle);

    /** The line style of the four borders of the control, see the IPen::DashStyle enum definition for the value range
    */
    int8_t GetBorderDashStyle() const;

    /** Set whether the border is on top
    * @param [in] bBordersOnTop true means the border is on top, i.e. child controls are drawn first and the border afterwards, so the border is not covered by child controls
    */
    void SetBordersOnTop(bool bBordersOnTop);

    /** Get whether the border is on top, the default value is true
    */
    bool IsBordersOnTop() const;

    /** Get the corner radius
     * @param [out] fRoundWidth The corner width, the result after DPI scaling
     * @param [out] fRoundHeight The corner height, the result after DPI scaling
     * @return Returns false if the corner radius is not set, returns true if the corner radius is set
     */
    bool GetBorderRound(float& fRoundWidth, float& fRoundHeight) const;

    /** Whether the corner radius is set
     */
    bool HasBorderRound() const;

    /** Set the border corner radius (the original value, not DPI scaled)
     * @param [in] borderRound A UiSize structure representing the corner radius
     */
    void SetBorderRound(UiSize borderRound);

    /** Set the border shadow
     * @param[in] The shadow attribute to set
     */
    void SetBoxShadow(const DString& strShadow);

    /// Mouse related
    /**
     * @brief Get the mouse cursor type
     */
    virtual CursorType GetCursorType() const;

    /**@brief Set the current mouse cursor type
     * @param[in] cursorType The mouse type to set, refer to the CursorType enum
     */
    void SetCursorType(CursorType cursorType);

    /**
     * @brief Get the tooltip text of the control in the mouse hover state
     * @return Returns the tooltip text in the current mouse hover state
     */
    virtual DString GetToolTipText() const;

    /**
     * @brief Get the tooltip text of the control in the mouse hover state (UTF8 format)
     * @return Returns the tooltip text in the current mouse hover state
     */
    virtual std::string GetUTF8ToolTipText() const;

    /**
     * @brief Set the tooltip text shown when the mouse hovers over the control
     * @param[in] strText The text to set
     * @return None
     */
    virtual void SetToolTipText(const DString& strText);

    /**
     * @brief Set the tooltip text shown when the mouse hovers over the control (UTF8 format)
     * @param[in] strText The text to set
     * @return None
     */
    virtual void SetUTF8ToolTipText(const std::string& strText);

    /**
     * @brief Set the text in the language file corresponding to the tooltip text shown when the mouse hovers over the control
     * @param[in] strTextId The tooltip text ID in the language file
     * @return None
     */
    virtual void SetToolTipTextId(const DString& strTextId);

    /**
     * @brief Set the text in the language file corresponding to the tooltip text shown when the mouse hovers over the control (UTF8 format)
     * @param[in] strTextId The tooltip text ID in the language file
     * @return None
     */
    virtual void SetUTF8ToolTipTextId(const std::string& strTextId);

    /** Set the maximum width of a single line of the tooltip text shown when the mouse hovers over the control
     * @param [in] nWidth The width value to set
     * @param [in] bNeedDpiScale Whether DPI scaling is supported
     */
    virtual void SetToolTipWidth(int32_t nWidth, bool bNeedDpiScale);

    /**
     * @brief Get the maximum width of a single line of the tooltip text shown when the mouse hovers over the control
     * @return The current maximum width of the tooltip text
     */
    virtual int32_t GetToolTipWidth(void) const;

    /// Menu
    /**
     * @brief Whether the control responds to context menu messages
     * @return The result indicates whether the context menu messages are responded to; true responds to context menu messages, false does not
     */
    virtual bool IsContextMenuUsed() const { return m_bContextMenuUsed; };

    /**
     * @brief Set whether the control responds to context menu messages
     * @param[in] bMenuUsed Whether to respond to context menu messages, true for yes, false for no
     * @return None
     */
    virtual void SetContextMenuUsed(bool bMenuUsed);

    /// User data, helper functions, for user use

    /** Get the data string bound to the control by the user
     * @return Returns the concrete data string
     */
    DString GetDataID() const;

    /** Get the data string bound to the control by the user (UTF8 format)
     * @return Returns the concrete data string
     */
    std::string GetUTF8DataID() const;

    /** Bind a string data to the control
     * @param[in] strText The string data to bind
     */
    void SetDataID(const DString& strText);

    /** Bind a string data to the control (UTF8 format)
     * @param[in] strText The string data to bind
     */
    void SetUTF8DataID(const std::string& strText);

    /** Bind an integer value data to the control
     * @param[in] dataID The integer data to bind
     */
    void SetUserDataID(size_t dataID);

    /** Get the integer value data bound to the control
    */
    size_t GetUserDataID() const;

    /// Some important attributes
    /** Set whether the control is visible with animations such as fade in/out; the result is the same as SetVisible, except the process includes animation effects.
        After calling SetFadeVisible, there is no need to call SetVisible to modify the visibility.
        This function internally calls SetVisible.
     * @param[in] bVisible When true the control is visible, when false the control is hidden
     */
    virtual void SetFadeVisible(bool bVisible);

    /** Check whether the control has focus
     * @return Returns whether the control has focus; true means it currently has focus, false means the control has no focus
     */
    virtual bool IsFocused() const;

    /** Let the control get focus
     */
    virtual void SetFocus();

    /** Return the flags of the control, used to determine whether it can respond to TAB switching events
     * @return Returns the flag type of the control
     */
    virtual uint32_t GetControlFlags() const;

    /** Make the control never able to get focus
     */
    void SetNoFocus(); // The control never gets focus, different from KillFocus

    /** Whether the control does not get focus
    */
    bool IsNoFocus() const { return m_bNoFocus; }

    /** Set whether to show the focus state (a rectangle made of dashed lines)
    */
    void SetShowFocusRect(bool bShowFocusRect);

    /** Set whether to show the focus state (a rectangle made of dashed lines)
    */
    bool IsShowFocusRect() const;

    /** Set the color of the focus state dashed rectangle (the color of the lines)
    */
    void SetFocusRectColor(const DString& focusRectColor);

    /** Get the color of the focus state dashed rectangle (the color of the lines)
    */
    DString GetFocusRectColor() const;

    /** Determine whether the current mouse focus is on the control
     * @return Returns whether the mouse focus is on the control; true means the mouse focus is on the control, false means it is not
     */
    virtual bool IsMouseFocused() const { return m_bMouseFocused;}

    /**
     * @brief Set whether to put the mouse focus on the control
     * @param[in] bMouseFocused When true, put the mouse focus on the control; when false, let the control lose mouse focus
     * @return None
     */
    virtual void SetMouseFocused(bool bMouseFocused) { m_bMouseFocused = bMouseFocused; }

    /**
     * @brief Determine whether the control is currently activatable
     * @return Returns the control state; true means the control is currently activatable, visible and usable; false means it is not activatable, possibly invisible or disabled
     */
    virtual bool IsActivatable() const;

    /** Activate the control, such as clicking, selecting, expanding, etc.
     * @param [in] pMsg The message corresponding to the activation, may be nullptr
     */
    virtual void Activate(const EventArgs* pMsg);

    /** Find the specified control by coordinates
     * @param [in] Proc The matching function for the search
     * @param [in] pProcData The custom data of the search function
     * @param [in] ptMouse The mouse coordinates, valid only when UIFIND_HITTEST is included
     * @param [in] uFlags The search flags, e.g. UIFIND_ENABLED, etc.
     * @param [in] scrollPos The scroll position of the scroll bar
     * @return Returns the pointer of the control
     */
    virtual Control* FindControl(FINDCONTROLPROC Proc, void* pProcData, uint32_t uFlags,
                                 const UiPoint& ptMouse = UiPoint(),
                                 const UiPoint& scrollPos = UiPoint());

    /** Find the control by name; search scope: the current control/container; if it is itself a container, search within the container and its child controls
    * @param [in] name The name of the control (case-sensitive)
    */
    Control* FindControl(const DString& name);

    /// Position related
    /** Get the control position (subclasses can change the behavior)
    * @return Returns the rectangle area of the control, including the padding but not the margin
     */
    virtual UiRect GetPos() const override;

    /** Set the control position (subclasses can change the behavior)
     * @param [in] rc The rectangle area info to set, including the padding but not the margin
     */
    virtual void SetPos(UiRect rc) override;

    /** Get the drawing expanded area of this control including box-shadow
    * @return Returns the total area of rc + the box-shadow expansion; if there is no box-shadow, returns rc
    */
    virtual UiRect GetBoxShadowExpandedRect(const UiRect& rc) const override;

    /** Calculate the control size (width and height)
        If an image is set and either width or height is set to auto, the final size is calculated based on the image size and text size
     *  @param [in] szAvailable The available size, not including the padding or the margin
     *  @return The estimated size of the control, including the padding (Box), not including the margin
     */
    virtual UiEstSize EstimateSize(UiSize szAvailable);

    /** Calculate the text area size (width and height)
     *  @param [in] szAvailable The available size, not including the padding or the margin
     *  @return The estimated text size of the control, including the padding (Box), not including the margin
     */
    virtual UiSize EstimateText(UiSize szAvailable);

    /** Calculate the image area size (width and height)
     *  @param [in] szAvailable The available size, not including the padding or the margin
     *  @param [in] estImageType The type of image estimation
     *  @return The estimated image size of the control, including the padding (Box), not including the margin
     */
    virtual UiSize EstimateImage(UiSize szAvailable, EstimateImageType estImageType);

    /**
     * @brief Check whether the specified coordinate is within the range of the current scroll position of the scroll bar
     * @param[in] point The concrete coordinate
     * @return Returns whether it is in the range; true means it is within the current scroll position range of the scroll bar, false means it is not
     */
    virtual bool IsPointInWithScrollOffset(const UiPoint& point) const;

    /** The message processing entry of the control, converting messages into custom format messages
     * @param [in] eventType The message type
     * @param [in] wParam Parameter 1 when the event is generated
     * @param [in] lParam Parameter 2 when the event is generated
     */
    void SendEvent(EventType eventType, WPARAM wParam = 0, LPARAM lParam = 0);

    /** The message processing entry of the control, converting messages into custom format messages
     * @param [in] eventType The message type
     * @param [in] wParam Parameter 1 when the event is generated
     * @param [in] lParam Parameter 2 when the event is generated
     * @param [in] pEventData The pointer parameter when the event is generated
     */
    void SendEvent(EventType eventType, WPARAM wParam, LPARAM lParam, void* pEventData);

    /** The message processing entry of the control, converting messages into custom format messages
     * @param [in] eventType The message type
     * @param [in] msg The message content, eventType does not need to be set
     */
    void SendEvent(EventType eventType, EventArgs msg);

    /** Dispatch the message to the message processing function
     * @param [in] msg The message content
     */
    virtual void SendEventMsg(const EventArgs& msg);

    /**
     * @brief Determine whether the control has a HOT state
     * @return Returns true if there is a HOT state, otherwise false
     */
    virtual bool HasHotState();

    // Attribute setting
    /**
     * @brief Set the specified attribute of the control
     * @param[in] strName The attribute name to set (e.g. width)
     * @param[in] strValue The attribute value to set (e.g. 100)
     * @return None
     */
    virtual void SetAttribute(const DString& strName, const DString& strValue);

    /**
     * @brief Set the class global attribute of the control
     * @param[in] strClass The class name to set, must exist in global.xml
     * @return None
     */
    void SetClass(const DString& strClass);

    /**
     * @brief Apply an attribute list
     * @param[in] strList The string representation of the attribute list, e.g. `width="100" height="30"`
     * @return None
     */
    void ApplyAttributeList(const DString& strList);

    /**
     * @brief To be completed
     * @param[in] To be completed
     * @return To be completed
     */
    bool OnApplyAttributeList(const DString& strReceiver, const DString& strList, const EventArgs& eventArgs);

    /// Draw operations
    /** Draw an image
     * @param [in] pRender The draw context
     * @param [in] pImage The interface of the image object
     * @param [in] strModify The additional attributes of the image
     * @param [in] nFade The transparency of the control; if the animation effect is enabled, this value changes continuously during drawing
     * @param [in] pMatrix The transformation matrix used when drawing the image
     * @param [in] pDestRect The target area for drawing the image passed in externally; if nullptr, the area returned by GetRect() is used internally
     * @param [out] pPaintedRect Returns the final target rectangle area of the image drawing
     * @return Returns true on success, false on failure
     */
    bool PaintImage(IRender* pRender,
                    Image* pImage,
                    const DString& strModify = _T(""),
                    int32_t nFade = DUI_NOSET_VALUE,
                    IMatrix* pMatrix = nullptr,
                    const UiRect* pDestRect = nullptr,
                    UiRect* pPaintedRect = nullptr) const;

    /** Draw the control itself and its child controls
     * @param[in] pRender The render interface
     * @param[in] rcPaint The specified draw area
     */
    virtual void AlphaPaint(IRender* pRender, const UiRect& rcPaint);

    /** Draw the control itself
    * @param[in] pRender The render interface
    * @param[in] rcPaint The specified draw area
    */
    virtual void Paint(IRender* pRender, const UiRect& rcPaint);

    /** Draw the child controls of the control
    * @param[in] pRender The render interface
    * @param[in] rcPaint The specified draw area
    */
    virtual void PaintChild(IRender* pRender, const UiRect& rcPaint) { (void)pRender; (void)rcPaint; };

    /** Set the transparency of the control
     * @param [in] nAlpha The transparency value from 0 to 255, 255 is opaque
     */
    void SetAlpha(uint8_t nAlpha);

    /** Get the transparency of the control
     * @return Returns the transparency of the control
     */
    uint8_t GetAlpha() const { return m_nAlpha; }

    /** Check whether the control has a transparency attribute
     * @return Returns whether the control is transparent; true means the control currently has a transparency attribute, false means it does not
     */
    bool IsAlpha() const { return m_nAlpha != 255; }

    /** Set the transparency in the Hot state
     * @param [in] alpha The transparency value from 0 to 255, 255 is opaque
     */
    void SetHotAlpha(uint8_t nHotAlpha);

    /** Get the transparency in the Hot state
     * @return Returns the transparency of the control in the Hot state
     */
    uint8_t GetHotAlpha() const { return m_nHotAlpha; }

    /**
     * @brief Set whether to accept TAB key focus switching
     * @param[in] enable
     * @return None
     */
    void SetTabStop(bool enable);

    /**
     * @brief Check whether TAB key focus switching is accepted
     * @return Returns whether the control accepts TAB key focus switching
     */
    bool IsAllowTabStop() const { return m_bAllowTabstop; }

    /** Get the draw offset of the control (the value has been DPI scaled)
     * @return Returns the current draw offset of the control
     */
    UiPoint GetRenderOffset() const;

    /** Set the draw offset of the control
     * @param [in] renderOffset The control offset data
     * @param [in] bNeedDpiScale Whether DPI scaling is supported
     */
    void SetRenderOffset(UiPoint renderOffset, bool bNeedDpiScale);

    /** Set the X coordinate of the control offset
     * @param [in] renderOffsetX The X coordinate value (the value has been DPI scaled)
     */
    void SetRenderOffsetX(int32_t renderOffsetX);

    /** Set the Y coordinate of the control offset
     * @param [in] renderOffsetY The Y coordinate value (the value has been DPI scaled)
     */
    void SetRenderOffsetY(int32_t renderOffsetY);

public:
    /// Loading status management
    /** Set the loading attributes, create the display UI according to the XML file specified in the attributes, and set the UI attributes
    * @param [in] loadingAttribute The attribute string of loading
    */
    bool SetLoadingAttribute(const DString& loadingAttribute);

    /** Start the loading status
    * @param [in] nIntervalMs The callback interval (milliseconds), minimum 10 milliseconds
    * @param [in] nMaxCount Set the maximum number of callbacks, loading stops automatically when exceeded, the total loading time is nMaxCount * nIntervalMs milliseconds; -1 means loading continuously until manually stopped
    */
    bool StartLoading(int32_t nIntervalMs, int32_t nMaxCount);

    /** Close the loading status
    */
    void StopLoading();

    /** Whether it is currently loading
    */
    bool IsLoading() const;

    /** Get the root container interface of the loading UI (available only after StartLoading succeeds, and unavailable after StopLoading ends)
    */
    Box* GetLoadingUiRootBox() const;

    /** Listen for the notification of loading start
     * @param [in] callback The callback function listening to the event
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachLoadingStart(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventLoadingStart, callback, callbackID); }

    /** Listen for the notification of loading callbacks
     * @param[in] callback The callback function listening to the event
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachLoading(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventLoading, callback, callbackID); }

    /** Listen for the notification of loading end
     * @param [in] callback The callback function listening to the event
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachLoadingStop(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventLoadingStop, callback, callbackID); }

public:
    /// Animated images
    /** Play the animation
     * @param [in] imageName The image resource name, i.e. the name field configured for the image resource in XML; an empty string means the background image
     * @param [in] nStartFrame Which frame to start playing from; the first frame, current frame, and last frame can be set. Refer to the AnimationImagePos enum
     * @param [in] nPlayCount The number of times to play
                   -1: play indefinitely
                    0: no valid play count, use the default value of the image (or a preset value)
                   >0: a concrete play count, stop playing after reaching the count
     */
    bool StartImageAnimation(const DString& imageName = _T(""),
                             AnimationImagePos nStartFrame = AnimationImagePos::kFrameCurrent,
                             int32_t nPlayCount = 0);

    /** Stop playing the animation
     * @param [in] imageName The image resource name, i.e. the name field configured for the image resource in XML; an empty string means the background image
     * @param [in] nStopFrame Which frame to stop at when the playback ends; the first frame, current frame, and last frame can be set. Refer to the AnimationImagePos enum
     * @param [in] bTriggerEvent Whether to notify subscribers of the stop event, refer to the AttachImageAnimationStop method
     */
    bool StopImageAnimation(const DString& imageName = _T(""),
                            AnimationImagePos nStopFrame = AnimationImagePos::kFrameCurrent,
                            bool bTriggerEvent = true);

    /** The current frame of the animation
     * @param [in] imageName The image resource name, i.e. the name field configured for the image resource in XML; an empty string means the background image
     * @param [in] nFrameIndex The image frame index, starting from 0
     */
    bool SetImageAnimationFrame(int32_t nFrameIndex);//The background image
    bool SetImageAnimationFrame(const DString& imageName, int32_t nFrameIndex);

    /** Get the index of the current frame of the animation
    * @param [in] imageName The image resource name, i.e. the name field configured for the image resource in XML; an empty string means the background image
    */
    uint32_t GetImageAnimationFrameIndex() const;//The background image
    uint32_t GetImageAnimationFrameIndex(const DString& imageName) const;

    /** Get the total number of frames of the animation
    * @param [in] imageName The image resource name, i.e. the name field configured for the image resource in XML; an empty string means the background image
    */
    uint32_t GetImageAnimationFrameCount();//The background image
    uint32_t GetImageAnimationFrameCount(const DString& imageName);

    /** Whether the animation image has been loaded
    */
    bool IsImageAnimationLoaded() const;//The background image
    bool IsImageAnimationLoaded(const DString& imageName) const;

    /** Listen for the notification of animation playback start (all image animations)
     * @param [in] callback The callback function listening to the animation stop playback
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachImageAnimationStart(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventImageAnimationStart, callback, callbackID); }

    /** Listen for the notification of animation image frame playback (all image animations)
     * @param [in] callback The callback function listening to the animation stop playback
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachImageAnimationPlayFrame(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventImageAnimationPlayFrame, callback, callbackID); }

    /** Listen for the notification of animation playback stop (all image animations)
     * @param [in] callback The callback function listening to the animation stop playback
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachImageAnimationStop(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventImageAnimationStop, callback, callbackID); }

public:
    /// Control animation related interfaces

    /** Set or clear the Hot state playback animation (corresponding animation type: kAnimationHot)
    * @param [in] bFadeHot true means set the animation, false means clear the animation
    */
    void SetFadeHot(bool bFadeHot);

    /** Set the timer interval of the Hot state animation playback (milliseconds) (corresponding animation type: kAnimationHot)
    * @param [in] frameIntervalMillSeconds The timer interval of the animation playback (milliseconds)
    */
    void SetFadeHotFrameIntervalMillSeconds(int32_t frameIntervalMillSeconds);

    /** Get the timer interval of the Hot state animation playback (milliseconds) (corresponding animation type: kAnimationHot)
    */
    int32_t GetFadeHotFrameIntervalMillSeconds() const;

    /** Set the total playback time of the Hot state animation (milliseconds) (corresponding animation type: kAnimationHot)
    * @param [in] totalMillSeconds The total playback time of the animation (milliseconds)
    */
    void SetFadeHotTotalMillSeconds(int32_t totalMillSeconds);

    /** Get the total playback time of the Hot state animation (milliseconds) (corresponding animation type: kAnimationHot)
    */
    int32_t GetFadeHotTotalMillSeconds() const;

    /** Set the easing function type of the Hot state animation (corresponding animation type: kAnimationHot)
    */
    void SetFadeHotEasingFunctionType(EasingFunctionType easingFunctionType);

    /** Get the easing function type of the Hot state animation (corresponding animation type: kAnimationHot)
    */
    EasingFunctionType GetFadeHotEasingFunctionType() const;

    /** Determine whether the animation player of the specified type is present
    */
    bool HasAnimationPlayer(AnimationType animationType) const;

    /** Determine whether the control animation of the specified type is playing
    */
    bool IsAnimationPlayerPlaying(AnimationType animationType) const;

    /** Get the control animation manager interface (control animations)
     */
    AnimationManager& GetAnimationManager();

public:
    /// Image resources
    /** Load the image info into the cache according to the image attribute setting
     * @param [in,out] duiImage Marks the path info of the image on input; on success, the image is cached and recorded into the members of this parameter
     * @param [out] bPaintImage true means the image is loaded during drawing, false means the image is loaded in other cases
     */
    bool LoadImageInfo(Image& duiImage, bool bPaintImage = false) const;

    /** Get the image resource interface with the specified name
    */
    Image* FindImageByName(const DString& imageName) const;

    /** Clear the image cache; after clearing, the images will be reloaded when used
     */
    virtual void ClearImageCache();

public:
    /** Calculate the relative position of the mouse on a control (relative to the top-left corner of the control)
     * @param [in] ptMouse The current mouse position, client area coordinates
     * @param [out] ptLayoutPos Returns the relative coordinates of the mouse in the client area, relative to the top-left corner of the current control
     * @return true means the mouse is within the control range, false means it is not
     */
    bool MousePosToLayoutPos(const UiPoint& ptMouse, UiPoint& ptLayoutPos);

    /** Convert screen coordinates to client area coordinates
    */
    virtual bool ScreenToClient(UiPoint& pt);

    /** Convert client area coordinates to screen coordinates
    */
    virtual bool ClientToScreen(UiPoint& pt);

    /** DPI has changed, update the control size and layout
    * @param [in] nOldDpiScale The old DPI scale percentage
    * @param [in] nNewDpiScale The new DPI scale percentage, consistent with the value of Dpi().GetScale()
    */
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale);

    /** The language has changed, refresh the UI text display related content
    */
    virtual void OnLanguageChanged();

public:
    /** Listen for all events of the control
     * @param [in] callback The event handling callback function, please refer to the EventCallback declaration
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachAllEvents(const EventCallback& callback, EventCallbackID callbackID = 0)    { AttachEvent(kEventAll, callback, callbackID); }

    /** Listen for the mouse enter event
     * @param [in] callback The event handling callback function, please refer to the EventCallback declaration
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachMouseEnter(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventMouseEnter, callback, callbackID); }

    /** Listen for the mouse leave event
     * @param [in] callback The event handling callback function, please refer to the EventCallback declaration
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachMouseLeave(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventMouseLeave, callback, callbackID); }

    /** Listen for the mouse hover event
     * @param [in] callback The event handling callback function, please refer to the EventCallback declaration
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachMouseHover(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventMouseHover, callback, callbackID); }

    /** Listen for the mouse move event
     * @param [in] callback The event handling callback function, please refer to the EventCallback declaration
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachMouseMove(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventMouseMove, callback, callbackID); }

    /** Listen for the mouse button down event (left button)
     * @param [in] callback The event handling callback function, please refer to the EventCallback declaration
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachButtonDown(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventMouseButtonDown, callback, callbackID); }

    /** Listen for the mouse button up event (left button)
     * @param [in] callback The event handling callback function, please refer to the EventCallback declaration
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachButtonUp(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventMouseButtonUp, callback, callbackID); }

    /** Listen for the mouse button down event (right button)
     * @param [in] callback The event handling callback function, please refer to the EventCallback declaration
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachRButtonDown(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventMouseRButtonDown, callback, callbackID); }

    /** Listen for the mouse button up event (right button)
     * @param [in] callback The event handling callback function, please refer to the EventCallback declaration
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachRButtonUp(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventMouseRButtonUp, callback, callbackID); }

    /** Listen for the focus gained event
     * @param [in] callback The event handling callback function, please refer to the EventCallback declaration
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachSetFocus(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventSetFocus, callback, callbackID); }

    /** Listen for the focus lost event
     * @param [in] callback The event handling callback function, please refer to the EventCallback declaration
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachKillFocus(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventKillFocus, callback, callbackID); }

    /** Listen for the window focus lost event
     * @param [in] callback The event handling callback function, please refer to the EventCallback declaration
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachWindowKillFocus(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventWindowKillFocus, callback, callbackID); }

    /** Listen for the context menu event
     * @param [in] callback The event handling callback function, please refer to the EventCallback declaration
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachContextMenu(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventContextMenu, callback, callbackID); }

    /** Listen for the control position change event
     * @param [in] callback The event handling callback function, please refer to the EventCallback declaration
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachPosChanged(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventPosChanged, callback, callbackID); }

    /** Listen for the control size change event
     * @param [in] callback The event handling callback function, please refer to the EventCallback declaration
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachSizeChanged(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventSizeChanged, callback, callbackID); }

    /** Listen for the double click event
     * @param [in] callback The event handling callback function, please refer to the EventCallback declaration
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachDoubleClick(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventMouseDoubleClick, callback, callbackID); }

    /** Bind the mouse click handling function
    * @param [in] callback The callback function to bind
    * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
    */
    void AttachClick(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventClick, callback, callbackID); }

    /** Bind the mouse right click handling function
    * @param [in] callback The callback function to bind
    * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
    */
    void AttachRClick(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventRClick, callback, callbackID); }

    /** Listen for the control show or hide event
    * @param [in] callback The event handling callback function, please refer to the EventCallback declaration
    * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
    */
    void AttachVisibleChanged(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventVisibleChanged, callback, callbackID); }

    /** Listen for the control state change event
    * @param [in] callback The event handling callback function, please refer to the EventCallback declaration
    * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
    */
    void AttachStateChanged(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventStateChanged, callback, callbackID); }

    /** Listen for the control drag and drop enter event
    * @param [in] callback The event handling callback function, please refer to the EventCallback declaration
    * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
    */
    void AttachDropEnter(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventDropEnter, callback, callbackID); }

    /** Listen for the control drag and drop move event
    * @param [in] callback The event handling callback function, please refer to the EventCallback declaration
    * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
    */
    void AttachDropOver(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventDropOver, callback, callbackID); }

    /** Listen for the control drag and drop leave event
    * @param [in] callback The event handling callback function, please refer to the EventCallback declaration
    * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
    */
    void AttachDropLeave(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventDropLeave, callback, callbackID); }

    /** Listen for the control drag and drop data event
    * @param[in] callback The event handling callback function, please refer to the EventCallback declaration
    * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
    */
    void AttachDropData(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventDropData, callback, callbackID); }

    /** Listen for the control image load complete event
    * @param [in] callback The event handling callback function, please refer to the EventCallback declaration
    * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
    */
    void AttachImageLoad(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventImageLoad, callback, callbackID); }

    /** Listen for the control image decode complete event
    * @param [in] callback The event handling callback function, please refer to the EventCallback declaration
    * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
    */
    void AttachImageDecode(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventImageDecode, callback, callbackID); }

    /** Listen for the control destroy event
    * @param [in] callback The event handling callback function, please refer to the EventCallback declaration
    * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
    */
    void AttachDestroy(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventDestroy, callback, callbackID); }
    bool HasDestroyEventCallback() const;

public:
    /** Check whether the color definition is valid
    * @param [in] colorName The name of the color; valid color names can be the following definitions (in order of lookup priority):
    *           (1) Priority 1: starts with the '#' character, directly specifies the color value, e.g. #FFFFFFFF
    *           (2) Priority 2: defined as a child node of the <Window> node in the configuration XML, e.g. <TextColor name="wnd_darkcolor" value="#FF00BB96"/>
    *           (3) Priority 3: defined as a child node of the <Global> node in global.xml, e.g. <TextColor name="white" value="#FFFFFFFF"/>
    *           (4) Priority 4: see the definitions in the ui::UiColors::UiColorConsts function
    */
    bool HasUiColor(const DString& colorName) const;

    /** Get the value corresponding to a color
    * @param [in] colorName The name of the color; valid color names can be the following definitions (in order of lookup priority):
    *           (1) Priority 1: starts with the '#' character, directly specifies the color value, e.g. #FFFFFFFF
    *           (2) Priority 2: defined as a child node of the <Window> node in the configuration XML, e.g. <TextColor name="wnd_darkcolor" value="#FF00BB96"/>
    *           (3) Priority 3: defined as a child node of the <Global> node in global.xml, e.g. <TextColor name="white" value="#FFFFFFFF"/>
    *           (4) Priority 4: directly specify a predefined color alias, see the definitions in the ui::UiColors::UiColorConsts function
    * @return The ARGB color value
    */
    UiColor GetUiColor(const DString& colorName) const;

    /** Get the string corresponding to a color value, returning the string for the color
    * @param [in] color The color value
    * @return Returns the string corresponding to the color value, e.g. "#FF123456"
    */
    DString GetColorString(const UiColor& color) const;

    /** Determine whether the control type is selectable
     * @return Returns false by default
     */
    virtual bool IsSelectableType() const;

    /** Determine whether TAB key messages are accepted
     * @return Returns true for accepted, false for not accepted; the default is false
     */
    virtual bool IsWantTab() const;

    /** Whether the control can be placed on the title bar (to handle NC message responses)
     * @return Returns true for yes, false for no; the default is false
     */
    virtual bool CanPlaceCaptionBar() const;

    /** Whether the current control and its cascading parent containers are visible (from the current control up to the top-level parent control, if any Visible is false, returns false)
    * @return true means the current control and all parent controls are visible
    *         false means there is a non-visible control among the current control or all parent controls
     */
    bool CheckVisibleAncestor(void) const;

    /** Whether the specified key is pressed in this message
    * @param [in] msg The message currently being processed
    * @param [in] modifierKey The keyboard state to check
    */
    bool IsKeyDown(const EventArgs& msg, ModifierKey modifierKey) const;

    /** Whether it is a CEF child window mode control
    */
    virtual bool IsCefNative() const { return false; }

    /** Whether it is a CEF off-screen rendering control
    */
    virtual bool IsCefOSR() const { return false; }

    /** Whether it is a CEF off-screen rendering control that handles input method messages itself
    */
    virtual bool IsCefOsrImeMode() const { return false; }

    /** Set whether the drag and drop feature is allowed
    */
    virtual void SetEnableDragDrop(bool bEnable);

    /** Determine whether the drag and drop feature is already allowed (drag in text and drag in files)
    */
    virtual bool IsEnableDragDrop() const;

    /** Get the drag and drop interface (Windows)
    * @return Returns the drag and drop target interface; nullptr means drag and drop is not supported
    */
    virtual ControlDropTarget_Windows* GetControlDropTarget();

    /** Get the drag and drop interface (SDL)
    * @return Returns the drag and drop target interface; nullptr means drag and drop is not supported
    */
    virtual ControlDropTarget_SDL* GetControlDropTarget_SDL();

    /** Set whether dragging files is allowed
    */
    void SetEnableDropFile(bool bEnable);

    /** Determine whether dragging files is already allowed
    */
    bool IsEnableDropFile() const;

    /** Set the file extension list for file drag and drop
    * @param [in] fileTypes The file extension list, e.g. ".txt;.csv", meaning only txt and csv files are supported; empty means all files are supported
    */
    void SetDropFileTypes(const DString& fileTypes);

    /** Get the filter of the file drag and drop
    */
    DString GetDropFileTypes() const;

public:
    /**@name Event listening related interfaces
    * @{
    */

    /** (m_pOnEvent) Listen to the specified event
     * @param [in] eventType The event type, see the EventType enum
     * @param [in] callback The event handling callback function, please refer to the EventCallback declaration
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachEvent(EventType eventType, const EventCallback& callback, EventCallbackID callbackID);

    /** (m_pOnEvent) Stop listening to the specified event
     * @param [in] eventType The event type, see the EventType enum
     * @param [in] callbackID The ID corresponding to the callback function
     */
    void DetachEvent(EventType eventType);
    void DetachEventByID(EventCallbackID callbackID);
    void DetachEventByID(EventType eventType, EventCallbackID callbackID);

    /** (m_pOnEvent) Whether the specified event is present
     * @param [in] eventType The event type, see the EventType enum
     * @param [in] callbackID The ID corresponding to the callback function
     */
    bool HasEvent(EventType eventType) const;
    bool HasEventByID(EventCallbackID callbackID) const;
    bool HasEventByID(EventType eventType, EventCallbackID callbackID) const;

public:
    /** (m_pOnXmlEvent) Response events added by configuring the <Event tag in XML, the concrete operations are finally handled by the Control::OnApplyAttributeList function
     * @param [in] eventType The event type, see the EventType enum
     * @param [in] callback The event handling callback function, please refer to the EventCallback declaration
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachXmlEvent(EventType eventType, const EventCallback& callback, EventCallbackID callbackID);

    /** (m_pOnXmlEvent) Stop listening to the specified event
     * @param [in] eventType The event type, see the EventType enum
     * @param [in] callbackID The ID corresponding to the callback function
     */
    void DetachXmlEvent(EventType eventType);
    void DetachXmlEventByID(EventCallbackID callbackID);
    void DetachXmlEventByID(EventType eventType, EventCallbackID callbackID);

    /** (m_pOnXmlEvent) Whether the specified event is present
     * @param [in] eventType The event type, see the EventType enum
     * @param [in] callbackID The ID corresponding to the callback function
     */
    bool HasXmlEvent(EventType eventType) const;
    bool HasXmlEventByID(EventCallbackID callbackID) const;
    bool HasXmlEventByID(EventType eventType, EventCallbackID callbackID) const;

public:
    /** (m_pOnBubbledEvent) Bind an event handling function
     * @param [in] eventType The event type
     * @param [in] callback The specified callback function
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachBubbledEvent(EventType eventType, const EventCallback& callback, EventCallbackID callbackID);

    /** (m_pOnBubbledEvent) Unbind an event handling function
     * @param [in] eventType The event type
     * @param [in] callbackID The ID corresponding to the callback function
     */
    void DetachBubbledEvent(EventType eventType);
    void DetachBubbledEventByID(EventCallbackID callbackID);
    void DetachBubbledEventByID(EventType eventType, EventCallbackID callbackID);

    /** (m_pOnBubbledEvent) Whether the event handling function of the specified type is present
     * @param [in] eventType The event type
     * @param [in] callbackID The ID corresponding to the callback function
     */
    bool HasBubbledEvent(EventType eventType) const;
    bool HasBubbledEventByID(EventCallbackID callbackID) const;
    bool HasBubbledEventByID(EventType eventType, EventCallbackID callbackID) const;

public:
    /** Bind the handling functions of the Event and BubbleEvent events written in XML
     * @param [in] eventType The event type
     * @param [in] callback The specified callback function
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachXmlBubbledEvent(EventType eventType, const EventCallback& callback, EventCallbackID callbackID);

    /** Unbind XML event handling functions
     * @param [in] eventType The event type
     * @param [in] callbackID The ID corresponding to the callback function
     */
    void DetachXmlBubbledEvent(EventType eventType);
    void DetachXmlBubbledEventByID(EventCallbackID callbackID);
    void DetachXmlBubbledEventByID(EventType eventType, EventCallbackID callbackID);

    /** Whether the specified event handling function is present
     * @param [in] eventType The event type
     * @param [in] callbackID The ID corresponding to the callback function
     */
    bool HasXmlBubbledEvent(EventType eventType) const;
    bool HasXmlBubbledEventByID(EventCallbackID callbackID) const;
    bool HasXmlBubbledEventByID(EventType eventType, EventCallbackID callbackID) const;

public:
    /** Fire all events, sending events to the listeners of all containers (m_pOnEvent, m_pOnXmlEvent, m_pOnBubbledEvent, m_pOnXmlBubbledEvent)
     *   For events registered in the m_pOnEvent and m_pOnXmlEvent containers: GetSender() needs to be checked to see if it is this; if it is not this, the event is not fired
     *   For events registered in the m_pOnBubbledEvent and m_pOnXmlBubbledEvent containers: GetSender() is not checked to see if it is this, the event is fired directly
     * @param [in] msg The message content
     * @return If all listener callback functions return true, this function returns true; otherwise returns false
     */
    bool FireAllEvents(const EventArgs& msg);

    /** Fire normal events, sending events to the listeners of the following containers (m_pOnEvent, m_pOnXmlEventt)
     *  For events registered in the m_pOnEvent and m_pOnXmlEvent containers: GetSender() needs to be checked to see if it is this; if it is not this, the event is not fired
     * @param [in] msg The message content
     * @return If all listener callback functions return true, this function returns true; otherwise returns false
     */
    bool FireNormalEvents(const EventArgs& msg);

    /** Fire Bubbled events, sending events to the listeners of the following containers (m_pOnBubbledEvent, m_pOnXmlBubbledEvent)
     *   For events registered in the m_pOnBubbledEvent and m_pOnXmlBubbledEvent containers: GetSender() is not checked to see if it is this, the event is fired directly
     * @param [in] msg The message content
     * @return If all listener callback functions return true, this function returns true; otherwise returns false
     */
    bool FireBubbledEvents(const EventArgs& msg);

    /** Determine whether there is an event callback function of a certain type (including all types of events; as long as eventType has been attached, returns true)
    */
    bool HasEventCallback(EventType eventType) const;

    /** @} */

protected:

    //Logic related to giving up the control focus
    void EnsureNoFocus();

    /** Determine whether the message is one that should be filtered out, a helper function
    *   If the current control is in the !IsEnabled() || !IsMouseEnabled() || !IsKeyboardEnabled() state,
        and the message is a mouse or keyboard message, returns true, otherwise returns false
    */
    bool IsDisabledEvents(const EventArgs& msg) const;

    //Protected member functions for message handling, external direct calls are not allowed

    /** The message handling function
    * @param [in] msg The message content
    */
    virtual void HandleEvent(const EventArgs& msg);

    //Mouse messages (returns true: the message has been handled; returns false: the message is unhandled and needs to be forwarded to the parent control)
    virtual bool MouseEnter(const EventArgs& msg);
    virtual bool MouseLeave(const EventArgs& msg);
    virtual bool ButtonDown(const EventArgs& msg);
    virtual bool ButtonUp(const EventArgs& msg);
    virtual bool ButtonDoubleClick(const EventArgs& msg);
    virtual bool RButtonDown(const EventArgs& msg);
    virtual bool RButtonUp(const EventArgs& msg);
    virtual bool RButtonDoubleClick(const EventArgs& msg);
    virtual bool MButtonDown(const EventArgs& msg);
    virtual bool MButtonUp(const EventArgs& msg);
    virtual bool MButtonDoubleClick(const EventArgs& msg);
    virtual bool MouseMove(const EventArgs& msg);
    virtual bool MouseHover(const EventArgs& msg);
    virtual bool MouseWheel(const EventArgs& msg);
    virtual bool MouseMenu(const EventArgs& msg);

    //Keyboard messages (returns true: the message has been handled; returns false: the message is unhandled and needs to be forwarded to the parent control)
    virtual bool OnChar(const EventArgs& msg);
    virtual bool OnKeyDown(const EventArgs& msg);
    virtual bool OnKeyUp(const EventArgs& msg);

    //Cursor and focus related messages (returns true: the message has been handled; returns false: the message is unhandled and needs to be forwarded to the parent control)
    virtual bool OnSetCursor(const EventArgs& msg);
    virtual bool OnSetFocus(const EventArgs& msg);
    virtual bool OnKillFocus(const EventArgs& msg); //The control loses focus
    virtual bool OnWindowSetFocus(const EventArgs& msg);//The window to which the control belongs gains focus
    virtual bool OnWindowKillFocus(const EventArgs& msg);//The window to which the control belongs loses focus
    virtual bool OnCaptureChanged(const EventArgs& msg);//The mouse capture of the window to which the control belongs is lost
    virtual bool OnImeSetContext(const EventArgs& msg);
    virtual bool OnImeStartComposition(const EventArgs& msg);
    virtual bool OnImeComposition(const EventArgs& msg);
    virtual bool OnImeEndComposition(const EventArgs& msg);

    /// Protected member functions for drawing, external direct calls are not allowed
    virtual void PaintShadow(IRender* pRender);         //Draw BoxShadow
    virtual void PaintBkColor(IRender* pRender);        //Draw the background color
    virtual void PaintForeColor(IRender* pRender);      //Draw the foreground color
    virtual void PaintBkImage(IRender* pRender);        //Draw the background image
    virtual void PaintStateColors(IRender* pRender);    //Draw the state colors
    virtual void PaintStateImages(IRender* pRender);    //Draw the state images
    virtual void PaintText(IRender* pRender);           //Draw the text
    virtual void PaintBorder(IRender* pRender);         //Draw the border
    virtual void PaintFocusRect(IRender* pRender);      //Draw the dashed rectangle in the focus state
    virtual void PaintLoading(IRender* pRender, const UiRect& rcPaint);//Draw the control loading state

protected:
    /** Whether there are state images; returns true as long as any state image is present
    */
    bool HasStateImages(void) const;

    /** Whether the image of the specified type is present
    */
    bool HasStateImage(StateImageType stateImageType) const;

    /** Get the image path in the specified state
     */
    DString GetStateImage(StateImageType imageType, ControlStateType stateType) const;

    /** Set the image in the specified state
     */
    void SetStateImage(StateImageType imageType, ControlStateType stateType, const DString& strImage);

    /** Draw the image of the specified type and state
    * @param [in] pRender The draw interface
    * @param [in] stateImageType The image type
    * @param [in] stateType The control state, used to select which image to draw
    * @param [in] sImageModify The additional attributes of the image
    * @param [out] pDestRect Returns the final target rectangle area of the image drawing
    * @return Returns true on successful drawing, otherwise returns false
    */
    bool PaintStateImage(IRender* pRender, 
                         StateImageType stateImageType, 
                         ControlStateType stateType, 
                         const DString& sImageModify = _T(""),
                         UiRect* pDestRect = nullptr);

    /** Clear all state image attributes
    */
    void ClearStateImages();

    /** Set the left margin value of the state images (the margins of all state images are adjusted together)
    * @param [in] leftOffset The image margin offset to set, positive means increase, negative means decrease
    * @param [in] bNeedDpiScale Compatible with DPI scaling, defaults to true
    * @return Returns true on success, false on failure
    */
    bool AdjustStateImagesMarginLeft(int32_t leftOffset, bool bNeedDpiScale);

    /** Get the outer margin of the background image
     */
    UiMargin GetBkImageMargin() const;

    /** Set the outer margin of the background image
     * @param[in] rcMargin The image margin to set
     * @param[in] bNeedDpiScale Compatible with DPI scaling
     */
    bool SetBkImageMargin(UiMargin rcMargin, bool bNeedDpiScale);

    /** Determine whether the background image drawing is disabled
    */
    bool IsBkImagePaintEnabled() const;

    /** Set whether the background image drawing is disabled
    */
    void SetBkImagePaintEnabled(bool bEnable);

    /** Get the background image path (without attributes)
    */
    DString GetBkImagePath() const;

    /** Get the background image size (loads the image on demand)
    */
    UiSize GetBkImageSize() const;

    /** The interface of the background image
    */
    Image* GetBkImagePtr() const;

    /** Check the control estimate result (including whether there is a cached result), and pre-process the estimate conditions
    * @param [in,out] szAvailable The available width and height values input for the estimate, constrained and adjusted internally
    * @param [out] fixedSize The preset width and height values of this control
    * @param [out] returnEstSize Returns the estimate result; when this function returns false, it needs to be used as the return value of EstimateSize
    * @return If false is returned, the estimate needs to be terminated; if true is returned, the estimate needs to continue
    */
    bool PreEstimateSize(UiSize& szAvailable, UiFixedSize& fixedSize, UiEstSize& returnEstSize) const;

protected:
    /** Draw the color of the specified state
    */
    void PaintStateColor(IRender* pRender, ControlStateType stateType) const;

    /** @brief Get the draw area of the control
    */
    const UiRect& GetPaintRect() const { return m_rcPaint; }

    /** @brief Set the draw area of the control
    */
    void SetPaintRect(const UiRect& rect);

    /** Draw the rectangle of the focus state (a rectangle made of dashed lines)
    */
    void DoPaintFocusRect(IRender* pRender);

    /** Pause all animation playback within the control (background image animations, etc.)
    */
    void PauseImageAnimation();

    /** Calculate the control size (width and height)
        If an image is set and either width or height is set to auto, the final size is calculated based on the image size and text size
     *  @param [in] szAvailable The available size, not including the padding or the margin
     *  @param [in] estImageType The type of image estimation
     *  @return The estimated size of the control, including the padding (Box), not including the margin
     */
    UiSize EstimateControlSize(UiSize szAvailable, EstimateImageType estImageType);

    /** Draw the border line
    */
    void DrawBorderLine(IRender* pRender, const UiPointF& pt1, const UiPointF& pt2,
                        float fBorderSize, UiColor dwBorderColor, int8_t borderDashStyle);

    /** Set a rectangular clip area
    */
    std::unique_ptr<AutoClip> CreateRectClip(IRender* pRender, const UiRect& rc, bool bClip) const;

    /** Set a rounded corner clip area
    */
    std::unique_ptr<AutoClip> CreateRoundClip(IRender* pRender, const UiRect& rc, bool bRoundClip) const;

private:
    /** Draw the border: determine whether to draw a rounded rectangle border or a normal rectangle border according to the conditions
    */
    void PaintBorders(IRender* pRender, UiRect rcDraw, 
                      float fBorderSize, UiColor dwBorderColor,
                      int8_t borderDashStyle) const;

    /** Draw a rounded rectangle
    */
    void DrawRoundRect(IRender* pRender, const UiRect& rc, float rx, float ry,
                       UiColor dwBorderColor, float fBorderSize,
                       int8_t borderDashStyle) const;
    void DrawRoundRect(IRender* pRender, const UiRectF& rc, float rx, float ry,
                       UiColor dwBorderColor, float fBorderSize,
                       int8_t borderDashStyle) const;

    /** Fill a rounded rectangle
    */
    void FillRoundRect(IRender* pRender, const UiRect& rc, float rx, float ry, UiColor dwColor) const;

    /** Whether the current control is the Root node of the window
    */
    bool IsRootBox() const;

    /** Whether the current Window is rounded (used to determine whether to use the same rounded corner drawing method as Windows)
    */
    bool IsWindowRoundRect() const;

    /** Determine whether a rounded rectangle should be used to draw the border
    */
    bool ShouldBeRoundRectBorders() const;

public:
    /** Determine whether a rounded rectangle should be used to fill the background color
    */
    bool ShouldBeRoundRectFill() const;

    /** Get the image size in the specified state (loads the image on demand)
    */
    UiSize GetStateImageSize(StateImageType imageType, ControlStateType stateType);

    /** Set the cursor
    * @param [in] cursorType The cursor type
    */
    void SetCursor(CursorType cursorType);

    /** Set the paint order
    * @param [in] nPaintOrder 0 means normal painting, non-zero means a specified paint order, the larger the value the later the painting
    */
    void SetPaintOrder(uint8_t nPaintOrder);

    /** Get the paint order
    */
    uint8_t GetPaintOrder() const;

    /** Get the font data interface corresponding to a font ID
    * @param[in] strFontId The font ID to set, must exist in global.xml
    * @return Returns the font interface on success, external callers do not need to release the resource; returns nullptr on failure
    */
    IFont* GetIFontById(const DString& strFontId) const;

    /** Get the color value corresponding to the color name
    */
    UiColor GetUiColorByName(const DString& colorName) const;

protected:
    /** The visible state change event
    * @param [in] bChanged true means the state has changed, false means the state has not changed
    */
    virtual void OnSetVisible(bool bChanged) override;

    /** The enabled state change event
    * @param [in] bChanged true means the state has changed, false means the state has not changed
    */
    virtual void OnSetEnabled(bool bChanged) override;

private:
    /** Whether BoxShadow is present
    */
    bool HasBoxShadow() const;

    /** Set the value of the control state and trigger the state change event
     * @param[in] controlState The control state to set, please refer to the `ControlStateType` enum
     */
    void PrivateSetState(ControlStateType controlState);

    /** Get the direction of the gradient color
    */
    int8_t GetColor2Direction(const UiString& bkColor2Direction) const;

    /** Parse and process the animation playback attribute
    */
    void ParseStartImageAnimation(const DString& value);

    /** Parse and process the animation stop attribute
    */
    void ParseStopImageAnimation(const DString& value);

    /** Parse and process the animation set current frame attribute
    */
    void ParseSetImageAnimationFrame(const DString& value);

    /** Fire the image load complete and decode complete events
    */
    void FireImageEvent(Image* pImagePtr, const DString& imageFilePath, bool bLoadImage, bool bLoadError, bool bDecodeError) const;

    /** Create a temporary Render object
    */
    std::unique_ptr<IRender> CreateTempRender() const;

    /** Check whether the event type is valid and matches
    * @return Returns true if the check passes, returns false if the check fails
    */
    bool CheckEventType(const EventArgs& msg, EventType eventType) const;

private:
    /** Get the listening event manager of the AttachXXX interfaces
    */
    EventMap& GetAttachEventMap();
    bool HasAttachEventMap() const;

    /** Get the response event management interface added by configuring the <Event> tag in XML
    */
    EventMap& GetXmlEventMap();
    bool HasXmlEventMap() const;

    /** Get the event management interface added through the AttachBubbledEvent interface
    */
    EventMap& GetBubbledEventMap();
    bool HasBubbledEventMap() const;

    /** Get the response event management interface added by configuring the <BubbledEvent> tag in XML
    */
    EventMap& GetXmlBubbledEventMap();
    bool HasXmlBubbledEventMap() const;

private:
    /** Get the playback interface of the Hot animation
    */
    AnimationPlayer* GetHotAnimationPlayer() const;

private:
    /** The implementation function of asynchronous image decoding
    */
    struct TAsyncImageDecode;
    static void AsyncDecodeImageData(std::shared_ptr<TAsyncImageDecode> pAsyncDecoder);

private:
    //Callback event management
    struct TEventMapData
    {
        //Listening events added through the AttachXXX interfaces
        EventMap m_attachEvent;

        //Response events added by configuring the <Event tag in XML, the concrete operations are finally handled by the Control::OnApplyAttributeList function
        EventMap* m_pXmlEvent = nullptr;

        //Events added through the AttachBubbledEvent interface
        EventMap* m_pBubbledEvent = nullptr;

        //Response events added by configuring the <BubbledEvent tag in XML, the concrete operations are finally handled by the Control::OnApplyAttributeList function
        EventMap* m_pXmlBubbledEvent = nullptr;

        //Release resources in the destructor
        ~TEventMapData()
        {
            if (m_pXmlEvent != nullptr) {
                delete m_pXmlEvent;
                m_pXmlEvent = nullptr;
            }
            if (m_pBubbledEvent != nullptr) {
                delete m_pBubbledEvent;
                m_pBubbledEvent = nullptr;
            }
            if (m_pXmlBubbledEvent != nullptr) {
                delete m_pXmlBubbledEvent;
                m_pXmlBubbledEvent = nullptr;
            }
        }
    };

    //Tooltip data
    struct TTooltipData
    {
        //The text content of the ToolTip
        UiString m_sToolTipText;

        //The text ID of the ToolTip
        UiString m_sToolTipTextId;

        //The width of the ToolTip
        int32_t m_nTooltipWidth = 500;
    };

    //Border related data
    struct TBorderData
    {
        //The border sizes of the four sides of the control (the top/bottom/left/right values can be set separately)
        UiRectF m_rcBorderSize;

        //The line styles of the four borders of the control
        int8_t m_borderDashStyle = 0;

        //The border color, each state can specify a different border color
        std::unique_ptr<StateColorMap> m_pBorderColorMap;

        //The border color in the focus state
        UiString m_focusBorderColor;

        /** The border corner radius (applied together with m_rcBorderSize) or the shadow corner radius (applied together with m_boxShadow)
            Only when all four m_rcBorderSize border values are valid and identical
            The value is the original value, not DPI scaled
        */
        UiSize16 m_borderRound;
    };

    //Color related data such as the background color/foreground color
    struct TColorData
    {
        //The background color of the control
        UiString m_strBkColor;

        //The second background color of the control (to implement the gradient background color)
        UiString m_strBkColor2;

        //The direction of the second background color: "1": left->right, "2": top->bottom, "3": top-left->bottom-right, "4": top-right->bottom-left
        int8_t m_nBkColor2Direction = 1;

        //The foreground color of the control
        UiString m_strForeColor;

        //The color of the focus state dashed rectangle
        UiString m_focusRectColor;
    };

    //Drag and drop related data
    struct TDragDropData
    {
        //Whether the drag and drop feature is enabled
        bool m_bDragDropEnabled = false;

        //Whether the file drag and drop feature is enabled
        bool m_bDropFileEnabled = false;
        bool m_bDropFileEnabledDefined = false;

        //The filter of the file drag and drop (by file extension, e.g. ".txt;.csv", meaning only txt and csv files are supported)
        UiString m_dropFileTypes;

#ifdef DUI_BUILD_FOR_WIN
        //Drag and drop implementation: Windows
        std::shared_ptr<ControlDropTargetImpl_Windows> m_pDropTargetWindows;
#endif

#ifdef DUI_BUILD_FOR_SDL
        //Drag and drop implementation: SDL
        std::shared_ptr<ControlDropTargetImpl_SDL> m_pDropTargetSDL;
#endif
    };

    /** Animation related data
    */
    struct TAnimationData
    {
        /** The control animation playback manager
        */
        std::unique_ptr<AnimationManager> m_animationManager;

        /** The render offset when the control plays an animation (X coordinate offset and Y coordinate offset)
        */
        UiPoint m_renderOffset;
    };

    /** Infrequently used feature data
    */
    struct TOtherData
    {
        /** The control shadow, its corner radius is controlled by the m_borderRound variable
        */
        std::unique_ptr<BoxShadow> m_pBoxShadow;

        /** The implementation interface of the control "loading" logic
        */
        std::unique_ptr<ControlLoading> m_pLoading;

        /** Tooltip data
        */
        std::unique_ptr<TTooltipData> m_pTooltip;
    };

private:
    /** The background image
    */
    std::unique_ptr<Image> m_pBkImage;

    /** The border data
    */
    std::unique_ptr<TBorderData> m_pBorderData;

    /** The background color
    */
    std::unique_ptr<TColorData> m_pColorData;

    /** The MAP of states and color values, each state can specify a different color
    */
    std::unique_ptr<StateColorMap2> m_pColorMap;

    /** The MAP of control image types and state images
    */
    std::unique_ptr<StateImageMap> m_pImageMap;

    /** The draw render engine interface (the control itself, only used when transparency is set)
    */
    std::unique_ptr<IRender> m_pTempRender;

    /** The callback event manager
    */
    std::unique_ptr<TEventMapData> m_pEventMapData;

    /** The control animation related data
    */
    std::unique_ptr<TAnimationData> m_pAnimationData;

    /** The drag and drop related data
    */
    std::unique_ptr<TDragDropData> m_pDragDropData;

    /** Other infrequently used data
    */
    std::unique_ptr<TOtherData> m_pOtherData;

    /** The playback interface of the Hot animation
    */
    std::unique_ptr<AnimationPlayer> m_pHotAnimationPlayer;

    /** The draw area of the control
    */
    UiRect m_rcPaint;

    /** The user data ID (string)
    */
    UiString m_sUserDataID;

    /** The user data ID (integer value)
    */
    size_t m_uUserDataID;

private:
    /** Whether box-shadow has been drawn (since box-shadow drawing exceeds the GetRect() range, special handling is required)
    */
    bool m_bBoxShadowPainted;

    /** The control state (ControlStateType)
    */
    int8_t m_controlState;

    //The transparency of the control (0 - 255, 0 is fully transparent, 255 is opaque)
    uint8_t m_nAlpha;

    //The transparency of the control in the Hot state (0 - 255, 0 is fully transparent, 255 is opaque)
    uint8_t m_nHotAlpha;

    //Whether the mouse focus is on the control
    bool m_bMouseFocused;

    //Whether the control responds to the context menu
    bool m_bContextMenuUsed;

    //The control does not need focus (if true, the control will not get focus)
    bool m_bNoFocus;

    //Whether TAB focus switching is allowed
    bool m_bAllowTabstop;

    //The cursor type of the control (CursorType)
    CursorType m_cursorType;

    //Whether to show the focus state (a rectangle made of dashed lines)
    bool m_bShowFocusRect;

    //The paint order: 0 means normal painting, non-zero means a specified paint order, the larger the value the later the painting
    uint8_t m_nPaintOrder;

    //Whether the border is on top (i.e. child controls are drawn first and the border afterwards, so the border is not covered by child controls)
    bool m_bBordersOnTop;

    //Whether it is in the MouseEnter state (a flag used to trigger events)
    bool m_bMouseEnter;
};

} // namespace ui

#endif // UI_CORE_CONTROL_H_
