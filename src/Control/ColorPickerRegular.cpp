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
    void SetColors(const std::vector<std::pair<std::string, int32_t>>& uiColors);

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
    void GetDefaultColors(std::vector<std::pair<std::string, int32_t>>& uiColors);

    /** Basic color values and their corresponding string constants
    */
    void GetBasicColors(std::vector<std::pair<std::string, int32_t>>& uiColors);

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
            std::string colorString = pControl->GetBkColor();
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

std::string ColorPickerRegular::GetType() const { return DUI_CTR_COLOR_PICKER_REGULAR; }

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

void ColorPickerRegular::SetAttribute(const std::string& strName, const std::string& strValue)
{
    if (strName == "color_type") {
        if (strValue == "basic") {
            //Use the basic colors
            std::vector<std::pair<std::string, int32_t>> uiColors;
            m_regularColors->GetBasicColors(uiColors);
            m_regularColors->SetColors(uiColors);
        }
        else {
            //Use the default colors
            std::vector<std::pair<std::string, int32_t>> uiColors;
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
    std::vector<std::pair<std::string, int32_t>> uiColors;
    GetDefaultColors(uiColors);
    SetColors(uiColors);
}

void ColorPickerRegularProvider::SetColors(const std::vector<std::pair<std::string, int32_t>>& uiColors)
{
    m_colors.clear();
    RegularColor regularColor;
    regularColor.m_bSelected = false;
    for (const auto& color : uiColors) {
        regularColor.colorValue = UiColor(color.second);
        std::string colorString = ui::StringUtil::Printf("#%02X%02X%02X%02X",
            regularColor.colorValue.GetA(),
            regularColor.colorValue.GetR(),
            regularColor.colorValue.GetG(),
            regularColor.colorValue.GetB());
        std::string colorName = color.first;
        StringUtil::ReplaceAll(",", ", ", colorName);
        colorName = colorString + ", " + colorName;
        regularColor.colorName = colorName;
        if (regularColor.colorValue.GetARGB() != UiColors::Transparent) {
            m_colors.push_back(regularColor);
        }
    }
    std::sort(m_colors.begin(), m_colors.end());
}

Control* ColorPickerRegularProvider::CreateElement(VirtualListBox* pVirtualListBox)
{
    if (pVirtualListBox == nullptr) {
        return nullptr;
    }
    if (pVirtualListBox->GetWindow() == nullptr) {
        return nullptr;
    }
    ListBoxItem* pControl = new ListBoxItem(pVirtualListBox->GetWindow());
    float fBorderSize = 2.0f;
    UiRectF borderSize(fBorderSize, fBorderSize, fBorderSize, fBorderSize);
    pControl->SetBorderSize(borderSize, true);
    pControl->SetBorderColor(kControlStatePushed, "blue");
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

void ColorPickerRegularProvider::GetDefaultColors(std::vector<std::pair<std::string, int32_t>>& uiColors)
{
    uiColors = {
                {"AliceBlue",UiColors::AliceBlue},
                {"AntiqueWhite",UiColors::AntiqueWhite},
                {"Aqua",UiColors::Aqua},
                {"Aquamarine",UiColors::Aquamarine},
                {"Azure",UiColors::Azure},
                {"Beige",UiColors::Beige},
                {"Bisque",UiColors::Bisque},
                {"Black",UiColors::Black},
                {"BlanchedAlmond",UiColors::BlanchedAlmond},
                {"Blue",UiColors::Blue},
                {"BlueViolet",UiColors::BlueViolet},
                {"Brown",UiColors::Brown},
                {"BurlyWood",UiColors::BurlyWood},
                {"CadetBlue",UiColors::CadetBlue},
                {"Chartreuse",UiColors::Chartreuse},
                {"Chocolate",UiColors::Chocolate},
                {"Coral",UiColors::Coral},
                {"CornflowerBlue",UiColors::CornflowerBlue},
                {"Cornsilk",UiColors::Cornsilk},
                {"Crimson",UiColors::Crimson},
                {"Cyan",UiColors::Cyan},
                {"DarkBlue",UiColors::DarkBlue},
                {"DarkCyan",UiColors::DarkCyan},
                {"DarkGoldenrod",UiColors::DarkGoldenrod},
                {"DarkGray",UiColors::DarkGray},
                {"DarkGreen",UiColors::DarkGreen},
                {"DarkKhaki",UiColors::DarkKhaki},
                {"DarkMagenta",UiColors::DarkMagenta},
                {"DarkOliveGreen",UiColors::DarkOliveGreen},
                {"DarkOrange",UiColors::DarkOrange},
                {"DarkOrchid",UiColors::DarkOrchid},
                {"DarkRed",UiColors::DarkRed},
                {"DarkSalmon",UiColors::DarkSalmon},
                {"DarkSeaGreen",UiColors::DarkSeaGreen},
                {"DarkSlateBlue",UiColors::DarkSlateBlue},
                {"DarkSlateGray",UiColors::DarkSlateGray},
                {"DarkTurquoise",UiColors::DarkTurquoise},
                {"DarkViolet",UiColors::DarkViolet},
                {"DeepPink",UiColors::DeepPink},
                {"DeepSkyBlue",UiColors::DeepSkyBlue},
                {"DimGray",UiColors::DimGray},
                {"DodgerBlue",UiColors::DodgerBlue},
                {"Firebrick",UiColors::Firebrick},
                {"FloralWhite",UiColors::FloralWhite},
                {"ForestGreen",UiColors::ForestGreen},
                {"Fuchsia",UiColors::Fuchsia},
                {"Gainsboro",UiColors::Gainsboro},
                {"GhostWhite",UiColors::GhostWhite},
                {"Gold",UiColors::Gold},
                {"Goldenrod",UiColors::Goldenrod},
                {"Gray",UiColors::Gray},
                {"Green",UiColors::Green},
                {"GreenYellow",UiColors::GreenYellow},
                {"Honeydew",UiColors::Honeydew},
                {"HotPink",UiColors::HotPink},
                {"IndianRed",UiColors::IndianRed},
                {"Indigo",UiColors::Indigo},
                {"Ivory",UiColors::Ivory},
                {"Khaki",UiColors::Khaki},
                {"Lavender",UiColors::Lavender},
                {"LavenderBlush",UiColors::LavenderBlush},
                {"LawnGreen",UiColors::LawnGreen},
                {"LemonChiffon",UiColors::LemonChiffon},
                {"LightBlue",UiColors::LightBlue},
                {"LightCoral",UiColors::LightCoral},
                {"LightCyan",UiColors::LightCyan},
                {"LightGoldenrodYellow",UiColors::LightGoldenrodYellow},
                {"LightGray",UiColors::LightGray},
                {"LightGreen",UiColors::LightGreen},
                {"LightPink",UiColors::LightPink},
                {"LightSalmon",UiColors::LightSalmon},
                {"LightSeaGreen",UiColors::LightSeaGreen},
                {"LightSkyBlue",UiColors::LightSkyBlue},
                {"LightSlateGray",UiColors::LightSlateGray},
                {"LightSteelBlue",UiColors::LightSteelBlue},
                {"LightYellow",UiColors::LightYellow},
                {"Lime",UiColors::Lime},
                {"LimeGreen",UiColors::LimeGreen},
                {"Linen",UiColors::Linen},
                {"Magenta",UiColors::Magenta},
                {"Maroon",UiColors::Maroon},
                {"MediumAquamarine",UiColors::MediumAquamarine},
                {"MediumBlue",UiColors::MediumBlue},
                {"MediumOrchid",UiColors::MediumOrchid},
                {"MediumPurple",UiColors::MediumPurple},
                {"MediumSeaGreen",UiColors::MediumSeaGreen},
                {"MediumSlateBlue",UiColors::MediumSlateBlue},
                {"MediumSpringGreen",UiColors::MediumSpringGreen},
                {"MediumTurquoise",UiColors::MediumTurquoise},
                {"MediumVioletRed",UiColors::MediumVioletRed},
                {"MidnightBlue",UiColors::MidnightBlue},
                {"MintCream",UiColors::MintCream},
                {"MistyRose",UiColors::MistyRose},
                {"Moccasin",UiColors::Moccasin},
                {"NavajoWhite",UiColors::NavajoWhite},
                {"Navy",UiColors::Navy},
                {"OldLace",UiColors::OldLace},
                {"Olive",UiColors::Olive},
                {"OliveDrab",UiColors::OliveDrab},
                {"Orange",UiColors::Orange},
                {"OrangeRed",UiColors::OrangeRed},
                {"Orchid",UiColors::Orchid},
                {"PaleGoldenrod",UiColors::PaleGoldenrod},
                {"PaleGreen",UiColors::PaleGreen},
                {"PaleTurquoise",UiColors::PaleTurquoise},
                {"PaleVioletRed",UiColors::PaleVioletRed},
                {"PapayaWhip",UiColors::PapayaWhip},
                {"PeachPuff",UiColors::PeachPuff},
                {"Peru",UiColors::Peru},
                {"Pink",UiColors::Pink},
                {"Plum",UiColors::Plum},
                {"PowderBlue",UiColors::PowderBlue},
                {"Purple",UiColors::Purple},
                {"Red",UiColors::Red},
                {"RosyBrown",UiColors::RosyBrown},
                {"RoyalBlue",UiColors::RoyalBlue},
                {"SaddleBrown",UiColors::SaddleBrown},
                {"Salmon",UiColors::Salmon},
                {"SandyBrown",UiColors::SandyBrown},
                {"SeaGreen",UiColors::SeaGreen},
                {"SeaShell",UiColors::SeaShell},
                {"Sienna",UiColors::Sienna},
                {"Silver",UiColors::Silver},
                {"SkyBlue",UiColors::SkyBlue},
                {"SlateBlue",UiColors::SlateBlue},
                {"SlateGray",UiColors::SlateGray},
                {"Snow",UiColors::Snow},
                {"SpringGreen",UiColors::SpringGreen},
                {"SteelBlue",UiColors::SteelBlue},
                {"Tan",UiColors::Tan},
                {"Teal",UiColors::Teal},
                {"Thistle",UiColors::Thistle},
                {"Tomato",UiColors::Tomato},
                {"Transparent",UiColors::Transparent},
                {"Turquoise",UiColors::Turquoise},
                {"Violet",UiColors::Violet},
                {"Wheat",UiColors::Wheat},
                {"White",UiColors::White},
                {"WhiteSmoke",UiColors::WhiteSmoke},
                {"Yellow",UiColors::Yellow},
                {"YellowGreen",UiColors::YellowGreen}
    };
}

void ColorPickerRegularProvider::GetBasicColors(std::vector<std::pair<std::string, int32_t>>& uiColors)
{
    uiColors = {
        {"Rose",0xFFF08784},
        {"Rose",0xFFEB3324},
        {"Brown",0xFF774342},
        {"Red",0xFF8E403A},
        {"Dark Red",0xFF3A0603},
        {"Sky Blue",0xFF9FFCFD},
        {"Sky Blue",0xFF73FBFD},
        {"Blue",0xFF3282F6},
        {"Blue",0xFF0023F5},
        {"Dark Blue",0xFF00129A},
        {"Dark Blue",0xFF16417C},
        {"Dark Blue",0xFF000C7B},

        {"Light Yellow",0xFFFFFE91},
        {"Yellow",0xFFFFFD55},
        {"Orange",0xFFF09B59},
        {"Orange",0xFFF08650},
        {"Brown",0xFF784315},
        {"Dark Yellow",0xFF817F26},
        {"Light Blue",0xFF7E84F7},
        {"Purple",0xFF732BF5},
        {"Blue",0xFF3580BB},
        {"Dark Blue",0xFF00023D},
        {"Dark Purple",0xFF58135E},
        {"Dark Purple",0xFF3A083E},

        {"Light Green",0xFFA1FB8E},
        {"Green",0xFFA1FA4F},
        {"Green",0xFF75F94D},
        {"Light Green",0xFF75FA61},
        {"Light Green",0xFF75FA8D},
        {"Brown",0xFF818049},
        {"Pink",0xFFEF88BE},
        {"Light Purple",0xFFEE8AF8},
        {"Pale Purple",0xFFEA3FF7},
        {"Pink",0xFFEA3680},
        {"Pale Purple",0xFF7F82BB},
        {"Magenta",0xFF75163F},

        {"Light Green",0xFF377D22},
        {"Dark Green",0xFF377E47},
        {"Dark Cyan",0xFF367E7F},
        {"Cyan",0xFF507F80},
        {"Dark Green",0xFF183E0C},
        {"Dark Cyan",0xFF173F3F},
        {"Dark Purple",0xFF741B7C},
        {"Dark Purple",0xFF39107B},
        {"Black",0xFF000000},
        {"Gray",0xFF808080},
        {"Light Gray",0xFFC0C0C0},
        {"White",0xFFFFFFFF}
    };
}

}//namespace ui
