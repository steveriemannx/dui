/* Added in 2026-10: the NavigationView sidebar navigation control (NavigationView + NavigationViewItem) */

#include "dui/Control/NavigationView.h"
#include "dui/Box/VBox.h"
#include "dui/Box/ScrollBox.h"
#include "dui/Box/TabBox.h"
#include "dui/Control/Button.h"
#include "dui/Control/Label.h"
#include "dui/Control/Line.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Render/IRender.h"
#include "dui/Utils/StringUtil.h"
#include <algorithm>

namespace ui
{

// ============================================================================
// NavigationViewItem
// ============================================================================

NavigationViewItem::NavigationViewItem(Window* pWindow) :
    Control(pWindow),
    m_itemType(ItemType::kItem),
    m_bSelected(false),
    m_bHovered(false),
    m_bCompact(false),
    m_pOwner(nullptr),
    m_nIconSize(16),
    m_nIconLeft(16),
    m_nTextLeft(44),
    m_nTextRight(8),
    m_nPillRadius(6),
    m_nPillInsetX(6),
    m_nPillInsetY(3),
    m_selectedBgColor("bg_list_item_selected"),
    m_hoveredBgColor("bg_list_item_hovered"),
    m_selectedTextColor("color_accent"),
    m_normalTextColor("text_default"),
    m_disabledTextColor("text_disabled"),
    m_headerTextColor("text_muted"),
    m_separatorColor("border_window")
{
}

std::string NavigationViewItem::GetType() const
{
    return DUI_CTR_NAVIGATION_VIEW_ITEM;
}

void NavigationViewItem::OnInit()
{
    BaseClass::OnInit();
    // Different layouts use different fixed heights (DIP, DPI aware)
    switch (m_itemType) {
    case ItemType::kItem:
        SetFixedHeight(UiFixedInt(36), true, true);
        break;
    case ItemType::kHeader:
        SetFixedHeight(UiFixedInt(32), true, true);
        break;
    case ItemType::kSeparator:
        SetFixedHeight(UiFixedInt(9), true, true);
        break;
    default:
        break;
    }
}

void NavigationViewItem::SetAttribute(const std::string& strName, const std::string& strValue)
{
    if (strName == "text") {
        SetItemText(strValue);
    }
    else if (strName == "text_id") {
        SetItemTextId(strValue);
    }
    else if (strName == "icon") {
        SetIconAttr(strValue);
    }
    else if (strName == "page") {
        SetPageName(strValue);
    }
    else if (strName == "item_type") {
        if ((strValue == "header") || (strValue == "group_header")) {
            SetItemType(ItemType::kHeader);
        }
        else if ((strValue == "separator") || (strValue == "sep")) {
            SetItemType(ItemType::kSeparator);
        }
        else {
            SetItemType(ItemType::kItem);
        }
    }
    else if (strName == "icon_size") {
        int32_t nValue = StringUtil::StringToInt32(strValue);
        if (nValue > 0) {
            m_nIconSize = nValue;
        }
    }
    else if (strName == "icon_left") {
        m_nIconLeft = StringUtil::StringToInt32(strValue);
    }
    else if (strName == "text_left") {
        m_nTextLeft = StringUtil::StringToInt32(strValue);
    }
    else if (strName == "text_right") {
        m_nTextRight = StringUtil::StringToInt32(strValue);
    }
    else if (strName == "pill_radius") {
        m_nPillRadius = StringUtil::StringToInt32(strValue);
    }
    else if (strName == "pill_inset_x") {
        m_nPillInsetX = StringUtil::StringToInt32(strValue);
    }
    else if (strName == "pill_inset_y") {
        m_nPillInsetY = StringUtil::StringToInt32(strValue);
    }
    else if (strName == "selected_bg_color") {
        m_selectedBgColor = strValue;
    }
    else if (strName == "hovered_bg_color") {
        m_hoveredBgColor = strValue;
    }
    else if (strName == "selected_text_color") {
        m_selectedTextColor = strValue;
    }
    else if (strName == "normal_text_color") {
        m_normalTextColor = strValue;
    }
    else if (strName == "disabled_text_color") {
        m_disabledTextColor = strValue;
    }
    else if (strName == "header_text_color") {
        m_headerTextColor = strValue;
    }
    else if (strName == "separator_color") {
        m_separatorColor = strValue;
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

void NavigationViewItem::Paint(IRender* pRender, const UiRect& rcPaint)
{
    BaseClass::Paint(pRender, rcPaint);
    if (pRender == nullptr) {
        return;
    }
    switch (m_itemType) {
    case ItemType::kItem:
        PaintItem(pRender);
        break;
    case ItemType::kHeader:
        PaintHeader(pRender);
        break;
    case ItemType::kSeparator:
        PaintSeparator(pRender);
        break;
    default:
        break;
    }
}

void NavigationViewItem::PaintStateImages(IRender* /*pRender*/)
{
    // The foreground icon is positioned and painted by PaintItem according to the expanded/compact layout; suppress the default painting of the base class over the whole row, to avoid a duplicated or misplaced icon
}

bool NavigationViewItem::MouseEnter(const EventArgs& msg)
{
    bool bRet = BaseClass::MouseEnter(msg);
    // The hover area is not shown in the disabled state
    if (IsSelectable() && IsEnabled() && !m_bHovered) {
        m_bHovered = true;
        Invalidate();
    }
    return bRet;
}

bool NavigationViewItem::MouseLeave(const EventArgs& msg)
{
    bool bRet = BaseClass::MouseLeave(msg);
    if (m_bHovered) {
        m_bHovered = false;
        Invalidate();
    }
    return bRet;
}

bool NavigationViewItem::ButtonUp(const EventArgs& msg)
{
    bool bRet = BaseClass::ButtonUp(msg);
    // Selecting is not triggered in the disabled state, to avoid accidental operations
    if (IsSelectable() && IsEnabled() && (msg.GetSender() == this)) {
        if (m_pOwner != nullptr) {
            m_pOwner->OnItemClicked(this);
        }
    }
    return bRet;
}

void NavigationViewItem::OnLanguageChanged()
{
    BaseClass::OnLanguageChanged();
    Invalidate();
}

uint32_t NavigationViewItem::GetControlFlags() const
{
    // When selectable and enabled, allow Tab focus (used for keyboard navigation)
    return (IsSelectable() && IsEnabled() && IsKeyboardEnabled()) ? UIFLAG_TABSTOP : UIFLAG_DEFAULT;
}

void NavigationViewItem::HandleEvent(const EventArgs& msg)
{
    if ((msg.eventType == kEventKeyDown) && IsSelectable() && IsEnabled()) {
        if ((msg.vkCode == kVK_RETURN) || (msg.vkCode == kVK_SPACE)) {
            // Enter/Space: activate the current item (equivalent to a mouse click)
            if (m_pOwner != nullptr) {
                m_pOwner->OnItemClicked(this);
            }
            return;
        }
        if ((msg.vkCode == kVK_UP) || (msg.vkCode == kVK_DOWN)) {
            // Up/Down arrow keys: move the focus between the selectable items
            if (m_pOwner != nullptr) {
                m_pOwner->MoveFocusByKey(this, msg.vkCode == kVK_DOWN);
            }
            return;
        }
    }
    BaseClass::HandleEvent(msg);
}

void NavigationViewItem::SetItemText(const std::string& strText)
{
    m_text = strText;
    // In the compact state, show the full text as a tooltip
    if (m_bCompact && IsSelectable()) {
        SetToolTipText(GetDisplayText());
    }
    Invalidate();
}

std::string NavigationViewItem::GetItemText() const
{
    return m_text.c_str();
}

void NavigationViewItem::SetItemTextId(const std::string& strTextId)
{
    m_textId = strTextId;
    if (m_bCompact && IsSelectable()) {
        SetToolTipText(GetDisplayText());
    }
    Invalidate();
}

std::string NavigationViewItem::GetItemTextId() const
{
    return m_textId.c_str();
}

std::string NavigationViewItem::GetDisplayText() const
{
    if (!m_textId.empty()) {
        std::string strText = GlobalManager::Instance().Lang().GetStringViaID(m_textId.c_str());
        if (!strText.empty()) {
            return strText;
        }
    }
    return m_text.c_str();
}

void NavigationViewItem::SetIconAttr(const std::string& strIcon)
{
    m_iconAttr = strIcon;
    if (!strIcon.empty()) {
        SetForeStateImage(kControlStateNormal, strIcon);
    }
}

std::string NavigationViewItem::GetIconAttr() const
{
    return m_iconAttr.c_str();
}

void NavigationViewItem::SetPageName(const std::string& strPage)
{
    m_pageName = strPage;
}

std::string NavigationViewItem::GetPageName() const
{
    return m_pageName.c_str();
}

void NavigationViewItem::SetItemType(ItemType itemType)
{
    m_itemType = itemType;
}

void NavigationViewItem::SetSelected(bool bSelected)
{
    if (m_bSelected == bSelected) {
        return;
    }
    m_bSelected = bSelected;
    Invalidate();
}

void NavigationViewItem::SetCompact(bool bCompact)
{
    if (m_bCompact == bCompact) {
        return;
    }
    m_bCompact = bCompact;
    if (IsSelectable()) {
        SetToolTipText(bCompact ? GetDisplayText() : std::string());
    }
    else if (m_itemType != ItemType::kItem) {
        // In the compact state, group titles and separators are not painted, and their placeholder height is collapsed as well
        SetFixedHeight(UiFixedInt(bCompact ? 0 : (m_itemType == ItemType::kHeader ? 32 : 9)), true, true);
    }
    Invalidate();
}

void NavigationViewItem::PaintItem(IRender* pRender)
{
    UiRect rc = GetRect();
    if (rc.IsEmpty()) {
        return;
    }

    // Selected/hovered background (rounded hot area, the inset can be customized)
    UiColor bgColor;
    if (m_bSelected) {
        bgColor = GetUiColor(m_selectedBgColor.c_str());
    }
    else if (m_bHovered && IsEnabled()) {
        bgColor = GetUiColor(m_hoveredBgColor.c_str());
    }
    const int32_t nInsetX = Dpi().GetScaleInt(m_nPillInsetX);
    const int32_t nInsetY = Dpi().GetScaleInt(m_nPillInsetY);
    const float fRadius = Dpi().GetScaleFloat(static_cast<float>(m_nPillRadius));
    if (!bgColor.IsEmpty()) {
        UiRect rcPill = rc;
        rcPill.Deflate(nInsetX, nInsetY);
        UiRectF rcPillF;
        rcPillF.left = static_cast<float>(rcPill.left);
        rcPillF.top = static_cast<float>(rcPill.top);
        rcPillF.right = static_cast<float>(rcPill.right);
        rcPillF.bottom = static_cast<float>(rcPill.bottom);
        pRender->FillRoundRect(rcPillF, fRadius, fRadius, bgColor);
    }

    // Keyboard focus indicator: paint an accent-colored stroke along the outer edge of the hot area (focus state only)
    if (IsFocused() && IsEnabled()) {
        UiColor focusColor = GetUiColor("color_accent");
        if (!focusColor.IsEmpty()) {
            UiRect rcRing = rc;
            rcRing.Deflate(nInsetX, nInsetY);
            UiRectF rcRingF;
            rcRingF.left = static_cast<float>(rcRing.left);
            rcRingF.top = static_cast<float>(rcRing.top);
            rcRingF.right = static_cast<float>(rcRing.right);
            rcRingF.bottom = static_cast<float>(rcRing.bottom);
            const float fStroke = Dpi().GetScaleFloat(1.0f);
            pRender->DrawRoundRect(rcRingF, fRadius, fRadius, focusColor, fStroke);
        }
    }

    const int32_t nIconSize = Dpi().GetScaleInt(m_nIconSize);
    UiRect rcIcon;
    rcIcon.top = rc.CenterY() - nIconSize / 2;
    rcIcon.bottom = rcIcon.top + nIconSize;
    if (m_bCompact) {
        // Compact state: the icon is horizontally centered in the pane
        rcIcon.left = rc.CenterX() - nIconSize / 2;
        rcIcon.right = rcIcon.left + nIconSize;
    }
    else {
        // Expanded state: the icon left margin can be customized
        rcIcon.left = rc.left + Dpi().GetScaleInt(m_nIconLeft);
        rcIcon.right = rcIcon.left + nIconSize;
    }

    // Paint the icon (the disabled state uses the disabled image, falling back to the normal image when it is missing)
    const ControlStateType iconState = IsEnabled() ? kControlStateNormal : kControlStateDisabled;
    PaintStateImage(pRender, kStateImageFore, iconState, "", &rcIcon);

    // Paint the text in the expanded state; the compact state shows only the icon
    if (!m_bCompact) {
        std::string strText = GetDisplayText();
        if (!strText.empty()) {
            UiRect rcText = rc;
            rcText.left = rc.left + Dpi().GetScaleInt(m_nTextLeft);
            rcText.right = rc.right - Dpi().GetScaleInt(m_nTextRight);
            UiColor textColor;
            if (!IsEnabled()) {
                // Disabled state: muted color
                textColor = GetUiColor(m_disabledTextColor.c_str());
            }
            if (textColor.IsEmpty()) {
                textColor = m_bSelected ? GetUiColor(m_selectedTextColor.c_str())
                                        : GetUiColor(m_normalTextColor.c_str());
            }
            if (textColor.IsEmpty()) {
                textColor = UiColor(0xFF000000);
            }
            IFont* pFont = GetIFontById("system_regular_14");
            DrawStringParam param;
            param.textRect = rcText;
            param.dwTextColor = textColor;
            param.pFont = pFont;
            param.uFormat = TEXT_LEFT | TEXT_VCENTER | TEXT_SINGLELINE | TEXT_END_ELLIPSIS;
            pRender->DrawString(strText, param);
        }
    }
}

void NavigationViewItem::PaintHeader(IRender* pRender)
{
    // Group titles do not respond to clicks and do not paint a hot area
    UiRect rc = GetRect();
    if (rc.IsEmpty() || m_bCompact) {
        // In the compact state the group title text is hidden (keeping the placeholder height would waste space, so it is simply not painted)
        return;
    }
    std::string strText = GetDisplayText();
    if (strText.empty()) {
        return;
    }
    UiRect rcText = rc;
    rcText.left = rc.left + Dpi().GetScaleInt(16);
    rcText.right = rc.right - Dpi().GetScaleInt(8);
    UiColor textColor = GetUiColor(m_headerTextColor.c_str());
    if (textColor.IsEmpty()) {
        textColor = UiColor(0xFF808080);
    }
    IFont* pFont = GetIFontById("system_regular_12");
    DrawStringParam param;
    param.textRect = rcText;
    param.dwTextColor = textColor;
    param.pFont = pFont;
    param.uFormat = TEXT_LEFT | TEXT_VCENTER | TEXT_SINGLELINE | TEXT_END_ELLIPSIS;
    pRender->DrawString(strText, param);
}

void NavigationViewItem::PaintSeparator(IRender* pRender)
{
    UiRect rc = GetRect();
    if (rc.IsEmpty() || m_bCompact) {
        return;
    }
    UiColor lineColor = GetUiColor(m_separatorColor.c_str());
    if (lineColor.IsEmpty()) {
        lineColor = UiColor(0xFFD5D5D5);
    }
    const int32_t nMargin = Dpi().GetScaleInt(12);
    UiRectF rcLine;
    rcLine.left = static_cast<float>(rc.left + nMargin);
    rcLine.right = static_cast<float>(rc.right - nMargin);
    rcLine.top = static_cast<float>(rc.CenterY());
    rcLine.bottom = rcLine.top + 1.0f;
    pRender->FillRect(rcLine, lineColor);
}

// ============================================================================
// NavigationView
// ============================================================================

NavigationView::NavigationView(Window* pWindow) :
    HBox(pWindow),
    m_bInternalsReady(false),
    m_nPaneWidth(220),
    m_nCompactPaneWidth(48),
    m_bCollapsed(false),
    m_bShowHeader(true),
    m_bSettingsItem(false),
    m_pPane(nullptr),
    m_pTopBar(nullptr),
    m_pToggleBtn(nullptr),
    m_pPaneTitleLabel(nullptr),
    m_pItemHost(nullptr),
    m_pBottomHost(nullptr),
    m_pSettingsNavItem(nullptr),
    m_pSeparator(nullptr),
    m_pRight(nullptr),
    m_pHeader(nullptr),
    m_pContent(nullptr),
    m_pSelected(nullptr)
{
    m_settingsPage = "settings";
    // The text/icon of the settings item and the hamburger button icon are all provided by the user through attributes; the control itself ships no concrete resources
}

NavigationView::~NavigationView()
{
    // Child controls are owned by the parent container, which releases them in its destructor; only the weak references are cleared here
    m_pSelected = nullptr;
    m_pSettingsNavItem = nullptr;
    m_items.clear();
}

std::string NavigationView::GetType() const
{
    return DUI_CTR_NAVIGATIONVIEW;
}

void NavigationView::SetAttribute(const std::string& strName, const std::string& strValue)
{
    if (strName == "pane_width") {
        int32_t nValue = StringUtil::StringToInt32(strValue);
        if (nValue > 0) {
            m_nPaneWidth = nValue;
        }
    }
    else if (strName == "compact_pane_width") {
        int32_t nValue = StringUtil::StringToInt32(strValue);
        if (nValue > 0) {
            m_nCompactPaneWidth = nValue;
        }
    }
    else if (strName == "pane_title") {
        m_paneTitle = strValue;
        if (m_pPaneTitleLabel != nullptr) {
            m_pPaneTitleLabel->SetText(strValue);
        }
    }
    else if (strName == "pane_title_id") {
        m_paneTitleId = strValue;
        if (m_pPaneTitleLabel != nullptr) {
            m_pPaneTitleLabel->SetTextId(strValue);
        }
    }
    else if (strName == "toggle_icon") {
        m_toggleIcon = strValue;
        if ((m_pToggleBtn != nullptr) && !strValue.empty()) {
            m_pToggleBtn->SetStateImage(kControlStateNormal, strValue);
        }
    }
    else if (strName == "collapsed") {
        m_bCollapsed = (strValue == "true");
    }
    else if (strName == "show_header") {
        m_bShowHeader = (strValue == "true");
    }
    else if (strName == "settings_item") {
        m_bSettingsItem = (strValue == "true");
    }
    else if (strName == "settings_page") {
        m_settingsPage = strValue;
    }
    else if (strName == "settings_text_id") {
        m_settingsTextId = strValue;
    }
    else if (strName == "settings_icon") {
        m_settingsIcon = strValue;
    }
    else if (strName == "selected_id") {
        m_initSelectedId = strValue;
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

bool NavigationView::AddItem(Control* pControl)
{
    ASSERT(pControl != nullptr);
    if (pControl == nullptr) {
        return false;
    }
    // The internal structure is created before the first user child control is added
    EnsureInternals();

    NavigationViewItem* pItem = dynamic_cast<NavigationViewItem*>(pControl);
    if (pItem != nullptr) {
        // Navigation items (including group titles/separators) are added to the pane list
        RegisterNavItem(pItem);
        return m_pItemHost->AddItem(pItem);
    }

    TabBox* pTabBox = dynamic_cast<TabBox*>(pControl);
    if (pTabBox != nullptr) {
        // The user provides the content-area TabBox explicitly: take it over as the content container (only one is allowed)
        if (m_pContent != nullptr) {
            ASSERT(m_pContent == nullptr);
            return false;
        }
        m_pContent = pTabBox;
        m_pContent->SetFixedWidth(UiFixedInt::MakeStretch(), false, false);
        m_pContent->SetFixedHeight(UiFixedInt::MakeStretch(), false, false);
        return m_pRight->AddItem(pControl);
    }

    // Other controls become content pages in the content area; a TabBox is created automatically when none is provided explicitly
    EnsureContentHost();
    return m_pContent->AddItem(pControl);
}

void NavigationView::OnInit()
{
    BaseClass::OnInit();
    // Attributes may be set before the internal structure is created, so synchronize them once here
    if (!m_paneTitle.empty() && (m_pPaneTitleLabel != nullptr)) {
        m_pPaneTitleLabel->SetText(m_paneTitle.c_str());
    }
    if (!m_paneTitleId.empty() && (m_pPaneTitleLabel != nullptr)) {
        m_pPaneTitleLabel->SetTextId(m_paneTitleId.c_str());
    }

    // Bottom settings item (added to the fixed area at the bottom of the pane, so it stays at the bottom while the navigation list scrolls)
    if (m_bSettingsItem && (m_pSettingsNavItem == nullptr)) {
        m_pSettingsNavItem = new NavigationViewItem(GetWindow());
        m_pSettingsNavItem->SetPageName(m_settingsPage.c_str());
        m_pSettingsNavItem->SetItemTextId(m_settingsTextId.c_str());
        if (!m_settingsIcon.empty()) {
            m_pSettingsNavItem->SetIconAttr(m_settingsIcon.c_str());
        }
        // The settings item is registered as well (for click linkage and lookup), but IsIndexedItem excludes it from the index numbering
        RegisterNavItem(m_pSettingsNavItem);
        if (m_pBottomHost != nullptr) {
            m_pBottomHost->AddItem(m_pSettingsNavItem);
        }
        else {
            m_pPane->AddItem(m_pSettingsNavItem);
        }
    }

    ApplyCollapsed();
    ApplyHeaderVisible();
    DoInitialSelection();
}

void NavigationView::OnLanguageChanged()
{
    BaseClass::OnLanguageChanged();
    // The header text follows the language text of the currently selected item
    UpdateHeaderText(m_pSelected);
}

void NavigationView::EnsureInternals()
{
    if (m_bInternalsReady) {
        return;
    }
    m_bInternalsReady = true;
    Window* pWindow = GetWindow();

    // --- Left navigation pane ---
    m_pPane = new VBox(pWindow);
    m_pPane->SetBkColor("bg_window_card");
    m_pPane->SetFixedHeight(UiFixedInt::MakeStretch(), false, false);
    BaseClass::AddItem(m_pPane);

    // 1px vertical separator between the pane and the content area: it uses the dedicated area-separator semantic color with a solid line, soft and unobtrusive
    m_pSeparator = new Line(pWindow);
    m_pSeparator->SetAttribute("vertical", "true");
    m_pSeparator->SetAttribute("dash_style", "solid");
    m_pSeparator->SetAttribute("line_color", "border_split_level1");
    m_pSeparator->SetFixedWidth(UiFixedInt(1), true, true);
    m_pSeparator->SetFixedHeight(UiFixedInt::MakeStretch(), false, false);
    BaseClass::AddItem(m_pSeparator);

    // --- Right side: header + content ---
    m_pRight = new VBox(pWindow);
    m_pRight->SetBkColor("bg_window_main");
    m_pRight->SetFixedWidth(UiFixedInt::MakeStretch(), false, false);
    m_pRight->SetFixedHeight(UiFixedInt::MakeStretch(), false, false);
    BaseClass::AddItem(m_pRight);

    // Top of the pane: hamburger button + pane title
    m_pTopBar = new HBox(pWindow);
    m_pTopBar->SetBkColor("bg_window_card");
    m_pTopBar->SetFixedHeight(UiFixedInt(44), true, true);
    m_pPane->AddItem(m_pTopBar);

    m_pToggleBtn = new Button(pWindow);
    m_pToggleBtn->SetFixedWidth(UiFixedInt(48), true, true);
    m_pToggleBtn->SetFixedHeight(UiFixedInt(44), true, true);
    m_pToggleBtn->SetAttribute("border_size", "0");
    m_pToggleBtn->SetAttribute("normal_color", "bg_window_card");
    m_pToggleBtn->SetAttribute("hovered_color", "bg_list_item_hovered");
    m_pToggleBtn->SetAttribute("pressed_color", "bg_list_item_selected");
    if (!m_toggleIcon.empty()) {
        m_pToggleBtn->SetStateImage(kControlStateNormal, m_toggleIcon.c_str());
    }
    m_pToggleBtn->AttachClick([this](const EventArgs& /*msg*/) {
        OnToggleClicked();
        return true;
    });
    m_pTopBar->AddItem(m_pToggleBtn);

    m_pPaneTitleLabel = new Label(pWindow);
    m_pPaneTitleLabel->SetFixedWidth(UiFixedInt::MakeStretch(), false, false);
    m_pPaneTitleLabel->SetFixedHeight(UiFixedInt::MakeStretch(), false, false);
    m_pPaneTitleLabel->SetAttribute("font", "system_bold_14");
    m_pPaneTitleLabel->SetAttribute("text_color", "text_default");
    m_pPaneTitleLabel->SetAttribute("text_align", "vcenter");
    m_pPaneTitleLabel->SetAttribute("padding", "4,0,8,0");
    if (!m_paneTitleId.empty()) {
        m_pPaneTitleLabel->SetTextId(m_paneTitleId.c_str());
    }
    else {
        m_pPaneTitleLabel->SetText(m_paneTitle.c_str());
    }
    m_pTopBar->AddItem(m_pPaneTitleLabel);

    // Scrollable list of navigation items (stretched vertically to fill the remaining pane space)
    m_pItemHost = new VScrollBox(pWindow);
    m_pItemHost->SetFixedWidth(UiFixedInt::MakeStretch(), false, false);
    m_pItemHost->SetFixedHeight(UiFixedInt::MakeStretch(), false, false);
    m_pItemHost->SetAttribute("vscrollbar", "true"); // Support a vertical scroll bar
    m_pPane->AddItem(m_pItemHost);

    // Fixed area at the bottom of the pane (holds the settings item; the height is auto and it does not scroll with the navigation list, so it stays at the bottom even with many navigation items)
    m_pBottomHost = new VBox(pWindow);
    m_pBottomHost->SetFixedWidth(UiFixedInt::MakeStretch(), false, false);
    m_pBottomHost->SetFixedHeight(UiFixedInt::MakeAuto(), false, false);
    m_pPane->AddItem(m_pBottomHost);

    // Content area header
    m_pHeader = new Label(pWindow);
    m_pHeader->SetBkColor("bg_window_main");
    m_pHeader->SetFixedHeight(UiFixedInt(48), true, true);
    m_pHeader->SetAttribute("font", "system_bold_18");
    m_pHeader->SetAttribute("text_color", "text_default");
    m_pHeader->SetAttribute("text_align", "vcenter");
    m_pHeader->SetAttribute("padding", "20,0,20,0");
    m_pRight->AddItem(m_pHeader);
}

void NavigationView::EnsureContentHost()
{
    if (m_pContent != nullptr) {
        return;
    }
    // When the user does not provide a TabBox explicitly, create the content page container automatically
    m_pContent = new TabBox(GetWindow());
    m_pContent->SetFixedWidth(UiFixedInt::MakeStretch(), false, false);
    m_pContent->SetFixedHeight(UiFixedInt::MakeStretch(), false, false);
    m_pRight->AddItem(m_pContent);
}

void NavigationView::RegisterNavItem(NavigationViewItem* pItem)
{
    ASSERT(pItem != nullptr);
    if (pItem == nullptr) {
        return;
    }
    pItem->SetOwnerView(this);
    pItem->SetCompact(m_bCollapsed);
    m_items.push_back(pItem);
}

void NavigationView::UnregisterNavItem(NavigationViewItem* pItem)
{
    if (pItem == nullptr) {
        return;
    }
    auto it = std::find(m_items.begin(), m_items.end(), pItem);
    if (it != m_items.end()) {
        m_items.erase(it);
    }
    if (m_pSelected == pItem) {
        m_pSelected = nullptr;
        UpdateHeaderText(nullptr);
    }
}

bool NavigationView::IsIndexedItem(const NavigationViewItem* pItem) const
{
    // Items that take part in the index numbering: selectable (item form) and not the settings item
    return (pItem != nullptr) && pItem->IsSelectable() && (pItem != m_pSettingsNavItem);
}

NavigationViewItem* NavigationView::FindItemByPage(const std::string& strPageName) const
{
    if (strPageName.empty()) {
        return nullptr;
    }
    for (NavigationViewItem* pItem : m_items) {
        if ((pItem != nullptr) && pItem->IsSelectable() && (pItem->GetPageName() == strPageName)) {
            return pItem;
        }
    }
    return nullptr;
}

void NavigationView::ApplyCollapsed()
{
    if (m_pPane != nullptr) {
        const int32_t nWidth = m_bCollapsed ? m_nCompactPaneWidth : m_nPaneWidth;
        m_pPane->SetFixedWidth(UiFixedInt(nWidth), true, true);
    }
    if (m_pPaneTitleLabel != nullptr) {
        m_pPaneTitleLabel->SetVisible(!m_bCollapsed);
    }
    // Set the compact state one by one; SetCompact already returns early when the state is unchanged, so this loop only takes effect for the items that actually change,
    // and no additional incremental list needs to be maintained (the O(n) traversal is on the same order as the item count, so the benefit would be limited)
    for (NavigationViewItem* pItem : m_items) {
        if (pItem != nullptr) {
            pItem->SetCompact(m_bCollapsed);
        }
    }
}

void NavigationView::ApplyHeaderVisible()
{
    if (m_pHeader != nullptr) {
        m_pHeader->SetVisible(m_bShowHeader);
    }
}

void NavigationView::UpdateHeaderText(NavigationViewItem* pItem)
{
    if (m_pHeader == nullptr) {
        return;
    }
    if (pItem != nullptr) {
        m_pHeader->SetText(pItem->GetDisplayText());
    }
    else {
        m_pHeader->SetText("");
    }
}

void NavigationView::DoInitialSelection()
{
    NavigationViewItem* pInitItem = nullptr;
    if (!m_initSelectedId.empty()) {
        pInitItem = FindItemByPage(m_initSelectedId.c_str());
    }
    if (pInitItem == nullptr) {
        // Select the first selectable item by default (excluding the settings item, so that the settings page does not become the initial page)
        for (NavigationViewItem* pItem : m_items) {
            if (IsIndexedItem(pItem)) {
                pInitItem = pItem;
                break;
            }
        }
    }
    if (pInitItem != nullptr) {
        SelectItem(pInitItem, false);
    }
}

void NavigationView::OnToggleClicked()
{
    SetCollapsed(!m_bCollapsed);
}

void NavigationView::OnItemClicked(NavigationViewItem* pItem)
{
    if ((pItem == nullptr) || !pItem->IsSelectable()) {
        return;
    }
    // Fire the "navigation item clicked" event first: clicking an already selected item also fires it, WPARAM is the selectable item index
    size_t nIndex = 0;
    for (NavigationViewItem* p : m_items) {
        if (IsIndexedItem(p)) {
            if (p == pItem) {
                SendEvent(kEventNavigationItemClick, static_cast<WPARAM>(nIndex), 0);
                break;
            }
            ++nIndex;
        }
    }
    SelectItem(pItem, true);
}

void NavigationView::MoveFocusByKey(NavigationViewItem* pFrom, bool bForward)
{
    if (pFrom == nullptr) {
        return;
    }
    // Collect all focusable items (including the settings item; ordered by m_items, which is basically the visual order)
    std::vector<NavigationViewItem*> focusable;
    focusable.reserve(m_items.size());
    for (NavigationViewItem* p : m_items) {
        if ((p != nullptr) && p->IsSelectable() && p->IsEnabled()) {
            focusable.push_back(p);
        }
    }
    if (focusable.empty()) {
        return;
    }
    auto it = std::find(focusable.begin(), focusable.end(), pFrom);
    int32_t nIndex = (it == focusable.end()) ? (bForward ? -1 : 0)
                                             : static_cast<int32_t>(std::distance(focusable.begin(), it));
    const int32_t nCount = static_cast<int32_t>(focusable.size());
    // Move cyclically
    nIndex = bForward ? (nIndex + 1) : (nIndex - 1);
    if (nIndex < 0) {
        nIndex = nCount - 1;
    }
    else if (nIndex >= nCount) {
        nIndex = 0;
    }
    NavigationViewItem* pTarget = focusable[static_cast<size_t>(nIndex)];
    if ((pTarget != nullptr) && (pTarget != pFrom)) {
        pTarget->SetFocus();
    }
}

void NavigationView::SetCollapsed(bool bCollapsed)
{
    if (m_bCollapsed == bCollapsed) {
        return;
    }
    // Fire the "about to collapse/expand" event: it proceeds only when all listeners return true (returning false cancels this toggle)
    EventArgs msg;
    msg.eventType = kEventNavigationPaneToggling;
    msg.SetSender(this);
    Window* pWindow = GetWindow();
    if (pWindow != nullptr) {
        msg.ptMouse = pWindow->GetLastMousePos();
    }
    msg.wParam = bCollapsed ? 1 : 0;
    if (!FireAllEvents(msg)) {
        return;
    }

    m_bCollapsed = bCollapsed;
    ApplyCollapsed();

    // Fire the "collapse/expand finished" event, WPARAM is the current state (1 = collapsed, 0 = expanded)
    SendEvent(kEventNavigationPaneToggled, static_cast<WPARAM>(bCollapsed ? 1 : 0), 0);
}

bool NavigationView::SelectItem(NavigationViewItem* pItem, bool bFireEvent)
{
    if ((pItem == nullptr) || !pItem->IsSelectable() || !pItem->IsEnabled()) {
        return false;
    }
    size_t nOldIndex = GetSelectedIndex();
    if (m_pSelected != pItem) {
        if (m_pSelected != nullptr) {
            m_pSelected->SetSelected(false);
        }
        m_pSelected = pItem;
        pItem->SetSelected(true);
    }

    // Switch the content page in sync
    std::string strPage = pItem->GetPageName();
    if (!strPage.empty() && (m_pContent != nullptr)) {
        m_pContent->SelectItem(strPage);
    }
    UpdateHeaderText(pItem);

    if (bFireEvent) {
        // Selection changed event: WPARAM is the new index, LPARAM is the old index
        size_t nNewIndex = GetSelectedIndex();
        if (nNewIndex != nOldIndex) {
            SendEvent(kEventNavigationSelectionChanged,
                      static_cast<WPARAM>(nNewIndex), static_cast<LPARAM>(nOldIndex));
        }
    }
    return true;
}

bool NavigationView::SelectItem(const std::string& strPageName)
{
    NavigationViewItem* pItem = FindItemByPage(strPageName);
    return SelectItem(pItem, true);
}

bool NavigationView::SelectItemByIndex(size_t nIndex)
{
    size_t nSelectable = 0;
    for (NavigationViewItem* pItem : m_items) {
        if (IsIndexedItem(pItem)) {
            if (nSelectable == nIndex) {
                return SelectItem(pItem, true);
            }
            ++nSelectable;
        }
    }
    return false;
}

NavigationViewItem* NavigationView::GetSelectedItem() const
{
    return m_pSelected;
}

size_t NavigationView::GetSelectedIndex() const
{
    size_t nSelectable = 0;
    for (NavigationViewItem* pItem : m_items) {
        if (IsIndexedItem(pItem)) {
            if (pItem == m_pSelected) {
                return nSelectable;
            }
            ++nSelectable;
        }
    }
    return Box::InvalidIndex;
}

std::string NavigationView::GetSelectedPage() const
{
    if (m_pSelected != nullptr) {
        return m_pSelected->GetPageName();
    }
    return std::string();
}

NavigationViewItem* NavigationView::AddNavItem(const std::string& strTextId, const std::string& strIcon,
                                               const std::string& strPage, Control* pPage)
{
    EnsureInternals();
    // Attach the content pages first (if any): this ensures that a click on a navigation item finds the matching page name on the right, otherwise "a node without a page" would appear
    if (pPage != nullptr) {
        // Align the page name with the navigation item: when the page is unnamed, name it after strPage so that the linkage can find it
        if (!strPage.empty() && pPage->GetName().empty()) {
            pPage->SetName(strPage);
        }
        if (!AddPage(pPage)) {
            return nullptr;
        }
    }

    NavigationViewItem* pItem = new NavigationViewItem(GetWindow());
    if (!strTextId.empty()) {
        pItem->SetItemTextId(strTextId);
    }
    if (!strIcon.empty()) {
        pItem->SetIconAttr(strIcon);
    }
    pItem->SetPageName(strPage);
    RegisterNavItem(pItem);
    if (!m_pItemHost->AddItem(pItem)) {
        UnregisterNavItem(pItem);
        delete pItem;
        return nullptr;
    }
    return pItem;
}

bool NavigationView::AddPage(Control* pPage)
{
    if (pPage == nullptr) {
        return false;
    }
    // Other controls become content pages in the content area; a TabBox is created automatically when none is provided explicitly
    EnsureContentHost();
    if (m_pContent == nullptr) {
        return false;
    }
    return m_pContent->AddItem(pPage);
}

bool NavigationView::RemoveNavItem(NavigationViewItem* pItem, bool bRemovePage)
{
    if (pItem == nullptr) {
        return false;
    }
    auto it = std::find(m_items.begin(), m_items.end(), pItem);
    if (it == m_items.end()) {
        // Not a navigation item of this control
        return false;
    }
    // Remove it from its container (may be the list or the fixed bottom area)
    bool bRemoved = false;
    const std::string strPage = pItem->GetPageName();
    if (m_pItemHost != nullptr) {
        bRemoved = m_pItemHost->RemoveItem(pItem);
    }
    if (!bRemoved && (m_pBottomHost != nullptr)) {
        bRemoved = m_pBottomHost->RemoveItem(pItem);
    }
    if (!bRemoved && (m_pPane != nullptr)) {
        bRemoved = m_pPane->RemoveItem(pItem);
    }

    // Optional: remove the associated content page as well (find it in the content area by page name and remove it)
    if (bRemovePage && (m_pContent != nullptr)) {        
        if (!strPage.empty()) {
            if (Control* pPage = m_pContent->FindSubControl(strPage)) {
                m_pContent->RemoveItem(pPage);
            }
        }
    }

    UnregisterNavItem(pItem);
    // After the control is removed from the container, the caller decides whether to delete it (if it was created dynamically by this control, see the ownership rule of AddNavItem)
    return bRemoved;
}

void NavigationView::RemoveAllNavItems()
{
    if (m_pItemHost != nullptr) {
        m_pItemHost->RemoveAllItems();
    }
    if (m_pBottomHost != nullptr) {
        m_pBottomHost->RemoveAllItems();
    }
    m_items.clear();
    m_pSelected = nullptr;
    m_pSettingsNavItem = nullptr;
    UpdateHeaderText(nullptr);
}

void NavigationView::SetItemEnabled(NavigationViewItem* pItem, bool bEnabled)
{
    if (pItem == nullptr) {
        return;
    }
    pItem->SetEnabled(bEnabled);

    // When the currently selected item is disabled, cancel its selected state in sync (otherwise a dangling highlight would visually remain, showing "a disabled item is selected"),
    // and clear the title bar accordingly. Whether to switch to another available item is left to the caller (this avoids the side effect of an implicit jump caused by a disable operation).
    if (!bEnabled && (pItem == m_pSelected)) {
        pItem->SetSelected(false);
        m_pSelected = nullptr;
        UpdateHeaderText(nullptr);
    }
}

} // namespace ui
