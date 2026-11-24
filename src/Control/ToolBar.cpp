#include "dui/Control/ToolBar.h"
#include "dui/Control/Button.h"
#include "dui/Control/Flyout.h"
#include "dui/Core/DpiManager.h"
#include "dui/Layout/Layout.h"
#include "dui/Utils/StringUtil.h"
#include <algorithm>

namespace ui
{

namespace
{
/** Escapes the XML special characters of the text, used to generate the XML text of the flyout dynamically
*/
std::string EscapeXmlText(const std::string& strText)
{
    std::string strResult;
    strResult.reserve(strText.size() + 16);
    for (std::string::value_type ch : strText) {
        switch (ch) {
        case '&':  strResult += "&amp;";  break;
        case '<':  strResult += "&lt;";   break;
        case '>':  strResult += "&gt;";   break;
        case '\"': strResult += "&quot;"; break;
        case '\'': strResult += "&apos;"; break;
        default:       strResult += ch;           break;
        }
    }
    return strResult;
}

//Fixed size of the flyout list items (DPI logical values, scaled automatically by the XML)
constexpr int32_t kOverflowFlyoutWidthDip = 180;
constexpr int32_t kOverflowFlyoutItemHeightDip = 30;
constexpr int32_t kOverflowFlyoutPaddingDip = 4;
} // namespace

////////////////////////////////////////////////////////////////////////////////

ToolBar::ToolBar(Window* pWindow) :
    HBox(pWindow),
    m_bOverflowEnabled(true),
    m_nOverflowButtonWidth(0),
    m_overflowButtonText(">>"),
    m_bInLayout(false)
{
}

ToolBar::~ToolBar() = default;

std::string ToolBar::GetType() const { return DUI_CTR_TOOL_BAR; }

void ToolBar::SetAttribute(const std::string& strName, const std::string& strValue2)
{
    //dui does not support variable expansion in attribute values, so the value is used as is
    const std::string& strValue = strValue2;
    if ((strName == "overflow") || (strName == "overflow_enabled") || (strName == "overflowenabled")) {
        //dui has no StringUtil::IsValueTrue; booleans are compared against "true", as in the other dui controls
        SetOverflowEnabled(strValue == "true");
    }
    else if ((strName == "overflow_button_class") || (strName == "overflowbuttonclass")) {
        SetOverflowButtonClass(strValue);
    }
    else if ((strName == "overflow_button_width") || (strName == "overflowbuttonwidth")) {
        SetOverflowButtonWidth(StringUtil::StringToInt32(strValue));
    }
    else if ((strName == "overflow_button_text") || (strName == "overflowbuttontext")) {
        SetOverflowButtonText(strValue);
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

void ToolBar::AttachOverflowItemActivated(OverflowItemActivatedEvent callback)
{
    if (callback != nullptr) {
        m_callbackList.push_back(callback);
    }
}

void ToolBar::ClearOverflowItemActivated()
{
    m_callbackList.clear();
}

void ToolBar::SetOverflowEnabled(bool bEnabled)
{
    if (m_bOverflowEnabled != bEnabled) {
        m_bOverflowEnabled = bEnabled;
        if (!bEnabled) {
            //When the overflow handling is turned off, all items hidden automatically are shown again
            for (Control* pItem : m_hiddenItems) {
                if (IsChildControl(pItem)) {
                    pItem->SetVisible(true);
                }
            }
            m_hiddenItems.clear();
            Control* pButton = m_pOverflowButton.get();
            if ((pButton != nullptr) && pButton->IsVisible()) {
                pButton->SetVisible(false);
            }
        }
        Invalidate();
    }
}

bool ToolBar::IsOverflowEnabled() const
{
    return m_bOverflowEnabled;
}

void ToolBar::SetOverflowButtonClass(const std::string& strClass)
{
    if (m_overflowButtonClass != strClass) {
        m_overflowButtonClass = strClass;
        Control* pButton = m_pOverflowButton.get();
        if (pButton != nullptr) {
            pButton->SetClass(m_overflowButtonClass.c_str());
        }
        Invalidate();
    }
}

std::string ToolBar::GetOverflowButtonClass() const
{
    return m_overflowButtonClass.c_str();
}

void ToolBar::SetOverflowButtonWidth(int32_t nWidth)
{
    if (m_nOverflowButtonWidth != nWidth) {
        m_nOverflowButtonWidth = nWidth;
        Control* pButton = m_pOverflowButton.get();
        if (pButton != nullptr) {
            if (m_nOverflowButtonWidth > 0) {
                pButton->SetFixedWidth(UiFixedInt(m_nOverflowButtonWidth), true, true);
            }
            else {
                pButton->SetFixedWidth(UiFixedInt::MakeAuto(), true, false);
            }
        }
        Invalidate();
    }
}

int32_t ToolBar::GetOverflowButtonWidth() const
{
    return m_nOverflowButtonWidth;
}

void ToolBar::SetOverflowButtonText(const std::string& strText)
{
    if (m_overflowButtonText != strText) {
        m_overflowButtonText = strText;
        Label* pButton = dynamic_cast<Label*>(m_pOverflowButton.get());
        if (pButton != nullptr) {
            pButton->SetText(m_overflowButtonText.c_str());
        }
        Invalidate();
    }
}

std::string ToolBar::GetOverflowButtonText() const
{
    return m_overflowButtonText.c_str();
}

bool ToolBar::IsOverflowing() const
{
    return !m_hiddenItems.empty();
}

bool ToolBar::IsChildControl(Control* pControl) const
{
    if (pControl == nullptr) {
        return false;
    }
    const size_t nCount = GetItemCount();
    for (size_t nIndex = 0; nIndex < nCount; ++nIndex) {
        if (GetItemAt(nIndex) == pControl) {
            return true;
        }
    }
    return false;
}

bool ToolBar::ContainsItem(const std::vector<Control*>& items, Control* pControl)
{
    for (Control* pItem : items) {
        if (pItem == pControl) {
            return true;
        }
    }
    return false;
}

void ToolBar::CreateOverflowButton()
{
    if (m_pOverflowButton != nullptr) {
        return;
    }
    Window* pWindow = GetWindow();
    if (pWindow == nullptr) {
        return;
    }
    Button* pButton = new Button(pWindow);
    pButton->SetText(m_overflowButtonText.c_str());
    if (!m_overflowButtonClass.empty()) {
        pButton->SetClass(m_overflowButtonClass.c_str());
    }
    if (m_nOverflowButtonWidth > 0) {
        pButton->SetFixedWidth(UiFixedInt(m_nOverflowButtonWidth), false, true);
    }
    else {
        pButton->SetFixedWidth(UiFixedInt::MakeAuto(), false, false);
    }
    pButton->SetVisible(false);
    pButton->AttachClick([this](const EventArgs& /*args*/) {
        ShowOverflowFlyout();
        return true;
        });
    if (AddItem(pButton)) {
        m_pOverflowButton = pButton;
    }
    else {
        delete pButton;
    }
}

void ToolBar::SetPos(UiRect rc)
{
    if (!m_bOverflowEnabled || m_bInLayout) {
        BaseClass::SetPos(rc);
        return;
    }
    if (m_pOverflowButton == nullptr) {
        CreateOverflowButton();
    }
    m_bInLayout = true;
    UpdateOverflow(rc);
    m_bInLayout = false;
}

void ToolBar::UpdateOverflow(const UiRect& rc)
{
    Control* pButton = m_pOverflowButton.get();
    if (pButton == nullptr) {
        //When the "more" button has not been created, lay out as an ordinary container
        BaseClass::SetPos(rc);
        return;
    }

    const UiPadding padding = GetPadding();
    const int32_t nContentLeft = rc.left + padding.left;
    const int32_t nContentRight = rc.right - padding.right;
    const int32_t nAvailable = nContentRight - nContentLeft;

    //Collect the child controls that take part in the layout (excluding the "more" button, floating controls and the
    //controls hidden by the application)
    std::vector<Control*> items;
    bool bHasStretch = false;
    if (nAvailable > 0) {
        const size_t nCount = GetItemCount();
        for (size_t nIndex = 0; nIndex < nCount; ++nIndex) {
            Control* pItem = GetItemAt(nIndex);
            if ((pItem == nullptr) || (pItem == pButton) || pItem->IsFloat()) {
                continue;
            }
            const bool bAutoHidden = ContainsItem(m_hiddenItems, pItem);
            if (!pItem->IsVisible() && !bAutoHidden) {
                //Controls hidden by the application do not take part in the overflow handling
                continue;
            }
            if (pItem->EstimateSize(UiSize(nAvailable, rc.Height())).cx.IsStretch()) {
                //No overflow handling when a stretch child control exists
                bHasStretch = true;
                break;
            }
            items.push_back(pItem);
        }
    }

    //Compute the items that should be collected (accumulate the widths from the first item on and start collecting once
    //the available width is exceeded)
    std::vector<Control*> desiredHidden;
    if ((nAvailable > 0) && !bHasStretch && !items.empty()) {
        //Width taken by the "more" button
        int32_t nButtonWidth = 0;
        if (m_nOverflowButtonWidth > 0) {
            nButtonWidth = Dpi().GetScaleInt(m_nOverflowButtonWidth);
        }
        else {
            UiEstSize estButton = pButton->EstimateSize(UiSize(nAvailable, rc.Height()));
            nButtonWidth = std::max(estButton.cx.GetInt32(), 0);
        }

        const int32_t nLimit = nContentRight - nButtonWidth;
        const int32_t nMarginX = (GetLayout() != nullptr) ? GetLayout()->GetChildMarginX() : 0;
        int32_t nPosX = nContentLeft;
        bool bFirstItem = true;
        for (size_t nIndex = 0; nIndex < items.size(); ++nIndex) {
            Control* pItem = items[nIndex];
            if (!bFirstItem) {
                nPosX += nMarginX;
            }
            bFirstItem = false;
            const UiMargin margin = pItem->GetMargin();
            nPosX += margin.left;
            UiEstSize estItem = pItem->EstimateSize(UiSize(nAvailable, rc.Height()));
            int32_t nItemWidth = std::clamp(estItem.cx.GetInt32(), pItem->GetMinWidth(), pItem->GetMaxWidth());
            nItemWidth = std::max(nItemWidth, 0);
            if ((nPosX + nItemWidth) > nLimit) {
                desiredHidden.assign(items.begin() + nIndex, items.end());
                break;
            }
            nPosX += nItemWidth + margin.right;
        }
    }
    const bool bNeedOverflow = !desiredHidden.empty();

    //Change the visibility of the child controls only when the collected state changed, so the layout is not triggered
    //over and over again
    bool bChanged = (desiredHidden.size() != m_hiddenItems.size());
    if (!bChanged) {
        for (size_t nIndex = 0; nIndex < desiredHidden.size(); ++nIndex) {
            if (desiredHidden[nIndex] != m_hiddenItems[nIndex]) {
                bChanged = true;
                break;
            }
        }
    }
    if (bChanged) {
        for (Control* pItem : m_hiddenItems) {
            if (!ContainsItem(desiredHidden, pItem) && IsChildControl(pItem)) {
                pItem->SetVisible(true);
            }
        }
        for (Control* pItem : desiredHidden) {
            if (!ContainsItem(m_hiddenItems, pItem)) {
                pItem->SetVisible(false);
            }
        }
        m_hiddenItems = desiredHidden;
    }

    if (pButton->IsVisible() != bNeedOverflow) {
        pButton->SetVisible(bNeedOverflow);
    }

    //Apply the layout
    BaseClass::SetPos(rc);

    if (bNeedOverflow) {
        //Align the "more" button to the right end of the content area
        UiRect rcButton = pButton->GetRect();
        int32_t nWidth = rcButton.Width();
        if (nWidth <= 0) {
            nWidth = Dpi().GetScaleInt(24);
        }
        rcButton.right = nContentRight;
        rcButton.left = std::max(nContentRight - nWidth, nContentLeft);
        pButton->SetPos(rcButton);
    }
}

void ToolBar::ShowOverflowFlyout()
{
    if (m_hiddenItems.empty()) {
        return;
    }
    Window* pWindow = GetWindow();
    Control* pButton = m_pOverflowButton.get();
    if ((pWindow == nullptr) || (pButton == nullptr)) {
        return;
    }

    //Clicking the same anchor again closes the flyout that is already open
    Flyout* pActive = Flyout::GetActiveFlyout();
    if ((pActive != nullptr) && (pActive->GetAnchor() == pButton)) {
        pActive->Dismiss();
        return;
    }

    //Collect the names and display texts of the collected items
    std::vector<std::string> itemNames;
    std::vector<std::string> itemTexts;
    for (Control* pItem : m_hiddenItems) {
        if (!IsChildControl(pItem)) {
            continue;
        }
        std::string strText;
        Label* pLabel = dynamic_cast<Label*>(pItem);
        if (pLabel != nullptr) {
            strText = pLabel->GetText();
        }
        if (strText.empty()) {
            strText = pItem->GetToolTipText();
        }
        if (strText.empty()) {
            strText = pItem->GetName();
        }
        itemNames.push_back(pItem->GetName().c_str());
        itemTexts.push_back(strText.c_str());
    }
    if (itemNames.empty()) {
        return;
    }

    //Generate the flyout content dynamically (XML text)
    const int32_t nWidthDip = kOverflowFlyoutWidthDip;
    const int32_t nItemHeightDip = kOverflowFlyoutItemHeightDip;
    const int32_t nPaddingDip = kOverflowFlyoutPaddingDip;
    const int32_t nHeightDip = (int32_t)itemTexts.size() * nItemHeightDip + nPaddingDip * 2;

    std::string xml;
    xml += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    xml += "<Window size=\"";
    xml += StringUtil::Int32ToString(nWidthDip);
    xml += ",";
    xml += StringUtil::Int32ToString(nHeightDip);
    xml += "\" caption=\"0,0,0,0\" use_system_caption=\"false\"";
    xml += " shadow_type=\"default\" shadow_attached=\"true\"";
    xml += " layered_window=\"true\" size_box=\"0,0,0,0\"";
    xml += " icon=\"public/caption/logo.ico\">";
    xml += "<VBox bkcolor=\"bg_menu\" width=\"";
    xml += StringUtil::Int32ToString(nWidthDip);
    xml += "\" height=\"auto\" padding=\"";
    xml += StringUtil::Int32ToString(nPaddingDip);
    xml += ",";
    xml += StringUtil::Int32ToString(nPaddingDip);
    xml += ",";
    xml += StringUtil::Int32ToString(nPaddingDip);
    xml += ",";
    xml += StringUtil::Int32ToString(nPaddingDip);
    xml += "\">";
    for (size_t nIndex = 0; nIndex < itemTexts.size(); ++nIndex) {
        xml += "<Button name=\"ovf_";
        xml += StringUtil::Int32ToString((int32_t)nIndex);
        xml += "\" text=\"";
        xml += EscapeXmlText(itemTexts[nIndex]);
        xml += "\" width=\"stretch\" height=\"";
        xml += StringUtil::Int32ToString(nItemHeightDip);
        xml += "\" text_align=\"left,vcenter\" text_padding=\"12,0,0,0\"";
        xml += " normal_text_color=\"text_default\" hot_text_color=\"color_accent\"";
        xml += " cursor_type=\"hand\"/>";
    }
    xml += "</VBox></Window>";

    Flyout* pFlyout = new Flyout(pWindow);
    pFlyout->SetSkinFolder(pWindow->GetResourcePath().ToString());
    if (!pFlyout->ShowAt(pButton, xml, Flyout::Placement::BottomEnd)) {
        //When the creation fails, ShowAt has already destroyed the object
        return;
    }

    //Bind the click event of every list item
    std::weak_ptr<WeakFlag> toolbarFlag = GetWeakFlag();
    for (size_t nIndex = 0; nIndex < itemNames.size(); ++nIndex) {
        std::string strItemControlName = "ovf_" + StringUtil::Int32ToString((int32_t)nIndex);
        Button* pItemButton = dynamic_cast<Button*>(pFlyout->FindControl(strItemControlName));
        if (pItemButton == nullptr) {
            continue;
        }
        std::string strItemName = itemNames[nIndex];
        pItemButton->AttachClick([this, toolbarFlag, strItemName, pFlyout](const EventArgs& /*args*/) {
            if (!toolbarFlag.expired()) {
                FireOverflowItemActivated(strItemName);
            }
            pFlyout->Dismiss();
            return true;
            });
    }
}

void ToolBar::FireOverflowItemActivated(const std::string& itemName)
{
    std::vector<OverflowItemActivatedEvent> callbackList(m_callbackList);
    for (const OverflowItemActivatedEvent& callback : callbackList) {
        if (callback != nullptr) {
            callback(itemName);
        }
    }
}

} // namespace ui
