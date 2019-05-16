/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <iostream>
#include <map>
#include <math.h>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace egt;

struct ButtonPage : public NotebookTab
{
    ButtonPage()
    {
        auto grid0 = make_shared<StaticGrid>(Tuple(3, 10), 5);
        add(expand(grid0));

        grid0->add(expand(make_shared<Button>("button 1", Size(100, 40))));

        auto btn2 = make_shared<Button>("button 2");
        btn2->disable();
        grid0->add(expand(btn2));

        grid0->add(expand(make_shared<ImageButton>(Image("@flag_red.png"), "Flag")));

        auto imagebutton1 = make_shared<ImageButton>(Image("@flag_red.png"), "Flag");
        grid0->add(expand(imagebutton1));
        imagebutton1->set_text_align(alignmask::center | alignmask::left);

        auto imagebutton2 = make_shared<ImageButton>(Image("@flag_red.png"), "Flag");
        grid0->add(expand(imagebutton2));
        imagebutton2->set_text_align(alignmask::center | alignmask::right);

        auto imagebutton3 = make_shared<ImageButton>(Image("@flag_red.png"), "Flag");
        grid0->add(expand(imagebutton3));
        imagebutton3->set_text_align(alignmask::center | alignmask::top);

        auto imagebutton4 = make_shared<ImageButton>(Image("@flag_red.png"), "Flag");
        grid0->add(expand(imagebutton4));
        imagebutton4->set_text_align(alignmask::center | alignmask::bottom);

        auto imagebutton5 = make_shared<ImageButton>(Image("@flag_red.png"));
        grid0->add(expand(imagebutton5));
    }
};

struct CheckBoxPage : public NotebookTab
{
    CheckBoxPage()
    {
        auto grid0 = std::make_shared<StaticGrid>(Tuple(3, 10), 5);
        add(expand(grid0));

        auto toggle1 = std::make_shared<ToggleBox>();
        toggle1->set_text("On", "Off");
        grid0->add(expand(toggle1));

        auto toggle2 = std::make_shared<ToggleBox>();
        toggle2->check(true);
        grid0->add(expand(toggle2));

        auto toggle3 = std::make_shared<ToggleBox>();
        toggle3->disable();
        grid0->add(expand(toggle3));

        auto checkbox1 = std::make_shared<CheckBox>("checkbox 1");
        grid0->add(expand(checkbox1));

        auto checkbox2 = std::make_shared<CheckBox>("checkbox 2");
        grid0->add(expand(checkbox2));

        auto checkbox3 = std::make_shared<CheckBox>("checkbox 3");
        grid0->add(expand(checkbox3));

        auto checkbox4 = std::make_shared<CheckBox>("checkbox 4");
        checkbox4->disable();
        grid0->add(expand(checkbox4));

        /// @todo leak
        auto checkbox_group = new ButtonGroup(true, false);
        checkbox_group->add(*checkbox1);
        checkbox_group->add(*checkbox2);
        checkbox_group->add(*checkbox3);
        checkbox_group->add(*checkbox4);
        checkbox_group->remove(*checkbox3);

        auto radiobox1 = std::make_shared<RadioBox>("radiobox 1");
        grid0->add(expand(radiobox1));

        auto radiobox2 = std::make_shared<RadioBox>("radiobox 2");
        grid0->add(expand(radiobox2));
        radiobox2->check(true);

        auto radiobox3 = std::make_shared<RadioBox>("radiobox 3");
        grid0->add(expand(radiobox3));
        radiobox3->disable();

        /// @todo leak
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
        auto grid0 = std::make_shared<StaticGrid>(Tuple(3, 10), 5);
        add(expand(grid0));

        auto label1 = std::make_shared<Label>("left align",
                                              alignmask::left | alignmask::center);
        label1->set_boxtype(Theme::boxtype::blank);
        label1->set_border(1);
        grid0->add(expand(label1));

        auto label2 = std::make_shared<Label>("right align",
                                              alignmask::right | alignmask::center);
        label2->set_boxtype(Theme::boxtype::blank);
        label2->set_border(1);
        grid0->add(expand(label2));

        auto label3 = std::make_shared<Label>("top align",
                                              alignmask::top | alignmask::center);
        label3->set_boxtype(Theme::boxtype::blank);
        label3->set_border(1);
        grid0->add(expand(label3));

        auto label4 = std::make_shared<Label>("bottom align",
                                              alignmask::bottom | alignmask::center);
        label4->set_boxtype(Theme::boxtype::blank);
        label4->set_border(1);
        grid0->add(expand(label4));

        auto imagelabel0 = std::make_shared<ImageLabel>(Image("@bug.png"), "Bug");
        imagelabel0->set_boxtype(Theme::boxtype::blank);
        imagelabel0->set_border(1);
        grid0->add(expand(imagelabel0));

        auto imagelabel1 = std::make_shared<ImageLabel>(Image("@phone.png"), "Phone");
        imagelabel1->set_boxtype(Theme::boxtype::blank);
        imagelabel1->set_border(1);
        grid0->add(expand(imagelabel1));
        imagelabel1->set_text_align(alignmask::center);

        auto imagelabel2 = std::make_shared<ImageLabel>(Image("@phone.png"), "Phone");
        imagelabel2->set_boxtype(Theme::boxtype::blank);
        imagelabel2->set_border(1);
        grid0->add(expand(imagelabel2));
        imagelabel2->set_text_align(alignmask::center | alignmask::right);

        auto imagelabel3 = std::make_shared<ImageLabel>(Image("@phone.png"), "Phone");
        imagelabel3->set_boxtype(Theme::boxtype::blank);
        imagelabel3->set_border(1);
        grid0->add(expand(imagelabel3));
        imagelabel3->set_text_align(alignmask::center | alignmask::top);

        auto imagelabel4 = std::make_shared<ImageLabel>(Image("@phone.png"), "Phone");
        imagelabel4->set_boxtype(Theme::boxtype::blank);
        imagelabel4->set_border(1);
        grid0->add(expand(imagelabel4));
        imagelabel4->set_text_align(alignmask::center | alignmask::bottom);

        auto imagelabel5 = std::make_shared<ImageLabel>(Image("@phone.png"));
        imagelabel5->set_boxtype(Theme::boxtype::blank);
        imagelabel5->set_border(1);
        grid0->add(expand(imagelabel5));
    }
};

struct TextPage : public NotebookTab
{
    TextPage()
    {
        auto grid1 = std::make_shared<StaticGrid>(Tuple(2, 1), 5);
        add(expand(grid1));

        auto grid0 = std::make_shared<StaticGrid>(Tuple(1, 10), 5);
        grid1->add(expand(grid0));

        auto text1 = std::make_shared<TextBox>("text 1");
        grid0->add(expand(text1));

        auto text2 = std::make_shared<TextBox>("text 2 disabled");
        text2->disable();
        grid0->add(expand(text2));

        auto text3 = std::make_shared<TextBox>("right aligned text");
        text3->set_boxtype(Theme::boxtype::fill | Theme::boxtype::border_bottom);
        text3->set_text_align(alignmask::right);
        grid0->add(expand(text3));

        auto text4 = std::make_shared<TextBox>("text 4");
        text4->set_boxtype(Theme::boxtype::fill | Theme::boxtype::border_bottom);
        text4->disable();
        grid0->add(expand(text4));

        auto text5 = std::make_shared<TextBox>("all characters allowed");
        text5->add_validator_function([this](std::string s) { return true; });
        text5->enable_input_validation();
        grid0->add(expand(text5));

        auto text6 = std::make_shared<TextBox>("abc123 only");
        text6->add_validator_function([this](std::string s)
        {
            return std::regex_match(s, regex("[abc123]*"));
        });
        text6->enable_input_validation();
        grid0->add(expand(text6));

        auto text7 = std::make_shared<TextBox>(u8"This is a utf-8 \u2190\u2191\u2192\u2193\nMulti-line\nTextBox", Rect(), alignmask::left | alignmask::top);
        text7->set_selection(2, 5);
        text7->text_flags().set({TextBox::flag::multiline, TextBox::flag::word_wrap});
        grid1->add(expand(text7));
    }
};

template<class T>
static void demo_up_down_animator(std::shared_ptr<T> widget)
{
    auto animationup = std::make_shared<PropertyAnimator>(0, 100, std::chrono::seconds(5), easing_circular_easein);
    animationup->on_change(std::bind(&T::set_value, std::ref(*widget), std::placeholders::_1));

    auto animationdown = std::make_shared<PropertyAnimator>(100, 0, std::chrono::seconds(5), easing_circular_easeout);
    animationdown->on_change(std::bind(&T::set_value, std::ref(*widget), std::placeholders::_1));

    /// @todo leak
    auto sequence = new AnimationSequence(true);
    sequence->add(animationup);
    sequence->add(animationdown);
    sequence->start();
}

struct ProgressPage : public NotebookTab
{
    ProgressPage()
    {
        auto grid0 = std::make_shared<StaticGrid>(Tuple(2, 8), 5);
        add(expand(grid0));

        auto spinprogress = std::make_shared<SpinProgress>();
        grid0->add(expand(spinprogress));

        auto progressbar = std::make_shared<ProgressBar>();
        grid0->add(expand(progressbar));

        demo_up_down_animator(spinprogress);
        demo_up_down_animator(progressbar);
    }
};

struct SliderPage : public NotebookTab
{
    SliderPage()
    {
        auto hsizer1 = std::make_shared<BoxSizer>(orientation::flex);
        add(expand(hsizer1));

        auto slider1 = std::make_shared<Slider>(Rect(0, 0, 200, 80));
        slider1->set_value(50);
        hsizer1->add(slider1);

        auto slider2 = std::make_shared<Slider>(Rect(0, 0, 80, 200), 0, 100, 0, orientation::vertical);
        slider2->set_value(75);
        hsizer1->add(slider2);

        auto slider3 = std::make_shared<Slider>(Rect(0, 0, 200, 80));
        slider3->set_value(50);
        slider3->slider_flags().set({Slider::flag::round_handle, Slider::flag::show_label});
        hsizer1->add(slider3);

        auto slider4 = std::make_shared<Slider>(Rect(0, 0, 80, 200), 0, 100, 0, orientation::vertical);
        slider4->set_value(75);
        slider4->slider_flags().set({Slider::flag::round_handle});
        slider4->disable();
        hsizer1->add(slider4);

        auto slider5 = std::make_shared<Slider>(Rect(0, 0, 80, 200), 100, 200, 150, orientation::vertical);
        slider5->set_value(180);
        slider5->slider_flags().set({Slider::flag::square_handle, Slider::flag::show_labels});
        hsizer1->add(slider5);

        auto slider6 = std::make_shared<Slider>(Rect(0, 0, 200, 80), 100, 200, 150);
        slider6->slider_flags().set({Slider::flag::rectangle_handle, Slider::flag::show_labels});
        hsizer1->add(slider6);
    }
};

struct MeterPage : public NotebookTab
{
    MeterPage()
    {
        auto grid0 = std::make_shared<StaticGrid>(Tuple(3, 3), 10);
        add(expand(grid0));

        auto lp1 = std::make_shared<LevelMeter>();
        lp1->set_num_bars(10);
        grid0->add(expand(lp1));

        auto am1 = std::make_shared<AnalogMeter>();
        grid0->add(expand(am1));

        auto r1 = std::make_shared<Radial>(Rect(), 0, 100, 0);
        r1->radial_flags().set({Radial::flag::primary_value,
                                Radial::flag::text_value,
                                Radial::flag::primary_handle});
        grid0->add(expand(r1));

        demo_up_down_animator(lp1);
        demo_up_down_animator(am1);
        demo_up_down_animator(r1);
    }
};

struct ComboPage : public NotebookTab
{
    ComboPage()
    {
        auto grid = std::make_shared<StaticGrid>(Tuple(1, 3));
        add(expand(grid));

        {
            ComboBox::item_array combo_items;
            for (auto x = 0; x < 5; x++)
                combo_items.push_back("item " + std::to_string(x));
            auto combo1 = std::make_shared<ComboBox>(combo_items);
            grid->add(combo1);

            auto combo2 = std::make_shared<ComboBox>(combo_items);
            combo2->disable();
            grid->add(combo2);
        }

        {
            ComboBox::item_array combo_items;
            for (auto x = 0; x < 25; x++)
                combo_items.push_back("item " + std::to_string(x));
            auto combo3 = std::make_shared<ComboBox>(combo_items);
            grid->add(combo3);
        }
    }
};

struct ListPage : public NotebookTab
{
    ListPage()
    {
        auto hsizer1 = std::make_shared<BoxSizer>(orientation::horizontal, justification::middle);
        add(expand(hsizer1));

        auto list0 = std::make_shared<ListBox>(Rect(0, 0, 200, 0));
        for (auto x = 0; x < 25; x++)
            list0->add_item(std::make_shared<StringItem>("item " + std::to_string(x)));
        list0->set_align(alignmask::expand_vertical | alignmask::left);
        hsizer1->add(list0);

        auto list1 = std::make_shared<ListBox>(Rect(0, 0, 200, 300));
        for (auto x = 0; x < 5; x++)
            list1->add_item(std::make_shared<StringItem>("item " + std::to_string(x)));
        hsizer1->add(left(list1));
    }
};

struct ScrollwheelPage : public NotebookTab
{
    ScrollwheelPage()
    {
        auto hsizer1 =
            std::make_shared<BoxSizer>(orientation::horizontal);
        add(expand(hsizer1));

        auto scrollwheel_day =
            std::make_shared<Scrollwheel>(Rect(0, 0, 50, 75), 1, 31, 1);

        std::vector<std::string> months = { "January", "February", "March",
                                            "April", "May", "June", "July", "August", "September", "October",
                                            "November", "December"
                                          };
        auto scrollwheel_month =
            std::make_shared<Scrollwheel>(Rect(0, 0, 200, 75), months);
        scrollwheel_month->add_item("");

        auto scrollwheel_year =
            std::make_shared<Scrollwheel>(Rect(0, 0, 100, 75),
                                          1900, 2019, 1, true);

        auto label_day =
            std::make_shared<Label>(scrollwheel_day->get_value(),
                                    Rect(0, 0, 50, 30));

        auto label_month =
            std::make_shared<Label>(scrollwheel_month->get_value(),
                                    Rect(0, 0, 100, 30));

        auto label_year =
            std::make_shared<Label>(scrollwheel_year->get_value(),
                                    Rect(0, 0, 75, 30));

        scrollwheel_day->on_event([label_day, scrollwheel_day](Event&)
        {
            label_day->set_text(scrollwheel_day->get_value());
        }, {eventid::property_changed});

        scrollwheel_month->on_event([label_month, scrollwheel_month](Event&)
        {
            label_month->set_text(scrollwheel_month->get_value());
        }, {eventid::property_changed});

        scrollwheel_year->on_event([label_year, scrollwheel_year](Event&)
        {
            label_year->set_text(scrollwheel_year->get_value());
        }, {eventid::property_changed});

        hsizer1->add(scrollwheel_day);
        hsizer1->add(scrollwheel_month);
        hsizer1->add(scrollwheel_year);

        hsizer1->add(label_day);
        hsizer1->add(label_month);
        hsizer1->add(label_year);

        scrollwheel_month->set_index(4);
    }
};

int main(int argc, const char** argv)
{
    Application app(argc, argv, "widgets");

    TopWindow win;

    VerticalBoxSizer vsizer(win);
    expand(vsizer);

    auto logo = make_shared<ImageLabel>(Image("@egt_logo_black.png"));
    logo->set_margin(5);
    logo->set_align(alignmask::center_horizontal);
    vsizer.add(logo);

    BoxSizer hsizer(orientation::horizontal);
    vsizer.add(expand(hsizer));

    auto list = make_shared<ListBox>();
    list->resize(Size(150, 0));

    auto notebook = make_shared<Notebook>();

    map<string, shared_ptr<NotebookTab>> pages =
    {
        {"Buttons", make_shared<ButtonPage>()},
        {"Text", make_shared<TextPage>()},
        {"CheckBox", make_shared<CheckBoxPage>()},
        {"Label", make_shared<LabelPage>()},
        {"Progress", make_shared<ProgressPage>()},
        {"Sliders", make_shared<SliderPage>()},
        {"Meters", make_shared<MeterPage>()},
        {"ComboBox", make_shared<ComboPage>()},
        {"ListBox", make_shared<ListPage>()},
        {"Scrollwheel", make_shared<ScrollwheelPage>()},
    };

    for (auto& i : pages)
    {
        list->add_item(make_shared<StringItem>(i.first));
        notebook->add(i.second);
    }

    hsizer.add(expand_vertical(list));
    hsizer.add(expand(notebook));

    list->on_event([&notebook, &list](Event&)
    {
        notebook->set_select(list->selected());
    }, {eventid::property_changed});

    win.show();

    return app.run();
}
