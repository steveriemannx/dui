#ifndef EXAMPLES_GENLIST_MAIN_FORM_H_
#define EXAMPLES_GENLIST_MAIN_FORM_H_

#include "duilib/duilib.h"

class MainForm : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
public:
    MainForm();
    virtual ~MainForm() override;

    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;
    virtual void GetCreateWindowAttributes(ui::WindowCreateAttributes& attrs) override;
    virtual void OnInitWindow() override;

private:
    bool OnButtonClick(const ui::EventArgs& msg);
    void InitListBox();
    void UpdateCounts();

    ui::VirtualListBox* m_pListBox;
    ui::RichEdit* m_pEditColumn;
    ui::RichEdit* m_pEditTotal;
    ui::RichEdit* m_pEditUpdate;
};

#endif
