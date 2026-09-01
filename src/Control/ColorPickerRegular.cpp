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
    if (strName == DUI_T("color_type")) {
        if (strValue == DUI_T("basic")) {
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
        DString colorString = ui::StringUtil::Printf(DUI_T("#%02X%02X%02X%02X"),
            regularColor.colorValue.GetA(),
            regularColor.colorValue.GetR(),
            regularColor.colorValue.GetG(),
            regularColor.colorValue.GetB());
        DString colorName = color.first;
        StringUtil::ReplaceAll(DUI_T(","), DUI_T(", "), colorName);
        colorName = colorString + DUI_T(", ") + colorName;
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
    pControl->SetBorderColor(kControlStatePushed, DUI_T("blue"));
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
                {DUI_T("AliceBlue"),UiColors::AliceBlue},
                {DUI_T("AntiqueWhite"),UiColors::AntiqueWhite},
                {DUI_T("Aqua"),UiColors::Aqua},
                {DUI_T("Aquamarine"),UiColors::Aquamarine},
                {DUI_T("Azure"),UiColors::Azure},
                {DUI_T("Beige"),UiColors::Beige},
                {DUI_T("Bisque"),UiColors::Bisque},
                {DUI_T("Black"),UiColors::Black},
                {DUI_T("BlanchedAlmond"),UiColors::BlanchedAlmond},
                {DUI_T("Blue"),UiColors::Blue},
                {DUI_T("BlueViolet"),UiColors::BlueViolet},
                {DUI_T("Brown"),UiColors::Brown},
                {DUI_T("BurlyWood"),UiColors::BurlyWood},
                {DUI_T("CadetBlue"),UiColors::CadetBlue},
                {DUI_T("Chartreuse"),UiColors::Chartreuse},
                {DUI_T("Chocolate"),UiColors::Chocolate},
                {DUI_T("Coral"),UiColors::Coral},
                {DUI_T("CornflowerBlue"),UiColors::CornflowerBlue},
                {DUI_T("Cornsilk"),UiColors::Cornsilk},
                {DUI_T("Crimson"),UiColors::Crimson},
                {DUI_T("Cyan"),UiColors::Cyan},
                {DUI_T("DarkBlue"),UiColors::DarkBlue},
                {DUI_T("DarkCyan"),UiColors::DarkCyan},
                {DUI_T("DarkGoldenrod"),UiColors::DarkGoldenrod},
                {DUI_T("DarkGray"),UiColors::DarkGray},
                {DUI_T("DarkGreen"),UiColors::DarkGreen},
                {DUI_T("DarkKhaki"),UiColors::DarkKhaki},
                {DUI_T("DarkMagenta"),UiColors::DarkMagenta},
                {DUI_T("DarkOliveGreen"),UiColors::DarkOliveGreen},
                {DUI_T("DarkOrange"),UiColors::DarkOrange},
                {DUI_T("DarkOrchid"),UiColors::DarkOrchid},
                {DUI_T("DarkRed"),UiColors::DarkRed},
                {DUI_T("DarkSalmon"),UiColors::DarkSalmon},
                {DUI_T("DarkSeaGreen"),UiColors::DarkSeaGreen},
                {DUI_T("DarkSlateBlue"),UiColors::DarkSlateBlue},
                {DUI_T("DarkSlateGray"),UiColors::DarkSlateGray},
                {DUI_T("DarkTurquoise"),UiColors::DarkTurquoise},
                {DUI_T("DarkViolet"),UiColors::DarkViolet},
                {DUI_T("DeepPink"),UiColors::DeepPink},
                {DUI_T("DeepSkyBlue"),UiColors::DeepSkyBlue},
                {DUI_T("DimGray"),UiColors::DimGray},
                {DUI_T("DodgerBlue"),UiColors::DodgerBlue},
                {DUI_T("Firebrick"),UiColors::Firebrick},
                {DUI_T("FloralWhite"),UiColors::FloralWhite},
                {DUI_T("ForestGreen"),UiColors::ForestGreen},
                {DUI_T("Fuchsia"),UiColors::Fuchsia},
                {DUI_T("Gainsboro"),UiColors::Gainsboro},
                {DUI_T("GhostWhite"),UiColors::GhostWhite},
                {DUI_T("Gold"),UiColors::Gold},
                {DUI_T("Goldenrod"),UiColors::Goldenrod},
                {DUI_T("Gray"),UiColors::Gray},
                {DUI_T("Green"),UiColors::Green},
                {DUI_T("GreenYellow"),UiColors::GreenYellow},
                {DUI_T("Honeydew"),UiColors::Honeydew},
                {DUI_T("HotPink"),UiColors::HotPink},
                {DUI_T("IndianRed"),UiColors::IndianRed},
                {DUI_T("Indigo"),UiColors::Indigo},
                {DUI_T("Ivory"),UiColors::Ivory},
                {DUI_T("Khaki"),UiColors::Khaki},
                {DUI_T("Lavender"),UiColors::Lavender},
                {DUI_T("LavenderBlush"),UiColors::LavenderBlush},
                {DUI_T("LawnGreen"),UiColors::LawnGreen},
                {DUI_T("LemonChiffon"),UiColors::LemonChiffon},
                {DUI_T("LightBlue"),UiColors::LightBlue},
                {DUI_T("LightCoral"),UiColors::LightCoral},
                {DUI_T("LightCyan"),UiColors::LightCyan},
                {DUI_T("LightGoldenrodYellow"),UiColors::LightGoldenrodYellow},
                {DUI_T("LightGray"),UiColors::LightGray},
                {DUI_T("LightGreen"),UiColors::LightGreen},
                {DUI_T("LightPink"),UiColors::LightPink},
                {DUI_T("LightSalmon"),UiColors::LightSalmon},
                {DUI_T("LightSeaGreen"),UiColors::LightSeaGreen},
                {DUI_T("LightSkyBlue"),UiColors::LightSkyBlue},
                {DUI_T("LightSlateGray"),UiColors::LightSlateGray},
                {DUI_T("LightSteelBlue"),UiColors::LightSteelBlue},
                {DUI_T("LightYellow"),UiColors::LightYellow},
                {DUI_T("Lime"),UiColors::Lime},
                {DUI_T("LimeGreen"),UiColors::LimeGreen},
                {DUI_T("Linen"),UiColors::Linen},
                {DUI_T("Magenta"),UiColors::Magenta},
                {DUI_T("Maroon"),UiColors::Maroon},
                {DUI_T("MediumAquamarine"),UiColors::MediumAquamarine},
                {DUI_T("MediumBlue"),UiColors::MediumBlue},
                {DUI_T("MediumOrchid"),UiColors::MediumOrchid},
                {DUI_T("MediumPurple"),UiColors::MediumPurple},
                {DUI_T("MediumSeaGreen"),UiColors::MediumSeaGreen},
                {DUI_T("MediumSlateBlue"),UiColors::MediumSlateBlue},
                {DUI_T("MediumSpringGreen"),UiColors::MediumSpringGreen},
                {DUI_T("MediumTurquoise"),UiColors::MediumTurquoise},
                {DUI_T("MediumVioletRed"),UiColors::MediumVioletRed},
                {DUI_T("MidnightBlue"),UiColors::MidnightBlue},
                {DUI_T("MintCream"),UiColors::MintCream},
                {DUI_T("MistyRose"),UiColors::MistyRose},
                {DUI_T("Moccasin"),UiColors::Moccasin},
                {DUI_T("NavajoWhite"),UiColors::NavajoWhite},
                {DUI_T("Navy"),UiColors::Navy},
                {DUI_T("OldLace"),UiColors::OldLace},
                {DUI_T("Olive"),UiColors::Olive},
                {DUI_T("OliveDrab"),UiColors::OliveDrab},
                {DUI_T("Orange"),UiColors::Orange},
                {DUI_T("OrangeRed"),UiColors::OrangeRed},
                {DUI_T("Orchid"),UiColors::Orchid},
                {DUI_T("PaleGoldenrod"),UiColors::PaleGoldenrod},
                {DUI_T("PaleGreen"),UiColors::PaleGreen},
                {DUI_T("PaleTurquoise"),UiColors::PaleTurquoise},
                {DUI_T("PaleVioletRed"),UiColors::PaleVioletRed},
                {DUI_T("PapayaWhip"),UiColors::PapayaWhip},
                {DUI_T("PeachPuff"),UiColors::PeachPuff},
                {DUI_T("Peru"),UiColors::Peru},
                {DUI_T("Pink"),UiColors::Pink},
                {DUI_T("Plum"),UiColors::Plum},
                {DUI_T("PowderBlue"),UiColors::PowderBlue},
                {DUI_T("Purple"),UiColors::Purple},
                {DUI_T("Red"),UiColors::Red},
                {DUI_T("RosyBrown"),UiColors::RosyBrown},
                {DUI_T("RoyalBlue"),UiColors::RoyalBlue},
                {DUI_T("SaddleBrown"),UiColors::SaddleBrown},
                {DUI_T("Salmon"),UiColors::Salmon},
                {DUI_T("SandyBrown"),UiColors::SandyBrown},
                {DUI_T("SeaGreen"),UiColors::SeaGreen},
                {DUI_T("SeaShell"),UiColors::SeaShell},
                {DUI_T("Sienna"),UiColors::Sienna},
                {DUI_T("Silver"),UiColors::Silver},
                {DUI_T("SkyBlue"),UiColors::SkyBlue},
                {DUI_T("SlateBlue"),UiColors::SlateBlue},
                {DUI_T("SlateGray"),UiColors::SlateGray},
                {DUI_T("Snow"),UiColors::Snow},
                {DUI_T("SpringGreen"),UiColors::SpringGreen},
                {DUI_T("SteelBlue"),UiColors::SteelBlue},
                {DUI_T("Tan"),UiColors::Tan},
                {DUI_T("Teal"),UiColors::Teal},
                {DUI_T("Thistle"),UiColors::Thistle},
                {DUI_T("Tomato"),UiColors::Tomato},
                {DUI_T("Transparent"),UiColors::Transparent},
                {DUI_T("Turquoise"),UiColors::Turquoise},
                {DUI_T("Violet"),UiColors::Violet},
                {DUI_T("Wheat"),UiColors::Wheat},
                {DUI_T("White"),UiColors::White},
                {DUI_T("WhiteSmoke"),UiColors::WhiteSmoke},
                {DUI_T("Yellow"),UiColors::Yellow},
                {DUI_T("YellowGreen"),UiColors::YellowGreen}
    };
}

void ColorPickerRegularProvider::GetBasicColors(std::vector<std::pair<DString, int32_t>>& uiColors)
{
    uiColors = {
        {DUI_T("Rose"),0xFFF08784},
        {DUI_T("Rose"),0xFFEB3324},
        {DUI_T("Brown"),0xFF774342},
        {DUI_T("Red"),0xFF8E403A},
        {DUI_T("Dark Red"),0xFF3A0603},
        {DUI_T("Sky Blue"),0xFF9FFCFD},
        {DUI_T("Sky Blue"),0xFF73FBFD},
        {DUI_T("Blue"),0xFF3282F6},
        {DUI_T("Blue"),0xFF0023F5},
        {DUI_T("Dark Blue"),0xFF00129A},
        {DUI_T("Dark Blue"),0xFF16417C},
        {DUI_T("Dark Blue"),0xFF000C7B},

        {DUI_T("Light Yellow"),0xFFFFFE91},
        {DUI_T("Yellow"),0xFFFFFD55},
        {DUI_T("Orange"),0xFFF09B59},
        {DUI_T("Orange"),0xFFF08650},
        {DUI_T("Brown"),0xFF784315},
        {DUI_T("Dark Yellow"),0xFF817F26},
        {DUI_T("Light Blue"),0xFF7E84F7},
        {DUI_T("Purple"),0xFF732BF5},
        {DUI_T("Blue"),0xFF3580BB},
        {DUI_T("Dark Blue"),0xFF00023D},
        {DUI_T("Dark Purple"),0xFF58135E},
        {DUI_T("Dark Purple"),0xFF3A083E},

        {DUI_T("Light Green"),0xFFA1FB8E},
        {DUI_T("Green"),0xFFA1FA4F},
        {DUI_T("Green"),0xFF75F94D},
        {DUI_T("Light Green"),0xFF75FA61},
        {DUI_T("Light Green"),0xFF75FA8D},
        {DUI_T("Brown"),0xFF818049},
        {DUI_T("Pink"),0xFFEF88BE},
        {DUI_T("Light Purple"),0xFFEE8AF8},
        {DUI_T("Pale Purple"),0xFFEA3FF7},
        {DUI_T("Pink"),0xFFEA3680},
        {DUI_T("Pale Purple"),0xFF7F82BB},
        {DUI_T("Magenta"),0xFF75163F},

        {DUI_T("Light Green"),0xFF377D22},
        {DUI_T("Dark Green"),0xFF377E47},
        {DUI_T("Dark Cyan"),0xFF367E7F},
        {DUI_T("Cyan"),0xFF507F80},
        {DUI_T("Dark Green"),0xFF183E0C},
        {DUI_T("Dark Cyan"),0xFF173F3F},
        {DUI_T("Dark Purple"),0xFF741B7C},
        {DUI_T("Dark Purple"),0xFF39107B},
        {DUI_T("Black"),0xFF000000},
        {DUI_T("Gray"),0xFF808080},
        {DUI_T("Light Gray"),0xFFC0C0C0},
        {DUI_T("White"),0xFFFFFFFF}
    };
}

}//namespace ui
