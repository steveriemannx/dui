#include "MainForm.h"
#include "dui/Utils/UiBuilder.h"

void MainForm::OnInitWindow()
{
    BaseClass::OnInitWindow();

    // Populate all combo controls with sample items
    const std::string comboNames[] = {
        "combo_default", "combo_small_round",
        "combo_round", "combo_not_round",
        "combo_edit_default", "combo_edit_small_round",
        "combo_edit_round", "combo_edit_not_round",
        "filter_default", "filter_small_round",
        "filter_round", "filter_not_round",
    };
    for (const std::string& name : comboNames) {
        ui::Combo* pCombo = dynamic_cast<ui::Combo*>(FindControl(name));
        if (pCombo == nullptr) {
            continue;
        }
        ui::TreeNode* pRoot = pCombo->GetTreeView()->GetRootNode();
        for (int32_t i = 0; i < 8; ++i) {
            auto* pNode = ui::Create<ui::TreeNode>(this, {
                {"class", "tree_node"},
                {"text", ui::StringUtil::Printf("Item %d", i + 1)}
            });
            pRoot->AddChildNode(pNode);
        }
    }
}
