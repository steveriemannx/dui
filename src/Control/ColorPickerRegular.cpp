#include "dui/Control/ColorPickerRegular.h"

namespace ui
{
/** Color picker: regular colors
*/
class ColorPickerRegularProvider : public ui::VirtualListBoxElement
{
public:
    ColorPickerRegularProvider();

    /** Set the color data
    * @param [in] uiColors The color data provided externally; the first is the color name, the second is the color value
    */
    void SetColors(const std::vector<std::pair<DString, int32_t>>& uiColors);

    /** Create a data item
    * @param [in] pVirtualListBox The interface of the associated virtual list
    * @return Returns the pointer of the created data item
    */
    virtual ui::Control* CreateElement(ui::VirtualListBox* pVirtualListBox) override;

    /** Fill the specified data item
    * @param [in] pControl The pointer of the data item control
    * @param [in] nElementIndex The index ID of the data element, range: [0, GetElementCount())
    */
    virtual bool FillElement(ui::Control* pControl, size_t nElementIndex) override;

    /** Get the total number of data items
    * @return Returns the total number of data items
    */
    virtual size_t GetElementCount() const override;

    /** Set the selection state
    * @param [in] nElementIndex The index ID of the data element, range: [0, GetElementCount())
    * @param [in] bSelected true means selected, false means not selected
    */
    virtual void SetElementSelected(size_t nElementIndex, bool bSelected) override;

    /** Get the selection state
    * @param [in] nElementIndex The index ID of the data element, range: [0, GetElementCount())
    * @return true means selected, false means not selected
    */
    virtual bool IsElementSelected(size_t nElementIndex) const override;

    /** Get the list of selected elements
    * @param [in] selectedIndexs Returns the list of currently selected elements, valid range: [0, GetElementCount())
    */
    virtual void GetSelectedElements(std::vector<size_t>& selectedIndexs) const override;

    /** Whether multiple selection is supported
    */
    virtual bool IsMultiSelect() const override;

    /** Set whether multiple selection is supported; called by the UI layer to keep consistent with the UI control
    * @return bMultiSelect true means multiple selection is supported, false means it is not supported
    */
    virtual void SetMultiSelect(bool bMultiSelect) override;

    /** Select a color
    */
    size_t SelectColor(const UiColor& color);

    /** Get the currently selected color
    */
    UiColor GetSelectedColor() const;

public:
    /** Common color values and their corresponding string constants
    */
    void GetDefaultColors(std::vector<std::pair<DString, int32_t>>& uiColors);

    /** Basic color values and their corresponding string constants
    */
    void GetBasicColors(std::vector<std::pair<DString, int32_t>>& uiColors);

private:
    /** Color structure
    */
    struct RegularColor
    {
        //The display name of the color
        UiString colorName;
        //The color value
        UiColor colorValue;
        //Whether selected
        bool m_bSelected = false;

        //Comparison function
        bool operator < (const RegularColor& r) const
        {
            //Sort by the gray value of the colors
            double v1 = colorValue.GetR() * 0.299 + colorValue.GetG() * 0.587 + colorValue.GetB() * 0.114;
            double v2 = r.colorValue.GetR() * 0.299 + r.colorValue.GetG() * 0.587 + r.colorValue.GetB() * 0.114;
            return v1 < v2;
        }
    };

    /** The color list
    */
    std::vector<RegularColor> m_colors;
};

ColorPickerRegular::ColorPickerRegular(Window* pWindow):
    VirtualVTileListBox(pWindow)
{
    m_regularColors = std::make_unique<ColorPickerRegularProvider>();
    SetDataProvider(m_regularColors.get());
    AttachSelect([this](const ui::EventArgs& args) {
        Control* pControl = GetItemAt(args.wParam);
        if (pControl != nullptr) {
            DString colorString = pControl->GetBkColor();
            if (!colorString.empty()) {
                UiColor newColor = pControl->GetUiColor(colorString);
                SendEvent(kEventSelectColor, newColor.GetARGB());
            }
        }
        return true;
        });
}

ColorPickerRegular::~ColorPickerRegular()
{
}

DString ColorPickerRegular::GetType() const { return DUI_CTR_COLOR_PICKER_REGULAR; }

void ColorPickerRegular::SelectColor(const UiColor& color)
{
    size_t nCurSel = m_regularColors->SelectColor(color);
    size_t nOldSel = GetCurSel();
    if (nCurSel != nOldSel) {
        Control* pControl = GetItemAt(nOldSel);
        if (pControl != nullptr) {
            ListBoxItem* pListItem = dynamic_cast<ListBoxItem*>(pControl);
            if ((pListItem != nullptr) && (pListItem->IsSelected())) {
                pListItem->SetSelected(false);
            }
        }
        if (nCurSel < GetItemCount()) {
            SetCurSel(nCurSel);
        }        
    }
    Refresh();
    Invalidate();
}

UiColor ColorPickerRegular::GetSelectedColor() const
{
    return m_regularColors->GetSelectedColor();
}

void ColorPickerRegular::SetPos(UiRect rc)
{
    UiRect rect = rc;
    rect.Deflate(GetPadding());
    //Automatically calculate the size of each color block to adapt to stretching
    int32_t elementCount = 0;
    VirtualListBoxElement* pListBoxElement = GetDataProvider();
    if (pListBoxElement != nullptr) {
        elementCount = (int32_t)pListBoxElement->GetElementCount();
    }
    VirtualVTileLayout* pVirtualLayout = dynamic_cast<VirtualVTileLayout*>(GetLayout());
    if ((pVirtualLayout != nullptr) && (elementCount > 0)) {
        if ((m_szItem.cx == 0) && (m_szItem.cy == 0)) {
            m_szItem = pVirtualLayout->GetItemSize();
        }
        int32_t columns = pVirtualLayout->GetColumns();
        if (columns > 0) {
            int32_t rows = elementCount / columns;
            if ((elementCount % columns) != 0) {
                rows += 1;
            }
            if (rows <= 0) {
                rows += 1;
            }
            //Automatically calculate the Item size
            int32_t childMarginX = pVirtualLayout->GetChildMarginX();
            int32_t childMarginY = pVirtualLayout->GetChildMarginY();
            UiSize szItem;
            szItem.cx = (rect.Width() - childMarginX * (columns - 1)) / columns;
            szItem.cy = (rect.Height() - childMarginY * (rows - 1)) / rows;
            if ((szItem.cx > 0) && (szItem.cy > 0)) {
                pVirtualLayout->SetItemSize(szItem);
            }
            else if ((m_szItem.cx > 0) && (m_szItem.cy > 0)) {
                pVirtualLayout->SetItemSize(m_szItem);
            }
        }
    }
    BaseClass::SetPos(rc);
}

void ColorPickerRegular::SetColumns(int32_t nColumns)
{
    ASSERT(nColumns > 0);
    if (nColumns <= 0) {
        return;
    }
    VirtualVTileLayout* pVirtualLayout = dynamic_cast<VirtualVTileLayout*>(GetLayout());
    if (pVirtualLayout != nullptr) {
        pVirtualLayout->SetColumns(nColumns);
    }
}

void ColorPickerRegular::SetAttribute(const DString& strName, const DString& strValue)
{
    if (strName == _T("color_type")) {
        if (strValue == _T("basic")) {
            //Use the basic colors
            std::vector<std::pair<DString, int32_t>> uiColors;
            m_regularColors->GetBasicColors(uiColors);
            m_regularColors->SetColors(uiColors);
        }
        else {
            //Use the default colors
            std::vector<std::pair<DString, int32_t>> uiColors;
            m_regularColors->GetDefaultColors(uiColors);
            m_regularColors->SetColors(uiColors);
        }
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

////////////////////////////////////////////////////////////
//
ColorPickerRegularProvider::ColorPickerRegularProvider()
{
    std::vector<std::pair<DString, int32_t>> uiColors;
    GetDefaultColors(uiColors);
    SetColors(uiColors);
}

void ColorPickerRegularProvider::SetColors(const std::vector<std::pair<DString, int32_t>>& uiColors)
{
    m_colors.clear();
    RegularColor regularColor;
    regularColor.m_bSelected = false;
    for (const auto& color : uiColors) {
        regularColor.colorValue = UiColor(color.second);
        DString colorString = ui::StringUtil::Printf(_T("#%02X%02X%02X%02X"),
            regularColor.colorValue.GetA(),
            regularColor.colorValue.GetR(),
            regularColor.colorValue.GetG(),
            regularColor.colorValue.GetB());
        DString colorName = color.first;
        StringUtil::ReplaceAll(_T(","), _T(", "), colorName);
        colorName = colorString + _T(", ") + colorName;
        regularColor.colorName = colorName;
        if (regularColor.colorValue.GetARGB() != UiColors::Transparent) {
            m_colors.push_back(regularColor);
        }
    }
    std::sort(m_colors.begin(), m_colors.end());
}

Control* ColorPickerRegularProvider::CreateElement(VirtualListBox* pVirtualListBox)
{
    ASSERT(pVirtualListBox != nullptr);
    if (pVirtualListBox == nullptr) {
        return nullptr;
    }
    ASSERT(pVirtualListBox->GetWindow() != nullptr);
    if (pVirtualListBox->GetWindow() == nullptr) {
        return nullptr;
    }
    ListBoxItem* pControl = new ListBoxItem(pVirtualListBox->GetWindow());
    float fBorderSize = 2.0f;
    UiRectF borderSize(fBorderSize, fBorderSize, fBorderSize, fBorderSize);
    pControl->SetBorderSize(borderSize, true);
    pControl->SetBorderColor(kControlStatePushed, _T("blue"));
    return pControl;
}

bool ColorPickerRegularProvider::FillElement(ui::Control* pControl, size_t nElementIndex)
{
    if ((pControl != nullptr) && (nElementIndex < m_colors.size())) {
        const RegularColor& regularColor = m_colors[nElementIndex];
        pControl->SetBkColor(regularColor.colorValue);
        pControl->SetToolTipText(regularColor.colorName.c_str());
        pControl->SetUserDataID(nElementIndex);
        return true;
    }
    return false;
}

size_t ColorPickerRegularProvider::GetElementCount() const
{
    return m_colors.size();
}

void ColorPickerRegularProvider::SetElementSelected(size_t nElementIndex, bool bSelected)
{
    if (nElementIndex < m_colors.size()) {
        RegularColor& regularColor = m_colors[nElementIndex];
        regularColor.m_bSelected = bSelected;
    }
}

bool ColorPickerRegularProvider::IsElementSelected(size_t nElementIndex) const
{
    if (nElementIndex < m_colors.size()) {
        const RegularColor& regularColor = m_colors[nElementIndex];
        return regularColor.m_bSelected;
    }
    return false;
}

void ColorPickerRegularProvider::GetSelectedElements(std::vector<size_t>& selectedIndexs) const
{
    selectedIndexs.clear();
    size_t nCount = m_colors.size();
    for (size_t nElementIndex = 0; nElementIndex < nCount; ++nElementIndex) {
        const RegularColor& regularColor = m_colors[nElementIndex];
        if (regularColor.m_bSelected) {
            selectedIndexs.push_back(nElementIndex);
        }
    }
}

bool ColorPickerRegularProvider::IsMultiSelect() const
{
    return false;
}

void ColorPickerRegularProvider::SetMultiSelect(bool /*bMultiSelect*/)
{
}

size_t ColorPickerRegularProvider::SelectColor(const UiColor& color)
{
    size_t selectIndex = Box::InvalidIndex;
    for (size_t index = 0; index < m_colors.size(); ++index) {
        RegularColor& regularColor = m_colors[index];
        if (regularColor.colorValue == color) {
            regularColor.m_bSelected = true;
            selectIndex = index;
        }
        else {
            regularColor.m_bSelected = false;
        }
    }
    return selectIndex;
}

UiColor ColorPickerRegularProvider::GetSelectedColor() const
{
    for (const RegularColor& regularColor : m_colors) {
        if (regularColor.m_bSelected) {
            return regularColor.colorValue;
        }
    }
    return UiColor();
}

void ColorPickerRegularProvider::GetDefaultColors(std::vector<std::pair<DString, int32_t>>& uiColors)
{
    uiColors = {
                {_T("AliceBlue"),UiColors::AliceBlue},
                {_T("AntiqueWhite"),UiColors::AntiqueWhite},
                {_T("Aqua"),UiColors::Aqua},
                {_T("Aquamarine"),UiColors::Aquamarine},
                {_T("Azure"),UiColors::Azure},
                {_T("Beige"),UiColors::Beige},
                {_T("Bisque"),UiColors::Bisque},
                {_T("Black"),UiColors::Black},
                {_T("BlanchedAlmond"),UiColors::BlanchedAlmond},
                {_T("Blue"),UiColors::Blue},
                {_T("BlueViolet"),UiColors::BlueViolet},
                {_T("Brown"),UiColors::Brown},
                {_T("BurlyWood"),UiColors::BurlyWood},
                {_T("CadetBlue"),UiColors::CadetBlue},
                {_T("Chartreuse"),UiColors::Chartreuse},
                {_T("Chocolate"),UiColors::Chocolate},
                {_T("Coral"),UiColors::Coral},
                {_T("CornflowerBlue"),UiColors::CornflowerBlue},
                {_T("Cornsilk"),UiColors::Cornsilk},
                {_T("Crimson"),UiColors::Crimson},
                {_T("Cyan"),UiColors::Cyan},
                {_T("DarkBlue"),UiColors::DarkBlue},
                {_T("DarkCyan"),UiColors::DarkCyan},
                {_T("DarkGoldenrod"),UiColors::DarkGoldenrod},
                {_T("DarkGray"),UiColors::DarkGray},
                {_T("DarkGreen"),UiColors::DarkGreen},
                {_T("DarkKhaki"),UiColors::DarkKhaki},
                {_T("DarkMagenta"),UiColors::DarkMagenta},
                {_T("DarkOliveGreen"),UiColors::DarkOliveGreen},
                {_T("DarkOrange"),UiColors::DarkOrange},
                {_T("DarkOrchid"),UiColors::DarkOrchid},
                {_T("DarkRed"),UiColors::DarkRed},
                {_T("DarkSalmon"),UiColors::DarkSalmon},
                {_T("DarkSeaGreen"),UiColors::DarkSeaGreen},
                {_T("DarkSlateBlue"),UiColors::DarkSlateBlue},
                {_T("DarkSlateGray"),UiColors::DarkSlateGray},
                {_T("DarkTurquoise"),UiColors::DarkTurquoise},
                {_T("DarkViolet"),UiColors::DarkViolet},
                {_T("DeepPink"),UiColors::DeepPink},
                {_T("DeepSkyBlue"),UiColors::DeepSkyBlue},
                {_T("DimGray"),UiColors::DimGray},
                {_T("DodgerBlue"),UiColors::DodgerBlue},
                {_T("Firebrick"),UiColors::Firebrick},
                {_T("FloralWhite"),UiColors::FloralWhite},
                {_T("ForestGreen"),UiColors::ForestGreen},
                {_T("Fuchsia"),UiColors::Fuchsia},
                {_T("Gainsboro"),UiColors::Gainsboro},
                {_T("GhostWhite"),UiColors::GhostWhite},
                {_T("Gold"),UiColors::Gold},
                {_T("Goldenrod"),UiColors::Goldenrod},
                {_T("Gray"),UiColors::Gray},
                {_T("Green"),UiColors::Green},
                {_T("GreenYellow"),UiColors::GreenYellow},
                {_T("Honeydew"),UiColors::Honeydew},
                {_T("HotPink"),UiColors::HotPink},
                {_T("IndianRed"),UiColors::IndianRed},
                {_T("Indigo"),UiColors::Indigo},
                {_T("Ivory"),UiColors::Ivory},
                {_T("Khaki"),UiColors::Khaki},
                {_T("Lavender"),UiColors::Lavender},
                {_T("LavenderBlush"),UiColors::LavenderBlush},
                {_T("LawnGreen"),UiColors::LawnGreen},
                {_T("LemonChiffon"),UiColors::LemonChiffon},
                {_T("LightBlue"),UiColors::LightBlue},
                {_T("LightCoral"),UiColors::LightCoral},
                {_T("LightCyan"),UiColors::LightCyan},
                {_T("LightGoldenrodYellow"),UiColors::LightGoldenrodYellow},
                {_T("LightGray"),UiColors::LightGray},
                {_T("LightGreen"),UiColors::LightGreen},
                {_T("LightPink"),UiColors::LightPink},
                {_T("LightSalmon"),UiColors::LightSalmon},
                {_T("LightSeaGreen"),UiColors::LightSeaGreen},
                {_T("LightSkyBlue"),UiColors::LightSkyBlue},
                {_T("LightSlateGray"),UiColors::LightSlateGray},
                {_T("LightSteelBlue"),UiColors::LightSteelBlue},
                {_T("LightYellow"),UiColors::LightYellow},
                {_T("Lime"),UiColors::Lime},
                {_T("LimeGreen"),UiColors::LimeGreen},
                {_T("Linen"),UiColors::Linen},
                {_T("Magenta"),UiColors::Magenta},
                {_T("Maroon"),UiColors::Maroon},
                {_T("MediumAquamarine"),UiColors::MediumAquamarine},
                {_T("MediumBlue"),UiColors::MediumBlue},
                {_T("MediumOrchid"),UiColors::MediumOrchid},
                {_T("MediumPurple"),UiColors::MediumPurple},
                {_T("MediumSeaGreen"),UiColors::MediumSeaGreen},
                {_T("MediumSlateBlue"),UiColors::MediumSlateBlue},
                {_T("MediumSpringGreen"),UiColors::MediumSpringGreen},
                {_T("MediumTurquoise"),UiColors::MediumTurquoise},
                {_T("MediumVioletRed"),UiColors::MediumVioletRed},
                {_T("MidnightBlue"),UiColors::MidnightBlue},
                {_T("MintCream"),UiColors::MintCream},
                {_T("MistyRose"),UiColors::MistyRose},
                {_T("Moccasin"),UiColors::Moccasin},
                {_T("NavajoWhite"),UiColors::NavajoWhite},
                {_T("Navy"),UiColors::Navy},
                {_T("OldLace"),UiColors::OldLace},
                {_T("Olive"),UiColors::Olive},
                {_T("OliveDrab"),UiColors::OliveDrab},
                {_T("Orange"),UiColors::Orange},
                {_T("OrangeRed"),UiColors::OrangeRed},
                {_T("Orchid"),UiColors::Orchid},
                {_T("PaleGoldenrod"),UiColors::PaleGoldenrod},
                {_T("PaleGreen"),UiColors::PaleGreen},
                {_T("PaleTurquoise"),UiColors::PaleTurquoise},
                {_T("PaleVioletRed"),UiColors::PaleVioletRed},
                {_T("PapayaWhip"),UiColors::PapayaWhip},
                {_T("PeachPuff"),UiColors::PeachPuff},
                {_T("Peru"),UiColors::Peru},
                {_T("Pink"),UiColors::Pink},
                {_T("Plum"),UiColors::Plum},
                {_T("PowderBlue"),UiColors::PowderBlue},
                {_T("Purple"),UiColors::Purple},
                {_T("Red"),UiColors::Red},
                {_T("RosyBrown"),UiColors::RosyBrown},
                {_T("RoyalBlue"),UiColors::RoyalBlue},
                {_T("SaddleBrown"),UiColors::SaddleBrown},
                {_T("Salmon"),UiColors::Salmon},
                {_T("SandyBrown"),UiColors::SandyBrown},
                {_T("SeaGreen"),UiColors::SeaGreen},
                {_T("SeaShell"),UiColors::SeaShell},
                {_T("Sienna"),UiColors::Sienna},
                {_T("Silver"),UiColors::Silver},
                {_T("SkyBlue"),UiColors::SkyBlue},
                {_T("SlateBlue"),UiColors::SlateBlue},
                {_T("SlateGray"),UiColors::SlateGray},
                {_T("Snow"),UiColors::Snow},
                {_T("SpringGreen"),UiColors::SpringGreen},
                {_T("SteelBlue"),UiColors::SteelBlue},
                {_T("Tan"),UiColors::Tan},
                {_T("Teal"),UiColors::Teal},
                {_T("Thistle"),UiColors::Thistle},
                {_T("Tomato"),UiColors::Tomato},
                {_T("Transparent"),UiColors::Transparent},
                {_T("Turquoise"),UiColors::Turquoise},
                {_T("Violet"),UiColors::Violet},
                {_T("Wheat"),UiColors::Wheat},
                {_T("White"),UiColors::White},
                {_T("WhiteSmoke"),UiColors::WhiteSmoke},
                {_T("Yellow"),UiColors::Yellow},
                {_T("YellowGreen"),UiColors::YellowGreen}
    };
}

void ColorPickerRegularProvider::GetBasicColors(std::vector<std::pair<DString, int32_t>>& uiColors)
{
    uiColors = {
        {_T("Rose"),0xFFF08784},
        {_T("Rose"),0xFFEB3324},
        {_T("Brown"),0xFF774342},
        {_T("Red"),0xFF8E403A},
        {_T("Dark Red"),0xFF3A0603},
        {_T("Sky Blue"),0xFF9FFCFD},
        {_T("Sky Blue"),0xFF73FBFD},
        {_T("Blue"),0xFF3282F6},
        {_T("Blue"),0xFF0023F5},
        {_T("Dark Blue"),0xFF00129A},
        {_T("Dark Blue"),0xFF16417C},
        {_T("Dark Blue"),0xFF000C7B},

        {_T("Light Yellow"),0xFFFFFE91},
        {_T("Yellow"),0xFFFFFD55},
        {_T("Orange"),0xFFF09B59},
        {_T("Orange"),0xFFF08650},
        {_T("Brown"),0xFF784315},
        {_T("Dark Yellow"),0xFF817F26},
        {_T("Light Blue"),0xFF7E84F7},
        {_T("Purple"),0xFF732BF5},
        {_T("Blue"),0xFF3580BB},
        {_T("Dark Blue"),0xFF00023D},
        {_T("Dark Purple"),0xFF58135E},
        {_T("Dark Purple"),0xFF3A083E},

        {_T("Light Green"),0xFFA1FB8E},
        {_T("Green"),0xFFA1FA4F},
        {_T("Green"),0xFF75F94D},
        {_T("Light Green"),0xFF75FA61},
        {_T("Light Green"),0xFF75FA8D},
        {_T("Brown"),0xFF818049},
        {_T("Pink"),0xFFEF88BE},
        {_T("Light Purple"),0xFFEE8AF8},
        {_T("Pale Purple"),0xFFEA3FF7},
        {_T("Pink"),0xFFEA3680},
        {_T("Pale Purple"),0xFF7F82BB},
        {_T("Magenta"),0xFF75163F},

        {_T("Light Green"),0xFF377D22},
        {_T("Dark Green"),0xFF377E47},
        {_T("Dark Cyan"),0xFF367E7F},
        {_T("Cyan"),0xFF507F80},
        {_T("Dark Green"),0xFF183E0C},
        {_T("Dark Cyan"),0xFF173F3F},
        {_T("Dark Purple"),0xFF741B7C},
        {_T("Dark Purple"),0xFF39107B},
        {_T("Black"),0xFF000000},
        {_T("Gray"),0xFF808080},
        {_T("Light Gray"),0xFFC0C0C0},
        {_T("White"),0xFFFFFFFF}
    };
}

}//namespace ui
