#ifndef EXAMPLES_GENLIST_ITEM_H_
#define EXAMPLES_GENLIST_ITEM_H_

#include "duilib/duilib.h"

class Item : public ui::ListBoxItem
{
public:
    explicit Item(ui::Window* pWindow);
    virtual ~Item() override;

    void InitSubControls(const DString& img, const DString& title, int nDataIndex);

private:
    bool OnRemove(const ui::EventArgs& args);

    ui::Control*  m_pImageControl;
    ui::Label*    m_pTitleLabel;
    ui::Progress* m_pProgressControl;
    ui::Button*   m_pDelBtn;
    size_t        m_nDataIndex;
};

#endif
