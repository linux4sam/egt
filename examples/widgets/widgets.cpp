/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
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

int main()
{
    Application app;

    set_image_path("../share/egt/");

#if 0
    Window win(Size(800, 480));

    Label label1("left align", Rect(Point(100, 50), Size(200, 40)),
                 alignmask::LEFT | alignmask::CENTER, Font(), widgetmask::NONE);
    win.add(&label1);

    Label label2("right align", Rect(Point(100, 100), Size(200, 40)),
                 alignmask::RIGHT | alignmask::CENTER, Font(), widgetmask::NONE);
    win.add(&label2);

    Label label3("top align", Rect(Point(100, 150), Size(200, 40)),
                 alignmask::TOP | alignmask::CENTER, Font(), widgetmask::NONE);
    win.add(&label3);

    Label label4("bottom align", Rect(Point(100, 200), Size(200, 40)),
                 alignmask::BOTTOM | alignmask::CENTER, Font(), widgetmask::NONE);
    win.add(&label4);

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

    Button btn2("button 2", Rect(Point(200, 250), Size(100, 40)));
    win.add(&btn2);

    Keyboard<PlaneWindow> keyboard;
    btn2.on_event([&keyboard](eventid event)
    {
        if (event == eventid::MOUSE_DOWN)
        {
            if (keyboard.visible())
                keyboard.hide();
            else
                keyboard.show();
        }
    });

    Slider slider1(0, 100, Rect(Point(100, 300), Size(200, 40)));
    win.add(&slider1);

    Slider slider2(0, 100, Rect(Point(10, 200), Size(40, 200)), orientation::VERTICAL);
    win.add(&slider2);

#ifdef DEVELOPMENT
    Combo combo1("combo 1", Rect(Point(100, 350), Size(200, 40)));
    win.add(&combo1);
#endif

    SlidingCheckBox sliding1(Rect(Point(100, 350), Size(200, 40)));
    win.add(&sliding1);

    TextBox text1("text 1", Rect(Point(100, 400), Size(200, 40)));
    win.add(&text1);
    // TODO: this is broken with keyboard and focus - get dups
    detail::IInput::global_input().on_event([&text1](eventid event)
    {
        switch (event)
        {
        case eventid::KEYBOARD_DOWN:
        case eventid::KEYBOARD_UP:
        case eventid::KEYBOARD_REPEAT:
            text1.handle(event);
            break;
        default:
            break;
        }
    });

    vector<StringItem> names = { "item 1", "item 2", "item3" };
    ListBox::item_array items;
    items.resize(names.size());
    transform(names.begin(), names.end(), items.begin(), [](const StringItem & v) { return new StringItem(v);});

    ListBox list1(items, Rect(Point(350, 50), Size(100, 40 * 3)));
    win.add(&list1);
    list1.select(1);

    ImageLabel imagelabel1("icons/bug.png",
                           "Bug",
                           Rect(Point(350, 250),
                                Size(200, 40)));
    win.add(&imagelabel1);

    ImageLabel imagelabel2("icons/phone.png",
                           "Phone",
                           Rect(Point(350, 300),
                                Size(200, 40)));
    win.add(&imagelabel2);

    CheckBox checkbox1("checkbox 1", Rect(Point(350, 350), Size(200, 40)));
    win.add(&checkbox1);
    checkbox1.check(true);

    CheckBox checkbox2("checkbox 2", Rect(Point(350, 400), Size(200, 40)));
    win.add(&checkbox2);

    LevelMeter lp1(Rect(Point(600, 250), Size(50, 100)));
    win.add(&lp1);

    AnalogMeter am1(Rect(Point(600, 280), Size(180, 180)));
    win.add(&am1);

    CPUMonitorUsage tools;
    PeriodicTimer cputimer(std::chrono::seconds(1));
    cputimer.on_timeout([&tools, &lp1, &am1]()
    {
        tools.update();
        lp1.value(tools.usage(0));
        am1.value(tools.usage(0));
    });
    cputimer.start();

    win.add(&popup);
    win.add(&keyboard);

    win.show();

    asio::signal_set signals(app.event().io(), SIGQUIT);
    signals.async_wait([&win](const asio::error_code & error, int signal_number)
    {
        ignoreparam(signal_number);
        if (error)
            return;
        win.save_children_to_file();
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
    btn2.disable(true);
    grid0.add(&btn2);

    Slider slider1(0, 100);
    slider1.position(50);
    grid0.add(&slider1);

    SlidingCheckBox sliding1;
    grid0.add(&sliding1);

    SlidingCheckBox sliding2;
    sliding2.check(true);
    grid0.add(&sliding2);

    TextBox text1("text 1");
    grid0.add(&text1);

    TextBox text2("text 2 disabled");
    text2.disable(true);
    grid0.add(&text2);

    // TODO: this is broken with keyboard and focus - get dups
    detail::IInput::global_input().on_event([&text1](eventid event)
    {
        switch (event)
        {
        case eventid::KEYBOARD_DOWN:
        case eventid::KEYBOARD_UP:
        case eventid::KEYBOARD_REPEAT:
            text1.handle(event);
            break;
        default:
            break;
        }
    });

    ImageLabel imagelabel1("icons/bug.png", "Bug");
    grid0.add(&imagelabel1);

    ImageLabel imagelabel2("icons/phone.png", "Phone");
    grid0.add(&imagelabel2);

    CheckBox checkbox1("checkbox 1");
    grid0.add(&checkbox1);
    checkbox1.check(true);

    CheckBox checkbox2("checkbox 2");
    grid0.add(&checkbox2);

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
        lp1.value(tools.usage(0));
        am1.value(tools.usage(0));
    });
    cputimer.start();

    Slider slider2(0, 100, Rect(), orientation::VERTICAL);
    slider2.position(75);
    grid1.add(&slider2);

    win.show();

    return app.run();
}
