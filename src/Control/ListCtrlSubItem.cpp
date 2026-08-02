#include "duilib/Control/ListCtrlSubItem.h"
#include "duilib/Control/ListCtrl.h"

namespace ui
{
ListCtrlSubItem::ListCtrlSubItem(Window* pWindow):
    ListCtrlLabel(pWindow),
    m_pItem(nullptr),
    m_imageId(-1),
    m_nIconSpacing(0),
    m_nColumnId(Box::InvalidIndex)
{
    SetIconSpacing(2, true);
}

DString ListCtrlSubItem::GetType() const { return _T("ListCtrlSubItem"); }

void ListCtrlSubItem::SetAttribute(const DString& strName, const DString& strValue)
{
    if (strName == _T("icon_spacing")) {
        SetIconSpacing(StringUtil::StringToInt32(strValue), true);
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

void ListCtrlSubItem::ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale)
{
    if (!Dpi().CheckDisplayScaleFactor(nNewDpiScale)) {
        return;
    }
    int32_t iValue = GetIconSpacing();
    iValue = Dpi().GetScaleInt(iValue, nOldDpiScale);
    SetIconSpacing(iValue, false);
    BaseClass::ChangeDpiScale(nOldDpiScale, nNewDpiScale);
}

bool ListCtrlSubItem::SupportCheckMode() const
{
    return true;
}

void ListCtrlSubItem::SetListCtrlItem(ListCtrlItem* pItem)
{
    m_pItem = pItem;
}

ListCtrlItem* ListCtrlSubItem::GetListCtrlItem() const
{
    ASSERT(m_pItem != nullptr);
    return m_pItem;
}

size_t ListCtrlSubItem::GetDataItemIndex() const
{
    if (m_pItem != nullptr) {
        return m_pItem->GetDataItemIndex();
    }
    return Box::InvalidIndex;
}

size_t ListCtrlSubItem::GetDataColumnIndex() const
{
    if (m_pItem != nullptr) {
        ListCtrl* pListCtrl = m_pItem->GetListCtrl();
        if (pListCtrl != nullptr) {
            return pListCtrl->GetColumnIndex(m_nColumnId);
        }
    }
    return Box::InvalidIndex;
}

size_t ListCtrlSubItem::GetDataColumnId() const
{
    return m_nColumnId;
}

void ListCtrlSubItem::SetDataColumnId(size_t nColumnId)
{
    m_nColumnId = nColumnId;
}

bool ListCtrlSubItem::SetShowCheckBox(bool bShow)
{
    ListCtrlItem* pItem = GetListCtrlItem();
    if (pItem == nullptr) {
        return false;
    }
    bool bRet = false;
    if (bShow) {
        if (IsShowCheckBox()) {
            return true;
        }
        ListCtrl* pListCtrl = pItem->GetListCtrl();
        if (pListCtrl != nullptr) {
            DString checkBoxClass = pListCtrl->GetCheckBoxClass();
            if (!checkBoxClass.empty()) {
                SetClass(checkBoxClass);
                bRet = IsShowCheckBox();
            }
        }
    }
    else {
        //Clear the CheckBox image resources, so it will not be displayed
        ClearStateImages();
        ASSERT(!IsShowCheckBox());
        bRet = true;
    }
    return bRet;
}

bool ListCtrlSubItem::IsShowCheckBox() const
{
    //If there is a CheckBox image resource, the CheckBox is considered displayed
    return !GetStateImage(kControlStateNormal).empty() && !GetSelectedStateImage(kControlStateNormal).empty();
}

bool ListCtrlSubItem::IsCheckBoxChecked() const
{
    return IsShowCheckBox() && IsChecked();
}

int32_t ListCtrlSubItem::GetCheckBoxImageWidth()
{
    if (GetWindow() == nullptr) {
        return 0;
    }
    UiSize sz = GetStateImageSize(kStateImageBk, kControlStateNormal);
    return sz.cx;
}

void ListCtrlSubItem::SetImageId(int32_t imageId)
{
    if (m_imageId != imageId) {
        m_imageId = imageId;
        Invalidate();
    }
}

int32_t ListCtrlSubItem::GetImageId() const
{
    return m_imageId;
}

void ListCtrlSubItem::SetIconSpacing(int32_t nIconSpacing, bool bNeedDpiScale)
{
    if (bNeedDpiScale) {
        Dpi().ScaleInt(nIconSpacing);
    }
    if (m_nIconSpacing != nIconSpacing) {
        m_nIconSpacing = nIconSpacing;
        if (m_nIconSpacing < 0) {
            m_nIconSpacing = 0;
        }
        Invalidate();
    }
}

int32_t ListCtrlSubItem::GetIconSpacing() const
{
    return m_nIconSpacing;
}

ImagePtr ListCtrlSubItem::LoadItemImage() const
{
    ImagePtr pItemImage;
    if ((m_imageId >= 0) && (m_pItem != nullptr)) {
        ListCtrl* pListCtrl = m_pItem->GetListCtrl();
        if (pListCtrl != nullptr) {
            ImageListPtr pImageList = pListCtrl->GetImageList(ListCtrlType::Report);
            if (pImageList != nullptr) {
                pItemImage = pImageList->GetImageData(m_imageId);
                ASSERT(pItemImage != nullptr);
            }
        }
    }
    if (pItemImage != nullptr) {
        LoadImageInfo(*pItemImage);
        std::shared_ptr<ImageInfo> pItemImageCache = pItemImage->GetImageInfo();
        if (pItemImageCache == nullptr) {
            pItemImage = nullptr;
        }
        else {
            if ((pItemImageCache->GetWidth() <= 0) ||
                (pItemImageCache->GetHeight() <= 0)) {
                pItemImage = nullptr;
            }
        }
    }
    return pItemImage;
}

void ListCtrlSubItem::PaintText(IRender* pRender)
{
    //The content to be drawn includes: icon and text
    SetTextRect(UiRect());
    if (pRender == nullptr) {
        return;
    }
    //The icon before the text
    ImagePtr pItemImage = LoadItemImage();
    if (pItemImage == nullptr) {
        BaseClass::PaintText(pRender);
        return;
    }

    int32_t nIconTextSpacing = GetIconSpacing();

    //The width of the CheckBox needs to be reserved
    int32_t nCheckBoxWidth = 0;
    if (IsShowCheckBox()) {
        nCheckBoxWidth += GetCheckBoxImageWidth();
        nCheckBoxWidth += nIconTextSpacing;
    }

    UiSize imageSize;
    ListCtrl* pListCtrl = nullptr;
    if (m_pItem != nullptr) {
        pListCtrl = m_pItem->GetListCtrl();
    }
    if (pListCtrl != nullptr) {
        ImageListPtr pImageList = pListCtrl->GetImageList(ListCtrlType::Report);
        if (pImageList != nullptr) {
            imageSize = pImageList->GetImageSize();
        }
    }

    if (imageSize.cx <= 0) {
        imageSize.cx = pItemImage->GetImageInfo()->GetWidth();
    }
    if (imageSize.cy <= 0) {
        imageSize.cy = pItemImage->GetImageInfo()->GetHeight();
    }

    //The icon is on the left, and the text is drawn in the original way
    UiRect rc = GetRect();
    rc.Deflate(GetControlPadding());
    rc.left += nCheckBoxWidth;
    rc.Validate();
    rc.right = rc.left + imageSize.cx;

    UiRect imageRect = rc;
    VAlignRect(imageRect, GetTextStyle(), imageSize.cy);
    PaintImage(pRender, pItemImage.get(), _T(""), -1, nullptr, &imageRect, nullptr);
    rc.left += imageSize.cx;
    rc.left += nIconTextSpacing;

    UiRect textRect = GetRect();
    textRect.Deflate(GetControlPadding());
    textRect.Deflate(GetTextPadding());
    textRect.left = std::max(textRect.left, rc.left);

    DoPaintText(textRect, pRender);
    SetTextRect(textRect);
}

void ListCtrlSubItem::VAlignRect(UiRect& rc, uint32_t textStyle, int32_t nImageHeight)
{
    if ((nImageHeight <= 0) || (nImageHeight >= rc.Height())) {
        return;
    }
    if (textStyle & TEXT_VCENTER) {
        //Center aligned
        rc.top = rc.CenterY() - nImageHeight / 2;
        rc.bottom = rc.top + nImageHeight;
    }
    else if (textStyle & TEXT_BOTTOM) {
        //Bottom aligned
        rc.top = rc.bottom - nImageHeight;
    }
    else {
        //Top aligned
        rc.bottom = rc.top + nImageHeight;
    }
}

UiSize ListCtrlSubItem::EstimateText(UiSize szAvailable)
{
    UiSize sz = BaseClass::EstimateText(szAvailable);

    int32_t nIconTextSpacing = GetIconSpacing();

    //The width of the CheckBox needs to be reserved
    int32_t nCheckBoxWidth = 0;
    if (IsShowCheckBox()) {
        nCheckBoxWidth += GetCheckBoxImageWidth();
        nCheckBoxWidth += nIconTextSpacing;
    }

    const UiPadding rcPadding = GetControlPadding();
    const UiPadding rcTextPadding = GetTextPadding();

    //The icon is on the left, and the text is drawn in the original way
    UiRect rc = GetRect();
    rc.Deflate(rcPadding);
    rc.left += nCheckBoxWidth;
    rc.Validate();

    ImagePtr pItemImage = LoadItemImage();
    if (pItemImage != nullptr) {
        rc.left += pItemImage->GetImageInfo()->GetWidth();
        rc.left += nIconTextSpacing;
    }   

    UiRect textRect = GetRect();
    textRect.Deflate(rcPadding);
    textRect.Deflate(rcTextPadding);
    textRect.left = std::max(textRect.left, rc.left);

    int32_t nPaddingLeft = rcPadding.left + rcTextPadding.left;
    if (textRect.left > nPaddingLeft) {
        sz.cx -= nPaddingLeft;
        sz.cx += textRect.left;
    }    
    return sz;
}

}//namespace ui

