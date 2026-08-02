#ifndef EXAMPLES_ITEM_H_
#define EXAMPLES_ITEM_H_

// duilib
#include "duilib/duilib.h"

// Inherit all available functionality from ui::ListBoxItem
class Item : public ui::ControlDragableT<ui::ListBoxItem>
{
public:
    explicit Item(ui::Window* pWindow);
    virtual ~Item() override;

    /** Provides an external call to initialize the item data
    */
    void InitSubControls(const DString& img, const DString& title);
    
private:
    /** Delete event
    */
    bool OnRemove(const ui::EventArgs& args);

private:
    /** List container
    */
    ui::ListBox* m_pListBox;

    /** Image
    */
    ui::Control* m_pImageControl;

    /** Text title
    */
    ui::Label* m_pTitleLabel;

    /** Progress bar
    */
    ui::Progress* m_pProgressControl;

    /** Delete button
    */
    ui::Button* m_pDelBtn;
};

#endif //#define EXAMPLES_ITEM_H_
