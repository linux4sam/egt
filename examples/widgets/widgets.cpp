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

struct ButtonPage : public NotebookTab
{
    ButtonPage()
    {
        auto grid0 = new StaticGrid(3, 10, 5);
        grid0->set_align(alignmask::EXPAND);
        grid0->palette().set(Palette::BORDER, Palette::GROUP_NORMAL, Color::TRANSPARENT);
        add(grid0);

        auto btn1 = new Button("button 1", Rect(Point(), Size(100, 40)));
        grid0->add(btn1);

        auto btn2 = new Button("button 2");
        btn2->disable();
        grid0->add(btn2);

        auto imagebutton0 = new ImageButton(Image("@flag_red.png"), "Flag");
        grid0->add(imagebutton0);

        auto imagebutton1 = new ImageButton(Image("@flag_red.png"), "Flag");
        grid0->add(imagebutton1);
        imagebutton1->set_text_align(alignmask::CENTER | alignmask::LEFT);

        auto imagebutton2 = new ImageButton(Image("@flag_red.png"), "Flag");
        grid0->add(imagebutton2);
        imagebutton2->set_text_align(alignmask::CENTER | alignmask::RIGHT);

        auto imagebutton3 = new ImageButton(Image("@flag_red.png"), "Flag");
        grid0->add(imagebutton3);
        imagebutton3->set_text_align(alignmask::CENTER | alignmask::TOP);

        auto imagebutton4 = new ImageButton(Image("@flag_red.png"), "Flag");
        grid0->add(imagebutton4);
        imagebutton4->set_text_align(alignmask::CENTER | alignmask::BOTTOM);

        auto imagebutton5 = new ImageButton(Image("@flag_red.png"));
        grid0->add(imagebutton5);

    }
};

struct CheckBoxPage : public NotebookTab
{
    CheckBoxPage()
    {
        auto grid0 = new StaticGrid(3, 10, 5);
        grid0->set_align(alignmask::EXPAND);
        grid0->palette().set(Palette::BORDER, Palette::GROUP_NORMAL, Color::TRANSPARENT);
        add(grid0);

        auto sliding1 = new SlidingCheckBox;
        grid0->add(sliding1);

        auto sliding2 = new SlidingCheckBox;
        sliding2->check(true);
        grid0->add(sliding2);

        auto checkbox1 = new CheckBox("checkbox 1");
        grid0->add(checkbox1);

        auto checkbox2 = new CheckBox("checkbox 2");
        grid0->add(checkbox2);

        auto checkbox3 = new CheckBox("checkbox 3");
        grid0->add(checkbox3);

        auto checkbox_group = new ButtonGroup(true, false);
        checkbox_group->add(*checkbox1);
        checkbox_group->add(*checkbox2);
        checkbox_group->add(*checkbox3);
        checkbox_group->remove(*checkbox3);

        auto radiobox1 = new RadioBox("radiobox 1");
        grid0->add(radiobox1);

        auto radiobox2 = new RadioBox("radiobox 2");
        grid0->add(radiobox2);
        radiobox2->check(true);

        auto radiobox_group = new ButtonGroup(true);
        radiobox_group->add(*radiobox1);
        radiobox_group->add(*radiobox2);

    }
};

struct LabelPage : public NotebookTab
{
    LabelPage()
    {
        auto grid0 = new StaticGrid(3, 10, 5);
        grid0->set_align(alignmask::EXPAND);
        grid0->palette().set(Palette::BORDER, Palette::GROUP_NORMAL, Color::TRANSPARENT);
        add(grid0);

        auto label1 = new Label("left align", Rect(),
                                alignmask::LEFT | alignmask::CENTER, Font(), widgetmask::NONE);
        grid0->add(label1);

        auto label2 = new Label("right align", Rect(),
                                alignmask::RIGHT | alignmask::CENTER, Font(), widgetmask::NONE);
        grid0->add(label2);

        auto label3 = new Label("top align", Rect(),
                                alignmask::TOP | alignmask::CENTER, Font(), widgetmask::NONE);
        grid0->add(label3);

        auto label4 = new Label("bottom align", Rect(),
                                alignmask::BOTTOM | alignmask::CENTER, Font(), widgetmask::NONE);
        grid0->add(label4);

        auto imagelabel0 = new ImageLabel(Image("@bug.png"), "Bug");
        grid0->add(imagelabel0);

        auto imagelabel1 = new ImageLabel(Image("@phone.png"), "Phone");
        grid0->add(imagelabel1);
        imagelabel1->set_text_align(alignmask::CENTER);

        auto imagelabel2 = new ImageLabel(Image("@phone.png"), "Phone");
        grid0->add(imagelabel2);
        imagelabel2->set_text_align(alignmask::CENTER | alignmask::RIGHT);

        auto imagelabel3 = new ImageLabel(Image("@phone.png"), "Phone");
        grid0->add(imagelabel3);
        imagelabel3->set_text_align(alignmask::CENTER | alignmask::TOP);

        auto imagelabel4 = new ImageLabel(Image("@phone.png"), "Phone");
        grid0->add(imagelabel4);
        imagelabel4->set_text_align(alignmask::CENTER | alignmask::BOTTOM);

        auto imagelabel5 = new ImageLabel(Image("@phone.png"));
        grid0->add(imagelabel5);

    }
};

struct TextPage : public NotebookTab
{
    TextPage()
    {
        auto grid0 = new StaticGrid(3, 10, 5);
        grid0->set_align(alignmask::EXPAND);
        grid0->palette().set(Palette::BORDER, Palette::GROUP_NORMAL, Color::TRANSPARENT);
        add(grid0);

        auto text1 = new TextBox("text 1");
        grid0->add(text1);

        auto text2 = new TextBox("text 2 disabled");
        text2->disable();
        grid0->add(text2);
    }
};

struct ProgressPage : public NotebookTab
{
    ProgressPage()
    {
        auto grid0 = new StaticGrid(3, 10, 5);
        grid0->set_align(alignmask::EXPAND);
        grid0->palette().set(Palette::BORDER, Palette::GROUP_NORMAL, Color::TRANSPARENT);
        add(grid0);

        auto spinprogress = new SpinProgress;
        spinprogress->set_value(50);
        grid0->add(spinprogress);

        auto progressbar = new ProgressBar;
        progressbar->set_value(50);
        grid0->add(progressbar);
    }
};

struct SliderPage : public NotebookTab
{
    SliderPage()
    {
        auto hsizer = new BoxSizer(orientation::HORIZONTAL, 10);
        hsizer->set_align(alignmask::EXPAND);
        add(hsizer);

        auto slider1 = new Slider(Rect(0, 0, 200, 50));
        slider1->set_value(50);
        hsizer->add(slider1);

        auto slider2 = new Slider(Rect(0, 0, 50, 200), 0, 100, 0, orientation::VERTICAL);
        slider2->set_value(75);
        hsizer->add(slider2);
    }
};

struct MeterPage : public NotebookTab
{
    MeterPage()
    {
        auto grid0 = new StaticGrid(3, 3, 10);
        grid0->set_align(alignmask::EXPAND);
        grid0->palette().set(Palette::BORDER, Palette::GROUP_NORMAL, Color::TRANSPARENT);
        add(grid0);

        auto lp1 = new LevelMeter;
        grid0->add(lp1);

        auto am1 = new AnalogMeter;
        grid0->add(am1);

        auto tools = new CPUMonitorUsage;
        auto cputimer = new PeriodicTimer(std::chrono::seconds(1));
        cputimer->on_timeout([tools, lp1, am1]()
        {
            tools->update();
            lp1->set_value(tools->usage(0));
            am1->set_value(tools->usage(0));
        });
        cputimer->start();
    }
};

struct ComboPage : public NotebookTab
{
    ComboPage()
    {
        auto grid0 = new StaticGrid(3, 10, 5);
        grid0->set_align(alignmask::EXPAND);
        grid0->palette().set(Palette::BORDER, Palette::GROUP_NORMAL, Color::TRANSPARENT);
        add(grid0);

        ComboBox::item_array combo_items =
        {
            "item 1",
            "item 2",
            "item 3",
        };
        auto combo1 = new ComboBox(combo_items);
        grid0->add(combo1);
    }
};

int main(int argc, const char** argv)
{
    Application app(argc, argv, "widgets");

    TopWindow win;

    BoxSizer vsizer(orientation::VERTICAL);
    vsizer.set_align(alignmask::EXPAND);
    win.add(&vsizer);

    StaticGrid grid(Rect(Point(), Size(0, win.h() * 0.10)), 3, 1, 10);
    grid.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::RED);
    grid.set_boxtype(Theme::boxtype::fill);
    grid.flag_clear(widgetmask::NO_BACKGROUND);
    grid.set_align(alignmask::EXPAND_HORIZONTAL);

    ImageLabel logo(Image("@microchip_logo_white.png"));
    grid.add(&logo, 1, 0);

    vsizer.add(&grid);

    BoxSizer hsizer(orientation::HORIZONTAL);
    hsizer.set_align(alignmask::EXPAND);
    vsizer.add(&hsizer);

    ListBox list(Rect(Point(), Size(win.w() * 0.15, 0)));
    list.add_item(new StringItem("Buttons"));
    list.add_item(new StringItem("Text"));
    list.add_item(new StringItem("CheckBox"));
    list.add_item(new StringItem("Label"));
    list.add_item(new StringItem("Progress"));
    list.add_item(new StringItem("Sliders"));
    list.add_item(new StringItem("Meters"));
    list.add_item(new StringItem("Combo & List"));

    list.set_align(alignmask::EXPAND_VERTICAL | alignmask::LEFT);
    hsizer.add(&list);

    Notebook notebook;
    notebook.set_align(alignmask::EXPAND);
    hsizer.add(&notebook);
    notebook.add(new ButtonPage());
    notebook.add(new TextPage());
    notebook.add(new CheckBoxPage());
    notebook.add(new LabelPage());
    notebook.add(new ProgressPage());
    notebook.add(new SliderPage());
    notebook.add(new MeterPage());
    notebook.add(new ComboPage());

    list.on_event([&notebook, &list](eventid event)
    {
        ignoreparam(event);
        notebook.set_select(list.selected());
        return 1;
    }, {eventid::PROPERTY_CHANGED});

    win.show();

    app.dump(cout);

    return app.run();
}
