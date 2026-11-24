#include "dui/Control/ListCtrlSubItem.h"
#include "dui/Control/ListCtrl.h"

namespace ui
{
ListCtrlSubItem::ListCtrlSubItem(Window* pWindow):
    ListCtrlLabel(pWindow),
    m_pItem(nullptr),
    m_imageId(-1),
    m_nIconSpacing(0),
    m_nColumnId(Box::InvalidIndex),
    m_nProgress(-1),
    m_sProgressColor("bg_progress"),
    m_sProgressBkColor("bg_progress_track"),
    m_sProgressTextPos("center")
{
    SetIconSpacing(2, true);
}

std::string ListCtrlSubItem::GetType() const { return "ListCtrlSubItem"; }

void ListCtrlSubItem::SetAttribute(const std::string& strName, const std::string& strValue)
{
    if (strName == "icon_spacing") {
        SetIconSpacing(StringUtil::StringToInt32(strValue), true);
    }
    else if ((strName == "progress_color") || (strName == "progresscolor")) {
        if (m_sProgressColor != strValue) {
            m_sProgressColor = strValue;
            Invalidate();
        }
    }
    else if ((strName == "progress_bkcolor") || (strName == "progress_bk_color") || (strName == "progressbkcolor")) {
        if (m_sProgressBkColor != strValue) {
            m_sProgressBkColor = strValue;
            Invalidate();
        }
    }
    else if ((strName == "progress_text_pos") || (strName == "progresstextpos") || (strName == "progress_text_position")) {
        SetProgressTextPos(strValue);
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
            std::string checkBoxClass = pListCtrl->GetCheckBoxClass();
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

void ListCtrlSubItem::SetProgress(int32_t nProgress)
{
    if (nProgress > 100) {
        nProgress = 100;
    }
    if (nProgress < -1) {
        nProgress = -1;
    }
    if (m_nProgress != nProgress) {
        m_nProgress = nProgress;
        //The progress bar and the text share the painting area; when the progress value changes, the text area is updated
        SetTextRect(UiRect());
        Invalidate();
    }
}

int32_t ListCtrlSubItem::GetProgress() const
{
    return m_nProgress;
}

const std::string& ListCtrlSubItem::GetProgressColor() const
{
    return m_sProgressColor;
}

const std::string& ListCtrlSubItem::GetProgressBkColor() const
{
    return m_sProgressBkColor;
}

void ListCtrlSubItem::SetProgressTextPos(const std::string& textPos)
{
    if (m_sProgressTextPos != textPos) {
        m_sProgressTextPos = textPos;
        Invalidate();
    }
}

const std::string& ListCtrlSubItem::GetProgressTextPos() const
{
    return m_sProgressTextPos;
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
    if (m_nProgress >= 0) {
        //Display as a progress bar (the icon/normal text is not painted)
        PaintProgress(pRender);
        return;
    }
    //The icon before the text
    ImagePtr pItemImage = LoadItemImage();
    if (pItemImage == nullptr) {
        if (IsShowCheckBox()) {
            //When the CheckBox is shown, the text must be shifted to the right by the CheckBox width, so that the text does not overlap the CheckBox
            UiRect textRect = GetRect();
            textRect.Deflate(GetControlPadding());
            textRect.Deflate(GetTextPadding());
            textRect.left += GetCheckBoxImageWidth();
            textRect.left += GetIconSpacing();
            textRect.Validate();
            DoPaintText(textRect, pRender);
            SetTextRect(textRect);
        }
        else {
            BaseClass::PaintText(pRender);
        }
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
    PaintImage(pRender, pItemImage.get(), "", -1, nullptr, &imageRect, nullptr);
    rc.left += imageSize.cx;
    rc.left += nIconTextSpacing;

    UiRect textRect = GetRect();
    textRect.Deflate(GetControlPadding());
    textRect.Deflate(GetTextPadding());
    textRect.left = std::max(textRect.left, rc.left);

    DoPaintText(textRect, pRender);
    SetTextRect(textRect);
}

void ListCtrlSubItem::PaintProgress(IRender* pRender)
{
    if (pRender == nullptr) {
        return;
    }
    //The width of the CheckBox needs to be reserved
    int32_t nCheckBoxWidth = 0;
    if (IsShowCheckBox()) {
        nCheckBoxWidth += GetCheckBoxImageWidth();
        nCheckBoxWidth += GetIconSpacing();
    }

    //Icon: painted to the left of the progress bar, with its width reserved so that it is not covered by the progress bar (following the same approach as the icon branch)
    int32_t nIconWidth = 0;
    ImagePtr pItemImage = LoadItemImage();
    if (pItemImage != nullptr) {
        const int32_t nIconTextSpacing = GetIconSpacing();
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

        UiRect rcImage = GetRect();
        rcImage.Deflate(GetControlPadding());
        rcImage.left += nCheckBoxWidth;
        rcImage.Validate();
        rcImage.right = rcImage.left + imageSize.cx;
        VAlignRect(rcImage, GetTextStyle(), imageSize.cy);
        PaintImage(pRender, pItemImage.get(), "", -1, nullptr, &rcImage, nullptr);

        nIconWidth = imageSize.cx + nIconTextSpacing;
    }

    UiRect rc = GetRect();
    rc.Deflate(GetControlPadding());
    rc.Deflate(GetTextPadding());
    rc.left += nCheckBoxWidth;
    rc.left += nIconWidth;
    rc.Validate();
    if (rc.IsEmpty()) {
        return;
    }

    //Text position mode: "center" (default, centered in the cell) / "right" (right aligned in the cell) / "outside" (displayed outside, to the right of the progress bar)
    const bool bTextOutside = (m_sProgressTextPos == "outside") && !GetText().empty();
    const int32_t nTextGap = Dpi().GetScaleInt(4);

    //Text area: by default the same as the cell content area; in "outside" mode the text occupies the space on the right and the progress bar width is reduced accordingly
    UiRect rcText = rc;
    if (bTextOutside) {
        const UiSize szText = EstimateText(UiSize(rc.Width(), rc.Height()));
        if ((szText.cx > 0) && (szText.cx + nTextGap < rc.Width())) {
            rcText.left = rc.right - szText.cx;
        }
        else {
            rcText.left = rc.right;
        }
    }

    //Progress bar height: tall enough to hold the text (the line height of the default font plus a little padding), vertically centered, with space above and below,
    //so that the progress bars of adjacent cells do not touch each other and form one solid color band
    int32_t nBarHeight = Dpi().GetScaleInt(22);
    if (nBarHeight > rc.Height()) {
        nBarHeight = rc.Height();
    }
    if (nBarHeight < Dpi().GetScaleInt(6)) {
        nBarHeight = Dpi().GetScaleInt(6);
    }

    UiRect rcBar = rc;
    if (bTextOutside) {
        rcBar.right = rcText.left - nTextGap;
        if (rcBar.right < rcBar.left) {
            rcBar.right = rcBar.left;
        }
    }
    rcBar.top = rc.CenterY() - nBarHeight / 2;
    rcBar.bottom = rcBar.top + nBarHeight;

    //The corner radius is half of the progress bar height, forming a capsule shape
    const float fRoundRadius = static_cast<float>(nBarHeight) / 2.0f;

    //Background track
    UiColor trackColor = GetUiColor(m_sProgressBkColor.c_str());
    if ((trackColor.GetARGB() != 0) && (rcBar.Width() > 0)) {
        pRender->FillRoundRect(UiRectF::MakeFromRect(rcBar), fRoundRadius, fRoundRadius, trackColor, GetAlpha());
    }

    //Foreground progress (filled according to the percentage)
    UiColor foreColor = GetUiColor(m_sProgressColor.c_str());
    if ((foreColor.GetARGB() != 0) && (rcBar.Width() > 0)) {
        int32_t nProgress = m_nProgress;
        if (nProgress > 100) {
            nProgress = 100;
        }
        else if (nProgress < 0) {
            nProgress = 0;
        }
        int32_t nForeWidth = rcBar.Width() * nProgress / 100;
        if (nForeWidth > 0) {
            float fForeRadius = fRoundRadius;
            if (nForeWidth < nBarHeight) {
                //When the progress is very small, display at least a dot so that a thin sliver does not appear
                fForeRadius = static_cast<float>(nForeWidth) / 2.0f;
            }
            UiRect rcFore = rcBar;
            rcFore.right = rcFore.left + nForeWidth;
            pRender->FillRoundRect(UiRectF::MakeFromRect(rcFore), fForeRadius, fForeRadius, foreColor, GetAlpha());
        }
    }

    //If there is text, overlay it (for example to show the percentage text); the text is always vertically centered and horizontally aligned according to the position mode
    if (!GetText().empty()) {
        const uint32_t nOldTextStyle = GetTextStyle();
        uint32_t nNewTextStyle = nOldTextStyle;
        nNewTextStyle &= ~(TEXT_TOP | TEXT_BOTTOM | TEXT_VJUSTIFY | TEXT_LEFT | TEXT_RIGHT | TEXT_HJUSTIFY | TEXT_HCENTER);
        nNewTextStyle |= TEXT_VCENTER;
        if (bTextOutside || (m_sProgressTextPos == "right")) {
            nNewTextStyle |= TEXT_RIGHT;
        }
        else if (m_sProgressTextPos == "left") {
            nNewTextStyle |= TEXT_LEFT;
        }
        else {
            nNewTextStyle |= TEXT_HCENTER;
        }
        SetTextStyle(nNewTextStyle, false);
        DoPaintText(rcText, pRender);
        SetTextStyle(nOldTextStyle, false);
        SetTextRect(rcText);
    }
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
    if (sz.cx <= 0) {
        //The text is empty, or it cannot be estimated, so no adjustment is needed
        return sz;
    }

    //Extra width to reserve on the left of the text: CheckBox + icon + spacing (consistent with the painting logic in PaintText)
    const int32_t nIconTextSpacing = GetIconSpacing();
    int32_t nExtraLeftWidth = 0;
    if (IsShowCheckBox()) {
        nExtraLeftWidth += GetCheckBoxImageWidth();
        nExtraLeftWidth += nIconTextSpacing;
    }
    ImagePtr pItemImage = LoadItemImage();
    if (pItemImage != nullptr) {
        nExtraLeftWidth += pItemImage->GetImageInfo()->GetWidth();
        nExtraLeftWidth += nIconTextSpacing;
    }

    //Estimated width = text width + padding + extra width occupied on the left
    sz.cx += nExtraLeftWidth;
    return sz;
}

}//namespace ui

