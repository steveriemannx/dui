#ifndef _UI_CONTROL_DATETIME_H_
#define _UI_CONTROL_DATETIME_H_

#include "duilib/Control/Label.h"
#include "duilib/Box/HBox.h"
#include <ctime>

namespace ui
{
/** Date time selection control
*/
class DateTimeWnd;
class DUILIB_API DateTime : public LabelTemplate<HBox>
{
    typedef LabelTemplate<HBox> BaseClass;
    friend class DateTimeWnd;
public:
    explicit DateTime(Window* pWindow);
    DateTime(const DateTime& r) = delete;
    DateTime& operator=(const DateTime& r) = delete;
    virtual ~DateTime() override;

public:
    /** Initialize with the local time
    */
    void InitLocalTime();

    /** Clear the date time value
    */
    void ClearTime();

    /** Get the date time value
    */
    const struct tm& GetDateTime() const;

    /** Set the date time value
    */
    void SetDateTime(const struct tm& dateTime);

    /** Whether the current date time value is a valid date time
    */
    bool IsValidDateTime() const;

    /** Get the date time string; the date format can be set via the SetStringFormat() function
    */
    DString GetDateTimeString() const;

    /** Set the date time string and update the date time value according to the format specified by GetStringFormat()
    */
    bool SetDateTimeString(const DString& dateTime);

    /** Get the string format of the date
    */
    DString GetStringFormat() const;
    
    /** Set the date format; the default value depends on the EditFormat value
    * @param [in] sFormat The date format; for details, refer to the description of the std::put_time function:
        %a Abbreviated weekday name
        %A Full weekday name
        %b Abbreviated month name
        %B Full month name
        %c Standard date and time string
        %C The last two digits of the year
        %d Day of the month as a decimal number
        %D Month/day/year
        %e Day of the month as a decimal number in a two-character field
        %F Year-month-day
        %g The last two digits of the year, using the week-based year
        %G The year, using the week-based year
        %h Abbreviated month name
        %H The hour in 24-hour format
        %I The hour in 12-hour format
        %j Day of the year as a decimal number
        %m Month as a decimal number
        %M Minutes as a decimal number
        %n Newline character
        %p The equivalent display of local AM or PM
        %r The time in 12-hour format
        %R Display hour and minute: hh:mm
        %S Seconds as a decimal number
        %t Horizontal tab character
        %T Display hour, minute and second: hh:mm:ss
        %u Day of the week, Monday as the first day (values from 0 to 6, Monday is 0)
        %U Week number of the year, Sunday as the first day (values from 0 to 53)
        %V Week number of the year, using the week-based year
        %w Weekday as a decimal number (values from 0 to 6, Sunday is 0)
        %W Week number of the year, Monday as the first day (values from 0 to 53)
        %x Standard date string
        %X Standard time string
        %y The decimal year without the century (values from 0 to 99)
        %Y The decimal year including the century
        %z, %Z Time zone name; if the time zone name cannot be obtained, an empty character is returned.
        %% Percent sign
    */
    void SetStringFormat(const DString& sFormat);

    /** The edit format of the date time
    */
    enum class EditFormat
    {
        kDateCalendar,      //When editing, displays: year-month-day; the date is modified through a drop-down month calendar, selecting the time is not supported (in the SDL implementation, same as kDateUpDown)
        kDateUpDown,        //When editing, displays: year-month-day; the date is modified through an up-down control placed on the right side of the control, selecting the time is not supported
        kDateTimeUpDown,    //When editing, displays: year-month-day hour:minute:second; the date and time are modified through an up-down control placed on the right side of the control
        kDateMinuteUpDown,  //When editing, displays: year-month-day hour:minute; the date and time are modified through an up-down control placed on the right side of the control
        kTimeUpDown,        //When editing, displays: hour:minute:second; the time is modified through an up-down control placed on the right side of the control, modifying the date is not supported
        kMinuteUpDown,      //When editing, displays: hour:minute; the time is modified through an up-down control placed on the right side of the control, modifying the date is not supported
    };

    /** Set the edit format
    */
    void SetEditFormat(EditFormat editFormat);

    /** Get the edit mode
    */
    EditFormat GetEditFormat() const;

    /** Get the separator of the year, month and day
    */
    DString::value_type GetDateSeparator() const;

    /** Update the position of the edit window
    */
    void UpdateEditWndPos();

    /** Set the Class name of the Spin feature
    */
    void SetSpinClass(const DString& spinClass);

    /** Get the Class name of the Spin feature
    */
    DString GetSpinClass() const;

    /** Add a listener for the date time value change event
    * @param [in] callback The callback function to bind
    * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
    */
    void AttachTimeChanged(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventValueChanged, callback, callbackID); }

public:
    //Virtual functions of the base class
    virtual DString GetType() const override;
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;
    virtual void HandleEvent(const EventArgs& msg) override;

    //Used to initialize xml attributes
    virtual void OnInit() override;

    /** Dispatch the message to the message handler
     * @param[in] msg The message content
     */
    virtual void SendEventMsg(const EventArgs& msg) override;

private:
    /** Compare whether two times are the same
    */
    bool IsEqual(const struct tm& a, const struct tm& b) const;

    /** End the editing
    */
    void EndEditDateTime();

private:
    /** The current date time value
    */
    struct tm m_dateTime;

    /* The display format of the date time
    */
    UiString m_sFormat;

    /** The edit format of the date time
    */
    EditFormat m_editFormat;

    /** The separator of the year, month and day
    */
    DString::value_type m_dateSeparator;

    /** The window interface of the date control
    */
    DateTimeWnd* m_pDateWindow;

    /** The Class name of the Spin feature
    */
    UiString m_spinClass;
};

}//namespace ui

#endif // _UI_CONTROL_DATETIME_H_
