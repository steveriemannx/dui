#ifndef UI_CONTROL_NAVIGATION_VIEW_H_
#define UI_CONTROL_NAVIGATION_VIEW_H_

#include "dui/Box/HBox.h"
#include "dui/Control/Button.h"
#include "dui/Control/Label.h"
#include <vector>

namespace ui
{

class NavigationView;
class TabBox;
class VBox;
class VScrollBox;
class HBox;
class Line;

/** Sidebar navigation item control (NavigationViewItem)
*
*   Used as a direct child of NavigationView; it paints the "icon + text" row itself
*   and supports three forms:
*     item (default): a clickable navigation item, gray background + accent-colored text when selected
*     header        : a group title (smaller, muted color, not clickable)
*     separator     : a separator line
*
*   Keyboard navigation is supported: an item can be focused with Tab, the up/down arrow keys move the focus between selectable items
*   and Enter/Space selects the current item; the disabled state (SetEnabled(false)) ignores clicks and the keyboard.
*
*   XML attributes:
*     text / text_id  item text (text_id supports automatic multi-language switching)
*     icon            icon image attribute string, e.g. file='public/nav/home.svg' width='16' height='16'
*     page            name of the associated content page control (a content page is a non-navigation direct child of NavigationView)
*     item_type       item (default) / header / separator
*     styles (all optional, the default values match the built-in appearance):
*       icon_size / icon_left / text_left / text_right        icon size and left/right margins (DIP)
*       pill_radius / pill_inset_x / pill_inset_y             corner radius and inset of the selected/hovered hot area (DIP)
*       selected_bg_color / hovered_bg_color                  selected/hovered background color names
*       selected_text_color / normal_text_color               selected/normal text color names
*       disabled_text_color / header_text_color / separator_color  color names of the disabled text/group title/separator
*/
class DUI_API NavigationViewItem : public Control
{
    typedef Control BaseClass;
public:
    /** Navigation item form
    */
    enum class ItemType : uint8_t
    {
        kItem       = 0,    // Clickable navigation item
        kHeader     = 1,    // Group title
        kSeparator  = 2     // Separator line
    };

public:
    explicit NavigationViewItem(Window* pWindow);
    NavigationViewItem(const NavigationViewItem& r) = delete;
    NavigationViewItem& operator=(const NavigationViewItem& r) = delete;
    virtual ~NavigationViewItem() override = default;

    virtual std::string GetType() const override;
    virtual void OnInit() override;
    virtual void SetAttribute(const std::string& strName, const std::string& strValue) override;
    virtual void Paint(IRender* pRender, const UiRect& rcPaint) override;
    virtual void PaintStateImages(IRender* pRender) override;
    virtual bool MouseEnter(const EventArgs& msg) override;
    virtual bool MouseLeave(const EventArgs& msg) override;
    virtual bool ButtonUp(const EventArgs& msg) override;
    virtual void OnLanguageChanged() override;
    virtual uint32_t GetControlFlags() const override;
    virtual void HandleEvent(const EventArgs& msg) override;

public:
    /** Set the item text
    */
    void SetItemText(const std::string& strText);
    std::string GetItemText() const;

    /** Set the item text ID (multi-language)
    */
    void SetItemTextId(const std::string& strTextId);
    std::string GetItemTextId() const;

    /** Get the text that should currently be displayed (text_id takes priority and is resolved to the current language text)
    */
    std::string GetDisplayText() const;

    /** Set the icon (image attribute string)
    */
    void SetIconAttr(const std::string& strIcon);
    std::string GetIconAttr() const;

    /** Set the name of the associated content page
    */
    void SetPageName(const std::string& strPage);
    std::string GetPageName() const;

    /** Set the form
    */
    void SetItemType(ItemType itemType);
    ItemType GetItemType() const { return m_itemType; }

    /** Whether it can be clicked to select (item form only)
    */
    bool IsSelectable() const { return m_itemType == ItemType::kItem; }

    /** Set the selected state (managed centrally by NavigationView)
    */
    void SetSelected(bool bSelected);
    bool IsSelected() const { return m_bSelected; }

    /** Set the compact mode (true = show only the centered icon, switched centrally by NavigationView when expanding/collapsing)
    */
    void SetCompact(bool bCompact);
    bool IsCompact() const { return m_bCompact; }

    /** Set the owning NavigationView (set by it when the item joins the navigation pane)
    */
    void SetOwnerView(NavigationView* pOwner) { m_pOwner = pOwner; }

private:
    /** Paint the three forms
    */
    void PaintItem(IRender* pRender);
    void PaintHeader(IRender* pRender);
    void PaintSeparator(IRender* pRender);

private:
    /** Item text
    */
    UiString m_text;

    /** Item text ID (multi-language)
    */
    UiString m_textId;

    /** Icon image attribute string
    */
    UiString m_iconAttr;

    /** Name of the associated content page
    */
    UiString m_pageName;

    /** Form
    */
    ItemType m_itemType;

    /** Whether it is selected
    */
    bool m_bSelected;

    /** Whether the mouse hovers over it
    */
    bool m_bHovered;

    /** Whether it is in compact mode
    */
    bool m_bCompact;

    /** Owning NavigationView
    */
    NavigationView* m_pOwner;

    //--- Customizable styles (the default values match the built-in appearance; they can be overridden with XML attributes) ---

    /** Icon size (DIP, default 16) */
    int32_t m_nIconSize;

    /** Left margin of the icon in the expanded state (DIP, default 16) */
    int32_t m_nIconLeft;

    /** Left margin of the text in the expanded state (DIP, default 44) */
    int32_t m_nTextLeft;

    /** Right margin of the text in the expanded state (DIP, default 8) */
    int32_t m_nTextRight;

    /** Corner radius of the selected/hovered hot area (DIP, default 6) */
    int32_t m_nPillRadius;

    /** Horizontal inset of the hot area (DIP, default 6) */
    int32_t m_nPillInsetX;

    /** Vertical inset of the hot area (DIP, default 3) */
    int32_t m_nPillInsetY;

    /** Selected background color name (default bg_list_item_selected) */
    UiString m_selectedBgColor;

    /** Hovered background color name (default bg_list_item_hovered) */
    UiString m_hoveredBgColor;

    /** Selected text color name (default color_accent) */
    UiString m_selectedTextColor;

    /** Normal text color name (default text_default) */
    UiString m_normalTextColor;

    /** Disabled text color name (default text_disabled) */
    UiString m_disabledTextColor;

    /** Group title text color name (default text_muted) */
    UiString m_headerTextColor;

    /** Separator color name (default border_window) */
    UiString m_separatorColor;    
};

/** Sidebar navigation control (NavigationView)
*
*   It combines "a left navigation pane + a right content area (header + TabBox pages)" internally:
*   the pane holds the hamburger button, the pane title, a scrollable navigation item list and an optional settings item at the bottom;
*   clicking a navigation item switches the content page in sync and updates the header; the hamburger button toggles between the expanded and compact states.
*
*   XML usage (recommended: put the content pages into one TabBox explicitly, which makes the page container easy to control from outside):
*   <NavigationView pane_width="220" compact_pane_width="48" pane_title_id="..."
*                   show_header="true" settings_item="true" settings_page="page_settings"
*                   toggle_icon="file='nav/hamburger.svg' ..." selected_id="page_home">
*     <NavigationViewItem text_id="..." icon="..." page="page_home"/>
*     <NavigationViewItem item_type="header" text_id="..."/>
*     <NavigationViewItem item_type="separator"/>
*     <TabBox name="nav_content">
*       <VBox name="page_home"> ... </VBox>
*       <VBox name="page_settings"> ... </VBox>
*     </TabBox>
*   </NavigationView>
*
*   Routing rules for the direct children (implemented by overriding AddItem):
*     NavigationViewItem -> the navigation item list (header/separator also enter the list, only their rendering form differs)
*     TabBox             -> taken over as the content area page container (only one is allowed; its attributes such as the switch animation can be configured freely)
*     other controls     -> they enter the content area as pages; a TabBox is created automatically when none is provided explicitly
*
*   Index convention: the index counts only the "selectable navigation items" (item form); header/separator and the bottom settings item are not counted;
*             the settings item is always shown at the bottom of the pane, does not scroll with the navigation list, and does not take part in the index numbering.
*
*   Runtime usage: AddNavItem(textId, icon, page, pPage) completes "create the content page + add the navigation item + link them by name" in one step;
*             when only the navigation item is needed (the page already exists), just omit pPage. AddPage() adds a page to the content area on its own;
*             RemoveNavItem(pItem, bRemovePage=true) removes the associated content page together with the item.
*
*   XML attributes:
*     pane_width           width of the pane in the expanded state (pixels, default 220, DPI aware)
*     compact_pane_width   width of the pane in the compact state (pixels, default 48, DPI aware)
*     pane_title           pane title text
*     pane_title_id        pane title text ID (multi-language)
*     toggle_icon          hamburger button icon image attribute string (when empty the button shows no icon and can be customized with the text styles)
*     collapsed            whether the initial state is compact (true/false, default false)
*     show_header          whether to show the content area header (true/false, default true)
*     settings_item        whether to show the settings item at the bottom of the pane (true/false, default false)
*     settings_page        name of the content page associated with the settings item (default settings)
*     settings_text_id     text ID of the settings item (multi-language, empty by default, provided by the user)
*     settings_icon        icon image attribute string of the settings item (empty by default, provided by the user)
*     selected_id          page name of the initially selected item (default: the first selectable item)
*
*   Events (all fired by NavigationView itself as the sender, independent of the kEventTabSelect of the inner TabBox):
*     kEventNavigationItemClick         fired when a selectable navigation item is clicked (clicking the already selected item fires it again),
*                                       WPARAM is the index of that item among all selectable items; suitable for "refresh/scroll to top on click" requirements
*     kEventNavigationSelectionChanged  fired when the selected item changes, WPARAM is the new index and LPARAM the old index
*                                       (Box::InvalidIndex when nothing is selected)
*     kEventNavigationPaneToggling      fired when the pane is about to collapse/expand, WPARAM is the target state (1 = collapsed, 0 = expanded),
*                                       returning false from the callback cancels this toggle
*     kEventNavigationPaneToggled       fired when the pane has collapsed/expanded, WPARAM is the current state (1 = collapsed, 0 = expanded)
*/
class DUI_API NavigationView : public HBox
{
    typedef HBox BaseClass;
public:
    explicit NavigationView(Window* pWindow);
    NavigationView(const NavigationView& r) = delete;
    NavigationView& operator=(const NavigationView& r) = delete;
    virtual ~NavigationView() override;

    virtual std::string GetType() const override;
    virtual void OnInit() override;
    virtual void SetAttribute(const std::string& strName, const std::string& strValue) override;
    virtual bool AddItem(Control* pControl) override;
    virtual void OnLanguageChanged() override;

public:
    /** Select the specified navigation item
    * @param [in] pItem The navigation item pointer (it must belong to this control and be selectable)
    * @param [in] bFireEvent Whether to fire the kEventTabSelect event
    * @return true on success
    */
    bool SelectItem(NavigationViewItem* pItem, bool bFireEvent = true);

    /** Select the navigation item by the name of its associated content page
    */
    bool SelectItem(const std::string& strPageName);

    /** Select by selectable item index (the index counts only the item form, not header/separator)
    */
    bool SelectItemByIndex(size_t nIndex);

    /** Get the pointer of the currently selected item (nullptr when nothing is selected)
    */
    NavigationViewItem* GetSelectedItem() const;

    /** Get the index of the currently selected item among all selectable items (Box::InvalidIndex when nothing is selected)
    */
    size_t GetSelectedIndex() const;

    /** Get the name of the content page associated with the currently selected item
    */
    std::string GetSelectedPage() const;

    /** Expand/collapse the navigation pane
    */
    void SetCollapsed(bool bCollapsed);
    bool IsCollapsed() const { return m_bCollapsed; }

    /** Listen for the navigation item click event (kEventNavigationItemClick)
    *   Difference from the selection-changed event: this event still fires when the currently selected item is clicked again; WPARAM is the selectable item index
    */
    void AttachNavItemClick(const EventCallback& callback, EventCallbackID callbackID = 0)
    {
        AttachEvent(kEventNavigationItemClick, callback, callbackID);
    }

    /** Listen for the navigation selection-changed event (kEventNavigationSelectionChanged)
    *   WPARAM is the new index, LPARAM is the old index (Box::InvalidIndex when nothing is selected)
    */
    void AttachNavSelectionChanged(const EventCallback& callback, EventCallbackID callbackID = 0)
    {
        AttachEvent(kEventNavigationSelectionChanged, callback, callbackID);
    }

    /** Listen for the "pane is about to collapse/expand" event (kEventNavigationPaneToggling, cancelable)
    *   WPARAM is the target state (1 = collapsed, 0 = expanded); returning false from the callback prevents this toggle
    */
    void AttachNavPaneToggling(const EventCallback& callback, EventCallbackID callbackID = 0)
    {
        AttachEvent(kEventNavigationPaneToggling, callback, callbackID);
    }

    /** Listen for the "pane collapsed/expanded" event (kEventNavigationPaneToggled)
    *   WPARAM is the current state (1 = collapsed, 0 = expanded)
    */
    void AttachNavPaneToggled(const EventCallback& callback, EventCallbackID callbackID = 0)
    {
        AttachEvent(kEventNavigationPaneToggled, callback, callbackID);
    }

    /** Append a navigation item dynamically
    *   Two usages:
    *     1) Pass pPage: attach that page control to the content area TabBox (linked by its name), completing "add the navigation item + add the content page" in one step;
    *     2) Do not pass pPage: only append the navigation item; the associated content page must already exist in the content area (or be added later with AddPage).
    * @param [in] strTextId The item text ID (multi-language); when it is empty, strText is used
    * @param [in] strIcon The icon image attribute string
    * @param [in] strPage The name of the associated content page (it must match the name of pPage or of an existing content page)
    * @param [in] pPage The associated content page control; when it is nullptr only the navigation item is added.
    *                   When it is passed in, its name should equal strPage (if pPage has no name, strPage is used as its name).
    * @return The newly created navigation item pointer (still managed by the control, the caller must not release it); nullptr on failure
    */
    NavigationViewItem* AddNavItem(const std::string& strTextId, const std::string& strIcon, const std::string& strPage,
                                   Control* pPage = nullptr);

    /** Add a page control to the content area TabBox (for callers that assemble the "create the page first, link it later" flow themselves)
    * @param [in] pPage The page control; it is recommended to set its name first (matching the page of the navigation item)
    * @return true on success
    */
    bool AddPage(Control* pPage);

    /** Remove a navigation item
    * @param [in] pItem The navigation item pointer (it must belong to this control)
    * @param [in] bRemovePage When true, also remove its associated content page from the content area (found by page name)
    * @return true on success
    */
    bool RemoveNavItem(NavigationViewItem* pItem, bool bRemovePage = false);

    /** Remove all navigation items (group titles/separators/settings item are removed as well; content pages are not removed)
    */
    void RemoveAllNavItems();

    /** Enable/disable a navigation item (when it is disabled it cannot be clicked to select)
    */
    void SetItemEnabled(NavigationViewItem* pItem, bool bEnabled);

    /** Callback from NavigationViewItem when a navigation item is clicked
    */
    void OnItemClicked(NavigationViewItem* pItem);

    /** Keyboard navigation: move the focus between selectable items (bForward = true moves down, false moves up)
    */
    void MoveFocusByKey(NavigationViewItem* pFrom, bool bForward);

private:
    /** Lazily create the internal child structure (pane, separator, right header); it must be done before the first user child is routed
    */
    void EnsureInternals();

    /** Make sure the content area TabBox exists: created lazily when none is provided explicitly
    */
    void EnsureContentHost();

    /** Register a navigation item: set the owner, the compact state, and add it to the list
    */
    void RegisterNavItem(NavigationViewItem* pItem);

    /** Unregister a navigation item: remove it from the list (the control is not destroyed)
    */
    void UnregisterNavItem(NavigationViewItem* pItem);

    /** Check whether it is a "selectable navigation item" that takes part in the index numbering (the settings item, header and separator are excluded)
    */
    bool IsIndexedItem(const NavigationViewItem* pItem) const;

    /** Find a selectable item by content page name
    */
    NavigationViewItem* FindItemByPage(const std::string& strPageName) const;

    /** Apply the current expanded/compact state (pane width, title visibility, compact flag of each navigation item)
    */
    void ApplyCollapsed();

    /** Apply the header visibility
    */
    void ApplyHeaderVisible();

    /** Synchronize the header with the text of the selected item
    */
    void UpdateHeaderText(NavigationViewItem* pItem);

    /** Complete the initial selection (called from OnInit)
    */
    void DoInitialSelection();

    /** Hamburger button click
    */
    void OnToggleClicked();

private:
    /** Whether the internal structure has been created
    */
    bool m_bInternalsReady;

    /** Pane width in the expanded/compact state (the raw value before scaling)
    */
    int32_t m_nPaneWidth;
    int32_t m_nCompactPaneWidth;

    /** Whether the compact state is active
    */
    bool m_bCollapsed;

    /** Whether to show the content area header
    */
    bool m_bShowHeader;

    /** Whether to show the bottom settings item
    */
    bool m_bSettingsItem;

    /** Page name / text ID / icon of the settings item
    */
    UiString m_settingsPage;
    UiString m_settingsTextId;
    UiString m_settingsIcon;

    /** Pane title text / text ID
    */
    UiString m_paneTitle;
    UiString m_paneTitleId;

    /** Hamburger button icon image attribute string
    */
    UiString m_toggleIcon;

    /** Page name of the initially selected item
    */
    UiString m_initSelectedId;

    /** Inner controls
    */
    VBox* m_pPane;             ///< Left navigation pane (vertical container)
    HBox* m_pTopBar;           ///< Top of the pane: hamburger button + pane title
    Button* m_pToggleBtn;      ///< Hamburger button
    Label* m_pPaneTitleLabel;  ///< Pane title
    VScrollBox* m_pItemHost;   ///< Scrollable list of navigation items (fills the remaining pane space, stretch)
    VBox* m_pBottomHost;       ///< Fixed bottom area of the pane (holds the settings item; does not scroll with the list)
    NavigationViewItem* m_pSettingsNavItem; ///< Bottom settings item
    Line* m_pSeparator;        ///< Vertical separator between the pane and the content area
    VBox* m_pRight;            ///< Right side: header + content
    Label* m_pHeader;          ///< Content area header
    TabBox* m_pContent;        ///< Content area page container

    /** All navigation items (including header/separator, in the order they were added)
    */
    std::vector<NavigationViewItem*> m_items;

    /** Currently selected item
    */
    NavigationViewItem* m_pSelected;
};

} // namespace ui

#endif // UI_CONTROL_NAVIGATION_VIEW_H_
