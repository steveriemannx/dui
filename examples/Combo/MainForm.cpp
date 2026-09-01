#include "MainForm.h"
#include "dui/Utils/UiBuilder.h"

void MainForm::OnInitWindow()
{
    BaseClass::OnInitWindow();

    // Populate all combo controls with sample items
    const DString comboNames[] = {
        DUI_T("combo_default"), DUI_T("combo_small_round"),
        DUI_T("combo_round"), DUI_T("combo_not_round"),
        DUI_T("combo_edit_default"), DUI_T("combo_edit_small_round"),
        DUI_T("combo_edit_round"), DUI_T("combo_edit_not_round"),
        DUI_T("filter_default"), DUI_T("filter_small_round"),
        DUI_T("filter_round"), DUI_T("filter_not_round"),
    };
    for (const DString& name : comboNames) {
        ui::Combo* pCombo = dynamic_cast<ui::Combo*>(FindControl(name));
        if (pCombo == nullptr) {
            continue;
        }
        ui::TreeNode* pRoot = pCombo->GetTreeView()->GetRootNode();
        for (int32_t i = 0; i < 8; ++i) {
            auto* pNode = ui::Create<ui::TreeNode>(this, {
                {DUI_T("class"), DUI_T("tree_node")},
                {DUI_T("text"), ui::StringUtil::Printf(DUI_T("Item %d"), i + 1)}
            });
            pRoot->AddChildNode(pNode);
        }
    }
}
