#include "duilib/Control/TreeView.h"
#include "duilib/Core/ScrollBar.h"
#include "duilib/Core/GlobalManager.h"
#include "duilib/Image/Image.h"

namespace ui
{

TreeNode::TreeNode(Window* pWindow) :
    ListBoxItem(pWindow),
    m_bExpand(true),
    m_pTreeView(nullptr),
    m_pParentTreeNode(nullptr),
    m_uDepth(0),
    m_expandCheckBoxPadding(0),
    m_expandIconPadding(0),
    m_expandTextPadding(0),
    m_checkBoxIconPadding(0),
    m_checkBoxTextPadding(0),
    m_iconTextPadding(0),
    m_pExpandImageRect(nullptr),
    m_pCollapseImageRect(nullptr),
    m_expandIndent(0),
    m_checkBoxIndent(0),
    m_iconIndent(0)
{
    SetExpandIndent(4, true);
    SetCheckBoxIndent(6, true);
    SetIconIndent(4, true);
}

TreeNode::~TreeNode()
{
    if (m_pExpandImageRect != nullptr) {
        delete m_pExpandImageRect;
        m_pExpandImageRect = nullptr;
    }
    if (m_pCollapseImageRect != nullptr) {
        delete m_pCollapseImageRect;
        m_pCollapseImageRect = nullptr;
    }
}

DString TreeNode::GetType() const { return DUI_CTR_TREENODE; }

void TreeNode::SetAttribute(const DString& strName, const DString& strValue)
{
    if (strName == _T("expand_normal_image")) {
        SetExpandStateImage(kControlStateNormal, strValue);
    }
    else if (strName == _T("expand_hot_image")) {
        SetExpandStateImage(kControlStateHot, strValue);
    }
    else if (strName == _T("expand_pushed_image")) {
        SetExpandStateImage(kControlStatePushed, strValue);
    }
    else if (strName == _T("expand_disabled_image")) {
        SetExpandStateImage(kControlStateDisabled, strValue);
    }
    else if (strName == _T("collapse_normal_image")) {
        SetCollapseStateImage(kControlStateNormal, strValue);
    }
    else if (strName == _T("collapse_hot_image")) {
        SetCollapseStateImage(kControlStateHot, strValue);
    }
    else if (strName == _T("collapse_pushed_image")) {
        SetCollapseStateImage(kControlStatePushed, strValue);
    }
    else if (strName == _T("collapse_disabled_image")) {
        SetCollapseStateImage(kControlStateDisabled, strValue);
    }
    else if (strName == _T("expand_image_right_space")) {
        int32_t iValue = StringUtil::StringToInt32(strValue);
        SetExpandIndent(iValue, true);
    }
    else if (strName == _T("check_box_image_right_space")) {
        int32_t iValue = StringUtil::StringToInt32(strValue);
        SetCheckBoxIndent(iValue, true);
    }
    else if (strName == _T("icon_image_right_space")) {
        int32_t iValue = StringUtil::StringToInt32(strValue);
        SetIconIndent(iValue, true);
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

void TreeNode::ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale)
{
    if (!Dpi().CheckDisplayScaleFactor(nNewDpiScale)) {
        return;
    }

    int32_t iValue = GetExpandIndent();
    iValue = Dpi().GetScaleInt(iValue, nOldDpiScale);
    SetExpandIndent(iValue, false);

    iValue = GetCheckBoxIndent();
    iValue = Dpi().GetScaleInt(iValue, nOldDpiScale);
    SetCheckBoxIndent(iValue, false);

    iValue = GetIconIndent();
    iValue = Dpi().GetScaleInt(iValue, nOldDpiScale);
    SetIconIndent(iValue, false);

    if (m_expandCheckBoxPadding > 0) {
        m_expandCheckBoxPadding = ui::TruncateToUInt16(Dpi().GetScaleInt((int32_t)m_expandCheckBoxPadding, nOldDpiScale));
    }
    if (m_expandIconPadding > 0) {
        m_expandIconPadding = ui::TruncateToUInt16(Dpi().GetScaleInt((int32_t)m_expandIconPadding, nOldDpiScale));
    }
    if (m_expandTextPadding > 0) {
        m_expandTextPadding = ui::TruncateToUInt16(Dpi().GetScaleInt((int32_t)m_expandTextPadding, nOldDpiScale));
    }
    if (m_checkBoxIconPadding > 0) {
        m_checkBoxIconPadding = ui::TruncateToUInt16(Dpi().GetScaleInt((int32_t)m_checkBoxIconPadding, nOldDpiScale));
    }
    if (m_checkBoxTextPadding > 0) {
        m_checkBoxTextPadding = ui::TruncateToUInt16(Dpi().GetScaleInt((int32_t)m_checkBoxTextPadding, nOldDpiScale));
    }
    if (m_iconTextPadding > 0) {
        m_iconTextPadding = ui::TruncateToUInt16(Dpi().GetScaleInt((int32_t)m_iconTextPadding, nOldDpiScale));
    }
    BaseClass::ChangeDpiScale(nOldDpiScale, nNewDpiScale);
}

void TreeNode::SetExpandIndent(int32_t nExpandIndent, bool bNeedDpiScale)
{
    if (nExpandIndent < 0) {
        nExpandIndent = 4;
    }
    if (bNeedDpiScale) {
        Dpi().ScaleInt(nExpandIndent);
    }
    m_expandIndent = ui::TruncateToUInt16(nExpandIndent);
}

uint16_t TreeNode::GetExpandIndent() const
{
    return m_expandIndent;
}

void TreeNode::SetCheckBoxIndent(int32_t nIndent, bool bNeedDpiScale)
{
    if (nIndent < 0) {
        nIndent = 6;
    }
    if (bNeedDpiScale) {
        Dpi().ScaleInt(nIndent);
    }
    m_checkBoxIndent = ui::TruncateToUInt16(nIndent);
}

uint16_t TreeNode::GetCheckBoxIndent() const
{
    return m_checkBoxIndent;
}

void TreeNode::SetIconIndent(int32_t nIndent, bool bNeedDpiScale)
{
    if (nIndent < 0) {
        nIndent = 4;
    }
    if (bNeedDpiScale) {
        Dpi().ScaleInt(nIndent);
    }
    m_iconIndent = ui::TruncateToUInt16(nIndent);
}

uint16_t TreeNode::GetIconIndent() const
{
    return m_iconIndent;
}

DString TreeNode::GetExpandStateImage(ControlStateType stateType)
{
    Image* pImage = nullptr;
    if (m_expandImage != nullptr) {
        pImage = m_expandImage->GetStateImage(stateType);
    }
    if (pImage != nullptr) {
        return pImage->GetImageString();
    }
    return DString();
}

void TreeNode::SetExpandStateImage(ControlStateType stateType, const DString& strImage)
{
    if (m_expandImage == nullptr) {
        m_expandImage.reset(new StateImage);
        m_expandImage->SetControl(this);
    }
    m_expandImage->SetImageString(stateType, strImage, Dpi());
}

DString TreeNode::GetCollapseStateImage(ControlStateType stateType)
{
    Image* pImage = nullptr;
    if (m_collapseImage != nullptr) {
        pImage = m_collapseImage->GetStateImage(stateType);
    }
    if (pImage != nullptr) {
        return pImage->GetImageString();
    }
    return DString();
}

void TreeNode::SetCollapseStateImage(ControlStateType stateType, const DString& strImage)
{
    if (m_collapseImage == nullptr) {
        m_collapseImage.reset(new StateImage);
        m_collapseImage->SetControl(this);
    }
    m_collapseImage->SetImageString(stateType, strImage, Dpi());
}

void TreeNode::PaintStateImages(IRender* pRender)
{
    BaseClass::PaintStateImages(pRender);
    if (IsExpand()) {
        //Draw the expanded state icon; if there are no child nodes, this icon will not be drawn
        if ((m_expandImage != nullptr) && !m_aTreeNodes.empty()){
            if (m_pExpandImageRect == nullptr) {
                m_pExpandImageRect = new UiRect;
            }
            m_expandImage->PaintStateImage(pRender, GetState(), _T(""), m_pExpandImageRect);
        }
    }
    else {
        //Draw the collapsed state icon
        if (m_collapseImage != nullptr) {
            if (m_pCollapseImageRect == nullptr) {
                m_pCollapseImageRect = new UiRect;
            }
            m_collapseImage->PaintStateImage(pRender, GetState(), _T(""), m_pCollapseImageRect);
        }
    }
}

bool TreeNode::ButtonDown(const EventArgs& msg)
{
    bool bRet = BaseClass::ButtonDown(msg);
    if (msg.IsSenderExpired()) {
        return false;
    }
    if (!IsEnabled()) {
        return bRet;
    }
    UiRect pos = GetPos();
    UiPoint pt(msg.ptMouse);
    pt.Offset(GetScrollOffsetInScrollBox());
    if (!pos.ContainsPt(pt)) {
        return bRet;
    }
    if (IsExpand()) {
        //Expanded state
        if ((m_expandImage != nullptr) && !m_aTreeNodes.empty()) {
            //If clicked on the expand icon, collapse
            if ((m_pExpandImageRect != nullptr) && 
                m_pExpandImageRect->ContainsPt(pt)) {
                SetExpand(false, true);
            }
        }
    }
    else {
        //Collapsed state
        if (m_collapseImage != nullptr) {
            //If clicked on the collapse icon, expand
            if ((m_pCollapseImageRect != nullptr) && 
                m_pCollapseImageRect->ContainsPt(pt)) {
                SetExpand(true, true);
            }
        }
    }
    return bRet;
}

int32_t TreeNode::GetExpandImagePadding(void) const
{
    int32_t imageWidth = 0;
    Image* pImage = nullptr;
    if (m_collapseImage != nullptr) {
        pImage = m_collapseImage->GetStateImage(kControlStateNormal);
    }
    if(pImage == nullptr){
        if (m_expandImage != nullptr) {
            pImage = m_expandImage->GetStateImage(kControlStateNormal);
        }
    }
    if (pImage != nullptr) {
        LoadImageInfo(*pImage);
        if (pImage->GetImageInfo() != nullptr) {
            imageWidth = pImage->GetImageInfo()->GetWidth();
        }
    }
    if (imageWidth > 0) {
        imageWidth += GetExpandIndent();
    }
    return imageWidth;
}

void TreeNode::SetTreeView(TreeView* pTreeView)
{
    m_pTreeView = pTreeView;
}

TreeView* TreeNode::GetTreeView() const
{
    return m_pTreeView;
}

bool TreeNode::OnDoubleClickItem(const EventArgs& args)
{
    TreeNode* pItem = dynamic_cast<TreeNode*>(args.GetSender());
    ASSERT(pItem != nullptr);
    ASSERT(pItem == this);
    if (pItem != nullptr) {
        pItem->SetExpand(!pItem->IsExpand(), true);
    }
    return true;
}

bool TreeNode::OnReturnKeyDown(const EventArgs& msg)
{
    TreeNode* pItem = dynamic_cast<TreeNode*>(msg.GetSender());
    ASSERT(pItem != nullptr);
    ASSERT_UNUSED_VARIABLE(pItem == this);
    if ((msg.eventType == kEventReturn) && !IsKeyDown(msg, ModifierKey::kControl)) {
        if (IsActivatable()) {
            Activate(nullptr);
        }
    }
    return true;
}

bool TreeNode::OnNodeCheckStatusChanged(const EventArgs& msg)
{
    TreeNode* pItem = dynamic_cast<TreeNode*>(msg.GetSender());
    ASSERT(pItem != nullptr);
    ASSERT(pItem == this);
    if ((pItem != nullptr) && (m_pTreeView != nullptr)) {
        m_pTreeView->OnNodeCheckStatusChanged(pItem);
    }
    return true;
}

bool TreeNode::IsVisibleInternal() const
{
    if (!BaseClass::IsVisibleInternal()) {
        return false;
    }
    if (m_pParentTreeNode != nullptr) {
        //If the parent node is not expanded, or the parent node is invisible, then the child node is also invisible
        if (!m_pParentTreeNode->IsExpand() || !m_pParentTreeNode->IsVisible()) {
            return false;
        }
    }
    return true;
}

bool TreeNode::SupportCheckMode() const
{
    bool bHasStateImages = HasStateImages();
    if (!bHasStateImages || (m_pTreeView == nullptr)) {
        //If there is no state image (the image of the CheckBox being checked), return the default value
        return BaseClass::SupportCheckMode();
    }
    //Supported in multi-select mode; not supported in single-select mode
    return m_pTreeView->IsMultiCheckMode();
}

TreeNode* TreeNode::GetParentNode() const
{
    return m_pParentTreeNode;
}

void TreeNode::SetParentNode(TreeNode* pParentTreeNode)
{
    m_pParentTreeNode = pParentTreeNode;
}

bool TreeNode::AddChildNode(TreeNode* pTreeNode)
{
    return AddChildNodeAt(pTreeNode, GetChildNodeCount());
}

bool TreeNode::AddChildNodeAt(TreeNode* pTreeNode, const size_t iIndex)
{
    ASSERT(pTreeNode != nullptr);
    if (pTreeNode == nullptr) {
        return false;
    }
    ASSERT(m_pTreeView != nullptr);
    if (m_pTreeView == nullptr) {
        return false;
    }
    ASSERT(iIndex <= m_aTreeNodes.size());
    if (iIndex > m_aTreeNodes.size()) {
        return false;
    }
    ASSERT(std::find(m_aTreeNodes.begin(), m_aTreeNodes.end(), pTreeNode) == m_aTreeNodes.end());
    if (std::find(m_aTreeNodes.begin(), m_aTreeNodes.end(), pTreeNode) != m_aTreeNodes.end()) {
        return false;
    }
    
    ASSERT(m_uDepth < UINT16_MAX);//The maximum is 65535 levels
    if (m_uDepth >= UINT16_MAX) {
        return false;
    }

    pTreeNode->m_uDepth = m_uDepth + 1;
    pTreeNode->SetParentNode(this);
    pTreeNode->SetTreeView(m_pTreeView);
    pTreeNode->SetWindow(GetWindow());

    //Listen for the double-click event: used to expand the child node
    pTreeNode->AttachDoubleClick(UiBind(&TreeNode::OnDoubleClickItem, pTreeNode, std::placeholders::_1));

    //Listen for the return key event: used to activate the child node
    pTreeNode->AttachReturn(UiBind(&TreeNode::OnReturnKeyDown, pTreeNode, std::placeholders::_1));
    
    //Listen for the check event: used to synchronize the check status of child nodes and the tri-state check status of the parent node in multi-select mode
    pTreeNode->AttachCheck(UiBind(&TreeNode::OnNodeCheckStatusChanged, pTreeNode, std::placeholders::_1));
    pTreeNode->AttachUnCheck(UiBind(&TreeNode::OnNodeCheckStatusChanged, pTreeNode, std::placeholders::_1));

    UiPadding padding = GetPadding();
    
    if (m_uDepth != 0) {
        //If the current node is not the root node (the m_uDepth of the root node is 0), one level of indentation needs to be added
        padding.left += m_pTreeView->GetIndent();
    }
    pTreeNode->SetPadding(padding, false);

    //[Collapsed/expanded] image flag
    DString expandImageClass = m_pTreeView->GetExpandImageClass();
    pTreeNode->SetExpandImageClass(expandImageClass);

    //CheckBox option
    DString checkBoxClass = m_pTreeView->GetCheckBoxClass();
    pTreeNode->SetCheckBoxClass(checkBoxClass);

    //Whether to display the icon
    pTreeNode->SetEnableIcon(m_pTreeView->IsEnableIcon());

    //Add to the ListBox container
    size_t nInsertIndex = GetDescendantNodeMaxListBoxIndex(iIndex);
    if (!Box::IsValidItemIndex(nInsertIndex)) {
        //The first node
        nInsertIndex = 0;
    }
    else {
        //If it is not the first node, the insertion position needs to be after all descendant nodes
        nInsertIndex += 1;
    }
    ASSERT(nInsertIndex <= m_pTreeView->ListBox::GetItemCount());
    m_aTreeNodes.insert(m_aTreeNodes.begin() + iIndex, pTreeNode);
    bool bAdded = m_pTreeView->ListBox::AddItemAt(pTreeNode, nInsertIndex);
    if (bAdded) {
        if (SupportCheckMode()) {
            //The state of the newly added node follows the parent node
            pTreeNode->SetChecked(IsChecked());
            //Update the check status of the node
            UpdateSelfCheckStatus();
            UpdateParentCheckStatus(false);
        }
    }
    else {
        //If adding fails, remove it
        auto iter = std::find(m_aTreeNodes.begin(), m_aTreeNodes.end(), pTreeNode);
        if (iter != m_aTreeNodes.end()) {
            m_aTreeNodes.erase(iter);
        }
    }
    return bAdded;
}

#ifdef DUILIB_BUILD_FOR_WIN

void TreeNode::SetBkIcon(HICON hIcon, uint32_t nIconSize, bool bNeedDpiScale)
{
    if (hIcon == nullptr) {
        ClearBkIcon();
        return;
    }
    uint32_t nIconID = GlobalManager::Instance().Icon().AddIcon(hIcon);
    SetBkIconID(nIconID, nIconSize, bNeedDpiScale);
}
#endif //DUILIB_BUILD_FOR_WIN

void TreeNode::SetBkIconID(uint32_t nIconID, uint32_t nIconSize, bool bNeedDpiScale)
{
    IconManager& iconManager = GlobalManager::Instance().Icon();
    DString iconString = iconManager.GetIconString(nIconID);
    if (iconString.empty()) {
        ClearBkIcon();
        return;
    }

    Image iconImage;
    DString iconImagePath;
    if (iconManager.IsImageString(nIconID)) {
        //Image resource (using the image resource path and resource attributes)
        DString iconImageString = iconManager.GetImageString(nIconID);        
        iconImage.SetImageString(iconImageString, GetWindow() != nullptr ? GetWindow()->Dpi() : GlobalManager::Instance().Dpi());
        iconImagePath = iconImage.GetImagePath();
        if (!iconImagePath.empty()) {
            //Replace with the resource path of the resource image
            iconString = iconImagePath;
        }
    }

    if (nIconSize > 0) {        
        DString dpiScale = bNeedDpiScale ? _T("true") : _T("false");
        iconString = StringUtil::Printf(_T("file='%s' width='%d' height='%d' halign='left' valign='center' dpi_scale='%s'"),
                                        iconString.c_str(), nIconSize, nIconSize, dpiScale.c_str());

    }
    else {
        if (!iconImagePath.empty()) {
            //Image resource: use the specified attributes first
            iconString = iconImage.GetImageString();
        }
        else {
            //Image data: use the original image size
            iconString = StringUtil::Printf(_T("file='%s' halign='left' valign='center'"), iconString.c_str());
        }
    }

    DString oldIconString = GetBkImage();
    if (iconString == oldIconString) {
        //No change, return directly
        return;
    }
    if (!oldIconString.empty()) {
        //The old icon exists; first hide the original icon
        ClearBkIcon();
    }

    SetBkImage(iconString);
    AdjustIconPadding();

    //Modify whether to display the icon flag as needed
    if (m_pTreeView != nullptr) {
        SetEnableIcon(m_pTreeView->IsEnableIcon());
    }
}

void TreeNode::ClearBkIcon()
{
    SetBkImage(_T(""));
    m_expandIconPadding = 0;
    m_checkBoxIconPadding = 0;
    AdjustIconPadding();
}

void TreeNode::SetExpandImageClass(const DString& expandClass)
{
    if (!expandClass.empty()) {
        //Enable the expand flag feature
        SetClass(expandClass);
    }
    else {
        //Disable the expand flag feature
        m_expandImage.reset();
        m_collapseImage.reset();
        if (m_pExpandImageRect != nullptr) {
            delete m_pExpandImageRect;
            m_pExpandImageRect = nullptr;
        }
        if (m_pCollapseImageRect != nullptr) {
            delete m_pCollapseImageRect;
            m_pCollapseImageRect = nullptr;
        }
    }
    AdjustExpandImagePadding();
}

bool TreeNode::SetCheckBoxClass(const DString& checkBoxClass)
{
    bool bSetOk = true;
    if (!checkBoxClass.empty()) {
        //Enable the CheckBox feature
        SetClass(checkBoxClass);
        if (!HasStateImage(kStateImageBk) && !HasStateImage(kStateImageSelectedBk)) {
            ASSERT(!"TreeNode::SetCheckBoxClass failed!");
            bSetOk = false;
        }
    }
    else {
        //Disable the CheckBox feature
        ClearStateImages();
    }
    AdjustCheckBoxPadding();
    return bSetOk;
}

void TreeNode::AdjustExpandImagePadding()
{
    uint16_t expandPadding = ui::TruncateToUInt8(GetExpandImagePadding());
    if (expandPadding != 0) {
        //Display the [expand/collapse] flag
        if (m_expandCheckBoxPadding == 0) {
            int32_t leftOffset = (int32_t)expandPadding;
            if (AdjustStateImagesMarginLeft(leftOffset, false)) {
                m_expandCheckBoxPadding = expandPadding;
            }
        }
        else if (!HasStateImage(kStateImageBk)) {
            //The CheckBox icon is already hidden
            m_expandCheckBoxPadding = 0;
        }

        if (m_expandIconPadding == 0) {
            //There is a CheckBox state image; the margin of the background image needs to be set to avoid the two images overlapping
            UiMargin rcBkMargin = GetBkImageMargin();
            rcBkMargin.left += expandPadding;
            if (SetBkImageMargin(rcBkMargin, false)) {
                m_expandIconPadding = expandPadding;
            }
        }

        if (m_expandTextPadding == 0) {
            //Set the padding of the text
            UiPadding rcTextPadding = GetTextPadding();
            rcTextPadding.left += expandPadding;
            SetTextPadding(rcTextPadding, false);
            m_expandTextPadding = expandPadding;
        }
    }
    else {
        //Do not display the [expand/collapse] flag
        if (m_expandCheckBoxPadding > 0) {
            int32_t leftOffset = -(int32_t)m_expandCheckBoxPadding;
            AdjustStateImagesMarginLeft(leftOffset, false);
            m_expandCheckBoxPadding = 0;
        }

        if (m_expandIconPadding > 0) {
            UiMargin rcBkMargin = GetBkImageMargin();
            rcBkMargin.left -= (int32_t)m_expandIconPadding;
            if (rcBkMargin.left >= 0) {
                SetBkImageMargin(rcBkMargin, false);
            }
            m_expandIconPadding = 0;
        }
        if (m_expandTextPadding > 0) {
            UiPadding rcTextPadding = GetTextPadding();
            rcTextPadding.left -= (int32_t)m_expandTextPadding;
            if (rcTextPadding.left >= 0) {
                SetTextPadding(rcTextPadding, false);
            }
            m_expandTextPadding = 0;
        }        
    }
    Invalidate();
}

void TreeNode::AdjustCheckBoxPadding()
{
    if (HasStateImage(kStateImageBk)) {
        //Display the CheckBox
        uint16_t extraPadding = GetCheckBoxIndent();
        UiSize imageSize = GetStateImageSize(kStateImageBk, kControlStateNormal);
        uint16_t checkBoxPadding = TruncateToUInt16(imageSize.cx);
        if (checkBoxPadding > 0) {
            checkBoxPadding += extraPadding;
        }
        
        if ((checkBoxPadding > 0) && (m_checkBoxIconPadding == 0)){
            //There is a CheckBox state image; the margin of the background image needs to be set to avoid the two images overlapping
            UiMargin rcBkMargin = GetBkImageMargin();
            rcBkMargin.left += checkBoxPadding;
            if (SetBkImageMargin(rcBkMargin, false)) {
                m_checkBoxIconPadding = checkBoxPadding;
            }            
        }

        if ((checkBoxPadding > 0) && (m_checkBoxTextPadding == 0)) {
            //Set the padding of the text
            UiPadding rcTextPadding = GetTextPadding();
            rcTextPadding.left += checkBoxPadding;
            SetTextPadding(rcTextPadding, false);
            m_checkBoxTextPadding = checkBoxPadding;
        }
    }
    else {
        //Hide the CheckBox
        if (m_checkBoxIconPadding > 0) {
            UiMargin rcBkMargin = GetBkImageMargin();
            rcBkMargin.left -= (int32_t)m_checkBoxIconPadding;
            if (rcBkMargin.left >= 0) {
                SetBkImageMargin(rcBkMargin, false);
            }
            m_checkBoxIconPadding = 0;
        }
        if (m_checkBoxTextPadding > 0) {
            UiPadding rcTextPadding = GetTextPadding();
            rcTextPadding.left -= (int32_t)m_checkBoxTextPadding;
            if (rcTextPadding.left >= 0) {
                SetTextPadding(rcTextPadding, false);
            }
            m_checkBoxTextPadding = 0;
        }
    }
    AdjustExpandImagePadding();
}

void TreeNode::AdjustIconPadding()
{
    DString iconString = GetBkImagePath();
    if (!IsBkImagePaintEnabled()) {
        //Icon drawing is disabled
        iconString.clear();
    }
    if (!iconString.empty()) {
        //Display the icon
        if (m_iconTextPadding == 0) {
            const uint16_t extraPadding = GetIconIndent();
            UiSize imageSize = GetBkImageSize();
            uint16_t iconTextPadding = TruncateToUInt16(imageSize.cx);
            if (iconTextPadding > 0) {
                iconTextPadding += extraPadding;
            }
            if (iconTextPadding > 0) {
                //Set the padding of the text
                UiPadding rcTextPadding = GetTextPadding();
                rcTextPadding.left += iconTextPadding;
                SetTextPadding(rcTextPadding, false);
                m_iconTextPadding = iconTextPadding;
            }
        }
    }
    else {
        //Hide the icon
        if (m_iconTextPadding > 0) {
            UiPadding rcTextPadding = GetTextPadding();
            rcTextPadding.left -= (int32_t)m_iconTextPadding;
            if (rcTextPadding.left >= 0) {
                SetTextPadding(rcTextPadding, false);
            }
            m_iconTextPadding = 0;
        }
    }
    AdjustCheckBoxPadding();
}

void TreeNode::SetEnableIcon(bool bEnable)
{
    if (IsBkImagePaintEnabled() != bEnable) {
        SetBkImagePaintEnabled(bEnable);
        AdjustIconPadding();
    }
}

void TreeNode::SetChildrenCheckStatus(bool bChecked)
{
    if (!SupportCheckMode()) {
        //Single-select or no CheckBox displayed: ignore
        return;
    }
    for (TreeNode* pTreeNode : m_aTreeNodes) {
        if (pTreeNode) {
            pTreeNode->SetChecked(bChecked, false);
            pTreeNode->SetChildrenCheckStatus(bChecked);
        }
    }
}

void TreeNode::UpdateParentCheckStatus(bool bUpdateSelf)
{
    if (!SupportCheckMode()) {
        //Single-select or no CheckBox displayed: ignore
        return;
    }
    if (bUpdateSelf) {
        UpdateSelfCheckStatus();
    }
    if (m_pParentTreeNode != nullptr) {
        m_pParentTreeNode->UpdateParentCheckStatus(true);
    }
}

void TreeNode::UpdateSelfCheckStatus()
{
    if (!SupportCheckMode()) {
        //Single-select or no CheckBox displayed: ignore
        return;
    }
    bool bChecked = IsChecked();
    TreeNodeCheck nodeCheck = GetChildrenCheckStatus();//Modify the check status of the current node according to the check status of the child nodes
    if (nodeCheck == TreeNodeCheck::UnCheck) {
        if (!bChecked) {
            return;
        }
        else {
            //Update to: TreeNodeCheck::UnCheck
            SetChecked(false);
            SetPartChecked(false);
            Invalidate();
        }
    }
    else if (nodeCheck == TreeNodeCheck::CheckedAll) {
        //Update to: TreeNodeCheck::CheckedAll
        if (bChecked) {
            if (IsPartChecked()) {
                SetPartChecked(false);
                Invalidate();
            }
        }
        else {            
            SetChecked(true);
            SetPartChecked(false);
            Invalidate();
        }
    }
    else if (nodeCheck == TreeNodeCheck::CheckedPart) {
        //Update to: TreeNodeCheck::CheckedPart
        SetChecked(true);
        SetPartChecked(true);
        Invalidate();
    }
}

TreeNodeCheck TreeNode::GetCheckStatus(void) const
{
    if (!SupportCheckMode()) {
        //Single-select or no CheckBox displayed: only judge the result by the state of the current node
        return IsSelected() ? TreeNodeCheck::CheckedAll : TreeNodeCheck::UnCheck;
    }

    //Multi-select
    bool bChecked = IsChecked();
    for (TreeNode* pTreeNode : m_aTreeNodes) {
        if (pTreeNode == nullptr) {
            continue;
        }
        if (bChecked != pTreeNode->IsChecked()) {
            return TreeNodeCheck::CheckedPart;
        }
    }

    for (TreeNode* pTreeNode : m_aTreeNodes) {
        if (pTreeNode == nullptr) {
            continue;
        }
        TreeNodeCheck childCheck = pTreeNode->GetCheckStatus();
        if (bChecked) {
            if (childCheck == TreeNodeCheck::UnCheck) {
                return TreeNodeCheck::CheckedPart;
            }
            else if (childCheck == TreeNodeCheck::CheckedPart) {
                return TreeNodeCheck::CheckedPart;
            }
        }
        else {
            if (childCheck == TreeNodeCheck::CheckedAll) {
                return TreeNodeCheck::CheckedPart;
            }
            else if (childCheck == TreeNodeCheck::CheckedPart) {
                return TreeNodeCheck::CheckedPart;
            }
        }
    }
    return bChecked ? TreeNodeCheck::CheckedAll : TreeNodeCheck::UnCheck;
}

TreeNodeCheck TreeNode::GetChildrenCheckStatus(void) const
{
    if (!SupportCheckMode()) {
        //Single-select or no CheckBox displayed: only judge the result by the state of the current node
        return IsSelected() ? TreeNodeCheck::CheckedAll : TreeNodeCheck::UnCheck;
    }

    if (m_aTreeNodes.empty()) {
        //No child nodes: return the state of the current node
        return IsChecked() ? TreeNodeCheck::CheckedAll : TreeNodeCheck::UnCheck;
    }
    //Multi-select: first scan the first-level child nodes
    bool bLastChecked = false;
    bool bSetLastChecked = false;
    for (TreeNode* pTreeNode : m_aTreeNodes) {
        if (pTreeNode == nullptr) {
            continue;
        }
        bool bChildChecked = pTreeNode->IsChecked();
        if (!bSetLastChecked) {
            bLastChecked = bChildChecked;
            bSetLastChecked = true;
        }
        else {
            if (bLastChecked != bChildChecked) {
                return TreeNodeCheck::CheckedPart;
            }
        }        
    }

    //Then scan the multi-level child nodes
    TreeNodeCheck lastChildCheck = TreeNodeCheck::UnCheck;
    bool bSetLastChildCheck = false;
    for (TreeNode* pTreeNode : m_aTreeNodes) {
        if (pTreeNode == nullptr){
            continue;
        }
        TreeNodeCheck childSelect = pTreeNode->GetCheckStatus();//Get the check status including itself/child nodes
        if (childSelect == TreeNodeCheck::CheckedPart) {
            return TreeNodeCheck::CheckedPart;
        }
        if (!bSetLastChildCheck) {
            lastChildCheck = childSelect;
            bSetLastChildCheck = true;
        }
        else {
            if (childSelect != lastChildCheck) {
                return TreeNodeCheck::CheckedPart;
            }
        }
    }

    //If all the states are the same, return the state of the first child node
    TreeNode* pTreeNode = m_aTreeNodes.front();
    if (pTreeNode != nullptr) {
        return pTreeNode->IsChecked() ? TreeNodeCheck::CheckedAll : TreeNodeCheck::UnCheck;
    }
    else {
        return IsChecked() ? TreeNodeCheck::CheckedAll : TreeNodeCheck::UnCheck;
    }
}

bool TreeNode::RemoveChildNodeAt(size_t iIndex, bool bUpdateCheckStatus)
{
    ASSERT(iIndex < m_aTreeNodes.size());
    if (iIndex >= m_aTreeNodes.size()) {
        return false;
    }

    bool bRemoved = false;
    TreeNode* pTreeNode = ((TreeNode*)m_aTreeNodes[iIndex]);
    m_aTreeNodes.erase(m_aTreeNodes.begin() + iIndex);
    if (pTreeNode != nullptr) {
        bRemoved = pTreeNode->RemoveSelf();
    }
    if (bUpdateCheckStatus && SupportCheckMode()) {
        //Update the check status of the node
        UpdateSelfCheckStatus();
        UpdateParentCheckStatus(false);
    }
    return bRemoved;
}

bool TreeNode::RemoveChildNodeAt(size_t iIndex)
{
    return RemoveChildNodeAt(iIndex, true);
}

bool TreeNode::RemoveChildNode(TreeNode* pTreeNode)
{
    auto it = std::find(m_aTreeNodes.begin(), m_aTreeNodes.end(), pTreeNode);
    if (it == m_aTreeNodes.end()) {
        return false;
    }        
    size_t iIndex = it - m_aTreeNodes.begin();
    return RemoveChildNodeAt(iIndex, true);
}
    
void TreeNode::RemoveAllChildNodes()
{
    while (m_aTreeNodes.size() > 0) {
        RemoveChildNodeAt(0, false);
    }
}

bool TreeNode::RemoveSelf()
{
    for(TreeNode* pTreeNode : m_aTreeNodes) {
        if (pTreeNode != nullptr) {
            pTreeNode->RemoveSelf();
        }
    }
    m_aTreeNodes.clear();

    //Remove the element from the ListBox
    bool bRemoved = false;
    size_t nListBoxIndex = GetListBoxIndex();
    if (Box::IsValidItemIndex(nListBoxIndex)) {
        ASSERT(m_pTreeView->ListBox::GetItemAt(nListBoxIndex) == this);
        bRemoved = m_pTreeView->ListBox::RemoveItemAt(nListBoxIndex);
    }
    return bRemoved;
}

size_t TreeNode::GetDescendantNodeCount() const
{
    size_t nodeCount = GetChildNodeCount();
    for (TreeNode* pTreeNode : m_aTreeNodes) {
        if (pTreeNode != nullptr) {
            nodeCount += pTreeNode->GetDescendantNodeCount();
        }
    }
    return nodeCount;
}

size_t TreeNode::GetChildNodeCount() const
{
    return m_aTreeNodes.size();
}

size_t TreeNode::GetDescendantNodeMaxListBoxIndex(size_t nInsertIndex) const
{
    size_t maxListBoxIndex = GetListBoxIndex();
    if (!Box::IsValidItemIndex(maxListBoxIndex)) {
        if (m_aTreeNodes.empty()) {
            return maxListBoxIndex;
        }
        maxListBoxIndex = 0;
    }
    for (size_t nIndex = 0; nIndex < nInsertIndex; ++nIndex) {
        if (nIndex >= m_aTreeNodes.size()) {
            break;
        }
        TreeNode* pTreeNode = m_aTreeNodes[nIndex];
        if (pTreeNode != nullptr) {
            maxListBoxIndex = std::max(pTreeNode->GetDescendantNodeMaxListBoxIndex(Box::InvalidIndex), maxListBoxIndex);
        }
    }
    return maxListBoxIndex;
}
    
TreeNode* TreeNode::GetChildNode(size_t iIndex) const
{
    if (iIndex >= m_aTreeNodes.size()) {
        return nullptr;
    }
    return m_aTreeNodes[iIndex];
}
    
size_t TreeNode::GetChildNodeIndex(TreeNode* pTreeNode) const
{
    auto it = std::find(m_aTreeNodes.begin(), m_aTreeNodes.end(), pTreeNode);
    if (it == m_aTreeNodes.end()) {
        return Box::InvalidIndex;
    }
    return it - m_aTreeNodes.begin();
}

void TreeNode::GetChildNodes(std::vector<TreeNode*>& childNodes) const
{
    childNodes.clear();
    const size_t nCount = m_aTreeNodes.size();
    for (size_t nIndex = 0; nIndex < nCount; ++nIndex) {
        TreeNode* pChildNode = m_aTreeNodes[nIndex];
        if (pChildNode != nullptr) {
            childNodes.push_back(pChildNode);
        }
    }
}

TreeNode* TreeNode::FindChildNodeByName(const DString& name, bool bRecursive) const
{
    for (TreeNode* pNode : m_aTreeNodes) {
        if (pNode != nullptr) {
            if (pNode->IsNameEquals(name)) {
                return pNode;
            }
        }
    }
    if (!bRecursive) {
        return nullptr;
    }
    //Search recursively, including grandchild nodes and other multi-level child nodes
    for (TreeNode* pNode : m_aTreeNodes) {
        if (pNode != nullptr) {
            TreeNode* pFoundNode = pNode->FindChildNodeByName(name, bRecursive);
            if (pFoundNode != nullptr) {
                return pFoundNode;
            }
        }
    }
    return nullptr;
}

TreeNode* TreeNode::FindChildNodeByText(const DString& text, bool bRecursive) const
{
    for (TreeNode* pNode : m_aTreeNodes) {
        if (pNode != nullptr) {
            if (pNode->IsTextEquals(text)) {
                return pNode;
            }
        }
    }
    if (!bRecursive) {
        return nullptr;
    }
    //Search recursively, including grandchild nodes and other multi-level child nodes
    for (TreeNode* pNode : m_aTreeNodes) {
        if (pNode != nullptr) {
            TreeNode* pFoundNode = pNode->FindChildNodeByText(text, bRecursive);
            if (pFoundNode != nullptr) {
                return pFoundNode;
            }
        }
    }
    return nullptr;
}

bool TreeNode::IsExpand() const
{
    return m_bExpand;
}

void TreeNode::SetExpand(bool bExpand, bool bTriggerEvent)
{
    if(m_bExpand == bExpand) {
        return;
    }
    m_bExpand = bExpand;

    if (bTriggerEvent) {
        SendEvent(m_bExpand ? kEventExpand : kEventCollapse);
    }
    if (m_pTreeView != nullptr) {
        m_pTreeView->Arrange();
    }    
}

uint16_t TreeNode::GetDepth() const
{
    return m_uDepth;
}

TreeView::TreeView(Window* pWindow) :
    ListBox(pWindow, new VLayout),
    m_iIndent(0),
    m_rootNode(),
    m_bEnableIcon(true)
{
    m_bMultiCheckMode = BaseClass::IsMultiSelect();
    m_rootNode.reset(new TreeNode(pWindow));
    m_rootNode->SetTreeView(this);
    //The indent defaults to 20 pixels
    SetIndent(20, true);
}

TreeView::~TreeView()
{
    m_rootNode->RemoveSelf();
    m_rootNode.reset();
}

DString TreeView::GetType() const { return DUI_CTR_TREEVIEW; }

void TreeView::SetAttribute(const DString& strName, const DString& strValue)
{
    //List of supported attributes: those implemented by the base class are forwarded directly
    if (strName == _T("indent")) {
        //The indent of the tree node (each level of nodes is indented by one indent unit)
        SetIndent(StringUtil::StringToInt32(strValue), true);
    }
    else if (strName == _T("multi_select")) {
        //Multi-select; the default is single-select, implemented in the base class
        SetMultiSelect(strValue == _T("true"));
    }
    else if (strName == _T("check_box_class")) {
        //Whether to display the CheckBox
        SetCheckBoxClass(strValue);
    }
    else if (strName == _T("expand_image_class")) {
        //Whether to display the [expand/collapse] icon
        SetExpandImageClass(strValue);
    }
    else if (strName == _T("show_icon")) {
        //Whether to display the icon
        SetEnableIcon(strValue == _T("true"));
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

void TreeView::ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale)
{
    if (!Dpi().CheckDisplayScaleFactor(nNewDpiScale)) {
        return;
    }
    int32_t iValue = GetIndent();
    iValue = Dpi().GetScaleInt(iValue, nOldDpiScale);
    SetIndent(iValue, false);

    BaseClass::ChangeDpiScale(nOldDpiScale, nNewDpiScale);
}

bool TreeView::IsMultiSelect() const
{
    if (!m_checkBoxClass.empty()) {
        //If the CheckBox is displayed, the Check mode takes precedence; the selection state of the tree is handled as single-select
        if (IsMultiCheckMode()) {
            return false;
        }        
    }
    return BaseClass::IsMultiSelect();
}

void TreeView::SetMultiSelect(bool bMultiSelect)
{
    bool bSelectChanged = (bMultiSelect != BaseClass::IsMultiSelect()) ||
                          (m_bMultiCheckMode != bMultiSelect);
    if (!bSelectChanged) {
        return;
    }
    bool bOldCheckMode = IsMultiCheckMode();
    m_bMultiCheckMode = bMultiSelect;
    BaseClass::SetMultiSelect(bMultiSelect);

    bool isChanged = false;
    if (IsMultiCheckMode()) {
        //Switch to: Check mode (i.e. when the CheckBox is displayed)
        if (OnCheckBoxShown()) {
            isChanged = true;
        }
    }
    else if (IsMultiSelect()) {
        //Switch to multi-select mode (i.e. when the CheckBox is hidden)
        if (OnCheckBoxHided()) {
            isChanged = true;
        }
    }
    else {
        //Switch to single-select mode
        if (bOldCheckMode) {
            //Switch from Check mode to single-select: the currently selected item needs to be synchronized first to avoid the problem of inconsistent selection after switching (an unchecked item becomes the currently selected item)
            if (UpdateCurSelItemCheckStatus()) {
                isChanged = true;
            }
        }
        //Switch to single-select mode: ensure that the data in the ListBox is single-select
        if (OnSwitchToSingleSelect()) {
            isChanged = true;
        }
    }
    if (isChanged) {
        Invalidate();
    }
}

bool TreeView::IsMultiCheckMode() const
{
    if (!m_checkBoxClass.empty()) {
        ASSERT(m_bMultiCheckMode == BaseClass::IsMultiSelect());
        return m_bMultiCheckMode;
    }
    return false;
}

bool TreeView::CanPaintSelectedColors(bool bHasStateImages) const
{
    if (bHasStateImages && IsMultiCheckMode()) {
        //If there is a CheckBox, in Check mode the selected background color is not displayed by default
        return false;
    }
    return BaseClass::CanPaintSelectedColors(bHasStateImages);
}

bool TreeView::OnSwitchToSingleSelect()
{
    ASSERT(!IsMultiSelect());
    bool bChanged = BaseClass::OnSwitchToSingleSelect();
    if (IsMultiCheckMode()) {
        return bChanged;
    }
    //Already switched to single-select
    TreeNode* pItem = nullptr;
    const size_t itemCount = m_items.size();
    for (size_t i = 0; i < itemCount; ++i) {
        pItem = dynamic_cast<TreeNode*>(m_items[i]);
        if ((pItem != nullptr) && pItem->IsChecked()) {
            if (GetCurSel() != i) {
                //After switching to single-select, if it is not the currently selected item, all Checked flags are set to false
                pItem->SetChecked(false, false);
                pItem->Invalidate();
                bChanged = true;
            }
        }
    }
    return bChanged;
}

bool TreeView::UpdateCurSelItemCheckStatus()
{
    //Take the Check state as the criterion to set the currently selected item
    bool bChanged = false;
    size_t curSelIndex = GetCurSel();
    if (Box::IsValidItemIndex(curSelIndex)) {
        TreeNode* pItem = dynamic_cast<TreeNode*>(GetItemAt(curSelIndex));
        if (pItem != nullptr) {
            if (!pItem->IsChecked()) {
                SetCurSel(Box::InvalidIndex);
                pItem->SetSelected(false);
                pItem->Invalidate();
                bChanged = true;
            }
            else {
                pItem->SetSelected(true);
                pItem->Invalidate();
            }
        }
    }
    return bChanged;
}

bool TreeView::OnCheckBoxHided()
{
    ASSERT(IsMultiSelect() && !IsMultiCheckMode());
    //Synchronization direction: Check -> Select
    if (m_items.empty()) {
        return false;
    }
    bool isChaned = false;
    TreeNode* pItem = nullptr;
    const size_t itemCount = m_items.size();
    for (size_t i = 0; i < itemCount; ++i) {
        pItem = dynamic_cast<TreeNode*>(m_items[i]);
        if (pItem == nullptr) {
            continue;
        }
        //Synchronize the Check and Select flags
        if (pItem->IsSelected() != pItem->IsChecked()) {
            pItem->SetSelected(pItem->IsChecked());
            pItem->Invalidate();
            isChaned = true;
        }
        if (pItem->IsChecked()) {
            //All Check flags are set to false
            pItem->SetChecked(false);
            pItem->Invalidate();
            isChaned = true;
        }        
    }
    //Synchronize the currently selected item
    if (UpdateCurSelItemSelectStatus()) {
        isChaned = true;
    }
    return isChaned;
}

bool TreeView::OnCheckBoxShown()
{
    ASSERT(IsMultiCheckMode());
    //Synchronization direction: Select -> Check
    if (m_items.empty()) {
        return false;
    }
    bool isChanged = false;
    const size_t curSelIndex = GetCurSel();
    TreeNode* pItem = nullptr;
    const size_t itemCount = m_items.size();
    for (size_t i = 0; i < itemCount; ++i) {
        pItem = dynamic_cast<TreeNode*>(m_items[i]);
        if (pItem == nullptr) {
            continue;
        }
        //Synchronize the Select and Check flags
        if (pItem->IsChecked() != pItem->IsSelected()) {
            pItem->SetChecked(pItem->IsSelected());
            pItem->Invalidate();
            isChanged = true;
        }
        if (curSelIndex != i) {
            //Remove the selected state, because in Check mode the ListBox behaves as single-select
            if (pItem->IsSelected()) {
                pItem->SetSelected(false);
                pItem->Invalidate();
                isChanged = true;
            }
        }
    }
    //Synchronize the currently selected item    
    if (Box::IsValidItemIndex(curSelIndex)) {
        bool bSelectItem = false;
        pItem = dynamic_cast<TreeNode*>(GetItemAt(curSelIndex));
        if (pItem != nullptr) {
            bSelectItem = pItem->IsSelected();
        }
        if (!bSelectItem) {
            SetCurSel(Box::InvalidIndex);
            isChanged = true;
        }
        else if(pItem->IsChecked()){
            //Synchronize the check status
            pItem->UpdateParentCheckStatus(true);
        }
    }
    return isChanged;
}

void TreeView::SetIndent(int32_t indent, bool bNeedDpiScale)
{
    ASSERT(indent >= 0);
    if (bNeedDpiScale) {
        Dpi().ScaleInt(indent);
    }
    if (indent >= 0) {
        m_iIndent = indent;
    }    
}

void TreeView::SetCheckBoxClass(const DString& className)
{
    if (m_checkBoxClass == className) {
        return;
    }
    DString oldCheckBoxClass = m_checkBoxClass.c_str();
    m_checkBoxClass = className;
    bool bSetOk = true;
    bool hasSetOk = false;
    for (Control* pControl : m_items) {
        TreeNode* pTreeNode = dynamic_cast<TreeNode*>(pControl);
        if (pTreeNode != nullptr) {
            if (!pTreeNode->SetCheckBoxClass(className)) {
                bSetOk = false;
            }
            else {
                hasSetOk = true;
            }
        }
    }

    if (!bSetOk && !hasSetOk) {
        //Invalid setting
        ASSERT(!"TreeView::SetCheckBoxClass failed!");
        m_checkBoxClass.clear();
        if (oldCheckBoxClass == m_checkBoxClass) {
            return;
        }
    }

    bool isChanged = false;
    if (m_checkBoxClass.empty()) {
        //From displaying the CheckBox to hiding the CheckBox: two modes need to be handled (multi-select mode, single-select mode)
        //The currently selected item needs to be synchronized first to avoid the problem of inconsistent selection after switching (an unchecked item becomes the currently selected item)
        if (UpdateCurSelItemCheckStatus()) {
            isChanged = true;
        }
        if (IsMultiSelect()) {
            //Switch to multi-select mode
            if (OnCheckBoxHided()) {
                isChanged = true;
            }
        }
        else {
            //Switch to single-select mode: ensure that the data in the ListBox is single-select
            if (OnSwitchToSingleSelect()) {
                isChanged = true;
            }
        }
    }
    else {
        //From hiding the CheckBox to displaying the CheckBox: two modes need to be handled (Check mode, single-select mode)
        if (IsMultiCheckMode()) {
            //Check mode (i.e. when the CheckBox is displayed)
            isChanged = OnCheckBoxShown();
        }
        else if(!IsMultiSelect()) {
            //Single-select mode: ensure that the data in the ListBox is single-select
            if (OnSwitchToSingleSelect()) {
                isChanged = true;
            }
        }
        else {
            //This case does not exist
            ASSERT(!"ERROR!");
        }
    }
    if (isChanged) {
        Invalidate();
    }
}

DString TreeView::GetCheckBoxClass() const
{
    return m_checkBoxClass.c_str();
}

void TreeView::SetExpandImageClass(const DString& className)
{
    bool isChanged = m_expandImageClass != className;
    m_expandImageClass = className;
    if (isChanged) {
        for (Control* pControl : m_items) {
            TreeNode* pTreeNode = dynamic_cast<TreeNode*>(pControl);
            if (pTreeNode != nullptr) {
                pTreeNode->SetExpandImageClass(className);
            }
        }
    }
}

DString TreeView::GetExpandImageClass() const
{
    return m_expandImageClass.c_str();
}

void TreeView::SetEnableIcon(bool bEnable)
{
    bool isChanged = m_bEnableIcon != bEnable;
    m_bEnableIcon = bEnable;
    if (isChanged) {
        for (Control* pControl : m_items) {
            TreeNode* pTreeNode = dynamic_cast<TreeNode*>(pControl);
            if (pTreeNode != nullptr) {
                pTreeNode->SetEnableIcon(bEnable);
            }
        }
    }
}

bool TreeView::IsEnableIcon() const
{
    return m_bEnableIcon;
}

bool TreeView::InsertControlBeforeNode(TreeNode* pTreeNode, Control* pControl)
{
    if ((pTreeNode == nullptr) || (pControl == nullptr)) {
        return false;
    }
    bool bAdded = false;
    if (dynamic_cast<TreeNode*>(pControl) != nullptr) {
        //Adding tree nodes through this interface is not allowed
        return false;
    }
    size_t iIndex = pTreeNode->GetListBoxIndex();
    if (Box::IsValidItemIndex(iIndex)) {
        bAdded = ListBox::AddItemAt(pControl, iIndex);
    }
    return bAdded;
}

bool TreeView::RemoveControl(Control* pControl)
{
    if (pControl == nullptr) {
        return false;
    }
    bool bRemoved = false;
    if (dynamic_cast<TreeNode*>(pControl) != nullptr) {
        //Removing tree nodes through this interface is not allowed
        return false;
    }
    bRemoved = ListBox::RemoveItem(pControl);
    return bRemoved;
}

bool TreeView::RemoveTreeNode(TreeNode* pTreeNode)
{
    if (m_rootNode.get() == pTreeNode) {
        //The root node is not allowed to be deleted
        return false;
    }
    TreeNode* pParentTreeNode = pTreeNode->GetParentNode();
    if (pParentTreeNode == nullptr) {
        return false;
    }
    return pParentTreeNode->RemoveChildNode(pTreeNode);
}

void TreeView::RemoveAllNodes()
{
    m_rootNode->RemoveAllChildNodes();
}

bool TreeView::AddItem(Control* /*pControl*/)
{
    ASSERT(0);
    return false;
}

bool TreeView::AddItemAt(Control* /*pControl*/, size_t /*iIndex*/)
{
    ASSERT(0);
    return false;
}

bool TreeView::RemoveItem(Control* /*pControl*/)
{
    ASSERT(0);
    return false;
}

bool TreeView::RemoveItemAt(size_t /*iIndex*/)
{
    ASSERT(0);
    return false;
}

void TreeView::RemoveAllItems()
{
    ASSERT(0);
}

void TreeView::SetParent(Box* pParent)
{
    ListBox::SetParent(pParent);
    m_rootNode->SetParent(pParent);
}

void TreeView::SetWindow(Window* pWindow)
{
    BaseClass::SetWindow(pWindow);
    m_rootNode->SetWindow(pWindow);
}

void TreeView::OnNodeCheckStatusChanged(TreeNode* pTreeNode)
{
    if (pTreeNode == nullptr) {
        return;
    }
    if (!pTreeNode->SupportCheckMode()) {
        //Single-select or no CheckBox displayed: return directly
        return;
    }

    //Multi-select
    bool isChecked = pTreeNode->IsChecked();
    //Synchronize the check status of the child nodes: follow the current node
    pTreeNode->SetChildrenCheckStatus(isChecked);

    //Synchronize the check status of the parent node
    pTreeNode->UpdateParentCheckStatus(false);
}

bool TreeView::SelectTreeNode(TreeNode* pTreeNode)
{
    size_t itemIndex = GetItemIndex(pTreeNode);
    if (!Box::IsValidItemIndex(itemIndex)) {
        return false;
    }
    if (!pTreeNode->IsVisible()) {
        //Expand the parent directory
        std::vector<TreeNode*> parents;
        TreeNode* pNode = pTreeNode->GetParentNode();
        while ((pNode != nullptr) && (pNode != GetRootNode())) {
            parents.push_back(pNode);
            pNode = pNode->GetParentNode();
        }
        if (!parents.empty()) {
            for (auto iter = parents.rbegin(); iter != parents.rend(); ++iter) {
                pNode = *iter;
                if (!pNode->IsExpand()) {
                    pNode->SetExpand(true, false);
                }
            }
        }
    }
    //Update the scroll bar position
    SetPos(GetPos());

    //Ensure visibility
    EnsureVisible(itemIndex);

    //Set the selection (deselect first and then select: to avoid changing from selected to unselected in multi-select mode; to avoid not triggering the selection event when it is already selected)
    UnSelectItem(itemIndex, false);
    SelectItem(itemIndex, true, true);

    return true;
}

bool TreeView::ExpandTreeNode(TreeNode* pTreeNode)
{
    size_t itemIndex = GetItemIndex(pTreeNode);
    if (!Box::IsValidItemIndex(itemIndex)) {
        return false;
    }
    if (!pTreeNode->IsVisible()) {
        //Expand the parent directory
        std::vector<TreeNode*> parents;
        TreeNode* pNode = pTreeNode->GetParentNode();
        while ((pNode != nullptr) && (pNode != GetRootNode())) {
            parents.push_back(pNode);
            pNode = pNode->GetParentNode();
        }
        if (!parents.empty()) {
            for (auto iter = parents.rbegin(); iter != parents.rend(); ++iter) {
                pNode = *iter;
                if (!pNode->IsExpand()) {
                    pNode->SetExpand(true, false);
                }
            }
        }
    }

    //Update the scroll bar position
    SetPos(GetPos());

    //Ensure visibility
    EnsureVisible(itemIndex);

    //Expand the node
    if (!pTreeNode->IsExpand()) {
        pTreeNode->SetExpand(true, true);
    }
    return true;
}

bool TreeView::EnsureTreeNodeVisible(TreeNode* pTreeNode)
{
    size_t itemIndex = GetItemIndex(pTreeNode);
    if (!Box::IsValidItemIndex(itemIndex)) {
        return false;
    }
    if (!pTreeNode->IsVisible()) {
        //Expand the parent directory
        std::vector<TreeNode*> parents;
        TreeNode* pNode = pTreeNode->GetParentNode();
        while ((pNode != nullptr) && (pNode != GetRootNode())) {
            parents.push_back(pNode);
            pNode = pNode->GetParentNode();
        }
        if (!parents.empty()) {
            for (auto iter = parents.rbegin(); iter != parents.rend(); ++iter) {
                pNode = *iter;
                if (!pNode->IsExpand()) {
                    pNode->SetExpand(true, false);
                }
            }
        }
    }

    //Update the scroll bar position
    SetPos(GetPos());

    //Ensure visibility
    EnsureVisible(itemIndex);
    return true;
}

bool TreeView::IsValidTreeNode(TreeNode* pTreeNode) const
{
    size_t itemIndex = GetItemIndex(pTreeNode);
    return BaseClass::IsValidItemIndex(itemIndex);
}

size_t TreeView::GetDisplayItemCount(bool bIsHorizontal, size_t& nColumns, size_t& nRows) const
{
    size_t nCount = 0;
    nRows = 0;
    nColumns = 1;
    if (bIsHorizontal) {
        //This case does not currently exist: the vertical layout is already fixed
        nCount = BaseClass::GetDisplayItemCount(bIsHorizontal, nColumns, nRows);
        ASSERT(0);
    }
    else {
        const size_t nItemCount = GetItemCount();
        for (size_t nItemIndex = 0; nItemIndex < nItemCount; ++nItemIndex) {
            Control* pControl = GetItemAt(nItemIndex);
            if ((pControl == nullptr) || !pControl->IsVisible() || pControl->IsFloat()) {
                continue;
            }
            ++nCount;
        }
    }
    nRows = nCount;
    return nCount;
}

}
