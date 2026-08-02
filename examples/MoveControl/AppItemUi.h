#ifndef EXAMPLES_APP_ITEM_UI_H_
#define EXAMPLES_APP_ITEM_UI_H_

// duilib
#include "duilib/duilib.h"

#define APP_HEIGHT 90
#define APP_WIDTH  90
#define EACH_LINE  6

// App details; here we assume id, name, _icon, _isFrequent; extend as needed
struct AppItem
{
    std::string m_id;
    DString m_name;
    DString m_icon;
    bool m_isFrequent=false;
};

// App UI class
class AppItemUi : public ui::ControlDragableT<ui::VBox>
{
    typedef ui::ControlDragableT<ui::VBox> BaseClass;
public:
    static AppItemUi* Create(const AppItem& item, Box* p);

    explicit AppItemUi(ui::Window* pWindow);
    virtual void OnInit() override;
    void SetAppdata(const AppItem& item,bool refresh);
    inline const AppItem& getAppData() const { return m_app_data; }

    /** Container for frequent controls
    */
    void SetFrequentBox(ui::Box* pFrequentBox);

private:
    /** Target position indicator control
    * @param [in] pTargetBox The target Box control interface to be dragged into
    */
    virtual Control* CreateDestControl(Box* pTargetBox) override;

    /** Drag-complete event of the container to which the control belongs (within the same container or different containers)
    * @param [in] pOldBox The container interface the control originally belonged to
    * @param [in] nOldItemIndex The original child item index
    * @param [in] pNewBox The container interface the control now belongs to
    * @param [in] nNewItemIndex The latest child item index
    */
    virtual void OnItemBoxChanged(Box* pOldBox, size_t nOldItemIndex, 
                                  Box* pNewBox, size_t nNewItemIndex) override;

private:
    AppItem m_app_data;
    ui::Control* m_app_icon = nullptr;
    ui::Label* m_app_name = nullptr;

    /** Container for frequent controls
    */
    Box* m_pFrequentBox = nullptr;
};
#endif //EXAMPLES_APP_ITEM_UI_H_
