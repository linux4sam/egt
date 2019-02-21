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
        grid0->set_align(alignmask::expand);
        grid0->palette().set(Palette::ColorId::border, Palette::GroupId::normal, Palette::transparent);
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
        imagebutton1->set_text_align(alignmask::center | alignmask::left);

        auto imagebutton2 = new ImageButton(Image("@flag_red.png"), "Flag");
        grid0->add(imagebutton2);
        imagebutton2->set_text_align(alignmask::center | alignmask::right);

        auto imagebutton3 = new ImageButton(Image("@flag_red.png"), "Flag");
        grid0->add(imagebutton3);
        imagebutton3->set_text_align(alignmask::center | alignmask::top);

        auto imagebutton4 = new ImageButton(Image("@flag_red.png"), "Flag");
        grid0->add(imagebutton4);
        imagebutton4->set_text_align(alignmask::center | alignmask::bottom);

        auto imagebutton5 = new ImageButton(Image("@flag_red.png"));
        grid0->add(imagebutton5);

    }
};

struct CheckBoxPage : public NotebookTab
{
    CheckBoxPage()
    {
        auto grid0 = new StaticGrid(3, 10, 5);
        grid0->set_align(alignmask::expand);
        grid0->palette().set(Palette::ColorId::border, Palette::GroupId::normal, Palette::transparent);
        add(grid0);

        auto toggle1 = new ToggleBox;
        grid0->add(toggle1);

        auto toggle2 = new ToggleBox;
        toggle2->check(true);
        grid0->add(toggle2);

        auto toggle3 = new ToggleBox;
        toggle3->disable();
        grid0->add(toggle3);

        auto checkbox1 = new CheckBox("checkbox 1");
        grid0->add(checkbox1);

        auto checkbox2 = new CheckBox("checkbox 2");
        grid0->add(checkbox2);

        auto checkbox3 = new CheckBox("checkbox 3");
        grid0->add(checkbox3);

        auto checkbox4 = new CheckBox("checkbox 4");
        checkbox4->disable();
        grid0->add(checkbox4);

        auto checkbox_group = new ButtonGroup(true, false);
        checkbox_group->add(*checkbox1);
        checkbox_group->add(*checkbox2);
        checkbox_group->add(*checkbox3);
        checkbox_group->add(*checkbox4);
        checkbox_group->remove(*checkbox3);

        auto radiobox1 = new RadioBox("radiobox 1");
        grid0->add(radiobox1);

        auto radiobox2 = new RadioBox("radiobox 2");
        grid0->add(radiobox2);
        radiobox2->check(true);

        auto radiobox3 = new RadioBox("radiobox 3");
        grid0->add(radiobox3);
        radiobox3->disable();

        auto radiobox_group = new ButtonGroup(true);
        radiobox_group->add(*radiobox1);
        radiobox_group->add(*radiobox2);
        radiobox_group->add(*radiobox3);
    }
};

struct LabelPage : public NotebookTab
{
    LabelPage()
    {
        auto grid0 = new StaticGrid(3, 10, 5);
        grid0->set_align(alignmask::expand);
        grid0->palette().set(Palette::ColorId::border, Palette::GroupId::normal, Palette::transparent);
        add(grid0);

        auto label1 = new Label("left align", Rect(),
                                alignmask::left | alignmask::center, Font());
        grid0->add(label1);

        auto label2 = new Label("right align", Rect(),
                                alignmask::right | alignmask::center, Font());
        grid0->add(label2);

        auto label3 = new Label("top align", Rect(),
                                alignmask::top | alignmask::center, Font());
        grid0->add(label3);

        auto label4 = new Label("bottom align", Rect(),
                                alignmask::bottom | alignmask::center, Font());
        grid0->add(label4);

        auto imagelabel0 = new ImageLabel(Image("@bug.png"), "Bug");
        grid0->add(imagelabel0);

        auto imagelabel1 = new ImageLabel(Image("@phone.png"), "Phone");
        grid0->add(imagelabel1);
        imagelabel1->set_text_align(alignmask::center);

        auto imagelabel2 = new ImageLabel(Image("@phone.png"), "Phone");
        grid0->add(imagelabel2);
        imagelabel2->set_text_align(alignmask::center | alignmask::right);

        auto imagelabel3 = new ImageLabel(Image("@phone.png"), "Phone");
        grid0->add(imagelabel3);
        imagelabel3->set_text_align(alignmask::center | alignmask::top);

        auto imagelabel4 = new ImageLabel(Image("@phone.png"), "Phone");
        grid0->add(imagelabel4);
        imagelabel4->set_text_align(alignmask::center | alignmask::bottom);

        auto imagelabel5 = new ImageLabel(Image("@phone.png"));
        grid0->add(imagelabel5);

    }
};

struct TextPage : public NotebookTab
{
    TextPage()
    {
        auto grid1 = new StaticGrid(2, 1, 5);
        grid1->palette().set(Palette::ColorId::border, Palette::GroupId::normal, Palette::transparent);
        grid1->set_align(alignmask::expand);
        add(grid1);

        auto grid0 = new StaticGrid(1, 10, 5);
        grid0->set_align(alignmask::expand);
        grid0->palette().set(Palette::ColorId::border, Palette::GroupId::normal, Palette::transparent);
        grid1->add(grid0);

        auto text1 = new TextBox("text 1");
        grid0->add(text1);

        auto text2 = new TextBox("text 2 disabled");
        text2->disable();
        grid0->add(text2);

        auto text3 = new TextBox("text 3");
        text3->set_boxtype(Theme::boxtype::bottom_border);
        grid0->add(text3);

        auto text4 = new TextBox("text 4");
        text4->set_boxtype(Theme::boxtype::bottom_border);
        text4->disable();
        grid0->add(text4);

        auto text5 = new TextBox("Multiline\nText", Rect(), alignmask::left | alignmask::top);
        text5->text_flags().set({TextBox::flag::multiline, TextBox::flag::word_wrap});
        grid1->set_align(alignmask::expand);
        grid1->add(text5);
    }
};

template<class T>
static void demo_up_down_animator(T* widget)
{
    auto animationup = new PropertyAnimator(0, 100, std::chrono::seconds(5), easing_circular_easein);
    animationup->on_change(std::bind(&T::set_value, std::ref(*widget), std::placeholders::_1));

    auto animationdown = new PropertyAnimator(100, 0, std::chrono::seconds(5), easing_circular_easeout);
    animationdown->on_change(std::bind(&T::set_value, std::ref(*widget), std::placeholders::_1));

    auto animation = new AnimationSequence(true);
    animation->add(animationup);
    animation->add(animationdown);
    animation->start();
}

struct ProgressPage : public NotebookTab
{
    ProgressPage()
    {
        auto grid0 = new StaticGrid(3, 10, 5);
        grid0->set_align(alignmask::expand);
        grid0->palette().set(Palette::ColorId::border, Palette::GroupId::normal, Palette::transparent);
        add(grid0);

        auto spinprogress = new SpinProgress;
        grid0->add(spinprogress);

        auto progressbar = new ProgressBar;
        grid0->add(progressbar);

        demo_up_down_animator(spinprogress);
        demo_up_down_animator(progressbar);
    }
};

struct SliderPage : public NotebookTab
{
    SliderPage()
    {
        auto vsizer = new BoxSizer(orientation::vertical, 10, 10, 10);
        vsizer->set_align(alignmask::expand);
        add(vsizer);

        auto hsizer1 = new BoxSizer(orientation::horizontal, 10, 10, 0);
        hsizer1->set_align(alignmask::expand_horizontal);
        vsizer->add(hsizer1);

        auto slider1 = new Slider(Rect(0, 0, 200, 80));
        slider1->set_value(50);
        hsizer1->add(slider1);

        auto slider2 = new Slider(Rect(0, 0, 80, 200), 0, 100, 0, orientation::vertical);
        slider2->set_value(75);
        hsizer1->add(slider2);

        auto slider3 = new Slider(Rect(0, 0, 200, 80));
        slider3->set_value(50);
        slider3->slider_flags().set({Slider::flag::round_handle, Slider::flag::show_label});
        hsizer1->add(slider3);

        auto hsizer2 = new BoxSizer(orientation::horizontal, 10, 10, 0);
        hsizer2->set_align(alignmask::expand_horizontal);
        vsizer->add(hsizer2);

        auto slider4 = new Slider(Rect(0, 0, 80, 200), 0, 100, 0, orientation::vertical);
        slider4->set_value(75);
        slider4->slider_flags().set({Slider::flag::round_handle});
        slider4->disable();
        hsizer2->add(slider4);

        auto slider5 = new Slider(Rect(0, 0, 80, 200), 100, 200, 150, orientation::vertical);
        slider5->set_value(180);
        slider5->slider_flags().set({Slider::flag::square_handle, Slider::flag::show_labels});
        hsizer2->add(slider5);

        auto slider6 = new Slider(Rect(0, 0, 200, 80), 100, 200, 150);
        slider6->slider_flags().set({Slider::flag::rectangle_handle, Slider::flag::show_labels});
        hsizer2->add(slider6);
    }
};

struct MeterPage : public NotebookTab
{
    MeterPage()
    {
        auto grid0 = new StaticGrid(3, 3, 10);
        grid0->set_align(alignmask::expand);
        grid0->palette().set(Palette::ColorId::border, Palette::GroupId::normal, Palette::transparent);
        add(grid0);

        auto lp1 = new LevelMeter;
        lp1->set_num_bars(8);
        grid0->add(lp1);

        auto am1 = new AnalogMeter;
        grid0->add(am1);

        auto r1 = new Radial(Rect(), 0, 100, 0);
        r1->radial_flags().set({Radial::flag::primary_value,
                                Radial::flag::text_value,
                                Radial::flag::primary_handle});
        grid0->add(r1);

        demo_up_down_animator(lp1);
        demo_up_down_animator(am1);
        demo_up_down_animator(r1);
    }
};

struct ComboPage : public NotebookTab
{
    ComboPage()
    {
        auto hsizer1 = new BoxSizer(orientation::horizontal, 10, 10, 10);
        hsizer1->set_align(alignmask::expand);
        add(hsizer1);

        auto grid0 = new StaticGrid(Rect(0, 0, 200, 0), 1, 10, 5);
        grid0->set_align(alignmask::expand_vertical);
        grid0->palette().set(Palette::ColorId::border, Palette::GroupId::normal, Palette::transparent);
        hsizer1->add(grid0);

        {
            ComboBox::item_array combo_items;
            for (auto x = 0; x < 5; x++)
                combo_items.push_back("item " + std::to_string(x));
            auto combo1 = new ComboBox(combo_items);
            grid0->add(combo1);

            auto combo2 = new ComboBox(combo_items);
            combo2->disable();
            grid0->add(combo2);
        }

        {
            ComboBox::item_array combo_items;
            for (auto x = 0; x < 25; x++)
                combo_items.push_back("item " + std::to_string(x));
            auto combo3 = new ComboBox(combo_items);
            grid0->add(combo3);
        }
    }
};

struct ListPage : public NotebookTab
{
    ListPage()
    {
        auto hsizer1 = new BoxSizer(orientation::horizontal, 10, 10, 10);
        hsizer1->set_align(alignmask::expand);
        add(hsizer1);

        auto list0 = new ListBox(Rect(0, 0, 200, 0));
        for (auto x = 0; x < 25; x++)
            list0->add_item(new StringItem("item " + std::to_string(x)));
        list0->set_align(alignmask::expand_vertical | alignmask::left);
        hsizer1->add(list0);

        auto list1 = new ListBox(Rect(0, 0, 200, 300));
        for (auto x = 0; x < 5; x++)
            list1->add_item(new StringItem("item " + std::to_string(x)));
        list1->set_align(alignmask::left);
        hsizer1->add(list1);
    }
};

int main(int argc, const char** argv)
{
    Application app(argc, argv, "widgets");

    TopWindow win;

    BoxSizer vsizer(orientation::vertical);
    vsizer.set_align(alignmask::expand);
    win.add(&vsizer);

    StaticGrid grid(Rect(Point(), Size(0, win.h() * 0.10)), 3, 1, 10);
    grid.palette().set(Palette::ColorId::bg, Palette::GroupId::normal, Color(0xed2924ff));
    grid.set_boxtype(Theme::boxtype::fill);
    grid.set_align(alignmask::expand_horizontal);

    ImageLabel logo(Image("@microchip_logo_white.png"));
    grid.add(&logo, 1, 0);

    vsizer.add(&grid);

    BoxSizer hsizer(orientation::horizontal);
    hsizer.set_align(alignmask::expand);
    vsizer.add(&hsizer);

    ListBox list(Rect(Point(), Size(win.w() * 0.15, 0)));
    list.add_item(new StringItem("Buttons"));
    list.add_item(new StringItem("Text"));
    list.add_item(new StringItem("CheckBox"));
    list.add_item(new StringItem("Label"));
    list.add_item(new StringItem("Progress"));
    list.add_item(new StringItem("Sliders"));
    list.add_item(new StringItem("Meters"));
    list.add_item(new StringItem("ComboBox"));
    list.add_item(new StringItem("ListBox"));

    list.set_align(alignmask::expand_vertical | alignmask::left);
    hsizer.add(&list);

    Notebook notebook;
    notebook.set_align(alignmask::expand);
    hsizer.add(&notebook);
    notebook.add(new ButtonPage());
    notebook.add(new TextPage());
    notebook.add(new CheckBoxPage());
    notebook.add(new LabelPage());
    notebook.add(new ProgressPage());
    notebook.add(new SliderPage());
    notebook.add(new MeterPage());
    notebook.add(new ComboPage());
    notebook.add(new ListPage());

    list.on_event([&notebook, &list](eventid event)
    {
        ignoreparam(event);
        notebook.set_select(list.selected());
        return 1;
    }, {eventid::property_changed});

    win.show();

    return app.run();
}
