//纯代码构建 UI（对应 controls.xml 布局）（从 generated_ui.inc 转录的源码级定义，不依赖 XML/生成器）
static void BuildUIFromXml(ui::Window* pWindow)
{
    ui::VBox* p0 = new ui::VBox(pWindow);
        p0->SetAttribute(_T("bkcolor"), _T("bk_wnd_darkcolor"));
    ui::HBox* p1 = new ui::HBox(pWindow);
        p1->SetAttribute(_T("name"), _T("window_caption_bar"));
        p1->SetAttribute(_T("width"), _T("stretch"));
        p1->SetAttribute(_T("height"), _T("36"));
        p1->SetAttribute(_T("bkcolor"), _T("bk_wnd_lightcolor"));
    ui::Control* p2 = new ui::Control(pWindow);
        p2->SetAttribute(_T("width"), _T("18"));
        p2->SetAttribute(_T("height"), _T("18"));
        p2->SetAttribute(_T("bkimage"), _T("public/caption/logo.svg"));
        p2->SetAttribute(_T("valign"), _T("center"));
        p2->SetAttribute(_T("margin"), _T("8"));
        p1->AddItem(p2);

    ui::Label* p3 = new ui::Label(pWindow);
        p3->SetAttribute(_T("name"), _T("window_title"));
        p3->SetAttribute(_T("text"), _T("Controls"));
        p3->SetAttribute(_T("valign"), _T("center"));
        p3->SetAttribute(_T("margin"), _T("8"));
        p3->SetAttribute(_T("mouse_enabled"), _T("false"));
        p1->AddItem(p3);

    ui::Control* p4 = new ui::Control(pWindow);
        p4->SetAttribute(_T("mouse_enabled"), _T("false"));
        p1->AddItem(p4);

    ui::Button* p5 = new ui::Button(pWindow);
        p5->SetAttribute(_T("class"), _T("btn_wnd_settings_11"));
        p5->SetAttribute(_T("height"), _T("32"));
        p5->SetAttribute(_T("width"), _T("40"));
        p5->SetAttribute(_T("name"), _T("settings"));
        p5->SetAttribute(_T("margin"), _T("0,2,0,2"));
        p5->SetAttribute(_T("tooltip_text"), _T("设置"));
        p1->AddItem(p5);

    ui::Button* p6 = new ui::Button(pWindow);
        p6->SetAttribute(_T("class"), _T("btn_wnd_fullscreen_11"));
        p6->SetAttribute(_T("height"), _T("32"));
        p6->SetAttribute(_T("width"), _T("40"));
        p6->SetAttribute(_T("name"), _T("fullscreenbtn"));
        p6->SetAttribute(_T("margin"), _T("0,2,0,2"));
        p6->SetAttribute(_T("tooltip_text"), _T("窗口全屏，按ESC键可退出全屏"));
        p1->AddItem(p6);

    ui::Button* p7 = new ui::Button(pWindow);
        p7->SetAttribute(_T("class"), _T("btn_wnd_min_11"));
        p7->SetAttribute(_T("height"), _T("32"));
        p7->SetAttribute(_T("width"), _T("40"));
        p7->SetAttribute(_T("name"), _T("minbtn"));
        p7->SetAttribute(_T("margin"), _T("0,2,0,2"));
        p7->SetAttribute(_T("tooltip_text"), _T("最小化"));
        p1->AddItem(p7);

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

        p1->AddItem(p8);

    ui::Button* p11 = new ui::Button(pWindow);
        p11->SetAttribute(_T("class"), _T("btn_wnd_close_11"));
        p11->SetAttribute(_T("height"), _T("stretch"));
        p11->SetAttribute(_T("width"), _T("40"));
        p11->SetAttribute(_T("name"), _T("closebtn"));
        p11->SetAttribute(_T("margin"), _T("0,0,0,2"));
        p11->SetAttribute(_T("tooltip_text"), _T("关闭"));
        p1->AddItem(p11);

        p0->AddItem(p1);

    ui::VBox* p12 = new ui::VBox(pWindow);
        p12->SetAttribute(_T("padding"), _T("10,10,10,10"));
    ui::HBox* p13 = new ui::HBox(pWindow);
        p13->SetAttribute(_T("height"), _T("170"));
        p13->SetAttribute(_T("padding"), _T("0,0,0,10"));
    ui::VBox* p14 = new ui::VBox(pWindow);
        p14->SetAttribute(_T("width"), _T("120"));
    ui::Button* p15 = new ui::Button(pWindow);
        p15->SetAttribute(_T("class"), _T("btn_global_blue_80x30"));
        p15->SetAttribute(_T("text"), _T("blue"));
        p15->SetAttribute(_T("tooltip_text"), _T("ui::Buttons"));
        p14->AddItem(p15);

    ui::Button* p16 = new ui::Button(pWindow);
        p16->SetAttribute(_T("class"), _T("btn_global_white_80x30"));
        p16->SetAttribute(_T("text"), _T("white"));
        p14->AddItem(p16);

    ui::Button* p17 = new ui::Button(pWindow);
        p17->SetAttribute(_T("class"), _T("btn_global_red_80x30"));
        p17->SetAttribute(_T("text"), _T("red"));
        p14->AddItem(p17);

    ui::Button* p18 = new ui::Button(pWindow);
        p18->SetAttribute(_T("class"), _T("btn_global_color_gray"));
        p18->SetAttribute(_T("text"), _T("控件测试"));
        p18->SetAttribute(_T("name"), _T("test_btn"));
        p18->SetAttribute(_T("width"), _T("80"));
        p18->SetAttribute(_T("height"), _T("30"));
        p18->SetAttribute(_T("border_round"), _T("3,3"));
        p18->SetAttribute(_T("margin"), _T("0,2,0,0"));
        p14->AddItem(p18);

    ui::Button* p19 = new ui::Button(pWindow);
        p19->SetAttribute(_T("class"), _T("btn_global_color_gray"));
        p19->SetAttribute(_T("name"), _T("animation_btn"));
        p19->SetAttribute(_T("text"), _T("动画测试"));
        p19->SetAttribute(_T("width"), _T("80"));
        p19->SetAttribute(_T("height"), _T("30"));
        p19->SetAttribute(_T("border_round"), _T("3,3"));
        p19->SetAttribute(_T("margin"), _T("0,2,0,0"));
        p14->AddItem(p19);

        p13->AddItem(p14);

    ui::VBox* p20 = new ui::VBox(pWindow);
        p20->SetAttribute(_T("width"), _T("120"));
    ui::CheckBox* p21 = new ui::CheckBox(pWindow);
        p21->SetAttribute(_T("class"), _T("checkbox_1"));
        p21->SetAttribute(_T("font"), _T("system_14"));
        p21->SetAttribute(_T("text"), _T("多选框1"));
        p21->SetAttribute(_T("margin"), _T("0,3,0,3"));
        p21->SetAttribute(_T("selected"), _T("true"));
        p21->SetAttribute(_T("tooltip_text"), _T("ui::Checkbox"));
        p20->AddItem(p21);

    ui::CheckBox* p22 = new ui::CheckBox(pWindow);
        p22->SetAttribute(_T("class"), _T("checkbox_1"));
        p22->SetAttribute(_T("text"), _T("多选框2"));
        p22->SetAttribute(_T("margin"), _T("0,3,0,3"));
        p20->AddItem(p22);

    ui::CheckBox* p23 = new ui::CheckBox(pWindow);
        p23->SetAttribute(_T("class"), _T("checkbox_2"));
        p23->SetAttribute(_T("text"), _T("多选框3"));
        p23->SetAttribute(_T("margin"), _T("0,3,0,3"));
        p20->AddItem(p23);

    ui::Button* p24 = new ui::Button(pWindow);
        p24->SetAttribute(_T("class"), _T("btn_global_color_gray"));
        p24->SetAttribute(_T("name"), _T("domodal2"));
        p24->SetAttribute(_T("text"), _T("模态对话框"));
        p24->SetAttribute(_T("width"), _T("110"));
        p24->SetAttribute(_T("height"), _T("30"));
        p24->SetAttribute(_T("border_round"), _T("3,3"));
        p24->SetAttribute(_T("margin"), _T("0,12,0,0"));
        p20->AddItem(p24);

    ui::Button* p25 = new ui::Button(pWindow);
        p25->SetAttribute(_T("class"), _T("btn_global_color_gray"));
        p25->SetAttribute(_T("name"), _T("show_color_picker"));
        p25->SetAttribute(_T("text"), _T("拾色器(模态)"));
        p25->SetAttribute(_T("width"), _T("110"));
        p25->SetAttribute(_T("height"), _T("30"));
        p25->SetAttribute(_T("border_round"), _T("3,3"));
        p25->SetAttribute(_T("margin"), _T("0,2,0,0"));
        p20->AddItem(p25);

        p13->AddItem(p20);

    ui::VBox* p26 = new ui::VBox(pWindow);
        p26->SetAttribute(_T("width"), _T("120"));
        p26->SetAttribute(_T("margin"), _T("20,0,0,0"));
    ui::Option* p27 = new ui::Option(pWindow);
        p27->SetAttribute(_T("class"), _T("option_1"));
        p27->SetAttribute(_T("group"), _T("option_group"));
        p27->SetAttribute(_T("text"), _T("单选1"));
        p27->SetAttribute(_T("margin"), _T("0,3,0,3"));
        p27->SetAttribute(_T("selected"), _T("true"));
        p27->SetAttribute(_T("tooltip_text"), _T("ui::Option"));
        p26->AddItem(p27);

    ui::Option* p28 = new ui::Option(pWindow);
        p28->SetAttribute(_T("class"), _T("option_1"));
        p28->SetAttribute(_T("group"), _T("option_group"));
        p28->SetAttribute(_T("text"), _T("单选2"));
        p28->SetAttribute(_T("margin"), _T("0,3,0,3"));
        p26->AddItem(p28);

    ui::Option* p29 = new ui::Option(pWindow);
        p29->SetAttribute(_T("class"), _T("option_1"));
        p29->SetAttribute(_T("group"), _T("option_group"));
        p29->SetAttribute(_T("text"), _T("单选3"));
        p29->SetAttribute(_T("margin"), _T("0,3,0,3"));
        p26->AddItem(p29);

    ui::HBox* p30 = new ui::HBox(pWindow);
        p30->SetAttribute(_T("height"), _T("36"));
    ui::Label* p31 = new ui::Label(pWindow);
        p31->SetAttribute(_T("text"), _T("托盘图标"));
        p31->SetAttribute(_T("height"), _T("36"));
        p31->SetAttribute(_T("valign"), _T("center"));
        p30->AddItem(p31);

    ui::CheckBox* p32 = new ui::CheckBox(pWindow);
        p32->SetAttribute(_T("class"), _T("checkbox_toggle_1"));
        p32->SetAttribute(_T("name"), _T("checkbox_tray_icon"));
        p32->SetAttribute(_T("selected"), _T("false"));
        p32->SetAttribute(_T("margin"), _T("4,3,0,3"));
        p30->AddItem(p32);

        p26->AddItem(p30);

    ui::HBox* p33 = new ui::HBox(pWindow);
        p33->SetAttribute(_T("height"), _T("36"));
    ui::Label* p34 = new ui::Label(pWindow);
        p34->SetAttribute(_T("text"), _T("开关示例"));
        p34->SetAttribute(_T("height"), _T("36"));
        p34->SetAttribute(_T("valign"), _T("center"));
        p33->AddItem(p34);

    ui::CheckBox* p35 = new ui::CheckBox(pWindow);
        p35->SetAttribute(_T("class"), _T("checkbox_toggle_2"));
        p35->SetAttribute(_T("margin"), _T("4,3,0,3"));
        p33->AddItem(p35);

        p26->AddItem(p33);

        p13->AddItem(p26);

    ui::VListBox* p36 = new ui::VListBox(pWindow);
        p36->SetAttribute(_T("class"), _T("list"));
        p36->SetAttribute(_T("name"), _T("list"));
        p36->SetAttribute(_T("padding"), _T("5,3,5,3"));
        p36->SetAttribute(_T("tooltip_text"), _T("ui::VListBox"));
    ui::ListBoxItem* p37 = new ui::ListBoxItem(pWindow);
        p37->SetAttribute(_T("class"), _T("listitem"));
        p37->SetAttribute(_T("height"), _T("20"));
        p37->SetAttribute(_T("text"), _T("ui::VListBox::ListBoxItem"));
        p36->AddItem(p37);

        p13->AddItem(p36);

    ui::TreeView* p38 = new ui::TreeView(pWindow);
        p38->SetAttribute(_T("class"), _T("list"));
        p38->SetAttribute(_T("name"), _T("tree"));
        p38->SetAttribute(_T("padding"), _T("5,3,5,3"));
        p38->SetAttribute(_T("margin"), _T("8,0,0,0"));
        p38->SetAttribute(_T("indent"), _T("20"));
        p38->SetAttribute(_T("tooltip_text"), _T("ui::TreeView"));
    ui::TreeNode* p39 = new ui::TreeNode(pWindow);
        p39->SetAttribute(_T("class"), _T("listitem"));
        p39->SetAttribute(_T("height"), _T("20"));
        p39->SetAttribute(_T("text"), _T("ui::TreeView Parent Node 0"));
        p38->GetRootNode()->AddChildNode(p39);

    ui::TreeNode* p40 = new ui::TreeNode(pWindow);
        p40->SetAttribute(_T("class"), _T("listitem"));
        p40->SetAttribute(_T("height"), _T("20"));
        p40->SetAttribute(_T("text"), _T("ui::TreeView Parent Node 1"));
    ui::TreeNode* p41 = new ui::TreeNode(pWindow);
        p41->SetAttribute(_T("class"), _T("listitem"));
        p41->SetAttribute(_T("height"), _T("20"));
        p41->SetAttribute(_T("text"), _T("ui::TreeView::TreeNode 1-1"));
        p40->AddChildNode(p41);

    ui::TreeNode* p42 = new ui::TreeNode(pWindow);
        p42->SetAttribute(_T("class"), _T("listitem"));
        p42->SetAttribute(_T("height"), _T("20"));
        p42->SetAttribute(_T("text"), _T("ui::TreeView::TreeNode 1-2"));
        p40->AddChildNode(p42);

    ui::TreeNode* p43 = new ui::TreeNode(pWindow);
        p43->SetAttribute(_T("class"), _T("listitem"));
        p43->SetAttribute(_T("height"), _T("20"));
        p43->SetAttribute(_T("text"), _T("ui::TreeView::TreeNode 1-3"));
    ui::TreeNode* p44 = new ui::TreeNode(pWindow);
        p44->SetAttribute(_T("class"), _T("listitem"));
        p44->SetAttribute(_T("height"), _T("20"));
        p44->SetAttribute(_T("text"), _T("ui::TreeView::TreeNode 1-3-1"));
        p43->AddChildNode(p44);

    ui::TreeNode* p45 = new ui::TreeNode(pWindow);
        p45->SetAttribute(_T("class"), _T("listitem"));
        p45->SetAttribute(_T("height"), _T("20"));
        p45->SetAttribute(_T("text"), _T("ui::TreeView::TreeNode 1-3-2"));
    ui::TreeNode* p46 = new ui::TreeNode(pWindow);
        p46->SetAttribute(_T("class"), _T("listitem"));
        p46->SetAttribute(_T("height"), _T("20"));
        p46->SetAttribute(_T("text"), _T("ui::TreeView::TreeNode 1-3-2-1"));
        p45->AddChildNode(p46);

        p43->AddChildNode(p45);

        p40->AddChildNode(p43);

        p38->GetRootNode()->AddChildNode(p40);

    ui::TreeNode* p47 = new ui::TreeNode(pWindow);
        p47->SetAttribute(_T("class"), _T("listitem"));
        p47->SetAttribute(_T("height"), _T("20"));
        p47->SetAttribute(_T("text"), _T("ui::TreeView Parent Node 2"));
    ui::TreeNode* p48 = new ui::TreeNode(pWindow);
        p48->SetAttribute(_T("class"), _T("listitem"));
        p48->SetAttribute(_T("height"), _T("20"));
        p48->SetAttribute(_T("text"), _T("ui::TreeView::TreeNode 2-1"));
        p47->AddChildNode(p48);

    ui::TreeNode* p49 = new ui::TreeNode(pWindow);
        p49->SetAttribute(_T("class"), _T("listitem"));
        p49->SetAttribute(_T("height"), _T("20"));
        p49->SetAttribute(_T("text"), _T("ui::TreeView::TreeNode 2-2"));
        p47->AddChildNode(p49);

    ui::TreeNode* p50 = new ui::TreeNode(pWindow);
        p50->SetAttribute(_T("class"), _T("listitem"));
        p50->SetAttribute(_T("height"), _T("20"));
        p50->SetAttribute(_T("text"), _T("ui::TreeView::TreeNode 2-3"));
        p47->AddChildNode(p50);

        p38->GetRootNode()->AddChildNode(p47);

        p13->AddItem(p38);

        p12->AddItem(p13);

    ui::Control* p51 = new ui::Control(pWindow);
        p51->SetAttribute(_T("class"), _T("splitline_hor_level1"));
        p12->AddItem(p51);

    ui::HBox* p52 = new ui::HBox(pWindow);
        p52->SetAttribute(_T("height"), _T("220"));
    ui::VBox* p53 = new ui::VBox(pWindow);
        p53->SetAttribute(_T("width"), _T("300"));
        p53->SetAttribute(_T("margin"), _T("0,0,10"));
    ui::Combo* p54 = new ui::Combo(pWindow);
        p54->SetAttribute(_T("class"), _T("combo"));
        p54->SetAttribute(_T("name"), _T("combo"));
        p54->SetAttribute(_T("combo_type"), _T("drop_list"));
        p54->SetAttribute(_T("dropbox_size"), _T("0,150"));
        p54->SetAttribute(_T("height"), _T("30"));
        p54->SetAttribute(_T("margin"), _T("0,12,0,0"));
        p54->SetAttribute(_T("tooltip_text"), _T("ui::Combo"));
        p53->AddItem(p54);

    ui::FilterCombo* p55 = new ui::FilterCombo(pWindow);
        p55->SetAttribute(_T("class"), _T("filter_combo"));
        p55->SetAttribute(_T("name"), _T("filter_combo"));
        p55->SetAttribute(_T("dropbox_size"), _T("0,150"));
        p55->SetAttribute(_T("height"), _T("30"));
        p55->SetAttribute(_T("margin"), _T("0,12,0,0"));
        p55->SetAttribute(_T("tooltip_text"), _T("ui::FilterCombo"));
        p53->AddItem(p55);

    ui::RichEdit* p56 = new ui::RichEdit(pWindow);
        p56->SetAttribute(_T("class"), _T("simple prompt simple_border_bottom"));
        p56->SetAttribute(_T("height"), _T("30"));
        p56->SetAttribute(_T("margin"), _T("0,3"));
        p56->SetAttribute(_T("font"), _T("system_14"));
        p56->SetAttribute(_T("text_align"), _T("vcenter"));
        p56->SetAttribute(_T("text_padding"), _T("8,4,8,4"));
        p56->SetAttribute(_T("default_context_menu"), _T("true"));
        p56->SetAttribute(_T("prompt_text"), _T("ui::RichEdit: 单行文本"));
        p53->AddItem(p56);

    ui::CheckCombo* p57 = new ui::CheckCombo(pWindow);
        p57->SetAttribute(_T("class"), _T("check_combo"));
        p57->SetAttribute(_T("name"), _T("check_combo"));
        p57->SetAttribute(_T("height"), _T("30"));
        p57->SetAttribute(_T("margin"), _T("0,3"));
        p57->SetAttribute(_T("tooltip_text"), _T("nim_comp::CheckCombo"));
        p53->AddItem(p57);

    ui::Button* p58 = new ui::Button(pWindow);
        p58->SetAttribute(_T("class"), _T("btn_global_color_gray"));
        p58->SetAttribute(_T("text"), _T("全屏显示RichEdit"));
        p58->SetAttribute(_T("name"), _T("rich_edit_fullscreen_btn"));
        p58->SetAttribute(_T("width"), _T("160"));
        p58->SetAttribute(_T("height"), _T("30"));
        p58->SetAttribute(_T("border_round"), _T("3,3"));
        p58->SetAttribute(_T("margin"), _T("0,2,0,0"));
        p53->AddItem(p58);

        p52->AddItem(p53);

    ui::VBox* p59 = new ui::VBox(pWindow);
        p59->SetAttribute(_T("height"), _T("240"));
    ui::HBox* p60 = new ui::HBox(pWindow);
    ui::VBox* p61 = new ui::VBox(pWindow);
        p61->SetAttribute(_T("width"), _T("200"));
    ui::HBox* p62 = new ui::HBox(pWindow);
        p62->SetAttribute(_T("margin"), _T("0,10"));
        p62->SetAttribute(_T("height"), _T("32"));
    ui::Control* p63 = new ui::Control(pWindow);
        p63->SetAttribute(_T("width"), _T("20"));
        p63->SetAttribute(_T("height"), _T("20"));
        p63->SetAttribute(_T("bkimage"), _T("file='public/animation/loading3.json'"));
        p63->SetAttribute(_T("valign"), _T("center"));
        p62->AddItem(p63);

    ui::Progress* p64 = new ui::Progress(pWindow);
        p64->SetAttribute(_T("class"), _T("progress_horizontal_blue"));
        p64->SetAttribute(_T("name"), _T("progress"));
        p64->SetAttribute(_T("value"), _T("0"));
        p64->SetAttribute(_T("margin"), _T("10"));
        p64->SetAttribute(_T("tooltip_text"), _T("ui::Progress"));
        p62->AddItem(p64);

        p61->AddItem(p62);

    ui::HBox* p65 = new ui::HBox(pWindow);
        p65->SetAttribute(_T("margin"), _T("0,0,0,10"));
        p65->SetAttribute(_T("height"), _T("32"));
    ui::Control* p66 = new ui::Control(pWindow);
        p66->SetAttribute(_T("width"), _T("24"));
        p66->SetAttribute(_T("height"), _T("24"));
        p66->SetAttribute(_T("bkimage"), _T("file='set_speaker.svg' width='20' height='20' valign='center' halign='center'"));
        p66->SetAttribute(_T("valign"), _T("center"));
        p65->AddItem(p66);

    ui::Slider* p67 = new ui::Slider(pWindow);
        p67->SetAttribute(_T("class"), _T("slider_horizontal_green"));
        p67->SetAttribute(_T("value"), _T("70"));
        p67->SetAttribute(_T("margin"), _T("10"));
        p67->SetAttribute(_T("tooltip_text"), _T("ui::Slider"));
        p65->AddItem(p67);

        p61->AddItem(p65);

        p60->AddItem(p61);

    ui::VBox* p68 = new ui::VBox(pWindow);
        p68->SetAttribute(_T("width"), _T("90"));
        p68->SetAttribute(_T("margin"), _T("10,0"));
    ui::CircleProgress* p69 = new ui::CircleProgress(pWindow);
        p69->SetAttribute(_T("name"), _T("circleprogress"));
        p69->SetAttribute(_T("circular"), _T("true"));
        p69->SetAttribute(_T("height"), _T("80"));
        p69->SetAttribute(_T("width"), _T("80"));
        p69->SetAttribute(_T("circle_width"), _T("12"));
        p69->SetAttribute(_T("bgcolor"), _T("gray"));
        p69->SetAttribute(_T("fgcolor"), _T("green"));
        p69->SetAttribute(_T("gradient_color"), _T("red"));
        p69->SetAttribute(_T("clockwise"), _T("true"));
        p69->SetAttribute(_T("min"), _T("0"));
        p69->SetAttribute(_T("max"), _T("100"));
        p69->SetAttribute(_T("value"), _T("75"));
        p69->SetAttribute(_T("margin"), _T("10"));
        p69->SetAttribute(_T("text_padding"), _T("10,32,10,10"));
        p69->SetAttribute(_T("normal_text_color"), _T("darkcolor"));
        p69->SetAttribute(_T("indicator"), _T("file='public/progress/indicator.svg' width='12' height='12'"));
        p69->SetAttribute(_T("tooltip_text"), _T("ui::CircleProgress"));
        p68->AddItem(p69);

        p60->AddItem(p68);

    ui::VBox* p70 = new ui::VBox(pWindow);
        p70->SetAttribute(_T("margin"), _T("4,8,0,0"));
    ui::HBox* p71 = new ui::HBox(pWindow);
        p71->SetAttribute(_T("child_margin"), _T("4"));
        p71->SetAttribute(_T("width"), _T("300"));
    ui::Label* p72 = new ui::Label(pWindow);
        p72->SetAttribute(_T("text"), _T("日期"));
        p72->SetAttribute(_T("height"), _T("24"));
        p72->SetAttribute(_T("text_align"), _T("vcenter"));
        p72->SetAttribute(_T("hot_text_color"), _T("red"));
        p72->SetAttribute(_T("pushed_text_color"), _T("blue"));
        p71->AddItem(p72);

    ui::DateTime* p73 = new ui::DateTime(pWindow);
        p73->SetAttribute(_T("width"), _T("100%"));
        p73->SetAttribute(_T("height"), _T("24"));
        p73->SetAttribute(_T("text"), _T("2022-02-01"));
        p71->AddItem(p73);

    ui::DateTime* p74 = new ui::DateTime(pWindow);
        p74->SetAttribute(_T("width"), _T("120"));
        p74->SetAttribute(_T("height"), _T("24"));
        p74->SetAttribute(_T("edit_format"), _T("date_up_down"));
        p71->AddItem(p74);

        p70->AddItem(p71);

    ui::HBox* p75 = new ui::HBox(pWindow);
        p75->SetAttribute(_T("child_margin"), _T("4"));
        p75->SetAttribute(_T("width"), _T("320"));
    ui::Label* p76 = new ui::Label(pWindow);
        p76->SetAttribute(_T("text"), _T("时间"));
        p76->SetAttribute(_T("height"), _T("24"));
        p76->SetAttribute(_T("text_align"), _T("vcenter"));
        p75->AddItem(p76);

    ui::DateTime* p77 = new ui::DateTime(pWindow);
        p77->SetAttribute(_T("width"), _T("100%"));
        p77->SetAttribute(_T("height"), _T("24"));
        p77->SetAttribute(_T("edit_format"), _T("date_time_up_down"));
        p75->AddItem(p77);

    ui::DateTime* p78 = new ui::DateTime(pWindow);
        p78->SetAttribute(_T("width"), _T("72"));
        p78->SetAttribute(_T("height"), _T("24"));
        p78->SetAttribute(_T("edit_format"), _T("minute_up_down"));
        p75->AddItem(p78);

        p70->AddItem(p75);

        p60->AddItem(p70);

        p59->AddItem(p60);

    ui::HBox* p79 = new ui::HBox(pWindow);
    ui::VBox* p80 = new ui::VBox(pWindow);
        p80->SetAttribute(_T("margin"), _T("10,0,0,0"));
    ui::HBox* p81 = new ui::HBox(pWindow);
        p81->SetAttribute(_T("child_margin"), _T("10"));
    ui::Label* p82 = new ui::Label(pWindow);
        p82->SetAttribute(_T("text"), _T("IP地址"));
        p82->SetAttribute(_T("height"), _T("28"));
        p82->SetAttribute(_T("text_align"), _T("vcenter"));
        p81->AddItem(p82);

    ui::IPAddress* p83 = new ui::IPAddress(pWindow);
        p83->SetAttribute(_T("class"), _T("ip_address"));
        p83->SetAttribute(_T("ip"), _T("192.168.0.1"));
        p81->AddItem(p83);

        p80->AddItem(p81);

    ui::HBox* p84 = new ui::HBox(pWindow);
        p84->SetAttribute(_T("child_margin"), _T("10"));
    ui::Label* p85 = new ui::Label(pWindow);
        p85->SetAttribute(_T("text"), _T("RichEdit Spin"));
        p85->SetAttribute(_T("height"), _T("28"));
        p85->SetAttribute(_T("text_align"), _T("vcenter"));
        p84->AddItem(p85);

    ui::RichEdit* p86 = new ui::RichEdit(pWindow);
        p86->SetAttribute(_T("class"), _T("simple simple_border rich_edit_spin"));
        p86->SetAttribute(_T("min_number"), _T("-64"));
        p86->SetAttribute(_T("max_number"), _T("64"));
        p86->SetAttribute(_T("limit_text"), _T("3"));
        p86->SetAttribute(_T("text"), _T("0"));
        p84->AddItem(p86);

        p80->AddItem(p84);

        p79->AddItem(p80);

    ui::VBox* p87 = new ui::VBox(pWindow);
        p87->SetAttribute(_T("margin"), _T("10,0,0,0"));
    ui::HBox* p88 = new ui::HBox(pWindow);
        p88->SetAttribute(_T("child_margin"), _T("10"));
    ui::HotKey* p89 = new ui::HotKey(pWindow);
        p89->SetAttribute(_T("class"), _T("hot_key"));
        p89->SetAttribute(_T("name"), _T("set_hot_key"));
        p89->SetAttribute(_T("default_text"), _T("无"));
        p88->AddItem(p89);

    ui::Button* p90 = new ui::Button(pWindow);
        p90->SetAttribute(_T("class"), _T("btn_global_color_gray"));
        p90->SetAttribute(_T("name"), _T("btn_set_hot_key"));
        p90->SetAttribute(_T("text"), _T("设置窗口热键"));
        p90->SetAttribute(_T("width"), _T("100%"));
        p90->SetAttribute(_T("height"), _T("28"));
        p90->SetAttribute(_T("border_round"), _T("3,3"));
        p90->SetAttribute(_T("text_align"), _T("vcenter"));
        p88->AddItem(p90);

        p87->AddItem(p88);

    ui::HBox* p91 = new ui::HBox(pWindow);
        p91->SetAttribute(_T("child_margin"), _T("10"));
    ui::HotKey* p92 = new ui::HotKey(pWindow);
        p92->SetAttribute(_T("class"), _T("hot_key"));
        p92->SetAttribute(_T("name"), _T("set_system_hot_key"));
        p92->SetAttribute(_T("default_text"), _T("无"));
        p91->AddItem(p92);

    ui::Button* p93 = new ui::Button(pWindow);
        p93->SetAttribute(_T("class"), _T("btn_global_color_gray"));
        p93->SetAttribute(_T("name"), _T("btn_set_system_hot_key"));
        p93->SetAttribute(_T("text"), _T("设置系统热键"));
        p93->SetAttribute(_T("width"), _T("100%"));
        p93->SetAttribute(_T("height"), _T("28"));
        p93->SetAttribute(_T("border_round"), _T("3,3"));
        p93->SetAttribute(_T("text_align"), _T("vcenter"));
        p91->AddItem(p93);

        p87->AddItem(p91);

        p79->AddItem(p87);

        p59->AddItem(p79);

    ui::HBox* p94 = new ui::HBox(pWindow);
        p94->SetAttribute(_T("child_margin"), _T("10"));
    ui::HBox* p95 = new ui::HBox(pWindow);
        p95->SetAttribute(_T("child_margin"), _T("10"));
    ui::RichText* p96 = new ui::RichText(pWindow);
        p96->SetAttribute(_T("class"), _T("rich_text"));
        p96->SetAttribute(_T("name"), _T("rich_text_demo"));
        p96->SetAttribute(_T("row_spacing_mul"), _T("1.2"));
    ui::WindowBuilder::ParseRichTextXmlText(_T("<RichText>\n                                RichText:　<a href=\"www.baidu.com\">点击访问百度首页</a><br/>\n                                RichText:　<a href=\"www.sohu.com\">点击访问搜狐首页</a></RichText>"), p96);
        p95->AddItem(p96);

    ui::VBox* p97 = new ui::VBox(pWindow);
        p97->SetAttribute(_T("width"), _T("80"));
    ui::HyperLink* p98 = new ui::HyperLink(pWindow);
        p98->SetAttribute(_T("class"), _T("hyper_link"));
        p98->SetAttribute(_T("name"), _T("hyper_link1"));
        p98->SetAttribute(_T("text"), _T("百度首页"));
        p98->SetAttribute(_T("url"), _T("www.baidu.com"));
        p98->SetAttribute(_T("tooltip_text"), _T("这是一个HyperLink控件"));
        p97->AddItem(p98);

    ui::HyperLink* p99 = new ui::HyperLink(pWindow);
        p99->SetAttribute(_T("class"), _T("hyper_link"));
        p99->SetAttribute(_T("name"), _T("hyper_link2"));
        p99->SetAttribute(_T("text"), _T("文字按钮"));
        p99->SetAttribute(_T("tooltip_text"), _T("这是一个HyperLink控件"));
        p97->AddItem(p99);

        p95->AddItem(p97);

        p94->AddItem(p95);

    ui::HBox* p100 = new ui::HBox(pWindow);
        p100->SetAttribute(_T("child_margin"), _T("10"));
    ui::Label* p101 = new ui::Label(pWindow);
        p101->SetAttribute(_T("text"), _T("Line:"));
        p101->SetAttribute(_T("valign"), _T("center"));
        p100->AddItem(p101);

    ui::VBox* p102 = new ui::VBox(pWindow);
    ui::Line* p103 = new ui::Line(pWindow);
        p103->SetAttribute(_T("vertical"), _T("false"));
        p103->SetAttribute(_T("line_color"), _T("blue"));
        p103->SetAttribute(_T("dash_style"), _T("solid"));
        p103->SetAttribute(_T("line_width"), _T("2"));
        p102->AddItem(p103);

    ui::Line* p104 = new ui::Line(pWindow);
        p104->SetAttribute(_T("vertical"), _T("false"));
        p104->SetAttribute(_T("line_color"), _T("red"));
        p104->SetAttribute(_T("dash_style"), _T("dash"));
        p104->SetAttribute(_T("line_width"), _T("2"));
        p102->AddItem(p104);

    ui::Line* p105 = new ui::Line(pWindow);
        p105->SetAttribute(_T("vertical"), _T("false"));
        p105->SetAttribute(_T("line_color"), _T("green"));
        p105->SetAttribute(_T("dash_style"), _T("dot"));
        p105->SetAttribute(_T("line_width"), _T("2"));
        p102->AddItem(p105);

    ui::Line* p106 = new ui::Line(pWindow);
        p106->SetAttribute(_T("vertical"), _T("false"));
        p106->SetAttribute(_T("line_color"), _T("black"));
        p106->SetAttribute(_T("dash_style"), _T("dash_dot"));
        p106->SetAttribute(_T("line_width"), _T("2"));
        p102->AddItem(p106);

    ui::Line* p107 = new ui::Line(pWindow);
        p107->SetAttribute(_T("vertical"), _T("false"));
        p107->SetAttribute(_T("line_color"), _T("orange"));
        p107->SetAttribute(_T("dash_style"), _T("dash_dot_dot"));
        p107->SetAttribute(_T("line_width"), _T("2"));
        p102->AddItem(p107);

        p100->AddItem(p102);

    ui::VBox* p108 = new ui::VBox(pWindow);
    ui::HBox* p109 = new ui::HBox(pWindow);
        p109->SetAttribute(_T("margin"), _T("0,4,0,4"));
    ui::Line* p110 = new ui::Line(pWindow);
        p110->SetAttribute(_T("vertical"), _T("true"));
        p110->SetAttribute(_T("line_color"), _T("blue"));
        p110->SetAttribute(_T("dash_style"), _T("solid"));
        p110->SetAttribute(_T("line_width"), _T("1"));
        p109->AddItem(p110);

    ui::Line* p111 = new ui::Line(pWindow);
        p111->SetAttribute(_T("vertical"), _T("true"));
        p111->SetAttribute(_T("line_color"), _T("red"));
        p111->SetAttribute(_T("dash_style"), _T("dash"));
        p111->SetAttribute(_T("line_width"), _T("1"));
        p109->AddItem(p111);

    ui::Line* p112 = new ui::Line(pWindow);
        p112->SetAttribute(_T("vertical"), _T("true"));
        p112->SetAttribute(_T("line_color"), _T("green"));
        p112->SetAttribute(_T("dash_style"), _T("dot"));
        p112->SetAttribute(_T("line_width"), _T("1"));
        p109->AddItem(p112);

    ui::Line* p113 = new ui::Line(pWindow);
        p113->SetAttribute(_T("vertical"), _T("true"));
        p113->SetAttribute(_T("line_color"), _T("black"));
        p113->SetAttribute(_T("dash_style"), _T("dash_dot"));
        p113->SetAttribute(_T("line_width"), _T("1"));
        p109->AddItem(p113);

    ui::Line* p114 = new ui::Line(pWindow);
        p114->SetAttribute(_T("vertical"), _T("true"));
        p114->SetAttribute(_T("line_color"), _T("orange"));
        p114->SetAttribute(_T("dash_style"), _T("dash_dot_dot"));
        p114->SetAttribute(_T("line_width"), _T("1"));
        p109->AddItem(p114);

        p108->AddItem(p109);

        p100->AddItem(p108);

        p94->AddItem(p100);

        p59->AddItem(p94);

        p52->AddItem(p59);

        p12->AddItem(p52);

    ui::Control* p115 = new ui::Control(pWindow);
        p115->SetAttribute(_T("class"), _T("splitline_hor_level1"));
        p115->SetAttribute(_T("margin"), _T("0,8,0,4"));
        p12->AddItem(p115);

    ui::HBox* p116 = new ui::HBox(pWindow);
    ui::RichEdit* p117 = new ui::RichEdit(pWindow);
        p117->SetAttribute(_T("class"), _T("prompt"));
        p117->SetAttribute(_T("name"), _T("edit"));
        p117->SetAttribute(_T("bkcolor"), _T("white"));
        p117->SetAttribute(_T("width"), _T("stretch"));
        p117->SetAttribute(_T("height"), _T("stretch"));
        p117->SetAttribute(_T("hide_selection"), _T("false"));
        p117->SetAttribute(_T("multiline"), _T("true"));
        p117->SetAttribute(_T("auto_vscroll"), _T("true"));
        p117->SetAttribute(_T("vscrollbar"), _T("true"));
        p117->SetAttribute(_T("hscrollbar"), _T("true"));
        p117->SetAttribute(_T("want_return_msg"), _T("true"));
        p117->SetAttribute(_T("rich"), _T("false"));
        p117->SetAttribute(_T("enable_drag_drop"), _T("true"));
        p117->SetAttribute(_T("enable_drop_file"), _T("true"));
        p117->SetAttribute(_T("drop_file_types"), _T(".txt;.md"));
        p116->AddItem(p117);

    ui::RichEdit* p118 = new ui::RichEdit(pWindow);
        p118->SetAttribute(_T("class"), _T("prompt"));
        p118->SetAttribute(_T("name"), _T("edit2"));
        p118->SetAttribute(_T("bkcolor"), _T("white"));
        p118->SetAttribute(_T("width"), _T("stretch"));
        p118->SetAttribute(_T("height"), _T("stretch"));
        p118->SetAttribute(_T("hide_selection"), _T("false"));
        p118->SetAttribute(_T("multiline"), _T("true"));
        p118->SetAttribute(_T("auto_vscroll"), _T("true"));
        p118->SetAttribute(_T("vscrollbar"), _T("true"));
        p118->SetAttribute(_T("hscrollbar"), _T("true"));
        p118->SetAttribute(_T("scrollbar_float"), _T("true"));
        p118->SetAttribute(_T("vscrollbar_class"), _T("vscrollbar2"));
        p118->SetAttribute(_T("hscrollbar_class"), _T("hscrollbar2"));
        p118->SetAttribute(_T("vscrollbar_style"), _T("auto_hide_scroll='false'"));
        p118->SetAttribute(_T("hscrollbar_style"), _T("auto_hide_scroll='false'"));
        p118->SetAttribute(_T("want_return_msg"), _T("true"));
        p118->SetAttribute(_T("rich"), _T("false"));
        p118->SetAttribute(_T("margin"), _T("4,0,0,0"));
        p118->SetAttribute(_T("enable_drag_drop"), _T("true"));
        p118->SetAttribute(_T("enable_drop_file"), _T("false"));
        p116->AddItem(p118);

        p12->AddItem(p116);

        p0->AddItem(p12);


    pWindow->AttachBox(p0);
}
