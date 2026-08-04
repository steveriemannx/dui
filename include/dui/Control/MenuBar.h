#ifndef UI_CONTROL_MENU_BAR_H_
#define UI_CONTROL_MENU_BAR_H_

#include "dui/Control/Menu.h"
#include "dui/Control/Button.h"
#include "dui/Box/HBox.h"
#include "dui/Core/ControlPtrT.h"

namespace ui
{
/** Top-level menu item data
*/
struct MenuBarItem
{
    DString m_menuItemId;               //ID of this menu item, used in the callback function to identify which menu's command was activated
    DString m_menuText;                 //Menu text
    DString m_menuTextId;               //Menu text ID, used to support the multilingual version
    DString m_menuXmlPath;              //XML path of the menu resource (relative path in the resource directory)

    DString m_menuTextButtonClass;      //Class name of the Button attribute of the control that displays the menu text; if empty, the default style is used (the "menu_bar_button" attribute in global.xml)
    DString m_menuTextButtonAttributes; //Additional attribute list of the Button attribute of the control that displays the menu text; can be empty    
};

/** Prototype of the callback function for selecting a menu item in the top-level menu: after the menu disappears, it is used to get which menu item the user clicked (activated by mouse click or keyboard Enter)
* @param [in] menuItemId ID of this menu item, i.e., the m_menuItemId value in MenuBarItem
* @param [in] menuName Menu name (i.e., the name attribute in XML, which represents the ID of the menu item)
* @param [in] nMenuLevel Menu level (0 indicates the first-level menu, 1 indicates the second-level menu, ...)
* @param [in] itemName Name of the menu item, equivalent to the command ID (i.e., the name attribute in XML, which represents the ID of the menu item)
* @param [in] nItemIndex Index number of the menu item (the index starts from 0)
*/
typedef std::function<void (const DString& menuItemId,
                            const DString& menuName, int32_t nMenuLevel,
                            const DString& itemName, size_t nItemIndex)> MenuBarItemActivatedEvent;

//Text button on the MenuBar
class MenuBarButton;

/** Menu bar
*/
class DUI_API MenuBar: public HBox
{
    typedef HBox BaseClass;
public:
    explicit MenuBar(Window* pWindow);

public:
    /// Override the parent class interface to provide personalized functionality. For specific method descriptions, please refer to the Control control
    virtual DString GetType() const override;
    virtual void OnInit() override;

public:
    /** Add a top-level menu and return its index number
    * @param [in] menuItemId ID of this menu item, used in the callback function to identify which menu's command was activated
    * @param [in] menuText Text of the top-level menu
    * @param [in] menuTextId Text ID of the top-level menu, used to support the multilingual version
    * @param [in] menuXmlPath Path of the menu XML resource file
    * @param [in] menuTextButtonClass Class name of the Button attribute of the control that displays the top-level menu text; pass empty to use the default value
    * @param [in] menuTextButtonAttributes Additional attribute list of the Button attribute of the control that displays the top-level menu text
    * @return Returns the index number on success, or -1 on failure
    */
    int32_t AddTopMenu(const DString& menuItemId,
                       const DString& menuText,
                       const DString& menuTextId,
                       const DString& menuXmlPath,
                       const DString& menuTextButtonClass = _T(""),
                       const DString& menuTextButtonAttributes = _T(""));

    /** Add a top-level menu and return its index number
    * @param [in] menuBarItem The related data of the top-level menu
    * @return Returns the index number on success, or -1 on failure
    */
    int32_t AddTopMenu(const MenuBarItem& menuBarItem);

    /** Add a top-level menu (pure-code mode, without an XML template) and return its index number
    * @param [in] menuItemId ID of this menu item, used in the callback function to identify which menu's command was activated
    * @param [in] menuText Text of the top-level menu
    * @param [in] menuBuilder The menu building callback, called when the menu pops up, used to add menu items
    * @param [in] menuTextButtonClass Class name of the Button attribute of the control that displays the top-level menu text; pass empty to use the default value
    * @param [in] menuTextButtonAttributes Additional attribute list of the Button attribute of the control that displays the top-level menu text
    * @return Returns the index number on success, or -1 on failure
    */
    int32_t AddTopMenu(const DString& menuItemId,
                       const DString& menuText,
                       const std::function<void(Menu*)>& menuBuilder,
                       const DString& menuTextButtonClass = _T(""),
                       const DString& menuTextButtonAttributes = _T(""));

    /** Add a top-level menu at the specified position and return its index number
    * @param [in] nMenuIndex The specified index number
    * @param [in] menuText Text of the top-level menu
    * @param [in] menuXmlPath Path of the menu XML resource file
    * @param [in] menuTextButtonClass Class name of the Button attribute of the control that displays the top-level menu text; pass empty to use the default value
    * @param [in] menuTextButtonAttributes Additional attribute list of the Button attribute of the control that displays the top-level menu text
    * @return Returns the index number on success, or -1 on failure
    */
    int32_t InsertTopMenu(int32_t nMenuIndex,
                          const DString& menuItemId,
                          const DString& menuText,
                          const DString& menuTextId,
                          const DString& menuXmlPath,
                          const DString& menuTextButtonClass = _T(""),
                          const DString& menuTextButtonAttributes = _T(""));

    /** Add a top-level menu at the specified position and return its index number
    * @param [in] menuBarItem The related data of the top-level menu
    * @return Returns the index number on success, or -1 on failure
    */
    int32_t InsertTopMenu(int32_t nMenuIndex, const MenuBarItem& menuBarItem);

    /** Get the information of the top-level menu with the specified index number
    * @param [in] nMenuIndex The specified index number
    * @param [out] menuBarItem The related data of the top-level menu
    */
    bool GetTopMenu(int32_t nMenuIndex, MenuBarItem& menuBarItem);

    /** Remove a top-level menu according to its index number
    */
    bool RemoveTopMenu(int32_t nMenuIndex);

    /** Set the currently activated top-level menu (expand its submenus)
    */
    bool SetActiveTopMenuIndex(int32_t nMenuIndex);

    /** Get the index number of the currently activated top-level menu
    */
    int32_t GetActiveTopMenuIndex() const;

    /** Register the callback function for menu item activation; after the menu disappears, it is used to get which menu item the user clicked (activated by mouse click or keyboard Enter)
    * @param [in] callback The callback function
    */
    void AttachMenuBarItemActivated(MenuBarItemActivatedEvent callback);

    /** Clear all callback functions
    */
    void ClearMenuBarItemActivated();

private:
    //Top-level menu item data
    struct TopMenuData
    {
        UiString m_menuItemId;               //ID of this menu item, used in the callback function to identify which menu's command was activated
        UiString m_menuText;                 //Menu text
        UiString m_menuTextId;               //Menu text ID, used to support the multilingual version
        UiString m_menuXmlPath;              //XML path of the menu resource

        UiString m_menuTextButtonClass;      //Class name of the Button attribute of the control that displays the menu text
        UiString m_menuTextButtonAttributes; //Additional attribute list of the Button attribute of the control that displays the menu text

        std::function<void(Menu*)> m_menuBuilder;    //Pure-code mode: menu building callback; empty means the XML template is used

        size_t m_nItemDataId = Box::InvalidIndex;    //Control ID in the UI container
        bool m_bMouseEnter = false;                  //Whether it is in the MouseEnter state (used to avoid repeatedly popping up the menu)
    };

private:
    //Add the top-level menu to the UI control
    void AddTopMenuToUI(const TopMenuData& menuData, size_t nInsertItem);

    //Remove the top-level menu from the UI control
    void RemoveTopMenuFromUI(const TopMenuData& menuData);

private:
    //Mouse events of the top-level menu button
    friend class MenuBarButton;
    friend class Menu;

    void OnMenuMouseEnter(MenuBarButton* pButton, const EventArgs& msg);
    void OnMenuMouseLeave(MenuBarButton* pButton, const EventArgs& msg);
    void OnMenuMouseButtonDown(MenuBarButton* pButton, const EventArgs& msg);
    void OnMenuMouseButtonUp(MenuBarButton* pButton, const EventArgs& msg);

    //Check and display the submenu as needed
    void CheckShowPopupMenu(MenuBarButton* pButton);

    //Popup the submenu in the specified top-level menu item
    void ShowPopupMenu(MenuBarButton* pButton);

    //Hide the submenu
    void HidePopupMenu(MenuBarButton* pButton);

    //Whether there is currently an active menu
    bool HasActivePopupMenu() const;

    /** A key is pressed on the menu
    * @param [in] pMenu The menu interface
    * @param [in] vkCode The virtual key code
    * @param [in] modifierKey Key modifier flags; valid values: ModifierKey::kFirstPress, ModifierKey::kAlt
    */
    void OnMenuKeyDownMsg(Menu* pMenu, VirtualKeyCode vkCode, uint32_t modifierKey);

private:
    //Top-level menu item data
    std::vector<TopMenuData> m_topMenuList;

    //Callback functions for menu item activation
    std::vector<MenuBarItemActivatedEvent> m_callbackList;

    //ID value of the top-level menu in the UI
    size_t m_nItemDataId;

    //Interface of the currently active menu
    ControlPtrT<Menu> m_pActiveMenu;

    //Whether it is currently in the active state
    bool m_bActiveState;

    //Whether the menu item is currently allowed to be displayed when the button is activated
    bool m_bEnableBtnActive;
};

} // namespace ui

#endif // UI_CONTROL_MENU_BAR_H_
