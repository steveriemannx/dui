#ifndef UI_BOX_TABBOX_H_
#define UI_BOX_TABBOX_H_

#include "dui/Core/Box.h"

namespace ui
{
/** Stacked layout/card layout: among the multiple child controls inside, only one is visible and the others are hidden; they can be switched dynamically, and switch animations are supported
*/
class DUI_API TabBox : public Box
{
    typedef Box BaseClass;
public:
    explicit TabBox(Window* pWindow, Layout* pLayout = new Layout());

    // Used to initialize xml attributes
    virtual void OnInit() override;

    /// Override the parent class methods to provide personalized functionality; please refer to the parent class declarations
    virtual DString GetType() const override;
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;
    virtual bool AddItem(Control* pControl) override;
    virtual bool AddItemAt(Control* pControl, size_t iIndex) override;
    virtual bool RemoveItem(Control* pControl) override;
    virtual bool RemoveItemAt(size_t iIndex) override;
    virtual void RemoveAllItems() override;    

public:
    /** Get the current selected item
     * @return Returns the index of the current selected item (if there is no valid index, returns Box::InvalidIndex)
     */
    size_t GetCurSel() const;

    /** Select a child item by control index
     * @param [in] iIndex The index of the child item to select
     * @return Returns true on success, otherwise returns false
     */
    bool SelectItem(size_t iIndex);

    /** Select a child item by control pointer
     * @param [in] pControl The pointer of the child item to select
     * @return Returns true on success, otherwise returns false
     */
    bool SelectItem(Control* pControl);

    /** Select a child item by control name
     * @param [in] pControlName The name of the control to select
     * @return Returns true on success, otherwise returns false
     */
    bool SelectItem(const DString& pControlName);
   
    /** Listen for the tab page selection event
     * @param [in] callback The callback function for event handling, please refer to the EventCallback declaration
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachTabSelect(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventTabSelect, callback, callbackID); }

public:
    /** The type of the page switch animation
    */
    enum class FadeSwitchType : int8_t
    {
        kNone,          //No animation effect
        kFadeInOutX,    //The content area slides horizontally (switches left/right)
        kFadeInOut,     //The content area fades in and out
    };

    /** Set whether to show the animation effect
     * @param [in] bFadeSwitch Set to true to show, false to not show the animation
     */
    void SetFadeSwitch(bool bFadeSwitch);

    /** Check whether the animation effect needs to be shown
     * @return Returns true to show the animation, false to not show the animation effect
     */
    bool IsFadeSwitch() const;

    /** Set the type of the page switch animation
    */
    void SetFadeSwitchType(FadeSwitchType fadeSwitchType);

    /** Get the type of the page switch animation
    */
    FadeSwitchType GetFadeSwitchType() const;

    /** Set the timer interval for playing the switch animation (milliseconds)
    * @param [in] frameIntervalMillSeconds The timer interval for playing the animation (milliseconds)
    */
    void SetFadeSwitchFrameIntervalMillSeconds(int32_t frameIntervalMillSeconds);

    /** Get the timer interval for playing the switch animation (milliseconds)
    */
    int32_t GetFadeSwitchFrameIntervalMillSeconds() const;

    /** Set the total playing time of the switch animation (milliseconds)
    * @param [in] totalMillSeconds The total playing time of the animation (milliseconds)
    */
    void SetFadeSwitchTotalMillSeconds(int32_t totalMillSeconds);

    /** Get the total playing time of the switch animation (milliseconds)
    */
    int32_t GetFadeSwitchTotalMillSeconds() const;

    /** Set the easing function type of the switch animation
    */
    void SetFadeSwitchEasingFunctionType(EasingFunctionType easingFunctionType);

    /** Get the easing function type of the switch animation
    */
    EasingFunctionType GetFadeSwitchEasingFunctionType() const;

protected:
    /** When showing a TAB item, handle some attributes
     * @param [in] index The TAB item index
     */
    void OnShowTabItem(size_t index);

    /** When hiding a TAB item, handle some attributes
     * @param [in] index The TAB item index
     */
    void OnHideTabItem(size_t index);

protected:
    /** Start playing the animation
     * @param [in] pNewItemControl The interface of the newly selected tab page
     * @param [in] pOldItemControl The interface of the previously selected tab page
     */
    virtual bool StartSwitchItemAnimation(Control* pNewItemControl, Control* pOldItemControl);

protected:
    /** Set the visible state event
    * @param [in] bChanged true means the state has changed, false means the state has not changed
    */
    virtual void OnSetVisible(bool bChanged) override;

private:
    /** Select a child item by control index
     * @param [in] nItemIndex The index of the child item to select
     * @param [in] bFadeSwith Whether the switch animation is needed
     * @param [in] bCheckChanged Whether to check whether the selection changed
     * @return Returns true on success, otherwise returns false
     */
    bool DoSelectItem(size_t nItemIndex, bool bFadeSwith, bool bCheckChanged);

    /** Set the type of the page switch animation
    */
    void SetFadeSwitchTypeByString(const DString& fadeSwitchType);

    /** Start playing the animation (dynamically switch the X coordinate, the content area slides horizontally)
     * @param [in] pNewItemControl The interface of the newly selected tab page
     * @param [in] pOldItemControl The interface of the previously selected tab page
     */
    bool StartSwitchItemAnimationFadeInOutX(Control* pNewItemControl, Control* pOldItemControl);

    /** Start playing the animation (the content area fades in and out)
     * @param [in] pNewItemControl The interface of the newly selected tab page
     * @param [in] pOldItemControl The interface of the previously selected tab page
     */
    bool StartSwitchItemAnimationFadeInOut(Control* pNewItemControl, Control* pOldItemControl);

private:
    /** The animation playing interface
    */
    std::unique_ptr<AnimationPlayer> m_pAnimationPlayer;

    /** The timer interval for playing the animation (milliseconds)
    */
    int32_t m_frameIntervalMillSeconds;

    /** The total playing time (milliseconds)
    */
    int32_t m_totalMillSeconds;

    /** The index of the current selected item
    */
    size_t m_nCurSel;

    /** The index of the item to be selected during initialization
    */
    size_t m_nInitSel;

    /** The easing function type
    */
    EasingFunctionType m_easingFunctionType;

    /** Whether the switch animation is needed
    */
    FadeSwitchType m_fadeSwithType;
};

} //namespace ui

#endif // UI_BOX_TABBOX_H_
