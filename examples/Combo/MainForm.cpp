#include "MainForm.h"
#include "dui/Utils/UiBuilder.h"

void MainForm::OnInitWindow()
{
    BaseClass::OnInitWindow();

    // Populate all combo controls with sample items
    const DString comboNames[] = {
        _T("combo_default"), _T("combo_small_round"),
        _T("combo_round"), _T("combo_not_round"),
        _T("combo_edit_default"), _T("combo_edit_small_round"),
        _T("combo_edit_round"), _T("combo_edit_not_round"),
        _T("filter_default"), _T("filter_small_round"),
        _T("filter_round"), _T("filter_not_round"),
    };
    for (const DString& name : comboNames) {
        ui::Combo* pCombo = dynamic_cast<ui::Combo*>(FindControl(name));
        if (pCombo == nullptr) {
            continue;
        }
        ui::TreeNode* pRoot = pCombo->GetTreeView()->GetRootNode();
        for (int32_t i = 0; i < 8; ++i) {
            auto* pNode = ui::Create<ui::TreeNode>(this, {
                {"class", "tree_node"},
                {"text", ui::StringUtil::Printf(_T("Item %d"), i + 1)}
            });
            pRoot->AddChildNode(pNode);
        }
    }
}
