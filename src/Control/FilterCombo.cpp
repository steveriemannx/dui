#include "dui/Control/FilterCombo.h"

namespace ui 
{

FilterCombo::FilterCombo(Window* pWindow):
    Combo(pWindow)
{
    SetComboType(kCombo_DropDown);
}

FilterCombo::~FilterCombo()
{
}

std::string FilterCombo::GetType() const { return DUI_CTR_FILTER_COMBO; }

void FilterCombo::SetAttribute(const std::string& strName, const std::string& strValue)
{
    if (strName == "combo_type") {
        //Ignore this attribute setting
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

void FilterCombo::OnInit()
{
    if (IsInited()) {
        return;
    }
    BaseClass::OnInit();
    SetComboType(kCombo_DropDown);
}

bool FilterCombo::OnEditButtonDown(const EventArgs& /*args*/)
{
    ShowComboList();
    return true;
}

bool FilterCombo::OnEditButtonUp(const EventArgs& /*args*/)
{
    return true;
}

bool FilterCombo::OnEditTextChanged(const ui::EventArgs& /*args*/)
{
    std::string editText = GetText();
    //Convert to lowercase so that the comparison is case-insensitive
    editText = StringUtil::MakeLowerString(editText);
    ShowComboList();
    FilterComboList(editText);
    return true;
}

void FilterCombo::FilterComboList(const std::string& filterText)
{
    TreeView* pTreeView = GetTreeView();
    if (pTreeView == nullptr) {
        return;
    }
    size_t itemCount = pTreeView->GetItemCount();
    for (size_t iIndex = 0; iIndex < itemCount; ++iIndex) {
        Control* pControl = pTreeView->GetItemAt(iIndex);
        if (pControl != nullptr) {
            TreeNode* pTreeNode = dynamic_cast<TreeNode*>(pControl);
            ASSERT(pTreeNode != nullptr);
            if (pTreeNode != nullptr) {
                pTreeNode->SetExpand(true, false);
                if (IsFilterText(filterText, pTreeNode->GetText())) {
                    pTreeNode->SetVisible(true);
                }
                else {
                    pTreeNode->SetVisible(false);
                }
            }
        }
    }
    UpdateComboList();
}

bool FilterCombo::IsFilterText(const std::string& filterText, const std::string& itemText) const
{
    std::string lowerItemText = StringUtil::MakeLowerString(itemText);
    if (filterText.empty()) {
        return true;
    }
    return (lowerItemText.find(filterText) != std::string::npos) ? true : false;
}

} // namespace ui

