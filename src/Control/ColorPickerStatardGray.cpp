#include "dui/Control/ColorPickerStatardGray.h"
#include "dui/Utils/StringUtil.h"
#include "dui/Core/GlobalManager.h"

namespace ui
{

ColorPickerStatardGray::ColorPickerStatardGray(Window* pWindow):
    Control(pWindow)
{
    InitColorMap();
}

DString ColorPickerStatardGray::GetType() const { return DUI_CTR_COLOR_PICKER_STANDARD_GRAY; }

void ColorPickerStatardGray::SelectColor(const UiColor& color)
{
    m_selectedColor = color;
    Invalidate();
}

void ColorPickerStatardGray::Paint(IRender* pRender, const UiRect& rcPaint)
{
    BaseClass::Paint(pRender, rcPaint);
    ASSERT(pRender != nullptr);
    if (pRender == nullptr) {
        return;
    }
    IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
    ASSERT(pRenderFactory != nullptr);
    if (pRenderFactory == nullptr) {
        return;
    }
        
    UiRect rect = GetRect();
    UiPadding rcPadding = this->GetControlPadding(); //Padding
    rect.Deflate(rcPadding);
    DrawColorMap(pRender, rect);
}

UiEstSize ColorPickerStatardGray::EstimateSize(UiSize szAvailable)
{
    UiFixedSize fixedSize;
    UiEstSize returnEstSize;
    if (!PreEstimateSize(szAvailable, fixedSize, returnEstSize)) {
        return returnEstSize;
    }

    const UiPadding rcPadding = this->GetControlPadding(); //Padding
    szAvailable.cx -= (rcPadding.left + rcPadding.right);
        
    const float cosRadius = std::cos(30 / 57.2957795f);//The cosine length of 30 degrees of the side length
    int32_t bigRadius = static_cast<int32_t>(szAvailable.cy / 2);
    int32_t smallRadius = bigRadius / 2;
    const int32_t marginX = Dpi().GetScaleInt(10);
    int32_t totalWidth = static_cast<int32_t>(smallRadius * 23 * cosRadius + marginX * 2);
    if (totalWidth > szAvailable.cx) {
        smallRadius = static_cast<int32_t>((szAvailable.cx - marginX * 2) / 23 / cosRadius);
        bigRadius = smallRadius * 2;
    }
    totalWidth = static_cast<int32_t>(smallRadius * 23 * cosRadius + marginX * 2);

    UiSize colorSize;
    colorSize.cx = totalWidth;
    colorSize.cy = bigRadius * 2;

    if (colorSize.cx > 0) {
        colorSize.cx += (rcPadding.left + rcPadding.right);
    }
    if (colorSize.cy > 0) {
        colorSize.cy += (rcPadding.top + rcPadding.bottom);
    }

    //Take the maximum of the height and width of the image and text areas
    if (fixedSize.cx.IsAuto()) {
        fixedSize.cx.SetInt32(colorSize.cx);
    }
    if (fixedSize.cy.IsAuto()) {
        fixedSize.cy.SetInt32(colorSize.cy);
    }
    //Cache the result to avoid re-estimating every time
    UiEstSize estSize = MakeEstSize(fixedSize);
    SetEstimateSize(estSize, szAvailable);
    SetReEstimateSize(false);
    return estSize;
}

void ColorPickerStatardGray::DrawColorMap(IRender* pRender, const UiRect& rect)
{
    if (m_colorMap.size() != 16) {
        return;
    }
    const float cosRadius = std::cos(30 / 57.2957795f);//The cosine length of 30 degrees of the side length
    int32_t bigRadius = static_cast<int32_t>(rect.Height() / 2);
    int32_t smallRadius = bigRadius / 2;
    const int32_t marginX = Dpi().GetScaleInt(10);
    int32_t totalWidth = static_cast<int32_t>(smallRadius * 23 * cosRadius + marginX * 2);
    if (totalWidth > rect.Width()) {
        smallRadius = static_cast<int32_t>((rect.Width() - marginX * 2) / 23 / cosRadius);
        bigRadius = smallRadius * 2;
    }
    totalWidth = static_cast<int32_t>(smallRadius * 23 * cosRadius + marginX * 2);

    UiPointF firstCenterPt((float)rect.left + bigRadius * cosRadius, (float)rect.top + bigRadius);
    if (rect.Width() > totalWidth) {
        firstCenterPt.x += ((rect.Width() - totalWidth) / 2);
    }
    if (rect.Height() > (bigRadius * 2)) {
        firstCenterPt.y += ((rect.Height() - bigRadius * 2) / 2);
    }

    //The drawing parameters of the currently selected image
    const UiColor penColor = UiColor(UiColors::Orange);
    const float penWidth = Dpi().GetScaleFloat(2);
    UiPointF selectCenterPt;
    UiColor selectBrushColor;
    int32_t selectedRadius = 0;
        
    //Draw the first large one
    if (!m_colorMap.empty()) {
        UiPointF centerPt = firstCenterPt;
        ColorInfo& colorInfo = m_colorMap[0];
        UiColor brushColor = colorInfo.color;
        colorInfo.m_radius = bigRadius;
        colorInfo.centerPt = centerPt;
        if (m_selectedColor == brushColor) {
            //The currently selected color, displayed with a thicker border
            selectCenterPt = centerPt;
            selectBrushColor = brushColor;
            selectedRadius = bigRadius;
        }
        else {
            DrawRegularHexagon(pRender, centerPt, bigRadius, UiColor(), 0, brushColor);
        }        
    }

    //Draw the small ones in the middle: first row
    UiPointF centerPt = firstCenterPt;
    centerPt.y -= smallRadius;
    centerPt.x += (bigRadius * cosRadius + marginX + smallRadius * 2 * cosRadius);
    size_t startIndex = 1;
    for (size_t index = startIndex; index < (startIndex + 7); ++index) {
        ColorInfo& colorInfo = m_colorMap[index];
        UiColor brushColor = colorInfo.color;
        colorInfo.m_radius = smallRadius;
        colorInfo.centerPt = centerPt;
        if (m_selectedColor == brushColor) {
            //The currently selected color, displayed with a thicker border
            selectCenterPt = centerPt;
            selectBrushColor = brushColor;
            selectedRadius = smallRadius;
        }
        else {
            DrawRegularHexagon(pRender, centerPt, smallRadius, UiColor(), 0, brushColor);
        }
        //The center coordinates of the next hexagon
        centerPt.x += smallRadius * 2 * cosRadius;
    }

    //Second row
    centerPt = firstCenterPt;
    centerPt.y += (smallRadius * 0.5f);
    centerPt.x += (bigRadius * cosRadius + marginX + smallRadius * 1 * cosRadius);
    startIndex = 8;
    for (size_t index = startIndex; index < (startIndex + 7); ++index) {
        ColorInfo& colorInfo = m_colorMap[index];

        UiColor brushColor = colorInfo.color;
        colorInfo.m_radius = smallRadius;
        colorInfo.centerPt = centerPt;
        if (m_selectedColor == brushColor) {
            //The currently selected color, displayed with a thicker border
            selectCenterPt = centerPt;
            selectBrushColor = brushColor;
            selectedRadius = smallRadius;
        }
        else {
            DrawRegularHexagon(pRender, centerPt, smallRadius, UiColor(), 0, brushColor);
        }
        //The center coordinates of the next hexagon
        centerPt.x += smallRadius * 2 * cosRadius;
    }

    //Draw the last large one
    if (!m_colorMap.empty()) {
        centerPt = firstCenterPt;
        centerPt.x += (bigRadius * 2 * cosRadius + marginX * 2 + smallRadius * 15 * cosRadius);
        ColorInfo& colorInfo = m_colorMap[m_colorMap.size() - 1];
        UiColor brushColor = colorInfo.color;
        colorInfo.m_radius = bigRadius;
        colorInfo.centerPt = centerPt;
        if (m_selectedColor == brushColor) {
            //The currently selected color, displayed with a thicker border
            selectCenterPt = centerPt;
            selectBrushColor = brushColor;
            selectedRadius = bigRadius;
        }
        else {
            DrawRegularHexagon(pRender, centerPt, bigRadius, UiColor(), 0, brushColor);
        }        
    }

    //Draw the selected hexagon (to avoid the selection border being covered by content drawn later, which would affect the appearance)
    if (!selectBrushColor.IsEmpty()) {
        DrawRegularHexagon(pRender, selectCenterPt, selectedRadius, penColor, penWidth, selectBrushColor);
    }
}

bool ColorPickerStatardGray::DrawRegularHexagon(IRender* pRender, const UiPointF& centerPt, int32_t radius,
                                                const UiColor& penColor, float penWidth, const UiColor& brushColor)
{
    ASSERT(pRender != nullptr);
    if (pRender == nullptr) {
        return false;
    }
    IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
    ASSERT(pRenderFactory != nullptr);
    if (pRenderFactory == nullptr) {
        return false;
    }
    ASSERT(radius > 0); //The radius of the polygon
    if (radius <= 0) {
        return false;
    }

    constexpr const int32_t count = 6; //The number of sides of the polygon
    //The coordinates of any vertex of a regular polygon are: x = r * cos(θ) y = r * sin(θ)
    std::vector<UiPointF> polygonPoints;
    for (int32_t i = 0; i < count; ++i) {
        int32_t degree = i * 60 + 30;// +30 is to make the vertex at the top of the center point
        float radian = degree / 57.2957795f;
        float x = radius * std::cos(radian) + 0.5f;
        float y = radius * std::sin(radian) + 0.5f;
        polygonPoints.push_back(UiPointF(centerPt.x + x, centerPt.y + y));
    }

    std::unique_ptr<IPath> path(pRenderFactory->CreatePath());
    path->AddPolygon(polygonPoints.data(), (int32_t)polygonPoints.size());
    path->Close();

    bool bRet = false;
    if (!brushColor.IsEmpty()) {
        std::unique_ptr<IBrush> brush(pRenderFactory->CreateBrush(brushColor));
        pRender->FillPath(path.get(), brush.get());
        bRet = true;
    }
    if (!penColor.IsEmpty() && (penWidth > 0.1f)) {
        std::unique_ptr<IPen> pen(pRenderFactory->CreatePen(penColor, penWidth));
        pRender->DrawPath(path.get(), pen.get());
        bRet = true;
    }
    return bRet;
}

bool ColorPickerStatardGray::MouseMove(const EventArgs& msg)
{
    //Update the ToolTip information
    if (GetRect().ContainsPt(msg.ptMouse)) {
        UiColor color;
        if (GetColorInfo(msg.ptMouse, color)) {
            DString colorString = StringUtil::Printf(_T("#%02X%02X%02X%02X"), color.GetA(), color.GetR(), color.GetG(), color.GetB());
            SetToolTipText(colorString);
        }
        else {
            SetToolTipText(_T(""));
        }
    }
    return BaseClass::MouseMove(msg);
}

bool ColorPickerStatardGray::ButtonDown(const EventArgs& msg)
{
    bool bRet = BaseClass::ButtonDown(msg);
    if (msg.IsSenderExpired()) {
        return false;
    }
    if (GetRect().ContainsPt(msg.ptMouse)) {
        UiColor color;
        if (GetColorInfo(msg.ptMouse, color)) {
            //The current color is selected
            m_selectedColor = color;
            Invalidate();
            SendEvent(kEventSelectColor, color.GetARGB());
        }
        else {
            Invalidate();
        }
    }
    return bRet;
}

bool ColorPickerStatardGray::GetColorInfo(const UiPoint& ptMouse, UiColor& ptColor) const
{
    struct ColorPt
    {
        //The color value
        UiColor color;
        //The distance between the hexagon center point corresponding to this color value and ptMouse
        float distance;
        //Comparison function; the closest one is sorted first
        bool operator < (const ColorPt& r) const
        {
            return distance < r.distance;
        }
    };

    std::vector<ColorPt> maybeColors;
    for (const ColorInfo& colorInfo : m_colorMap) {
        float distance = GetPointsDistance(colorInfo.centerPt,
                                            UiPointF((float)ptMouse.x, (float)ptMouse.y));
        if (distance <= colorInfo.m_radius) {
            maybeColors.push_back({ colorInfo.color, distance });
        }
    }

    if (maybeColors.empty()) {
        return false;
    }
    else {
        std::sort(maybeColors.begin(), maybeColors.end()); //Select the color whose hexagon center point is closest to the mouse point
        ptColor = maybeColors.front().color;
        return true;
    }        
}

float ColorPickerStatardGray::GetPointsDistance(const UiPointF& pt1, const UiPointF& pt2) const
{
    float a = std::abs(pt1.x - pt2.x);
    float b = std::abs(pt1.y - pt2.y);
    float c = sqrtf(a * a + b * b);
    return c;
}

void ColorPickerStatardGray::InitColorMap()
{
    m_colorMap = {
        {UiColor(0xFFFFFFFF),UiPointF(),0},
        {UiColor(0xFFDDDDDD),UiPointF(),0},
        {UiColor(0xFFB2B2B2),UiPointF(),0},
        {UiColor(0xFF808080),UiPointF(),0},
        {UiColor(0xFF5F5F5F),UiPointF(),0},
        {UiColor(0xFF333333),UiPointF(),0},
        {UiColor(0xFF1C1C1C),UiPointF(),0},
        {UiColor(0xFF080808),UiPointF(),0},
        {UiColor(0xFFEAEAEA),UiPointF(),0},
        {UiColor(0xFFC0C0C0),UiPointF(),0},
        {UiColor(0xFF969696),UiPointF(),0},
        {UiColor(0xFF777777),UiPointF(),0},
        {UiColor(0xFF4D4D4D),UiPointF(),0},
        {UiColor(0xFF292929),UiPointF(),0},
        {UiColor(0xFF111111),UiPointF(),0},
        {UiColor(0xFF000000),UiPointF(),0}
    };
    ASSERT(m_colorMap.size() == 16);
}

}//namespace ui
