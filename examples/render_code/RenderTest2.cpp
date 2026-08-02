#include "RenderTest2.h"
#include "duilib/Utils/BitmapHelper_Windows.h"

namespace ui {

RenderTest2::RenderTest2(ui::Window* pWindow):
    ui::Control(pWindow)
{
}

RenderTest2::~RenderTest2()
{
}

void RenderTest2::AlphaPaint(IRender* pRender, const UiRect& rcPaint)
{
    BaseClass::AlphaPaint(pRender, rcPaint);
}

void RenderTest2::Paint(IRender* pRender, const UiRect& rcPaint)
{
    BaseClass::Paint(pRender, rcPaint);
    IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
    ASSERT(pRenderFactory != nullptr);

    int marginLeft = 8;
    int marginTop = 4;
    Dpi().ScaleInt(marginLeft);
    Dpi().ScaleInt(marginTop);

    UiRect rect = GetRect();
    UiRect textRect = rect;
    rect.left += marginLeft;
    rect.top += marginTop;

    int nSize = 110;
    int nTextLineHeight = 40;
    Dpi().ScaleInt(nSize);
    Dpi().ScaleInt(nTextLineHeight);

    rect.right = rect.left + nSize;
    rect.bottom = rect.top + nSize;
    int currentBottom = rect.bottom;//Record the current bottom value

    //Draw a line
    int32_t nLineIndex = 0;
    const int sep = DpiScaledInt(10);
    for (int32_t topValue = rect.top; topValue <= rect.bottom; topValue += sep) {
        const int nLineWidth = 3;
        const float fWidth = Dpi().GetDisplayScale() * nLineWidth;
        if ((nLineIndex % 2) == 0) {
            pRender->DrawLine(UiPointF((float)rect.left, (float)topValue), UiPointF((float)rect.right, (float)topValue), UiColor(UiColors::DarkCyan), fWidth);
        }
        else {
            pRender->DrawLine(UiPointF(rect.left, topValue), UiPointF(rect.right, topValue), UiColor(UiColors::DarkCyan), (float)DpiScaledInt(nLineWidth));
        }
        ++nLineIndex;
    }
    textRect = rect;
    textRect.top = rect.bottom;
    textRect.bottom = textRect.top + nTextLineHeight;

    DrawStringParam drawParam;
    drawParam.textRect = textRect;
    drawParam.dwTextColor = UiColor(UiColors::Blue);
    drawParam.pFont = GetIFontById(_T("system_14"));
    drawParam.uFormat = TEXT_HCENTER | TEXT_VCENTER;

    pRender->DrawString(_T("DrawLine"), drawParam);

    //Draw lines with various line styles
    rect.Offset(UiPoint(rect.Width() + 10, 0));
    if (pRenderFactory != nullptr) {
        std::unique_ptr<IPen> pen(pRenderFactory->CreatePen(UiColor(UiColors::CornflowerBlue), DpiScaledFloat(2)));
        int32_t style = 0;
        for (int32_t topValue = rect.top; topValue <= rect.bottom; topValue += sep) {
            if (style == 0) {
                pen->SetDashStyle(ui::IPen::DashStyle::kDashStyleSolid);
            }
            else if (style == 1) {
                pen->SetDashStyle(ui::IPen::DashStyle::kDashStyleDash);
            }
            else if (style == 2) {
                pen->SetDashStyle(ui::IPen::DashStyle::kDashStyleDot);
            }
            else if (style == 3) {
                pen->SetDashStyle(ui::IPen::DashStyle::kDashStyleDashDot);
            }
            else if (style == 4) {
                pen->SetDashStyle(ui::IPen::DashStyle::kDashStyleDashDotDot);
            }
            pRender->DrawLine(UiPointF(rect.left, topValue), UiPointF(rect.right, topValue), pen.get());
            ++style;
            if (style > 4) {
                style = 0;
            }
        }
    }


    //Draw a regular hexagon
    UiRect hexagonRect = rect;
    hexagonRect.Offset(UiPoint(rect.Width() + 10, 0));
    DrawRegularHexagon3(pRender, hexagonRect.Center(), rect.Width() / 2, UiColor(UiColors::White), 2, UiColor(UiColors::Olive));

    //Draw a regular hexagon
    hexagonRect.Offset(UiPoint(rect.Width() + 10, 0));
    UiPointF centerF((float)hexagonRect.CenterX(), (float)hexagonRect.CenterY());
    DrawRegularHexagon(pRender, centerF, rect.Width() / 2, UiColor(UiColors::White), 2, UiColor(UiColors::SandyBrown));

    //Compose a complex shape with regular hexagons
    hexagonRect.Offset(UiPoint(rect.Width() + 10, 0));
    DrawColorMap(pRender, hexagonRect);

    //New line
    currentBottom = textRect.bottom;//Record the current bottom value
    rect = GetRect();
    rect.left += marginLeft;
    rect.right = rect.left;
    rect.top = currentBottom + marginTop;
    rect.bottom = rect.top + nSize;
    
    //Draw a rectangle
    rect.left = rect.right + marginLeft;
    rect.right = rect.left + nSize;
    pRender->DrawRect(UiRectF::MakeFromRect(rect), UiColor(UiColors::Fuchsia), (float)DpiScaledInt(2));
    textRect = rect;
    textRect.top = rect.bottom;
    textRect.bottom = textRect.top + nTextLineHeight;

    drawParam.textRect = textRect;
    pRender->DrawString(_T("DrawRect"), drawParam);

    //Draw rectangles with various line styles
    for (size_t style = 0; style < 5; ++style) {
        rect.left = rect.right + marginLeft;
        rect.right = rect.left + nSize;

        std::unique_ptr<IPen> pen(pRenderFactory->CreatePen(UiColor(UiColors::CornflowerBlue), DpiScaledFloat(2)));
        if (style == 0) {
            pen->SetDashStyle(ui::IPen::DashStyle::kDashStyleSolid);
        }
        else if (style == 1) {
            pen->SetDashStyle(ui::IPen::DashStyle::kDashStyleDash);
        }
        else if (style == 2) {
            pen->SetDashStyle(ui::IPen::DashStyle::kDashStyleDot);
        }
        else if (style == 3) {
            pen->SetDashStyle(ui::IPen::DashStyle::kDashStyleDashDot);
        }
        else if (style == 4) {
            pen->SetDashStyle(ui::IPen::DashStyle::kDashStyleDashDotDot);
        }
        pRender->DrawRect(UiRectF::MakeFromRect(rect), pen.get());
    }
    

    //Fill a rectangle
    rect.left = rect.right + marginLeft;
    rect.right = rect.left + nSize;
    pRender->FillRect(UiRectF::MakeFromRect(rect), UiColor(UiColors::Brown));
    textRect = rect;
    textRect.top = rect.bottom;
    textRect.bottom = textRect.top + nTextLineHeight;

    drawParam.textRect = textRect;
    pRender->DrawString(_T("FillRect"), drawParam);

    rect.left = rect.right + marginLeft;
    rect.right = rect.left + nSize;
    pRender->FillRect(UiRectF::MakeFromRect(rect), UiColor(UiColors::Brown), 128);
    textRect = rect;
    textRect.top = rect.bottom;
    textRect.bottom = textRect.top + nTextLineHeight;

    drawParam.textRect = textRect;
    pRender->DrawString(_T("FillRect Alpha"), drawParam);

    //New line
    currentBottom = textRect.bottom;//Record the current bottom value
    rect = GetRect();
    rect.left += marginLeft;
    rect.right = rect.left;
    rect.top = currentBottom + marginTop;
    rect.bottom = rect.top + nSize;

    //Draw a rounded rectangle
    rect.left = rect.right + marginLeft;
    rect.right = rect.left + nSize;

    //The corner radius must be even; otherwise the path cannot be closed
    UiSize roundSize(12, 12);
    Dpi().ScaleSize(roundSize);
    roundSize.cx = (roundSize.cx / 2) * 2;
    roundSize.cy = (roundSize.cy / 2) * 2;

    pRender->DrawRoundRect(UiRectF::MakeFromRect(rect), (float)roundSize.cx, (float)roundSize.cy, UiColor(0xffC63535), DpiScaledFloat(2));
    textRect = rect;
    textRect.top = rect.bottom;
    textRect.bottom = textRect.top + nTextLineHeight;

    drawParam.textRect = textRect;
    drawParam.pFont = GetIFontById(_T("system_12"));
    pRender->DrawString(_T("DrawRoundRect"), drawParam);

    //Draw rounded rectangles with various line styles
    for (size_t style = 0; style < 5; ++style) {
        rect.left = rect.right + marginLeft;
        rect.right = rect.left + nSize;

        std::unique_ptr<IPen> pen(pRenderFactory->CreatePen(UiColor(UiColors::CornflowerBlue), DpiScaledFloat(2)));
        if (style == 0) {
            pen->SetDashStyle(ui::IPen::DashStyle::kDashStyleSolid);
        }
        else if (style == 1) {
            pen->SetDashStyle(ui::IPen::DashStyle::kDashStyleDash);
        }
        else if (style == 2) {
            pen->SetDashStyle(ui::IPen::DashStyle::kDashStyleDot);
        }
        else if (style == 3) {
            pen->SetDashStyle(ui::IPen::DashStyle::kDashStyleDashDot);
        }
        else if (style == 4) {
            pen->SetDashStyle(ui::IPen::DashStyle::kDashStyleDashDotDot);
        }
        pRender->DrawRoundRect(UiRectF::MakeFromRect(rect), (float)roundSize.cx, (float)roundSize.cy, pen.get());
    }

    //Fill a rounded rectangle
    rect.left = rect.right + marginLeft;
    rect.right = rect.left + nSize;
    pRender->FillRoundRect(UiRectF::MakeFromRect(rect), (float)roundSize.cx, (float)roundSize.cy, UiColor(UiColors::Blue));
    textRect = rect;
    textRect.top = rect.bottom;
    textRect.bottom = textRect.top + nTextLineHeight;

    drawParam.textRect = textRect;
    drawParam.pFont = GetIFontById(_T("system_14"));
    pRender->DrawString(_T("FillRoundRect"), drawParam);

    rect.left = rect.right + marginLeft;
    rect.right = rect.left + nSize;
    pRender->FillRoundRect(UiRectF::MakeFromRect(rect), (float)roundSize.cx, (float)roundSize.cy, UiColor(UiColors::Blue), 128);
    textRect = rect;
    textRect.top = rect.bottom;
    textRect.bottom = textRect.top + nTextLineHeight;

    drawParam.textRect = textRect;
    pRender->DrawString(_T("FillRoundRect Alpha"), drawParam);

    //New line
    currentBottom = textRect.bottom;//Record the current bottom value
    rect = GetRect();
    rect.left += marginLeft;
    rect.right = rect.left;
    rect.top = currentBottom + marginTop;
    rect.bottom = rect.top + nSize;

    //Draw/fill a circle
    rect.left = rect.right + marginLeft;
    rect.right = rect.left + nSize;
    float fRadius = std::min(rect.Width(), rect.Height()) / 2.0f;//Radius of the circle
    pRender->DrawCircle(UiPointF::MakeFromPoint(rect.Center()), fRadius, UiColor(UiColors::Blue), DpiScaledFloat(2));
    textRect = rect;
    textRect.top = rect.bottom;
    textRect.bottom = textRect.top + nTextLineHeight;

    drawParam.textRect = textRect;
    pRender->DrawString(_T("DrawCircle"), drawParam);

    //Draw circles with various line styles
    for (size_t style = 0; style < 5; ++style) {
        rect.left = rect.right + marginLeft;
        rect.right = rect.left + nSize;

        std::unique_ptr<IPen> pen(pRenderFactory->CreatePen(UiColor(UiColors::CornflowerBlue), DpiScaledFloat(2)));
        if (style == 0) {
            pen->SetDashStyle(ui::IPen::DashStyle::kDashStyleSolid);
        }
        else if (style == 1) {
            pen->SetDashStyle(ui::IPen::DashStyle::kDashStyleDash);
        }
        else if (style == 2) {
            pen->SetDashStyle(ui::IPen::DashStyle::kDashStyleDot);
        }
        else if (style == 3) {
            pen->SetDashStyle(ui::IPen::DashStyle::kDashStyleDashDot);
        }
        else if (style == 4) {
            pen->SetDashStyle(ui::IPen::DashStyle::kDashStyleDashDotDot);
        }
        pRender->DrawCircle(UiPointF::MakeFromPoint(rect.Center()), fRadius, pen.get());
    }

    rect.left = rect.right + marginLeft;
    rect.right = rect.left + nSize;
    pRender->FillCircle(UiPointF::MakeFromPoint(rect.Center()), fRadius, UiColor(UiColors::CadetBlue), 255);
    textRect = rect;
    textRect.top = rect.bottom;
    textRect.bottom = textRect.top + nTextLineHeight;

    drawParam.textRect = textRect;
    pRender->DrawString(_T("FillCircle"), drawParam);

    rect.left = rect.right + marginLeft;
    rect.right = rect.left + nSize;
    pRender->FillCircle(UiPointF::MakeFromPoint(rect.Center()), fRadius, UiColor(UiColors::CadetBlue), 96);
    textRect = rect;
    textRect.top = rect.bottom;
    textRect.bottom = textRect.top + nTextLineHeight;

    drawParam.textRect = textRect;
    pRender->DrawString(_T("FillCircle Alpha"), drawParam);
    
    //New line
    currentBottom = textRect.bottom;//Record the current bottom value
    rect = GetRect();
    rect.left += marginLeft;
    rect.right = rect.left;
    rect.top = currentBottom + marginTop;
    rect.bottom = rect.top + nSize;

    //Draw a rounded quadrilateral with DrawPath
    rect.left = rect.right + marginLeft;
    rect.right = rect.left + nSize;    
    if (pRenderFactory != nullptr) {
        std::unique_ptr<IPen> pen(pRenderFactory->CreatePen(UiColor(0xff006DD9), DpiScaledFloat(2)));
        std::unique_ptr<IPath> path(pRenderFactory->CreatePath());
        if (pen && path) {
            const UiRect& rc = rect;
            path->AddArc(UiRect(rc.left, rc.top, rc.left + roundSize.cx, rc.top + roundSize.cy), 180, 90);
            path->AddLine(rc.left + roundSize.cx / 2, rc.top, rc.right - roundSize.cx / 2, rc.top);
            path->AddArc(UiRect(rc.right - roundSize.cx, rc.top, rc.right, rc.top + roundSize.cy), 270, 90);
            path->AddLine(rc.right, rc.top + roundSize.cy / 2, rc.right, rc.bottom - roundSize.cy / 2);
            path->AddArc(UiRect(rc.right - roundSize.cx, rc.bottom - roundSize.cy, rc.right, rc.bottom), 0, 90);
            path->AddLine(rc.right - roundSize.cx / 2, rc.bottom, rc.left + roundSize.cx / 2, rc.bottom);
            path->AddArc(UiRect(rc.left, rc.bottom - roundSize.cy, rc.left + roundSize.cx, rc.bottom), 90, 90);
            path->AddLine(rc.left, rc.bottom - roundSize.cy / 2, rc.left, rc.top + roundSize.cy / 2);
            path->Close();

            pRender->DrawPath(path.get(), pen.get());
        }
    }

    textRect = rect;
    textRect.top = rect.bottom;
    textRect.bottom = textRect.top + nTextLineHeight;

    drawParam.textRect = textRect;
    pRender->DrawString(_T("DrawPath"), drawParam);

    //Fill a rounded quadrilateral with FillPath
    rect.left = rect.right + marginLeft;
    rect.right = rect.left + nSize;
    if (pRenderFactory != nullptr) {
        std::unique_ptr<IPen> pen(pRenderFactory->CreatePen(UiColor(0xff006DD9), DpiScaledFloat(2)));
        std::unique_ptr<IBrush> brush(pRenderFactory->CreateBrush(UiColor(UiColors::Red)));
        std::unique_ptr<IPath> path(pRenderFactory->CreatePath());

        if (pen && brush && path) {
            const UiRect& rc = rect;

            path->AddArc(UiRect(rc.left, rc.top, rc.left + roundSize.cx, rc.top + roundSize.cy), 180, 90);
            path->AddLine(rc.left + roundSize.cx / 2, rc.top, rc.right - roundSize.cx / 2, rc.top);                      
            path->AddArc(UiRect(rc.right - roundSize.cx, rc.top, rc.right, rc.top + roundSize.cy), 270, 90);
            path->AddLine(rc.right, rc.top + roundSize.cy / 2, rc.right, rc.bottom - roundSize.cy / 2);
            path->AddArc(UiRect(rc.right - roundSize.cx, rc.bottom - roundSize.cy, rc.right, rc.bottom), 0, 90);
            path->AddLine(rc.right - roundSize.cx / 2, rc.bottom, rc.left + roundSize.cx / 2, rc.bottom);
            path->AddArc(UiRect(rc.left, rc.bottom - roundSize.cy, rc.left + roundSize.cx, rc.bottom), 90, 90);
            path->AddLine(rc.left, rc.bottom - roundSize.cy / 2, rc.left, rc.top + roundSize.cy / 2);
            path->Close();

            pRender->DrawPath(path.get(), pen.get());
            pRender->FillPath(path.get(), brush.get());
        }
    }

    textRect = rect;
    textRect.top = rect.bottom;
    textRect.bottom = textRect.top + nTextLineHeight;

    drawParam.textRect = textRect;
    pRender->DrawString(_T("FillPath"), drawParam);

    //DrawBoxShadow
    rect.left = rect.right + marginLeft * 2;
    rect.right = rect.left + nSize;
    int nBlurRadius = DpiScaledInt(6);
    int nSpreadRadius = DpiScaledInt(2);

    UiSize roundSize2(6, 6);
    Dpi().ScaleSize(roundSize2);
    roundSize2.cx = (roundSize2.cx / 2) * 2;
    roundSize2.cy = (roundSize2.cy / 2) * 2;
    pRender->DrawBoxShadow(rect, roundSize2, UiPoint(0, 0), nBlurRadius, nSpreadRadius, UiColor(0xffC63535), 255);

    textRect = rect;
    textRect.top = rect.bottom;
    textRect.bottom = textRect.top + nTextLineHeight;

    drawParam.textRect = textRect;
    drawParam.pFont = GetIFontById(_T("system_12"));
    pRender->DrawString(_T("DrawBoxShadow"), drawParam);

    //DrawBoxShadow
    rect.left = rect.right + marginLeft * 2 + 40;
    rect.right = rect.left + nSize;
    nBlurRadius = DpiScaledInt(6);
    nSpreadRadius = DpiScaledInt(4);
    pRender->DrawBoxShadow(rect, UiSize(0, 0), UiPoint(0, 0), nBlurRadius, nSpreadRadius, UiColor(0xffC63535), 255);

    textRect = rect;
    textRect.top = rect.bottom;
    textRect.bottom = textRect.top + nTextLineHeight;

    drawParam.textRect = textRect;
    pRender->DrawString(_T("DrawBoxShadow"), drawParam);

    //DrawBoxShadow
    rect.left = rect.right + marginLeft * 2 + 40;
    rect.right = rect.left + nSize;
    nBlurRadius = DpiScaledInt(4);
    nSpreadRadius = DpiScaledInt(4);
    UiPoint cpOffset;
    cpOffset.x = DpiScaledInt(2);
    cpOffset.y = DpiScaledInt(3);
    pRender->DrawBoxShadow(rect, UiSize(0, 0), cpOffset, nBlurRadius, nSpreadRadius, UiColor(0xffC63535), 255);

    textRect = rect;
    textRect.top = rect.bottom;
    textRect.bottom = textRect.top + nTextLineHeight;

    drawParam.textRect = textRect;
    pRender->DrawString(_T("DrawBoxShadow"), drawParam);
}

void RenderTest2::PaintChild(IRender* pRender, const UiRect& rcPaint)
{
    BaseClass::PaintChild(pRender, rcPaint);
}

int RenderTest2::DpiScaledInt(int iValue)
{
    return Dpi().GetScaleInt(iValue);
}

float RenderTest2::DpiScaledFloat(int32_t iValue)
{
    return Dpi().GetScaleFloat(iValue);
}

void RenderTest2::DrawColorMap(IRender* pRender, const UiRect& rect)
{
    int32_t radius = static_cast<int32_t>(rect.Width() / 13 / 2 / std::cos(30 / 57.2957795f)); //Radius
    const float distance = radius * std::cos(30 / 57.2957795f); //The perpendicular distance from the center to the side

    UiPointF firstCenterPt = UiPointF((float)rect.CenterX(), (float)rect.CenterY()); //Center point coordinates of the rectangle
    firstCenterPt.x = firstCenterPt.x - distance * 2 * 6 * std::sin(30 / 57.2957795f); //X coordinate of the center point of the first hexagon
    firstCenterPt.y = firstCenterPt.y - distance * 2 * 6 * std::cos(30 / 57.2957795f); //Y coordinate of the center point of the first hexagon

    for (int32_t y = 0; y < 13; ++y) { //13 rows in total
        int32_t count = 0;
        if (y < 7) {
            count = 7 + y;
        }
        else {
            count = 7 + (13 - y - 1);
        }
        for (int32_t x = 0; x < count; ++x) {
            UiPointF centerPt = firstCenterPt;
            if (y < 7) {
                centerPt.x += distance * 2 * x - distance * y;
            }
            else {
                centerPt.x += distance * 2 * x - distance * (13 - y - 1);
            }
            centerPt.y += radius * 1.5f * y;
            DrawRegularHexagon(pRender, centerPt, radius, UiColor(UiColors::Blue), 1, UiColor(UiColors::Salmon));
        }
    }
}

bool RenderTest2::DrawRegularHexagon(IRender* pRender, const UiPointF& centerPt, int32_t radius,
                                     const UiColor& penColor, int32_t penWidth, const UiColor& brushColor)
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
    ASSERT(radius > 0); //Radius of the polygon
    if (radius <= 0) {
        return false;
    }

    const int32_t count = 6; //Number of sides of the polygon
    //The coordinate of any vertex of a regular polygon is: x = r * cos(θ) y = r * sin(θ) 
    std::vector<UiPointF> polygonPoints;
    for (int32_t i = 0; i < count; ++i) {
        int32_t degree = i * 60 + 30;// +30 is to place the vertex directly above the center point
        float radian = degree / 57.2957795f;
        float x = radius * std::cos(radian) + 0.5f;
        float y = radius * std::sin(radian) + 0.5f;
        polygonPoints.push_back(UiPointF(centerPt.x + x, centerPt.y + y));
    }

    std::unique_ptr<IPath> path(pRenderFactory->CreatePath());
    path->AddPolygon(polygonPoints.data(), (int32_t)polygonPoints.size());
    path->Close();

    bool bRet = false;
    if (brushColor.GetARGB() != 0) {
        std::unique_ptr<IBrush> brush(pRenderFactory->CreateBrush(brushColor));
        pRender->FillPath(path.get(), brush.get());
        bRet = true;
    }
    if ((penColor.GetARGB() != 0) && (penWidth > 0)) {
        std::unique_ptr<IPen> pen(pRenderFactory->CreatePen(penColor, (float)penWidth));
        pRender->DrawPath(path.get(), pen.get());
        bRet = true;
    }
    return bRet;
}

bool RenderTest2::DrawRegularHexagon3(IRender* pRender, const UiPoint& centerPt, int32_t radius,
                                      const UiColor& penColor, int32_t penWidth, const UiColor& brushColor)
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

    bool bRet = false;
    const int count = 6;
    UiPoint oldWindowOrg = pRender->SetWindowOrg(UiPoint(centerPt.x, centerPt.y));

    //Start drawing the polygon and color each block
    for (int i = 0; i < count; ++i)
    {
        //Let the angle between the perpendicular from the center to the side and the radius be degree=(360/count)/2, i.e.:
        float degree = 180.0f / count;

        float radian = degree / 57.2957795f;
        int32_t width = static_cast<int32_t>(radius * std::sin(radian));
        int32_t height = static_cast<int32_t>(radius * std::cos(radian));

        std::unique_ptr<IPath> path(pRenderFactory->CreatePath());

        //Draw the triangle block
        path->AddLine(0, 0, -width, -height);
        path->AddLine(-width, -height, width, -height);
        path->AddLine(width, -height, 0, 0);

        std::unique_ptr<IMatrix> spMatrix(pRenderFactory->CreateMatrix());
        if (spMatrix != nullptr) {
            float angle = 2 * degree * i;
            spMatrix->RotateAt(angle, 0.0f, 0.0f);
            path->Transform(spMatrix.get());
        }        
        if (brushColor.GetARGB() != 0) {
            std::unique_ptr<IBrush> brush(pRenderFactory->CreateBrush(brushColor));
            pRender->FillPath(path.get(), brush.get());
            bRet = true;
        }
        if ((penColor.GetARGB() != 0) && (penWidth > 0)) {
            std::unique_ptr<IPen> pen(pRenderFactory->CreatePen(penColor, (float)penWidth));
            pRender->DrawPath(path.get(), pen.get());
            bRet = true;
        }
    }
    pRender->SetWindowOrg(oldWindowOrg);
    return bRet;
}

} //end of namespace ui
