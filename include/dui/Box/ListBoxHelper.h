#ifndef UI_BOX_LISTBOX_HELPER_H_
#define UI_BOX_LISTBOX_HELPER_H_

#include "dui/Core/Callback.h"
#include "dui/Core/EventArgs.h"
#include "dui/Core/UiTypes.h"

namespace ui 
{
class ListBox;
class IRender;

/** Helper class for ListBox mouse and keyboard operations
*/
class DUI_API ListBoxHelper: public SupportWeakCallback
{
public:
    explicit ListBoxHelper(ListBox* pListBox);
    ~ListBoxHelper();
    ListBoxHelper(const ListBoxHelper&) = delete;
    ListBoxHelper& operator = (const ListBoxHelper&) = delete;

public:
    /** Set whether mouse frame selection is supported
    */
    void SetEnableFrameSelection(bool bEnable);

    /** Get whether mouse frame selection is supported
    */
    bool IsEnableFrameSelection() const;

    /** Set the fill color of the mouse frame selection
    */
    void SetFrameSelectionColor(const DString& frameSelectionColor);

    /** Get the fill color of the mouse frame selection
    */
    DString GetFrameSelectionColor() const;

    /** Set the Alpha value of the mouse frame selection fill color
    */
    void SetframeSelectionAlpha(uint8_t frameSelectionAlpha);

    /** Get the Alpha value of the mouse frame selection fill color
    */
    uint8_t GetFrameSelectionAlpha() const;

    /** Set the border color of the mouse frame selection
    */
    void SetFrameSelectionBorderColor(const DString& frameSelectionBorderColor);

    /** Get the border color of the mouse frame selection
    */
    DString GetFrameSelectionBorderColor() const;

    /** Set the border size of the mouse frame selection
    * @param [in] nBorderSize The border size (not DPI scaled)
    */
    void SetFrameSelectionBorderSize(int32_t nBorderSize);

    /** Get the border size of the mouse frame selection (not DPI scaled)
    */
    int32_t GetFrameSelectionBorderSize() const;

    /** Set the top coordinate of normal list items (not Header, not pinned to top) (currently used by ListCtrl)
    */
    void SetNormalItemTop(int32_t nNormalItemTop);

    /** Get the top coordinate of normal list items (not Header, not pinned to top) (currently used by ListCtrl)
    */
    int32_t GetNormalItemTop() const;

public:
    //Mouse message handling
    void OnButtonDown(const UiPoint& ptMouse, Control* pSender);
    void OnButtonUp(const UiPoint& ptMouse, Control* pSender);
    void OnRButtonDown(const UiPoint& ptMouse, Control* pSender);
    void OnRButtonUp(const UiPoint& ptMouse, Control* pSender);
    void OnMouseMove(const UiPoint& ptMouse, Control* pSender);

    /** Received the window lost focus message
    */
    void OnWindowKillFocus();

public:
    /** Draw the border and fill color of the mouse frame selection
    */
    void PaintFrameSelection(IRender* pRender);

private:

    /** Check whether the view needs to be scrolled
    */
    void OnCheckScrollView();

private:
    /** The associated ListBox control
    */
    ListBox* m_pListBox;

private:
    /** Whether the left or right mouse button was pressed in the view
    */
    bool m_bMouseDownInView = false;

    /** Whether the left mouse button is pressed
    */
    bool m_bMouseDown = false;

    /** Whether the right mouse button is pressed
    */
    bool m_bRMouseDown = false;

    /** Whether the mouse drag operation is in progress
    */
    bool m_bInMouseMove = false;

    /** The mouse position when the mouse button was pressed
    */
    UiSize64 m_ptMouseDown;

    /** The mouse position while dragging
    */
    UiSize64 m_ptMouseMove;

    /** The control interface when the mouse button was pressed
    */
    Control* m_pMouseSender;

    /** The cancellation mechanism when the timer scrolls the view
    */
    WeakCallbackFlag m_scrollViewFlag;

    /** Whether mouse frame selection is supported
    */
    bool m_bEnableFrameSelection;

    /** The fill color of the frame selection
    */
    UiString m_frameSelectionColor;

    /** The Alpha value of the frame selection fill color
    */
    uint8_t m_frameSelectionAlpha;

    /** The border color of the frame selection
    */
    UiString m_frameSelectionBorderColor;

    /** The border size of the frame selection
    */
    uint8_t m_frameSelectionBorderSize;

    /** The top coordinate of normal list items (ListCtrl: not Header, not pinned to top)
    */
    int32_t m_nNormalItemTop;
private:

};

} // namespace ui

#endif // UI_BOX_LISTBOX_HELPER_H_
