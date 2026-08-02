//纯代码构建 UI（对应 tree_view.xml 布局）（从 generated_ui.inc 转录的源码级定义，不依赖 XML/生成器）
static void BuildUIFromXml(ui::Window* pWindow)
{
    pWindow->AddClass(_T("btn_view_common"), _T(" width=\"32\" height=\"24\" border_round=\"4,4\" valign=\"center\" normal_color=\"\" hot_color=\"#FFE4E7EB\" pushed_color=\"#FFB0B4B8\""));
    pWindow->AddClass(_T("btn_view_up"), _T(" normal_image=\"file='../tree_view/arrow-up.svg' width='16' height='16' valign='center' halign='center'\" disabled_image=\"file='../tree_view/arrow-up.svg' width='16' height='16' valign='center' halign='center' fade='128'\""));
    pWindow->AddClass(_T("btn_view_left"), _T(" normal_image=\"file='../tree_view/arrow-left.svg' width='16' height='16' valign='center' halign='center'\" disabled_image=\"file='../tree_view/arrow-left.svg' width='16' height='16' valign='center' halign='center' fade='128'\""));
    pWindow->AddClass(_T("btn_view_right"), _T(" normal_image=\"file='../tree_view/arrow-right.svg' width='16' height='16' valign='center' halign='center'\" disabled_image=\"file='../tree_view/arrow-right.svg' width='16' height='16' valign='center' halign='center' fade='128'\""));
    pWindow->AddClass(_T("btn_view_refresh"), _T(" normal_image=\"file='../tree_view/refresh.svg' width='16' height='16' valign='center' halign='center'\" disabled_image=\"file='../tree_view/refresh.svg' width='16' height='16' valign='center' halign='center' fade='128'\""));
    ui::VBox* p0 = new ui::VBox(pWindow);
        p0->SetAttribute(_T("bkcolor"), _T("bk_wnd_darkcolor"));
    ui::HBox* p1 = new ui::HBox(pWindow);
        p1->SetAttribute(_T("name"), _T("window_caption_bar"));
        p1->SetAttribute(_T("width"), _T("stretch"));
        p1->SetAttribute(_T("height"), _T("36"));
        p1->SetAttribute(_T("bkcolor"), _T("bk_wnd_lightcolor"));
    ui::HBox* p2 = new ui::HBox(pWindow);
        p2->SetAttribute(_T("margin"), _T("0,0,30,0"));
        p2->SetAttribute(_T("valign"), _T("center"));
        p2->SetAttribute(_T("width"), _T("auto"));
        p2->SetAttribute(_T("height"), _T("auto"));
        p2->SetAttribute(_T("mouse_enabled"), _T("false"));
    ui::Control* p3 = new ui::Control(pWindow);
        p3->SetAttribute(_T("width"), _T("18"));
        p3->SetAttribute(_T("height"), _T("18"));
        p3->SetAttribute(_T("bkimage"), _T("public/caption/logo.svg"));
        p3->SetAttribute(_T("valign"), _T("center"));
        p3->SetAttribute(_T("margin"), _T("8,0,0,0"));
        p2->AddItem(p3);

    ui::Label* p4 = new ui::Label(pWindow);
        p4->SetAttribute(_T("text"), _T("TreeView控件测试程序"));
        p4->SetAttribute(_T("valign"), _T("center"));
        p4->SetAttribute(_T("margin"), _T("8,0,0,0"));
        p4->SetAttribute(_T("mouse_enabled"), _T("false"));
        p2->AddItem(p4);

        p1->AddItem(p2);

    ui::Control* p5 = new ui::Control(pWindow);
        p5->SetAttribute(_T("mouse_enabled"), _T("false"));
        p1->AddItem(p5);

    ui::HBox* p6 = new ui::HBox(pWindow);
        p6->SetAttribute(_T("margin"), _T("0,0,0,0"));
        p6->SetAttribute(_T("valign"), _T("center"));
        p6->SetAttribute(_T("width"), _T("auto"));
        p6->SetAttribute(_T("height"), _T("36"));
    ui::Button* p7 = new ui::Button(pWindow);
        p7->SetAttribute(_T("class"), _T("btn_wnd_min_11"));
        p7->SetAttribute(_T("height"), _T("32"));
        p7->SetAttribute(_T("width"), _T("40"));
        p7->SetAttribute(_T("name"), _T("minbtn"));
        p7->SetAttribute(_T("margin"), _T("0,2,0,2"));
        p7->SetAttribute(_T("tooltip_text"), _T("最小化"));
        p6->AddItem(p7);

    ui::Box* p8 = new ui::Box(pWindow);
        p8->SetAttribute(_T("height"), _T("stretch"));
        p8->SetAttribute(_T("width"), _T("40"));
        p8->SetAttribute(_T("margin"), _T("0,2,0,2"));
    ui::Button* p9 = new ui::Button(pWindow);
        p9->SetAttribute(_T("class"), _T("btn_wnd_max_11"));
        p9->SetAttribute(_T("height"), _T("32"));
        p9->SetAttribute(_T("width"), _T("stretch"));
        p9->SetAttribute(_T("name"), _T("maxbtn"));
        p9->SetAttribute(_T("tooltip_text"), _T("最大化"));
        p8->AddItem(p9);

    ui::Button* p10 = new ui::Button(pWindow);
        p10->SetAttribute(_T("class"), _T("btn_wnd_restore_11"));
        p10->SetAttribute(_T("height"), _T("32"));
        p10->SetAttribute(_T("width"), _T("stretch"));
        p10->SetAttribute(_T("name"), _T("restorebtn"));
        p10->SetAttribute(_T("visible"), _T("false"));
        p10->SetAttribute(_T("tooltip_text"), _T("还原"));
        p8->AddItem(p10);

        p6->AddItem(p8);

    ui::Button* p11 = new ui::Button(pWindow);
        p11->SetAttribute(_T("class"), _T("btn_wnd_close_11"));
        p11->SetAttribute(_T("height"), _T("stretch"));
        p11->SetAttribute(_T("width"), _T("40"));
        p11->SetAttribute(_T("name"), _T("closebtn"));
        p11->SetAttribute(_T("margin"), _T("0,0,0,2"));
        p11->SetAttribute(_T("tooltip_text"), _T("关闭"));
        p6->AddItem(p11);

        p1->AddItem(p6);

        p0->AddItem(p1);

    ui::VBox* p12 = new ui::VBox(pWindow);
    ui::HBox* p13 = new ui::HBox(pWindow);
        p13->SetAttribute(_T("minheight"), _T("18"));
        p13->SetAttribute(_T("bkcolor"), _T("#FFCCD5F0"));
        p13->SetAttribute(_T("height"), _T("auto"));
    ui::Label* p14 = new ui::Label(pWindow);
        p14->SetAttribute(_T("text"), _T("功能控制："));
        p14->SetAttribute(_T("valign"), _T("center"));
        p14->SetAttribute(_T("margin"), _T("12,0,10,0"));
        p13->AddItem(p14);

    ui::CheckBox* p15 = new ui::CheckBox(pWindow);
        p15->SetAttribute(_T("class"), _T("checkbox_2"));
        p15->SetAttribute(_T("text"), _T("显示展开标志"));
        p15->SetAttribute(_T("selected"), _T("true"));
        p15->SetAttribute(_T("valign"), _T("center"));
        p15->SetAttribute(_T("margin"), _T("10,4,0,2"));
        p13->AddItem(p15);

    ui::CheckBox* p16 = new ui::CheckBox(pWindow);
        p16->SetAttribute(_T("class"), _T("checkbox_2"));
        p16->SetAttribute(_T("text"), _T("显示CheckBox"));
        p16->SetAttribute(_T("selected"), _T("true"));
        p16->SetAttribute(_T("valign"), _T("center"));
        p16->SetAttribute(_T("margin"), _T("10,4,0,2"));
        p13->AddItem(p16);

    ui::CheckBox* p17 = new ui::CheckBox(pWindow);
        p17->SetAttribute(_T("class"), _T("checkbox_2"));
        p17->SetAttribute(_T("text"), _T("显示图标"));
        p17->SetAttribute(_T("selected"), _T("true"));
        p17->SetAttribute(_T("valign"), _T("center"));
        p17->SetAttribute(_T("margin"), _T("10,4,0,2"));
        p13->AddItem(p17);

    ui::CheckBox* p18 = new ui::CheckBox(pWindow);
        p18->SetAttribute(_T("class"), _T("checkbox_2"));
        p18->SetAttribute(_T("text"), _T("支持多选"));
        p18->SetAttribute(_T("selected"), _T("false"));
        p18->SetAttribute(_T("valign"), _T("center"));
        p18->SetAttribute(_T("margin"), _T("10,4,0,2"));
        p13->AddItem(p18);

    ui::CheckBox* p19 = new ui::CheckBox(pWindow);
        p19->SetAttribute(_T("class"), _T("checkbox_2"));
        p19->SetAttribute(_T("text"), _T("显示隐藏文件"));
        p19->SetAttribute(_T("selected"), _T("false"));
        p19->SetAttribute(_T("valign"), _T("center"));
        p19->SetAttribute(_T("margin"), _T("10,4,0,2"));
        p13->AddItem(p19);

    ui::CheckBox* p20 = new ui::CheckBox(pWindow);
        p20->SetAttribute(_T("class"), _T("checkbox_2"));
        p20->SetAttribute(_T("text"), _T("显示系统文件"));
        p20->SetAttribute(_T("selected"), _T("false"));
        p20->SetAttribute(_T("valign"), _T("center"));
        p20->SetAttribute(_T("margin"), _T("10,4,0,2"));
        p13->AddItem(p20);

        p12->AddItem(p13);

    ui::HBox* p21 = new ui::HBox(pWindow);
        p21->SetAttribute(_T("width"), _T("stretch"));
        p21->SetAttribute(_T("height"), _T("32"));
        p21->SetAttribute(_T("bkcolor"), _T("#FFF8F8F8"));
    ui::Button* p22 = new ui::Button(pWindow);
        p22->SetAttribute(_T("class"), _T("btn_view_common btn_view_up"));
        p22->SetAttribute(_T("name"), _T("btn_view_up"));
        p22->SetAttribute(_T("margin"), _T("4,0,0,0"));
        p22->SetAttribute(_T("tooltip_text"), _T("上移到父目录"));
        p21->AddItem(p22);

    ui::Button* p23 = new ui::Button(pWindow);
        p23->SetAttribute(_T("class"), _T("btn_view_common btn_view_left"));
        p23->SetAttribute(_T("name"), _T("btn_view_left"));
        p23->SetAttribute(_T("tooltip_text"), _T("返回"));
        p21->AddItem(p23);

    ui::Button* p24 = new ui::Button(pWindow);
        p24->SetAttribute(_T("class"), _T("btn_view_common btn_view_right"));
        p24->SetAttribute(_T("name"), _T("btn_view_right"));
        p24->SetAttribute(_T("tooltip_text"), _T("前进"));
        p21->AddItem(p24);

    ui::Button* p25 = new ui::Button(pWindow);
        p25->SetAttribute(_T("class"), _T("btn_view_common btn_view_refresh"));
        p25->SetAttribute(_T("name"), _T("btn_view_refresh"));
        p25->SetAttribute(_T("tooltip_text"), _T("刷新左侧的目录树和文件显示区"));
        p21->AddItem(p25);

    ui::ButtonHBox* p26 = new ui::ButtonHBox(pWindow);
        p26->SetAttribute(_T("name"), _T("btn_view_sort"));
        p26->SetAttribute(_T("width"), _T("auto"));
        p26->SetAttribute(_T("height"), _T("26"));
        p26->SetAttribute(_T("border_round"), _T("5,5"));
        p26->SetAttribute(_T("padding"), _T("6,0,6,0"));
        p26->SetAttribute(_T("valign"), _T("center"));
        p26->SetAttribute(_T("halign"), _T("center"));
        p26->SetAttribute(_T("tooltip_text"), _T("排序方式"));
        p26->SetAttribute(_T("hot_color"), _T("#AAB2B4B8"));
        p26->SetAttribute(_T("pushed_color"), _T("#FFB2B4B8"));
    ui::Control* p27 = new ui::Control(pWindow);
        p27->SetAttribute(_T("width"), _T("auto"));
        p27->SetAttribute(_T("bkimage"), _T("file='../tree_view/view-sort.svg' width='16' height='16' valign='center' halign='center'"));
        p27->SetAttribute(_T("mouse_enabled"), _T("false"));
        p27->SetAttribute(_T("keyboard_enabled"), _T("false"));
        p26->AddItem(p27);

    ui::Label* p28 = new ui::Label(pWindow);
        p28->SetAttribute(_T("text"), _T("排序"));
        p28->SetAttribute(_T("width"), _T("auto"));
        p28->SetAttribute(_T("height"), _T("28"));
        p28->SetAttribute(_T("margin"), _T("4,0,4,0"));
        p28->SetAttribute(_T("font"), _T("system_12"));
        p28->SetAttribute(_T("text_align"), _T("hcenter,vcenter"));
        p28->SetAttribute(_T("valign"), _T("center"));
        p28->SetAttribute(_T("mouse_enabled"), _T("false"));
        p28->SetAttribute(_T("keyboard_enabled"), _T("false"));
        p26->AddItem(p28);

    ui::Control* p29 = new ui::Control(pWindow);
        p29->SetAttribute(_T("width"), _T("auto"));
        p29->SetAttribute(_T("bkimage"), _T("file='../tree_view/chevron-bottom.svg' width='8' height='8' valign='center' halign='center'"));
        p29->SetAttribute(_T("mouse_enabled"), _T("false"));
        p29->SetAttribute(_T("keyboard_enabled"), _T("false"));
        p26->AddItem(p29);

        p21->AddItem(p26);

    ui::ButtonHBox* p30 = new ui::ButtonHBox(pWindow);
        p30->SetAttribute(_T("name"), _T("btn_view_list_type"));
        p30->SetAttribute(_T("width"), _T("auto"));
        p30->SetAttribute(_T("height"), _T("26"));
        p30->SetAttribute(_T("border_round"), _T("5,5"));
        p30->SetAttribute(_T("padding"), _T("6,0,6,0"));
        p30->SetAttribute(_T("valign"), _T("center"));
        p30->SetAttribute(_T("halign"), _T("center"));
        p30->SetAttribute(_T("tooltip_text"), _T("切换视图模式"));
        p30->SetAttribute(_T("hot_color"), _T("#AAB2B4B8"));
        p30->SetAttribute(_T("pushed_color"), _T("#FFB2B4B8"));
    ui::Control* p31 = new ui::Control(pWindow);
        p31->SetAttribute(_T("width"), _T("auto"));
        p31->SetAttribute(_T("bkimage"), _T("file='../tree_view/view-list-symbolic.svg' width='16' height='16' valign='center' halign='center'"));
        p31->SetAttribute(_T("mouse_enabled"), _T("false"));
        p31->SetAttribute(_T("keyboard_enabled"), _T("false"));
        p30->AddItem(p31);

    ui::Label* p32 = new ui::Label(pWindow);
        p32->SetAttribute(_T("text"), _T("查看"));
        p32->SetAttribute(_T("width"), _T("auto"));
        p32->SetAttribute(_T("height"), _T("28"));
        p32->SetAttribute(_T("margin"), _T("4,0,4,0"));
        p32->SetAttribute(_T("font"), _T("system_12"));
        p32->SetAttribute(_T("text_align"), _T("hcenter,vcenter"));
        p32->SetAttribute(_T("valign"), _T("center"));
        p32->SetAttribute(_T("mouse_enabled"), _T("false"));
        p32->SetAttribute(_T("keyboard_enabled"), _T("false"));
        p30->AddItem(p32);

    ui::Control* p33 = new ui::Control(pWindow);
        p33->SetAttribute(_T("width"), _T("auto"));
        p33->SetAttribute(_T("bkimage"), _T("file='../tree_view/chevron-bottom.svg' width='8' height='8' valign='center' halign='center'"));
        p33->SetAttribute(_T("mouse_enabled"), _T("false"));
        p33->SetAttribute(_T("keyboard_enabled"), _T("false"));
        p30->AddItem(p33);

        p21->AddItem(p30);

    ui::Label* p34 = new ui::Label(pWindow);
        p34->SetAttribute(_T("text"), _T("当前路径："));
        p34->SetAttribute(_T("valign"), _T("center"));
        p34->SetAttribute(_T("margin"), _T("10,0,0,0"));
        p21->AddItem(p34);

    ui::AddressBar* p35 = new ui::AddressBar(pWindow);
        p35->SetAttribute(_T("class"), _T("address_bar"));
        p35->SetAttribute(_T("name"), _T("file_path"));
        p35->SetAttribute(_T("width"), _T("stretch"));
        p35->SetAttribute(_T("height"), _T("28"));
        p35->SetAttribute(_T("valign"), _T("center"));
        p35->SetAttribute(_T("margin"), _T("8,0,10,0"));
        p35->SetAttribute(_T("kill_focus_update_ui"), _T("false"));
        p21->AddItem(p35);

        p12->AddItem(p21);

    ui::HBox* p36 = new ui::HBox(pWindow);
        p36->SetAttribute(_T("minheight"), _T("60"));
        p36->SetAttribute(_T("bkcolor"), _T("white"));
        p36->SetAttribute(_T("border_size"), _T("0,2,0,0"));
        p36->SetAttribute(_T("border_color"), _T("splitline_level2"));
        p36->SetAttribute(_T("padding"), _T("0,4,0,0"));
    ui::HBox* p37 = new ui::HBox(pWindow);
        p37->SetAttribute(_T("width"), _T("300"));
        p37->SetAttribute(_T("minwidth"), _T("60"));
    ui::DirectoryTree* p38 = new ui::DirectoryTree(pWindow);
        p38->SetAttribute(_T("class"), _T("tree_view"));
        p38->SetAttribute(_T("name"), _T("tree"));
        p38->SetAttribute(_T("multi_select"), _T("false"));
        p38->SetAttribute(_T("check_box_class"), _T("tree_node_checkbox"));
        p38->SetAttribute(_T("expand_image_class"), _T("tree_node_expand"));
        p38->SetAttribute(_T("padding"), _T("5,3,5,3"));
        p38->SetAttribute(_T("indent"), _T("20"));
        p37->AddItem(p38);

        p36->AddItem(p37);

    ui::Split* p39 = new ui::Split(pWindow);
        p39->SetAttribute(_T("bkcolor"), _T("splitline_level1"));
        p39->SetAttribute(_T("width"), _T("5"));
        p39->SetAttribute(_T("border_color"), _T("white"));
        p39->SetAttribute(_T("border_size"), _T("2,0,2,0"));
        p36->AddItem(p39);

    ui::TabBox* p40 = new ui::TabBox(pWindow);
        p40->SetAttribute(_T("name"), _T("main_view_tab_box"));
        p40->SetAttribute(_T("bkcolor"), _T("white"));
        p40->SetAttribute(_T("selected_id"), _T("0"));
    ui::VirtualListBox* p41 = new ui::VirtualListBox(pWindow, new ui::VirtualVTileLayout);
        p41->SetAttribute(_T("class"), _T("list"));
        p41->SetAttribute(_T("name"), _T("simple_file_view"));
        p41->SetAttribute(_T("columns"), _T("auto"));
        p41->SetAttribute(_T("item_size"), _T("200,120"));
        p41->SetAttribute(_T("multi_select"), _T("false"));
        p41->SetAttribute(_T("vscrollbar"), _T("true"));
        p41->SetAttribute(_T("hscrollbar"), _T("true"));
        p41->SetAttribute(_T("child_halign"), _T("left"));
        p40->AddItem(p41);

    ui::ListCtrl* p42 = new ui::ListCtrl(pWindow);
        p42->SetAttribute(_T("name"), _T("explorer_view"));
        p42->SetAttribute(_T("bkcolor"), _T("YellowGreen"));
        p42->SetAttribute(_T("type"), _T("report"));
        p42->SetAttribute(_T("show_header"), _T("true"));
        p42->SetAttribute(_T("header_class"), _T("list_ctrl_header"));
        p42->SetAttribute(_T("header_item_class"), _T("list_ctrl_header_item"));
        p42->SetAttribute(_T("header_split_box_class"), _T("list_ctrl_header_split_box"));
        p42->SetAttribute(_T("header_split_control_class"), _T("list_ctrl_header_split_control"));
        p42->SetAttribute(_T("header_height"), _T("32"));
        p42->SetAttribute(_T("enable_header_drag_order"), _T("true"));
        p42->SetAttribute(_T("check_box_class"), _T("list_ctrl_checkbox"));
        p42->SetAttribute(_T("data_item_class"), _T("list_ctrl_item"));
        p42->SetAttribute(_T("data_sub_item_class"), _T("list_ctrl_sub_item"));
        p42->SetAttribute(_T("report_view_class"), _T("list_ctrl_report_view"));
        p42->SetAttribute(_T("data_item_height"), _T("46"));
        p42->SetAttribute(_T("row_grid_line_width"), _T("0"));
        p42->SetAttribute(_T("row_grid_line_color"), _T("lightgray"));
        p42->SetAttribute(_T("column_grid_line_width"), _T("0"));
        p42->SetAttribute(_T("column_grid_line_color"), _T("lightgray"));
        p42->SetAttribute(_T("multi_select"), _T("true"));
        p42->SetAttribute(_T("auto_check_select"), _T("false"));
        p42->SetAttribute(_T("show_header_checkbox"), _T("false"));
        p42->SetAttribute(_T("show_data_item_checkbox"), _T("false"));
        p42->SetAttribute(_T("icon_view_class"), _T("list_ctrl_icon_view"));
        p42->SetAttribute(_T("icon_view_item_class"), _T("list_ctrl_icon_view_item"));
        p42->SetAttribute(_T("icon_view_item_image_class"), _T("list_ctrl_icon_view_item_image"));
        p42->SetAttribute(_T("icon_view_item_label_class"), _T("list_ctrl_icon_view_item_label"));
        p42->SetAttribute(_T("list_view_class"), _T("list_ctrl_list_view"));
        p42->SetAttribute(_T("list_view_item_class"), _T("list_ctrl_list_view_item"));
        p42->SetAttribute(_T("list_view_item_image_class"), _T("list_ctrl_list_view_item_image"));
        p42->SetAttribute(_T("list_view_item_label_class"), _T("list_ctrl_list_view_item_label"));
        p42->SetAttribute(_T("enable_item_edit"), _T("true"));
        p42->SetAttribute(_T("list_ctrl_richedit_class"), _T("list_ctrl_richedit"));
        p40->AddItem(p42);

    ui::ListCtrl* p43 = new ui::ListCtrl(pWindow);
        p43->SetAttribute(_T("name"), _T("computer_view"));
        p43->SetAttribute(_T("bkcolor"), _T("YellowGreen"));
        p43->SetAttribute(_T("type"), _T("report"));
        p43->SetAttribute(_T("show_header"), _T("true"));
        p43->SetAttribute(_T("header_class"), _T("list_ctrl_header"));
        p43->SetAttribute(_T("header_item_class"), _T("list_ctrl_header_item"));
        p43->SetAttribute(_T("header_split_box_class"), _T("list_ctrl_header_split_box"));
        p43->SetAttribute(_T("header_split_control_class"), _T("list_ctrl_header_split_control"));
        p43->SetAttribute(_T("header_height"), _T("32"));
        p43->SetAttribute(_T("enable_header_drag_order"), _T("true"));
        p43->SetAttribute(_T("check_box_class"), _T("list_ctrl_checkbox"));
        p43->SetAttribute(_T("data_item_class"), _T("list_ctrl_item"));
        p43->SetAttribute(_T("data_sub_item_class"), _T("list_ctrl_sub_item"));
        p43->SetAttribute(_T("report_view_class"), _T("list_ctrl_report_view"));
        p43->SetAttribute(_T("data_item_height"), _T("46"));
        p43->SetAttribute(_T("row_grid_line_width"), _T("0"));
        p43->SetAttribute(_T("row_grid_line_color"), _T("lightgray"));
        p43->SetAttribute(_T("column_grid_line_width"), _T("0"));
        p43->SetAttribute(_T("column_grid_line_color"), _T("lightgray"));
        p43->SetAttribute(_T("multi_select"), _T("true"));
        p43->SetAttribute(_T("auto_check_select"), _T("false"));
        p43->SetAttribute(_T("show_header_checkbox"), _T("false"));
        p43->SetAttribute(_T("show_data_item_checkbox"), _T("false"));
        p43->SetAttribute(_T("icon_view_class"), _T("list_ctrl_icon_view"));
        p43->SetAttribute(_T("icon_view_item_class"), _T("list_ctrl_icon_view_item"));
        p43->SetAttribute(_T("icon_view_item_image_class"), _T("list_ctrl_icon_view_item_image"));
        p43->SetAttribute(_T("icon_view_item_label_class"), _T("list_ctrl_icon_view_item_label"));
        p43->SetAttribute(_T("list_view_class"), _T("list_ctrl_list_view"));
        p43->SetAttribute(_T("list_view_item_class"), _T("list_ctrl_list_view_item"));
        p43->SetAttribute(_T("list_view_item_image_class"), _T("list_ctrl_list_view_item_image"));
        p43->SetAttribute(_T("list_view_item_label_class"), _T("list_ctrl_list_view_item_label"));
        p43->SetAttribute(_T("enable_item_edit"), _T("true"));
        p43->SetAttribute(_T("list_ctrl_richedit_class"), _T("list_ctrl_richedit"));
        p40->AddItem(p43);

    ui::Box* p44 = new ui::Box(pWindow);
        p44->SetAttribute(_T("name"), _T("error_view"));
        p44->SetAttribute(_T("bkcolor"), _T("red"));
        p40->AddItem(p44);

        p36->AddItem(p40);

        p12->AddItem(p36);

        p0->AddItem(p12);


    pWindow->AttachBox(p0);
}
