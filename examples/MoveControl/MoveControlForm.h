#ifndef EXAMPLES_MOVE_CONTROL_FORM_H_
#define EXAMPLES_MOVE_CONTROL_FORM_H_

#include "AppDb.h"

class MoveControlForm : public ui::WindowImplBase
{
public:
    MoveControlForm(const DString& theme_directory, const DString& layout_xml);
    virtual ~MoveControlForm() override;

    /** Resource-related interfaces
     * GetSkinFolder sets the skin resource path of the window to be drawn
     * GetSkinFile sets the XML description file of the window to be drawn
     */
    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;

    /** Called after the window is created, for subclasses to do some initialization work
    */
    virtual void OnInitWindow() override;

private:
    DString m_theme_directory;
    DString m_layout_xml;

    ui::Box* m_frequent_app=nullptr;
    ui::Box* m_my_app = nullptr;
};
#endif //EXAMPLES_MOVE_CONTROL_FORM_H_
