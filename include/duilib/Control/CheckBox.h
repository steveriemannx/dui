#ifndef UI_CONTROL_CHECKBOX_H_
#define UI_CONTROL_CHECKBOX_H_

#include "duilib/Control/Button.h"
#include "duilib/Core/StateColorMap2.h"
#include "duilib/Utils/AttributeUtil.h"
#include "duilib/Core/StateColorMap.h"
#include "duilib/Animation/AnimationManager.h"
#include "duilib/Animation/AnimationPlayer.h"

namespace ui {


/** Implementation of the CheckBox functionality; the key points are as follows:
 *  1. CheckBox has two states: m_bSelected (selection state) and m_bChecked (check state)
 *  2. Selection state: clicking anywhere on the CheckBox will trigger a change of m_bSelected (selection state)
 *  3. Check state: only when the mouse clicks on the CheckBox icon will a change of m_bChecked (check state) be triggered; clicking other areas leaves the check state unchanged
 *  4. A change of the selection state (m_bSelected), when the IsAutoCheckSelect() function returns true, will cause a change of the check state (m_bChecked), keeping the states consistent (Select -> Check)
 *  5. A change of the check state (m_bChecked), when the IsAutoSelectCheck() function returns true, will cause a change of the selection state (m_bSelected), keeping the states consistent (Check - Select)
 *  6. By default, the check state is disabled; when the SupportCheckMode() function returns true, the check state is enabled
 *  7. m_bSelected (selection state) and m_bChecked (check state) share image resources and font-related resources
 *  8. m_bSelected (selection state) supports three states: selected / part selected / unselected
 *  9. m_bChecked (check state) supports three states: checked / part checked / unchecked
 * 10. CheckBox inherits from the Button class; it does not trigger the button's kEventClick event, but it does execute the Activate virtual function
 */

template<typename InheritType = Control>
class CheckBoxTemplate : public ButtonTemplate<InheritType>
{
    typedef ButtonTemplate<InheritType> BaseClass;
public:
    explicit CheckBoxTemplate(Window* pWindow);
    CheckBoxTemplate(const CheckBoxTemplate& r) = delete;
    CheckBoxTemplate& operator=(const CheckBoxTemplate& r) = delete;
    virtual ~CheckBoxTemplate() override;

    /// Override the parent class methods to provide personalized features; please refer to the parent class declarations
    virtual DString GetType() const override;
    virtual void Activate(const EventArgs* pMsg) override;
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;
    virtual void PaintStateColors(IRender* pRender) override;
    virtual void PaintStateImages(IRender* pRender) override;
    virtual void PaintText(IRender* pRender) override;
    virtual bool HasHotState() override;
    virtual DString GetBorderColor(ControlStateType stateType) const override;

    /** DPI change: update the control size and layout
    * @param [in] nOldDpiScale The old DPI scale percentage
    * @param [in] nNewDpiScale The new DPI scale percentage, consistent with the value of Dpi().GetScale()
    */
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;

public:
    /** Get the image used when selected
     * @param [in] stateType The state for which to get the image; refer to the ControlStateType enum
     * @return Returns the image path and attributes
     */
    DString GetSelectedStateImage(ControlStateType stateType) const;

    /** Set the image used when selected
     * @param [in] stateType The state for which to set the image
     * @param [in] strImage The image path and attributes
     */
    void SetSelectedStateImage(ControlStateType stateType, const DString& strImage);

    /** Get the foreground image used when selected
     * @param[in] stateType The state for which to get the foreground image
     */
    DString GetSelectedForeStateImage(ControlStateType stateType) const;

    /** Set the foreground image used when selected
     * @param[in] stateType The state for which to set the foreground image
     * @param[in] strImage The image location
     */
    void SetSelectedForeStateImage(ControlStateType stateType, const DString& strImage);

    /** Get the text color used when selected
     */
    DString GetSelectedTextColor() const;

    /** Set the text color used when selected
     * @param[in] dwTextColor The color string to set; this color must exist in global.xml
     */
    void SetSelectedTextColor(const DString& dwTextColor);

    /** Get the text color of the specified state used when selected
     * @param[in] stateType The state for which to get the color
     * @return Returns the color string, defined in global.xml
     */
    DString GetSelectedStateTextColor(ControlStateType stateType) const;

    /** Set the text color of the specified state used when selected
     * @param[in] stateType The state for which to set the color
     * @param[in] stateColor The color to set
     */
    void SetSelectedStateTextColor(ControlStateType stateType, const DString& dwTextColor);

    /** Get the control color used when selected
     * @param [in] stateType The state for which to get the color
     * @return Returns the color string, defined in global.xml
     */
    DString GetSelectStateColor(ControlStateType stateType) const;

    /** Get the color rectangle margin of the control color used when selected
     * @param [in] stateType The state for which to get the color value; refer to the ControlStateType enum
     * @return The color rectangle margin set for the specified state (already DPI scaled)
     */
    UiMargin GetSelectStateColorMargin(ControlStateType stateType) const;

    /** Get the color rectangle corner radius of the control color used when selected
     * @param [in] stateType The state for which to get the color value; refer to the ControlStateType enum
     * @return The color rectangle corner radius set for the specified state (already DPI scaled)
     */
    UiSize GetSelectStateColorRound(ControlStateType stateType) const;

    /** Set the control color used when selected
     * @param [in] stateType The state for which to set the color
     * @param [in] stateColor The color to set
     * @param [in] colorMargin The color rectangle margin to set; if not set, the color rectangle coincides with the control rectangle
     * @param [in] colorRound The color rectangle corner radius to set; if not set, the color rectangle follows the shape of the control rectangle
     * @param [in] bNeedDpiScale Whether DPI adaptation is needed
     */
    void SetSelectedStateColor(ControlStateType stateType, const DString& stateColor);
    void SetSelectedStateColorMargin(ControlStateType stateType, UiMargin colorMargin, bool bNeedDpiScale);
    void SetSelectedStateColorRound(ControlStateType stateType, UiSize colorRound, bool bNeedDpiScale);

public:
    /** Get the image used when part selected
     * @param [in] stateType The state for which to get the image; refer to the ControlStateType enum
     * @return Returns the image path and attributes
     */
    DString GetPartSelectedStateImage(ControlStateType stateType);

    /** Set the image used when part selected
     * @param [in] stateType The state for which to set the image
     * @param [in] strImage The image path and attributes
     */
    void SetPartSelectedStateImage(ControlStateType stateType, const DString& strImage);

    /** Get the foreground image used when part selected
     * @param[in] stateType The state for which to get the foreground image
     */
    DString GetPartSelectedForeStateImage(ControlStateType stateType);

    /** Set the foreground image used when part selected
     * @param[in] stateType The state for which to set the foreground image
     * @param[in] strImage The image location
     */
    void SetPartSelectedForeStateImage(ControlStateType stateType, const DString& strImage);

public:
    /** Judge whether the current state is the selection state
     * @return Returns true for the selection state, otherwise false
     */
    bool IsSelected() const { return m_bSelected; }

    /** Set the selection state, but do not trigger events or update the UI
    * @param [in] bSelected true is the selection state, false is the non-selection state
    */
    virtual void SetSelected(bool bSelected);

    /** Set whether the control is in the selection state
     * @param [in] bSelected true is the selection state, false is the deselection state
     * @param [in] bTriggerEvent Whether to send the state change event; true sends it, otherwise false. The default is false
     * @param [in] vkFlag The key flag; refer to the definition of enum VKFlag for the value range
     */
    virtual void Selected(bool bSelected, bool bTriggerEvent = false, uint64_t vkFlag = 0);

    /** Set the part-selected flag (supports the three-state selection flag: all selected / part selected / unselected)
    * @return Returns true if the state changed, otherwise false
    */
    bool SetPartSelected(bool bPartSelected);

    /** Whether the current state is part selected
    */
    bool IsPartSelected() const { return m_bPartSelected; }

    /** Set whether a change of the selection state is automatically synced to the check state, keeping the check state consistent with the selection state (Select -> Check)
    * @param [in] bAutoCheckSelect If true, the Check state stays in sync with the Select state
    */
    void SetAutoCheckSelect(bool bAutoCheckSelect) { m_bAutoCheckSelected = bAutoCheckSelect; }

    /** Get whether a change of the selection state is automatically synced to the check state (Select -> Check)
    */
    bool IsAutoCheckSelect() const { return m_bAutoCheckSelected; }

    /** Listen to the event when selected
     * @param [in] callback The callback function triggered when selected
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachSelect(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventSelect, callback, callbackID); }

    /** Listen to the event when deselected
     * @param [in] callback The callback function triggered when deselected
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachUnSelect(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventUnSelect, callback, callbackID); }

public:
    /// (Functions for the two features: three-state selection [all selected, part selected, unselected] / check mode)

    /** Whether it is in the check state; meaningful only when the SupportCheckMode() function returns true
    */
    bool IsChecked() const { return m_bChecked; }

    /** Set the Check state
    * @param [in] bChecked Whether to set the Check state
    * @param [in] bTriggerEvent Whether to send the state change event; true sends it, otherwise false. The default is false
    */
    void SetChecked(bool bChecked, bool bTriggerEvent = false);

    /** Set the part-checked flag (supports the three-state check flag: all checked / part checked / unchecked)
    * @return Returns true if the state changed, otherwise false
    */
    bool SetPartChecked(bool bPartChecked);

    /** Whether the current state is part checked
    */
    bool IsPartChecked() const { return m_bPartChecked; }

    /** Set whether a change of the check state is automatically synced to the selection state, keeping the selection state consistent with the check state (Check -> Select)
    * @param [in] bAutoSelectCheck If true, the Select state stays in sync with the Check state
    */
    void SetAutoSelectCheck(bool bAutoSelectCheck) { m_bAutoSelectChecked = bAutoSelectCheck; }

    /** Get whether a change of the check state is automatically synced to the selection state (Check -> Select)
    */
    bool IsAutoSelectCheck() const { return m_bAutoSelectChecked; }

    /** Listen to the event when checked (this event only exists when the SupportCheckMode() function returns true)
     * @param [in] callback The callback function triggered when selected
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachCheck(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventCheck, callback, callbackID); }

    /** Listen to the event when unchecked (this event only exists when the SupportCheckMode() function returns true)
     * @param [in] callback The callback function triggered when deselected
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachUnCheck(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventUnCheck, callback, callbackID); }

    /** Whether the current click is on the CheckBox icon (used in Check mode to determine the mouse click position in the Activate function)
    */
    bool IsCheckBoxImageClicked() const;

    /** Set whether the check mode is supported
    */
    void SetSupportCheckMode(bool bSupport) { m_bSupportCheckMode = bSupport; }

    /** Whether the check mode is supported (currently TreeView/ListCtrl use this mode)
        The check mode means:
        (1) Only when clicking on the CheckBox image is the check box image in the selected state (in non-check mode, clicking anywhere within the control rectangle selects it)
        (2) The check state and the selection state are separated; they are two different states
    */
    virtual bool SupportCheckMode() const { return m_bSupportCheckMode; }

    /** Whether to draw the background color of the selection state; a virtual function is provided as an optional choice
    *   (for example, ListBox/TreeView nodes in multi-select mode do not need to draw the selection state background color because of the check items)
    */
    virtual bool CanPaintSelectedColors() const { return true; }

public:
    /** Disable the CheckBox functionality and clear all image attributes of the CheckBox (for example, for tree nodes, the CheckBox functionality can be enabled or disabled with a switch)
    */
    void ClearStateImages();

    /** In the selection state, when no background color or background image is set, whether to draw with the corresponding attributes of the non-selection state
     * @return Returns true for the selection state, otherwise false
     */
    bool IsPaintNormalFirst() const { return m_bPaintNormalFirst; }

    /** Set that, in the control selection state, when no background color or background image is set, the corresponding attributes of the non-selection state are used for drawing
     * @param[in] bFirst true draws the non-selection state attributes, false does not draw
     * @return None
     */
    void SetPaintNormalFirst(bool bFirst) { m_bPaintNormalFirst = bFirst; }

protected:
    /** Mouse button up event, used to determine whether the click is on the CheckBox image (implements the check functionality)
    */
    virtual bool ButtonUp(const EventArgs& msg) override;

protected:
    /** Get the actually rendered text color of the specified state used when selected
     * @param [in] buttonStateType The state for which to get the color
     * @param [out] stateType The state actually rendered
     * @return Returns the color string, defined in global.xml
     */
    DString GetPaintSelectedStateTextColor(ControlStateType buttonStateType, ControlStateType& stateType) const;

    /** Internally set the selection state
    * @return Returns true if the internal state changed, otherwise false
    */
    bool PrivateSetSelected(bool bSelected);

    /** Internally set the check state
    * @return Returns true if the internal state changed, otherwise false
    */
    bool PrivateSetChecked(bool bChecked);

    /** Selection state change event (the m_bSelected variable changed)
    */
    virtual void OnPrivateSetSelected() {}

    /** Check state change event (the m_bChecked variable changed)
    */
    virtual void OnPrivateSetChecked() {}

private:
    //Text color of the selection state
    UiString m_dwSelectedTextColor;

    //Text color of the selection state
    StateColorMap* m_pSelectedTextColorMap;

    //Background color of the selection state
    StateColorMap2* m_pSelectedColorMap;

    //Selection state
    bool m_bSelected;

    //Whether it is part selected (only affects which image is drawn in the selection state; no effect on business logic)
    bool m_bPartSelected;

    /** When selected, whether to auto-check, i.e. when m_bSelected changes, synchronously modify m_bChecked to keep the values the same
     */
    bool m_bAutoCheckSelected;

    //Whether to draw the Normal state first
    bool m_bPaintNormalFirst;

private:
    /// (Variables for the two features: three-state selection [all selected, part selected, unselected] / check mode)

    //Whether the check functionality is supported
    bool m_bSupportCheckMode;

    //Whether it is already in the Check state (meaningful only when the SupportCheckMode() function returns true)
    bool m_bChecked;

    //Whether it is part checked (only affects which image is drawn in the check state; no effect on business logic)
    bool m_bPartChecked;

    /** When checked, whether to auto-select, i.e. when m_bChecked changes, synchronously modify m_bSelected to keep the values the same
    */
    bool m_bAutoSelectChecked;

    //Whether the current click is on the CheckBox icon
    bool m_bCheckBoxImageClicked;

    //The rectangle of the CheckBox icon (meaningful only when the SupportCheckMode() function returns true)
    UiRect* m_pCheckBoxImageRect;
};

template<typename InheritType>
CheckBoxTemplate<InheritType>::CheckBoxTemplate(Window* pWindow) :
    ButtonTemplate<InheritType>(pWindow),
    m_bSelected(false), 
    m_bPaintNormalFirst(false), 
    m_dwSelectedTextColor(), 
    m_pSelectedTextColorMap(nullptr),
    m_pSelectedColorMap(nullptr),
    m_bPartSelected(false),
    m_bSupportCheckMode(false),
    m_bChecked(false),
    m_bPartChecked(false),
    m_pCheckBoxImageRect(nullptr),
    m_bAutoCheckSelected(false),
    m_bAutoSelectChecked(false),
    m_bCheckBoxImageClicked(false)
{
}

template<typename InheritType>
CheckBoxTemplate<InheritType>::~CheckBoxTemplate()
{
    if (m_pSelectedTextColorMap != nullptr) {
        delete m_pSelectedTextColorMap;
        m_pSelectedTextColorMap = nullptr;
    }
    if (m_pSelectedColorMap != nullptr) {
        delete m_pSelectedColorMap;
        m_pSelectedColorMap = nullptr;
    }
    if (m_pCheckBoxImageRect != nullptr) {
        delete m_pCheckBoxImageRect;
        m_pCheckBoxImageRect = nullptr;
    }
}

template<typename InheritType>
inline DString CheckBoxTemplate<InheritType>::GetType() const { return DUI_CTR_CHECKBOX; }

template<>
inline DString CheckBoxTemplate<Box>::GetType() const { return DUI_CTR_CHECKBOXBOX; }

template<>
inline DString CheckBoxTemplate<HBox>::GetType() const { return DUI_CTR_CHECKBOXHBOX; }

template<>
inline DString CheckBoxTemplate<VBox>::GetType() const { return DUI_CTR_CHECKBOXVBOX; }

template<typename InheritType>
void CheckBoxTemplate<InheritType>::SetAttribute(const DString& strName, const DString& strValue)
{
    if (strName == _T("selected")) {
        Selected(strValue == _T("true"), true);
    }
    else if ((strName == _T("switch_select")) || (strName == _T("switchselect"))) {
        Selected(!IsSelected());
    }
    else if (strName == _T("support_check_Mode")) {
        SetSupportCheckMode(strValue == _T("true"));
    }
    else if (strName == _T("auto_check_select")) {
        //Set whether a change of the selection state is automatically synced to the check state, keeping the check state consistent with the selection state (Select->Check)
        SetAutoCheckSelect(strValue == _T("true"));
    }
    else if (strName == _T("auto_select_check")) {
        //Set whether a change of the check state is automatically synced to the selection state, keeping the selection state consistent with the check state (Check -> Select)
        SetAutoSelectCheck(strValue == _T("true"));
    }
    else if ((strName == _T("normal_first")) || (strName == _T("normalfirst"))) {
        SetPaintNormalFirst(strValue == _T("true"));
    }
    else if ((strName == _T("selected_normal_image")) || (strName == _T("selectednormalimage"))) {
        SetSelectedStateImage(kControlStateNormal, strValue);
    }
    else if ((strName == _T("selected_hot_image")) || (strName == _T("selectedhotimage"))) {
        SetSelectedStateImage(kControlStateHot, strValue);
    }
    else if ((strName == _T("selected_pushed_image")) || (strName == _T("selectedpushedimage"))) {
        SetSelectedStateImage(kControlStatePushed, strValue);
    }
    else if ((strName == _T("selected_disabled_image")) || (strName == _T("selecteddisabledimage"))) {
        SetSelectedStateImage(kControlStateDisabled, strValue);
    }
    else if ((strName == _T("selected_fore_normal_image")) || (strName == _T("selectedforenormalimage"))) {
        SetSelectedForeStateImage(kControlStateNormal, strValue);
    }
    else if ((strName == _T("selected_fore_hot_image")) || (strName == _T("selectedforehotimage"))) {
        SetSelectedForeStateImage(kControlStateHot, strValue);
    }
    else if ((strName == _T("selected_fore_pushed_image")) || (strName == _T("selectedforepushedimage"))) {
        SetSelectedForeStateImage(kControlStatePushed, strValue);
    }
    else if ((strName == _T("selected_fore_disabled_image")) || (strName == _T("selectedforedisabledimage"))) {
        SetSelectedForeStateImage(kControlStateDisabled, strValue);
    }
    else if (strName == _T("part_selected_normal_image")) {
        SetPartSelectedStateImage(kControlStateNormal, strValue);
    }
    else if (strName == _T("part_selected_hot_image")) {
        SetPartSelectedStateImage(kControlStateHot, strValue);
    }
    else if (strName == _T("part_selected_pushed_image")) {
        SetPartSelectedStateImage(kControlStatePushed, strValue);
    }
    else if (strName == _T("part_selected_disabled_image")) {
        SetPartSelectedStateImage(kControlStateDisabled, strValue);
    }
    else if (strName == _T("part_selected_fore_normal_image")) {
        SetPartSelectedForeStateImage(kControlStateNormal, strValue);
    }
    else if (strName == _T("part_selected_fore_hot_image")) {
        SetPartSelectedForeStateImage(kControlStateHot, strValue);
    }
    else if (strName == _T("part_selected_fore_pushed_image")) {
        SetPartSelectedForeStateImage(kControlStatePushed, strValue);
    }
    else if (strName == _T("part_selected_fore_disabled_image")) {
        SetPartSelectedForeStateImage(kControlStateDisabled, strValue);
    }
    else if ((strName == _T("selected_text_color")) || (strName == _T("selectedtextcolor"))) {
        SetSelectedTextColor(strValue);
    }
    else if ((strName == _T("selected_normal_text_color")) || (strName == _T("selectednormaltextcolor"))) {
        SetSelectedStateTextColor(kControlStateNormal, strValue);
    }
    else if ((strName == _T("selected_hot_text_color")) || (strName == _T("selectedhottextcolor"))) {
        SetSelectedStateTextColor(kControlStateHot, strValue);
    }
    else if ((strName == _T("selected_pushed_text_color")) || (strName == _T("selectedpushedtextcolor"))) {
        SetSelectedStateTextColor(kControlStatePushed, strValue);
    }
    else if ((strName == _T("selected_disabled_text_color")) || (strName == _T("selecteddisabledtextcolor"))) {
        SetSelectedStateTextColor(kControlStateDisabled, strValue);
    }
    else if ((strName == _T("selected_normal_color")) || (strName == _T("selectednormalcolor"))) {
        SetSelectedStateColor(kControlStateNormal, strValue);
    }
    else if ((strName == _T("selected_hot_color")) || (strName == _T("selectedhotcolor"))) {
        SetSelectedStateColor(kControlStateHot, strValue);
    }
    else if ((strName == _T("selected_pushed_color")) || (strName == _T("selectedpushedcolor"))) {
        SetSelectedStateColor(kControlStatePushed, strValue);
    }
    else if ((strName == _T("selected_disabled_color")) || (strName == _T("selecteddisabledcolor"))) {
        SetSelectedStateColor(kControlStateDisabled, strValue);
    }
    else if (strName == _T("selected_normal_color_margin")) {
        UiMargin rcMargin;
        AttributeUtil::ParseMarginValue(strValue.c_str(), rcMargin);
        SetSelectedStateColorMargin(kControlStateNormal, rcMargin, true);
    }
    else if (strName == _T("selected_hot_color_margin")) {
        UiMargin rcMargin;
        AttributeUtil::ParseMarginValue(strValue.c_str(), rcMargin);
        SetSelectedStateColorMargin(kControlStateHot, rcMargin, true);
    }
    else if (strName == _T("selected_pushed_color_margin")) {
        UiMargin rcMargin;
        AttributeUtil::ParseMarginValue(strValue.c_str(), rcMargin);
        SetSelectedStateColorMargin(kControlStatePushed, rcMargin, true);
    }
    else if (strName == _T("selected_disabled_color_margin")) {
        UiMargin rcMargin;
        AttributeUtil::ParseMarginValue(strValue.c_str(), rcMargin);
        SetSelectedStateColorMargin(kControlStateDisabled, rcMargin, true);
    }
    else if (strName == _T("selected_normal_color_round")) {
        UiSize szRound;
        AttributeUtil::ParseSizeValue(strValue.c_str(), szRound);
        SetSelectedStateColorRound(kControlStateNormal, szRound, true);
    }
    else if (strName == _T("selected_hot_color_round")) {
        UiSize szRound;
        AttributeUtil::ParseSizeValue(strValue.c_str(), szRound);
        SetSelectedStateColorRound(kControlStateHot, szRound, true);
    }
    else if (strName == _T("selected_pushed_color_round")) {
        UiSize szRound;
        AttributeUtil::ParseSizeValue(strValue.c_str(), szRound);
        SetSelectedStateColorRound(kControlStatePushed, szRound, true);
    }
    else if (strName == _T("selected_disabled_color_round")) {
        UiSize szRound;
        AttributeUtil::ParseSizeValue(strValue.c_str(), szRound);
        SetSelectedStateColorRound(kControlStateDisabled, szRound, true);
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

template<typename InheritType>
void CheckBoxTemplate<InheritType>::ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale)
{
    if (!this->Dpi().CheckDisplayScaleFactor(nNewDpiScale)) {
        return;
    }
    BaseClass::ChangeDpiScale(nOldDpiScale, nNewDpiScale);
    if (m_pSelectedColorMap != nullptr) {
        for (int32_t nStateType = 0; nStateType < kControlStateCount; ++nStateType) {
            ControlStateType stateType = (ControlStateType)nStateType;

            UiMargin colorMargin = m_pSelectedColorMap->GetStateColorMargin(stateType);
            UiMargin newColorMargin = this->Dpi().GetScaleMargin(colorMargin, nOldDpiScale);
            if (!newColorMargin.Equals(colorMargin)) {
                m_pSelectedColorMap->SetStateColorMargin(stateType, newColorMargin);
            }

            UiSize colorRound = m_pSelectedColorMap->GetStateColorRound(stateType);
            UiSize newColorRound = this->Dpi().GetScaleSize(colorRound, nOldDpiScale);
            if (!newColorRound.Equals(colorRound)) {
                m_pSelectedColorMap->SetStateColorRound(stateType, newColorRound);
            }
        }
    }
}

template<typename InheritType>
bool CheckBoxTemplate<InheritType>::ButtonUp(const EventArgs& msg)
{
    std::weak_ptr<WeakFlag> weakFlag = this->GetWeakFlag();
    bool bSetChecked = false;
    bool bCheckedMode = SupportCheckMode();
    if (bCheckedMode && (m_pCheckBoxImageRect != nullptr)) {
        if (this->IsEnabled()) {
            UiRect pos = this->GetPos();
            UiPoint pt(msg.ptMouse);
            pt.Offset(this->GetScrollOffsetInScrollBox());
            if (pos.ContainsPt(pt) && m_pCheckBoxImageRect->ContainsPt(pt)) {
                //Confirm that the click is on the CheckBox icon, change the check state (toggle attribute)
                SetChecked(!IsChecked(), true);
                bSetChecked = true;
                if (weakFlag.expired()) {
                    return true;
                }
            }
        }        
    }
    if (bSetChecked) {
        //Mark the click as being on the CheckBox icon
        m_bCheckBoxImageClicked = true;
    }
    bool bRet = BaseClass::ButtonUp(msg);
    if (bSetChecked && !weakFlag.expired()) {
        //Clear the mark of the click being on the CheckBox icon
        m_bCheckBoxImageClicked = false;
    }
    return bRet;
}

template<typename InheritType>
bool CheckBoxTemplate<InheritType>::IsCheckBoxImageClicked() const
{
    return m_bCheckBoxImageClicked;
}

template<typename InheritType>
void CheckBoxTemplate<InheritType>::Activate(const EventArgs* /*pMsg*/)
{
    if (!this->IsActivatable()) {
        return;
    }
    Selected(!m_bSelected, true);
}

template<typename InheritType>
void CheckBoxTemplate<InheritType>::SetSelected(bool bSelected)
{
    //Only modify the variable value; do not trigger events or update the UI
    PrivateSetSelected(bSelected);
}

template<typename InheritType>
void CheckBoxTemplate<InheritType>::Selected(bool bSelected, bool bTriggerEvent, uint64_t /*vkFlag*/)
{
    bool bSelectChanged = m_bSelected != bSelected;
    bool bChanged = PrivateSetSelected(bSelected);
    if (bTriggerEvent && bSelectChanged) {
        auto flag = this->GetWeakFlag();
        if (m_bSelected) {
            this->SendEvent(kEventSelect);
        }
        else {
            this->SendEvent(kEventUnSelect);
        }
        if (flag.expired()) {
            return;
        }
        if (IsAutoCheckSelect()) {
            //Trigger the Check event synchronously
            if (m_bChecked) {
                this->SendEvent(kEventCheck);
            }
            else {
                this->SendEvent(kEventUnCheck);
            }
            if (flag.expired()) {
                return;
            }
        }
    }
    if (bChanged) {
        this->Invalidate();
    }    
}

template<typename InheritType>
bool CheckBoxTemplate<InheritType>::PrivateSetSelected(bool bSelected)
{
    bool bChanged = false;
    bool bSelectChanged = false;
    if (m_bSelected != bSelected) {
        m_bSelected = bSelected;
        bSelectChanged = true;
        bChanged = true;
    }
    if (!bSelected && m_bPartSelected) {
        //When not in the selection state, reset the part-selected flag
        m_bPartSelected = false;
        bChanged = true;
    }
    if (IsAutoCheckSelect()) {
        //Automatically sync to the Check variable
        bChanged = PrivateSetChecked(m_bSelected);
    }
    if (bSelectChanged) {
        OnPrivateSetSelected();
    }
    return bChanged;
}

template<typename InheritType>
bool CheckBoxTemplate<InheritType>::SetPartSelected(bool bPartSelected)
{
    bool bChanged = false;
    if (m_bPartSelected != bPartSelected) {
        m_bPartSelected = bPartSelected;        
        bChanged = true;
    }
    if (IsAutoCheckSelect()) {
        //Automatically sync to the PartChecked variable
        if (m_bPartChecked != bPartSelected) {
            m_bPartChecked = bPartSelected;
            bChanged = true;
        }
    }
    if (bChanged) {
        this->Invalidate();
    }
    return bChanged;
}

template<typename InheritType>
void CheckBoxTemplate<InheritType>::SetChecked(bool bChecked, bool bTriggerEvent)
{
    bool bCheckChanged = m_bChecked != bChecked;
    bool bChanged = PrivateSetChecked(bChecked);
    if (bTriggerEvent && bCheckChanged) {
        auto flag = this->GetWeakFlag();
        if (m_bChecked) {
            this->SendEvent(kEventCheck);
        }
        else {
            this->SendEvent(kEventUnCheck);
        }
        if (flag.expired()) {
            return;
        }
        if (IsAutoSelectCheck()) {
            if (m_bSelected) {
                this->SendEvent(kEventSelect);
            }
            else {
                this->SendEvent(kEventUnSelect);
            }
            if (flag.expired()) {
                return;
            }
        }
    }
    if (bChanged) {
        this->Invalidate();
    }
}

template<typename InheritType>
bool CheckBoxTemplate<InheritType>::SetPartChecked(bool bPartChecked)
{
    bool bChanged = false;
    if (m_bPartChecked != bPartChecked) {
        m_bPartChecked = bPartChecked;
        bChanged = true;
    }
    if (IsAutoSelectCheck()) {
        //Automatically sync to the PartSelected variable
        if (m_bPartSelected != bPartChecked) {
            m_bPartSelected = bPartChecked;
            bChanged = true;
        }
    }
    if (bChanged) {
        this->Invalidate();
    }
    return bChanged;
}

template<typename InheritType>
bool CheckBoxTemplate<InheritType>::PrivateSetChecked(bool bChecked)
{
    bool bChanged = false;
    bool bCheckChanged = false;
    if (m_bChecked != bChecked) {
        m_bChecked = bChecked;
        bChanged = true;
        bCheckChanged = true;
    }
    if (!bChecked && m_bPartChecked) {
        //When not in the selection state, reset the part-selected flag
        m_bPartChecked = false;
        bChanged = true;
    }    
    if (IsAutoSelectCheck()) {
        //Automatically sync to the Select variable
        bChanged = PrivateSetSelected(m_bChecked);
    }
    if (bCheckChanged) {
        OnPrivateSetChecked();
    }
    return bChanged;
}

template<typename InheritType>
bool CheckBoxTemplate<InheritType>::HasHotState()
{
    if (BaseClass::HasHotState()) {
        return true;
    }
    if ((m_pSelectedColorMap != nullptr) && m_pSelectedColorMap->HasHotColor()) {
        return true;
    }
    if ((m_pSelectedTextColorMap != nullptr) && m_pSelectedTextColorMap->HasHotColor()) {
        return true;
    }
    return false;
}

template<typename InheritType>
DString CheckBoxTemplate<InheritType>::GetBorderColor(ControlStateType stateType) const
{
    if (this->IsSelected()) {
        DString borderColor = BaseClass::GetBorderColor(kControlStatePushed);
        if (!borderColor.empty()) {
            return borderColor;
        }
    }
    return BaseClass::GetBorderColor(stateType);
}

template<typename InheritType>
void CheckBoxTemplate<InheritType>::ClearStateImages()
{
    if (m_pCheckBoxImageRect != nullptr) {
        delete m_pCheckBoxImageRect;
        m_pCheckBoxImageRect = nullptr;
    }
    BaseClass::ClearStateImages();
}

template<typename InheritType>
void CheckBoxTemplate<InheritType>::PaintStateColors(IRender* pRender)
{
    if (!IsSelected() || !CanPaintSelectedColors()) {
        BaseClass::PaintStateColors(pRender);
        return;
    }

    if (m_pSelectedColorMap == nullptr) {
        if (IsPaintNormalFirst()) {
            this->PaintStateColor(pRender, this->GetState());
        }
    }
    else {
        if (IsPaintNormalFirst() && !m_pSelectedColorMap->HasStateColors()) {
            this->PaintStateColor(pRender, this->GetState());
        }
        else {
            m_pSelectedColorMap->PaintStateColor(pRender, this->GetRect(), this->GetState());
        }
    }
}

template<typename InheritType>
void CheckBoxTemplate<InheritType>::PaintStateImages(IRender* pRender)
{
    bool bCheckMode = SupportCheckMode();
    if (bCheckMode && (m_pCheckBoxImageRect == nullptr)) {
        m_pCheckBoxImageRect = new UiRect;
    }

    bool isSelectNone = false;
    if (bCheckMode) {
        //If SupportCheckMode() is true, use IsChecked() to determine whether to show the selection state image
        if (!IsChecked()) {
            isSelectNone = true;
        }
    }
    else {
        //If SupportCheckMode() is false, use IsSelected() to determine whether to show the selection state image
        if (!IsSelected()) {
            isSelectNone = true;
        }
    }

    if (isSelectNone) {
        //Unselected state
        this->PaintStateImage(pRender, kStateImageBk, this->GetState(), _T(""), m_pCheckBoxImageRect);
        this->PaintStateImage(pRender, kStateImageFore, this->GetState(), _T(""), m_pCheckBoxImageRect);
        return;
    }
    bool bPartSelected = bCheckMode ? this->IsPartChecked() : this->IsPartSelected();
    if (bPartSelected) {
        //Part selected state
        bool bPainted = false;
        if (this->HasStateImage(kStateImagePartSelectedBk)) {
            this->PaintStateImage(pRender, kStateImagePartSelectedBk, this->GetState(), _T(""), m_pCheckBoxImageRect);
            bPainted = true;
        }
        if (this->HasStateImage(kStateImagePartSelectedFore)) {
            this->PaintStateImage(pRender, kStateImagePartSelectedFore, this->GetState(), _T(""), m_pCheckBoxImageRect);
            bPainted = true;
        }
        if (bPainted) {
            //If the part selected state has already been drawn, return
            return;
        }
    }

    //All selected state
    if (IsPaintNormalFirst() && !this->HasStateImage(kStateImageSelectedBk)) {
        this->PaintStateImage(pRender, kStateImageBk, this->GetState(), _T(""), m_pCheckBoxImageRect);
    }
    else {
        this->PaintStateImage(pRender, kStateImageSelectedBk, this->GetState(), _T(""), m_pCheckBoxImageRect);
    }

    if (IsPaintNormalFirst() && !this->HasStateImage(kStateImageSelectedFore)) {
        this->PaintStateImage(pRender, kStateImageFore, this->GetState(), _T(""), m_pCheckBoxImageRect);
    }
    else {
        this->PaintStateImage(pRender, kStateImageSelectedFore, this->GetState(), _T(""), m_pCheckBoxImageRect);
    }
}

template<typename InheritType>
void CheckBoxTemplate<InheritType>::PaintText(IRender* pRender)
{
    if (!IsSelected()) {
        BaseClass::PaintText(pRender);
        return;
    }
    DString textValue = this->GetText();
    if (textValue.empty() || (pRender == nullptr)) {
        return;
    }
    UiRect rc = this->GetRect();
    UiPadding rcPadding = this->GetControlPadding();
    rc.Deflate(rcPadding);
    rc.Deflate(this->GetTextPadding());

    auto stateType = this->GetState();
    DString clrColor = GetPaintSelectedStateTextColor(this->GetState(), stateType);
    if (clrColor.empty()) {
        clrColor = m_dwSelectedTextColor.empty() ? this->GetPaintStateTextColor(this->GetState(), stateType) : m_dwSelectedTextColor.c_str();
    }
    UiColor dwClrColor = this->GetUiColor(clrColor);

    uint32_t uTextStyle = this->GetTextStyle();
    if (this->IsSingleLine()) {        
        uTextStyle |= TEXT_SINGLELINE;        
    }
    else {
        uTextStyle &= ~TEXT_SINGLELINE;
    }
    this->SetTextStyle(uTextStyle, false);

    DrawStringParam drawParam = this->GetDrawParam();//Drawing parameters
    drawParam.textRect = rc;

    if (this->IsAnimationPlayerPlaying(AnimationType::kAnimationHot)) {
        if ((stateType == kControlStateNormal || stateType == kControlStateHot) &&
            !GetSelectedStateTextColor(kControlStateHot).empty()) {
            //First draw the default text
            const uint8_t nHotAlpha = this->GetHotAlpha();
            bool bPainted = false;
            DString clrStateColor = GetSelectedStateTextColor(kControlStateNormal);
            if (!clrStateColor.empty()) {
                drawParam.dwTextColor = this->GetUiColor(clrStateColor);
                drawParam.uFade = 255 - nHotAlpha;
                pRender->DrawString(textValue, drawParam);
                bPainted = true;
            }
            //Draw the Hot state text (semi-transparent)
            DString textColor = GetSelectedStateTextColor(kControlStateHot);
            if (!textColor.empty()) {
                drawParam.dwTextColor = this->GetUiColor(textColor);
                drawParam.uFade = nHotAlpha;
                pRender->DrawString(textValue, drawParam);
                bPainted = true;
            }
            if (bPainted) {
                return;
            }
        }
    }

    drawParam.dwTextColor = dwClrColor;
    drawParam.uFade = 255;
    pRender->DrawString(textValue, drawParam);
}

template<typename InheritType>
DString CheckBoxTemplate<InheritType>::GetSelectedStateImage(ControlStateType stateType) const
{
    return this->GetStateImage(kStateImageSelectedBk, stateType);
}

template<typename InheritType>
void CheckBoxTemplate<InheritType>::SetSelectedStateImage(ControlStateType stateType, const DString& strImage)
{
    this->SetStateImage(kStateImageSelectedBk, stateType, strImage);
    this->RelayoutOrRedraw();
}

template<typename InheritType>
DString CheckBoxTemplate<InheritType>::GetSelectedForeStateImage(ControlStateType stateType) const
{
    return this->GetStateImage(kStateImageSelectedFore, stateType);
}

template<typename InheritType>
void CheckBoxTemplate<InheritType>::SetSelectedForeStateImage(ControlStateType stateType, const DString& strImage)
{
    this->SetStateImage(kStateImageSelectedFore, stateType, strImage);
    this->RelayoutOrRedraw();
}

template<typename InheritType>
DString CheckBoxTemplate<InheritType>::GetPartSelectedStateImage(ControlStateType stateType)
{
    return this->GetStateImage(kStateImagePartSelectedBk, stateType);
}

template<typename InheritType>
void CheckBoxTemplate<InheritType>::SetPartSelectedStateImage(ControlStateType stateType, const DString& strImage)
{
    this->SetStateImage(kStateImagePartSelectedBk, stateType, strImage);
    this->RelayoutOrRedraw();
}

template<typename InheritType>
DString CheckBoxTemplate<InheritType>::GetPartSelectedForeStateImage(ControlStateType stateType)
{
    return this->GetStateImage(kStateImagePartSelectedFore, stateType);
}

template<typename InheritType>
void CheckBoxTemplate<InheritType>::SetPartSelectedForeStateImage(ControlStateType stateType, const DString& strImage)
{
    this->SetStateImage(kStateImagePartSelectedFore, stateType, strImage);
    this->RelayoutOrRedraw();
}

template<typename InheritType>
DString CheckBoxTemplate<InheritType>::GetSelectedTextColor() const
{
    return m_dwSelectedTextColor.c_str();
}

template<typename InheritType>
void CheckBoxTemplate<InheritType>::SetSelectedTextColor(const DString& dwTextColor)
{
    m_dwSelectedTextColor = dwTextColor;
    this->Invalidate();
}

template<typename InheritType /*= Control*/>
DString ui::CheckBoxTemplate<InheritType>::GetSelectedStateTextColor(ControlStateType stateType) const
{
    if (m_pSelectedTextColorMap != nullptr) {
        return m_pSelectedTextColorMap->GetStateColor(stateType);
    }
    return DString();
}

template<typename InheritType /*= Control*/>
void ui::CheckBoxTemplate<InheritType>::SetSelectedStateTextColor(ControlStateType stateType, const DString& dwTextColor)
{
    if (m_pSelectedTextColorMap == nullptr) {
        m_pSelectedTextColorMap = new StateColorMap(this);
    }
    m_pSelectedTextColorMap->SetStateColor(stateType, dwTextColor);
    this->Invalidate();
}

template<typename InheritType /*= Control*/>
DString ui::CheckBoxTemplate<InheritType>::GetPaintSelectedStateTextColor(ControlStateType buttonStateType, ControlStateType& stateType) const
{
    stateType = buttonStateType;
    if (stateType == kControlStatePushed && GetSelectedStateTextColor(kControlStatePushed).empty()) {
        stateType = kControlStateHot;
    }
    if (stateType == kControlStateHot && GetSelectedStateTextColor(kControlStateHot).empty()) {
        stateType = kControlStateNormal;
    }
    if (stateType == kControlStateDisabled && GetSelectedStateTextColor(kControlStateDisabled).empty()) {
        stateType = kControlStateNormal;
    }
    return GetSelectedStateTextColor(stateType);
}

template<typename InheritType>
DString CheckBoxTemplate<InheritType>::GetSelectStateColor(ControlStateType stateType) const
{
    if (m_pSelectedColorMap != nullptr) {
        return m_pSelectedColorMap->GetStateColor(stateType);
    }
    return DString();
}

template<typename InheritType>
UiMargin CheckBoxTemplate<InheritType>::GetSelectStateColorMargin(ControlStateType stateType) const
{
    if (m_pSelectedColorMap != nullptr) {
        return m_pSelectedColorMap->GetStateColorMargin(stateType);
    }
    return UiMargin();
}

template<typename InheritType>
UiSize CheckBoxTemplate<InheritType>::GetSelectStateColorRound(ControlStateType stateType) const
{
    if (m_pSelectedColorMap != nullptr) {
        return m_pSelectedColorMap->GetStateColorRound(stateType);
    }
    return UiSize();
}

template<typename InheritType>
void CheckBoxTemplate<InheritType>::SetSelectedStateColor(ControlStateType stateType, const DString& stateColor)
{
    if (m_pSelectedColorMap == nullptr) {
        m_pSelectedColorMap = new StateColorMap2(this);
    }
    if (m_pSelectedColorMap->GetStateColor(stateType) != stateColor) {
        m_pSelectedColorMap->SetStateColor(stateType, stateColor);
        this->Invalidate();
    }
}

template<typename InheritType>
void CheckBoxTemplate<InheritType>::SetSelectedStateColorMargin(ControlStateType stateType, UiMargin colorMargin, bool bNeedDpiScale)
{

    if (m_pSelectedColorMap == nullptr) {
        m_pSelectedColorMap = new StateColorMap2(this);
    }
    if (bNeedDpiScale) {
        this->Dpi().ScaleMargin(colorMargin);
    }
    if (m_pSelectedColorMap->GetStateColorMargin(stateType) != colorMargin) {
        m_pSelectedColorMap->SetStateColorMargin(stateType, colorMargin);
        this->Invalidate();
    }
}

template<typename InheritType>
void CheckBoxTemplate<InheritType>::SetSelectedStateColorRound(ControlStateType stateType, UiSize colorRound, bool bNeedDpiScale)
{
    if (m_pSelectedColorMap == nullptr) {
        m_pSelectedColorMap = new StateColorMap2(this);
    }
    if (bNeedDpiScale) {
        this->Dpi().ScaleSize(colorRound);
    }
    if (m_pSelectedColorMap->GetStateColorRound(stateType) != colorRound) {
        m_pSelectedColorMap->SetStateColorRound(stateType, colorRound);
        this->Invalidate();
    }
}

typedef CheckBoxTemplate<Control> CheckBox;
typedef CheckBoxTemplate<Box> CheckBoxBox;
typedef CheckBoxTemplate<HBox> CheckBoxHBox;
typedef CheckBoxTemplate<VBox> CheckBoxVBox;

} // namespace ui

#endif // UI_CONTROL_CHECKBOX_H_
