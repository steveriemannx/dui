#ifndef UI_CONTROL_MARQUEE_H_
#define UI_CONTROL_MARQUEE_H_

#include "dui/Control/Label.h"
#include "dui/Core/Callback.h"

namespace ui {

/** Marquee text scrolling direction
*/
enum class MarqueeDirection
{
    kLeft  = 0,  //Horizontal text scrolling to the left
    kRight = 1,  //Horizontal text scrolling to the right
    kUp    = 2,  //Vertical text scrolling upwards
    kDown  = 3   //Vertical text scrolling downwards
};

/** Marquee scrolling text control (derived from Label, it reuses all text attributes such as text/text_id/font/text_color)
 *
 *  Used to scroll text horizontally or vertically in a loop. When the text is wider (or taller) than the display area,
 *  it scrolls in a loop automatically; otherwise it is shown statically (aligned as described by the Label text_align).
 *
 *  Features:
 *  - Horizontal text (scrolling left/right) and vertical text (scrolling up/down), selected by scroll_direction
 *  - Custom scroll speed (scroll_speed, pixels per second, scaled by DPI automatically)
 *  - Pause/Resume, plus automatic pausing while the mouse hovers over the control (hover_pause, enabled by default)
 *  - Dynamic text updates (SetText/SetTextId; the scroll position is reset after an update)
 *  - Seamless looping when the text exceeds the display area (the text is drawn twice, end to end)
 *
 *  XML attributes (all Label attributes plus the following):
 *  - scroll_direction: "left"/"right"/"up"/"down", default "left"
 *  - scroll_speed: scroll speed (pixels per second, scaled by DPI automatically), default 50
 *  - scroll_interval: timer frame interval in milliseconds, default 30
 *  - hover_pause: whether scrolling pauses while the mouse hovers over the control, default true
 */
class DUI_API Marquee : public Label
{
    typedef Label BaseClass;

public:
    explicit Marquee(Window* pWindow);
    Marquee(const Marquee& r) = delete;
    Marquee& operator=(const Marquee& r) = delete;
    ~Marquee() override;

    /** Gets the control type
    */
    std::string GetType() const override;

    /** Sets an XML attribute
    */
    void SetAttribute(const std::string& strName, const std::string& strValue) override;

    /** Paints the text (scrolling)
    */
    void PaintText(IRender* pRender) override;

    /** Text content update (resets the scroll position)
    */
    void SetText(const std::string& strText) override;

    /** Text language ID update (resets the scroll position)
    */
    void SetTextId(const std::string& strTextId) override;

    /** DPI change (clears the text measurement cache)
    */
    void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;

    /** Window association change (stops the timer)
    */
    void SetWindow(Window* pWindow) override;

    //Mouse interaction: pause on hover
    bool MouseEnter(const EventArgs& msg) override;
    bool MouseLeave(const EventArgs& msg) override;

public:
    /** Sets the scroll direction
    */
    void SetScrollDirection(MarqueeDirection direction);

    /** Gets the scroll direction
    */
    MarqueeDirection GetScrollDirection() const { return m_direction; }

    /** Sets the scroll speed (pixels per second, scaled by DPI automatically)
    */
    void SetScrollSpeed(int32_t nSpeed);

    /** Gets the scroll speed (pixels per second, the raw value before DPI scaling)
    */
    int32_t GetScrollSpeed() const { return m_nScrollSpeed; }

    /** Sets the timer frame interval in milliseconds
    */
    void SetScrollInterval(int32_t nIntervalMs);

    /** Gets the timer frame interval in milliseconds
    */
    int32_t GetScrollInterval() const { return m_nScrollInterval; }

    /** Sets whether scrolling pauses while the mouse hovers over the control
    */
    void SetHoverPause(bool bHoverPause);

    /** Gets whether scrolling pauses while the mouse hovers over the control
    */
    bool IsHoverPause() const { return m_bHoverPause; }

    /** Pauses scrolling (manual)
    */
    void Pause();

    /** Resumes scrolling
    */
    void Resume();

    /** Sets the paused state
    */
    void SetPaused(bool bPaused);

    /** Gets whether scrolling is paused
    */
    bool IsPaused() const { return m_bPaused; }

    /** Gets whether scrolling is currently active (the text exceeds the display area)
    */
    bool IsScrolling() const { return m_bScrolling; }

private:
    /** Starts the scroll timer
    */
    void StartTimer();

    /** Stops the scroll timer
    */
    void StopTimer();

    /** Resets the scroll offset (on a text change)
    */
    void ResetScroll();

    /** Timer callback: updates the scroll offset
    */
    void OnTimerTick();

    /** Checks whether scrolling should currently be paused (manually paused or paused on hover)
    */
    bool IsPausedEffective() const;

private:
    /** Scroll direction
    */
    MarqueeDirection m_direction;

    /** Scroll speed (pixels per second, the raw value before DPI scaling)
    */
    int32_t m_nScrollSpeed;

    /** Timer frame interval in milliseconds
    */
    int32_t m_nScrollInterval;

    /** Whether scrolling pauses while the mouse hovers over the control
    */
    bool m_bHoverPause;

    /** Manual pause flag
    */
    bool m_bPaused;

    /** Whether the mouse is hovering over the control
    */
    bool m_bHovering;

    /** Whether scrolling is currently active (the text exceeds the display area)
    */
    bool m_bScrolling;

    /** Whether the timer has been started
    */
    bool m_bTimerStarted;

    /** Timer ID (greater than 0 means it is registered)
    */
    size_t m_nTimerId;

    /** Current scroll offset (pixels, cycling within 0 ~ m_fPeriod)
    */
    float m_fOffset;

    /** Scroll period (size of the text in the scroll direction plus the gap), measured and cached by PaintText
    */
    float m_fPeriod;

    /** Text measurement cache: the text measured last time (the result is reused while the text is unchanged, avoiding a MeasureString call on every frame)
    */
    std::string m_strMeasuredText;

    /** Cached size of the text in the scroll direction (horizontal = width, vertical = height)
    */
    float m_fTextMainSize;

    /** Cached size of the text in the cross direction (horizontal = line height, vertical = column width)
    */
    float m_fTextCrossSize;

    /** Timer cancellation flag
    */
    WeakCallbackFlag m_timerFlag;
};

} // namespace ui

#endif // UI_CONTROL_MARQUEE_H_
