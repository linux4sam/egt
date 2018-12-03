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

#if 0
    Popup<Window> popup(Size(100, 100));
    popup.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::RED);

    Button btn1("button 1", Rect(Point(100, 250), Size(100, 40)));
    win.add(&btn1);

    btn1.on_event([&popup](eventid event)
    {
        if (event == eventid::MOUSE_DOWN)
        {
            if (popup.visible())
                popup.hide();
            else
                popup.show(true);
        }
    });
#endif

    Window win;

    StaticGrid grid0(Rect(Point(), Size(win.w() / 2, win.h())), 2, 10, 5);
    win.add(&grid0);
    //grid0.align(alignmask::EXPAND);

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

#if 0
    btn1.on_event([&popup](eventid event)
    {

        if (event == eventid::MOUSE_DOWN)
        {
            if (popup.visible())
                popup.hide();
            else
                popup.show(true);
        }

    });
#endif

    Button btn2("button 2");
    btn2.disable();
    grid0.add(&btn2);

    Slider slider1(0, 100);
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

#if 0
    // TODO: this is broken with keyboard and focus - get dups
    detail::IInput::global_input().on_event([&text1](eventid event)
    {
        switch (event)
        {
        case eventid::KEYBOARD_DOWN:
        case eventid::KEYBOARD_UP:
        case eventid::KEYBOARD_REPEAT:
            return text1.handle(event);
            break;
        default:
            break;
        }
        return 0;
    });
#endif

    ImageLabel imagelabel1("@bug.png", "Bug");
    grid0.add(&imagelabel1);

    ImageLabel imagelabel2("@phone.png", "Phone");
    grid0.add(&imagelabel2);

    CheckBox checkbox1("checkbox 1");
    grid0.add(&checkbox1);
    checkbox1.check(true);

    CheckBox checkbox2("checkbox 2");
    grid0.add(&checkbox2);

    experimental::ComboBox::item_array combo_items =
    {
        "item 1",
        "item 2",
        "item 3",
    };
    experimental::ComboBox combo1(combo_items);
    grid0.add(&combo1);

    StaticGrid grid1(Rect(Point(win.w() / 2, 0), Size(win.w() / 2, win.h())), 2, 3, 5);
    win.add(&grid1);

    LevelMeter lp1;
    grid1.add(&lp1);

    AnalogMeter am1;
    grid1.add(&am1);

    CPUMonitorUsage tools;
    PeriodicTimer cputimer(std::chrono::seconds(1));
    cputimer.on_timeout([&tools, &lp1, &am1]()
    {
        tools.update();
        lp1.set_value(tools.usage(0));
        am1.set_value(tools.usage(0));
    });
    cputimer.start();

    Slider slider2(0, 100, Rect(), orientation::VERTICAL);
    slider2.set_value(75);
    grid1.add(&slider2);

    win.show();

    app.event().dump();

    return app.run();
}
