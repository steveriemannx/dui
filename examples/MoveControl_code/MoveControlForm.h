#ifndef EXAMPLES_MOVE_CONTROL_CODE_FORM_H_
#define EXAMPLES_MOVE_CONTROL_CODE_FORM_H_

#include "AppDb.h"

class MoveControlForm : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
public:
    MoveControlForm();
    virtual ~MoveControlForm() override;

    /** Resource-related interfaces
     * GetSkinFolder sets the skin resource path of the window to be drawn
     * GetSkinFile sets the XML description file of the window to be drawn
     */
    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;
    virtual void GetCreateWindowAttributes(ui::WindowCreateAttributes& attrs) override;

    /** Called after the window is created, for subclasses to do some initialization work
    */
    virtual void OnInitWindow() override;

private:
    /** Build the UI in pure code (corresponding to the main.xml layout)
    */
    void BuildUI();

private:
    ui::Box* m_frequent_app=nullptr;
    ui::Box* m_my_app = nullptr;
};
#endif //EXAMPLES_MOVE_CONTROL_CODE_FORM_H_
