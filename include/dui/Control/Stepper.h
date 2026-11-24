#ifndef UI_CONTROL_STEPPER_H_
#define UI_CONTROL_STEPPER_H_

#include "dui/Core/Control.h"
#include "dui/Image/StateImage.h"
#include <vector>

namespace ui {

/** Stepper step status
*/
enum class StepStatus
{
    kPending = 0,   // Pending (gray, shows the step number)
    kActive = 1,    // Currently active (accent color, shows the step number)
    kCompleted = 2, // Completed (success color, shows a check mark)
    kDisabled = 3   // Disabled (dim gray, cannot be jumped to)
};

/** Stepper wizard step indicator control (inherits from Control, custom painted)
 *
 *  Used for installation/registration/configuration wizards, approval flows, order progress and other "process state machine" scenarios;
 *  its meaning differs from Timeline (the time axis): Steps is about "which step we are at", not a time sequence.
 *
 *  The status is derived from "the current index + the disabled set": the steps before the current index are completed, the current step is active,
 *  and the following ones are pending; a step that is explicitly disabled is shown in the disabled state with priority.
 *
 *  XML attributes:
 *  - step_title / step_title_id: step titles, multiple items separated by '|' ('_id' is the multi-language ID, refreshed automatically on a language change)
 *  - step_desc / step_desc_id: step descriptions, multiple items separated by '|'
 *  - current / current_index: index of the current step, defaults to 0
 *  - direction / orientation: arrangement direction, horizontal (default) / vertical
 *  - click_to_jump: whether clicking a step jumps to it, defaults to true
 *  - disabled_steps: list of disabled step indexes, separated by '|' or ',', e.g. "2|3"
 *  - node_size: node diameter (pixels, DPI scaled), defaults to 24
 *  - node_color_pending / node_color_active / node_color_completed / node_color_disabled: node color of each status
 *  - connector_color: connector line color of the unfinished segments; connector_active_color: connector line color of the finished segments
 *  - title_color_active / title_color_completed / title_color_pending: title color of each status
 *  - desc_color: description text color
 *  - title_font / desc_font: font ID of the title / description
 *  - node_hovered_image (alias node_hot_image): image of the hovered node state, optional
 *  - node_pressed_image (alias node_pushed_image): image of the pushed node state, optional; falls back to the hovered image when it is not set
 *
 *  Note: the hovered/pushed state image is drawn below the node circle, and only for "interactive steps" (the control is enabled, click_to_jump is on and the step is not disabled)
 *        while the mouse hovers/presses; it is used to render feedback such as an accent-colored glow/stroke, and does not affect the node color, the step number or the check mark.
 */
class DUI_API Stepper : public Control
{
    typedef Control BaseClass;

public:
    /** Step data
    */
    struct Step
    {
        std::string title;       // Title
        std::string description; // Description (may be empty)
    };

    /** Current step change data (used by the kEventValueChanged event, pEventData points to this structure)
    */
    struct StepValue
    {
        size_t currentIndex = 0; // New index of the current step
        size_t oldIndex = 0;     // Index of the current step before the change
    };

public:
    explicit Stepper(Window* pWindow);
    Stepper(const Stepper& r) = delete;
    Stepper& operator=(const Stepper& r) = delete;
    ~Stepper() override;

    /** Get the control type
    */
    std::string GetType() const override;

    /** Set an XML attribute
    */
    void SetAttribute(const std::string& strName, const std::string& strValue) override;

    /** Paint the step indicator content
    */
    void PaintStateImages(IRender* pRender) override;

    /** Refresh the multi-language data bindings when the language changes
    */
    void OnLanguageChanged() override;

    /** Left mouse button down: jump on a step click
    */
    bool ButtonDown(const EventArgs& msg) override;

    /** Left mouse button up: end the pushed state
    */
    bool ButtonUp(const EventArgs& msg) override;

    /** Mouse move: update the hovered step (used to show the hover state)
    */
    bool MouseMove(const EventArgs& msg) override;

    /** Mouse leaves the control: clear the hovered/pushed state
    */
    bool MouseLeave(const EventArgs& msg) override;

    /** Clear the image cache
    */
    void ClearImageCache() override;

public:
    /** Append a step
    */
    void AddStep(const Step& step);

    /** Append a step (convenience overload)
    */
    void AddStep(const std::string& title, const std::string& description = std::string());

    /** Insert a step at the specified position
    */
    bool InsertStep(size_t index, const Step& step);

    /** Remove the specified step
    */
    bool RemoveStepAt(size_t index);

    /** Remove all steps
    */
    void ClearSteps();

    /** Set all steps at once (replaces everything)
    */
    void SetSteps(const std::vector<Step>& steps);

    /** Get the step count
    */
    size_t GetStepCount() const { return m_steps.size(); }

    /** Get the specified step
    */
    const Step* GetStepAt(size_t index) const;

    /** Get all steps
    */
    const std::vector<Step>& GetSteps() const { return m_steps; }

    /** Get the index of the current step (already clamped to the valid range)
    */
    size_t GetCurrentIndex() const;

    /** Set the index of the current step (clamped automatically; fires kEventValueChanged when the index changes)
    */
    void SetCurrentIndex(size_t nIndex);

    /** Set the index of the current step (does not fire an event, used for internal synchronization)
    */
    void SetCurrentIndexNoEvent(size_t nIndex);

    /** Advance to the next step (returns false when out of range; fires the event)
    */
    bool Next();

    /** Advance to the next step (does not fire the event)
    */
    bool NextNoEvent();

    /** Go back to the previous step (returns false when out of range; fires the event)
    */
    bool Back();

    /** Go back to the previous step (does not fire the event)
    */
    bool BackNoEvent();

    /** Jump to the specified step (returns false when the target is disabled or the index is out of range; fires the event)
    */
    bool GoTo(size_t nIndex);

    /** Jump to the specified step (does not fire the event)
    */
    bool GoToNoEvent(size_t nIndex);

    /** Set whether the specified step is disabled
    */
    void SetStepDisabled(size_t index, bool bDisabled);

    /** Get whether the specified step is disabled
    */
    bool IsStepDisabled(size_t index) const;

    /** Set the disabled steps at once (replaces everything)
    */
    void SetDisabledSteps(const std::vector<size_t>& disabledSteps);

    /** Get the list of disabled steps
    */
    const std::vector<size_t>& GetDisabledSteps() const { return m_disabledSteps; }

    /** Get the derived status of the specified step
    */
    StepStatus GetStepStatus(size_t index) const;

    /** Set the arrangement direction: true = horizontal (default), false = vertical
    */
    void SetHorizontal(bool bHorizontal);

    /** Get whether the steps are arranged horizontally
    */
    bool IsHorizontal() const { return m_bHorizontal; }

    /** Set the node diameter (pixels, DPI scaling is applied automatically)
    */
    void SetNodeSize(int32_t nSize);

    /** Get the node diameter (the raw value before scaling)
    */
    int32_t GetNodeSize() const { return m_nNodeSize; }

    /** Set whether clicking a step jumps to it
    */
    void SetClickToJump(bool bEnable);

    /** Get whether clicking a step jumps to it
    */
    bool IsClickToJump() const { return m_bClickToJump; }

    /** Set the node color of the specified status (semantic color name or color value)
    */
    void SetStateColor(StepStatus status, const std::string& strColor);

    /** Get the node color of the specified status
    */
    const std::string& GetStateColor(StepStatus status) const;

    /** Set the title color of the specified status (semantic color name or color value)
    */
    void SetTitleColor(StepStatus status, const std::string& strColor);

    /** Get the title color of the specified status
    */
    const std::string& GetTitleColor(StepStatus status) const;

    /** Set the connector line color of the unfinished segments
    */
    void SetConnectorColor(const std::string& strColor);

    /** Get the connector line color of the unfinished segments
    */
    const std::string& GetConnectorColor() const { return m_strConnectorColor; }

    /** Set the connector line color of the finished segments
    */
    void SetConnectorActiveColor(const std::string& strColor);

    /** Get the connector line color of the finished segments
    */
    const std::string& GetConnectorActiveColor() const { return m_strConnectorActiveColor; }

    /** Set the description text color
    */
    void SetDescColor(const std::string& strColor);

    /** Get the description text color
    */
    const std::string& GetDescColor() const { return m_strDescColor; }

    /** Set the title font ID
    */
    void SetTitleFont(const std::string& strFontId);

    /** Get the title font ID
    */
    const std::string& GetTitleFont() const { return m_strTitleFont; }

    /** Set the description font ID
    */
    void SetDescFont(const std::string& strFontId);

    /** Get the description font ID
    */
    const std::string& GetDescFont() const { return m_strDescFont; }

    /** Listen for the current step change event
    * @param [in] callback The callback invoked after the current step changed
    * @param [in] callbackID The ID of the callback (used to remove the callback function)
    * Parameter description:
    *   wParam: 2 (step mode marker)
    *   lParam: 0
    *   pEventData: points to StepValue (which holds currentIndex / oldIndex);
    *               it is valid only while this callback is dispatched, do not keep the pointer
    */
    void AttachStepChanged(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventValueChanged, callback, callbackID); }

    /** Set the node state image (optional, used for the visual feedback of the hovered/pushed state)
    * @param [in] stateType The state type (valid values: kControlStateHot / kControlStatePushed)
    * @param [in] strImage The image attribute string, for example "file='public/stepper/stepper_node_hovered.svg'"
    */
    void SetNodeStateImage(ControlStateType stateType, const std::string& strImage);

    /** Get the node state image attribute string
    * @param [in] stateType The state type (kControlStateHot / kControlStatePushed)
    */
    std::string GetNodeStateImage(ControlStateType stateType) const;

private:
    /** Parse an XML data binding field (separated by '|')
    * @param [in] strValue The field value string
    * @param [in] nField The field number: 0 = title, 1 = description
    * @param [in] bTranslate Whether it is multi-language ID mode (each segment is fetched by STRID from the language file)
    */
    void ParseItemsField(const std::string& strValue, int32_t nField, bool bTranslate = false);

    /** Parse the list of disabled step indexes
    */
    void ParseDisabledSteps(const std::string& strValue);

    /** Fire the current step change event (not fired when m_bMuteStepEvent is true)
    * @param [in] nOldIndex The index before the change
    */
    void EmitStepChanged(size_t nOldIndex);

    /** Clamp the current index to [0, count-1] (0 when the list is empty)
    */
    void ClampCurrentIndex();

    /** Compute the slot rectangle of the specified step (control-relative coordinates)
    */
    UiRect CalcStepSlotRect(const UiRect& rc, size_t nCount, size_t index) const;

    /** Hit test: return the index of the step under the mouse, or -1 when nothing is hit
    */
    int32_t HitTestStep(const UiRect& rc, size_t nCount, const UiPoint& ptMouse) const;

    /** Paint the step indicator in the horizontal direction
    */
    void PaintHorizontal(IRender* pRender, const UiRect& rc);

    /** Paint the step indicator in the vertical direction
    */
    void PaintVertical(IRender* pRender, const UiRect& rc);

    /** Paint a single node (circle + step number/check mark)
    */
    void PaintNode(IRender* pRender, const UiPoint& ptCenter, int32_t nRadius, StepStatus status,
                   size_t index, bool bDimmed);

    /** Paint text (wraps DrawStringParam)
    */
    void DrawStepperText(IRender* pRender, const std::string& strText, const UiRect& rc, uint32_t uFormat,
                         UiColor textColor, const std::string& strFontId);

    /** Measure the text height
    */
    int32_t MeasureTextHeight(IRender* pRender, const std::string& strText, int32_t nWidth, const std::string& strFontId);

    /** Paint the hovered/pushed state icon of a node (drawn below the node circle; not painted when it is not configured or the step is not interactive)
    * @param [in] ptCenter The node center (control coordinates)
    * @param [in] nRadius The node radius (already DPI scaled)
    * @param [in] status The derived status of the step
    * @param [in] index The step index
    */
    void PaintNodeStateIcon(IRender* pRender, const UiPoint& ptCenter, int32_t nRadius,
                            StepStatus status, size_t index);

private:
    /** Step data
    */
    std::vector<Step> m_steps;

    /** List of disabled step indexes
    */
    std::vector<size_t> m_disabledSteps;

    /** Index of the current step
    */
    size_t m_nCurrentIndex;

    /** Whether the steps are arranged horizontally (false = vertical)
    */
    bool m_bHorizontal;

    /** Node diameter (the raw value before DPI scaling)
    */
    int32_t m_nNodeSize;

    /** Whether clicking a step jumps to it
    */
    bool m_bClickToJump;

    /** Whether the current step change event is suppressed (true during NoEvent)
    */
    bool m_bMuteStepEvent;

    /** Node color of each status (semantic color names, indexed by StepStatus)
    */
    std::string m_stateColors[4];

    /** Title color of each status (semantic color names, indexed by StepStatus)
    */
    std::string m_titleColors[4];

    /** Connector line color of the unfinished segments
    */
    std::string m_strConnectorColor;

    /** Connector line color of the finished segments
    */
    std::string m_strConnectorActiveColor;

    /** Description text color
    */
    std::string m_strDescColor;

    /** Title font ID
    */
    std::string m_strTitleFont;

    /** Description font ID
    */
    std::string m_strDescFont;

    /** Raw multi-language ID strings of the data bindings (step_title_id / step_desc_id, re-resolved on a language change)
    */
    std::string m_strItemTitleIds;
    std::string m_strItemDescIds;

    /** Node state images (hovered state kControlStateHot / pushed state kControlStatePushed, optional)
    */
    StateImage m_nodeStateImage;

    /** Index of the hovered step (-1 = none)
    */
    int32_t m_nHoverStep;

    /** Index of the pushed step (-1 = none)
    */
    int32_t m_nPressedStep;
};

} // namespace ui

#endif // UI_CONTROL_STEPPER_H_
