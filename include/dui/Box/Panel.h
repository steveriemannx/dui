#ifndef UI_BOX_PANEL_H_
#define UI_BOX_PANEL_H_

#include "dui/Core/Box.h"
#include "dui/Box/HBox.h"
#include "dui/Box/VBox.h"
#include "dui/Animation/AnimationPlayer.h"
#include "dui/Core/ControlFinder.h"
#include "dui/Core/ControlPtrT.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Core/Window.h"
#include "dui/Render/AutoClip.h"
#include "dui/Render/IRender.h"
#include "dui/Utils/AttributeUtil.h"
#include "dui/Utils/StringUtil.h"

namespace ui
{

/** Accordion group interface: it lets the three template instances Panel / PanelHBox / PanelVBox be
*   recognized and operated uniformly.
*
*   No window-level registry is needed: when a panel expands, it looks for the panels of the same group
*   in the control tree of its window and collapses them, so the grouping scope is a single Window, the
*   same as the group scope of Option.
*/
class IPanelGroup
{
public:
    virtual ~IPanelGroup() = default;

    /** Gets the accordion group name (an empty string means that the panel does not belong to any group)
    */
    virtual std::string GetPanelGroupName() const = 0;

    /** Whether the panel is currently collapsed
    */
    virtual bool IsPanelCollapsed() const = 0;

    /** Sets the collapsed state (called by the other panels of the same group for the accordion linkage)
    */
    virtual void SetPanelCollapsed(bool bCollapsed, bool bFireEvent) = 0;
};

/** Panel control: a container with a title bar that can be collapsed/expanded
*
*   Difference from GroupBox: GroupBox outlines a group with lines, while Panel owns a separate title bar
*   area whose background color/text color/font/height can be configured, and it collapses/expands the
*   content area when the title bar or the arrow is clicked.
*
*   XML attributes (in addition to the common Box/Control attributes):
*     title             Title text
*     title_id          Multilingual resource ID of the title text (a KEY in the language file); once it is
*                       set, the title refreshes automatically when the language changes, the same way as
*                       Label's text_id. When both title and title_id are set, the attribute set last wins
*     title_height      Title bar height (pixels, default 28, DPI aware)
*     title_bk_color    Title bar background color (semantic color name; no background is painted when unset)
*     title_text_color  Title text color (semantic color name, default text_default)
*     title_text_align  Horizontal alignment of the title text: "left" (default) / "hcenter" / "right"
*     title_font        Title font ID (a Font id from global.xml, default system_bold_14)
*     collapsible       Whether the panel can be collapsed (true/false, default false)
*     collapsed         Whether the panel starts collapsed (true/false, default false; the initial state is not animated)
*     collapse_trigger  Hot zone that toggles the collapsed state: "title" (the whole title bar, default) or
*                       "arrow" (the arrow area only)
*     collapse_anim     Collapse/expand animation duration (milliseconds, default 0 meaning no animation;
*                       150~300 is recommended)
*     arrow_align       Arrow position: "right" (right side of the title bar, default) or "left" (left side
*                       of the title bar)
*     group             Accordion group name: only one panel of a group may be expanded at a time, expanding a
*                       panel collapses the others (the scope is the current window; collapsing all panels is
*                       allowed; for the initial state mark only one expanded panel in the XML)
*     title_slot        Title bar slot: the name of a child control; that control (float="true" recommended) is
*                       positioned automatically on the right side of the title bar (left of the arrow), so it
*                       can host any control such as a button or a check box
*     arrow_expanded_normal_image / arrow_expanded_hovered_image /
*     arrow_expanded_pushed_image / arrow_expanded_disabled_image
*                       Images of the expanded (▼) arrow in each control state
*     arrow_collapsed_normal_image / arrow_collapsed_hovered_image /
*     arrow_collapsed_pushed_image / arrow_collapsed_disabled_image
*                       Images of the collapsed (▶) arrow in each control state
*                       (a default glyph arrow is used when no image is configured; a missing state image
*                       falls back to the normal image)
*
*   Layout notes:
*     1. The title bar space is reserved automatically at the top of the content area; the padding attribute
*        keeps its standard meaning: it sets the padding of the content area below the title bar and works
*        as usual.
*     2. Collapsing does not change the height mode of the control (fixed/auto/stretch are all safe) and does
*        not change the visible state of child controls: the content area is masked in measurement, painting
*        and hit testing instead, so expanding again has no side effects.
*
*   Events: kEventCollapse / kEventExpand fire when collapsing/expanding finishes (immediately when there is
*   no animation, at the end of the animation otherwise); they can be listened to with AttachCollapse /
*   AttachExpand. kEventPanelCollapsing / kEventPanelExpanding fire before collapsing/expanding; a listener may
*   return false to cancel the operation (the initial XML collapsed attribute and the accordion internal
*   linkage do not fire these cancellable events).
*/
template<typename InheritType = Box>
class PanelTemplate : public InheritType, public IPanelGroup
{
    typedef InheritType BaseClass;
public:
    explicit PanelTemplate(Window* pWindow);
    virtual ~PanelTemplate() override;

    /// Overrides of the base class interface for custom behavior; see the base class declarations for the details
    virtual std::string GetType() const override;
    virtual void SetAttribute(const std::string& strName, const std::string& strValue) override;
    virtual UiEstSize EstimateSize(UiSize szAvailable) override;
    virtual void SetPos(UiRect rc) override;
    virtual void PaintChild(IRender* pRender, const UiRect& rcPaint) override;
    virtual void PaintText(IRender* pRender) override;
    virtual bool ButtonUp(const EventArgs& msg) override;
    virtual Control* FindControl(FINDCONTROLPROC Proc, void* pProcData, uint32_t uFlags,
                                 const UiPoint& ptMouse, const UiPoint& scrollPos) override;

    /** The DPI changed: updates the control size and the layout
    * @param [in] nOldDpiScale The old DPI scale percentage
    * @param [in] nNewDpiScale The new DPI scale percentage, consistent with the value of Dpi().GetScale()
    */
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;

    /** The language changed: the title text is fetched again through its title text ID and refreshed
    */
    virtual void OnLanguageChanged() override;

public:
    /** Sets the title text
    */
    void SetTitle(const std::string& strTitle);

    /** Gets the title text
    */
    std::string GetTitle() const;

    /** Sets the title text ID (multilingual, used the same way as Label::SetTextId)
    * @param [in] strTitleId Title text ID configured in the language file; it is resolved and displayed
    *                        immediately in the current language and refreshes automatically when the
    *                        language changes
    */
    void SetTitleId(const std::string& strTitleId);

    /** Gets the title text ID
    */
    std::string GetTitleId() const;

    /** Sets the title bar height (pixels)
    * @param [in] nHeight Title bar height
    * @param [in] bNeedDpiScale Whether to scale the value by the DPI factor
    */
    void SetTitleBarHeight(int32_t nHeight, bool bNeedDpiScale);

    /** Gets the title bar height (in pixels, already DPI scaled)
    */
    int32_t GetTitleBarHeight() const;

    /** Sets the title bar background color (semantic color name)
    */
    void SetTitleBkColor(const std::string& strColor);

    /** Sets the title text color (semantic color name)
    */
    void SetTitleTextColor(const std::string& strColor);

    /** Sets the horizontal alignment of the title text
    * @param [in] uHAlign TEXT_LEFT (default) / TEXT_HCENTER / TEXT_RIGHT
    */
    void SetTitleTextHAlign(uint32_t uHAlign);

    /** Sets the title font ID (a Font id from global.xml)
    */
    void SetTitleFontId(const std::string& strFontId);

    /** Sets whether the panel can be collapsed
    */
    void SetCollapsible(bool bCollapsible);

    /** Whether the panel can be collapsed
    */
    bool IsCollapsible() const;

    /** Sets the collapsed state
    * @param [in] bCollapsed true collapses the panel (only the title bar is shown), false expands it
    * @param [in] bFireEvent Whether to fire the kEventCollapse / kEventExpand event
    * @param [in] bPlayAnim Whether to play the collapse/expand animation; when false the final state is
    *                       applied immediately
    */
    void SetCollapsed(bool bCollapsed, bool bFireEvent = true, bool bPlayAnim = true);

    /** Whether the panel is collapsed
    */
    bool IsCollapsed() const;

    /** Hot zone type that toggles the collapsed state
    */
    enum class CollapseTrigger
    {
        kTitle, ///< The whole title bar is clickable (default)
        kArrow  ///< Only the arrow area is clickable
    };

    /** Position of the arrow inside the title bar
    */
    enum class ArrowAlign
    {
        kRight, ///< The arrow is on the right side of the title bar (default)
        kLeft   ///< The arrow is on the left side of the title bar (right of the text)
    };

    /** Sets the arrow position
    */
    void SetArrowAlign(ArrowAlign align);

    /** Gets the arrow position
    */
    ArrowAlign GetArrowAlign() const;

    /** Sets the hot zone that toggles the collapsed state
    */
    void SetCollapseTrigger(CollapseTrigger trigger);

    /** Gets the hot zone that toggles the collapsed state
    */
    CollapseTrigger GetCollapseTrigger() const;

    /** Sets the collapse/expand animation duration
    * @param [in] nMillSeconds Animation duration (milliseconds); 0 means no animation, the state is switched immediately
    */
    void SetCollapseAnimMillSeconds(int32_t nMillSeconds);

    /** Gets the collapse/expand animation duration (milliseconds)
    */
    int32_t GetCollapseAnimMillSeconds() const;

    /** Sets the accordion group name
    * @param [in] strGroupName Group name; an empty string takes the panel out of its group
    */
    void SetGroup(const std::string& strGroupName);

    /** Gets the accordion group name
    */
    std::string GetGroup() const;

    //IPanelGroup interface implementation (used for the linkage between panels of the same group and to
    //recognize the three Panel types uniformly while the control tree is walked)
    virtual std::string GetPanelGroupName() const override;
    virtual bool IsPanelCollapsed() const override;
    virtual void SetPanelCollapsed(bool bCollapsed, bool bFireEvent) override;

    /** Sets the padding of the content area (the title bar space is reserved automatically by the control,
    *  so padding keeps its standard meaning)
    * @note This hides PlaceHolder::SetPadding with the same name: it takes effect when called through a
    *       Panel type pointer; the XML padding attribute also reaches this implementation
    */
    void SetPadding(UiPadding rcPadding, bool bNeedDpiScale);

    /** Listens for the expand event
    */
    void AttachExpand(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventExpand, callback, callbackID); }

    /** Listens for the collapse event
    */
    void AttachCollapse(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventCollapse, callback, callbackID); }

    /** Listens for the "about to collapse" event: returning false from the callback cancels this collapse
    */
    void AttachCollapsing(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventPanelCollapsing, callback, callbackID); }

    /** Listens for the "about to expand" event: returning false from the callback cancels this expand
    */
    void AttachExpanding(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventPanelExpanding, callback, callbackID); }

    /** Sets an arrow state image
    * @param [in] bExpanded true sets the expanded (▼) arrow image, false sets the collapsed (▶) arrow image
    * @param [in] stateType Control state (normal/hovered/pressed/disabled)
    * @param [in] strImage Image attribute string (file='...' width='..' height='..' and so on)
    */
    void SetArrowStateImage(bool bExpanded, ControlStateType stateType, const std::string& strImage);

    /** Gets an arrow state image attribute string
    */
    std::string GetArrowStateImage(bool bExpanded, ControlStateType stateType) const;

    /** Sets the title bar slot: the value is the name of a child control, and that child control is
    *  positioned on the right side of the title bar (left of the arrow)
    * @note float="true" is recommended for the slot control, so that it does not take layout space in the
    *       content area
    */
    void SetTitleSlotName(const std::string& strSlotName);

    /** Gets the name of the title bar slot child control
    */
    std::string GetTitleSlotName() const;

private:
    /** Applies the content area padding (the user padding plus the title bar height) to the padding storage
    *  of the base class
    */
    void ApplyContentPadding();

    /** Toggles the collapsed state (called when the title bar is clicked)
    */
    void ToggleCollapsed();

    /** Gets the title bar area (in window coordinates)
    */
    UiRect GetTitleBarRect() const;

    /** Gets the arrow hot zone (the square area on the side of the title bar)
    */
    UiRect GetArrowRect() const;

    /** Stops the current collapse animation (without firing the complete event; used when the direction is
    *  reversed or the control state changes)
    */
    void StopCollapseAnim();

    /** Enters the collapsed layout: the minimum height is temporarily relaxed to the title bar height
    *  The layout engine clamps EstimateSize with min_height, so without relaxing it the collapsed panel would
    *  still occupy min_height
    */
    void BeginCollapseLayout();

    /** Leaves the collapsed layout: the user configured minimum height is restored
    */
    void EndCollapseLayout();

    /** Starts the collapse/expand animation
    * @param [in] bCollapse true collapses (expanded height → title bar height), false expands (the reverse)
    * @param [in] nFromHeight Animation start height (pixels)
    * @param [in] bFireEvent Whether to fire kEventCollapse / kEventExpand when the animation completes
    */
    void StartCollapseAnim(bool bCollapse, int32_t nFromHeight, bool bFireEvent);

    /** Estimates the complete content height in the expanded state (pixels)
    * @param [in] nFallbackHeight Fallback height used when the layout cannot provide an estimate (such as in
    *                             stretch mode)
    */
    int32_t EstimateExpandedHeight(int32_t nFallbackHeight);

    /** Animation frame callback: applies the current animation height and requests a new layout
    */
    void OnAnimHeightChanged(int32_t nHeight);

    /** The animation finished: leaves the animation state and does the final layout according to the logical
    *  collapsed state
    * @param [in] bFireEvent Whether to fire kEventCollapse / kEventExpand
    */
    void OnCollapseAnimFinished(bool bFireEvent);

    /** Accordion linkage: collapses the other expanded panels of the same group in the current window
    * @param [in] bFireEvent Whether to fire their kEventCollapse event when the other panels are collapsed
    */
    void CollapseGroupPeers(bool bFireEvent);

    /** Internal implementation of collapsing/expanding (does not fire the "about to" cancellable event; used by
    *  the accordion linkage and other internal paths)
    */
    void ApplyCollapsed(bool bCollapsed, bool bFireEvent, bool bPlayAnim = true);

    /** Fires the "about to collapse/expand" cancellable event
    * @return false means that a listener asked to cancel the operation
    */
    bool FireBeforeCollapseEvent(bool bCollapsed);

    /** Finds the title bar slot child control (cached; the cache is dropped and the lookup repeated once the
    *  slot control has been destroyed)
    */
    Control* GetTitleSlotControl();

    /** Positions the title bar slot child control on the right side of the title bar (left of the arrow)
    */
    void LayoutTitleSlot();

    /** Paints the arrow image (used instead of the default glyph when a state image is configured)
    * @return true the image was painted; false no image is configured and the glyph should be painted instead
    */
    bool PaintArrowImage(IRender* pRender, const UiRect& rcArrow);

private:
    //Title text
    UiString m_title;

    //Title text ID, used for the multilingual support (empty means the title is not bound to a language resource)
    UiString m_titleId;

    //Title bar height (pixels, already DPI scaled)
    int32_t m_nTitleBarHeight;

    //Title bar background color
    UiString m_titleBkColor;

    //Title text color
    UiString m_titleTextColor;

    //Horizontal alignment of the title text (TEXT_LEFT / TEXT_HCENTER / TEXT_RIGHT)
    uint32_t m_uTitleTextHAlign;

    //Title font ID
    UiString m_titleFontId;

    //Arrow position (right side by default)
    ArrowAlign m_arrowAlign;

    //Whether the panel can be collapsed
    bool m_bCollapsible;

    //Whether the panel is collapsed
    bool m_bCollapsed;

    //Hot zone that toggles the collapsed state
    CollapseTrigger m_collapseTrigger;

    //Content area padding set by the user (already DPI scaled, without the reserved title bar space)
    UiPadding m_contentPadding;

    //Collapse/expand animation duration (milliseconds), 0 means no animation
    int32_t m_nCollapseAnimMs;

    //Animation player (non-null only while an animation is running)
    std::unique_ptr<AnimationPlayer> m_pAnimationPlayer;

    //Current height during the animation (pixels); -1 means no animation is playing
    int32_t m_nAnimHeight;

    //Actual height of the last fully expanded layout (pixels), used as the collapse animation start point and as
    //the fallback target when expanding
    int32_t m_nExpandedHeight;

    //User minimum height saved temporarily while collapsed (pixels); -1 means the collapsed layout is not active
    //While collapsed the layout engine still clamps the EstimateSize result with min_height, so the value has to
    //be relaxed to the title bar height temporarily
    int32_t m_nSavedMinHeight;

    //Accordion group name (empty means the panel does not belong to any group)
    UiString m_groupName;

    //Name of the title bar slot child control
    UiString m_titleSlotName;

    //Title bar slot child control (weak reference cache; cleared automatically once the control is destroyed)
    ControlPtr m_pTitleSlot;
};

template<typename InheritType>
PanelTemplate<InheritType>::PanelTemplate(Window* pWindow):
    InheritType(pWindow),
    m_nTitleBarHeight(0),
    m_uTitleTextHAlign(TEXT_LEFT),
    m_arrowAlign(ArrowAlign::kRight),
    m_bCollapsible(false),
    m_bCollapsed(false),
    m_collapseTrigger(CollapseTrigger::kTitle),
    m_nCollapseAnimMs(0),
    m_nAnimHeight(-1),
    m_nExpandedHeight(0),
    m_nSavedMinHeight(-1)
{
    this->SetMouseEnabled(true);
    SetTitleBarHeight(28, true);
}

template<typename InheritType>
PanelTemplate<InheritType>::~PanelTemplate()
{
    //Stop the animation timer before destruction, so that the timer callback does not access an already
    //destroyed object (the callback itself is protected by a weak reference)
    if (m_pAnimationPlayer != nullptr) {
        m_pAnimationPlayer->SetCompleteCallback(nullptr);
        m_pAnimationPlayer->SetPlayCallback(nullptr);
        m_pAnimationPlayer->Stop();
    }
}

template<typename InheritType>
inline std::string PanelTemplate<InheritType>::GetType() const { return DUI_CTR_PANEL; }

template<>
inline std::string PanelTemplate<HBox>::GetType() const { return DUI_CTR_PANEL_HBOX; }

template<>
inline std::string PanelTemplate<VBox>::GetType() const { return DUI_CTR_PANEL_VBOX; }

template<typename InheritType>
void PanelTemplate<InheritType>::SetAttribute(const std::string& strName, const std::string& strValue2)
{
    //dui does not support variable expansion in attribute values, so the value is used as is
    const std::string& strValue = strValue2;
    if (strName == "title") {
        SetTitle(strValue);
    }
    else if ((strName == "title_id") || (strName == "titleid")) {
        //Multilingual resource ID of the title text
        SetTitleId(strValue);
    }
    else if (strName == "title_height") {
        //Title bar height
        SetTitleBarHeight(StringUtil::StringToInt32(strValue), true);
    }
    else if (strName == "title_bk_color") {
        //Title bar background color
        SetTitleBkColor(strValue);
    }
    else if (strName == "title_text_color") {
        //Title text color
        SetTitleTextColor(strValue);
    }
    else if (strName == "title_text_align") {
        //Horizontal alignment of the title text: left (default) / hcenter / right
        if (strValue == "hcenter") {
            SetTitleTextHAlign(TEXT_HCENTER);
        }
        else if (strValue == "right") {
            SetTitleTextHAlign(TEXT_RIGHT);
        }
        else {
            SetTitleTextHAlign(TEXT_LEFT);
        }
    }
    else if (strName == "arrow_align") {
        //Arrow position: right (default) / left
        SetArrowAlign(strValue == "left" ? ArrowAlign::kLeft : ArrowAlign::kRight);
    }
    else if (strName == "title_font") {
        //Title font ID
        SetTitleFontId(strValue);
    }
    else if (strName == "collapsible") {
        //Whether the panel can be collapsed
        //dui has no StringUtil::IsValueTrue; booleans are compared against "true", as in the other dui controls
        SetCollapsible(strValue == "true");
    }
    else if (strName == "collapsed") {
        //Initial collapsed state (no event is fired and no animation is played; the child controls may not be
        //parsed yet at this point, the collapsed state is handled uniformly by measurement/painting/hit testing)
        SetCollapsed(strValue == "true", false, false);
    }
    else if (strName == "collapse_trigger") {
        //Hot zone that toggles the collapsed state
        if (strValue == "arrow") {
            SetCollapseTrigger(CollapseTrigger::kArrow);
        }
        else {
            SetCollapseTrigger(CollapseTrigger::kTitle);
        }
    }
    else if (strName == "collapse_anim") {
        //Collapse/expand animation duration (milliseconds, 0 disables the animation)
        SetCollapseAnimMillSeconds(StringUtil::StringToInt32(strValue));
    }
    else if (strName == "group") {
        //Accordion group name
        SetGroup(strValue);
    }
    else if (strName == "title_slot") {
        //Name of the title bar slot child control
        SetTitleSlotName(strValue);
    }
    else if (strName == "arrow_expanded_normal_image") {
        SetArrowStateImage(true, kControlStateNormal, strValue);
    }
    else if ((strName == "arrow_expanded_hovered_image") || (strName == "arrow_expanded_hot_image")) {
        SetArrowStateImage(true, kControlStateHot, strValue);
    }
    else if ((strName == "arrow_expanded_pushed_image") || (strName == "arrow_expanded_pressed_image")) {
        SetArrowStateImage(true, kControlStatePushed, strValue);
    }
    else if (strName == "arrow_expanded_disabled_image") {
        SetArrowStateImage(true, kControlStateDisabled, strValue);
    }
    else if (strName == "arrow_collapsed_normal_image") {
        SetArrowStateImage(false, kControlStateNormal, strValue);
    }
    else if ((strName == "arrow_collapsed_hovered_image") || (strName == "arrow_collapsed_hot_image")) {
        SetArrowStateImage(false, kControlStateHot, strValue);
    }
    else if ((strName == "arrow_collapsed_pushed_image") || (strName == "arrow_collapsed_pressed_image")) {
        SetArrowStateImage(false, kControlStatePushed, strValue);
    }
    else if (strName == "arrow_collapsed_disabled_image") {
        SetArrowStateImage(false, kControlStateDisabled, strValue);
    }
    else if (strName == "padding") {
        //padding keeps its standard meaning: the padding of the content area (the title bar space is reserved
        //on top of it automatically)
        UiPadding rcPadding;
        AttributeUtil::ParsePaddingValue(strValue.c_str(), rcPadding);
        SetPadding(rcPadding, true);
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

template<typename InheritType>
void PanelTemplate<InheritType>::SetTitle(const std::string& strTitle)
{
    if (m_title != strTitle) {
        m_title = strTitle;
        this->Invalidate();
    }
}

template<typename InheritType>
std::string PanelTemplate<InheritType>::GetTitle() const
{
    return m_title.c_str();
}

template<typename InheritType>
void PanelTemplate<InheritType>::SetTitleId(const std::string& strTitleId)
{
    if (m_titleId == strTitleId) {
        return;
    }
    m_titleId = strTitleId;
    //Resolve and display the text in the current language immediately; when the ID is not found in the language
    //file, the existing title is kept
    std::string strTitle = GlobalManager::Instance().Lang().GetStringViaID(strTitleId);
    if (!strTitle.empty()) {
        SetTitle(strTitle);
    }
}

template<typename InheritType>
std::string PanelTemplate<InheritType>::GetTitleId() const
{
    return m_titleId.c_str();
}

template<typename InheritType>
void PanelTemplate<InheritType>::OnLanguageChanged()
{
    BaseClass::OnLanguageChanged();
    //When the title is bound to a multilingual ID, the title text is resolved again in the new language
    if (!m_titleId.empty()) {
        std::string strTitle = GlobalManager::Instance().Lang().GetStringViaID(m_titleId.c_str());
        if (!strTitle.empty()) {
            SetTitle(strTitle);
        }
    }
}

template<typename InheritType>
void PanelTemplate<InheritType>::SetTitleBarHeight(int32_t nHeight, bool bNeedDpiScale)
{
    if (nHeight < 0) {
        nHeight = 0;
    }
    if (bNeedDpiScale) {
        nHeight = this->Dpi().GetScaleInt(nHeight);
    }
    if (m_nTitleBarHeight != nHeight) {
        m_nTitleBarHeight = nHeight;
        ApplyContentPadding();
        this->ArrangeAncestor();
        this->Invalidate();
    }
}

template<typename InheritType>
int32_t PanelTemplate<InheritType>::GetTitleBarHeight() const
{
    return m_nTitleBarHeight;
}

template<typename InheritType>
void PanelTemplate<InheritType>::SetTitleBkColor(const std::string& strColor)
{
    if (m_titleBkColor != strColor) {
        m_titleBkColor = strColor;
        this->Invalidate();
    }
}

template<typename InheritType>
void PanelTemplate<InheritType>::SetTitleTextColor(const std::string& strColor)
{
    if (m_titleTextColor != strColor) {
        m_titleTextColor = strColor;
        this->Invalidate();
    }
}

template<typename InheritType>
void PanelTemplate<InheritType>::SetTitleTextHAlign(uint32_t uHAlign)
{
    if ((uHAlign != TEXT_LEFT) && (uHAlign != TEXT_HCENTER) && (uHAlign != TEXT_RIGHT)) {
        uHAlign = TEXT_LEFT;
    }
    if (m_uTitleTextHAlign != uHAlign) {
        m_uTitleTextHAlign = uHAlign;
        this->Invalidate();
    }
}

template<typename InheritType>
void PanelTemplate<InheritType>::SetArrowAlign(ArrowAlign align)
{
    if (m_arrowAlign != align) {
        m_arrowAlign = align;
        //The arrow hot zone position affects the slot and the title text layout, so a new layout is needed
        this->ArrangeAncestor();
        this->Invalidate();
    }
}

template<typename InheritType>
typename PanelTemplate<InheritType>::ArrowAlign PanelTemplate<InheritType>::GetArrowAlign() const
{
    return m_arrowAlign;
}

template<typename InheritType>
void PanelTemplate<InheritType>::SetTitleFontId(const std::string& strFontId)
{
    if (m_titleFontId != strFontId) {
        m_titleFontId = strFontId;
        this->Invalidate();
    }
}

template<typename InheritType>
void PanelTemplate<InheritType>::SetCollapsible(bool bCollapsible)
{
    if (m_bCollapsible != bCollapsible) {
        m_bCollapsible = bCollapsible;
        //Stop the running animation when the collapsible capability changes, so that the animation does not end
        //in a state that differs from the final state
        StopCollapseAnim();
        //Lay out again after the collapsible capability changed: when collapsed=true was recorded earlier, the
        //panel is shown collapsed right after collapsing is enabled; when collapsing is disabled the panel
        //returns to the expanded state and the content area is no longer masked
        if (bCollapsible && m_bCollapsed) {
            BeginCollapseLayout();
        }
        else if (!bCollapsible) {
            EndCollapseLayout();
        }
        this->ArrangeAncestor();
        this->Invalidate();
    }
}

template<typename InheritType>
bool PanelTemplate<InheritType>::IsCollapsible() const
{
    return m_bCollapsible;
}

template<typename InheritType>
void PanelTemplate<InheritType>::SetCollapsed(bool bCollapsed, bool bFireEvent, bool bPlayAnim)
{
    if (m_bCollapsed == bCollapsed) {
        //The target state is the same (whether an animation is playing or not): ignore it; the direction is only
        //reversed in the middle of an animation when the target is the opposite state
        return;
    }
    //The initial collapsed state may be recorded before collapsible is set (independently of the attribute order
    //in the XML); the actual collapsing behavior is decided by (m_bCollapsible && m_bCollapsed)
    if (!m_bCollapsible) {
        m_bCollapsed = bCollapsed;
        return;
    }

    //Cancellable event before collapsing/expanding (not fired when the initial XML collapsed attribute is
    //applied with bFireEvent=false, because the control tree may not be parsed yet); the operation is dropped as
    //soon as one listener returns false
    if (bFireEvent && !FireBeforeCollapseEvent(bCollapsed)) {
        return;
    }
    ApplyCollapsed(bCollapsed, bFireEvent, bPlayAnim);
}

template<typename InheritType>
void PanelTemplate<InheritType>::ApplyCollapsed(bool bCollapsed, bool bFireEvent, bool bPlayAnim)
{
    m_bCollapsed = bCollapsed;

    //Accordion linkage: when this panel expands, the other panels of the same group in the same window are
    //collapsed (collapsing does not recursively trigger the linkage or the cancellable event)
    if (!bCollapsed) {
        CollapseGroupPeers(bFireEvent);
    }

    if (!bPlayAnim || (m_nCollapseAnimMs <= 0) || (m_nTitleBarHeight <= 0)) {
        //No animation: switch immediately
        StopCollapseAnim();
        if (bCollapsed) {
            BeginCollapseLayout();
        }
        else {
            EndCollapseLayout();
        }
        this->ArrangeAncestor();
        this->Invalidate();
        if (bFireEvent) {
            this->SendEvent(bCollapsed ? kEventCollapse : kEventExpand);
        }
        return;
    }

    //Relax the minimum height before the collapse animation starts, so that the animation is not clamped as soon
    //as it shrinks to min_height (the expand direction restores it when the animation completes)
    if (bCollapsed) {
        BeginCollapseLayout();
    }

    //Animation: start from the currently presented height (so the direction can be reversed mid-animation), the
    //target height follows from the direction
    int32_t nFromHeight = m_nTitleBarHeight;
    if (m_pAnimationPlayer != nullptr && m_pAnimationPlayer->IsPlaying()) {
        nFromHeight = (m_nAnimHeight >= 0) ? m_nAnimHeight : m_nTitleBarHeight;
        StopCollapseAnim();
    }
    else {
        UiRect rc = this->GetRect();
        if (!rc.IsEmpty()) {
            nFromHeight = rc.Height();
        }
        else if (m_nExpandedHeight > m_nTitleBarHeight) {
            nFromHeight = m_nExpandedHeight;
        }
    }
    StartCollapseAnim(bCollapsed, nFromHeight, bFireEvent);
}

template<typename InheritType>
bool PanelTemplate<InheritType>::IsCollapsed() const
{
    return m_bCollapsed;
}

template<typename InheritType>
void PanelTemplate<InheritType>::SetCollapseTrigger(CollapseTrigger trigger)
{
    m_collapseTrigger = trigger;
}

template<typename InheritType>
typename PanelTemplate<InheritType>::CollapseTrigger PanelTemplate<InheritType>::GetCollapseTrigger() const
{
    return m_collapseTrigger;
}

template<typename InheritType>
void PanelTemplate<InheritType>::SetPadding(UiPadding rcPadding, bool bNeedDpiScale)
{
    if (bNeedDpiScale) {
        this->Dpi().ScalePadding(rcPadding);
    }
    m_contentPadding = rcPadding;
    ApplyContentPadding();
}

template<typename InheritType>
void PanelTemplate<InheritType>::ApplyContentPadding()
{
    //The title bar space is reserved through the top padding, so the layout engine automatically avoids the title
    //bar area when it arranges the child controls
    UiPadding rcPadding = m_contentPadding;
    rcPadding.top += m_nTitleBarHeight;
    //Explicitly call the base class implementation to write the padding storage (SetPadding is hidden by the
    //same-named method of this class)
    InheritType::SetPadding(rcPadding, false);
}

template<typename InheritType>
void PanelTemplate<InheritType>::ToggleCollapsed()
{
    SetCollapsed(!m_bCollapsed);
}

template<typename InheritType>
UiRect PanelTemplate<InheritType>::GetTitleBarRect() const
{
    UiRect rc = this->GetRect();
    rc.bottom = rc.top + m_nTitleBarHeight;
    return rc;
}

template<typename InheritType>
UiRect PanelTemplate<InheritType>::GetArrowRect() const
{
    UiRect rcTitle = GetTitleBarRect();
    //The arrow area is a square whose size equals the title bar height
    if (m_arrowAlign == ArrowAlign::kLeft) {
        rcTitle.right = rcTitle.left + m_nTitleBarHeight;
    }
    else {
        rcTitle.left = rcTitle.right - m_nTitleBarHeight;
    }
    return rcTitle;
}

template<typename InheritType>
void PanelTemplate<InheritType>::SetCollapseAnimMillSeconds(int32_t nMillSeconds)
{
    if (nMillSeconds < 0) {
        nMillSeconds = 0;
    }
    m_nCollapseAnimMs = nMillSeconds;
}

template<typename InheritType>
int32_t PanelTemplate<InheritType>::GetCollapseAnimMillSeconds() const
{
    return m_nCollapseAnimMs;
}

template<typename InheritType>
void PanelTemplate<InheritType>::SetGroup(const std::string& strGroupName)
{
    m_groupName = strGroupName;
}

template<typename InheritType>
std::string PanelTemplate<InheritType>::GetGroup() const
{
    return m_groupName.c_str();
}

template<typename InheritType>
std::string PanelTemplate<InheritType>::GetPanelGroupName() const
{
    return m_groupName.c_str();
}

template<typename InheritType>
bool PanelTemplate<InheritType>::IsPanelCollapsed() const
{
    return m_bCollapsible && m_bCollapsed;
}

template<typename InheritType>
void PanelTemplate<InheritType>::SetPanelCollapsed(bool bCollapsed, bool bFireEvent)
{
    //Internal accordion linkage path: it bypasses the cancellable event and runs directly
    if (m_bCollapsed == bCollapsed) {
        return;
    }
    if (!m_bCollapsible) {
        return;
    }
    ApplyCollapsed(bCollapsed, bFireEvent);
}

namespace PanelGroupDetail
{
    /** Walks the control tree recursively and collapses the other expanded panels of the given group
    */
    inline void CollapsePeersRecursive(Control* pControl, const IPanelGroup* pSelf,
                                       const std::string& strGroupName, bool bFireEvent)
    {
        if (pControl == nullptr) {
            return;
        }
        IPanelGroup* pPanel = dynamic_cast<IPanelGroup*>(pControl);
        if ((pPanel != nullptr) && (pPanel != pSelf)
            && (pPanel->GetPanelGroupName() == strGroupName)
            && !pPanel->IsPanelCollapsed()) {
            //Collapse the other panel: SetCollapsed(true) does not trigger the group linkage again, so there is
            //no recursion
            pPanel->SetPanelCollapsed(true, bFireEvent);
        }
        Box* pBox = dynamic_cast<Box*>(pControl);
        if (pBox != nullptr) {
            const size_t nCount = pBox->GetItemCount();
            for (size_t i = 0; i < nCount; ++i) {
                CollapsePeersRecursive(pBox->GetItemAt(i), pSelf, strGroupName, bFireEvent);
            }
        }
    }
} //namespace PanelGroupDetail

template<typename InheritType>
void PanelTemplate<InheritType>::CollapseGroupPeers(bool bFireEvent)
{
    if (m_groupName.empty()) {
        return;
    }
    Window* pWindow = this->GetWindow();
    if (pWindow == nullptr) {
        return;
    }
    Box* pRoot = pWindow->GetRoot();
    if (pRoot != nullptr) {
        PanelGroupDetail::CollapsePeersRecursive(pRoot, this, m_groupName.c_str(), bFireEvent);
    }
}

template<typename InheritType>
bool PanelTemplate<InheritType>::FireBeforeCollapseEvent(bool bCollapsed)
{
    //Dispatch the "about to collapse/expand" event: it is let through only when all listeners return true
    //(HandleEvent is bypassed because these two events have no framework-internal handling)
    EventArgs msg;
    msg.eventType = bCollapsed ? kEventPanelCollapsing : kEventPanelExpanding;
    msg.SetSender(this);
    Window* pWindow = this->GetWindow();
    if (pWindow != nullptr) {
        msg.ptMouse = pWindow->GetLastMousePos();
    }
    return this->FireAllEvents(msg);
}

template<typename InheritType>
void PanelTemplate<InheritType>::SetArrowStateImage(bool bExpanded, ControlStateType stateType, const std::string& strImage)
{
    this->SetStateImage(bExpanded ? kStateImagePanelArrowExpanded : kStateImagePanelArrowCollapsed,
                        stateType, strImage);
    this->Invalidate();
}

template<typename InheritType>
std::string PanelTemplate<InheritType>::GetArrowStateImage(bool bExpanded, ControlStateType stateType) const
{
    return this->GetStateImage(bExpanded ? kStateImagePanelArrowExpanded : kStateImagePanelArrowCollapsed,
                               stateType);
}

template<typename InheritType>
void PanelTemplate<InheritType>::SetTitleSlotName(const std::string& strSlotName)
{
    if (m_titleSlotName != strSlotName) {
        m_titleSlotName = strSlotName;
        //Drop the cache after the slot name changed, so that the control is looked up again at the next layout
        m_pTitleSlot = nullptr;
        this->ArrangeAncestor();
    }
}

template<typename InheritType>
std::string PanelTemplate<InheritType>::GetTitleSlotName() const
{
    return m_titleSlotName.c_str();
}

template<typename InheritType>
Control* PanelTemplate<InheritType>::GetTitleSlotControl()
{
    if (m_titleSlotName.empty()) {
        return nullptr;
    }
    //Weak reference cache: it becomes empty once the control is destroyed and the lookup by name is repeated then
    if (m_pTitleSlot == nullptr) {
        Control* pSlot = this->FindSubControl(m_titleSlotName.c_str());
        if (pSlot != nullptr) {
            m_pTitleSlot = ControlPtr(pSlot);
        }
    }
    return m_pTitleSlot.get();
}

template<typename InheritType>
void PanelTemplate<InheritType>::LayoutTitleSlot()
{
    Control* pSlot = GetTitleSlotControl();
    if (pSlot == nullptr) {
        return;
    }
    UiRect rcTitle = GetTitleBarRect();
    if (rcTitle.IsEmpty()) {
        return;
    }
    //The slot is vertically centered and stuck to the right edge; when the arrow is on the right side it has to
    //move to the left of the arrow
    const int32_t nMargin = this->Dpi().GetScaleInt(6);
    int32_t nRight = rcTitle.right - nMargin;
    if (m_bCollapsible && (m_arrowAlign == ArrowAlign::kRight)) {
        nRight = GetArrowRect().left;
    }

    UiSize szAvailable(rcTitle.Width(), rcTitle.Height());
    UiEstSize estSize = pSlot->EstimateSize(szAvailable);
    int32_t nWidth = estSize.cx.IsInt32() ? estSize.cx.GetInt32() : (rcTitle.Height() * 2);
    int32_t nHeight = estSize.cy.IsInt32() ? estSize.cy.GetInt32() : rcTitle.Height();
    nWidth = std::min(nWidth, nRight - rcTitle.left - nMargin);
    nHeight = std::min(nHeight, rcTitle.Height() - 2);
    if ((nWidth <= 0) || (nHeight <= 0)) {
        return;
    }

    UiRect rcSlot;
    rcSlot.right = nRight;
    rcSlot.left = nRight - nWidth;
    rcSlot.top = rcTitle.CenterY() - nHeight / 2;
    rcSlot.bottom = rcSlot.top + nHeight;
    pSlot->SetPos(rcSlot);
}

template<typename InheritType>
bool PanelTemplate<InheritType>::PaintArrowImage(IRender* pRender, const UiRect& rcArrow)
{
    //The arrow direction is switched only after the animation finished (the same behavior as the glyph arrow)
    const bool bExpanded = !(m_bCollapsed && (m_nAnimHeight < 0));
    const StateImageType imageType = bExpanded ? kStateImagePanelArrowExpanded
                                               : kStateImagePanelArrowCollapsed;
    if (!this->HasStateImage(imageType)) {
        return false;
    }
    //Current control state (Hovered while the title bar is hovered); a state without an image falls back to the
    //normal state
    ControlStateType stateType = this->GetState();
    if (!this->IsEnabled()) {
        stateType = kControlStateDisabled;
    }
    if (this->GetStateImage(imageType, stateType).empty() &&
        !this->GetStateImage(imageType, kControlStateNormal).empty()) {
        stateType = kControlStateNormal;
    }

    //The natural image size is used, centered inside the arrow hot zone and never exceeding it
    UiSize imageSize = this->GetStateImageSize(imageType, stateType);
    int32_t nW = imageSize.cx;
    int32_t nH = imageSize.cy;
    if ((nW <= 0) || (nH <= 0)) {
        nW = nH = rcArrow.Height() - this->Dpi().GetScaleInt(8);
    }
    const int32_t nMaxSize = rcArrow.Height() - this->Dpi().GetScaleInt(4);
    if ((nW > nMaxSize) || (nH > nMaxSize)) {
        const double dScale = static_cast<double>(nMaxSize) / std::max(nW, nH);
        nW = static_cast<int32_t>(nW * dScale);
        nH = static_cast<int32_t>(nH * dScale);
    }
    UiRect rcDest;
    rcDest.left = rcArrow.CenterX() - nW / 2;
    rcDest.right = rcDest.left + nW;
    rcDest.top = rcArrow.CenterY() - nH / 2;
    rcDest.bottom = rcDest.top + nH;
    //The target rectangle must be passed in explicitly: PaintStateImage ignores the outer rectangle and lays the
    //image out relatively to the whole control according to the align of the image itself, which would paint the
    //arrow on the left side of the panel instead of the right side of the title bar
    Image* pImage = this->GetStateImageData(imageType, stateType);
    if (pImage == nullptr) {
        return false;
    }
    return this->PaintImage(pRender, pImage, "", -1, nullptr, &rcDest, nullptr);
}

template<typename InheritType>
int32_t PanelTemplate<InheritType>::EstimateExpandedHeight(int32_t nFallbackHeight)
{
    //Measure the expanded state once, constrained by the current width and with plenty of available height
    UiRect rc = this->GetRect();
    int32_t nWidth = rc.Width();
    if (nWidth <= 0) {
        nWidth = INT32_MAX / 2;
    }
    UiSize szAvailable(nWidth, INT32_MAX / 2);
    UiEstSize estSize = BaseClass::EstimateSize(szAvailable);
    if (estSize.cy.IsInt32()) {
        int32_t nHeight = estSize.cy.GetInt32();
        if (nHeight >= m_nTitleBarHeight) {
            return nHeight;
        }
    }
    //When the measurement is not a definite pixel value (stretch and similar modes), the fallback height is used
    return nFallbackHeight > m_nTitleBarHeight ? nFallbackHeight : m_nTitleBarHeight;
}

template<typename InheritType>
void PanelTemplate<InheritType>::OnAnimHeightChanged(int32_t nHeight)
{
    m_nAnimHeight = nHeight;
    //Ask the parent to lay out again: EstimateSize returns the current animation height, so the panel shrinks and
    //expands smoothly
    this->ArrangeAncestor();
    this->Invalidate();
}

template<typename InheritType>
void PanelTemplate<InheritType>::OnCollapseAnimFinished(bool bFireEvent)
{
    m_nAnimHeight = -1;
    m_pAnimationPlayer.reset();
    //Restore the user minimum height after expanding (the collapsed state keeps it relaxed, otherwise the resting
    //height would still be clamped by min_height)
    if (!m_bCollapsed) {
        EndCollapseLayout();
    }
    //Do a final layout after the animation, so the natural measurement is back (fixed/auto/stretch all have no
    //side effects)
    this->ArrangeAncestor();
    this->Invalidate();
    if (bFireEvent) {
        this->SendEvent(m_bCollapsed ? kEventCollapse : kEventExpand);
    }
}

template<typename InheritType>
void PanelTemplate<InheritType>::StopCollapseAnim()
{
    if (m_pAnimationPlayer == nullptr) {
        return;
    }
    m_pAnimationPlayer->SetCompleteCallback(nullptr);
    m_pAnimationPlayer->SetPlayCallback(nullptr);
    m_pAnimationPlayer->Stop();
    m_pAnimationPlayer.reset();
    m_nAnimHeight = -1;
}

template<typename InheritType>
void PanelTemplate<InheritType>::BeginCollapseLayout()
{
    if (m_nSavedMinHeight < 0) {
        m_nSavedMinHeight = this->GetMinHeight();
        if (m_nSavedMinHeight > m_nTitleBarHeight) {
            this->SetMinHeight(m_nTitleBarHeight, false);
        }
    }
}

template<typename InheritType>
void PanelTemplate<InheritType>::EndCollapseLayout()
{
    if (m_nSavedMinHeight >= 0) {
        this->SetMinHeight(m_nSavedMinHeight, false);
        m_nSavedMinHeight = -1;
    }
}

template<typename InheritType>
void PanelTemplate<InheritType>::StartCollapseAnim(bool bCollapse, int32_t nFromHeight, bool bFireEvent)
{
    int32_t nToHeight = bCollapse ? m_nTitleBarHeight : EstimateExpandedHeight(m_nExpandedHeight);
    if (nToHeight == nFromHeight) {
        //The start and end heights are equal, so no animation is needed; go to the final state directly
        m_nAnimHeight = -1;
        if (!bCollapse) {
            EndCollapseLayout();
        }
        this->ArrangeAncestor();
        this->Invalidate();
        if (bFireEvent) {
            this->SendEvent(bCollapse ? kEventCollapse : kEventExpand);
        }
        return;
    }

    //Occupy the start height first, so the height does not jump before the first timer frame fires
    m_nAnimHeight = nFromHeight;
    this->ArrangeAncestor();

    AnimationPlayer* pAnimationPlayer = new AnimationPlayer;
    m_pAnimationPlayer.reset(pAnimationPlayer);
    pAnimationPlayer->SetAnimationType(AnimationType::kAnimationNone);
    pAnimationPlayer->SetTotalMillSeconds(m_nCollapseAnimMs);
    pAnimationPlayer->SetFrameIntervalMillSeconds(16); //About 60 frames per second
    //Collapsing uses ease-in (the faster the smaller it gets) and expanding uses ease-out (the slower the larger it
    //gets), which matches the usual feel of this kind of motion
    pAnimationPlayer->SetEasingFunctionType(bCollapse ? EaseInCubic : EaseOutCubic);
    pAnimationPlayer->SetStartValue(nFromHeight);
    pAnimationPlayer->SetEndValue(nToHeight);

    ControlPtrT<PanelTemplate> pPanel(this);
    AnimationPlayCallback playCallback = [pPanel](int32_t nNewValue) {
        if (pPanel == nullptr) {
            return;
        }
        pPanel->OnAnimHeightChanged(nNewValue);
    };
    pAnimationPlayer->SetPlayCallback(playCallback);

    AnimationCompleteCallback completeCallback = [pPanel, bFireEvent]() {
        if (pPanel == nullptr) {
            return;
        }
        pPanel->OnCollapseAnimFinished(bFireEvent);
    };
    pAnimationPlayer->SetCompleteCallback(completeCallback);
    pAnimationPlayer->Start();
}

template<typename InheritType>
UiEstSize PanelTemplate<InheritType>::EstimateSize(UiSize szAvailable)
{
    if (m_nAnimHeight >= 0) {
        //Animation in progress: the measured height is the current animation height, which drives the parent layout
        //in real time
        UiEstSize estSize = BaseClass::EstimateSize(szAvailable);
        estSize.cy.SetInt32(m_nAnimHeight);
        return estSize;
    }
    UiEstSize estSize = BaseClass::EstimateSize(szAvailable);
    if (m_bCollapsible && m_bCollapsed) {
        //While collapsed the height is fixed to the title bar height and the layout system shrinks the control
        //naturally; the own fixed/auto/stretch height mode is not modified, so measurement recovers when expanded
        estSize.cy.SetInt32(m_nTitleBarHeight);
    }
    return estSize;
}

template<typename InheritType>
void PanelTemplate<InheritType>::SetPos(UiRect rc)
{
    if (m_nAnimHeight >= 0) {
        //Animation in progress: present the current animation height
        rc.bottom = rc.top + m_nAnimHeight;
    }
    else if (m_bCollapsible && m_bCollapsed) {
        //While collapsed the panel only occupies the title bar height, no matter how much height the parent allocates
        rc.bottom = rc.top + m_nTitleBarHeight;
    }
    BaseClass::SetPos(rc);

    //The title bar slot follows (float child controls do not take part in the content layout, the Panel places
    //them on the right side of the title bar by itself)
    LayoutTitleSlot();

    //Remember the actual height of the last fully expanded layout, used as the start point of a later collapse
    //animation and as the fallback target when expanding
    if ((m_nAnimHeight < 0) && !(m_bCollapsible && m_bCollapsed)) {
        int32_t nHeight = rc.Height();
        if (nHeight > m_nTitleBarHeight) {
            m_nExpandedHeight = nHeight;
        }
    }
}

template<typename InheritType>
void PanelTemplate<InheritType>::PaintChild(IRender* pRender, const UiRect& rcPaint)
{
    if ((m_nAnimHeight < 0) && m_bCollapsible && m_bCollapsed) {
        //Collapsed resting state: no content child control is painted, but the title bar slot (and its child
        //controls) is still shown
        Control* pSlot = GetTitleSlotControl();
        if ((pSlot != nullptr) && pSlot->IsVisible()) {
            pSlot->AlphaPaint(pRender, rcPaint);
        }
        return;
    }
    if (m_nAnimHeight >= 0) {
        //Animation in progress: the child controls keep their full layout position and are clipped to the current
        //panel rectangle, which produces the height collapsing/expanding effect
        UiRect rcClip = this->GetRect();
        float fRoundWidth = 0;
        float fRoundHeight = 0;
        std::unique_ptr<AutoClip> pClip;
        if (this->GetBorderRound(fRoundWidth, fRoundHeight)) {
            pClip = std::make_unique<AutoClip>(pRender, rcClip, fRoundWidth, fRoundHeight, true);
        }
        else {
            pClip = this->CreateRectClip(pRender, rcClip, true);
        }
        BaseClass::PaintChild(pRender, rcPaint);
        return;
    }
    BaseClass::PaintChild(pRender, rcPaint);
}

template<typename InheritType>
Control* PanelTemplate<InheritType>::FindControl(FINDCONTROLPROC Proc, void* pProcData,
                                                 uint32_t uFlags, const UiPoint& ptMouse,
                                                 const UiPoint& scrollPos)
{
    if (m_bCollapsible && m_bCollapsed && ((uFlags & UIFIND_HITTEST) != 0)) {
        //Hit testing while collapsed: the content child controls do not receive mouse events, but the title bar
        //slot (and the buttons inside it) stays usable
        Control* pSlot = GetTitleSlotControl();
        if ((pSlot != nullptr) && pSlot->IsVisible() && pSlot->GetRect().ContainsPt(ptMouse)) {
            Control* pHit = pSlot->FindControl(Proc, pProcData, uFlags, ptMouse, scrollPos);
            if (pHit != nullptr) {
                return pHit;
            }
        }
        //Only the panel itself matches in the remaining area; lookups by name/condition (not HITTEST) are not
        //affected
        return Control::FindControl(Proc, pProcData, uFlags, ptMouse, scrollPos);
    }
    return BaseClass::FindControl(Proc, pProcData, uFlags, ptMouse, scrollPos);
}

template<typename InheritType>
bool PanelTemplate<InheritType>::ButtonUp(const EventArgs& msg)
{
    bool bRet = BaseClass::ButtonUp(msg);
    if (msg.IsSenderExpired()) {
        return false;
    }
    if (m_bCollapsible && this->IsEnabled()) {
        //When the click comes from a child control (an interactive control on the title bar and so on), the
        //collapsed state is not toggled
        Control* pSender = msg.GetSender();
        if (pSender == static_cast<Control*>(this)) {
            //msg.ptMouse is in window client coordinates, while GetRect() returns virtual content coordinates when
            //the control is inside a scroll container; the scroll offset has to be added before the hit test (the
            //same handling as Slider), otherwise the title bar cannot be clicked once the content is scrolled
            UiPoint ptMouse(msg.ptMouse);
            ptMouse.Offset(this->GetScrollOffsetInScrollBox());
            bool bHit = false;
            UiRect rcTitle = GetTitleBarRect();
            if (rcTitle.ContainsPt(ptMouse)) {
                if (m_collapseTrigger == CollapseTrigger::kArrow) {
                    bHit = GetArrowRect().ContainsPt(ptMouse);
                }
                else {
                    //The title bar slot area does not toggle collapsing (interactive controls are placed in the slot)
                    Control* pSlot = GetTitleSlotControl();
                    bHit = !((pSlot != nullptr) && pSlot->IsVisible()
                             && pSlot->GetRect().ContainsPt(ptMouse));
                }
            }
            if (bHit) {
                ToggleCollapsed();
            }
        }
    }
    return bRet;
}

template<typename InheritType>
void PanelTemplate<InheritType>::PaintText(IRender* pRender)
{
    BaseClass::PaintText(pRender);
    if (pRender == nullptr) {
        return;
    }

    UiRect rcTitle = GetTitleBarRect();
    if (rcTitle.IsEmpty()) {
        return;
    }

    //Paint the title bar background: when the panel has rounded corners, the clip is intersected with the rounded
    //region of the panel, so the two top corners of the title bar follow the radius while the bottom stays square
    float fRoundWidth = 0;
    float fRoundHeight = 0;
    bool bHasRound = this->GetBorderRound(fRoundWidth, fRoundHeight);
    std::unique_ptr<AutoClip> pRoundClip;
    if (!m_titleBkColor.empty()) {
        if (bHasRound) {
            //Note: the clip has to be intersected with the rounded region (SetRoundClip with bIntersect = true);
            //using the opposite mode would cut out the whole title background/text/arrow
            pRoundClip = std::make_unique<AutoClip>(pRender, this->GetRect(), fRoundWidth, fRoundHeight, true);
        }
        UiColor bkColor = this->GetUiColor(m_titleBkColor.c_str());
        if (!bkColor.IsEmpty()) {
            pRender->FillRect(UiRectF::MakeFromRect(rcTitle), bkColor);
        }
    }

    //Title text color
    UiColor textColor;
    if (!m_titleTextColor.empty()) {
        textColor = this->GetUiColor(m_titleTextColor.c_str());
    }
    if (textColor.IsEmpty()) {
        textColor = this->GetUiColor("text_default");
    }

    //Title font
    std::string fontId = m_titleFontId.c_str();
    if (fontId.empty()) {
        fontId = "system_bold_14";
    }
    IFont* pFont = this->GetIFontById(fontId);

    //Paint the collapse/expand arrow (shown on the right or left side of the title bar when the panel is
    //collapsible, decided by arrow_align): the state image is preferred, the glyph is used when none is configured
    int32_t nArrowWidth = 0;
    if (m_bCollapsible) {
        nArrowWidth = m_nTitleBarHeight; //The arrow area width equals the title bar height, which keeps the hot zone square
        UiRect rcArrow = GetArrowRect();
        if (!PaintArrowImage(pRender, rcArrow)) {
            //When no arrow image is configured, a vector triangle is painted: the U+25B6 glyph may be rendered as an
            //emoji style blue/white triangle on Windows
            IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
            ASSERT(pRenderFactory != nullptr);
            if (pRenderFactory != nullptr) {
                //The accent color is used on hover, which matches the behavior of an arrow with hover images (the
                //text color stays unchanged)
                UiColor arrowColor = textColor;
                if (this->GetState() == kControlStateHot) {
                    UiColor accentColor = this->GetUiColor("color_accent");
                    if (!accentColor.IsEmpty()) {
                        arrowColor = accentColor;
                    }
                }
                const float fPad = rcArrow.Width() * 0.32f; //Gap between the triangle and the hot zone border
                const float fL = (float)rcArrow.left + fPad;
                const float fT = (float)rcArrow.top + fPad;
                const float fR = (float)rcArrow.right - fPad;
                const float fB = (float)rcArrow.bottom - fPad;
                const float fCx = (fL + fR) / 2.0f;
                const float fCy = (fT + fB) / 2.0f;
                UiPointF pts[3];
                if (m_bCollapsed && (m_nAnimHeight < 0)) {
                    pts[0] = UiPointF(fL, fT); //Triangle pointing right
                    pts[1] = UiPointF(fL, fB);
                    pts[2] = UiPointF(fR, fCy);
                }
                else {
                    pts[0] = UiPointF(fL, fT); //Triangle pointing down
                    pts[1] = UiPointF(fR, fT);
                    pts[2] = UiPointF(fCx, fB);
                }
                std::unique_ptr<IPath> pPath(pRenderFactory->CreatePath());
                pPath->AddPolygon(pts, 3);
                pPath->Close();
                std::unique_ptr<IBrush> pBrush(pRenderFactory->CreateBrush(arrowColor));
                pRender->FillPath(pPath.get(), pBrush.get());
            }
        }
    }

    //Paint the title text (vertically centered, horizontally aligned according to title_text_align, avoiding both
    //the slot and the arrow area)
    if (!m_title.empty()) {
        const int32_t nTextPadding = this->Dpi().GetScaleInt(8);
        UiRect rcText = rcTitle;
        rcText.left += nTextPadding;
        rcText.right -= nTextPadding;
        if (nArrowWidth > 0) {
            //The whole side that holds the arrow is left free, so the text does not overlap the arrow
            if (m_arrowAlign == ArrowAlign::kLeft) {
                rcText.left += nArrowWidth;
            }
            else {
                rcText.right -= nArrowWidth;
            }
        }
        Control* pSlot = GetTitleSlotControl();
        if ((pSlot != nullptr) && pSlot->IsVisible()) {
            UiRect rcSlot = pSlot->GetRect();
            if (!rcSlot.IsEmpty() && (rcSlot.left > rcTitle.left) && (rcSlot.left < rcText.right)) {
                rcText.right = rcSlot.left - this->Dpi().GetScaleInt(6);
            }
        }
        rcText.Validate();

        DrawStringParam drawParam;
        drawParam.textRect = rcText;
        drawParam.dwTextColor = textColor;
        drawParam.pFont = pFont;
        drawParam.uFormat = m_uTitleTextHAlign | TEXT_VCENTER | TEXT_SINGLELINE | TEXT_END_ELLIPSIS;
        pRender->DrawString(m_title.c_str(), drawParam);
    }
}

template<typename InheritType>
void PanelTemplate<InheritType>::ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale)
{
    if (!this->Dpi().CheckDisplayScaleFactor(nNewDpiScale)) {
        return;
    }
    //Stop the collapse animation during a DPI change and go to the logical state directly, so the animation height
    //is not interpolated with the old scale factor
    StopCollapseAnim();

    //Scale the title bar height and the user content padding first
    m_nTitleBarHeight = this->Dpi().GetScaleInt(m_nTitleBarHeight, nOldDpiScale);
    m_contentPadding = this->Dpi().GetScalePadding(m_contentPadding, nOldDpiScale);
    m_nExpandedHeight = this->Dpi().GetScaleInt(m_nExpandedHeight, nOldDpiScale);
    if (m_nSavedMinHeight >= 0) {
        //The collapsed layout is active: scale the saved minimum height as well and relax it again with the new
        //title bar height
        m_nSavedMinHeight = this->Dpi().GetScaleInt(m_nSavedMinHeight, nOldDpiScale);
        if (m_nSavedMinHeight > m_nTitleBarHeight) {
            this->SetMinHeight(m_nTitleBarHeight, false);
        }
    }

    //The base class writes the old merged padding into its storage (SetPadding is not virtual and cannot be
    //intercepted); the padding is merged again with the scaled title bar height and content padding afterwards
    BaseClass::ChangeDpiScale(nOldDpiScale, nNewDpiScale);
    ApplyContentPadding();
}

/** Panel control / horizontal panel / vertical panel
*/
typedef PanelTemplate<Box>  Panel;
typedef PanelTemplate<HBox> PanelHBox;
typedef PanelTemplate<VBox> PanelVBox;

}

#endif // UI_BOX_PANEL_H_
