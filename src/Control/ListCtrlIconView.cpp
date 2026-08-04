#include "dui/Control/ListCtrlIconView.h" 
#include "dui/Control/ListCtrl.h"
#include "dui/Control/ListCtrlData.h"

namespace ui
{
ListCtrlIconView::ListCtrlIconView(Window* pWindow):
    ListCtrlView(pWindow, new VirtualVTileLayout),
    m_pListCtrl(nullptr),
    m_pData(nullptr),
    m_bSingleLine(false),
    m_bSingleLineFlag(false)
{
    VirtualVTileLayout* pDataLayout = dynamic_cast<VirtualVTileLayout*>(GetLayout());
    ASSERT(pDataLayout != nullptr);
    VirtualLayout* pVirtualLayout = pDataLayout;
    SetVirtualLayout(pVirtualLayout);
}

ListCtrlIconView::~ListCtrlIconView()
{
}

void ListCtrlIconView::SetAttribute(const DString& strName, const DString& strValue)
{
    if (strName == _T("horizontal_layout")) {
        SetHorizontalLayout(strValue == _T("true"));
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

void ListCtrlIconViewItem::HandleEvent(const EventArgs& msg)
{
    BaseClass::HandleEvent(msg);
    if (m_pListCtrl != nullptr) {
        if ((msg.eventType > kEventKeyBegin) && (msg.eventType < kEventKeyEnd)) {
            m_pListCtrl->OnViewKeyboardEvents(msg);
        }
        else if ((msg.eventType > kEventMouseBegin) && (msg.eventType < kEventMouseEnd)) {
            m_pListCtrl->OnViewMouseEvents(msg);
        }
    }
}

void ListCtrlIconView::HandleEvent(const EventArgs& msg)
{
    BaseClass::HandleEvent(msg);
    if (m_pListCtrl != nullptr) {
        if ((msg.eventType > kEventKeyBegin) && (msg.eventType < kEventKeyEnd)) {
            m_pListCtrl->OnViewKeyboardEvents(msg);
        }
        else if ((msg.eventType > kEventMouseBegin) && (msg.eventType < kEventMouseEnd)) {
            m_pListCtrl->OnViewMouseEvents(msg);
        }
    }
}

void ListCtrlIconView::SetHorizontalLayout(bool bHorizontal)
{
    if (bHorizontal) {
        //Horizontal layout        
        if (dynamic_cast<VirtualHTileLayout*>(GetLayout()) == nullptr) {
            Layout* pLayout = GetLayout();
            VirtualVTileLayout* pOldLayout = dynamic_cast<VirtualVTileLayout*>(pLayout);
            VirtualHTileLayout* pNewLayout = new VirtualHTileLayout;
            ResetLayout(pNewLayout);
            SetVirtualLayout(pNewLayout);
            if (pOldLayout != nullptr) {
                //Synchronize attributes
                pNewLayout->SetItemSize(pOldLayout->GetItemSize());
                pNewLayout->SetRows(pOldLayout->GetColumns());
                pNewLayout->SetAutoCalcRows(pOldLayout->IsAutoCalcColumns());
                pNewLayout->SetChildMarginX(pOldLayout->GetChildMarginX());
                pNewLayout->SetChildMarginY(pOldLayout->GetChildMarginY());
                pNewLayout->SetScaleDown(pOldLayout->IsScaleDown());
            }
            FreeLayout(pLayout);
        }
    }
    else {
        //Vertical layout
        if (dynamic_cast<VirtualVTileLayout*>(GetLayout()) == nullptr) {
            Layout* pLayout = GetLayout();
            VirtualHTileLayout* pOldLayout = dynamic_cast<VirtualHTileLayout*>(pLayout);
            VirtualVTileLayout* pNewLayout = new VirtualVTileLayout;
            ResetLayout(pNewLayout);
            SetVirtualLayout(pNewLayout);
            if (pOldLayout != nullptr) {
                //Synchronize attributes
                pNewLayout->SetItemSize(pOldLayout->GetItemSize());
                pNewLayout->SetColumns(pOldLayout->GetRows());
                pNewLayout->SetAutoCalcColumns(pOldLayout->IsAutoCalcRows());
                pNewLayout->SetChildMarginX(pOldLayout->GetChildMarginX());
                pNewLayout->SetChildMarginY(pOldLayout->GetChildMarginY());
                pNewLayout->SetScaleDown(pOldLayout->IsScaleDown());
            }
            FreeLayout(pLayout);
        }
    }
}

bool ListCtrlIconView::IsHorizontalLayout() const
{
    VirtualHTileLayout* pDataLayout = dynamic_cast<VirtualHTileLayout*>(GetLayout());
    return pDataLayout != nullptr;
}

UiSize ListCtrlIconView::GetItemSize() const
{
    UiSize szItem;
    Layout* pLayout = GetLayout();
    VirtualVTileLayout* pVTileLayout = dynamic_cast<VirtualVTileLayout*>(pLayout);
    if (pVTileLayout != nullptr) {
        szItem = pVTileLayout->GetItemSize();
    }
    else {
        VirtualHTileLayout* pHTileLayout = dynamic_cast<VirtualHTileLayout*>(pLayout);
        if (pHTileLayout != nullptr) {
            szItem = pHTileLayout->GetItemSize();
        }
    }
    return szItem;
}

void ListCtrlIconView::SetItemSize(const UiSize& szItem)
{
    Layout* pLayout = GetLayout();
    VirtualVTileLayout* pVTileLayout = dynamic_cast<VirtualVTileLayout*>(pLayout);
    if (pVTileLayout != nullptr) {
        pVTileLayout->SetItemSize(szItem);
    }
    else {
        VirtualHTileLayout* pHTileLayout = dynamic_cast<VirtualHTileLayout*>(pLayout);
        if (pHTileLayout != nullptr) {
            pHTileLayout->SetItemSize(szItem);
        }
    }
}

void ListCtrlIconView::SetTextSingleLine(bool bSingleLine)
{
    m_bSingleLine = bSingleLine;
    m_bSingleLineFlag = true;
}

bool ListCtrlIconView::IsTextSingleLine(bool& bSingleLine) const
{
    if (m_bSingleLineFlag) {
        bSingleLine = m_bSingleLine;
    }
    return m_bSingleLineFlag;
}

void ListCtrlIconView::SetListCtrl(ListCtrl* pListCtrl)
{
    m_pListCtrl = pListCtrl;
}

void ListCtrlIconView::SetDataProvider(VirtualListBoxElement* pProvider)
{
    BaseClass::SetDataProvider(pProvider);
    m_pData = dynamic_cast<ListCtrlData*>(GetDataProvider());
}

void ListCtrlIconView::Refresh(bool bSync)
{
    if ((m_pListCtrl != nullptr) && !m_pListCtrl->IsEnableRefresh()) {
        //The refresh function has been disabled
        return;
    }
    if ((m_pListCtrl != nullptr) && !m_pListCtrl->IsInited()) {
        return;
    }
    if ((GetWindow() == nullptr) || !HasDataProvider()) {
        return;
    }
    BaseClass::Refresh(bSync);
}

Control* ListCtrlIconView::CreateDataItem()
{
    ASSERT(m_pListCtrl != nullptr);
    if (m_pListCtrl == nullptr) {
        return nullptr;
    }
    //Icon view
    ListCtrlIconViewItem* pItem = new ListCtrlIconViewItem(GetWindow());
    pItem->SetListCtrl(m_pListCtrl);
    pItem->SetClass(m_pListCtrl->GetIconViewItemClass());
    ListCtrlIcon* pItemImage = new ListCtrlIcon(GetWindow());
    ListCtrlLabel* pItemLabel = new ListCtrlLabel(GetWindow());
    pItemImage->SetListBoxItem(pItem);
    pItemLabel->SetListBoxItem(pItem);
    pItem->AddItem(pItemImage);
    pItem->AddItem(pItemLabel);
    return pItem;
}

bool ListCtrlIconView::FillDataItem(Control* pControl,
                                    size_t nElementIndex,
                                    const ListCtrlItemData& itemData,
                                    const std::vector<ListCtrlSubItemData2Pair>& subItemList)
{
    ASSERT((pControl != nullptr) && (m_pListCtrl != nullptr));
    if ((pControl == nullptr) || (m_pListCtrl == nullptr)) {
        return false;
    }
    ListCtrlSubItemData2Ptr pSubItemData;
    int32_t nImageId = -1;
    size_t nColumnId = m_pListCtrl->GetColumnId(0); //Get the ID of the first column
    for (const ListCtrlSubItemData2Pair& pair : subItemList) {
        if (pair.nColumnId == nColumnId) {
            if (pair.pSubItemData != nullptr) {
                nImageId = pair.pSubItemData->nImageId;
                pSubItemData = pair.pSubItemData;
            }
            break;
        }
    }
    if (nImageId < 0) {
        //If the column has no icon set, use the row's
        nImageId = itemData.nImageId;
    }
    ListCtrlIconViewItem* pViewItem = dynamic_cast<ListCtrlIconViewItem*>(pControl);
    ASSERT(pViewItem != nullptr);
    if (pViewItem == nullptr) {
        return false;
    }

    ListCtrlIcon* pItemImage = dynamic_cast<ListCtrlIcon*>(pViewItem->GetItemAt(0));
    ListCtrlLabel* pItemLabel = dynamic_cast<ListCtrlLabel*>(pViewItem->GetItemAt(1));
    ASSERT((pItemImage != nullptr) && (pItemLabel != nullptr));
    if ((pItemImage == nullptr) || (pItemLabel == nullptr)) {
        return false;
    }

    //Attributes of the icon
    pItemImage->SetClass(m_pListCtrl->GetIconViewItemImageClass());
    //Attributes of the text
    pItemLabel->SetClass(m_pListCtrl->GetIconViewItemLabelClass());

    DString imageString;
    UiSize imageSize;
    if (nImageId >= 0) {
        ImageListPtr pImageList = m_pListCtrl->GetImageList(ListCtrlType::Icon);
        if (pImageList == nullptr) {
            pImageList = m_pListCtrl->GetImageList(ListCtrlType::Report);
        }
        if (pImageList != nullptr) {
            imageString = pImageList->GetImageString(nImageId);
            imageSize = pImageList->GetImageSize();
        }
    }
    pItemImage->SetBkImage(imageString);
    if (imageSize.cx > 0) {
        pItemImage->SetFixedWidth(UiFixedInt(imageSize.cx), false, false);
    }
    if (imageSize.cy > 0) {
        pItemImage->SetFixedHeight(UiFixedInt(imageSize.cy), false, false);
    }
    if (m_bSingleLineFlag) {
        pItemLabel->SetSingleLine(m_bSingleLine);
    }
    if (pSubItemData != nullptr) {
        pItemLabel->SetText(pSubItemData->text.c_str());
    }
    else {
        pItemLabel->SetText(_T(""));
    }

    //Set attributes such as not getting focus
    pItemImage->SetNoFocus();
    pItemLabel->SetNoFocus();

    //Set the editable attribute
    const EventCallbackID callbackID = (EventCallbackID)this;
    bool bEditable = (pSubItemData != nullptr) ? pSubItemData->bEditable : false;
    if (bEditable && m_pListCtrl->IsEnableItemEdit()) {
        IListBoxItem* pItem = dynamic_cast<IListBoxItem*>(pControl);
        ListCtrlLabel* pSubItem = pItemLabel;
        ASSERT(pItem != nullptr);
        pItemLabel->SetEnableEdit(true);
        pItemLabel->DetachEventByID(kEventEnterEdit, callbackID);
        pItemLabel->AttachEvent(kEventEnterEdit, [this, nElementIndex, nColumnId, pItem, pSubItem](const EventArgs& /*args*/) {
            if (m_pListCtrl != nullptr) {
                m_pListCtrl->OnItemEnterEditMode(nElementIndex, nColumnId, pItem, pSubItem);
            }
            return true;
            }, callbackID);
    }
    else {
        pItemLabel->SetEnableEdit(false);
        pItemLabel->DetachEventByID(kEventEnterEdit, callbackID);
    }
    SendEvent(kEventIconViewItemFilled, (WPARAM)pViewItem->GetListBoxIndex(), (LPARAM)pViewItem->GetDataItemIndex(), pViewItem);
    return true;
}

int32_t ListCtrlIconView::GetMaxDataItemWidth(const std::vector<ListCtrlSubItemData2Ptr>& /*subItemList*/)
{
    //No implementation needed
    return -1;
}

}//namespace ui

