#ifndef _UI_CONTROL_DATETIME_WND_SDL_H_
#define _UI_CONTROL_DATETIME_WND_SDL_H_

#include "duilib/Core/UiTypes.h"
#include "duilib/Core/Callback.h"

#if defined(DUILIB_BUILD_FOR_SDL) || defined(DUILIB_BUILD_FOR_WAYLAND)

namespace ui
{
class DateTime;
class RichEdit;
class Control;
class VBox;

/** The implementation of the date time selection control (SDL)
*/
class DateTimeWnd : public virtual SupportWeakCallback
{
public:
    explicit DateTimeWnd(DateTime* pOwner);
    ~DateTimeWnd();

public:
    //Initialize
    bool Init(DateTime* pOwner);

    //Update the position of the window
    void UpdateWndPos();

    //Show the window
    void ShowWindow();

private:
    /** Clear the association with the control
    */
    void Clear();

    /** Create the edit boxes for year, month, day, hour, minute and second
    */
    RichEdit* CreateEditYear() const;
    RichEdit* CreateEditMon() const;
    RichEdit* CreateEditMDay() const;
    RichEdit* CreateEditHour() const;
    RichEdit* CreateEditMin() const;
    RichEdit* CreateEditSec() const;

    /** Create a text control
    */
    Control* CreateLabel(const DString& text) const;

    /** Control the day range (leap year, etc.)
    */
    void UpdateDayRange();

    /** Set the Class name of the Spin feature
    */
    bool SetSpinClass(const DString& spinClass);

    /** Adjust the numeric value of the text
    */
    void AdjustTextNumber(int32_t nDelta);

    /** Start the timer to adjust the numeric value of the text
    */
    void StartAutoAdjustTextNumberTimer(int32_t nDelta);

    /** Start automatically adjusting the numeric value of the text
    */
    void StartAutoAdjustTextNumber(int32_t nDelta);

    /** Stop automatically adjusting the numeric value of the text
    */
    void StopAutoAdjustTextNumber();

    /** End the editing
    */
    void EndEditDateTime();

    /** The focus transfer event of the edit box
    */
    void OnKillFocusEvent(Control* pControl, Control* pNewFocus);

private:
    /** The associated control
    */
    DateTime* m_pOwner;

    /** Whether it has been initialized
    */
    bool m_bInited;

    /** The currently initialized date edit format
    */
    int8_t m_editFormat;

private:
    /** Year, month, day, hour, minute and second
    */
    enum EditType
    {
        YEAR  = 0,
        MON   = 1,
        MDAY  = 2,
        HOUR  = 3,
        MIN   = 4,
        SEC   = 5,
        COUNT = 6
    };
    /** The list of edit boxes
    */
    std::vector<RichEdit*> m_editList;

    /** The container of the Spin feature
    */
    VBox* m_pSpinBox;

    /** Lifecycle management of the timer that automatically adjusts the numeric value of the text
    */
    WeakCallbackFlag m_flagAdjustTextNumber;
};

} //namespace ui

#endif // DUILIB_BUILD_FOR_SDL

#endif // _UI_CONTROL_DATETIME_WND_SDL_H_
