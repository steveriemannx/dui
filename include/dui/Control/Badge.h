#ifndef UI_CONTROL_BADGE_H_
#define UI_CONTROL_BADGE_H_

#include "dui/Control/Label.h"

namespace ui {

/** Badge control (derived from Label)
 *  Used to show an unread count, the number of new messages or a small red dot on TabCtrl tabs, buttons, icons and so on.
 *  Two forms:
 *  1. Numeric badge: rounded capsule background plus a number; when the count exceeds max_count, "max+" is shown (for example "99+");
 *  2. Red-dot badge (dot mode): a small dot without any number.
 *  A numeric badge hides itself automatically when count <= 0 (the usual convention: no badge when there is nothing unread);
 *  a red-dot badge is not affected by count and its visibility is controlled by the visible attribute.
 *  It is used as a child control inside any Box container; for a floating position combine float="true" with margin.
 *  The background is painted with a semantic color (corner radius = half of the height), so it adapts to light and dark themes.
 */
class DUI_API Badge : public Label
{
    typedef Label BaseClass;
public:
    explicit Badge(Window* pWindow);
    Badge(const Badge& r) = delete;
    Badge& operator=(const Badge& r) = delete;
    ~Badge() override;

    /** Gets the control type
    */
    virtual std::string GetType() const override;

    /** Sets an XML attribute
    */
    virtual void SetAttribute(const std::string& strName, const std::string& strValue) override;

    /** Paints the state colors of the control (the rounded capsule/dot background of the badge)
    */
    virtual void PaintStateColors(IRender* pRender) override;

public:
    /** Sets the badge count: in numeric mode the badge is shown when it is greater than 0 and hidden when it is less than or equal to 0 (dot mode is unaffected)
    */
    void SetCount(int64_t nCount);

    /** Gets the badge count
    */
    int64_t GetCount() const { return m_nCount; }

    /** Sets the count limit: above it "limit+" is shown (for example "99+"); default 99
    */
    void SetMaxCount(int64_t nMaxCount);

    /** Gets the count limit
    */
    int64_t GetMaxCount() const { return m_nMaxCount; }

    /** Sets whether the badge is in red-dot mode: true is a plain small dot without any number
    */
    void SetDotMode(bool bDotMode);

    /** Gets whether the badge is in red-dot mode
    */
    bool IsDotMode() const { return m_bDotMode; }

    /** Sets the badge background color (a semantic color name or a color value; the default is the semantic color bg_badge)
    */
    void SetBadgeColor(const std::string& strColor);

    /** Gets the badge background color
    */
    std::string GetBadgeColor() const { return m_strBadgeColor; }

private:
    /** Updates the display text and the visibility from the current count/red-dot mode
    */
    void UpdateBadge();

private:
    /** Badge count (in numeric mode a value less than or equal to 0 hides the badge; dot mode is unaffected)
    */
    int64_t m_nCount;

    /** Count limit; above it "limit+" is shown (for example "99+")
    */
    int64_t m_nMaxCount;

    /** Whether the badge is in red-dot mode (a plain small dot without any number)
    */
    bool m_bDotMode;

    /** Badge background color (a semantic color name or a color value)
    */
    std::string m_strBadgeColor;
};

} // namespace ui

#endif // UI_CONTROL_BADGE_H_
