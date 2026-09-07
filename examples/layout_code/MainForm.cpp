#include "MainForm.h"
// The layout is built directly in C++ for the pure-code example.

#include "dui/dui.h"
#include "dui/Utils/UiBuilder.h"

void MainForm::BuildUI() {

    this->AddClass(DUI_T("page_box"), DUI_T(" margin=\"6,6,6,6\" width=\"200\" height=\"100\" bkcolor=\"Sienna\" border_size=\"1\" border_color=\"green\""));

    this->AddClass(DUI_T("page_hbox"), DUI_T(" margin=\"6,6,6,6\" width=\"110\" height=\"auto\" multi_line=\"true\" bkcolor=\"Sienna\" border_size=\"1\" border_color=\"green\""));

    this->AddClass(DUI_T("page_vbox"), DUI_T(" margin=\"6,6,6,6\" width=\"auto\" height=\"40\" multi_line=\"true\" bkcolor=\"Sienna\" border_size=\"1\" border_color=\"green\""));

    this->AddClass(DUI_T("page_hflowbox"), DUI_T(" multi_line=\"true\" margin=\"6,6,6,6\" width=\"100\" height=\"60\" bkcolor=\"Sienna\" border_size=\"1\" border_color=\"green\""));

    this->AddClass(DUI_T("page_vflowbox"), DUI_T(" multi_line=\"true\" margin=\"6,6,6,6\" width=\"80\" height=\"60\" bkcolor=\"Sienna\" border_size=\"1\" border_color=\"green\""));

    this->AddClass(DUI_T("page_tile_box"), DUI_T(" margin=\"10,10,10,10\" width=\"200\" height=\"100\" text_padding=\"4,0,0,0\" bkcolor=\"Sienna\" border_size=\"1\" border_color=\"green\""));

    this->AddClass(DUI_T("page_scroll_box"), DUI_T(" text_padding=\"4,4,4,4\" halign=\"center\" valign=\"center\" bkcolor=\"Sienna\" border_size=\"2\" border_color=\"Blue\""));

    this->AddClass(DUI_T("page_hscroll_box"), DUI_T(" text_padding=\"4,4,4,4\" halign=\"center\" valign=\"center\" bkcolor=\"Sienna\" border_size=\"2\" border_color=\"Blue\""));

    this->AddClass(DUI_T("page_vscroll_box"), DUI_T(" text_padding=\"4,4,4,4\" halign=\"center\" valign=\"center\" bkcolor=\"Sienna\" border_size=\"2\" border_color=\"Blue\""));

    this->AddClass(DUI_T("page_hflow_scrollbox"), DUI_T(" margin=\"6,6,6,6\" width=\"100\" height=\"80\" bkcolor=\"Sienna\" border_size=\"1\" border_color=\"green\""));

    this->AddClass(DUI_T("page_vflow_scrollbox"), DUI_T(" margin=\"6,6,6,6\" width=\"100\" height=\"80\" bkcolor=\"Sienna\" border_size=\"1\" border_color=\"green\""));

    this->AddClass(DUI_T("page_tile_scroll_box"), DUI_T(" width=\"400\" height=\"160\" text_padding=\"4,4,4,4\" halign=\"center\" valign=\"center\" bkcolor=\"Sienna\" border_size=\"2\" border_color=\"Blue\""));

    this->AddClass(DUI_T("page_list_box_item"), DUI_T(" margin=\"8,2,8,2\" text_padding=\"20,6,10,6\" hot_color=\"bk_listitem_hovered\" pushed_color=\"bk_listitem_selected\" selected_normal_color=\"bk_listitem_selected\" fade_hot=\"false\" width=\"auto\" height=\"auto\" font=\"system_14\" normal_image=\"file='public/CheckBox/checkbox-outline-unchecked.svg' valign='center'\" disabled_image=\"file='public/CheckBox/checkbox-outline-unchecked.svg' valign='center' fade='80'\" selected_normal_image=\"file='public/CheckBox/checkbox-outline-checked.svg' valign='center'\" selected_disabled_image=\"file='public/CheckBox/checkbox-outline-checked.svg' valign='center' fade='80'\""));
    this->AddClass(DUI_T("page_list_box"), DUI_T(" margin=\"4,4,4,4\" padding=\"0,4,0,4\" bkcolor=\"bk_wnd_lightcolor\" vscrollbar=\"true\" hscrollbar=\"true\" scroll_select=\"false\" border_size=\"1\" border_color=\"red\""));

    this->AddClass(DUI_T("page_gridbox_calculator"), DUI_T(" multi_line=\"false\" text_align=\"center,hcenter\" font=\"system_16\" margin=\"0,0,0,0\" width=\"100%\" height=\"100%\" halign=\"center\" valign=\"center\" bkcolor=\"Sienna\" border_size=\"1\" border_color=\"green\""));
    this->AddClass(DUI_T("page_gridbox"), DUI_T(" multi_line=\"true\" margin=\"0,0,0,0\" width=\"90%\" height=\"90%\" bkcolor=\"Sienna\" border_size=\"1\" border_color=\"green\""));

    this->AddClass(DUI_T("page_grid_scrollbox"), DUI_T(" multi_line=\"true\" margin=\"0,0,0,0\" width=\"90%\" height=\"90%\" bkcolor=\"Sienna\" border_size=\"1\" border_color=\"green\""));
    auto* root = ui::Create<ui::VBox>(this, {{DUI_T("bkcolor"), DUI_T("bk_wnd_darkcolor")}});
    auto* hbox1 = ui::Create<ui::HBox>(this, {{DUI_T("name"), DUI_T("window_caption_bar")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("36")}, {DUI_T("bkcolor"), DUI_T("bk_wnd_lightcolor")}});
    auto* hbox2 = ui::Create<ui::HBox>(this, {{DUI_T("margin"), DUI_T("0,0,30,0")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("width"), DUI_T("auto")}, {DUI_T("height"), DUI_T("auto")}, {DUI_T("mouse_enabled"), DUI_T("false")}});
    auto* control1 = ui::Create<ui::Control>(this, {{DUI_T("width"), DUI_T("18")}, {DUI_T("height"), DUI_T("18")}, {DUI_T("bkimage"), DUI_T("public/caption/logo.svg")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}});
    ui::Attach(hbox2, control1);

    auto* label1 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Container Layout Test")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("mouse_enabled"), DUI_T("false")}});
    ui::Attach(hbox2, label1);

    ui::Attach(hbox1, hbox2);

    auto* control2 = ui::Create<ui::Control>(this, {{DUI_T("mouse_enabled"), DUI_T("false")}});
    ui::Attach(hbox1, control2);

    auto* hbox3 = ui::Create<ui::HBox>(this, {{DUI_T("margin"), DUI_T("0,0,0,0")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("width"), DUI_T("auto")}, {DUI_T("height"), DUI_T("36")}});
    auto* button1 = ui::Create<ui::Button>(this, {{DUI_T("class"), DUI_T("btn_wnd_min_11")}, {DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("name"), DUI_T("minbtn")}, {DUI_T("margin"), DUI_T("0,2,0,2")}, {DUI_T("tooltip_text"), DUI_T("Minimize")}});
    ui::Attach(hbox3, button1);

    auto* box1 = ui::Create<ui::Box>(this, {{DUI_T("height"), DUI_T("stretch")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("margin"), DUI_T("0,2,0,2")}});
    auto* button2 = ui::Create<ui::Button>(this, {{DUI_T("class"), DUI_T("btn_wnd_max_11")}, {DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("name"), DUI_T("maxbtn")}, {DUI_T("tooltip_text"), DUI_T("Maximize")}});
    ui::Attach(box1, button2);

    auto* button3 = ui::Create<ui::Button>(this, {{DUI_T("class"), DUI_T("btn_wnd_restore_11")}, {DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("name"), DUI_T("restorebtn")}, {DUI_T("visible"), DUI_T("false")}, {DUI_T("tooltip_text"), DUI_T("Restore")}});
    ui::Attach(box1, button3);

    ui::Attach(hbox3, box1);

    auto* button4 = ui::Create<ui::Button>(this, {{DUI_T("class"), DUI_T("btn_wnd_close_11")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("name"), DUI_T("closebtn")}, {DUI_T("margin"), DUI_T("0,0,0,2")}, {DUI_T("tooltip_text"), DUI_T("Close")}});
    ui::Attach(hbox3, button4);

    ui::Attach(hbox1, hbox3);

    ui::Attach(root, hbox1);

    auto* vbox1 = ui::Create<ui::VBox>(this, {});
    auto* tab_ctrl1 = ui::Create<ui::TabCtrl>(this, {{DUI_T("class"), DUI_T("tab_ctrl")}, {DUI_T("tab_box_name"), DUI_T("main_view_tab")}, {DUI_T("selected_id"), DUI_T("0")}});
    auto* tab_ctrl_item1 = ui::Create<ui::TabCtrlItem>(this, {{DUI_T("class"), DUI_T("tab_ctrl_item")}, {DUI_T("title"), DUI_T("Box")}, {DUI_T("tooltip_text"), DUI_T("Box")}, {DUI_T("tab_box_item_index"), DUI_T("0")}});
    ui::Attach(tab_ctrl1, tab_ctrl_item1);

    auto* tab_ctrl_item2 = ui::Create<ui::TabCtrlItem>(this, {{DUI_T("class"), DUI_T("tab_ctrl_item")}, {DUI_T("title"), DUI_T("HBox")}, {DUI_T("tooltip_text"), DUI_T("HBox")}, {DUI_T("tab_box_item_index"), DUI_T("1")}});
    ui::Attach(tab_ctrl1, tab_ctrl_item2);

    auto* tab_ctrl_item3 = ui::Create<ui::TabCtrlItem>(this, {{DUI_T("class"), DUI_T("tab_ctrl_item")}, {DUI_T("title"), DUI_T("VBox")}, {DUI_T("tooltip_text"), DUI_T("VBox")}, {DUI_T("tab_box_item_index"), DUI_T("2")}});
    ui::Attach(tab_ctrl1, tab_ctrl_item3);

    auto* tab_ctrl_item4 = ui::Create<ui::TabCtrlItem>(this, {{DUI_T("class"), DUI_T("tab_ctrl_item")}, {DUI_T("title"), DUI_T("HFlowBox")}, {DUI_T("tooltip_text"), DUI_T("HFlowBox")}, {DUI_T("tab_box_item_index"), DUI_T("3")}});
    ui::Attach(tab_ctrl1, tab_ctrl_item4);

    auto* tab_ctrl_item5 = ui::Create<ui::TabCtrlItem>(this, {{DUI_T("class"), DUI_T("tab_ctrl_item")}, {DUI_T("title"), DUI_T("VFlowBox")}, {DUI_T("tooltip_text"), DUI_T("VFlowBox")}, {DUI_T("tab_box_item_index"), DUI_T("4")}});
    ui::Attach(tab_ctrl1, tab_ctrl_item5);

    auto* tab_ctrl_item6 = ui::Create<ui::TabCtrlItem>(this, {{DUI_T("class"), DUI_T("tab_ctrl_item")}, {DUI_T("title"), DUI_T("TileBox")}, {DUI_T("tooltip_text"), DUI_T("TileBox")}, {DUI_T("tab_box_item_index"), DUI_T("5")}});
    ui::Attach(tab_ctrl1, tab_ctrl_item6);

    auto* tab_ctrl_item7 = ui::Create<ui::TabCtrlItem>(this, {{DUI_T("class"), DUI_T("tab_ctrl_item")}, {DUI_T("title"), DUI_T("ScrollBox")}, {DUI_T("tooltip_text"), DUI_T("ScrollBox")}, {DUI_T("tab_box_item_index"), DUI_T("6")}});
    ui::Attach(tab_ctrl1, tab_ctrl_item7);

    auto* tab_ctrl_item8 = ui::Create<ui::TabCtrlItem>(this, {{DUI_T("class"), DUI_T("tab_ctrl_item")}, {DUI_T("title"), DUI_T("HScrollBox")}, {DUI_T("tooltip_text"), DUI_T("HScrollBox")}, {DUI_T("tab_box_item_index"), DUI_T("7")}});
    ui::Attach(tab_ctrl1, tab_ctrl_item8);

    auto* tab_ctrl_item9 = ui::Create<ui::TabCtrlItem>(this, {{DUI_T("class"), DUI_T("tab_ctrl_item")}, {DUI_T("title"), DUI_T("VScrollBox")}, {DUI_T("tooltip_text"), DUI_T("VScrollBox")}, {DUI_T("tab_box_item_index"), DUI_T("8")}});
    ui::Attach(tab_ctrl1, tab_ctrl_item9);

    auto* tab_ctrl_item10 = ui::Create<ui::TabCtrlItem>(this, {{DUI_T("class"), DUI_T("tab_ctrl_item")}, {DUI_T("title"), DUI_T("HFlowScrollBox")}, {DUI_T("tooltip_text"), DUI_T("HFlowScrollBox")}, {DUI_T("tab_box_item_index"), DUI_T("9")}});
    ui::Attach(tab_ctrl1, tab_ctrl_item10);

    auto* tab_ctrl_item11 = ui::Create<ui::TabCtrlItem>(this, {{DUI_T("class"), DUI_T("tab_ctrl_item")}, {DUI_T("title"), DUI_T("VFlowScrollBox")}, {DUI_T("tooltip_text"), DUI_T("VFlowScrollBox")}, {DUI_T("tab_box_item_index"), DUI_T("10")}});
    ui::Attach(tab_ctrl1, tab_ctrl_item11);

    auto* tab_ctrl_item12 = ui::Create<ui::TabCtrlItem>(this, {{DUI_T("class"), DUI_T("tab_ctrl_item")}, {DUI_T("title"), DUI_T("TileScrollBox")}, {DUI_T("tooltip_text"), DUI_T("TileScrollBox")}, {DUI_T("tab_box_item_index"), DUI_T("11")}});
    ui::Attach(tab_ctrl1, tab_ctrl_item12);

    auto* tab_ctrl_item13 = ui::Create<ui::TabCtrlItem>(this, {{DUI_T("class"), DUI_T("tab_ctrl_item")}, {DUI_T("title"), DUI_T("ListBox")}, {DUI_T("tooltip_text"), DUI_T("ListBox")}, {DUI_T("tab_box_item_index"), DUI_T("12")}});
    ui::Attach(tab_ctrl1, tab_ctrl_item13);

    auto* tab_ctrl_item14 = ui::Create<ui::TabCtrlItem>(this, {{DUI_T("class"), DUI_T("tab_ctrl_item")}, {DUI_T("title"), DUI_T("Layout")}, {DUI_T("tooltip_text"), DUI_T("Layout")}, {DUI_T("tab_box_item_index"), DUI_T("13")}});
    ui::Attach(tab_ctrl1, tab_ctrl_item14);

    auto* tab_ctrl_item15 = ui::Create<ui::TabCtrlItem>(this, {{DUI_T("class"), DUI_T("tab_ctrl_item")}, {DUI_T("title"), DUI_T("TabBox")}, {DUI_T("tooltip_text"), DUI_T("TabBox")}, {DUI_T("tab_box_item_index"), DUI_T("14")}});
    ui::Attach(tab_ctrl1, tab_ctrl_item15);

    auto* tab_ctrl_item16 = ui::Create<ui::TabCtrlItem>(this, {{DUI_T("class"), DUI_T("tab_ctrl_item")}, {DUI_T("title"), DUI_T("GridBox")}, {DUI_T("tooltip_text"), DUI_T("GridBox")}, {DUI_T("tab_box_item_index"), DUI_T("15")}});
    ui::Attach(tab_ctrl1, tab_ctrl_item16);

    auto* tab_ctrl_item17 = ui::Create<ui::TabCtrlItem>(this, {{DUI_T("class"), DUI_T("tab_ctrl_item")}, {DUI_T("title"), DUI_T("GridScrollBox")}, {DUI_T("tooltip_text"), DUI_T("GridScrollBox")}, {DUI_T("tab_box_item_index"), DUI_T("16")}});
    ui::Attach(tab_ctrl1, tab_ctrl_item17);

    ui::Attach(vbox1, tab_ctrl1);

    auto* tab_box1 = ui::Create<ui::TabBox>(this, {{DUI_T("name"), DUI_T("main_view_tab")}, {DUI_T("bkcolor"), DUI_T("white")}, {DUI_T("selected_id"), DUI_T("0")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("fade_switch_type"), DUI_T("FadeInOutX")}, {DUI_T("fade_switch_frame_interval_ms"), DUI_T("16")}, {DUI_T("fade_switch_total_ms"), DUI_T("220")}, {DUI_T("fade_switch_easing_function"), DUI_T("EaseOutCubic")}});
    auto* vbox2 = ui::Create<ui::VBox>(this, {{DUI_T("name"), DUI_T("main_view_page_00")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("stretch")}});
    auto* box2 = ui::Create<ui::Box>(this, {{DUI_T("width"), DUI_T("100%")}, {DUI_T("height"), DUI_T("100%")}, {DUI_T("bkcolor"), DUI_T("gray")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("red")}, {DUI_T("padding"), DUI_T("8,8,8,8")}});
    auto* control3 = ui::Create<ui::Control>(this, {{DUI_T("float"), DUI_T("true")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("green")}, {DUI_T("bkcolor"), DUI_T("Cornsilk")}});
    ui::Attach(box2, control3);

    auto* label2 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("minwidth/minheight: 300*300")}, {DUI_T("float"), DUI_T("true")}, {DUI_T("text_align"), DUI_T("top")}, {DUI_T("width"), DUI_T("200")}, {DUI_T("height"), DUI_T("100")}, {DUI_T("minwidth"), DUI_T("300")}, {DUI_T("minheight"), DUI_T("300")}, {DUI_T("margin"), DUI_T("64,160,6,6")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("green")}, {DUI_T("bkcolor"), DUI_T("Olive")}});
    ui::Attach(box2, label2);

    auto* label3 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("maxwidth/maxheight: 80*80")}, {DUI_T("float"), DUI_T("true")}, {DUI_T("single_line"), DUI_T("false")}, {DUI_T("width"), DUI_T("200")}, {DUI_T("height"), DUI_T("100")}, {DUI_T("maxwidth"), DUI_T("80")}, {DUI_T("maxheight"), DUI_T("80")}, {DUI_T("margin"), DUI_T("600,160,6,6")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("green")}, {DUI_T("bkcolor"), DUI_T("LemonChiffon")}});
    ui::Attach(box2, label3);

    auto* label_box1 = ui::Create<ui::LabelBox>(this, {{DUI_T("text"), DUI_T("Layout Test")}, {DUI_T("float"), DUI_T("true")}, {DUI_T("single_line"), DUI_T("false")}, {DUI_T("width"), DUI_T("200")}, {DUI_T("height"), DUI_T("200")}, {DUI_T("margin"), DUI_T("800,160,6,6")}, {DUI_T("padding"), DUI_T("6,6,6,6")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("green")}, {DUI_T("bkcolor"), DUI_T("LemonChiffon")}});
    auto* control4 = ui::Create<ui::Control>(this, {{DUI_T("margin"), DUI_T("6,6,6,6")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("border_size"), DUI_T("2")}, {DUI_T("border_color"), DUI_T("red")}, {DUI_T("bkcolor"), DUI_T("Sienna")}});
    ui::Attach(label_box1, control4);

    ui::Attach(box2, label_box1);

    auto* label4 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_box")}, {DUI_T("text"), DUI_T("1 left,top")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("minwidth"), DUI_T("300")}, {DUI_T("maxheight"), DUI_T("60")}});
    ui::Attach(box2, label4);

    auto* label5 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_box")}, {DUI_T("text"), DUI_T("2 left,center")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(box2, label5);

    auto* label6 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_box")}, {DUI_T("text"), DUI_T("3 left,bottom")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("bottom")}});
    ui::Attach(box2, label6);

    auto* label7 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_box")}, {DUI_T("text"), DUI_T("4 center,top")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(box2, label7);

    auto* label8 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_box")}, {DUI_T("text"), DUI_T("5 center,center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(box2, label8);

    auto* label9 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_box")}, {DUI_T("text"), DUI_T("51 width: 70%")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("width"), DUI_T("70%")}, {DUI_T("height"), DUI_T("40")}, {DUI_T("alpha"), DUI_T("128")}, {DUI_T("margin"), DUI_T("10,200,10,10")}});
    ui::Attach(box2, label9);

    auto* label10 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_box")}, {DUI_T("text"), DUI_T("52 height: 70%")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("width"), DUI_T("200")}, {DUI_T("height"), DUI_T("70%")}, {DUI_T("alpha"), DUI_T("128")}, {DUI_T("margin"), DUI_T("300,10,10,10")}});
    ui::Attach(box2, label10);

    auto* label11 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_box")}, {DUI_T("text"), DUI_T("6 center,bottom")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("bottom")}});
    ui::Attach(box2, label11);

    auto* label12 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_box")}, {DUI_T("text"), DUI_T("7 right,top")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(box2, label12);

    auto* label13 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_box")}, {DUI_T("text"), DUI_T("8 right,center")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(box2, label13);

    auto* label14 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_box")}, {DUI_T("text"), DUI_T("9 right,bottom")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("bottom")}});
    ui::Attach(box2, label14);

    auto* label15 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Box Container Layout Test: the container does not support alignment (child_halign/child_valign); set alignment on the child controls (halign/valign)")}});
    ui::Attach(vbox2, label15);

    ui::Attach(vbox2, box2);
    ui::Attach(tab_box1, vbox2);

    auto* vbox3 = ui::Create<ui::VBox>(this, {{DUI_T("name"), DUI_T("main_view_page_01")}});
    auto* vbox4 = ui::Create<ui::VBox>(this, {});
    auto* hbox4 = ui::Create<ui::HBox>(this, {{DUI_T("name"), DUI_T("hbox_test")}, {DUI_T("child_halign"), DUI_T("left")}, {DUI_T("child_valign"), DUI_T("top")}, {DUI_T("bkcolor"), DUI_T("gray")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("red")}, {DUI_T("padding"), DUI_T("8,8,8,8")}, {DUI_T("height"), DUI_T("60%")}});
    auto* control5 = ui::Create<ui::Control>(this, {{DUI_T("float"), DUI_T("true")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("green")}, {DUI_T("bkcolor"), DUI_T("Cornsilk")}});
    ui::Attach(hbox4, control5);

    auto* label16 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("minwidth/minheight: 200*200")}, {DUI_T("float"), DUI_T("true")}, {DUI_T("text_align"), DUI_T("top")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("height"), DUI_T("100")}, {DUI_T("minwidth"), DUI_T("200")}, {DUI_T("minheight"), DUI_T("200")}, {DUI_T("margin"), DUI_T("64,60,6,6")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("green")}, {DUI_T("bkcolor"), DUI_T("Olive")}});
    ui::Attach(hbox4, label16);

    auto* label17 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("maxwidth/maxheight: 80*80")}, {DUI_T("float"), DUI_T("true")}, {DUI_T("single_line"), DUI_T("false")}, {DUI_T("width"), DUI_T("200")}, {DUI_T("height"), DUI_T("100")}, {DUI_T("maxwidth"), DUI_T("80")}, {DUI_T("maxheight"), DUI_T("80")}, {DUI_T("margin"), DUI_T("600,70,6,6")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("green")}, {DUI_T("bkcolor"), DUI_T("LemonChiffon")}});
    ui::Attach(hbox4, label17);

    auto* label_box2 = ui::Create<ui::LabelBox>(this, {{DUI_T("text"), DUI_T("Layout Test")}, {DUI_T("float"), DUI_T("true")}, {DUI_T("single_line"), DUI_T("false")}, {DUI_T("width"), DUI_T("200")}, {DUI_T("height"), DUI_T("200")}, {DUI_T("margin"), DUI_T("800,50,6,6")}, {DUI_T("padding"), DUI_T("6,6,6,6")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("green")}, {DUI_T("bkcolor"), DUI_T("LemonChiffon")}});
    auto* control6 = ui::Create<ui::Control>(this, {{DUI_T("margin"), DUI_T("6,6,6,6")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("border_size"), DUI_T("2")}, {DUI_T("border_color"), DUI_T("red")}, {DUI_T("bkcolor"), DUI_T("Sienna")}});
    ui::Attach(label_box2, control6);

    ui::Attach(hbox4, label_box2);

    auto* label18 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("1 left,top minwidth:200")}, {DUI_T("multi_line"), DUI_T("true")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("max_height"), DUI_T("44")}});
    ui::Attach(hbox4, label18);

    auto* label19 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("2 left,center")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hbox4, label19);

    auto* label20 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("3 left,bottom 50%")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("height"), DUI_T("50%")}, {DUI_T("width"), DUI_T("150")}});
    ui::Attach(hbox4, label20);

    auto* label21 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("4 center,top")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(hbox4, label21);

    auto* label22 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("5 center,center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hbox4, label22);

    auto* label23 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("6 center,bottom")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("height"), DUI_T("120")}, {DUI_T("valign"), DUI_T("bottom")}});
    ui::Attach(hbox4, label23);

    auto* label24 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("7 right,top")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(hbox4, label24);

    auto* label25 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("8 right,center")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hbox4, label25);

    auto* label26 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("9 right,bottom")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("bottom")}});
    ui::Attach(hbox4, label26);

    ui::Attach(vbox4, hbox4);

    auto* hbox5 = ui::Create<ui::HBox>(this, {{DUI_T("bkcolor"), DUI_T("gray")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("red")}, {DUI_T("padding"), DUI_T("8,8,8,8")}, {DUI_T("margin"), DUI_T("0,4,0,0")}, {DUI_T("height"), DUI_T("15%")}});
    auto* label27 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("1 left,top minwidth:200")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("max_height"), DUI_T("44")}});
    ui::Attach(hbox5, label27);

    auto* label28 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("2 left,center")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("width"), DUI_T("80")}});
    ui::Attach(hbox5, label28);

    auto* label29 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("3 left,bottom height:70%")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("height"), DUI_T("70%")}});
    ui::Attach(hbox5, label29);

    auto* label30 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("4 right,bottom 20%")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("width"), DUI_T("20%")}, {DUI_T("bkcolor"), DUI_T("Cornsilk")}});
    ui::Attach(hbox5, label30);

    auto* label31 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("5 right,top")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("width"), DUI_T("80")}});
    ui::Attach(hbox5, label31);

    auto* label32 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("6 right,bottom 30%")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("width"), DUI_T("30%")}, {DUI_T("bkcolor"), DUI_T("Cornsilk")}});
    ui::Attach(hbox5, label32);

    auto* label33 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("7 right,center")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("width"), DUI_T("80")}});
    ui::Attach(hbox5, label33);

    auto* label34 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("8 right,bottom 40%")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("width"), DUI_T("40%")}, {DUI_T("bkcolor"), DUI_T("Cornsilk")}});
    ui::Attach(hbox5, label34);

    ui::Attach(vbox4, hbox5);

    auto* hbox6 = ui::Create<ui::HBox>(this, {{DUI_T("bkcolor"), DUI_T("gray")}, {DUI_T("child_margin_x"), DUI_T("8")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("red")}, {DUI_T("margin"), DUI_T("0,4,0,0")}, {DUI_T("height"), DUI_T("10%")}});
    auto* label35 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("1 left,top")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("100%")}, {DUI_T("margin"), DUI_T("0,6,0,6")}});
    ui::Attach(hbox6, label35);

    auto* label36 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("2 left,center")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("100%")}, {DUI_T("margin"), DUI_T("0,6,0,6")}});
    ui::Attach(hbox6, label36);

    auto* label37 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("3 left,bottom")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("100%")}, {DUI_T("margin"), DUI_T("0,6,0,6")}});
    ui::Attach(hbox6, label37);

    ui::Attach(vbox4, hbox6);

    auto* hbox7 = ui::Create<ui::HBox>(this, {{DUI_T("child_margin_x"), DUI_T("2")}, {DUI_T("bkcolor"), DUI_T("gray")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("red")}, {DUI_T("padding"), DUI_T("8,8,8,8")}, {DUI_T("margin"), DUI_T("0,4,0,0")}, {DUI_T("height"), DUI_T("15%")}});
    auto* label38 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("1 left,top minwidth:100")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("width"), DUI_T("100%")}, {DUI_T("height"), DUI_T("70")}, {DUI_T("min_width"), DUI_T("100")}, {DUI_T("max_height"), DUI_T("60")}, {DUI_T("bkcolor"), DUI_T("Cornsilk")}});
    ui::Attach(hbox7, label38);

    auto* label39 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("2 left,center maxwidth:100")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("width"), DUI_T("100%")}, {DUI_T("height"), DUI_T("50")}, {DUI_T("max_width"), DUI_T("100")}, {DUI_T("bkcolor"), DUI_T("Cornsilk")}});
    ui::Attach(hbox7, label39);

    auto* label40 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("3 left,bottom fixed")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("width"), DUI_T("140")}, {DUI_T("height"), DUI_T("40")}});
    ui::Attach(hbox7, label40);

    auto* label41 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("4 center,top")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("width"), DUI_T("100%")}, {DUI_T("height"), DUI_T("50")}, {DUI_T("bkcolor"), DUI_T("Cornsilk")}});
    ui::Attach(hbox7, label41);

    auto* label42 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("5 right,center")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("width"), DUI_T("100%")}, {DUI_T("height"), DUI_T("40")}, {DUI_T("bkcolor"), DUI_T("Cornsilk")}});
    ui::Attach(hbox7, label42);

    auto* label43 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("6 right,bottom")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("width"), DUI_T("100%")}, {DUI_T("height"), DUI_T("50")}, {DUI_T("bkcolor"), DUI_T("Cornsilk")}});
    ui::Attach(hbox7, label43);

    ui::Attach(vbox4, hbox7);

    auto* vbox5 = ui::Create<ui::VBox>(this, {{DUI_T("height"), DUI_T("auto")}, {DUI_T("padding"), DUI_T("6,0,6,6")}});
    auto* label44 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("HBox Container Layout Test")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vbox5, label44);

    auto* hbox8 = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("28")}});
    auto* label45 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Container content horizontal alignment (child_halign): ")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hbox8, label45);

    auto* option1 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("hbox_group_halign")}, {DUI_T("text"), DUI_T("Align Left (left)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("true")}});
    ui::Attach(hbox8, option1);

    auto* option2 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("hbox_group_halign")}, {DUI_T("text"), DUI_T("Align Center (center)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("false")}});
    ui::Attach(hbox8, option2);

    auto* option3 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("hbox_group_halign")}, {DUI_T("text"), DUI_T("Align Right (right)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("false")}});
    ui::Attach(hbox8, option3);

    auto* label46 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("(effective only when total child width is smaller than the container width)")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hbox8, label46);

    ui::Attach(vbox5, hbox8);

    auto* hbox9 = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("28")}});
    auto* label47 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Container content vertical alignment (child_valign): not supported, set it on the child controls")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hbox9, label47);

    ui::Attach(vbox5, hbox9);

    auto* label48 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Child control alignment: halign not supported, valign supported")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vbox5, label48);

    ui::Attach(vbox3, vbox5);

    ui::Attach(vbox3, vbox4);
    ui::Attach(tab_box1, vbox3);

    auto* vbox6 = ui::Create<ui::VBox>(this, {{DUI_T("name"), DUI_T("main_view_page_02")}});
    auto* hbox10 = ui::Create<ui::HBox>(this, {});
    auto* vbox7 = ui::Create<ui::VBox>(this, {{DUI_T("name"), DUI_T("vbox_test")}, {DUI_T("bkcolor"), DUI_T("gray")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("red")}, {DUI_T("padding"), DUI_T("8,8,8,8")}, {DUI_T("width"), DUI_T("60%")}});
    auto* control7 = ui::Create<ui::Control>(this, {{DUI_T("float"), DUI_T("true")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("green")}, {DUI_T("bkcolor"), DUI_T("Cornsilk")}});
    ui::Attach(vbox7, control7);

    auto* label49 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("minwidth/minheight: 100*100")}, {DUI_T("float"), DUI_T("true")}, {DUI_T("text_align"), DUI_T("top")}, {DUI_T("width"), DUI_T("50")}, {DUI_T("height"), DUI_T("50")}, {DUI_T("minwidth"), DUI_T("100")}, {DUI_T("minheight"), DUI_T("100")}, {DUI_T("margin"), DUI_T("64,60,6,6")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("green")}, {DUI_T("bkcolor"), DUI_T("Olive")}});
    ui::Attach(vbox7, label49);

    auto* label50 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("maxwidth/maxheight: 80*80")}, {DUI_T("float"), DUI_T("true")}, {DUI_T("single_line"), DUI_T("false")}, {DUI_T("width"), DUI_T("200")}, {DUI_T("height"), DUI_T("100")}, {DUI_T("maxwidth"), DUI_T("80")}, {DUI_T("maxheight"), DUI_T("80")}, {DUI_T("margin"), DUI_T("600,160,6,6")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("green")}, {DUI_T("bkcolor"), DUI_T("LemonChiffon")}});
    ui::Attach(vbox7, label50);

    auto* label_box3 = ui::Create<ui::LabelBox>(this, {{DUI_T("text"), DUI_T("Layout Test")}, {DUI_T("float"), DUI_T("true")}, {DUI_T("single_line"), DUI_T("false")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("height"), DUI_T("100")}, {DUI_T("margin"), DUI_T("300,100,6,6")}, {DUI_T("padding"), DUI_T("6,6,6,6")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("green")}, {DUI_T("bkcolor"), DUI_T("LemonChiffon")}});
    auto* control8 = ui::Create<ui::Control>(this, {{DUI_T("margin"), DUI_T("6,6,6,6")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("border_size"), DUI_T("2")}, {DUI_T("border_color"), DUI_T("red")}, {DUI_T("bkcolor"), DUI_T("Sienna")}});
    ui::Attach(label_box3, control8);

    ui::Attach(vbox7, label_box3);

    auto* label51 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vbox")}, {DUI_T("text"), DUI_T("1 left,top")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("min_width"), DUI_T("400")}, {DUI_T("max_height"), DUI_T("30")}});
    ui::Attach(vbox7, label51);

    auto* label52 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vbox")}, {DUI_T("text"), DUI_T("2 left,center")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("max_height"), DUI_T("30")}});
    ui::Attach(vbox7, label52);

    auto* label53 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vbox")}, {DUI_T("text"), DUI_T("3 left,bottom: width=stretch")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("width"), DUI_T("stretch")}});
    ui::Attach(vbox7, label53);

    auto* label54 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vbox")}, {DUI_T("text"), DUI_T("4 center,top")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(vbox7, label54);

    auto* label55 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vbox")}, {DUI_T("text"), DUI_T("5 center,center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("min_height"), DUI_T("70")}});
    ui::Attach(vbox7, label55);

    auto* label56 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vbox")}, {DUI_T("text"), DUI_T("6 center,bottom width=50%")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("width"), DUI_T("50%")}});
    ui::Attach(vbox7, label56);

    auto* label57 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vbox")}, {DUI_T("text"), DUI_T("7 right,top")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(vbox7, label57);

    auto* label58 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vbox")}, {DUI_T("text"), DUI_T("8 right,center")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vbox7, label58);

    auto* label59 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vbox")}, {DUI_T("text"), DUI_T("9 right,bottom")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("max_height"), DUI_T("50")}});
    ui::Attach(vbox7, label59);

    ui::Attach(hbox10, vbox7);

    auto* vbox8 = ui::Create<ui::VBox>(this, {{DUI_T("bkcolor"), DUI_T("gray")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("red")}, {DUI_T("padding"), DUI_T("8,8,8,8")}, {DUI_T("width"), DUI_T("20%")}});
    auto* label60 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("1 left,top min_height:80")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("max_height"), DUI_T("80")}});
    ui::Attach(vbox8, label60);

    auto* label61 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("2 left,center")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("height"), DUI_T("80")}});
    ui::Attach(vbox8, label61);

    auto* label62 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("3 left,bottom width 70%")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("width"), DUI_T("70%")}});
    ui::Attach(vbox8, label62);

    auto* label63 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("4 right,bottom height 20%")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("height"), DUI_T("20%")}, {DUI_T("bkcolor"), DUI_T("Cornsilk")}});
    ui::Attach(vbox8, label63);

    auto* label64 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("5 right,top")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("width"), DUI_T("80")}});
    ui::Attach(vbox8, label64);

    auto* label65 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("6 right,bottom 30%")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("height"), DUI_T("30%")}, {DUI_T("bkcolor"), DUI_T("Cornsilk")}});
    ui::Attach(vbox8, label65);

    auto* label66 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("7 right,center")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("height"), DUI_T("20%")}});
    ui::Attach(vbox8, label66);

    auto* label67 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("8 right,bottom 40%")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("height"), DUI_T("40%")}, {DUI_T("bkcolor"), DUI_T("Cornsilk")}});
    ui::Attach(vbox8, label67);

    ui::Attach(hbox10, vbox8);

    auto* vbox9 = ui::Create<ui::VBox>(this, {{DUI_T("bkcolor"), DUI_T("gray")}, {DUI_T("child_margin_y"), DUI_T("8")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("red")}, {DUI_T("width"), DUI_T("20%")}});
    auto* label68 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("1 left,top")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("100%")}, {DUI_T("margin"), DUI_T("6,6,6,0")}});
    ui::Attach(vbox9, label68);

    auto* label69 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("2 left,center")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("100%")}, {DUI_T("margin"), DUI_T("6,0,6,0")}});
    ui::Attach(vbox9, label69);

    auto* label70 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hbox")}, {DUI_T("text"), DUI_T("3 left,bottom")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("100%")}, {DUI_T("margin"), DUI_T("6,0,6,6")}});
    ui::Attach(vbox9, label70);

    ui::Attach(hbox10, vbox9);

    auto* vbox10 = ui::Create<ui::VBox>(this, {{DUI_T("height"), DUI_T("auto")}, {DUI_T("padding"), DUI_T("6,0,6,6")}});
    auto* label71 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("VBox Container Layout Test")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vbox10, label71);

    auto* hbox11 = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("28")}});
    auto* label72 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Container content vertical alignment (child_valign): ")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hbox11, label72);

    auto* option4 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("vbox_group_valign")}, {DUI_T("text"), DUI_T("Align Top (top)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("true")}});
    ui::Attach(hbox11, option4);

    auto* option5 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("vbox_group_valign")}, {DUI_T("text"), DUI_T("Align Center (center)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("false")}});
    ui::Attach(hbox11, option5);

    auto* option6 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("vbox_group_valign")}, {DUI_T("text"), DUI_T("Align Bottom (bottom)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("false")}});
    ui::Attach(hbox11, option6);

    auto* label73 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("(effective only when total child height is smaller than the container height)")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hbox11, label73);

    ui::Attach(vbox10, hbox11);

    auto* hbox12 = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("28")}});
    auto* label74 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Container content horizontal alignment (child_halign): not supported, set it on the child controls")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hbox12, label74);

    ui::Attach(vbox10, hbox12);

    auto* label75 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Child control alignment: halign supported, valign not supported")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vbox10, label75);

    ui::Attach(vbox6, vbox10);
    ui::Attach(vbox6, hbox10);
    ui::Attach(tab_box1, vbox6);

    auto* vbox11 = ui::Create<ui::VBox>(this, {{DUI_T("name"), DUI_T("main_view_page_03")}});
    auto* vbox12 = ui::Create<ui::VBox>(this, {});
    auto* hflow_box1 = ui::Create<ui::HFlowBox>(this, {{DUI_T("name"), DUI_T("hflowbox_test")}, {DUI_T("child_halign"), DUI_T("left")}, {DUI_T("child_valign"), DUI_T("top")}, {DUI_T("bkcolor"), DUI_T("gray")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("red")}, {DUI_T("padding"), DUI_T("8,8,8,8")}, {DUI_T("height"), DUI_T("100%")}});
    auto* control9 = ui::Create<ui::Control>(this, {{DUI_T("float"), DUI_T("true")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("green")}, {DUI_T("bkcolor"), DUI_T("Cornsilk")}});
    ui::Attach(hflow_box1, control9);

    auto* label76 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("minwidth/minheight: 200*200")}, {DUI_T("float"), DUI_T("true")}, {DUI_T("text_align"), DUI_T("top")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("height"), DUI_T("100")}, {DUI_T("minwidth"), DUI_T("200")}, {DUI_T("minheight"), DUI_T("200")}, {DUI_T("margin"), DUI_T("64,60,6,6")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("green")}, {DUI_T("bkcolor"), DUI_T("Olive")}});
    ui::Attach(hflow_box1, label76);

    auto* label77 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("maxwidth/maxheight: 80*80")}, {DUI_T("float"), DUI_T("true")}, {DUI_T("single_line"), DUI_T("false")}, {DUI_T("width"), DUI_T("200")}, {DUI_T("height"), DUI_T("100")}, {DUI_T("maxwidth"), DUI_T("80")}, {DUI_T("maxheight"), DUI_T("80")}, {DUI_T("margin"), DUI_T("600,70,6,6")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("green")}, {DUI_T("bkcolor"), DUI_T("LemonChiffon")}});
    ui::Attach(hflow_box1, label77);

    auto* label78 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflowbox")}, {DUI_T("text"), DUI_T("1 top")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("minwidth"), DUI_T("300")}, {DUI_T("maxheight"), DUI_T("60")}});
    ui::Attach(hflow_box1, label78);

    auto* label79 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflowbox")}, {DUI_T("text"), DUI_T("2 center")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hflow_box1, label79);

    auto* label80 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflowbox")}, {DUI_T("text"), DUI_T("3 bottom")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("height"), DUI_T("40")}});
    ui::Attach(hflow_box1, label80);

    auto* label81 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflowbox")}, {DUI_T("text"), DUI_T("4 top")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(hflow_box1, label81);

    auto* label82 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflowbox")}, {DUI_T("text"), DUI_T("5 center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("height"), DUI_T("40")}});
    ui::Attach(hflow_box1, label82);

    auto* label83 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflowbox")}, {DUI_T("text"), DUI_T("6 bottom")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("bottom")}});
    ui::Attach(hflow_box1, label83);

    auto* label84 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflowbox")}, {DUI_T("text"), DUI_T("7 top")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("height"), DUI_T("40")}});
    ui::Attach(hflow_box1, label84);

    auto* label85 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflowbox")}, {DUI_T("text"), DUI_T("8 center")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hflow_box1, label85);

    auto* label86 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflowbox")}, {DUI_T("text"), DUI_T("9 bottom")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("width"), DUI_T("300")}});
    ui::Attach(hflow_box1, label86);

    auto* label87 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflowbox")}, {DUI_T("text"), DUI_T("10 top")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("minwidth"), DUI_T("300")}, {DUI_T("maxheight"), DUI_T("60")}});
    ui::Attach(hflow_box1, label87);

    auto* label88 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflowbox")}, {DUI_T("text"), DUI_T("11 center stretch: 100%")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("width"), DUI_T("stretch")}});
    ui::Attach(hflow_box1, label88);

    auto* label89 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflowbox")}, {DUI_T("text"), DUI_T("12 bottom")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("height"), DUI_T("40")}});
    ui::Attach(hflow_box1, label89);

    auto* label90 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflowbox")}, {DUI_T("text"), DUI_T("13 top stretch: 50%")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("width"), DUI_T("50%")}});
    ui::Attach(hflow_box1, label90);

    auto* label91 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflowbox")}, {DUI_T("text"), DUI_T("12 center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("height"), DUI_T("40")}});
    ui::Attach(hflow_box1, label91);

    auto* label92 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflowbox")}, {DUI_T("text"), DUI_T("15 bottom")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("bottom")}});
    ui::Attach(hflow_box1, label92);

    auto* label93 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflowbox")}, {DUI_T("text"), DUI_T("16 top")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("height"), DUI_T("40")}});
    ui::Attach(hflow_box1, label93);

    auto* label94 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflowbox")}, {DUI_T("text"), DUI_T("17 center")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hflow_box1, label94);

    auto* label95 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflowbox")}, {DUI_T("text"), DUI_T("18 bottom")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("width"), DUI_T("300")}});
    ui::Attach(hflow_box1, label95);

    auto* label96 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflowbox")}, {DUI_T("text"), DUI_T("19 center, width: 1600")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("width"), DUI_T("1600")}});
    ui::Attach(hflow_box1, label96);

    auto* label97 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflowbox")}, {DUI_T("text"), DUI_T("20 top")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("height"), DUI_T("40")}});
    ui::Attach(hflow_box1, label97);

    ui::Attach(vbox12, hflow_box1);

    auto* vbox13 = ui::Create<ui::VBox>(this, {{DUI_T("height"), DUI_T("84")}, {DUI_T("padding"), DUI_T("6,0,6,0")}});
    auto* label98 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("HFlowBox Container Layout Test")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vbox13, label98);

    auto* hbox13 = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("28")}});
    auto* label99 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Container content horizontal alignment:")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hbox13, label99);

    auto* option7 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("hflowbox_group_halign")}, {DUI_T("text"), DUI_T("Align Left (left)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("true")}});
    ui::Attach(hbox13, option7);

    auto* option8 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("hflowbox_group_halign")}, {DUI_T("text"), DUI_T("Align Center (center)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("false")}});
    ui::Attach(hbox13, option8);

    auto* option9 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("hflowbox_group_halign")}, {DUI_T("text"), DUI_T("Align Right (right)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("false")}});
    ui::Attach(hbox13, option9);

    auto* label100 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("(aligned within the same row)")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hbox13, label100);

    ui::Attach(vbox13, hbox13);

    auto* hbox14 = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("28")}});
    auto* label101 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Container content vertical alignment:")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hbox14, label101);

    auto* option10 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("hflowbox_group_valign")}, {DUI_T("text"), DUI_T("Align Top (top)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("true")}});
    ui::Attach(hbox14, option10);

    auto* option11 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("hflowbox_group_valign")}, {DUI_T("text"), DUI_T("Align Center (center)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("false")}});
    ui::Attach(hbox14, option11);

    auto* option12 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("hflowbox_group_valign")}, {DUI_T("text"), DUI_T("Align Bottom (bottom)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("false")}});
    ui::Attach(hbox14, option12);

    auto* label102 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("(effective only when total child height is smaller than the display area height)")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hbox14, label102);

    ui::Attach(vbox13, hbox14);

    ui::Attach(vbox11, vbox13);
    ui::Attach(vbox11, vbox12);
    ui::Attach(tab_box1, vbox11);

    auto* vbox14 = ui::Create<ui::VBox>(this, {{DUI_T("name"), DUI_T("main_view_page_04")}});
    auto* vbox15 = ui::Create<ui::VBox>(this, {});
    auto* vflow_box1 = ui::Create<ui::VFlowBox>(this, {{DUI_T("name"), DUI_T("vflowbox_test")}, {DUI_T("child_halign"), DUI_T("left")}, {DUI_T("child_valign"), DUI_T("top")}, {DUI_T("bkcolor"), DUI_T("gray")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("red")}, {DUI_T("padding"), DUI_T("8,8,8,8")}, {DUI_T("height"), DUI_T("100%")}});
    auto* control10 = ui::Create<ui::Control>(this, {{DUI_T("float"), DUI_T("true")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("green")}, {DUI_T("bkcolor"), DUI_T("Cornsilk")}});
    ui::Attach(vflow_box1, control10);

    auto* label103 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("minwidth/minheight: 200*200")}, {DUI_T("float"), DUI_T("true")}, {DUI_T("text_align"), DUI_T("top")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("height"), DUI_T("100")}, {DUI_T("minwidth"), DUI_T("200")}, {DUI_T("minheight"), DUI_T("200")}, {DUI_T("margin"), DUI_T("64,60,6,6")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("green")}, {DUI_T("bkcolor"), DUI_T("Olive")}});
    ui::Attach(vflow_box1, label103);

    auto* label104 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("maxwidth/maxheight: 80*80")}, {DUI_T("float"), DUI_T("true")}, {DUI_T("single_line"), DUI_T("false")}, {DUI_T("width"), DUI_T("200")}, {DUI_T("height"), DUI_T("100")}, {DUI_T("maxwidth"), DUI_T("80")}, {DUI_T("maxheight"), DUI_T("80")}, {DUI_T("margin"), DUI_T("600,70,6,6")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("green")}, {DUI_T("bkcolor"), DUI_T("LemonChiffon")}});
    ui::Attach(vflow_box1, label104);

    auto* label105 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflowbox")}, {DUI_T("text"), DUI_T("1 left")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("minwidth"), DUI_T("300")}, {DUI_T("maxheight"), DUI_T("60")}});
    ui::Attach(vflow_box1, label105);

    auto* label106 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflowbox")}, {DUI_T("text"), DUI_T("2 center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(vflow_box1, label106);

    auto* label107 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflowbox")}, {DUI_T("text"), DUI_T("3 right")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("height"), DUI_T("40")}});
    ui::Attach(vflow_box1, label107);

    auto* label108 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflowbox")}, {DUI_T("text"), DUI_T("4 left")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vflow_box1, label108);

    auto* label109 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflowbox")}, {DUI_T("text"), DUI_T("5 center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("height"), DUI_T("40")}});
    ui::Attach(vflow_box1, label109);

    auto* label110 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflowbox")}, {DUI_T("text"), DUI_T("6 right")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vflow_box1, label110);

    auto* label111 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflowbox")}, {DUI_T("text"), DUI_T("7 left")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("height"), DUI_T("40")}});
    ui::Attach(vflow_box1, label111);

    auto* label112 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflowbox")}, {DUI_T("text"), DUI_T("8 center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("bottom")}});
    ui::Attach(vflow_box1, label112);

    auto* label113 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflowbox")}, {DUI_T("text"), DUI_T("9 right")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("width"), DUI_T("200")}});
    ui::Attach(vflow_box1, label113);

    auto* label114 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflowbox")}, {DUI_T("text"), DUI_T("10 left")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("minwidth"), DUI_T("200")}, {DUI_T("maxheight"), DUI_T("60")}});
    ui::Attach(vflow_box1, label114);

    auto* label115 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflowbox")}, {DUI_T("text"), DUI_T("11 center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("height"), DUI_T("100")}, {DUI_T("width"), DUI_T("100")}});
    ui::Attach(vflow_box1, label115);

    auto* label116 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflowbox")}, {DUI_T("text"), DUI_T("12 right")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("height"), DUI_T("40")}});
    ui::Attach(vflow_box1, label116);

    auto* label117 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflowbox")}, {DUI_T("text"), DUI_T("13 left stretch: 50%")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("height"), DUI_T("50%")}, {DUI_T("width"), DUI_T("120")}});
    ui::Attach(vflow_box1, label117);

    auto* label118 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflowbox")}, {DUI_T("text"), DUI_T("12 center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("height"), DUI_T("40")}});
    ui::Attach(vflow_box1, label118);

    auto* label119 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflowbox")}, {DUI_T("text"), DUI_T("15 left")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("bottom")}});
    ui::Attach(vflow_box1, label119);

    auto* label120 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflowbox")}, {DUI_T("text"), DUI_T("16 center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("height"), DUI_T("40")}});
    ui::Attach(vflow_box1, label120);

    auto* label121 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflowbox")}, {DUI_T("text"), DUI_T("17 right")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vflow_box1, label121);

    auto* label122 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflowbox")}, {DUI_T("text"), DUI_T("18 left")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("width"), DUI_T("200")}});
    ui::Attach(vflow_box1, label122);

    auto* label123 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflowbox")}, {DUI_T("text"), DUI_T("19 center, height: 500")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("height"), DUI_T("500")}, {DUI_T("width"), DUI_T("100")}});
    ui::Attach(vflow_box1, label123);

    auto* label124 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflowbox")}, {DUI_T("text"), DUI_T("20 right")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("height"), DUI_T("40")}});
    ui::Attach(vflow_box1, label124);

    ui::Attach(vbox15, vflow_box1);

    auto* vbox16 = ui::Create<ui::VBox>(this, {{DUI_T("height"), DUI_T("84")}, {DUI_T("padding"), DUI_T("6,0,6,0")}});
    auto* label125 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("VFlowBox Container Layout Test")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vbox16, label125);

    auto* hbox15 = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("28")}});
    auto* label126 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Container content horizontal alignment:")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hbox15, label126);

    auto* option13 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("vflowbox_group_halign")}, {DUI_T("text"), DUI_T("Align Left (left)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("true")}});
    ui::Attach(hbox15, option13);

    auto* option14 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("vflowbox_group_halign")}, {DUI_T("text"), DUI_T("Align Center (center)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("false")}});
    ui::Attach(hbox15, option14);

    auto* option15 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("vflowbox_group_halign")}, {DUI_T("text"), DUI_T("Align Right (right)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("false")}});
    ui::Attach(hbox15, option15);

    auto* label127 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("(effective only when total child width is smaller than the display area width)")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hbox15, label127);

    ui::Attach(vbox16, hbox15);

    auto* hbox16 = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("28")}});
    auto* label128 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Container content vertical alignment:")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hbox16, label128);

    auto* option16 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("vflowbox_group_valign")}, {DUI_T("text"), DUI_T("Align Top (top)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("true")}});
    ui::Attach(hbox16, option16);

    auto* option17 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("vflowbox_group_valign")}, {DUI_T("text"), DUI_T("Align Center (center)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("false")}});
    ui::Attach(hbox16, option17);

    auto* option18 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("vflowbox_group_valign")}, {DUI_T("text"), DUI_T("Align Bottom (bottom)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("false")}});
    ui::Attach(hbox16, option18);

    auto* label129 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("(aligned within the same column)")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hbox16, label129);

    ui::Attach(vbox16, hbox16);

    ui::Attach(vbox14, vbox16);
    ui::Attach(vbox14, vbox15);
    ui::Attach(tab_box1, vbox14);

    auto* vbox17 = ui::Create<ui::VBox>(this, {{DUI_T("name"), DUI_T("main_view_page_05")}});

    auto* label149 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("VTileBox Container Layout Test (Vertical)")}});
    ui::Attach(vbox17, label149);

    auto* vtile_box1 = ui::Create<ui::VTileBox>(this, {{DUI_T("child_halign"), DUI_T("left")}, {DUI_T("columns"), DUI_T("0")}, {DUI_T("item_size"), DUI_T("0,0")}, {DUI_T("scale_down"), DUI_T("true")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("auto")}, {DUI_T("padding"), DUI_T("10,10,10,10")}, {DUI_T("bkcolor"), DUI_T("gray")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("red")}});
    auto* control12 = ui::Create<ui::Control>(this, {{DUI_T("float"), DUI_T("true")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("green")}, {DUI_T("bkcolor"), DUI_T("Cornsilk")}});
    ui::Attach(vtile_box1, control12);

    auto* label140 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_box")}, {DUI_T("text"), DUI_T("1 left,top")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(vtile_box1, label140);

    auto* label141 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_box")}, {DUI_T("text"), DUI_T("2 left,center")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vtile_box1, label141);

    auto* label142 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_box")}, {DUI_T("text"), DUI_T("3 left,bottom")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("bottom")}});
    ui::Attach(vtile_box1, label142);

    auto* label143 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_box")}, {DUI_T("text"), DUI_T("4 center,top")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(vtile_box1, label143);

    auto* label144 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_box")}, {DUI_T("text"), DUI_T("5 center,center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vtile_box1, label144);

    auto* label145 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_box")}, {DUI_T("text"), DUI_T("6 center,bottom")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("bottom")}});
    ui::Attach(vtile_box1, label145);

    auto* label146 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_box")}, {DUI_T("text"), DUI_T("7 right,top")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(vtile_box1, label146);

    auto* label147 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_box")}, {DUI_T("text"), DUI_T("8 right,center")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vtile_box1, label147);

    auto* label148 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_box")}, {DUI_T("text"), DUI_T("9 right,bottom")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("bottom")}});
    ui::Attach(vtile_box1, label148);

    ui::Attach(vbox17, vtile_box1);

    auto* label139 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("HTileBox Container Layout Test (Horizontal)")}, {DUI_T("margin"), DUI_T("0,4,0,4")}});
    ui::Attach(vbox17, label139);

    auto* htile_box1 = ui::Create<ui::HTileBox>(this, {{DUI_T("child_valign"), DUI_T("top")}, {DUI_T("rows"), DUI_T("0")}, {DUI_T("item_size"), DUI_T("200,0")}, {DUI_T("scale_down"), DUI_T("true")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("padding"), DUI_T("10,10,10,10")}, {DUI_T("bkcolor"), DUI_T("gray")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("red")}});
    auto* control11 = ui::Create<ui::Control>(this, {{DUI_T("float"), DUI_T("true")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("green")}, {DUI_T("bkcolor"), DUI_T("Cornsilk")}});
    ui::Attach(htile_box1, control11);

    auto* label130 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_box")}, {DUI_T("text"), DUI_T("1 left,top")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(htile_box1, label130);

    auto* label131 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_box")}, {DUI_T("text"), DUI_T("2 left,center")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(htile_box1, label131);

    auto* label132 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_box")}, {DUI_T("text"), DUI_T("3 left,bottom")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("bottom")}});
    ui::Attach(htile_box1, label132);

    auto* label133 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_box")}, {DUI_T("text"), DUI_T("4 center,top")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(htile_box1, label133);

    auto* label134 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_box")}, {DUI_T("text"), DUI_T("5 center,center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(htile_box1, label134);

    auto* label135 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_box")}, {DUI_T("text"), DUI_T("6 center,bottom")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("bottom")}});
    ui::Attach(htile_box1, label135);

    auto* label136 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_box")}, {DUI_T("text"), DUI_T("7 right,top")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(htile_box1, label136);

    auto* label137 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_box")}, {DUI_T("text"), DUI_T("8 right,center")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(htile_box1, label137);

    auto* label138 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_box")}, {DUI_T("text"), DUI_T("9 right,bottom")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("bottom")}});
    ui::Attach(htile_box1, label138);

    ui::Attach(vbox17, htile_box1);
    ui::Attach(tab_box1, vbox17);

    auto* vbox18 = ui::Create<ui::VBox>(this, {{DUI_T("name"), DUI_T("main_view_page_06")}});
    auto* label164 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("ScrollBox Container Layout Test (container content does not support alignment; set alignment on the child controls)")}});
    ui::Attach(vbox18, label164);

    auto* scroll_box1 = ui::Create<ui::ScrollBox>(this, {{DUI_T("vscrollbar"), DUI_T("true")}, {DUI_T("hscrollbar"), DUI_T("true")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("bkcolor"), DUI_T("white")}, {DUI_T("border_size"), DUI_T("2")}, {DUI_T("border_color"), DUI_T("red")}, {DUI_T("padding"), DUI_T("20,20,20,20")}});
    auto* label150 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_scroll_box")}, {DUI_T("text"), DUI_T("1 default left,top")}, {DUI_T("text_align"), DUI_T("left,top")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("width"), DUI_T("5000")}, {DUI_T("height"), DUI_T("2500")}, {DUI_T("bkcolor"), DUI_T("Cornsilk")}});
    ui::Attach(scroll_box1, label150);

    auto* label151 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_scroll_box")}, {DUI_T("text"), DUI_T("2 right,bottom")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("height"), DUI_T("80")}, {DUI_T("margin"), DUI_T("0,0,10,10")}});
    ui::Attach(scroll_box1, label151);

    auto* label152 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_scroll_box")}, {DUI_T("text"), DUI_T("3 left,center")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("height"), DUI_T("80")}});
    ui::Attach(scroll_box1, label152);

    auto* label153 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_scroll_box")}, {DUI_T("text"), DUI_T("4 left,bottom")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("width"), DUI_T("120")}, {DUI_T("height"), DUI_T("80")}, {DUI_T("margin"), DUI_T("10,0,0,10")}});
    ui::Attach(scroll_box1, label153);

    auto* label154 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_scroll_box")}, {DUI_T("text"), DUI_T("5 center,top")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("width"), DUI_T("80")}, {DUI_T("height"), DUI_T("80")}});
    ui::Attach(scroll_box1, label154);

    auto* label155 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_scroll_box")}, {DUI_T("text"), DUI_T("6 center,center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("height"), DUI_T("256")}});
    ui::Attach(scroll_box1, label155);

    auto* label156 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_scroll_box")}, {DUI_T("text"), DUI_T("7 center,bottom")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("height"), DUI_T("80")}});
    ui::Attach(scroll_box1, label156);

    auto* label157 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_scroll_box")}, {DUI_T("text"), DUI_T("8 right,top")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("height"), DUI_T("80")}, {DUI_T("margin"), DUI_T("0,10,10,0")}});
    ui::Attach(scroll_box1, label157);

    auto* label158 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_scroll_box")}, {DUI_T("text"), DUI_T("9 right,center")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("height"), DUI_T("80")}});
    ui::Attach(scroll_box1, label158);

    auto* label159 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_scroll_box")}, {DUI_T("text"), DUI_T("10 width:70%")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("width"), DUI_T("70%")}, {DUI_T("height"), DUI_T("40")}, {DUI_T("alpha"), DUI_T("128")}, {DUI_T("margin"), DUI_T("10,200,10,10")}});
    ui::Attach(scroll_box1, label159);

    auto* label160 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_scroll_box")}, {DUI_T("text"), DUI_T("11 height:70%")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("width"), DUI_T("200")}, {DUI_T("height"), DUI_T("70%")}, {DUI_T("alpha"), DUI_T("128")}, {DUI_T("margin"), DUI_T("300,10,10,10")}});
    ui::Attach(scroll_box1, label160);

    auto* label161 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("float 300*300 center,top")}, {DUI_T("float"), DUI_T("true")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("text_align"), DUI_T("top")}, {DUI_T("width"), DUI_T("300")}, {DUI_T("height"), DUI_T("300")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("green")}, {DUI_T("bkcolor"), DUI_T("Olive")}});
    ui::Attach(scroll_box1, label161);

    auto* label162 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("float 301*301 center,center")}, {DUI_T("float"), DUI_T("true")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("single_line"), DUI_T("false")}, {DUI_T("width"), DUI_T("301")}, {DUI_T("height"), DUI_T("301")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("green")}, {DUI_T("bkcolor"), DUI_T("LemonChiffon")}});
    ui::Attach(scroll_box1, label162);

    auto* label163 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("float 302*302 center,bottom")}, {DUI_T("float"), DUI_T("true")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("single_line"), DUI_T("false")}, {DUI_T("width"), DUI_T("302")}, {DUI_T("height"), DUI_T("302")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("green")}, {DUI_T("bkcolor"), DUI_T("LemonChiffon")}});
    ui::Attach(scroll_box1, label163);

    ui::Attach(vbox18, scroll_box1);
    ui::Attach(tab_box1, vbox18);

    auto* vbox19 = ui::Create<ui::VBox>(this, {{DUI_T("name"), DUI_T("main_view_page_07")}});
    auto* label174 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("HScrollBox Container Layout Test")}});
    ui::Attach(vbox19, label174);

    auto* hscroll_box1 = ui::Create<ui::HScrollBox>(this, {{DUI_T("vscrollbar"), DUI_T("true")}, {DUI_T("hscrollbar"), DUI_T("true")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("bkcolor"), DUI_T("white")}, {DUI_T("border_size"), DUI_T("2")}, {DUI_T("border_color"), DUI_T("red")}, {DUI_T("padding"), DUI_T("20,20,20,20")}});
    auto* label165 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hscroll_box")}, {DUI_T("text"), DUI_T("1 right,top")}, {DUI_T("text_align"), DUI_T("right,bottom")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("height"), DUI_T("80")}});
    ui::Attach(hscroll_box1, label165);

    auto* label166 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hscroll_box")}, {DUI_T("text"), DUI_T("2 left,top")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("width"), DUI_T("500")}, {DUI_T("height"), DUI_T("280")}});
    ui::Attach(hscroll_box1, label166);

    auto* label167 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hscroll_box")}, {DUI_T("text"), DUI_T("3 left,center")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("width"), DUI_T("500")}, {DUI_T("height"), DUI_T("280")}});
    ui::Attach(hscroll_box1, label167);

    auto* label168 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hscroll_box")}, {DUI_T("text"), DUI_T("4 left,bottom")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("width"), DUI_T("500")}, {DUI_T("height"), DUI_T("280")}});
    ui::Attach(hscroll_box1, label168);

    auto* label169 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hscroll_box")}, {DUI_T("text"), DUI_T("5 center,top")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("width"), DUI_T("500")}, {DUI_T("height"), DUI_T("280")}});
    ui::Attach(hscroll_box1, label169);

    auto* label170 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hscroll_box")}, {DUI_T("text"), DUI_T("6 center,center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("width"), DUI_T("500")}, {DUI_T("height"), DUI_T("256")}});
    ui::Attach(hscroll_box1, label170);

    auto* label171 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hscroll_box")}, {DUI_T("text"), DUI_T("7 center,bottom")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("width"), DUI_T("500")}, {DUI_T("height"), DUI_T("280")}});
    ui::Attach(hscroll_box1, label171);

    auto* label172 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hscroll_box")}, {DUI_T("text"), DUI_T("8 right,top")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("width"), DUI_T("500")}, {DUI_T("height"), DUI_T("280")}});
    ui::Attach(hscroll_box1, label172);

    auto* label173 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hscroll_box")}, {DUI_T("text"), DUI_T("9 right,center")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("width"), DUI_T("500")}, {DUI_T("height"), DUI_T("4000")}});
    ui::Attach(hscroll_box1, label173);

    ui::Attach(vbox19, hscroll_box1);
    ui::Attach(tab_box1, vbox19);

    auto* vbox20 = ui::Create<ui::VBox>(this, {{DUI_T("name"), DUI_T("main_view_page_08")}});
    auto* label184 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("VScrollBox Container Layout Test")}});
    ui::Attach(vbox20, label184);

    auto* vscroll_box1 = ui::Create<ui::VScrollBox>(this, {{DUI_T("vscrollbar"), DUI_T("true")}, {DUI_T("hscrollbar"), DUI_T("true")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("bkcolor"), DUI_T("white")}, {DUI_T("border_size"), DUI_T("2")}, {DUI_T("border_color"), DUI_T("red")}, {DUI_T("padding"), DUI_T("20,20,20,20")}});
    auto* label175 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vscroll_box")}, {DUI_T("text"), DUI_T("1 right,top")}, {DUI_T("text_align"), DUI_T("right,bottom")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("height"), DUI_T("80")}});
    ui::Attach(vscroll_box1, label175);

    auto* label176 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vscroll_box")}, {DUI_T("text"), DUI_T("2 left,top")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("width"), DUI_T("500")}, {DUI_T("height"), DUI_T("280")}});
    ui::Attach(vscroll_box1, label176);

    auto* label177 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vscroll_box")}, {DUI_T("text"), DUI_T("3 left,center")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("width"), DUI_T("500")}, {DUI_T("height"), DUI_T("280")}});
    ui::Attach(vscroll_box1, label177);

    auto* label178 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vscroll_box")}, {DUI_T("text"), DUI_T("4 left,bottom")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("width"), DUI_T("500")}, {DUI_T("height"), DUI_T("280")}});
    ui::Attach(vscroll_box1, label178);

    auto* label179 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vscroll_box")}, {DUI_T("text"), DUI_T("5 center,top")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("width"), DUI_T("500")}, {DUI_T("height"), DUI_T("280")}});
    ui::Attach(vscroll_box1, label179);

    auto* label180 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vscroll_box")}, {DUI_T("text"), DUI_T("6 center,center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("width"), DUI_T("500")}, {DUI_T("height"), DUI_T("256")}});
    ui::Attach(vscroll_box1, label180);

    auto* label181 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vscroll_box")}, {DUI_T("text"), DUI_T("7 center,bottom")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("width"), DUI_T("500")}, {DUI_T("height"), DUI_T("280")}});
    ui::Attach(vscroll_box1, label181);

    auto* label182 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vscroll_box")}, {DUI_T("text"), DUI_T("8 right,top")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("width"), DUI_T("500")}, {DUI_T("height"), DUI_T("280")}});
    ui::Attach(vscroll_box1, label182);

    auto* label183 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vscroll_box")}, {DUI_T("text"), DUI_T("9 right,center")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("width"), DUI_T("5000")}, {DUI_T("height"), DUI_T("400")}});
    ui::Attach(vscroll_box1, label183);

    auto* vlist_box1 = ui::Create<ui::VListBox>(this, {{DUI_T("class"), DUI_T("list")}, {DUI_T("height"), DUI_T("100")}, {DUI_T("width"), DUI_T("200")}, {DUI_T("margin"), DUI_T("0,4,0,0")}, {DUI_T("padding"), DUI_T("5,3,5,3")}, {DUI_T("border_color"), DUI_T("gray")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("tooltiptext"), DUI_T("ui::VListBox")}});
    auto* list_box_item1 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("VListBox::ListBoxItem")}});
    ui::Attach(vlist_box1, list_box_item1);

    auto* list_box_item2 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("VListBox::ListBoxItem")}});
    ui::Attach(vlist_box1, list_box_item2);

    auto* list_box_item3 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("VListBox::ListBoxItem")}});
    ui::Attach(vlist_box1, list_box_item3);

    auto* list_box_item4 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("VListBox::ListBoxItem")}});
    ui::Attach(vlist_box1, list_box_item4);

    auto* list_box_item5 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("VListBox::ListBoxItem")}});
    ui::Attach(vlist_box1, list_box_item5);

    auto* list_box_item6 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("VListBox::ListBoxItem")}});
    ui::Attach(vlist_box1, list_box_item6);

    auto* list_box_item7 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("VListBox::ListBoxItem")}});
    ui::Attach(vlist_box1, list_box_item7);

    auto* list_box_item8 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("VListBox::ListBoxItem")}});
    ui::Attach(vlist_box1, list_box_item8);

    auto* list_box_item9 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("VListBox::ListBoxItem")}});
    ui::Attach(vlist_box1, list_box_item9);

    auto* list_box_item10 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("VListBox::ListBoxItem")}});
    ui::Attach(vlist_box1, list_box_item10);

    auto* list_box_item11 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("VListBox::ListBoxItem")}});
    ui::Attach(vlist_box1, list_box_item11);

    ui::Attach(vscroll_box1, vlist_box1);

    auto* hbox17 = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("120")}, {DUI_T("margin"), DUI_T("0,4,0,0")}, {DUI_T("child_margin"), DUI_T("2")}});
    auto* vlist_box2 = ui::Create<ui::VListBox>(this, {{DUI_T("class"), DUI_T("list")}, {DUI_T("height"), DUI_T("100")}, {DUI_T("width"), DUI_T("200")}, {DUI_T("padding"), DUI_T("5,3,5,3")}, {DUI_T("border_color"), DUI_T("gray")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("tooltiptext"), DUI_T("ui::VListBox")}});
    auto* list_box_item12 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("VListBox::ListBoxItem")}});
    ui::Attach(vlist_box2, list_box_item12);

    auto* list_box_item13 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("VListBox::ListBoxItem")}});
    ui::Attach(vlist_box2, list_box_item13);

    auto* list_box_item14 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("VListBox::ListBoxItem")}});
    ui::Attach(vlist_box2, list_box_item14);

    auto* list_box_item15 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("VListBox::ListBoxItem")}});
    ui::Attach(vlist_box2, list_box_item15);

    auto* list_box_item16 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("VListBox::ListBoxItem")}});
    ui::Attach(vlist_box2, list_box_item16);

    auto* list_box_item17 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("VListBox::ListBoxItem")}});
    ui::Attach(vlist_box2, list_box_item17);

    auto* list_box_item18 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("VListBox::ListBoxItem")}});
    ui::Attach(vlist_box2, list_box_item18);

    auto* list_box_item19 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("VListBox::ListBoxItem")}});
    ui::Attach(vlist_box2, list_box_item19);

    auto* list_box_item20 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("VListBox::ListBoxItem")}});
    ui::Attach(vlist_box2, list_box_item20);

    auto* list_box_item21 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("VListBox::ListBoxItem")}});
    ui::Attach(vlist_box2, list_box_item21);

    auto* list_box_item22 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("VListBox::ListBoxItem")}});
    ui::Attach(vlist_box2, list_box_item22);

    ui::Attach(hbox17, vlist_box2);

    auto* hlist_box1 = ui::Create<ui::HListBox>(this, {{DUI_T("class"), DUI_T("list")}, {DUI_T("height"), DUI_T("50")}, {DUI_T("width"), DUI_T("600")}, {DUI_T("hscrollbar"), DUI_T("true")}, {DUI_T("padding"), DUI_T("5,3,5,3")}, {DUI_T("border_color"), DUI_T("gray")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("tooltiptext"), DUI_T("ui::HListBox")}});
    auto* list_box_item23 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("40")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("text"), DUI_T("HListBox::ListBoxItem")}});
    ui::Attach(hlist_box1, list_box_item23);

    auto* list_box_item24 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("40")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("text"), DUI_T("HListBox::ListBoxItem")}});
    ui::Attach(hlist_box1, list_box_item24);

    auto* list_box_item25 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("40")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("text"), DUI_T("HListBox::ListBoxItem")}});
    ui::Attach(hlist_box1, list_box_item25);

    auto* list_box_item26 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("40")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("text"), DUI_T("HListBox::ListBoxItem")}});
    ui::Attach(hlist_box1, list_box_item26);

    auto* list_box_item27 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("40")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("text"), DUI_T("HListBox::ListBoxItem")}});
    ui::Attach(hlist_box1, list_box_item27);

    auto* list_box_item28 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("40")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("text"), DUI_T("HListBox::ListBoxItem")}});
    ui::Attach(hlist_box1, list_box_item28);

    auto* list_box_item29 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("40")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("text"), DUI_T("HListBox::ListBoxItem")}});
    ui::Attach(hlist_box1, list_box_item29);

    auto* list_box_item30 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("40")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("text"), DUI_T("HListBox::ListBoxItem")}});
    ui::Attach(hlist_box1, list_box_item30);

    auto* list_box_item31 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("40")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("text"), DUI_T("HListBox::ListBoxItem")}});
    ui::Attach(hlist_box1, list_box_item31);

    auto* list_box_item32 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("40")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("text"), DUI_T("HListBox::ListBoxItem")}});
    ui::Attach(hlist_box1, list_box_item32);

    auto* list_box_item33 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("40")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("text"), DUI_T("HListBox::ListBoxItem")}});
    ui::Attach(hlist_box1, list_box_item33);

    ui::Attach(hbox17, hlist_box1);

    ui::Attach(vscroll_box1, hbox17);

    ui::Attach(vbox20, vscroll_box1);
    ui::Attach(tab_box1, vbox20);

    auto* vbox21 = ui::Create<ui::VBox>(this, {{DUI_T("name"), DUI_T("main_view_page_09")}});
    auto* vbox22 = ui::Create<ui::VBox>(this, {});
    auto* hflow_scroll_box1 = ui::Create<ui::HFlowScrollBox>(this, {{DUI_T("name"), DUI_T("hflow_scrollbox_test")}, {DUI_T("vscrollbar"), DUI_T("true")}, {DUI_T("hscrollbar"), DUI_T("true")}, {DUI_T("child_halign"), DUI_T("left")}, {DUI_T("child_valign"), DUI_T("top")}, {DUI_T("bkcolor"), DUI_T("Cornsilk")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("red")}, {DUI_T("padding"), DUI_T("8,8,8,8")}, {DUI_T("margin"), DUI_T("8,8,8,8")}, {DUI_T("height"), DUI_T("70%")}});
    auto* label185 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflow_scrollbox")}, {DUI_T("text"), DUI_T("1 top")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("minwidth"), DUI_T("300")}, {DUI_T("maxheight"), DUI_T("60")}});
    ui::Attach(hflow_scroll_box1, label185);

    auto* label186 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflow_scrollbox")}, {DUI_T("text"), DUI_T("2 enter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hflow_scroll_box1, label186);

    auto* label187 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflow_scrollbox")}, {DUI_T("text"), DUI_T("3 bottom")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("height"), DUI_T("40")}});
    ui::Attach(hflow_scroll_box1, label187);

    auto* label188 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflow_scrollbox")}, {DUI_T("text"), DUI_T("4 top")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(hflow_scroll_box1, label188);

    auto* label189 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflow_scrollbox")}, {DUI_T("text"), DUI_T("5 center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("height"), DUI_T("40")}});
    ui::Attach(hflow_scroll_box1, label189);

    auto* label190 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflow_scrollbox")}, {DUI_T("text"), DUI_T("6 bottom")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("bottom")}});
    ui::Attach(hflow_scroll_box1, label190);

    auto* label191 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflow_scrollbox")}, {DUI_T("text"), DUI_T("7 top")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("height"), DUI_T("40")}});
    ui::Attach(hflow_scroll_box1, label191);

    auto* label192 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflow_scrollbox")}, {DUI_T("text"), DUI_T("8 center")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hflow_scroll_box1, label192);

    auto* label193 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflow_scrollbox")}, {DUI_T("text"), DUI_T("9 bottom")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("width"), DUI_T("300")}});
    ui::Attach(hflow_scroll_box1, label193);

    auto* label194 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflow_scrollbox")}, {DUI_T("text"), DUI_T("10 top")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("minwidth"), DUI_T("300")}, {DUI_T("maxheight"), DUI_T("60")}});
    ui::Attach(hflow_scroll_box1, label194);

    auto* label195 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflow_scrollbox")}, {DUI_T("text"), DUI_T("11 center stretch: 100%")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("width"), DUI_T("stretch")}});
    ui::Attach(hflow_scroll_box1, label195);

    auto* label196 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflow_scrollbox")}, {DUI_T("text"), DUI_T("12 bottom")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("height"), DUI_T("40")}});
    ui::Attach(hflow_scroll_box1, label196);

    auto* label197 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflow_scrollbox")}, {DUI_T("text"), DUI_T("13 top stretch: 50%")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("width"), DUI_T("50%")}});
    ui::Attach(hflow_scroll_box1, label197);

    auto* label198 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflow_scrollbox")}, {DUI_T("text"), DUI_T("12 center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("height"), DUI_T("40")}});
    ui::Attach(hflow_scroll_box1, label198);

    auto* label199 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflow_scrollbox")}, {DUI_T("text"), DUI_T("15 bottom")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("bottom")}});
    ui::Attach(hflow_scroll_box1, label199);

    auto* label200 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflow_scrollbox")}, {DUI_T("text"), DUI_T("16 top")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("height"), DUI_T("40")}});
    ui::Attach(hflow_scroll_box1, label200);

    auto* label201 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflow_scrollbox")}, {DUI_T("text"), DUI_T("17 center")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hflow_scroll_box1, label201);

    auto* label202 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflow_scrollbox")}, {DUI_T("text"), DUI_T("18 bottom")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("width"), DUI_T("300")}});
    ui::Attach(hflow_scroll_box1, label202);

    auto* label203 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflow_scrollbox")}, {DUI_T("text"), DUI_T("19 center, width: 1600")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("width"), DUI_T("1600")}});
    ui::Attach(hflow_scroll_box1, label203);

    auto* label204 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_hflow_scrollbox")}, {DUI_T("text"), DUI_T("20 top")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("height"), DUI_T("40")}});
    ui::Attach(hflow_scroll_box1, label204);

    ui::Attach(vbox22, hflow_scroll_box1);

    auto* vbox23 = ui::Create<ui::VBox>(this, {{DUI_T("height"), DUI_T("84")}, {DUI_T("padding"), DUI_T("6,0,6,0")}});
    auto* label205 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("HFlowScrollBox Container Layout Test")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vbox23, label205);

    auto* hbox18 = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("28")}});
    auto* label206 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Container content horizontal alignment:")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hbox18, label206);

    auto* option19 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("hflow_scrollbox_group_halign")}, {DUI_T("text"), DUI_T("Align Left (left)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("true")}});
    ui::Attach(hbox18, option19);

    auto* option20 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("hflow_scrollbox_group_halign")}, {DUI_T("text"), DUI_T("Align Center (center)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("false")}});
    ui::Attach(hbox18, option20);

    auto* option21 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("hflow_scrollbox_group_halign")}, {DUI_T("text"), DUI_T("Align Right (right)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("false")}});
    ui::Attach(hbox18, option21);

    auto* label207 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("(aligned within the same row)")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hbox18, label207);

    ui::Attach(vbox23, hbox18);

    auto* hbox19 = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("28")}});
    auto* label208 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Container content vertical alignment:")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hbox19, label208);

    auto* option22 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("hflow_scrollbox_group_valign")}, {DUI_T("text"), DUI_T("Align Top (top)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("true")}});
    ui::Attach(hbox19, option22);

    auto* option23 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("hflow_scrollbox_group_valign")}, {DUI_T("text"), DUI_T("Align Center (center)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("false")}});
    ui::Attach(hbox19, option23);

    auto* option24 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("hflow_scrollbox_group_valign")}, {DUI_T("text"), DUI_T("Align Bottom (bottom)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("false")}});
    ui::Attach(hbox19, option24);

    auto* label209 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("(effective only when no scrollbar appears)")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hbox19, label209);

    ui::Attach(vbox23, hbox19);

    ui::Attach(vbox21, vbox23);
    ui::Attach(vbox21, vbox22);
    ui::Attach(tab_box1, vbox21);

    auto* vbox24 = ui::Create<ui::VBox>(this, {{DUI_T("name"), DUI_T("main_view_page_10")}});
    auto* vbox25 = ui::Create<ui::VBox>(this, {});
    auto* vflow_scroll_box1 = ui::Create<ui::VFlowScrollBox>(this, {{DUI_T("name"), DUI_T("vflow_scrollbox_test")}, {DUI_T("vscrollbar"), DUI_T("true")}, {DUI_T("hscrollbar"), DUI_T("true")}, {DUI_T("child_halign"), DUI_T("left")}, {DUI_T("child_valign"), DUI_T("top")}, {DUI_T("bkcolor"), DUI_T("Cornsilk")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("red")}, {DUI_T("padding"), DUI_T("8,8,8,8")}, {DUI_T("margin"), DUI_T("8,8,8,8")}, {DUI_T("height"), DUI_T("70%")}});
    auto* label210 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflow_scrollbox")}, {DUI_T("text"), DUI_T("1 left")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("minwidth"), DUI_T("300")}, {DUI_T("maxheight"), DUI_T("60")}});
    ui::Attach(vflow_scroll_box1, label210);

    auto* label211 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflow_scrollbox")}, {DUI_T("text"), DUI_T("2 center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(vflow_scroll_box1, label211);

    auto* label212 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflow_scrollbox")}, {DUI_T("text"), DUI_T("3 right")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("height"), DUI_T("40")}});
    ui::Attach(vflow_scroll_box1, label212);

    auto* label213 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflow_scrollbox")}, {DUI_T("text"), DUI_T("4 left")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vflow_scroll_box1, label213);

    auto* label214 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflow_scrollbox")}, {DUI_T("text"), DUI_T("5 center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("height"), DUI_T("40")}});
    ui::Attach(vflow_scroll_box1, label214);

    auto* label215 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflow_scrollbox")}, {DUI_T("text"), DUI_T("6 right")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vflow_scroll_box1, label215);

    auto* label216 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflow_scrollbox")}, {DUI_T("text"), DUI_T("7 left")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("height"), DUI_T("40")}});
    ui::Attach(vflow_scroll_box1, label216);

    auto* label217 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflow_scrollbox")}, {DUI_T("text"), DUI_T("8 center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("bottom")}});
    ui::Attach(vflow_scroll_box1, label217);

    auto* label218 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflow_scrollbox")}, {DUI_T("text"), DUI_T("9 right")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("width"), DUI_T("300")}});
    ui::Attach(vflow_scroll_box1, label218);

    auto* label219 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflow_scrollbox")}, {DUI_T("text"), DUI_T("10 left")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("minwidth"), DUI_T("300")}, {DUI_T("maxheight"), DUI_T("60")}});
    ui::Attach(vflow_scroll_box1, label219);

    auto* label220 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflow_scrollbox")}, {DUI_T("text"), DUI_T("11 center stretch: 100%")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("width"), DUI_T("180")}});
    ui::Attach(vflow_scroll_box1, label220);

    auto* label221 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflow_scrollbox")}, {DUI_T("text"), DUI_T("12 right")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("height"), DUI_T("40")}});
    ui::Attach(vflow_scroll_box1, label221);

    auto* label222 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflow_scrollbox")}, {DUI_T("text"), DUI_T("13 left stretch: 50%")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("height"), DUI_T("50%")}, {DUI_T("width"), DUI_T("180")}});
    ui::Attach(vflow_scroll_box1, label222);

    auto* label223 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflow_scrollbox")}, {DUI_T("text"), DUI_T("12 center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("height"), DUI_T("40")}});
    ui::Attach(vflow_scroll_box1, label223);

    auto* label224 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflow_scrollbox")}, {DUI_T("text"), DUI_T("15 left")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("bottom")}});
    ui::Attach(vflow_scroll_box1, label224);

    auto* label225 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflow_scrollbox")}, {DUI_T("text"), DUI_T("16 center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("height"), DUI_T("40")}});
    ui::Attach(vflow_scroll_box1, label225);

    auto* label226 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflow_scrollbox")}, {DUI_T("text"), DUI_T("17 right")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vflow_scroll_box1, label226);

    auto* label227 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflow_scrollbox")}, {DUI_T("text"), DUI_T("18 left")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("width"), DUI_T("300")}});
    ui::Attach(vflow_scroll_box1, label227);

    auto* label228 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflow_scrollbox")}, {DUI_T("text"), DUI_T("19 center, height: 600")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("height"), DUI_T("600")}, {DUI_T("width"), DUI_T("180")}});
    ui::Attach(vflow_scroll_box1, label228);

    auto* label229 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_vflow_scrollbox")}, {DUI_T("text"), DUI_T("20 right")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("height"), DUI_T("40")}});
    ui::Attach(vflow_scroll_box1, label229);

    ui::Attach(vbox25, vflow_scroll_box1);

    auto* vbox26 = ui::Create<ui::VBox>(this, {{DUI_T("height"), DUI_T("84")}, {DUI_T("padding"), DUI_T("6,0,6,0")}});
    auto* label230 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("VFlowScrollBox Container Layout Test")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vbox26, label230);

    auto* hbox20 = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("28")}});
    auto* label231 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Container content horizontal alignment:")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hbox20, label231);

    auto* option25 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("vflow_scrollbox_group_halign")}, {DUI_T("text"), DUI_T("Align Left (left)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("true")}});
    ui::Attach(hbox20, option25);

    auto* option26 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("vflow_scrollbox_group_halign")}, {DUI_T("text"), DUI_T("Align Center (center)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("false")}});
    ui::Attach(hbox20, option26);

    auto* option27 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("vflow_scrollbox_group_halign")}, {DUI_T("text"), DUI_T("Align Right (right)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("false")}});
    ui::Attach(hbox20, option27);

    auto* label232 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("(effective only when no scrollbar appears)")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hbox20, label232);

    ui::Attach(vbox26, hbox20);

    auto* hbox21 = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("28")}});
    auto* label233 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Container content vertical alignment:")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hbox21, label233);

    auto* option28 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("vflow_scrollbox_group_valign")}, {DUI_T("text"), DUI_T("Align Top (top)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("true")}});
    ui::Attach(hbox21, option28);

    auto* option29 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("vflow_scrollbox_group_valign")}, {DUI_T("text"), DUI_T("Align Center (center)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("false")}});
    ui::Attach(hbox21, option29);

    auto* option30 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("vflow_scrollbox_group_valign")}, {DUI_T("text"), DUI_T("Align Bottom (bottom)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("false")}});
    ui::Attach(hbox21, option30);

    auto* label234 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("(aligned within the same column)")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hbox21, label234);

    ui::Attach(vbox26, hbox21);

    ui::Attach(vbox24, vbox26);
    ui::Attach(vbox24, vbox25);
    ui::Attach(tab_box1, vbox24);

    auto* vbox27 = ui::Create<ui::VBox>(this, {{DUI_T("name"), DUI_T("main_view_page_11")}});

    auto* label268 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("VTileScrollBox Container Layout Test")}});
    ui::Attach(vbox27, label268);

    auto* vtile_scroll_box1 = ui::Create<ui::VTileScrollBox>(this, {{DUI_T("hscrollbar"), DUI_T("true")}, {DUI_T("vscrollbar"), DUI_T("true")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("400")}, {DUI_T("bkcolor"), DUI_T("white")}, {DUI_T("border_size"), DUI_T("2")}, {DUI_T("border_color"), DUI_T("red")}, {DUI_T("padding"), DUI_T("20,20,20,20")}});
    auto* label252 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_scroll_box")}, {DUI_T("text"), DUI_T("1 right,top")}, {DUI_T("text_align"), DUI_T("right,bottom")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(vtile_scroll_box1, label252);

    auto* label253 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_scroll_box")}, {DUI_T("text"), DUI_T("2 left,top")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(vtile_scroll_box1, label253);

    auto* label254 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_scroll_box")}, {DUI_T("text"), DUI_T("3 left,center")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vtile_scroll_box1, label254);

    auto* label255 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_scroll_box")}, {DUI_T("text"), DUI_T("4 left,bottom")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("bottom")}});
    ui::Attach(vtile_scroll_box1, label255);

    auto* label256 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_scroll_box")}, {DUI_T("text"), DUI_T("5 center,top")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(vtile_scroll_box1, label256);

    auto* label257 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_scroll_box")}, {DUI_T("text"), DUI_T("6 center,center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vtile_scroll_box1, label257);

    auto* label258 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_scroll_box")}, {DUI_T("text"), DUI_T("7 center,bottom")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("bottom")}});
    ui::Attach(vtile_scroll_box1, label258);

    auto* label259 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_scroll_box")}, {DUI_T("text"), DUI_T("8 right,top")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(vtile_scroll_box1, label259);

    auto* label260 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_scroll_box")}, {DUI_T("text"), DUI_T("9 right,center")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vtile_scroll_box1, label260);

    auto* label261 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_scroll_box")}, {DUI_T("text"), DUI_T("10 center,center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vtile_scroll_box1, label261);

    auto* label262 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_scroll_box")}, {DUI_T("text"), DUI_T("11 center,center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vtile_scroll_box1, label262);

    auto* label263 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_scroll_box")}, {DUI_T("text"), DUI_T("12 center,center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vtile_scroll_box1, label263);

    auto* label264 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_scroll_box")}, {DUI_T("text"), DUI_T("13 center,center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vtile_scroll_box1, label264);

    auto* label265 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_scroll_box")}, {DUI_T("text"), DUI_T("14 center,center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vtile_scroll_box1, label265);

    auto* label266 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_scroll_box")}, {DUI_T("text"), DUI_T("15 center,center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vtile_scroll_box1, label266);

    auto* label267 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_scroll_box")}, {DUI_T("text"), DUI_T("16 center,center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vtile_scroll_box1, label267);

    ui::Attach(vbox27, vtile_scroll_box1);

    auto* label251 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("HTileScrollBox Container Layout Test")}, {DUI_T("margin"), DUI_T("0,4,0,4")}});
    ui::Attach(vbox27, label251);

    auto* htile_scroll_box1 = ui::Create<ui::HTileScrollBox>(this, {{DUI_T("hscrollbar"), DUI_T("true")}, {DUI_T("vscrollbar"), DUI_T("false")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("380")}, {DUI_T("bkcolor"), DUI_T("white")}, {DUI_T("border_size"), DUI_T("2")}, {DUI_T("border_color"), DUI_T("red")}, {DUI_T("padding"), DUI_T("20,20,20,20")}});
    auto* label235 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_scroll_box")}, {DUI_T("text"), DUI_T("1 right,top")}, {DUI_T("text_align"), DUI_T("right,bottom")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(htile_scroll_box1, label235);

    auto* label236 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_scroll_box")}, {DUI_T("text"), DUI_T("2 left,top")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(htile_scroll_box1, label236);

    auto* label237 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_scroll_box")}, {DUI_T("text"), DUI_T("3 left,center")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(htile_scroll_box1, label237);

    auto* label238 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_scroll_box")}, {DUI_T("text"), DUI_T("4 left,bottom")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("bottom")}});
    ui::Attach(htile_scroll_box1, label238);

    auto* label239 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_scroll_box")}, {DUI_T("text"), DUI_T("5 center,top")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(htile_scroll_box1, label239);

    auto* label240 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_scroll_box")}, {DUI_T("text"), DUI_T("6 center,center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(htile_scroll_box1, label240);

    auto* label241 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_scroll_box")}, {DUI_T("text"), DUI_T("7 center,bottom")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("bottom")}});
    ui::Attach(htile_scroll_box1, label241);

    auto* label242 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_scroll_box")}, {DUI_T("text"), DUI_T("8 right,top")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(htile_scroll_box1, label242);

    auto* label243 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_scroll_box")}, {DUI_T("text"), DUI_T("9 right,center")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(htile_scroll_box1, label243);

    auto* label244 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_scroll_box")}, {DUI_T("text"), DUI_T("10 center,center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(htile_scroll_box1, label244);

    auto* label245 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_scroll_box")}, {DUI_T("text"), DUI_T("11 center,center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(htile_scroll_box1, label245);

    auto* label246 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_scroll_box")}, {DUI_T("text"), DUI_T("12 center,center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(htile_scroll_box1, label246);

    auto* label247 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_scroll_box")}, {DUI_T("text"), DUI_T("13 center,center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(htile_scroll_box1, label247);

    auto* label248 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_scroll_box")}, {DUI_T("text"), DUI_T("14 center,center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(htile_scroll_box1, label248);

    auto* label249 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_scroll_box")}, {DUI_T("text"), DUI_T("15 center,center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(htile_scroll_box1, label249);

    auto* label250 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_tile_scroll_box")}, {DUI_T("text"), DUI_T("16 center,center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(htile_scroll_box1, label250);

    ui::Attach(vbox27, htile_scroll_box1);
    ui::Attach(tab_box1, vbox27);

    auto* vbox28 = ui::Create<ui::VBox>(this, {{DUI_T("name"), DUI_T("main_view_page_12")}});
    auto* hbox22 = ui::Create<ui::HBox>(this, {{DUI_T("width"), DUI_T("auto")}});
    auto* vbox29 = ui::Create<ui::VBox>(this, {{DUI_T("width"), DUI_T("auto")}, {DUI_T("height"), DUI_T("240")}});
    auto* label269 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T(" VListBox Container Layout Test")}});
    ui::Attach(vbox29, label269);

    auto* vlist_box3 = ui::Create<ui::VListBox>(this, {{DUI_T("class"), DUI_T("page_list_box")}, {DUI_T("width"), DUI_T("auto")}, {DUI_T("height"), DUI_T("200")}});
    auto* list_box_item34 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("VListBox Test 01")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vlist_box3, list_box_item34);

    auto* list_box_item35 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("VListBox Test 02")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vlist_box3, list_box_item35);

    auto* list_box_item36 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("VListBox Test 03")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vlist_box3, list_box_item36);

    auto* list_box_item37 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("VListBox Test 04")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vlist_box3, list_box_item37);

    auto* list_box_item38 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("VListBox Test 05")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vlist_box3, list_box_item38);

    auto* list_box_item39 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("VListBox Test 06")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vlist_box3, list_box_item39);

    auto* list_box_item40 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("VListBox Test 07")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vlist_box3, list_box_item40);

    auto* list_box_item41 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("VListBox Test 08")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vlist_box3, list_box_item41);

    auto* list_box_item42 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("VListBox Test 09")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vlist_box3, list_box_item42);

    auto* list_box_item43 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("VListBox Test 10")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vlist_box3, list_box_item43);

    auto* list_box_item44 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("VListBox Test 11")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vlist_box3, list_box_item44);

    auto* list_box_item45 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("VListBox Test 12")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vlist_box3, list_box_item45);

    ui::Attach(vbox29, vlist_box3);

    ui::Attach(hbox22, vbox29);

    auto* vbox30 = ui::Create<ui::VBox>(this, {{DUI_T("width"), DUI_T("auto")}});
    auto* label270 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T(" VTileListBox Container Layout Test")}});
    ui::Attach(vbox30, label270);

    auto* vtile_list_box1 = ui::Create<ui::VTileListBox>(this, {{DUI_T("class"), DUI_T("page_list_box")}, {DUI_T("columns"), DUI_T("1")}, {DUI_T("width"), DUI_T("auto")}, {DUI_T("height"), DUI_T("200")}});
    auto* list_box_item46 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("VListBox Test 01")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vtile_list_box1, list_box_item46);

    auto* list_box_item47 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("VListBox Test 02")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vtile_list_box1, list_box_item47);

    auto* list_box_item48 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("VListBox Test 03")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vtile_list_box1, list_box_item48);

    auto* list_box_item49 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("VListBox Test 04")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vtile_list_box1, list_box_item49);

    auto* list_box_item50 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("VListBox Test 05")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vtile_list_box1, list_box_item50);

    auto* list_box_item51 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("VListBox Test 06")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vtile_list_box1, list_box_item51);

    auto* list_box_item52 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("VListBox Test 07")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vtile_list_box1, list_box_item52);

    auto* list_box_item53 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("VListBox Test 08")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vtile_list_box1, list_box_item53);

    auto* list_box_item54 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("VListBox Test 09")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vtile_list_box1, list_box_item54);

    auto* list_box_item55 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("VListBox Test 10")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vtile_list_box1, list_box_item55);

    auto* list_box_item56 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("VListBox Test 11")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vtile_list_box1, list_box_item56);

    auto* list_box_item57 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("VListBox Test 12")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vtile_list_box1, list_box_item57);

    auto* list_box_item58 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("VListBox Test 13")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vtile_list_box1, list_box_item58);

    auto* list_box_item59 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("VListBox Test 14")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vtile_list_box1, list_box_item59);

    auto* list_box_item60 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("VListBox Test 15")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vtile_list_box1, list_box_item60);

    auto* list_box_item61 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("VListBox Test 16")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vtile_list_box1, list_box_item61);

    ui::Attach(vbox30, vtile_list_box1);

    ui::Attach(hbox22, vbox30);

    auto* vbox31 = ui::Create<ui::VBox>(this, {{DUI_T("height"), DUI_T("auto")}});
    auto* label271 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T(" HListBox Container Layout Test")}});
    ui::Attach(vbox31, label271);

    auto* hlist_box2 = ui::Create<ui::HListBox>(this, {{DUI_T("class"), DUI_T("page_list_box")}, {DUI_T("vscrollbar"), DUI_T("false")}, {DUI_T("hscrollbar"), DUI_T("true")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("50")}});
    auto* list_box_item62 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("HListBox Test 01")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hlist_box2, list_box_item62);

    auto* list_box_item63 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("HListBox Test 02")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hlist_box2, list_box_item63);

    auto* list_box_item64 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("HListBox Test 03")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hlist_box2, list_box_item64);

    auto* list_box_item65 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("HListBox Test 04")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hlist_box2, list_box_item65);

    auto* list_box_item66 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("HListBox Test 05")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hlist_box2, list_box_item66);

    auto* list_box_item67 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("HListBox Test 06")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hlist_box2, list_box_item67);

    auto* list_box_item68 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("HListBox Test 07")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hlist_box2, list_box_item68);

    auto* list_box_item69 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("HListBox Test 08")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hlist_box2, list_box_item69);

    auto* list_box_item70 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("HListBox Test 09")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hlist_box2, list_box_item70);

    auto* list_box_item71 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("HListBox Test 10")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hlist_box2, list_box_item71);

    auto* list_box_item72 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("HListBox Test 11")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hlist_box2, list_box_item72);

    auto* list_box_item73 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("HListBox Test 12")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hlist_box2, list_box_item73);

    auto* list_box_item74 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("HListBox Test 13")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hlist_box2, list_box_item74);

    auto* list_box_item75 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("HListBox Test 14")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hlist_box2, list_box_item75);

    auto* list_box_item76 = ui::Create<ui::ListBoxItem>(this, {{DUI_T("class"), DUI_T("page_list_box_item")}, {DUI_T("text"), DUI_T("HListBox Test 15")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hlist_box2, list_box_item76);

    ui::Attach(vbox31, hlist_box2);

    ui::Attach(vbox28, vbox31);
    ui::Attach(vbox28, hbox22);
    ui::Attach(tab_box1, vbox28);

    auto* vbox32 = ui::Create<ui::VBox>(this, {{DUI_T("name"), DUI_T("main_view_page_13")}});
    auto* vbox33 = ui::Create<ui::VBox>(this, {});
    auto* box3 = ui::Create<ui::Box>(this, {{DUI_T("bkcolor"), DUI_T("red")}, {DUI_T("padding"), DUI_T("1,1,1,1")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("white")}});
    auto* label272 = ui::Create<ui::Label>(this, {{DUI_T("width"), DUI_T("80%")}, {DUI_T("height"), DUI_T("80%")}, {DUI_T("text"), DUI_T("Box: Label 1 width='80%' height='80%'")}, {DUI_T("bkcolor"), DUI_T("yellow")}, {DUI_T("text_align"), DUI_T("bottom")}});
    ui::Attach(box3, label272);

    auto* label273 = ui::Create<ui::Label>(this, {{DUI_T("width"), DUI_T("50%")}, {DUI_T("height"), DUI_T("50%")}, {DUI_T("text"), DUI_T("Box: Label 2 width='50%' height='50%'")}, {DUI_T("bkcolor"), DUI_T("orange")}});
    ui::Attach(box3, label273);

    ui::Attach(vbox33, box3);

    auto* vbox34 = ui::Create<ui::VBox>(this, {{DUI_T("bkcolor"), DUI_T("green")}, {DUI_T("padding"), DUI_T("1,1,1,1")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("white")}});
    auto* label274 = ui::Create<ui::Label>(this, {{DUI_T("width"), DUI_T("80%")}, {DUI_T("height"), DUI_T("30%")}, {DUI_T("text"), DUI_T("VBox: Label 1 width='80%' height='30%'")}, {DUI_T("bkcolor"), DUI_T("yellow")}, {DUI_T("text_align"), DUI_T("bottom")}});
    ui::Attach(vbox34, label274);

    auto* label275 = ui::Create<ui::Label>(this, {{DUI_T("width"), DUI_T("50%")}, {DUI_T("height"), DUI_T("70%")}, {DUI_T("text"), DUI_T("VBox: Label 2 width='50%' height='70%'")}, {DUI_T("bkcolor"), DUI_T("orange")}});
    ui::Attach(vbox34, label275);

    auto* label276 = ui::Create<ui::Label>(this, {{DUI_T("width"), DUI_T("100%")}, {DUI_T("height"), DUI_T("80")}, {DUI_T("text"), DUI_T("VBox: Label 3 width='100%' height='80'")}, {DUI_T("bkcolor"), DUI_T("light_gray")}});
    ui::Attach(vbox34, label276);

    ui::Attach(vbox33, vbox34);

    auto* hbox23 = ui::Create<ui::HBox>(this, {{DUI_T("bkcolor"), DUI_T("blue")}, {DUI_T("padding"), DUI_T("1,1,1,1")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("white")}});
    auto* label277 = ui::Create<ui::Label>(this, {{DUI_T("width"), DUI_T("70%")}, {DUI_T("height"), DUI_T("80%")}, {DUI_T("text"), DUI_T("HBox: Label 1 width='70%' height='80%'")}, {DUI_T("bkcolor"), DUI_T("yellow")}, {DUI_T("text_align"), DUI_T("bottom")}});
    ui::Attach(hbox23, label277);

    auto* label278 = ui::Create<ui::Label>(this, {{DUI_T("width"), DUI_T("30%")}, {DUI_T("height"), DUI_T("50%")}, {DUI_T("text"), DUI_T("HBox: Label 2 width='30%' height='50%'")}, {DUI_T("bkcolor"), DUI_T("orange")}});
    ui::Attach(hbox23, label278);

    auto* label279 = ui::Create<ui::Label>(this, {{DUI_T("width"), DUI_T("300")}, {DUI_T("height"), DUI_T("100%")}, {DUI_T("text"), DUI_T("VBox: Label 3 width='300' height='100%'")}, {DUI_T("bkcolor"), DUI_T("light_gray")}});
    ui::Attach(hbox23, label279);

    ui::Attach(vbox33, hbox23);

    ui::Attach(vbox32, vbox33);

    ui::Attach(tab_box1, vbox32);

    auto* vbox35 = ui::Create<ui::VBox>(this, {{DUI_T("name"), DUI_T("main_view_page_14")}});
    auto* tab_box2 = ui::Create<ui::TabBox>(this, {{DUI_T("name"), DUI_T("main_view_tab_test")}, {DUI_T("bkcolor"), DUI_T("white")}, {DUI_T("selected_id"), DUI_T("0")}});
    auto* vbox36 = ui::Create<ui::VBox>(this, {{DUI_T("bkcolor"), DUI_T("white")}});
    auto* label280 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Page 1: This is page 1")}});
    ui::Attach(vbox36, label280);

    auto* label281 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Window Shadow Function Test:")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("12,30,10,0")}});
    ui::Attach(vbox36, label281);

    auto* option31 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("checkbox_2")}, {DUI_T("group"), DUI_T("shadow_type")}, {DUI_T("text"), DUI_T("Shadow type: default, default shadow")}, {DUI_T("selected"), DUI_T("true")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("30,4,0,2")}});
    ui::Attach(vbox36, option31);

    auto* option32 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("checkbox_2")}, {DUI_T("group"), DUI_T("shadow_type")}, {DUI_T("text"), DUI_T("Shadow type: big, large shadow, square corners (suitable for normal windows)")}, {DUI_T("selected"), DUI_T("false")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("30,4,0,2")}});
    ui::Attach(vbox36, option32);

    auto* option33 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("checkbox_2")}, {DUI_T("group"), DUI_T("shadow_type")}, {DUI_T("text"), DUI_T("Shadow type: big_round, large shadow, rounded corners (suitable for normal windows)")}, {DUI_T("selected"), DUI_T("false")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("30,4,0,2")}});
    ui::Attach(vbox36, option33);

    auto* option34 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("checkbox_2")}, {DUI_T("group"), DUI_T("shadow_type")}, {DUI_T("text"), DUI_T("Shadow type: small, small shadow, square corners (suitable for normal windows)")}, {DUI_T("selected"), DUI_T("false")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("30,4,0,2")}});
    ui::Attach(vbox36, option34);

    auto* option35 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("checkbox_2")}, {DUI_T("group"), DUI_T("shadow_type")}, {DUI_T("text"), DUI_T("Shadow type: small_round, small shadow, rounded corners (suitable for normal windows)")}, {DUI_T("selected"), DUI_T("false")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("30,4,0,2")}});
    ui::Attach(vbox36, option35);

    auto* option36 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("checkbox_2")}, {DUI_T("group"), DUI_T("shadow_type")}, {DUI_T("text"), DUI_T("Shadow type: menu, small shadow, square corners (suitable for popup windows such as menus)")}, {DUI_T("selected"), DUI_T("false")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("30,4,0,2")}});
    ui::Attach(vbox36, option36);

    auto* option37 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("checkbox_2")}, {DUI_T("group"), DUI_T("shadow_type")}, {DUI_T("text"), DUI_T("Shadow type: menu_round, small shadow, rounded corners (suitable for popup windows such as menus)")}, {DUI_T("selected"), DUI_T("false")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("30,4,0,2")}});
    ui::Attach(vbox36, option37);

    auto* option38 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("checkbox_2")}, {DUI_T("group"), DUI_T("shadow_type")}, {DUI_T("text"), DUI_T("Shadow type: none, no shadow, with border, square corners")}, {DUI_T("selected"), DUI_T("false")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("30,4,0,2")}});
    ui::Attach(vbox36, option38);

    auto* option39 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("checkbox_2")}, {DUI_T("group"), DUI_T("shadow_type")}, {DUI_T("text"), DUI_T("Shadow type: none_round, no shadow, with border, rounded corners")}, {DUI_T("selected"), DUI_T("false")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("30,4,0,2")}});
    ui::Attach(vbox36, option39);

    auto* option40 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("checkbox_2")}, {DUI_T("group"), DUI_T("shadow_type")}, {DUI_T("text"), DUI_T("Disable Shadow")}, {DUI_T("selected"), DUI_T("false")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("30,4,0,2")}});
    ui::Attach(vbox36, option40);

    ui::Attach(tab_box2, vbox36);

    auto* vbox37 = ui::Create<ui::VBox>(this, {{DUI_T("bkcolor"), DUI_T("blue")}});
    auto* label282 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Page 2: This is page 2")}});
    ui::Attach(vbox37, label282);

    ui::Attach(tab_box2, vbox37);

    auto* vbox38 = ui::Create<ui::VBox>(this, {{DUI_T("bkcolor"), DUI_T("green")}});
    auto* label283 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Page 3: This is page 3")}});
    ui::Attach(vbox38, label283);

    auto* tab_box3 = ui::Create<ui::TabBox>(this, {{DUI_T("name"), DUI_T("sub_view_tab")}, {DUI_T("selected_id"), DUI_T("2")}, {DUI_T("bkcolor"), DUI_T("Cornsilk")}});
    auto* vbox39 = ui::Create<ui::VBox>(this, {{DUI_T("name"), DUI_T("sub_view_tab_page1")}, {DUI_T("height"), DUI_T("200")}, {DUI_T("bkcolor"), DUI_T("LightBlue")}});
    auto* label284 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Embedded Page 1: This is embedded page 1")}});
    ui::Attach(vbox39, label284);

    ui::Attach(tab_box3, vbox39);

    auto* vbox40 = ui::Create<ui::VBox>(this, {{DUI_T("name"), DUI_T("sub_view_tab_page2")}, {DUI_T("height"), DUI_T("400")}, {DUI_T("bkcolor"), DUI_T("LightCoral")}});
    auto* label285 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Embedded Page 2: This is embedded page 2")}});
    ui::Attach(vbox40, label285);

    auto* label286 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Embedded Page 2: This is embedded page 2")}});
    ui::Attach(vbox40, label286);

    ui::Attach(tab_box3, vbox40);

    auto* vbox41 = ui::Create<ui::VBox>(this, {{DUI_T("name"), DUI_T("sub_view_tab_page3")}, {DUI_T("height"), DUI_T("600")}, {DUI_T("bkcolor"), DUI_T("LightCyan")}});
    auto* label287 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Embedded Page 3: This is embedded page 3")}});
    ui::Attach(vbox41, label287);

    auto* label288 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Embedded Page 3: This is embedded page 3")}});
    ui::Attach(vbox41, label288);

    auto* label289 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Embedded Page 3: This is embedded page 3")}});
    ui::Attach(vbox41, label289);

    ui::Attach(tab_box3, vbox41);

    ui::Attach(vbox38, tab_box3);

    ui::Attach(tab_box2, vbox38);

    auto* label290 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("TabBox Test: TabBox contains an embedded TabBox control")}});
    ui::Attach(vbox35, label290);

    auto* tab_ctrl2 = ui::Create<ui::TabCtrl>(this, {{DUI_T("class"), DUI_T("tab_ctrl")}, {DUI_T("tab_box_name"), DUI_T("main_view_tab_test")}, {DUI_T("selected_id"), DUI_T("0")}});
    auto* tab_ctrl_item18 = ui::Create<ui::TabCtrlItem>(this, {{DUI_T("class"), DUI_T("tab_ctrl_item")}, {DUI_T("title"), DUI_T("Label 1")}, {DUI_T("tab_box_item_index"), DUI_T("0")}});
    ui::Attach(tab_ctrl2, tab_ctrl_item18);

    auto* tab_ctrl_item19 = ui::Create<ui::TabCtrlItem>(this, {{DUI_T("class"), DUI_T("tab_ctrl_item")}, {DUI_T("title"), DUI_T("Label 2")}, {DUI_T("tab_box_item_index"), DUI_T("1")}});
    ui::Attach(tab_ctrl2, tab_ctrl_item19);

    auto* tab_ctrl_item20 = ui::Create<ui::TabCtrlItem>(this, {{DUI_T("class"), DUI_T("tab_ctrl_item")}, {DUI_T("title"), DUI_T("Label 3")}, {DUI_T("tab_box_item_index"), DUI_T("2")}});
    ui::Attach(tab_ctrl2, tab_ctrl_item20);

    ui::Attach(vbox35, tab_ctrl2);

    ui::Attach(vbox35, tab_box2);

    ui::Attach(tab_box1, vbox35);

    auto* vbox42 = ui::Create<ui::VBox>(this, {{DUI_T("name"), DUI_T("main_view_page_15")}});
    auto* hbox24 = ui::Create<ui::HBox>(this, {{DUI_T("child_margin"), DUI_T("2")}});
    auto* grid_box1 = ui::Create<ui::GridBox>(this, {{DUI_T("name"), DUI_T("gridbox_test")}, {DUI_T("padding"), DUI_T("8,8,8,8")}, {DUI_T("width"), DUI_T("70%")}, {DUI_T("height"), DUI_T("100%")}, {DUI_T("grid_width"), DUI_T("120")}, {DUI_T("grid_height"), DUI_T("90")}, {DUI_T("rows"), DUI_T("0")}, {DUI_T("columns"), DUI_T("0")}, {DUI_T("scale_down"), DUI_T("false")}, {DUI_T("child_margin_x"), DUI_T("1")}, {DUI_T("child_margin_y"), DUI_T("1")}, {DUI_T("child_halign"), DUI_T("center")}, {DUI_T("child_valign"), DUI_T("top")}, {DUI_T("bkcolor"), DUI_T("gray")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("red")}});
    auto* control13 = ui::Create<ui::Control>(this, {{DUI_T("float"), DUI_T("true")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("green")}, {DUI_T("bkcolor"), DUI_T("Cornsilk")}});
    ui::Attach(grid_box1, control13);

    auto* label291 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("minwidth/minheight: 200*200")}, {DUI_T("float"), DUI_T("true")}, {DUI_T("text_align"), DUI_T("top")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("height"), DUI_T("100")}, {DUI_T("minwidth"), DUI_T("200")}, {DUI_T("minheight"), DUI_T("200")}, {DUI_T("margin"), DUI_T("64,60,6,6")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("green")}, {DUI_T("bkcolor"), DUI_T("Olive")}});
    ui::Attach(grid_box1, label291);

    auto* label292 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("maxwidth/maxheight: 80*80")}, {DUI_T("float"), DUI_T("true")}, {DUI_T("single_line"), DUI_T("false")}, {DUI_T("width"), DUI_T("200")}, {DUI_T("height"), DUI_T("100")}, {DUI_T("maxwidth"), DUI_T("80")}, {DUI_T("maxheight"), DUI_T("80")}, {DUI_T("margin"), DUI_T("600,70,6,6")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("green")}, {DUI_T("bkcolor"), DUI_T("LemonChiffon")}});
    ui::Attach(grid_box1, label292);

    auto* label293 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox")}, {DUI_T("text"), DUI_T("1 top")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(grid_box1, label293);

    auto* label294 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox")}, {DUI_T("text"), DUI_T("2 center")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("row_span"), DUI_T("2")}});
    ui::Attach(grid_box1, label294);

    auto* label295 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox")}, {DUI_T("text"), DUI_T("3 bottom")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("col_span"), DUI_T("2")}});
    ui::Attach(grid_box1, label295);

    auto* label296 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox")}, {DUI_T("text"), DUI_T("4 top")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(grid_box1, label296);

    auto* label297 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox")}, {DUI_T("text"), DUI_T("5 center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(grid_box1, label297);

    auto* label298 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox")}, {DUI_T("text"), DUI_T("6 bottom")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("bottom")}});
    ui::Attach(grid_box1, label298);

    auto* label299 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox")}, {DUI_T("text"), DUI_T("7 top")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(grid_box1, label299);

    auto* label300 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox")}, {DUI_T("text"), DUI_T("8 center")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(grid_box1, label300);

    auto* label301 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox")}, {DUI_T("text"), DUI_T("9 bottom")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("bottom")}});
    ui::Attach(grid_box1, label301);

    auto* label302 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox")}, {DUI_T("text"), DUI_T("10 center")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(grid_box1, label302);

    auto* label303 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox")}, {DUI_T("text"), DUI_T("11 center stretch: 100%")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("col_span"), DUI_T("3")}});
    ui::Attach(grid_box1, label303);

    auto* label304 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox")}, {DUI_T("text"), DUI_T("12 bottom")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("bottom")}});
    ui::Attach(grid_box1, label304);

    auto* label305 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox")}, {DUI_T("text"), DUI_T("13 top stretch: 50%")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("row_span"), DUI_T("3")}});
    ui::Attach(grid_box1, label305);

    auto* label306 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox")}, {DUI_T("text"), DUI_T("12 center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(grid_box1, label306);

    auto* label307 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox")}, {DUI_T("text"), DUI_T("15 bottom")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("bottom")}});
    ui::Attach(grid_box1, label307);

    auto* label308 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox")}, {DUI_T("text"), DUI_T("16 top")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(grid_box1, label308);

    auto* label309 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox")}, {DUI_T("text"), DUI_T("17 center")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(grid_box1, label309);

    auto* label310 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox")}, {DUI_T("text"), DUI_T("18 bottom")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("bottom")}});
    ui::Attach(grid_box1, label310);

    auto* label311 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox")}, {DUI_T("text"), DUI_T("19 center, width: 200")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(grid_box1, label311);

    auto* label312 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox")}, {DUI_T("text"), DUI_T("20 top")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("col_span"), DUI_T("3")}});
    ui::Attach(grid_box1, label312);

    auto* label313 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox")}, {DUI_T("text"), DUI_T("21 top")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(grid_box1, label313);

    auto* label314 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox")}, {DUI_T("text"), DUI_T("22 top")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(grid_box1, label314);

    ui::Attach(hbox24, grid_box1);

    auto* grid_box2 = ui::Create<ui::GridBox>(this, {{DUI_T("width"), DUI_T("30%")}, {DUI_T("height"), DUI_T("100%")}, {DUI_T("bkcolor"), DUI_T("Cornsilk")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("red")}, {DUI_T("child_halign"), DUI_T("center")}, {DUI_T("child_valign"), DUI_T("center")}, {DUI_T("grid_width"), DUI_T("80")}, {DUI_T("grid_height"), DUI_T("60")}, {DUI_T("rows"), DUI_T("6")}, {DUI_T("columns"), DUI_T("4")}, {DUI_T("child_margin_x"), DUI_T("1")}, {DUI_T("child_margin_y"), DUI_T("1")}});
    auto* label315 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox_calculator")}, {DUI_T("text"), DUI_T("0")}, {DUI_T("col_span"), DUI_T("4")}, {DUI_T("text_align"), DUI_T("center,right")}, {DUI_T("text_padding"), DUI_T("0,0,6,0")}, {DUI_T("bkcolor"), DUI_T("Cornsilk")}});
    ui::Attach(grid_box2, label315);

    auto* label316 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox_calculator")}, {DUI_T("text"), DUI_T("Clear")}, {DUI_T("col_span"), DUI_T("2")}});
    ui::Attach(grid_box2, label316);

    auto* label317 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox_calculator")}, {DUI_T("text"), DUI_T("Undo")}, {DUI_T("col_span"), DUI_T("1")}});
    ui::Attach(grid_box2, label317);

    auto* label318 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox_calculator")}, {DUI_T("text"), DUI_T("=")}, {DUI_T("col_span"), DUI_T("1")}});
    ui::Attach(grid_box2, label318);

    auto* label319 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox_calculator")}, {DUI_T("text"), DUI_T("1")}, {DUI_T("col_span"), DUI_T("1")}});
    ui::Attach(grid_box2, label319);

    auto* label320 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox_calculator")}, {DUI_T("text"), DUI_T("2")}, {DUI_T("col_span"), DUI_T("1")}});
    ui::Attach(grid_box2, label320);

    auto* label321 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox_calculator")}, {DUI_T("text"), DUI_T("3")}, {DUI_T("col_span"), DUI_T("1")}});
    ui::Attach(grid_box2, label321);

    auto* label322 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox_calculator")}, {DUI_T("text"), DUI_T("+")}, {DUI_T("col_span"), DUI_T("1")}});
    ui::Attach(grid_box2, label322);

    auto* label323 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox_calculator")}, {DUI_T("text"), DUI_T("4")}, {DUI_T("col_span"), DUI_T("1")}});
    ui::Attach(grid_box2, label323);

    auto* label324 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox_calculator")}, {DUI_T("text"), DUI_T("5")}, {DUI_T("col_span"), DUI_T("1")}});
    ui::Attach(grid_box2, label324);

    auto* label325 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox_calculator")}, {DUI_T("text"), DUI_T("6")}, {DUI_T("col_span"), DUI_T("1")}});
    ui::Attach(grid_box2, label325);

    auto* label326 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox_calculator")}, {DUI_T("text"), DUI_T("-")}, {DUI_T("col_span"), DUI_T("1")}});
    ui::Attach(grid_box2, label326);

    auto* label327 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox_calculator")}, {DUI_T("text"), DUI_T("7")}, {DUI_T("col_span"), DUI_T("1")}});
    ui::Attach(grid_box2, label327);

    auto* label328 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox_calculator")}, {DUI_T("text"), DUI_T("8")}, {DUI_T("col_span"), DUI_T("1")}});
    ui::Attach(grid_box2, label328);

    auto* label329 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox_calculator")}, {DUI_T("text"), DUI_T("9")}, {DUI_T("col_span"), DUI_T("1")}});
    ui::Attach(grid_box2, label329);

    auto* label330 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox_calculator")}, {DUI_T("text"), DUI_T("*")}, {DUI_T("col_span"), DUI_T("1")}});
    ui::Attach(grid_box2, label330);

    auto* label331 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox_calculator")}, {DUI_T("text"), DUI_T("0")}, {DUI_T("col_span"), DUI_T("2")}});
    ui::Attach(grid_box2, label331);

    auto* label332 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox_calculator")}, {DUI_T("text"), DUI_T(".")}, {DUI_T("col_span"), DUI_T("1")}});
    ui::Attach(grid_box2, label332);

    auto* label333 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_gridbox_calculator")}, {DUI_T("text"), DUI_T("/")}, {DUI_T("col_span"), DUI_T("1")}});
    ui::Attach(grid_box2, label333);

    ui::Attach(hbox24, grid_box2);

    auto* vbox43 = ui::Create<ui::VBox>(this, {{DUI_T("height"), DUI_T("84")}, {DUI_T("padding"), DUI_T("6,0,6,0")}});
    auto* label334 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("GridBox Container Layout Test")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vbox43, label334);

    auto* hbox25 = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("28")}});
    auto* label335 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Container content horizontal alignment:")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hbox25, label335);

    auto* option41 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("gridbox_group_halign")}, {DUI_T("text"), DUI_T("Align Left (left)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("false")}});
    ui::Attach(hbox25, option41);

    auto* option42 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("gridbox_group_halign")}, {DUI_T("text"), DUI_T("Align Center (center)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("true")}});
    ui::Attach(hbox25, option42);

    auto* option43 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("gridbox_group_halign")}, {DUI_T("text"), DUI_T("Align Right (right)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("false")}});
    ui::Attach(hbox25, option43);

    auto* label336 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("(effective only when total content width is smaller than the display area width)")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hbox25, label336);

    ui::Attach(vbox43, hbox25);

    auto* hbox26 = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("28")}});
    auto* label337 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Container content vertical alignment:")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hbox26, label337);

    auto* option44 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("gridbox_group_valign")}, {DUI_T("text"), DUI_T("Align Top (top)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("true")}});
    ui::Attach(hbox26, option44);

    auto* option45 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("gridbox_group_valign")}, {DUI_T("text"), DUI_T("Align Center (center)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("false")}});
    ui::Attach(hbox26, option45);

    auto* option46 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("gridbox_group_valign")}, {DUI_T("text"), DUI_T("Align Bottom (bottom)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("false")}});
    ui::Attach(hbox26, option46);

    auto* label338 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("(effective only when total content height is smaller than the display area height)")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hbox26, label338);

    ui::Attach(vbox43, hbox26);

    ui::Attach(vbox42, vbox43);
    ui::Attach(vbox42, hbox24);
    ui::Attach(tab_box1, vbox42);

    auto* vbox44 = ui::Create<ui::VBox>(this, {{DUI_T("name"), DUI_T("main_view_page_16")}});
    auto* grid_scroll_box1 = ui::Create<ui::GridScrollBox>(this, {{DUI_T("name"), DUI_T("grid_scrollbox_test")}, {DUI_T("padding"), DUI_T("8,8,8,8")}, {DUI_T("width"), DUI_T("100%")}, {DUI_T("height"), DUI_T("100%")}, {DUI_T("grid_width"), DUI_T("180")}, {DUI_T("grid_height"), DUI_T("120")}, {DUI_T("rows"), DUI_T("0")}, {DUI_T("columns"), DUI_T("0")}, {DUI_T("scale_down"), DUI_T("false")}, {DUI_T("child_margin_x"), DUI_T("1")}, {DUI_T("child_margin_y"), DUI_T("1")}, {DUI_T("child_halign"), DUI_T("center")}, {DUI_T("child_valign"), DUI_T("top")}, {DUI_T("bkcolor"), DUI_T("gray")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("red")}, {DUI_T("hscrollbar"), DUI_T("true")}, {DUI_T("vscrollbar"), DUI_T("true")}});
    auto* control14 = ui::Create<ui::Control>(this, {{DUI_T("float"), DUI_T("true")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("green")}, {DUI_T("bkcolor"), DUI_T("Cornsilk")}});
    ui::Attach(grid_scroll_box1, control14);

    auto* label339 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_grid_scrollbox")}, {DUI_T("text"), DUI_T("1 top")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(grid_scroll_box1, label339);

    auto* label340 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_grid_scrollbox")}, {DUI_T("text"), DUI_T("2 center")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("row_span"), DUI_T("2")}});
    ui::Attach(grid_scroll_box1, label340);

    auto* label341 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_grid_scrollbox")}, {DUI_T("text"), DUI_T("3 bottom")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("bottom")}, {DUI_T("col_span"), DUI_T("2")}});
    ui::Attach(grid_scroll_box1, label341);

    auto* label342 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_grid_scrollbox")}, {DUI_T("text"), DUI_T("4 top")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(grid_scroll_box1, label342);

    auto* label343 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_grid_scrollbox")}, {DUI_T("text"), DUI_T("5 center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(grid_scroll_box1, label343);

    auto* label344 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_grid_scrollbox")}, {DUI_T("text"), DUI_T("6 bottom")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("bottom")}});
    ui::Attach(grid_scroll_box1, label344);

    auto* label345 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_grid_scrollbox")}, {DUI_T("text"), DUI_T("7 top")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(grid_scroll_box1, label345);

    auto* label346 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_grid_scrollbox")}, {DUI_T("text"), DUI_T("8 center")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(grid_scroll_box1, label346);

    auto* label347 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_grid_scrollbox")}, {DUI_T("text"), DUI_T("9 bottom")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("bottom")}});
    ui::Attach(grid_scroll_box1, label347);

    auto* label348 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_grid_scrollbox")}, {DUI_T("text"), DUI_T("10 center")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(grid_scroll_box1, label348);

    auto* label349 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_grid_scrollbox")}, {DUI_T("text"), DUI_T("11 center stretch: 100%")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("col_span"), DUI_T("3")}});
    ui::Attach(grid_scroll_box1, label349);

    auto* label350 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_grid_scrollbox")}, {DUI_T("text"), DUI_T("12 bottom")}, {DUI_T("halign"), DUI_T("left")}, {DUI_T("valign"), DUI_T("bottom")}});
    ui::Attach(grid_scroll_box1, label350);

    auto* label351 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_grid_scrollbox")}, {DUI_T("text"), DUI_T("13 top stretch: 50%")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("row_span"), DUI_T("3")}});
    ui::Attach(grid_scroll_box1, label351);

    auto* label352 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_grid_scrollbox")}, {DUI_T("text"), DUI_T("12 center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(grid_scroll_box1, label352);

    auto* label353 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_grid_scrollbox")}, {DUI_T("text"), DUI_T("15 bottom")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("bottom")}});
    ui::Attach(grid_scroll_box1, label353);

    auto* label354 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_grid_scrollbox")}, {DUI_T("text"), DUI_T("16 top")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(grid_scroll_box1, label354);

    auto* label355 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_grid_scrollbox")}, {DUI_T("text"), DUI_T("17 center")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(grid_scroll_box1, label355);

    auto* label356 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_grid_scrollbox")}, {DUI_T("text"), DUI_T("18 bottom")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("bottom")}});
    ui::Attach(grid_scroll_box1, label356);

    auto* label357 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_grid_scrollbox")}, {DUI_T("text"), DUI_T("19 center, width: 200")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(grid_scroll_box1, label357);

    auto* label358 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_grid_scrollbox")}, {DUI_T("text"), DUI_T("20 top")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}, {DUI_T("col_span"), DUI_T("3")}});
    ui::Attach(grid_scroll_box1, label358);

    auto* label359 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_grid_scrollbox")}, {DUI_T("text"), DUI_T("21 top")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(grid_scroll_box1, label359);

    auto* label360 = ui::Create<ui::Label>(this, {{DUI_T("class"), DUI_T("page_grid_scrollbox")}, {DUI_T("text"), DUI_T("22 top")}, {DUI_T("halign"), DUI_T("right")}, {DUI_T("valign"), DUI_T("top")}});
    ui::Attach(grid_scroll_box1, label360);

    auto* vbox45 = ui::Create<ui::VBox>(this, {{DUI_T("height"), DUI_T("84")}, {DUI_T("padding"), DUI_T("6,0,6,0")}});
    auto* label361 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("GridScrollBox Container Layout Test")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(vbox45, label361);

    auto* hbox27 = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("28")}});
    auto* label362 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Container content horizontal alignment:")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hbox27, label362);

    auto* option47 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("grid_scrollbox_group_halign")}, {DUI_T("text"), DUI_T("Align Left (left)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("false")}});
    ui::Attach(hbox27, option47);

    auto* option48 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("grid_scrollbox_group_halign")}, {DUI_T("text"), DUI_T("Align Center (center)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("true")}});
    ui::Attach(hbox27, option48);

    auto* option49 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("grid_scrollbox_group_halign")}, {DUI_T("text"), DUI_T("Align Right (right)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("false")}});
    ui::Attach(hbox27, option49);

    auto* label363 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("(effective only when total content width is smaller than the display area width)")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hbox27, label363);

    ui::Attach(vbox45, hbox27);

    auto* hbox28 = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("28")}});
    auto* label364 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Container content vertical alignment:")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hbox28, label364);

    auto* option50 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("grid_scrollbox_group_valign")}, {DUI_T("text"), DUI_T("Align Top (top)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("true")}});
    ui::Attach(hbox28, option50);

    auto* option51 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("grid_scrollbox_group_valign")}, {DUI_T("text"), DUI_T("Align Center (center)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("false")}});
    ui::Attach(hbox28, option51);

    auto* option52 = ui::Create<ui::Option>(this, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("grid_scrollbox_group_valign")}, {DUI_T("text"), DUI_T("Align Bottom (bottom)")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("selected"), DUI_T("false")}});
    ui::Attach(hbox28, option52);

    auto* label365 = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("(effective only when total content height is smaller than the display area height)")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(hbox28, label365);

    ui::Attach(vbox45, hbox28);

    ui::Attach(vbox44, vbox45);
    ui::Attach(vbox44, grid_scroll_box1);
    ui::Attach(tab_box1, vbox44);

    ui::Attach(vbox1, tab_box1);

    ui::Attach(root, vbox1);


    ui::Attach(this, root);
}

void MainForm::OnInitWindow()
{
    SetupWindow();
    BuildUI();
    BindEvents();

    BaseClass::OnInitWindow();
}

void MainForm::SetupWindow()
{
    ui::UiRect rcWork;
    ui::WindowBase::GetPrimaryMonitorWorkRect(rcWork);
    SetWindowSize(static_cast<int32_t>(rcWork.Width() * 0.75f),
                  static_cast<int32_t>(rcWork.Height() * 0.75f));
    CenterWindow();

    SetShadowAttached(true);
#if defined(DUI_BUILD_FOR_LINUX)
    SetShadowType(ui::Shadow::ShadowType::kShadowDrawDefault);
#else
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    SetLayeredWindow(false, false);
#endif
    SetEnableShadowSnap(true);
    SetShadowBorderSize(0);

    SetSizeBox(ui::UiRect(4, 4, 4, 4), true);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), true);
    SetWindowIcon(DUI_T("public/caption/logo.ico"));
}

void MainForm::BindEvents()
{
}
