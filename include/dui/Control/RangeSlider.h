#ifndef UI_CONTROL_RANGE_SLIDER_H_
#define UI_CONTROL_RANGE_SLIDER_H_

#include "dui/Control/Progress.h"
#include "dui/Image/StateImage.h"
#include <string>

namespace ui
{

/** Dual-thumb range selector
*  A single track carries a low and a high thumb; dragging a thumb adjusts the interval, and the selected
*  interval is painted highlighted. Commonly used for interval filters such as price or time ranges.
*/
class DUI_API RangeSlider : public Progress
{
    typedef Progress BaseClass;
public:
    /** Range value (used by the kEventValueChanged event, carrying both the low and the high value)
    */
    struct RangeValue
    {
        double lowValue = 0;    //Low value
        double highValue = 0;   //High value
    };

public:
    explicit RangeSlider(Window* pWindow);

    /// Overrides of the base class methods for custom behavior; see the base class declarations
    virtual std::string GetType() const override;
    virtual void SetAttribute(const std::string& strName, const std::string& strValue) override;
    virtual void HandleEvent(const EventArgs& msg) override;
    virtual void PaintStateImages(IRender* pRender) override;
    virtual void ClearImageCache() override;

    /** Called when the DPI changes; updates the control size and the layout
    * @param [in] nOldDpiScale Old DPI scale percentage
    * @param [in] nNewDpiScale New DPI scale percentage, equal to the value of Dpi().GetScale()
    */
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;

    /** Gets the low value
    */
    double GetLowValue() const { return m_fLowValue; }

    /** Sets the low value (clamped to [min, high - min_range]; fires the value-changed event)
    * @param [in] fValue Low value to set
    */
    void SetLowValue(double fValue);

    /** Sets the low value without firing the kEventValueChanged event, for internal value synchronization
    */
    void SetLowValueNoEvent(double fValue);

    /** Gets the high value (equivalent to GetValue())
    */
    double GetHighValue() const { return GetValue(); }

    /** Sets the high value (clamped to [low + min_range, max]; fires the value-changed event)
    * @param [in] fValue High value to set
    */
    void SetHighValue(double fValue);

    /** Sets the high value without firing the kEventValueChanged event
    */
    void SetHighValueNoEvent(double fValue);

    /** Sets both the low and the high value (clamped; fires one value-changed event)
    * @param [in] fLowValue  Low value
    * @param [in] fHighValue High value
    */
    void SetRange(double fLowValue, double fHighValue);

    /** Sets both the low and the high value without firing the kEventValueChanged event
    */
    void SetRangeNoEvent(double fLowValue, double fHighValue);

    /** Gets the minimum distance between the two thumbs (the minimum difference between the high and the low value); default 0
    */
    double GetMinRange() const { return m_fMinRange; }

    /** Sets the minimum distance between the two thumbs
    * @param [in] fMinRange Minimum distance; values below 0 are treated as 0
    */
    void SetMinRange(double fMinRange);

    /** Sets the thumb size (both thumbs share the same size)
    * @param [in] szXY Size to set
    * @param [in] bNeedDpiScale Whether the size is scaled by DPI
    */
    void SetThumbSize(UiSize szXY, bool bNeedDpiScale);

    /** Gets the thumb size
    */
    UiSize GetThumbSize() const;

    /** Gets the rectangle of the low thumb (the left/lower one)
    */
    UiRect GetLowThumbRect() const { return ThumbRectFromValue(GetLowValue()); }

    /** Gets the rectangle of the high thumb (the right/upper one)
    */
    UiRect GetHighThumbRect() const { return ThumbRectFromValue(GetHighValue()); }

    /** Gets the image of the low thumb for the given state
    * @param[in] stateType State identifier to query, see the ControlStateType enum
    */
    std::string GetThumbStateImage(ControlStateType stateType) const;

    /** Sets the image of the low thumb for the given state
    * @param[in] stateType State identifier to set, see the ControlStateType enum
    * @param[in] strImage Image location to set
    */
    void SetThumbStateImage(ControlStateType stateType, const std::string& strImage);

    /** Gets the image of the high thumb for the given state
    * @param[in] stateType State identifier to query, see the ControlStateType enum
    */
    std::string GetHighThumbStateImage(ControlStateType stateType) const;

    /** Sets the image of the high thumb for the given state (falls back to the low thumb image when empty)
    * @param[in] stateType State identifier to set, see the ControlStateType enum
    * @param[in] strImage Image location to set
    */
    void SetHighThumbStateImage(ControlStateType stateType, const std::string& strImage);

    /** Sets the color of the highlighted interval (the selected part): a semantic color name or a color value; empty falls back to progress_color
    */
    void SetRangeColor(const std::string& strColor);

    /** Gets the color of the highlighted interval
    */
    const std::string& GetRangeColor() const { return m_strRangeColor; }

    /** Sets the corner radius of the highlighted interval (pixels, scaled by the current DPI when painted; default 0 means square corners)
    */
    void SetRangeRadius(int32_t nRadius);

    /** Gets the corner radius of the highlighted interval
    */
    int32_t GetRangeRadius() const { return m_nRangeRadius; }

    /** Sets whether pressing the left mouse button on the track jumps the value to the click position immediately
    * @param [in] bEnable true jumps immediately (default), false keeps the old behavior of jumping only when the button is released
    */
    void SetClickToJump(bool bEnable);

    /** Gets whether pressing the left mouse button on the track jumps the value immediately
    */
    bool IsClickToJump() const { return m_bClickToJump; }

    /** Listens for the range value change event
    * @param [in] callback Callback invoked after the range value changed
    * @param [in] callbackID ID of this callback (used to remove it)
    * Parameter description:
    *   wParam: 1 (range mode marker)
    *   lParam: 0
    *   pEventData: points to RangeValue (holding lowValue / highValue) and is only valid while this callback is
    *               being dispatched; do not store the pointer
    * Recommended usage:
    *   if (msg.pEventData != nullptr) {
    *       auto* pRange = static_cast<const ui::RangeSlider::RangeValue*>(msg.pEventData);
    *       // read pRange->lowValue / pRange->highValue
    *   }
    */
    void AttachRangeChanged(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventValueChanged, callback, callbackID); }

protected:
    /** Computes the thumb rectangle for the given value (clamped)
    * @param [in] fValue Progress value
    */
    UiRect ThumbRectFromValue(double fValue) const;

    /** Computes the interval rectangle for the given value (relative coordinates with the control top-left as origin, clamped)
    *  Horizontal: rc.right is the horizontal fill amount for the value; vertical: rc.top is the vertical start for the value
    * @param [in] fValue Progress value
    */
    UiRect CalcRangeRect(double fValue) const;

    /** Computes the progress value from the mouse position (clamped)
    * @param [in] ptMouse Coordinates inside the control
    */
    double ValueFromMousePt(const UiPoint& ptMouse) const;

    /** Clamps the low value into [min, high - min_range]
    */
    void ClampLowValue();

private:
    /** Thumb identifier
    */
    enum ThumbPart : uint8_t
    {
        kThumbNone = 0, //None
        kThumbLow,      //Low thumb
        kThumbHigh      //High thumb
    };

    /** Fires the range value change event (not fired when m_bMuteRangeEvent is true)
    */
    void EmitRangeChanged();

    /** Paints one thumb
    * @param [in] pRender Render interface
    * @param [in] part Thumb to paint (low/high)
    */
    void PaintThumb(IRender* pRender, ThumbPart part);

    /** Low value
    */
    double m_fLowValue;

    /** Minimum distance between the two thumbs
    */
    double m_fMinRange;

    /** Thumb size (raw value before scaling)
    */
    UiSize m_szThumb;

    /** Image of the low thumb
    */
    StateImage m_thumbStateImage;

    /** Image of the high thumb (falls back to the low thumb image when empty)
    */
    StateImage m_highThumbStateImage;

    /** Color of the highlighted interval
    */
    std::string m_strRangeColor;

    /** Corner radius of the highlighted interval (raw value before scaling)
    */
    int32_t m_nRangeRadius;

    /** Whether pressing the track jumps the value immediately (default true)
    */
    bool m_bClickToJump;

    /** Whether range value change events are suppressed (true while SetXXXNoEvent is running)
    */
    bool m_bMuteRangeEvent;

    /** Whether the high value has been set before (until then the low value is not constrained by the minimum distance, so it can be assigned freely during initialization)
    */
    bool m_bHighValueSet;

    /** Thumb currently being dragged
    */
    ThumbPart m_dragThumb;

    /** Thumb the mouse is currently hovering over
    */
    ThumbPart m_hoverThumb;

    /** Cache of the thumb image painting parameters
    */
    UiString m_sImageModify;
};

} // namespace ui

#endif // UI_CONTROL_RANGE_SLIDER_H_
