#include "duilib/Control/ColorPicker.h"
#include "duilib/Box/TabBox.h"
#include "duilib/Control/ColorPickerRegular.h"
#include "duilib/Control/ColorPickerStatard.h"
#include "duilib/Control/ColorPickerStatardGray.h"
#include "duilib/Control/ColorPickerCustom.h"
#include "duilib/Core/GlobalManager.h"
#include "duilib/Core/WindowCreateParam.h"
#include "duilib/Utils/ScreenCapture.h"

namespace ui
{
/** Control for color preview
*/
class ColorPreviewLabel: public Label
{
    typedef Label BaseClass;
public:
    explicit ColorPreviewLabel(Window* pWindow): Label(pWindow)
    {
    }
    virtual ~ColorPreviewLabel() override = default;

    /** Draw the background color, adding a black-gray grid background for previewing transparent colors
    */
    virtual void PaintBkColor(IRender* pRender) override
    {
        if (pRender == nullptr) {
            return;
        }
        //Draw the transparent checkerboard grid
        UiRect rc = GetRect();
        const int32_t nGridSize = Dpi().GetScaleInt(8);
        int32_t nRows = rc.Width() / nGridSize + 1;
        int32_t nCols = rc.Height() / nGridSize + 1;
        for (int32_t i = 0; i < nRows; ++i) {
            for (int32_t j = 0; j < nCols; ++j) {
                UiRect rect;
                rect.left = rc.left + i * nGridSize;
                rect.top = rc.top + j * nGridSize;
                rect.right = rect.left + nGridSize;
                rect.bottom = rect.top + nGridSize;
                if (j % 2) {
                    pRender->FillRect(UiRectF::MakeFromRect(rect), (i % 2) == 1 ? UiColor(UiColors::DarkGray) : UiColor(UiColors::White));
                }
                else {
                    pRender->FillRect(UiRectF::MakeFromRect(rect), (i % 2) == 0 ? UiColor(UiColors::DarkGray) : UiColor(UiColors::White));
                }
            }
        }

        //Draw the actually displayed color
        BaseClass::PaintBkColor(pRender);
    }
};

ColorPicker::ColorPicker():
    m_pNewColor(nullptr),
    m_pOldColor(nullptr),
    m_pRegularPicker(nullptr),
    m_pStatardPicker(nullptr),
    m_pStatardGrayPicker(nullptr),
    m_pCustomPicker(nullptr)
{
}

ColorPicker::~ColorPicker()
{
}

DString ColorPicker::GetSkinFolder()
{
    return _T("public");
}

DString ColorPicker::GetSkinFile()
{
    return _T("color/color_picker.xml");
}

LRESULT ColorPicker::OnWindowCloseMsg(uint32_t wParam, const NativeMsg& nativeMsg, bool& bHandled)
{
    UiColor selectedColor;
    if (wParam == kWindowCloseOK) {
        //Only save the selected color when the "OK" button is clicked
        if (m_pNewColor != nullptr) {
            DString bkColor = m_pNewColor->GetBkColor();
            if (!bkColor.empty()) {
                selectedColor = m_pNewColor->GetUiColor(bkColor);
            }
        }
    }    
    m_selectedColor = selectedColor;
    return BaseClass::OnWindowCloseMsg(wParam, nativeMsg, bHandled);
}

void ColorPicker::AttachSelectColor(const EventCallback& callback)
{
    m_colorCallback = callback;
}

void ColorPicker::AttachWindowClose(const EventCallback& callback)
{
    BaseClass::AttachWindowCloseMsg(callback);
}

Control* ColorPicker::CreateControl(const DString& strClass)
{
    if (strClass == _T("ColorPreviewLabel")) {
        return new ColorPreviewLabel(this);
    }
    return nullptr;
}

void ColorPicker::OnInitWindow()
{
    m_pNewColor = dynamic_cast<Label*>(FindControl(_T("color_picker_new_color")));
    m_pOldColor = dynamic_cast<Label*>(FindControl(_T("color_picker_old_color")));

    ASSERT(m_pNewColor != nullptr);
    ASSERT(m_pOldColor != nullptr);

    m_pRegularPicker = dynamic_cast<ColorPickerRegular*>(FindControl(_T("color_picker_regular")));
    m_pStatardPicker = dynamic_cast<ColorPickerStatard*>(FindControl(_T("color_picker_standard")));
    m_pStatardGrayPicker = dynamic_cast<ColorPickerStatardGray*>(FindControl(_T("color_picker_standard_gray")));
    m_pCustomPicker = dynamic_cast<ColorPickerCustom*>(FindControl(_T("color_picker_custom")));

    if (m_pRegularPicker != nullptr) {
        m_pRegularPicker->AttachSelectColor([this](const ui::EventArgs& args) {
            UiColor newColor((uint32_t)args.wParam);
            OnSelectColor(newColor);
            return true;
            });
    }
    if (m_pStatardPicker != nullptr) {
        m_pStatardPicker->AttachSelectColor([this](const ui::EventArgs& args) {
            UiColor newColor((uint32_t)args.wParam);
            OnSelectColor(newColor);
            if (m_pStatardGrayPicker != nullptr) {
                m_pStatardGrayPicker->SelectColor(UiColor());
            }
            return true;
            });
    }
    if (m_pStatardGrayPicker != nullptr) {
        m_pStatardGrayPicker->AttachSelectColor([this](const ui::EventArgs& args) {
            UiColor newColor((uint32_t)args.wParam);
            OnSelectColor(newColor);
            if (m_pStatardPicker != nullptr) {
                m_pStatardPicker->SelectColor(UiColor());
            }
            return true;
            });
    }
    if (m_pCustomPicker != nullptr) {
        m_pCustomPicker->AttachSelectColor([this](const ui::EventArgs& args) {
            UiColor newColor((uint32_t)args.wParam);
            OnSelectColor(newColor);
            return true;
            });
    }

    TabBox* pTabBox = dynamic_cast<TabBox*>(FindControl(_T("color_picker_tab")));
    if (pTabBox != nullptr) {
        pTabBox->AttachTabSelect([this](const ui::EventArgs& args) {
            UiColor selectedColor;
            if (m_pNewColor != nullptr) {
                DString bkColor = m_pNewColor->GetBkColor();
                if (!bkColor.empty()) {
                    selectedColor = m_pNewColor->GetUiColor(bkColor);
                }                
            }
            if (args.wParam == 0) {
                //Regular colors
                if (m_pRegularPicker != nullptr) {
                    m_pRegularPicker->SelectColor(selectedColor);
                }                
            }
            else if (args.wParam == 1) {
                //Standard colors
                if (m_pStatardPicker != nullptr) {
                    m_pStatardPicker->SelectColor(selectedColor);
                }
                if (m_pStatardGrayPicker != nullptr) {
                    m_pStatardGrayPicker->SelectColor(selectedColor);
                }
            }
            else if (args.wParam == 2) {
                //Custom colors
                if (m_pCustomPicker != nullptr) {
                    m_pCustomPicker->SelectColor(selectedColor);
                }
            }
            return true;
            });
    }

    //OK button
    Button* pButton = dynamic_cast<Button*>(FindControl(_T("color_picker_ok")));
    if (pButton != nullptr) {
        pButton->AttachClick([this](const ui::EventArgs& /*args*/) {
            this->CloseWnd(kWindowCloseOK);
            return true;
            });
    }
    //Cancel button
    pButton = dynamic_cast<Button*>(FindControl(_T("color_picker_cancel")));
    if (pButton != nullptr) {
        pButton->AttachClick([this](const ui::EventArgs& /*args*/) {
            this->CloseWnd(kWindowCloseCancel);
            return true;
            });
    }

    //Selection: pick a color from the screen
    pButton = dynamic_cast<Button*>(FindControl(_T("color_picker_choose")));
    if (pButton != nullptr) {
        pButton->AttachClick([this](const ui::EventArgs& /*args*/) {
            OnPickColorFromScreen();
            return true;
            });
    }
}

void ColorPicker::OnSelectColor(const UiColor& newColor)
{
    UiColor oldColor;
    if (m_pNewColor != nullptr) {
        DString colorString = m_pNewColor->GetBkColor();
        if (!colorString.empty()) {
            oldColor = m_pNewColor->GetUiColor(colorString);
        }
        m_pNewColor->SetBkColor(newColor);
        m_pNewColor->SetText(m_pNewColor->GetBkColor());

        //Text color, use the inverted color
        UiColor textColor = UiColor(255 - newColor.GetR(), 255 - newColor.GetG(), 255 - newColor.GetB());
        m_pNewColor->SetStateTextColor(kControlStateNormal, m_pNewColor->GetColorString(textColor));
    }
    if (m_colorCallback != nullptr) {
        EventArgs args;
        args.wParam = newColor.GetARGB();
        args.lParam = oldColor.GetARGB();
        m_colorCallback(args);
    }
}

void ColorPicker::SetSelectedColor(const UiColor& color)
{
    if (m_pNewColor != nullptr) {
        m_pNewColor->SetBkColor(color);
        m_pNewColor->SetText(m_pNewColor->GetBkColor());

        //Text color, use the inverted color
        UiColor textColor = UiColor(255 - color.GetR(), 255 - color.GetG(), 255 - color.GetB());
        m_pNewColor->SetStateTextColor(kControlStateNormal, m_pNewColor->GetColorString(textColor));
    }
    if (m_pOldColor != nullptr) {
        m_pOldColor->SetBkColor(color);
        m_pOldColor->SetText(m_pOldColor->GetBkColor());

        //Text color, use the inverted color
        UiColor textColor = UiColor(255 - color.GetR(), 255 - color.GetG(), 255 - color.GetB());
        m_pOldColor->SetStateTextColor(kControlStateNormal, m_pOldColor->GetColorString(textColor));
    }
    if (m_pCustomPicker != nullptr) {
        m_pCustomPicker->SelectColor(color);
    }
    if (m_pRegularPicker != nullptr) {
        m_pRegularPicker->SelectColor(color);
    }
    if (m_pStatardPicker != nullptr) {
        m_pStatardPicker->SelectColor(color);
    }
    if (m_pStatardGrayPicker != nullptr) {
        m_pStatardGrayPicker->SelectColor(color);
    }
}

UiColor ColorPicker::GetSelectedColor() const
{
    return m_selectedColor;
}

/** Screen color picker preview control
*/
class ScreenColorPreview : public Label
{
    typedef Label BaseClass;
public:
    explicit ScreenColorPreview(Window* pWindow):
        Label(pWindow)
    {
    }
    /** The entry function for drawing the background image
    * @param[in] pRender Specifies the drawing area
    */
    virtual void PaintBkImage(IRender* pRender) override
    {
        BaseClass::PaintBkImage(pRender);
        if (pRender == nullptr) {
            return;
        }
        if (m_spBitmap == nullptr) {
            return;
        }
        if (m_spBitmap->GetWidth() < 1) {
            return;
        }
        UiRect rc = GetRect();
        UiRect rcPaint = GetPaintRect();
        IBitmap* pBitmap = m_spBitmap.get();
        if (pBitmap == nullptr) {
            return;
        }
        UiRect rcDest = rc;
        rcDest.bottom = rcDest.top + rcDest.Width() * pBitmap->GetHeight() / pBitmap->GetWidth(); //Keep the aspect ratio of the original image

        UiRect rcSource;
        rcSource.left = 0;
        rcSource.top = 0;
        rcSource.right = rcSource.left + pBitmap->GetWidth();
        rcSource.bottom = rcSource.top + pBitmap->GetHeight();

        uint8_t uFade = 255;
        IMatrix* pMatrix = nullptr;
        if (pBitmap != nullptr) {        
            pRender->DrawImageRect(rcPaint, pBitmap, rcDest, rcSource, uFade, pMatrix);
            if (GetTopBorderSize() > 0) {
                pRender->DrawRect(UiRectF::MakeFromRect(rcDest), GetUiColor(GetBorderColor(kControlStateNormal)), GetTopBorderSize());
            }
        }

        //Draw the circle at the center of the preview
        UiPoint centerPt = rcDest.Center();
        float fRadius = Dpi().GetScaleFloat(6);
        UiColor penColor = UiColor(UiColors::Brown);
        float fWidth = Dpi().GetScaleFloat(2);
        pRender->DrawCircle(UiPointF::MakeFromPoint(centerPt), fRadius, penColor, fWidth);
    }

    /** Get the size (width and height) of the preview bitmap capture
    */
    void GetPreviewBitmapSize(int32_t& nPreviewWidth, int32_t& nPreviewHeight) const
    {
        nPreviewWidth = GetWidth() / 16;
        if ((nPreviewWidth % 2) != 0) {
            nPreviewWidth += 1;
        }
        nPreviewHeight = nPreviewWidth / 2;//The aspect ratio is 2:1
        if ((nPreviewHeight % 2) != 0) {
            nPreviewHeight += 1;
        }
    }

    /** Set the preview bitmap
    */
    void SetPreviewBitmap(const std::shared_ptr<IBitmap>& spBitmap)
    {
        m_spBitmap = spBitmap;
        Invalidate();
    }

private:
    /** The screen color picker preview bitmap
    */
    std::shared_ptr<IBitmap> m_spBitmap;
};

/** Screen color picker control
*/
class ScreenColorPicker : public Control
{
    typedef Control BaseClass;
public:
    explicit ScreenColorPicker(Window* pWindow):
        Control(pWindow),
        m_cursorId(0),
        m_pColorPreview(nullptr)
    {
    }

    virtual ~ScreenColorPicker() override
    {
        m_pColorPreview = nullptr;
        m_selColor = UiColor();
    }

    /** Set the specified attribute of the control
     */
    virtual void SetAttribute(const DString& strName, const DString& strValue) override
    {
        if (strName == _T("cursor_file")) {
            m_cursorFile = strValue;
        }
        else {
            BaseClass::SetAttribute(strName, strValue);
        }
    }

    /** Set the screen bitmap
    */
    void SetBitmap(const std::shared_ptr<IBitmap>& spBitmap)
    {
        m_spBitmap = spBitmap;
    }

    /** Set the interface of the preview control
    */
    void SetColorPreview(ScreenColorPreview* pColorPreview)
    {
        m_pColorPreview = pColorPreview;
    }

    /** Get the selected color value
    */
    UiColor GetSelColor() const 
    {
        return m_selColor;
    }

private:
    /** The entry function for drawing the background image
    * @param[in] pRender Specifies the drawing area
    */
    virtual void PaintBkImage(IRender* pRender) override
    {
        BaseClass::PaintBkImage(pRender);
        if (pRender == nullptr) {
            return;
        }
        if (m_spBitmap == nullptr) {
            return;
        }
        UiRect rc = GetRect();
        UiRect rcPaint = GetPaintRect();
        IBitmap* pBitmap = m_spBitmap.get();
        UiRect rcDest = rc;
        UiRect rcSource;
        rcSource.left = 0;
        rcSource.top = 0;
        rcSource.right = rcSource.left + rc.Width();
        rcSource.bottom = rcSource.top + rc.Height();
        uint8_t uFade = 255;
        IMatrix* pMatrix = nullptr;

        if (pBitmap != nullptr) {
            pRender->DrawImageRect(rcPaint, pBitmap, rcDest, rcSource, uFade, pMatrix);
        }
    }

    /** Set the cursor shape
    */
    virtual bool OnSetCursor(const EventArgs& /*msg*/) override
    {
        if (m_cursorId != 0) {
            GlobalManager::Instance().Cursor().SetCursorByID(m_cursorId);
        }
        else if (!m_cursorFile.empty()) {
            if (GlobalManager::Instance().Cursor().SetImageCursor(GetWindow(), FilePath(m_cursorFile.c_str()))) {
                m_cursorId = GlobalManager::Instance().Cursor().GetCursorID();
            }
        }
        
        return true;
    }

    /** Mouse left button down, select a color
    */
    virtual bool ButtonDown(const EventArgs& msg) override
    {
        bool bRet = BaseClass::ButtonDown(msg);
        if (msg.IsSenderExpired()) {
            return false;
        }

        //Update the selected color
        m_selColor = GetMousePosColor(msg.ptMouse);

        Window* pWindow = GetWindow();
        if (pWindow != nullptr) {
            pWindow->CloseWnd();
        }
        return bRet;
    }

    /** Mouse move; update the color at the cursor position to the preview control
    */
    virtual bool MouseMove(const EventArgs& msg) override
    {
        if (m_pColorPreview == nullptr) {
            return true;
        }
        const int32_t offset = Dpi().GetScaleInt(36);
        const UiRect rcPickker = GetRect();
        const UiRect rcPreview = m_pColorPreview->GetRect();
        UiRect rcPreviewNew;
        if ((msg.ptMouse.x + offset + rcPreview.Width()) > rcPickker.right) {
            //Display on the left
            rcPreviewNew.left = msg.ptMouse.x - offset - rcPreview.Width();
        }
        else {
            //Display on the right
            rcPreviewNew.left = msg.ptMouse.x + offset;                
        }
        rcPreviewNew.right = rcPreviewNew.left + rcPreview.Width();

        if ((msg.ptMouse.y + offset + rcPreview.Height()) > rcPickker.bottom) {
            //Display on the top
            rcPreviewNew.top = msg.ptMouse.y - offset - rcPreview.Height();
        }
        else {
            //Display on the bottom
            rcPreviewNew.top = msg.ptMouse.y + offset;
        }
        rcPreviewNew.bottom = rcPreviewNew.top + rcPreview.Height();
        m_pColorPreview->SetPos(rcPreviewNew);

        //Update the preview bitmap
        std::shared_ptr<IBitmap> spBitmap = GetMousePosBitmap(msg.ptMouse);
        m_pColorPreview->SetPreviewBitmap(spBitmap);

        //Update the preview color
        UiColor selColor = GetMousePosColor(msg.ptMouse);
        if (!selColor.IsEmpty()) {
            //Set the background color
            m_pColorPreview->SetBkColor(selColor);

            //Set the text
            DString text = m_pColorPreview->GetColorString(selColor);
            m_pColorPreview->SetText(text);
            m_pColorPreview->SetAttribute(_T("text_align"), _T("hcenter,vcenter"));
            m_pColorPreview->SetTextPadding(UiPadding(0, m_pColorPreview->GetHeight() / 2, 0, 0), false);
            //Set the text color
            UiColor textColor = UiColor(255 - selColor.GetR(), 255 - selColor.GetG(), 255 - selColor.GetB());
            m_pColorPreview->SetStateTextColor(kControlStateNormal, m_pColorPreview->GetColorString(textColor));
        }
        return true;
    }

    /** Get the color value at the mouse position
    */
    UiColor GetMousePosColor(const UiPoint& pt) const
    {
        UiColor selColor;
        const UiRect rcPickker = GetRect();
        if (m_spBitmap != nullptr) {
            uint32_t* pPixelBits = (uint32_t*)m_spBitmap->LockPixelBits();
            if (pPixelBits != nullptr) {
                const int32_t nWidth = (int32_t)m_spBitmap->GetWidth();
                const int32_t nHeight = (int32_t)m_spBitmap->GetHeight();
                int32_t nColumn = pt.x - rcPickker.left;
                int32_t nRow = pt.y - rcPickker.top;
                if (nColumn >= nWidth) {
                    nColumn = nWidth - 1;
                }
                if (nColumn < 0) {
                    nColumn = 0;
                }
                if (nRow >= nHeight) {
                    nRow = nHeight - 1;
                }
                if (nRow < 0) {
                    nRow = 0;
                }
                int32_t colorXY = nRow * nWidth + nColumn;
                ASSERT(colorXY < nWidth * nHeight);
                uint32_t colorValue = pPixelBits[colorXY];
                selColor = UiColor(colorValue);
#ifdef DUILIB_BUILD_FOR_WIN
                //SDL_PIXELFORMAT_BGRA32
                selColor = UiColor(selColor.GetR(), selColor.GetG(), selColor.GetB());
#else
                //SDL_PIXELFORMAT_RGBA32
                selColor = UiColor(selColor.GetB(), selColor.GetG(), selColor.GetR());
#endif
            }
            m_spBitmap->UnLockPixelBits();
        }
        return selColor;
    }

    /** Get the bitmap around the mouse position
    */
    std::shared_ptr<IBitmap> GetMousePosBitmap(const UiPoint& pt) const
    {
        std::shared_ptr<IBitmap> spBitmap;
        if (m_pColorPreview == nullptr) {
            return spBitmap;
        }
        int32_t nPreviewWidth = 0;
        int32_t nPreviewHeight = 0;
        m_pColorPreview->GetPreviewBitmapSize(nPreviewWidth, nPreviewHeight);
        if ((nPreviewWidth <= 0) || (nPreviewHeight <= 0)) {
            return spBitmap;
        }

        IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
        ASSERT(pRenderFactory != nullptr);
        if (pRenderFactory != nullptr) {
            spBitmap.reset(pRenderFactory->CreateBitmap());
        }
        if (spBitmap == nullptr) {
            return spBitmap;
        }
        if (!spBitmap->Init(nPreviewWidth, nPreviewHeight, nullptr)) {
            return nullptr;
        }
        uint32_t* pDestPixelBits = (uint32_t*)spBitmap->LockPixelBits();
        if (pDestPixelBits == nullptr) {
            return nullptr;
        }

        int32_t destColorIndex = 0;
        const UiRect rcPickker = GetRect();
        if (m_spBitmap != nullptr) {
            uint32_t* pPixelBits = (uint32_t*)m_spBitmap->LockPixelBits();
            if (pPixelBits != nullptr) {
                const int32_t nWidth = (int32_t)m_spBitmap->GetWidth();
                const int32_t nHeight = (int32_t)m_spBitmap->GetHeight();
                for (int32_t y = pt.y - nPreviewHeight / 2; y < (pt.y + nPreviewHeight / 2); ++y) {
                    for (int32_t x = pt.x - nPreviewWidth / 2; x < (pt.x + nPreviewWidth / 2); ++x ) {                    
                        int32_t nColumn = x - rcPickker.left;
                        int32_t nRow = y - rcPickker.top;
                        if (nColumn >= nWidth) {
                            nColumn = nWidth - 1;
                        }
                        if (nColumn < 0) {
                            nColumn = 0;
                        }
                        if (nRow >= nHeight) {
                            nRow = nHeight - 1;
                        }
                        if (nRow < 0) {
                            nRow = 0;
                        }
                        int32_t colorXY = nRow * nWidth + nColumn;
                        ASSERT(colorXY < nWidth * nHeight);
                        ASSERT(destColorIndex < (int32_t)spBitmap->GetWidth() * (int32_t)spBitmap->GetHeight());
                        pDestPixelBits[destColorIndex++] = pPixelBits[colorXY];
                    }
                }
            }
            ASSERT(destColorIndex == (int32_t)(spBitmap->GetWidth() * spBitmap->GetHeight()));
            m_spBitmap->UnLockPixelBits();
        }
        spBitmap->UnLockPixelBits();
        return spBitmap;
    }

private:
    /** The cursor ID
    */
    CursorID m_cursorId;

    /** The file name of the cursor
    */
    UiString m_cursorFile;

    /** The screen bitmap
    */
    std::shared_ptr<IBitmap> m_spBitmap;

    /** The interface of the preview control
    */
    ScreenColorPreview* m_pColorPreview;

    /** The selected color
    */
    UiColor m_selColor;
};

/** Window for picking a color from the screen (fullscreen)
*/
class ScreenColorPickerWnd : public WindowImplBase
{
    typedef WindowImplBase BaseClass;
public:
    ScreenColorPickerWnd(): m_pScreenColorPicker(nullptr)
    {
    }

    /** The following three interfaces must be overridden; the parent class calls these interfaces to build the window
     * GetSkinFolder        The interface sets the window skin resource path to be drawn
     * GetSkinFile            The interface sets the xml description file of the window to be drawn
     */
    virtual DString GetSkinFolder() override { return _T("public");}
    virtual DString GetSkinFile() override { return _T("color/screen_color_picker.xml"); }

    /** Called when the control to be created is not a standard control name
    * @param [in] strClass The control name
    * @return Returns a custom control pointer; in general, create a custom control according to the strClass parameter
    */
    virtual Control* CreateControl(const DString& strClass) override
    {
        if (strClass == _T("ScreenColorPicker")) {
            if (m_pScreenColorPicker == nullptr) {
                m_pScreenColorPicker = new ScreenColorPicker(this);
                if (m_spBitmap != nullptr) {
                    m_pScreenColorPicker->SetBitmap(m_spBitmap);
                    m_spBitmap.reset();
                }
            }
            return m_pScreenColorPicker;
        }
        else if (strClass == _T("ScreenColorPreview")) {
            ScreenColorPreview* pScreenColorPreview = new ScreenColorPreview(this);
            if (m_pScreenColorPicker != nullptr) {
                m_pScreenColorPicker->SetColorPreview(pScreenColorPreview);
            }
            return pScreenColorPreview;
        }
        return nullptr;
    }

    /** Exit the fullscreen state
    */
    virtual void OnWindowExitFullscreen() override
    {
        BaseClass::OnWindowExitFullscreen();
        CloseWnd();
    }

    /** Capture the screen bitmap
    */
    bool ScreenCapture(const Window* pWindow)
    {
        m_spBitmap = ScreenCapture::CaptureBitmap(pWindow);
        return m_spBitmap != nullptr;
    }

    /** Get the selected color value
    */
    UiColor GetSelColor() const
    {
        UiColor selColor;
        if (m_pScreenColorPicker != nullptr) {
            selColor = m_pScreenColorPicker->GetSelColor();
        }
        return selColor;
    }
private:
    /** The bitmap display control
    */
    ScreenColorPicker* m_pScreenColorPicker;

    /** The screen bitmap
    */
    std::shared_ptr<IBitmap> m_spBitmap;
};

void ColorPicker::OnPickColorFromScreen()
{
    bool bHideWindow = true;
    CheckBox* pCheckBox = dynamic_cast<CheckBox*>(FindControl(_T("color_picker_choose_hide")));
    if (pCheckBox != nullptr) {
        bHideWindow = pCheckBox->IsSelected();
    }
#ifdef DUILIB_BUILD_FOR_SDL
    bHideWindow = false;
#else
    //In the SDL implementation, if the window is hidden, all child windows are hidden too, so this window cannot be hidden
    if (bHideWindow) {
        //Hide this window
        ShowWindow(kSW_HIDE);

        //The parent window is not hidden
        Window* pParentWnd = GetParentWindow();
        if (pParentWnd != nullptr) {
            pParentWnd->ShowWindow(kSW_SHOW_NORMAL);
            pParentWnd->SetWindowForeground();
            pParentWnd->UpdateWindow();
        }
        UpdateWindow();
    }
#endif

    //Capture the screen bitmap
    ScreenColorPickerWnd* pScreenColorPicker = new ScreenColorPickerWnd;    
    if (!pScreenColorPicker->ScreenCapture(this)) {
        delete pScreenColorPicker;
        return;
    }
    WindowCreateParam createWndParam;
    createWndParam.m_dwStyle = kWS_POPUP;
    createWndParam.m_dwExStyle = kWS_EX_TRANSPARENT;
    createWndParam.m_bCenterWindow = true;
    pScreenColorPicker->CreateWnd(nullptr, createWndParam);
    pScreenColorPicker->ShowWindow(ui::kSW_SHOW_NORMAL);
    pScreenColorPicker->EnterFullscreen();
    pScreenColorPicker->AttachWindowCloseMsg([this, pScreenColorPicker, bHideWindow](const ui::EventArgs& /*args*/) {
        //Update the selected color value
        UiColor selectedColor = pScreenColorPicker->GetSelColor();
        if (!selectedColor.IsEmpty()) {
            //Update the selected color
            this->OnSelectColor(selectedColor);
            //Update the regular colors
            if (m_pRegularPicker != nullptr) {
                m_pRegularPicker->SelectColor(selectedColor);
            }
            //Update the standard colors
            if (m_pStatardPicker != nullptr) {
                m_pStatardPicker->SelectColor(selectedColor);
            }
            if (m_pStatardGrayPicker != nullptr) {
                m_pStatardGrayPicker->SelectColor(selectedColor);
            }
            //Update the custom colors
            if (m_pCustomPicker != nullptr) {
                m_pCustomPicker->SelectColor(selectedColor);
            }
        }
        if (bHideWindow) {
            //Show the main window
            this->ShowWindow(ui::kSW_SHOW_NORMAL);
            InvalidateAll();
            UpdateWindow();
            
            //Redraw the parent window
            Window* pParentWnd = GetParentWindow();
            if (pParentWnd != nullptr) {
                pParentWnd->InvalidateAll();
                pParentWnd->UpdateWindow();
            }
        }
        return true;
        });
}

} // namespace ui
