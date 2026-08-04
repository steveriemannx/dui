#ifndef EXAMPLES_ITEM_H_
#define EXAMPLES_ITEM_H_

// dui
#include "dui/dui.h"

// Inherit all available functionality from ui::ListBoxItem
class Item : public ui::ListBoxItem
{
public:
    explicit Item(ui::Window* pWindow);
    virtual ~Item() override;

    /** Provides an external call to initialize the item data
    */
    void InitSubControls(const DString& img, const DString& title, size_t nDataIndex);

private:

    /** Delete event
    */
    bool OnRemove(const ui::EventArgs& args);

private:
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

    /** Timestamp
    */
    int64_t t_time;

    /** Data element index
    */
    size_t m_nDataIndex;
};

#endif //EXAMPLES_ITEM_H_
