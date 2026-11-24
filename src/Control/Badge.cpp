#include "dui/Control/Badge.h"
#include "dui/Utils/StringUtil.h"

namespace ui
{

Badge::Badge(Window* pWindow) :
    BaseClass(pWindow),
    m_nCount(0),
    m_nMaxCount(99),
    m_bDotMode(false),
    m_strBadgeColor("bg_badge")
{
}

Badge::~Badge()
{
}

std::string Badge::GetType() const
{
    return DUI_CTR_BADGE;
}

void Badge::SetAttribute(const std::string& strName, const std::string& strValue)
{
    if ((strName == "count") || (strName == "badge_count")) {
        SetCount(StringUtil::StringToInt64(strValue));
    }
    else if ((strName == "max_count") || (strName == "maxcount")) {
        SetMaxCount(StringUtil::StringToInt64(strValue));
    }
    else if (strName == "dot") {
        SetDotMode(strValue == "true");
    }
    else if ((strName == "badge_color") || (strName == "badgecolor")) {
        SetBadgeColor(strValue);
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

void Badge::SetCount(int64_t nCount)
{
    if (m_nCount == nCount) {
        return;
    }
    m_nCount = nCount;
    UpdateBadge();
}

void Badge::SetMaxCount(int64_t nMaxCount)
{
    if (nMaxCount < 1) {
        nMaxCount = 1;
    }
    if (m_nMaxCount == nMaxCount) {
        return;
    }
    m_nMaxCount = nMaxCount;
    UpdateBadge();
}

void Badge::SetDotMode(bool bDotMode)
{
    if (m_bDotMode == bDotMode) {
        return;
    }
    m_bDotMode = bDotMode;
    UpdateBadge();
}

void Badge::SetBadgeColor(const std::string& strColor)
{
    m_strBadgeColor = strColor;
    Invalidate();
}

void Badge::UpdateBadge()
{
    if (m_bDotMode) {
        //Red-dot mode: no number is shown
        SetText("");
    }
    else if (m_nCount > m_nMaxCount) {
        SetText(StringUtil::Printf("%lld+", static_cast<long long>(m_nMaxCount)));
    }
    else {
        SetText(StringUtil::Printf("%lld", static_cast<long long>(m_nCount)));
    }
    //Numeric mode: hide the badge automatically when the count is less than or equal to 0 (no badge when there is nothing unread); in dot mode the visibility is controlled by the visible attribute
    SetVisible(m_bDotMode || (m_nCount > 0));
    Invalidate();
}

void Badge::PaintStateColors(IRender* pRender)
{
    if (pRender == nullptr) {
        return;
    }
    UiRect rc = GetRect();
    UiPadding rcPadding = GetControlPadding();
    rc.Deflate(rcPadding);
    if ((rc.Width() <= 0) || (rc.Height() <= 0)) {
        return;
    }
    //Rounded capsule background (in dot mode the width equals the height, so it is a circle)
    const float fRound = (float)rc.Height() / 2.0f;
    pRender->FillRoundRect(UiRectF((float)rc.left, (float)rc.top, (float)rc.right, (float)rc.bottom),
                           fRound, fRound, GetUiColor(m_strBadgeColor), GetAlpha());
}

} // namespace ui
