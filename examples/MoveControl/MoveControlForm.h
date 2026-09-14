#ifndef EXAMPLES_MOVE_CONTROL_FORM_H_
#define EXAMPLES_MOVE_CONTROL_FORM_H_

#include "AppDb.h"

class MoveControlForm : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
public:
    MoveControlForm();
    MoveControlForm(const std::string& theme_directory, const std::string& layout_xml);
    virtual ~MoveControlForm() override = default;

    /** Resource-related interfaces
     * GetSkinFolder sets the skin resource path of the window to be drawn
     * GetSkinFile sets the XML description file of the window to be drawn
     */
    virtual std::string GetSkinFolder() override { return m_theme_directory; }
    virtual std::string GetSkinFile() override { return m_layout_xml; }

    /** Called after the window is created, for subclasses to do some initialization work
    */
    virtual void OnInitWindow() override;

private:
    void BindEvents();

    std::string m_theme_directory;
    std::string m_layout_xml;

    ui::Box* m_frequent_app=nullptr;
    ui::Box* m_my_app = nullptr;
};
#endif //EXAMPLES_MOVE_CONTROL_FORM_H_
