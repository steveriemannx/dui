#include "dui/Core/BoxShadow.h"
#include "dui/Core/Control.h"
#include "dui/Core/DpiManager.h"
#include "dui/Utils/AttributeUtil.h"
#include <algorithm>

namespace ui {

BoxShadow::BoxShadow(Control* pControl):
    m_cpOffset(0, 0),
    m_nBlurRadius(2),
    m_nSpreadRadius(2),
    m_pControl(pControl)
{
    ASSERT(m_pControl != nullptr);
    if (m_pControl != nullptr) {
        m_pControl->Dpi().ScaleInt(m_nBlurRadius);
        m_pControl->Dpi().ScaleInt(m_nSpreadRadius);
    }
}

void BoxShadow::SetBoxShadowString(const std::string& strBoxShadow)
{
    // Format like: "color='black' offset='1,1' blur_radius='2' spread_radius='2'"
    std::vector<std::pair<std::string, std::string>> attributeList;
    AttributeUtil::ParseAttributeList(strBoxShadow, '\'', attributeList);
    for (const auto& attribute : attributeList) {
        const std::string& name = attribute.first;
        const std::string& value = attribute.second;
        if (name.empty() || value.empty()) {
            continue;
        }
        if (name == "color") {
            m_strColor = value;
        }
        else if (name == "offset") {
            AttributeUtil::ParsePointValue(value.c_str(), m_cpOffset);
            ASSERT(m_pControl != nullptr);
            if (m_pControl != nullptr) {
                m_pControl->Dpi().ScalePoint(m_cpOffset);
            }
        }
        else if ((name == "blur_radius") || name == "blurradius") {
            m_nBlurRadius = StringUtil::StringToInt32(value);
            ASSERT(m_nBlurRadius >= 0);
            if (m_nBlurRadius < 0) {
                m_nBlurRadius = 0;
            }
            ASSERT(m_pControl != nullptr);
            if (m_pControl != nullptr) {
                m_pControl->Dpi().ScaleInt(m_nBlurRadius);
            }
        }
        else if ((name == "spread_radius") || (name == "spreadradius")) {
            m_nSpreadRadius = StringUtil::StringToInt32(value);
            ASSERT(m_nSpreadRadius >= 0);
            if (m_nSpreadRadius < 0) {
                m_nSpreadRadius = 0;
            }
            ASSERT(m_pControl != nullptr);
            if (m_pControl != nullptr) {
                m_pControl->Dpi().ScaleInt(m_nSpreadRadius);
            }
        }
        else {
            ASSERT(!"BoxShadow::SetBoxShadowString found unknown item name!");
        }
    }
}

bool BoxShadow::HasShadow() const
{
    return !m_strColor.empty() && ((m_nBlurRadius > 0) || (m_nSpreadRadius > 0));
}

UiRect BoxShadow::GetExpandedRect(const UiRect& controlRect) const
{
    int32_t expand = std::max(0, m_nBlurRadius + m_nSpreadRadius);
    UiRect extendedRect(controlRect.left   - expand + std::min(0, m_cpOffset.x),
                        controlRect.top    - expand + std::min(0, m_cpOffset.y),
                        controlRect.right  + expand + std::max(0, m_cpOffset.x),
                        controlRect.bottom + expand + std::max(0, m_cpOffset.y));
    return extendedRect;
}

} // namespace ui
