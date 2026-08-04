#ifndef UI_CONTROL_COMBO_BUTTON_H_
#define UI_CONTROL_COMBO_BUTTON_H_

#include "dui/Core/Box.h"
#include "dui/Box/VBox.h"
#include "dui/Control/TreeView.h"
#include "dui/Control/RichEdit.h"

namespace ui 
{
typedef ButtonTemplate<VBox> ButtonVBox;
class ComboButtonWnd;

/** Button with a drop-down combo box
*/
class DUI_API ComboButton : public Box
{
    typedef Box BaseClass;
    friend class ComboButtonWnd;
public:
    explicit ComboButton(Window* pWindow);
    ComboButton(const ComboButton& r) = delete;
    ComboButton& operator=(const ComboButton& r) = delete;
    virtual ~ComboButton() override;

    /// Override the parent class methods to provide personalized features; please refer to the parent class declarations
    virtual DString GetType() const override;
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;
    virtual bool CanPlaceCaptionBar() const override;
    virtual DString GetBorderColor(ControlStateType stateType) const override;

    /** DPI change: update the control size and layout
    * @param [in] nOldDpiScale The old DPI scale percentage
    * @param [in] nNewDpiScale The new DPI scale percentage, consistent with the value of Dpi().GetScale()
    */
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;

public:

    /** Get the drop-down list size (width and height)
    */
    const UiSize& GetDropBoxSize() const;

    /** Set the drop-down list size (width and height)
     * @param [in] szDropBox The size information to set
     * @param [in] bNeedScaleDpi Whether DPI adaptation is needed
     */
    void SetDropBoxSize(UiSize szDropBox, bool bNeedScaleDpi);

    /** Set whether the ComboBox pops up upward
     * @param[in] top true pops up upward, false pops down by default
     */
    void SetPopupTop(bool top) { m_bPopupTop = top; }

    /** Judge whether the ComboBox popup mode is popping up upward
     * @return Returns true if it pops up upward, otherwise false
     */
    bool IsPopupTop() const { return m_bPopupTop; }

    /** Set the Class attribute of the left button control
    */
    void SetLeftButtonClass(const DString& classValue);

    /** Set the Class attribute of the Label control above the left button
    */
    void SetLeftButtonTopLabelClass(const DString& classValue);

    /** Set the Class attribute of the Label control below the left button
    */
    void SetLeftButtonBottomLabelClass(const DString& classValue);

    /** Set the Class attribute of the right button control
    */
    void SetRightButtonClass(const DString& classValue);

    /** Set the Class attribute of the drop-down list container
    */
    void SetComboBoxClass(const DString& classValue);

public:
    /** Get the container interface of the drop-down list
    */
    Box* GetComboBox() const;

    /** Get the button control
    */
    ButtonVBox* GetLeftButtonBox() const;

    /** Get the text control above the button control
    */
    Label* GetLabelTop() const;

    /** Get the text control below the button control
    */
    Label* GetLabelBottom() const;

    /** The button control
    */
    Button* GetRightButton() const;

    /** Update the position of the drop-down list window
    */
    void UpdateComboWndPos();

    /** Set the shadow type of the drop-down window
    */
    void SetComboWndShadowType(Shadow::ShadowType nShadowType);

    /** Get the shadow type of the drop-down window
    */
    Shadow::ShadowType GetComboWndShadowType() const;

public:
    /** Listen to the child button click event (the left button is clicked)
     * @param [in] callback The triggered callback function
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachClick(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventClick, callback, callbackID);}

    /** Listen to the drop-down window creation event
     * @param [in] callback The callback function triggered after the drop-down window is closed
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachWindowCreate(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventWindowCreate, callback, callbackID); }

    /** Listen to the drop-down window close event
     * @param [in] callback The callback function triggered after the drop-down window is closed; wParam 1 means canceled, 0 means closed normally
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachWindowClose(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventWindowClose, callback, callbackID); }

protected:
    /** Show the drop-down list
    */
    virtual void ShowComboList();

    /** Close the drop-down list
    */
    virtual void HideComboList();

    /** Update the drop-down list
    */
    virtual void UpdateComboList();

    /** The drop-down window closed
    * @param [in] bCanceled true means canceled, otherwise closed normally
    */
    virtual void OnComboWndClosed(bool bCanceled);

    /** Left button click event
     * @param[in] args The parameter list
     * @return Always returns true
     */
    virtual bool OnLeftButtonClicked(const EventArgs& args);

    /** Right button mouse down event
     * @param[in] args The parameter list
     * @return Always returns true
     */
    virtual bool OnRightButtonDown(const EventArgs& args);

    /** Right button click event
     * @param[in] args The parameter list
     * @return Always returns true
     */
    virtual bool OnRightButtonClicked(const EventArgs& args);

    /** Window lost focus
    * @param[in] args The parameter list
    * @return Always returns true
    */
    virtual bool OnWindowKillFocus(const EventArgs& args) override;

    /** Window moved
    * @param[in] args The parameter list
    * @return Always returns true
    */
    virtual bool OnWindowMove(const EventArgs& args);

protected:
    /** Initialize function
    */
    virtual void OnInit() override;

private:
    /** Parse the attribute list
    */
    void ParseAttributeList(const DString& strList,
                            std::vector<std::pair<DString, DString>>& attributeList) const;

    /** Set the attribute list of the control
    */
    void SetAttributeList(Control* pControl, const DString& classValue);

    /** Remove the control
    */
    void RemoveControl(Control* pControl);

    /** The button state changed, sync the state
    * @param[in] args The parameter list
    * @return Always returns true
    */
    bool OnButtonStateChanged(const EventArgs& args);

private:
    /** The window interface of the drop-down list
    */
    ComboButtonWnd* m_pWindow;

    /** Shadow type
    */
    Shadow::ShadowType m_nShadowType;

    /** The size of the drop-down list (width and height)
    */
    UiSize m_szDropBox;

    /** Whether the drop-down list pops up upward
    */
    bool m_bPopupTop;

private:
    /** The container of the drop-down list
    */
    Box* m_pComboBox;

    /** The button container control ButtonVBox
    */
    ButtonVBox* m_pLeftButton;

    /** The text control above in the button container (inside ButtonVBox)
    */
    Label* m_pLabelTop;

    /** The text control below in the button container (inside ButtonVBox)
    */
    Label* m_pLabelBottom;

    /** The button control (the right drop-down button)
    */
    Button* m_pRightButton;

    /** Whether the drop-down list is being shown when the mouse is pressed
    */
    bool m_bDropListShown;
};

} // namespace ui

#endif // UI_CONTROL_COMBO_BUTTON_H_
