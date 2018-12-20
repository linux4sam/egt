/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <math.h>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>

using namespace std;
using namespace egt;

int main(int argc, const char** argv)
{
    Application app(argc, argv);

    set_image_path("../share/egt/examples/widgets/");

    TopWindow win;

    StaticGrid grid0(Rect(Point(), Size(win.w() / 4 * 3, win.h())), 3, 10, 5);
    win.add(&grid0);
    grid0.palette().set(Palette::BORDER, Palette::GROUP_NORMAL, Color::TRANSPARENT);
    grid0.set_align(alignmask::LEFT | alignmask::EXPAND_VERTICAL);

    Label label1("left align", Rect(),
                 alignmask::LEFT | alignmask::CENTER, Font(), widgetmask::NONE);
    grid0.add(&label1);

    Label label2("right align", Rect(),
                 alignmask::RIGHT | alignmask::CENTER, Font(), widgetmask::NONE);
    grid0.add(&label2);

    Label label3("top align", Rect(),
                 alignmask::TOP | alignmask::CENTER, Font(), widgetmask::NONE);
    grid0.add(&label3);

    Label label4("bottom align", Rect(),
                 alignmask::BOTTOM | alignmask::CENTER, Font(), widgetmask::NONE);
    grid0.add(&label4);

    Button btn1("button 1", Rect(Point(), Size(100, 40)));
    grid0.add(&btn1);

    Button btn2("button 2");
    btn2.disable();
    grid0.add(&btn2);

    Slider slider1;
    slider1.set_value(50);
    grid0.add(&slider1);

    SlidingCheckBox sliding1;
    grid0.add(&sliding1);

    SlidingCheckBox sliding2;
    sliding2.check(true);
    grid0.add(&sliding2);

    TextBox text1("text 1");
    grid0.add(&text1);

    TextBox text2("text 2 disabled");
    text2.disable();
    grid0.add(&text2);

    ImageLabel imagelabel0(Image("@bug.png"), "Bug");
    grid0.add(&imagelabel0);

    ImageLabel imagelabel1(Image("@phone.png"), "Phone");
    grid0.add(&imagelabel1);
    imagelabel1.set_text_align(alignmask::CENTER);

    ImageLabel imagelabel2(Image("@phone.png"), "Phone");
    grid0.add(&imagelabel2);
    imagelabel2.set_text_align(alignmask::CENTER | alignmask::RIGHT);

    ImageLabel imagelabel3(Image("@phone.png"), "Phone");
    grid0.add(&imagelabel3);
    imagelabel3.set_text_align(alignmask::CENTER | alignmask::TOP);

    ImageLabel imagelabel4(Image("@phone.png"), "Phone");
    grid0.add(&imagelabel4);
    imagelabel4.set_text_align(alignmask::CENTER | alignmask::BOTTOM);

    ImageLabel imagelabel5(Image("@phone.png"));
    grid0.add(&imagelabel5);

    ImageButton imagebutton0(Image("@flag_red.png"), "Flag");
    grid0.add(&imagebutton0);

    ImageButton imagebutton1(Image("@flag_red.png"), "Flag");
    grid0.add(&imagebutton1);
    imagebutton1.set_text_align(alignmask::CENTER | alignmask::LEFT);

    ImageButton imagebutton2(Image("@flag_red.png"), "Flag");
    grid0.add(&imagebutton2);
    imagebutton2.set_text_align(alignmask::CENTER | alignmask::RIGHT);

    ImageButton imagebutton3(Image("@flag_red.png"), "Flag");
    grid0.add(&imagebutton3);
    imagebutton3.set_text_align(alignmask::CENTER | alignmask::TOP);

    ImageButton imagebutton4(Image("@flag_red.png"), "Flag");
    grid0.add(&imagebutton4);
    imagebutton4.set_text_align(alignmask::CENTER | alignmask::BOTTOM);

    ImageButton imagebutton5(Image("@flag_red.png"));
    grid0.add(&imagebutton5);

    CheckBox checkbox1("checkbox 1");
    grid0.add(&checkbox1);
    checkbox1.check(true);

    CheckBox checkbox2("checkbox 2");
    grid0.add(&checkbox2);

    ComboBox::item_array combo_items =
    {
        "item 1",
        "item 2",
        "item 3",
    };
    ComboBox combo1(combo_items);
    grid0.add(&combo1);

    SpinProgress spinprogress;
    grid0.add(&spinprogress);

    ProgressBar progressbar;
    grid0.add(&progressbar);

    StaticGrid grid1(Rect(Point(win.w() / 4 * 3, 0), Size(win.w() / 4 * 1, win.h())), 1, 3, 5);
    win.add(&grid1);
    grid1.palette().set(Palette::BORDER, Palette::GROUP_NORMAL, Color::TRANSPARENT);
    grid1.set_align(alignmask::RIGHT | alignmask::EXPAND_VERTICAL);

    LevelMeter lp1;
    grid1.add(&lp1);

    AnalogMeter am1;
    grid1.add(&am1);

    CPUMonitorUsage tools;
    PeriodicTimer cputimer(std::chrono::seconds(1));
    cputimer.on_timeout([&]()
    {
        tools.update();
        lp1.set_value(tools.usage(0));
        am1.set_value(tools.usage(0));
        progressbar.set_value(tools.usage(0));
        spinprogress.set_value(tools.usage(0));
    });
    cputimer.start();

    Slider slider2(Rect(), 0, 100, 0, orientation::VERTICAL);
    slider2.set_value(75);
    grid1.add(&slider2);

    win.show();

    app.dump(cout);

    return app.run();
}
