#ifndef EXAMPLES_MOVE_CONTROL_CODE_FORM_H_
#define EXAMPLES_MOVE_CONTROL_CODE_FORM_H_

#include "AppDb.h"

class MoveControlForm : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
public:
    MoveControlForm();
    virtual ~MoveControlForm() override = default;

    /** Resource-related interfaces
     * GetSkinFolder sets the skin resource path of the window to be drawn
     * GetSkinFile sets the XML description file of the window to be drawn
     */
    virtual DString GetSkinFolder() override { return DUI_T("move_control"); }
    virtual DString GetSkinFile() override { return DUI_T(""); }

    /** Called after the window is created, for subclasses to do some initialization work
    */
    virtual void OnInitWindow() override;

private:
    void SetupWindow();
    void BuildUI();
    void BindEvents();

    ui::Box* m_frequent_app=nullptr;
    ui::Box* m_my_app = nullptr;
};
#endif //EXAMPLES_MOVE_CONTROL_CODE_FORM_H_
