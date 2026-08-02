#ifndef UI_CONTROL_MENU_H_
#define UI_CONTROL_MENU_H_

#include "duilib/Utils/WinImplBase.h"
#include "duilib/Box/ListBox.h"
#include "duilib/Core/ControlPtrT.h"

namespace ui {

//Menu alignment
enum MenuAlignment
{
    eMenuAlignment_Left         = 1 << 1,
    eMenuAlignment_Top          = 1 << 2,
    eMenuAlignment_Right        = 1 << 3,
    eMenuAlignment_Bottom       = 1 << 4,
    eMenuAlignment_Intelligent  = 1 << 5    //Intelligent, to prevent being obscured
};

//Menu close type
enum class MenuCloseType
{
    eMenuCloseThis,  //Used to close the menu window at the current level, e.g., when the mouse moves in
    eMenuCloseAll     //Close all menu windows, e.g., when focus is lost
};

//Type of the menu popup position
enum class MenuPopupPosType
{   //Which position of the menu the clicked point belongs to        1.-----.2       1 top-left 2 top-right              
    //                                     |     |
    //It is assumed here that the user prefers the intelligent alignment                3.-----.4       3 bottom-left 4 bottom-right
    RIGHT_BOTTOM    = eMenuAlignment_Right | eMenuAlignment_Bottom | eMenuAlignment_Intelligent,
    RIGHT_TOP       = eMenuAlignment_Right | eMenuAlignment_Top    | eMenuAlignment_Intelligent,
    LEFT_BOTTOM     = eMenuAlignment_Left  | eMenuAlignment_Bottom | eMenuAlignment_Intelligent,
    LEFT_TOP        = eMenuAlignment_Left  | eMenuAlignment_Top    | eMenuAlignment_Intelligent,
    //This is normal, non-intelligent alignment
    RIGHT_BOTTOM_N  = eMenuAlignment_Right | eMenuAlignment_Bottom,
    RIGHT_TOP_N     = eMenuAlignment_Right | eMenuAlignment_Top,
    LEFT_BOTTOM_N   = eMenuAlignment_Left  | eMenuAlignment_Bottom,
    LEFT_TOP_N      = eMenuAlignment_Left  | eMenuAlignment_Top
};

#include "observer_impl_base.hpp"
struct ContextMenuParam
{
    MenuCloseType wParam;
    WindowBase* pWindow;
};

typedef class ObserverImpl<bool, ContextMenuParam> ContextMenuObserver;
typedef class ReceiverImpl<bool, ContextMenuParam> ContextMenuReceiver;

/////////////////////////////////////////////////////////////////////////////////////
//


/** Prototype of the callback function for selecting a menu item: after the menu disappears, it is used to get which menu item the user clicked (activated by mouse click or keyboard Enter)
* @param [in] menuName Menu name (i.e., the name attribute in XML, which represents the ID of the menu item)
* @param [in] nMenuLevel Menu level (0 indicates the first-level menu, 1 indicates the second-level menu, ...)
* @param [in] itemName Name of the menu item, equivalent to the command ID (i.e., the name attribute in XML, which represents the ID of the menu item)
* @param [in] nItemIndex Index number of the menu item (the index starts from 0)
*/
typedef std::function<void (const DString& menuName, int32_t nMenuLevel,
                            const DString& itemName, size_t nItemIndex)> MenuItemActivatedEvent;

/** Menu class
*/
class MenuItem;
class MenuBar;
class DUILIB_API Menu : public WindowImplBase, public ContextMenuReceiver
{
    typedef WindowImplBase BaseClass;
public:
    /** Constructor, initializes the parent window handle of the menu
    * @param [in] pParentWindow The parent window of the menu
    * @param [in] pRelatedControl The related control of the menu; when the menu pops up, the state of the related control is set to Pushed
    * @param [in] pMenuBar The interface of the related menu bar control
    */
    explicit Menu(Window* pParentWindow,
                  Control* pRelatedControl = nullptr,
                  MenuBar* pMenuBar = nullptr);

    /** Set the folder name for resource loading; if not set, it defaults to "menu" internally
    *   Resources in XML files (images, XML, etc.) are all searched in this folder
    */
    void SetSkinFolder(const DString& skinFolder);

    /** Set the XML template file and properties of the multi-level submenu
    @param [in] submenuXml The XML template file name of the submenu; if not set, it defaults to "submenu.xml" internally
    @param [in] submenuNodeName The node name in the submenu XML file where submenu items are inserted; if not set, it defaults to "submenu" internally
    */
    void SetSubMenuXml(const DString& submenuXml, const DString& submenuNodeName);

    /** Initialize the menu configuration and display the menu
    *   After it returns, the FindControl function can be used to find menu items for subsequent operations
    * @param [in] xml The menu XML resource file name; it is concatenated with GetSkinFolder() internally to form a full path
    * @param [in] point The popup position of the menu, in screen coordinates
    * @param [in] popupPosType The type of the menu popup position
    * @param [in] noFocus After the menu pops up, do not activate the window, to avoid grabbing the focus
    * @Param [in] pOwner The interface of the parent menu; if this value is not nullptr, this menu is in multi-level menu mode
    */
    void ShowMenu(const DString& xml, 
                  const UiPoint& point,
                  MenuPopupPosType popupPosType = MenuPopupPosType::LEFT_TOP, 
                  bool noFocus = false,
                  MenuItem* pOwner = nullptr);

    /** Close the menu
    */
    void CloseMenu();

    /** Register the callback function for menu item activation; after the menu disappears, it is used to get which menu item the user clicked (activated by mouse click or keyboard Enter)
    * @param [in] callback The callback function
    */
    void AttachMenuItemActivated(MenuItemActivatedEvent callback);

    /** Clear all callback functions
    */
    void ClearMenuItemActivated();

public:
    //Add a submenu item
    bool AddMenuItem(MenuItem* pMenuItem);
    bool AddMenuItemAt(MenuItem* pMenuItem, size_t iIndex);

    /** Add a normal control to the menu (used in pure-code mode, e.g., separator lines, custom rows)
    * @param [in] pControl The control to be added
    */
    bool AddMenuControl(Control* pControl);

    //Remove a menu item
    bool RemoveMenuItem(MenuItem* pMenuItem);
    bool RemoveMenuItemAt(size_t iIndex);

    //Get the number of menu items
    size_t GetMenuItemCount() const;

    //Get the menu item interface
    MenuItem* GetMenuItemAt(size_t iIndex) const;
    MenuItem* GetMenuItemByName(const DString& name) const;

    //Get the control associated with the menu
    Control* GetRelatedControl() const;

private:
    friend MenuItem; //Needs to access some private member functions

    //Get the global menu Observer object
    static ContextMenuObserver& GetMenuObserver();

    //Disconnect the association with the parent menu object
    void DetachOwner();        //add by djj 20200506

private:
    // Resize the menu
    bool ResizeMenu();

    // Resize the submenu
    bool ResizeSubMenu();

    /** Get the ListBox interface of the layout management
    */
    ListBox* GetLayoutListBox() const;

    /** Menu item activated (activated by mouse click or keyboard Enter)
    * @param [in] menuName Menu name (i.e., the name attribute in XML, which represents the ID of the menu item)
    * @param [in] nMenuLevel Menu level (0 indicates the first-level menu, 1 indicates the second-level menu, ...)
    * @param [in] itemName Name of the menu item (i.e., the name attribute in XML, which represents the ID of the menu item)
    * @param [in] nItemIndex Index number of the menu item (the index starts from 0)
    */
    void OnMenuItemActivated(const DString& menuName, int32_t nMenuLevel,
                             const DString& itemName, size_t nItemIndex);

private:

    virtual bool Receive(ContextMenuParam param) override;

    virtual ui::Control* CreateControl(const DString& pstrClass) override;
    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;
    virtual void PreInitWindow() override;
    virtual void PostInitWindow() override;
    virtual void OnCloseWindow() override;

    /** Called when the window is destroyed; this is the last message of the window (the default implementation of this class is to clean up resources and destroy the window object)
    */
    virtual void OnFinalMessage() override;

    /** The window loses focus (WM_KILLFOCUS)
    * @param [in] pSetFocusWindow The window that receives the keyboard focus (can be nullptr)
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnKillFocusMsg(WindowBase* pSetFocusWindow, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Keyboard key pressed (WM_KEYDOWN or WM_SYSKEYDOWN)
    * @param [in] vkCode The virtual key code
    * @param [in] modifierKey Key modifier flags; valid values: ModifierKey::kFirstPress, ModifierKey::kAlt
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnKeyDownMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

    //Blocked messages
    virtual LRESULT OnContextMenuMsg(const UiPoint& pt, const NativeMsg& nativeMsg, bool& bHandled) override;
    virtual LRESULT OnMouseRButtonDownMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;
    virtual LRESULT OnMouseRButtonUpMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;
    virtual LRESULT OnMouseRButtonDbClickMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

private:
    //Parent window of the menu
    Window* m_pParentWindow;

    //Popup position of the menu
    UiPoint m_menuPoint;

    //Type of the menu popup position
    MenuPopupPosType m_popupPosType;

    //Folder name for resource loading
    UiString m_skinFolder;

    //XML template file name of the submenu
    UiString m_submenuXml;

    //Node name in the submenu XML file where submenu items are inserted
    UiString m_submenuNodeName;

    //XML file name of the menu resource
    UiString m_xml;

    //Whether it is in no-focus mode when the menu pops up
    bool m_noFocus;

    //Interface of the parent menu of this menu
    MenuItem* m_pOwner;

    //Layout interface of the menu
    ControlPtrT<ListBox> m_pListBox;

    //The related control
    ControlPtrT<Control> m_pRelatedControl;

    //Interface of the related menu bar control
    ControlPtrT<MenuBar> m_pMenuBar;

private:
    //Callback functions for menu item activation
    std::vector<MenuItemActivatedEvent> m_callbackList;

    //Information of the activated menu item
    struct ActiveMenuItem
    {
        DString m_menuName;
        int32_t m_menuLevel = 0;
        DString m_itemName;
        size_t m_itemIndex = Box::InvalidIndex;
    };
    std::unique_ptr<ActiveMenuItem> m_pActiveMenuItem;
};

/** Menu item
*/
class DUILIB_API MenuItem : public ListBoxItem
{
    typedef ListBoxItem BaseClass;
public:
    explicit MenuItem(Window* pWindow);

    //Add a submenu item
    bool AddSubMenuItem(MenuItem* pMenuItem);
    bool AddSubMenuItemAt(MenuItem* pMenuItem, size_t iIndex);

    //Remove a submenu item
    bool RemoveSubMenuItem(MenuItem* pMenuItem);
    bool RemoveSubMenuItemAt(size_t iIndex);
    void RemoveAllSubMenuItem();

    //Get the number of submenu items
    size_t GetSubMenuItemCount() const;

    //Get the submenu item interface
    MenuItem* GetSubMenuItemAt(size_t iIndex) const;
    MenuItem* GetSubMenuItemByName(const DString& name) const;

    //Menu item activation (activated by click or by pressing Enter)
    virtual void Activate(const EventArgs* pMsg) override;

private:
    //Get all submenu items under a menu item (only menu sub-item elements)
    static void GetAllSubMenuItem(const MenuItem* pParentElementUI, 
                                  std::vector<MenuItem*>& submenuItems);

    //Get all submenu controls under a menu item (including menu sub-item elements and other controls)
    static void GetAllSubMenuControls(const MenuItem* pParentElementUI,
                                      std::vector<Control*>& submenuControls);

private:
    virtual bool ButtonUp(const ui::EventArgs& msg) override;
    virtual bool MouseEnter(const ui::EventArgs& msg) override;
    virtual void PaintChild(ui::IRender* pRender, const ui::UiRect& rcPaint) override;

private:
    friend Menu; //Needs to access some private member functions

    //Check the submenu; if it is a lower-level menu, create the lower-level menu window and display it
    bool CheckSubMenuItem();

    //Create the lower-level menu window and display it
    void CreateMenuWnd();

private:
    //Interface of the lower-level menu window
    Menu* m_pSubWindow;
};

} // namespace ui

#endif // UI_CONTROL_MENU_H_
