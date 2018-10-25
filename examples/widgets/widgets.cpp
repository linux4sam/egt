/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include <mui/ui>
#include <math.h>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>

using namespace std;
using namespace mui;

/**
 * On-screen keyboard.
 * @todo This needs to be made an official widget.
 */
template <class T>
class Keyboard : public T
{
public:
    Keyboard()
        : T(Size(800, 200)),
          m_grid(Rect(), 10, 4, 5)
    {
        this->add(&m_grid);
        this->m_grid.align(alignmask::EXPAND);

        /*
        template <class T>
            struct Key : public T
        {

            unsigned int code;
        };
        */

        vector<vector<string>> buttons =
        {
            {"q", "w", "e", "r", "t", "y", "u", "i", "o", "p" },
            {"", "a", "s", "d", "f", "g", "h", "j", "k", "l" },
            {"icons/arrow_up.png", "z", "x", "c", "v", "b", "n", "m", "", "icons/arrow_left.png"},
            {"123", ",", "", "", "     ", "", "", "", "", "."}
        };

        for (size_t r = 0; r < buttons.size(); r++)
        {
            for (size_t c = 0; c < buttons[r].size(); c++)
            {
                string label = buttons[r][c];
                if (label.empty())
                    continue;

                Button* b;

                if (label.find(".png") != string::npos)
                    b = new ImageButton(label);
                else
                    b = new Button(label);

                b->on_event([b](int event)
                {
                    if (!b->text().empty())
                    {
                        if (event == EVT_MOUSE_DOWN)
                        {
                            key_value() = b->text()[0];
                            IInput::dispatch(EVT_KEY_DOWN);
                        }
                        else if (event == EVT_MOUSE_UP)
                        {
                            key_value() = b->text()[0];
                            IInput::dispatch(EVT_KEY_UP);
                        }
                    }
                });

                this->m_grid.add(b, c, r);
            }
        }

        this->m_grid.reposition();
    }

protected:
    StaticGrid m_grid;
};

int main()
{
    Application app;

    set_image_path("../share/mui/");

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

    btn1.on_event([&popup](int event)
    {
        if (event == EVT_MOUSE_DOWN)
        {
            if (popup.visible())
                popup.hide();
            else
                popup.show(true);
        }
    });

    Button btn2("button 2", Rect(Point(200, 250), Size(100, 40)));
    win.add(&btn2);

    Keyboard<Window> keyboard;
    btn2.on_event([&keyboard](int event)
    {
        if (event == EVT_MOUSE_DOWN)
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
    IInput::global_input().on_event([&text1](int event)
    {
        switch (event)
        {
        case EVT_KEY_DOWN:
        case EVT_KEY_UP:
        case EVT_KEY_REPEAT:
            text1.handle(event);
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

    PieChart pie1(Rect(Point(600, 50), Size(200, 200)));
    win.add(&pie1);

    std::map<std::string, float> data;
    data.insert(make_pair("truck", .25));
    data.insert(make_pair("car", .55));
    data.insert(make_pair("bike", .10));
    data.insert(make_pair("motorcycle", .10));
    pie1.data(data);

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

    return app.run();
}
