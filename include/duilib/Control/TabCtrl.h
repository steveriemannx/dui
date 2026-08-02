#ifndef UI_CONTROL_TABCTRL_H_
#define UI_CONTROL_TABCTRL_H_

#include "duilib/Box/ListBox.h"
#include "duilib/Core/ControlDragable.h"
#include "duilib/Control/IconControl.h"

namespace ui
{
/** Multi-tab control (similar to a browser's multiple tabs)
*/
class TabBox;
class DUILIB_API TabCtrl: public ListBox
{
    typedef ListBox BaseClass;
public:
    explicit TabCtrl(Window* pWindow);

    /** Get the control type
    */
    virtual DString GetType() const override;
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;

public:
    /** Set the name of the bound TabBox control
    */
    void SetTabBoxName(const DString& tabBoxName);

    /** Get the name of the bound TabBox control
    */
    DString GetTabBoxName() const;

    /** Set the bound TabBox interface
    */
    void SetTabBox(TabBox* pTabBox);

    /** Get the bound TabBox interface
    */
    TabBox* GetTabBox() const;

    /** Set whether dragging to change the order of controls is supported
    */
    void SetEnableDragOrder(bool bEnable);

    /** Determine whether dragging to change the order of controls is supported
    */
    bool IsEnableDragOrder() const;

    /** Handle showing or hiding the separator line
    */
    void AdjustItemLineStatus();

public:
    /** Set the position index of an item
     * @param [in] pControl item pointer
     * @param [in] iIndex index, range: [0, GetItemCount())
     */
    virtual bool SetItemIndex(Control* pControl, size_t iIndex) override;

    /** Append an item to the end
     * @param [in] pControl item pointer
     */
    virtual bool AddItem(Control* pControl) override;

    /** Insert an item after the specified position
     * @param [in] pControl item pointer
     * @param[in] iIndex the position index to insert at, range: [0, GetItemCount())
     */
    virtual bool AddItemAt(Control* pControl, size_t  iIndex) override;

    /** Remove an item by its pointer
     * @param [in] pControl item pointer
     */
    virtual bool RemoveItem(Control* pControl) override;

    /** Remove an item by index
     * @param [in] iIndex item index, range: [0, GetItemCount())
     */
    virtual bool RemoveItemAt(size_t iIndex) override;

    /** Remove all items
     */
    virtual void RemoveAllItems() override;

protected:
    /** Initialize interface
    */
    virtual void OnInit() override;

    /** Message handling function
    * @param [in] msg message content
    */
    virtual void HandleEvent(const EventArgs& msg) override;

private:
    /** Default selected item
    */
    size_t m_nSelectedId;

    /** Name of the bound TabBox
    */
    UiString m_tabBoxName;

    /** Interface of the bound TabBox
    */
    TabBox* m_pTabBox;

    /** Whether dragging to change the order of columns is supported (feature switch)
    */
    bool m_bEnableDragOrder;
};

/** A tab page of the multi-tab control
*/
class DUILIB_API TabCtrlItem : public ControlDragableT<ListBoxItemH>
{
    typedef ControlDragableT<ListBoxItemH> BaseClass;
public:
    explicit TabCtrlItem(Window* pWindow);

    /** Get the control type
    */
    virtual DString GetType() const override;
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;
    virtual DString GetToolTipText() const override;

public:
    /** Set the icon
    * @param [in] iconImageString icon resource string
    */
    void SetIcon(const DString& iconImageString);

    /** Get the icon
    * @return the icon resource string
    */
    DString GetIcon() const;

    /** Set the bitmap data of the icon (data format: ARGB, alpha type is kPremul_SkAlphaType)
    * @param [in] nWidth width
    * @param [in] nHeight height
    * @param [in] pPixelBits bitmap data
    * @param [in] nPixelBitsSize length of the bitmap data (in bytes)
    */
    bool SetIconData(int32_t nWidth, int32_t nHeight, const uint8_t* pPixelBits, int32_t nPixelBitsSize);

    /** Clear the icon data
    */
    void ClearIconData();

    /** Set the text content
    */
    void SetTitle(const DString& title);

    /** Get the text content
    */
    DString GetTitle() const;

    /** Set the text content ID (supports multilingual versions)
    */
    void SetTitleId(const DString& titleId);

    /** Get the text content (supports multilingual versions)
    */
    DString GetTitleId() const;

public:
    /** Set the index of the bound TabBox item
    */
    void SetTabBoxItemIndex(size_t nTabBoxItemIndex);

    /** Get the bound TabBox item ID index
    */
    size_t GetTabBoxItemIndex() const;

public:
    /** Get the icon control
    */
    IconControl* GetIconControl() const { return m_pIcon; }

    /** Get the text control
    */
    Label* GetTextLabel() const { return m_pLabel; }

    /** Get the close button
    */
    Button* GetCloseButton() const { return m_pCloseBtn; }

    /** Get the separator line control between tabs
    */
    Control* GetLineControl() const { return m_pLine; }

public:
    /** Class of the icon control
    */
    void SetIconClass(const DString& iconClass);
    DString GetIconClass() const;

    /** Class of the text control
    */
    void SetTitleClass(const DString& titleClass);
    DString GetTitleClass() const;

    /** Class of the close button control
    */
    void SetCloseButtonClass(const DString& closeButtonClass);
    DString GetCloseButtonClass() const;

    /** Class of the separator line control
    */
    void SetLineClass(const DString& lineClass);
    DString GetLineClass() const;

public:
    /** Set the corner radius of the selected tab
    * @param [in] szCorner width and height of the corner radius
    * @param [in] bNeedDpiScale whether to adapt to DPI, the default is true
    */
    void SetSelectedRoundCorner(UiSize szCorner, bool bNeedDpiScale);

    /** Get the corner radius of the selected tab
    */
    UiSize GetSelectedRoundCorner() const;

    /** Set the corner radius of the hovered tab
    * @param [in] szCorner width and height of the corner radius
    * @param [in] bNeedDpiScale whether to adapt to DPI, the default is true
    */
    void SetHotRoundCorner(UiSize szCorner, bool bNeedDpiScale);

    /** Get the corner radius of the hovered tab
    */
    UiSize GetHotRoundCorner() const;

    /** Set the padding of the background color in the Hot state
    * @param [in] rcPadding padding data
    * @param [in] bNeedDpiScale whether to adapt to DPI, the default is true
    */
    void SetHotPadding(UiPadding rcPadding, bool bNeedDpiScale);

    /** Get the padding of the background color in the Hot state
    */
    UiPadding GetHotPadding() const;

    /** Set whether the close button is automatically hidden
    * @param [in] bAutoHideCloseBtn true means the close button is automatically hidden, false means the close button is always shown
    */
    void SetAutoHideCloseButton(bool bAutoHideCloseBtn);

    /** Get whether the close button is automatically hidden
    */
    bool IsAutoHideCloseButton() const;

    /** Handle showing or hiding the separator line
    */
    void AdjustItemLineStatus();

    /** Whether the separator line of the current tab is visible
    */
    bool IsItemLineVisible() const;

    /** Set whether the tab separator line is shown or hidden
    */
    void SetItemLineVisible(bool bVisible);

public:
    /** Determine whether dragging to change the order of controls is supported
    */
    virtual bool IsEnableDragOrder() const override;

protected:
    /** Initialize interface
    */
    virtual void OnInit() override;

    /** Message handling function
    * @param [in] msg message content
    */
    virtual void HandleEvent(const EventArgs& msg) override;

    /** Handle the mouse enter message
    */
    virtual bool MouseEnter(const EventArgs& msg) override;

    /** Handle the mouse leave message
    */
    virtual bool MouseLeave(const EventArgs& msg) override;

    /** Mouse left button down message
    */
    virtual bool ButtonDown(const EventArgs& msg) override;

    /** Function for drawing the control state colors
    */
    virtual void PaintStateColors(IRender* pRender) override;

    /** Selection state change event (the m_bSelected variable changes)
    */
    virtual void OnPrivateSetSelected() override;

    /** Draw the tab page (selected state)
    */
    virtual void PaintTabItemSelected(IRender* pRender) ;

    /** Draw the tab page (hover state)
    */
    virtual void PaintTabItemHot(IRender* pRender);

    /** Set visible state event
    * @param [in] bChanged true means the state has changed, false means the state has not changed
    */
    virtual void OnSetVisible(bool bChanged) override;

protected:
    /** Fill the path to form a rounded rectangle
    */
    void AddTabItemPath(IPath* path, const UiRect& rect, UiSize roundSize) const;

    /** Adjust the order of the sub-controls
    */
    void AdjustSubItemIndex();

    /** Get the TabCtrl interface
    */
    TabCtrl* GetTabCtrl() const;

    /** Check and set the visibility of the icon
    */
    void CheckIconVisible();

private:
    /** Corner radius of the selected tab
    */
    struct RoundCorner
    {
        uint8_t cx; //width of the corner radius
        uint8_t cy; //height of the corner radius
    };

    /** Padding value of the Hot tab state
    */
    struct HotPadding
    {
        uint8_t left;
        uint8_t top;
        uint8_t right;
        uint8_t bottom;
    };

    /** Corner radius of the selected tab
    */
    RoundCorner m_rcSelected;

    /** Corner radius of the hovered tab
    */
    RoundCorner m_rcHot;

    /** Padding value of the Hot tab state
    */
    HotPadding m_hotPadding;

    /** Whether the close button is automatically hidden
    */
    bool m_bAutoHideCloseBtn;

    /** Icon control
    */
    IconControl* m_pIcon;

    /** Text control
    */
    Label* m_pLabel;

    /** Close button
    */
    Button* m_pCloseBtn;

    /** Separator line between tabs
    */
    Control* m_pLine;

    /** Class of the icon control
    */
    UiString m_iconClass;

    /** Class of the text control
    */
    UiString m_titleClass;

    /** Class of the close button control
    */
    UiString m_closeBtnClass;

    /** Class of the separator line
    */
    UiString m_lineClass;

    /** Text content
    */
    UiString m_title;

    /** Text content ID
    */
    UiString m_titleId;

    /** Icon resource string
    */
    UiString m_iconImageString;

private:
    /** Index of the bound TabBox item
    */
    size_t m_nTabBoxItemIndex;
};

}//namespace ui

#endif //UI_CONTROL_TABCTRL_H_
