#ifndef UI_CONTROL_ADDRESS_BAR_H_
#define UI_CONTROL_ADDRESS_BAR_H_

#include "dui/Box/HBox.h"

namespace ui
{
class RichEdit;

/** Address bar control, used to display the path of the local file system
*/
class DUI_API AddressBar : public HBox
{
    typedef HBox BaseClass;
public:
    explicit AddressBar(Window* pWindow);

    /** Get the control type
    */
    virtual DString GetType() const override;
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;
    virtual void SetPos(UiRect rc) override;

    /** Set the path
    */
    void SetAddressPath(const DString& addressPath);

    /** Get the path
    */
    DString GetAddressPath() const;

    /** Show or hide the address bar edit control
    * @param [in] bShow true means show the edit control, false means hide the edit control
    */
    void ShowAddressEdit(bool bShow);

    /** The sub-path most recently clicked by the user on an address bar sub-path button
    */
    DString GetClickedAddressPath() const;

    /** Get the path displayed by the control last time
    */
    DString GetPreviousAddressPath() const;

public:
    /** Set whether to show the tooltip of the path
    */
    void SetEnablePathTooltip(bool bEnable);

    /** Get whether to show the tooltip of the path
    */
    bool IsEnablePathTooltip() const;

    /** Set whether to automatically update the display controls when pressing Enter
    */
    void SetReturnUpdateUI(bool bUpdateUI);

    /** Get whether to automatically update the display controls when pressing Enter
    */
    bool IsReturnUpdateUI() const;

    /** Set whether to automatically update the display controls when pressing ESC
    */
    void SetEscUpdateUI(bool bUpdateUI);

    /** Get whether to automatically update the display controls when pressing ESC
    */
    bool IsEscUpdateUI() const;

    /** Set whether to automatically update the display controls when losing focus
    */
    void SetKillFocusUpdateUI(bool bUpdateUI);

    /** Get whether to automatically update the display controls when losing focus
    */
    bool IsKillFocusUpdateUI() const;

    /** Set the Class of the edit control
    */
    void SetRichEditClass(const DString& editClass);

    /** Get the Class of the edit control
    */
    DString GetRichEditClass() const;

    /** Set the clear button Class of the edit control
    */
    void SetRichEditClearBtnClass(const DString& clearBtnClass);

    /** Get the clear button Class of the edit control
    */
    DString GetRichEditClearBtnClass() const;

    /** Set the Class of the address bar path container (HBox); each sub-path has one HBox container
    */
    void SetSubPathHBoxClass(const DString& hboxClass);

    /** Get the Class of the address bar path container (HBox)
    */
    DString GetSubPathHBoxClass() const;

    /** Set the Class of the address bar sub-path buttons
    */
    void SetSubPathBtnClass(const DString& subPathBtnClass);

    /** Get the Class of the address bar sub-path buttons
    */
    DString GetSubPathBtnClass() const;

    /** Set the Class of the address bar root path (the "/" path)
    */
    void SetSubPathRootClass(const DString& subPathRootClass);

    /** Get the Class of the address bar root path (the "/" path)
    */
    DString GetSubPathRootClass() const;

    /** Set the Class of the address bar path separator
    */
    void SetPathSeparatorClass(const DString& pathSeparatorClass);

    /** Get the Class of the address bar path separator
    */
    DString GetPathSeparatorClass() const;

public:
    /** Listen to the path change event of the address bar (the address bar edit control data is applied when Enter is pressed or focus is lost)
    * @param [in] callback The callback function for event handling; please refer to the EventCallback declaration
    * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
    */
    void AttachPathChanged(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventPathChanged, callback, callbackID); }

    /** Listen to the click event of the user clicking a sub-path button on the address bar
    * @param [in] callback The callback function for event handling; please refer to the EventCallback declaration
    * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
    */
    void AttachPathClick(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventPathClick, callback, callbackID); }

    /** Listen to the Enter event on the address bar
    * @param [in] callback The callback function for event handling; please refer to the EventCallback declaration
    * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
    */
    void AttachReturn(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventReturn, callback, callbackID); }

    /** Listen to the ESC event on the address bar
    * @param [in] callback The callback function for event handling; please refer to the EventCallback declaration
    * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
    */
    void AttachEsc(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventEsc, callback, callbackID); }

protected:
    /** Initialize
    */
    virtual void OnInit() override;

    /** Set visible state event
    * @param [in] bChanged true means the state has changed, false means the state has not changed
    */
    virtual void OnSetVisible(bool bChanged) override;

private:
    /** Add a path
    * @param [in] displayName The display name
    * @param [in] filePath The local path of the file
    */
    bool AddSubPath(const DString& displayName, const DString& filePath);

    /** Enter is pressed in the address bar edit control
    */
    void OnAddressBarReturn();

    /** ESC is pressed in the address bar edit control
    */
    void OnAddressBarEsc();

    /** Focus event
    */
    void OnAddressBarSetFocus(Control* pNewFocus);

    /** Kill focus event
    */
    void OnAddressBarKillFocus(Control* pNewFocus);

    /** The user clicked a sub-path
    */
    void OnClickedSubPath(const DString& filePath);

    /** Set up the address display controls
    */
    bool UpdateAddressBarControls(const DString& addressPath);

    /** Update the status of the address display controls
    */
    void UpdateAddressBarControlsStatus();

    /** Dynamically calculate the display width of each control
    */
    std::vector<int32_t> AdjustControlsWidth(const std::vector<int32_t>& originalWidths, int32_t totalWidth);

private:
    /** The address bar edit control
    */
    RichEdit* m_pRichEdit;

    /** The address bar selection interface control
    */
    HBox* m_pBarBox;

    /** The currently set path
    */
    UiString m_addressPath;

    /** The path currently displayed by the control
    */
    UiString m_showAddressPath;

    /** The path previously displayed by the control
    */
    UiString m_prevShowAddressPath;

    /** The currently clicked sub-path
    */
    UiString m_clickedAddressPath;

    /** The Class of the edit control
    */
    UiString m_editClass;

    /** The clear button Class of the edit control
    */
    UiString m_editClearBtnClass;

    /** The Class of the address bar sub-path container (HBox)
    */
    UiString m_subPathHBoxClass;

    /** The Class of the address bar sub-path buttons
    */
    UiString m_subPathBtnClass;

    /** The Class of the address bar root path (the "/" path)
    */
    UiString m_subPathRootClass;

    /** The Class of the address bar path separator
    */
    UiString m_pathSeparatorClass;

    /** Whether to show the tooltip
    */
    bool m_bEnableTooltip;

    /** Whether to automatically update the display controls when pressing Enter
    */
    bool m_bReturnUpdateUI;

    /** Whether to automatically update the display controls when pressing ESC
    */
    bool m_bEscUpdateUI;

    /** Whether to automatically update the display controls when losing focus
    */
    bool m_bKillFocusUpdateUI;

    /** Whether an update operation is currently being performed
    */
    bool m_bUpdatingUI;
};

}//namespace ui

#endif //UI_CONTROL_ADDRESS_BAR_H_
