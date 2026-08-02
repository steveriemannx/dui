#ifndef EXAMPLES_CODEUI_CODE_FORM_H_
#define EXAMPLES_CODEUI_CODE_FORM_H_

#include "duilib/duilib.h"

/** Pure-code form - no XML at all, all UI defined in C++
*/
class CodeForm : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
public:
    CodeForm();
    virtual ~CodeForm() override;

    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;
    virtual void GetCreateWindowAttributes(ui::WindowCreateAttributes& createAttributes) override;
    virtual void OnInitWindow() override;
    virtual void OnCloseWindow() override;

private:
    bool OnButtonClick(const ui::EventArgs& msg);
};

#endif
