#include "MainForm.h"
#include "dui/Utils/UiBuilder.h"

MainForm::MainForm()
{
}

MainForm::~MainForm()
{
}

DString MainForm::GetSkinFolder()
{
    return _T("");
}

DString MainForm::GetSkinFile()
{
    // Pure code mode: no layout XML is loaded
    return _T("");
}

void MainForm::GetCreateWindowAttributes(ui::WindowCreateAttributes& attrs)
{
    // Window attributes correspond to the <Window> attributes in basic.xml
    using namespace ui;
    attrs.m_bInitSizeDefined = true;
    attrs.m_szInitSize.cx = 1000;
    attrs.m_szInitSize.cy = 600;
    attrs.m_bShadowAttached = true;
    attrs.m_bShadowAttachedDefined = true;
    attrs.m_bIsLayeredWindow = true;
    attrs.m_bIsLayeredWindowDefined = true;
    attrs.m_rcSizeBox = UiRect(4, 4, 4, 4);
    attrs.m_bSizeBoxDefined = true;
    attrs.m_rcCaption = UiRect(0, 0, 0, 36);
    attrs.m_bCaptionDefined = true;
    BaseClass::GetCreateWindowAttributes(attrs);
}

void MainForm::OnInitWindow()
{
    using namespace ui;
    SetSizeBox(UiRect(4, 4, 4, 4), false);
    SetCaptionRect(UiRect(0, 0, 0, 36), false);

    // The whole window tree is built with one nested ui::Make expression:
    // it mirrors the XML layout and reads much closer to Qt/gtkmm code.
    auto* root = Create<VBox>(this, {{"bkcolor", "bk_wnd_darkcolor"}},

        Create<HBox>(this,
            {{"name", "window_caption_bar"},
             {"width", "stretch"},
             {"height", "36"},
             {"bkcolor", "bk_wnd_lightcolor"}},

            Create<Control>(this, {{"mouse_enabled", "false"}}),

            Create<Button>(this,
                {{"class", "btn_wnd_fullscreen_11"}, {"height", "32"},
                 {"width", "40"}, {"name", "fullscreenbtn"},
                 {"margin", "0,2,0,2"},
                 {"tooltip_text", "Fullscreen, press ESC to exit fullscreen"}}),

            Create<Button>(this,
                {{"class", "btn_wnd_min_11"}, {"height", "32"},
                 {"width", "40"}, {"name", "minbtn"},
                 {"margin", "0,2,0,2"}, {"tooltip_text", "Minimize"}}),

            Create<Box>(this,
                {{"height", "stretch"}, {"width", "40"}, {"margin", "0,2,0,2"}},

                Create<Button>(this,
                    {{"class", "btn_wnd_max_11"}, {"height", "32"},
                     {"width", "stretch"}, {"name", "maxbtn"},
                     {"tooltip_text", "Maximize"}}),

                Create<Button>(this,
                    {{"class", "btn_wnd_restore_11"}, {"height", "32"},
                     {"width", "stretch"}, {"name", "restorebtn"},
                     {"visible", "false"}, {"tooltip_text", "Restore"}})),

            Create<Button>(this,
                {{"class", "btn_wnd_close_11"}, {"height", "stretch"},
                 {"width", "40"}, {"name", "closebtn"},
                 {"margin", "0,0,0,2"}, {"tooltip_text", "Close"}})),

        Create<Box>(this, {},

            Create<VBox>(this,
                {{"valign", "center"}, {"halign", "center"}},

                Create<Label>(this,
                    {{"name", "tooltip"},
                     {"text", "A simple window with a title bar and standard buttons."},
                     {"height", "100%"}, {"width", "100%"},
                     {"text_align", "hcenter,vcenter"}}))));

    AttachBox(root);
    BaseClass::OnInitWindow();
}
