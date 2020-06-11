/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <cmath>
#include <egt/ui>
#include <gtest/gtest.h>
#include <random>
#include <regex>
#include <string>
#include <vector>

using ::testing::Combine;
using ::testing::TestWithParam;
using ::testing::Values;
using ::testing::Range;

struct ButtonPage : public egt::NotebookTab
{
    ButtonPage()
    {
        auto grid0 = std::make_shared<egt::StaticGrid>(egt::StaticGrid::GridSize(3, 6), 5);
        add(expand(grid0));

        grid0->add(egt::expand(std::make_shared<egt::Button>("Button", egt::Size(100, 40))));

        auto btn2 = std::make_shared<egt::Button>("Disabled");
        btn2->disable();
        grid0->add(egt::expand(btn2));

        grid0->add(expand(std::make_shared<egt::ImageButton>(egt::Image("icon:calculator.png"), "Calculator")));

        auto imagebutton1 = std::make_shared<egt::ImageButton>(egt::Image("icon:calculator.png"), "Calculator");
        grid0->add(expand(imagebutton1));
        imagebutton1->text_align(egt::AlignFlag::center | egt::AlignFlag::left);

        auto imagebutton2 = std::make_shared<egt::ImageButton>(egt::Image("icon:calculator.png"), "Calculator");
        grid0->add(expand(imagebutton2));
        imagebutton2->text_align(egt::AlignFlag::center | egt::AlignFlag::right);

        auto imagebutton3 = std::make_shared<egt::ImageButton>(egt::Image("icon:calculator.png"), "Calculator");
        grid0->add(expand(imagebutton3));
        imagebutton3->text_align(egt::AlignFlag::center | egt::AlignFlag::top);

        auto imagebutton4 = std::make_shared<egt::ImageButton>(egt::Image("icon:calculator.png"), "Calculator");
        grid0->add(expand(imagebutton4));
        imagebutton4->text_align(egt::AlignFlag::center | egt::AlignFlag::bottom);
        imagebutton4->image_align(egt::AlignFlag::right);

        auto imagebutton5 = std::make_shared<egt::ImageButton>(egt::Image("icon:calculator.png"));
        grid0->add(expand(imagebutton5));

        auto button6 = std::make_shared<egt::Button>("Multiline\nButton Text");
        grid0->add(egt::expand(button6));
    }
};

struct CheckBoxPage : public egt::NotebookTab
{
    CheckBoxPage()
    {
        auto grid0 = std::make_shared<egt::StaticGrid>(egt::StaticGrid::GridSize(3, 10), 5);
        add(egt::expand(grid0));

        auto toggle1 = std::make_shared<egt::ToggleBox>();
        toggle1->toggle_text("Off", "On");
        grid0->add(egt::expand(toggle1));

        auto toggle2 = std::make_shared<egt::ToggleBox>();
        toggle2->checked(true);
        grid0->add(egt::expand(toggle2));

        auto toggle3 = std::make_shared<egt::ToggleBox>();
        toggle3->disable();
        grid0->add(egt::expand(toggle3));

        auto checkbox1 = std::make_shared<egt::CheckBox>("checkbox 1");
        grid0->add(egt::expand(checkbox1));

        auto checkbox2 = std::make_shared<egt::CheckBox>("checkbox 2");
        grid0->add(egt::expand(checkbox2));

        auto checkbox3 = std::make_shared<egt::CheckBox>("checkbox 3");
        grid0->add(egt::expand(checkbox3));

        auto checkbox4 = std::make_shared<egt::CheckBox>("Disabled");
        checkbox4->disable();
        grid0->add(egt::expand(checkbox4));

        m_checkbox_group = std::make_unique<egt::ButtonGroup>(true, false);
        m_checkbox_group->add(checkbox1);
        m_checkbox_group->add(checkbox2);
        m_checkbox_group->add(checkbox3);
        m_checkbox_group->add(checkbox4);
        m_checkbox_group->remove(checkbox3.get());

        auto radiobox1 = std::make_shared<egt::RadioBox>("radiobox 1");
        grid0->add(egt::expand(radiobox1));

        auto radiobox2 = std::make_shared<egt::RadioBox>("radiobox 2");
        grid0->add(egt::expand(radiobox2));
        radiobox2->checked(true);

        auto radiobox3 = std::make_shared<egt::RadioBox>("Disabled");
        grid0->add(egt::expand(radiobox3));
        radiobox3->disable();

        m_radiobox_group = std::make_unique<egt::ButtonGroup>(true);
        m_radiobox_group->add(radiobox1);
        m_radiobox_group->add(radiobox2);
        m_radiobox_group->add(radiobox3);
    }

    std::unique_ptr<egt::ButtonGroup> m_checkbox_group;
    std::unique_ptr<egt::ButtonGroup> m_radiobox_group;
};

struct LabelPage : public egt::NotebookTab
{
    LabelPage()
    {
        auto grid0 = std::make_shared<egt::StaticGrid>(egt::StaticGrid::GridSize(3, 5), 5);
        add(egt::expand(grid0));

        auto label1 = std::make_shared<egt::Label>("left align",
                      egt::AlignFlag::left | egt::AlignFlag::center);
        grid0->add(egt::expand(label1));

        auto label2 = std::make_shared<egt::Label>("right align",
                      egt::AlignFlag::right | egt::AlignFlag::center);
        grid0->add(egt::expand(label2));

        auto label3 = std::make_shared<egt::Label>("top align",
                      egt::AlignFlag::top | egt::AlignFlag::center);
        grid0->add(egt::expand(label3));

        auto label4 = std::make_shared<egt::Label>("bottom align",
                      egt::AlignFlag::bottom | egt::AlignFlag::center);
        grid0->add(egt::expand(label4));

        auto label5 = std::make_shared<egt::Label>("Multiline\nLabel Text", egt::AlignFlag::center);
        grid0->add(egt::expand(label5));

        auto imagelabel0 = std::make_shared<egt::ImageLabel>(egt::Image("icon:lock.png"), "Lock");
        grid0->add(expand(imagelabel0));
        imagelabel0->image_align(egt::AlignFlag::top);
        imagelabel0->text_align(egt::AlignFlag::center);

        auto imagelabel1 = std::make_shared<egt::ImageLabel>(egt::Image("icon:camera.png"), "Camera");
        grid0->add(expand(imagelabel1));
        imagelabel1->text_align(egt::AlignFlag::center);

        auto imagelabel2 = std::make_shared<egt::ImageLabel>(egt::Image("icon:camera.png"), "Camera");
        grid0->add(expand(imagelabel2));
        imagelabel2->text_align(egt::AlignFlag::center | egt::AlignFlag::right);
        imagelabel2->image_align(egt::AlignFlag::right);

        auto imagelabel3 = std::make_shared<egt::ImageLabel>(egt::Image("icon:camera.png"), "Camera");
        grid0->add(expand(imagelabel3));
        imagelabel3->text_align(egt::AlignFlag::center | egt::AlignFlag::top);
        imagelabel3->image_align(egt::AlignFlag::bottom);

        auto imagelabel4 = std::make_shared<egt::ImageLabel>(egt::Image("icon:camera.png"), "Camera");
        imagelabel4->fill_flags(egt::Theme::FillFlag::blend);
        imagelabel4->border(1);
        grid0->add(egt::expand(imagelabel4));
        imagelabel4->text_align(egt::AlignFlag::center | egt::AlignFlag::bottom);

        auto imagelabel5 = std::make_shared<egt::ImageLabel>(egt::Image("icon:camera.png"));
        imagelabel5->fill_flags(egt::Theme::FillFlag::blend);
        imagelabel5->border(1);
        grid0->add(egt::expand(imagelabel5));
    }
};

struct TextPage : public egt::NotebookTab
{
    TextPage()
    {
        auto grid1 = std::make_shared<egt::StaticGrid>(egt::StaticGrid::GridSize(2, 1), 5);
        add(egt::expand(grid1));

        auto grid0 = std::make_shared<egt::StaticGrid>(egt::StaticGrid::GridSize(1, 10), 5);
        grid1->add(egt::expand(grid0));

        auto text1 = std::make_shared<egt::TextBox>("text 1");
        grid0->add(egt::expand(text1));

        auto text2 = std::make_shared<egt::TextBox>("text 2 disabled");
        text2->disable();
        grid0->add(egt::expand(text2));

        auto text3 = std::make_shared<egt::TextBox>("right aligned text");
        text3->fill_flags(egt::Theme::FillFlag::blend);
        text3->border_flags({egt::Theme::BorderFlag::bottom, egt::Theme::BorderFlag::top});
        text3->text_align(egt::AlignFlag::right);
        grid0->add(egt::expand(text3));

        auto text4 = std::make_shared<egt::TextBox>("text 4");
        text4->fill_flags(egt::Theme::FillFlag::blend);
        text4->border_flags(egt::Theme::BorderFlag::bottom);
        text4->disable();
        grid0->add(egt::expand(text4));

        auto text5 = std::make_shared<egt::TextBox>("all characters allowed");
        text5->add_validator_function([](std::string s) { return true; });
        text5->input_validation_enabled(true);
        grid0->add(egt::expand(text5));

        auto text6 = std::make_shared<egt::TextBox>("abc123 only");
        text6->add_validator_function([](std::string s)
        {
            return std::regex_match(s, std::regex("[abc123]*"));
        });
        text6->input_validation_enabled(true);
        grid0->add(egt::expand(text6));

        auto text7 = std::make_shared<egt::TextBox>(
                         u8"The Ensemble Graphics Toolkit (EGT) is a free and open-source C++ GUI widget "
                         " toolkit for Microchip AT91/SAMA5 microprocessors.  It is used to develop"
                         " graphical applications for Linux.  EGT provides modern and complete GUI"
                         " functionality, look-and-feel, and performance.\n\nThis multi-line TextBox fully"
                         " supports UTF-8 encoding.  See: \u2190\u2191\u2192\u2193",
                         egt::TextBox::TextFlags({egt::TextBox::TextFlag::multiline, egt::TextBox::TextFlag::word_wrap}));
        text7->selection(4, 25);
        grid1->add(egt::expand(text7));
    }
};

template<class T>
static std::unique_ptr<egt::AnimationSequence> demo_up_down_animator(std::shared_ptr<T> widget)
{
    auto animationup =
        std::make_shared<egt::PropertyAnimator>(0, 100,
                std::chrono::seconds(5),
                egt::easing_circular_easein);
    animationup->on_change([widget](egt::PropertyAnimator::Value value)
    {
        widget->value(value);
    });

    auto animationdown =
        std::make_shared<egt::PropertyAnimator>(100, 0,
                std::chrono::seconds(5),
                egt::easing_circular_easeout);
    animationdown->on_change([widget](egt::PropertyAnimator::Value value)
    {
        widget->value(value);
    });

    auto sequence = std::make_unique<egt::AnimationSequence>(true);
    sequence->add(animationup);
    sequence->add(animationdown);
    sequence->start();
    return sequence;
}

struct ProgressPage : public egt::NotebookTab
{
    ProgressPage()
    {
        auto grid0 = std::make_shared<egt::StaticGrid>(egt::StaticGrid::GridSize(2, 8), 5);
        add(egt::expand(grid0));

        auto spinprogress = std::make_shared<egt::SpinProgress>();
        grid0->add(egt::expand(spinprogress));

        auto progressbar = std::make_shared<egt::ProgressBar>();
        grid0->add(egt::expand(progressbar));

        auto progressbar1 = std::make_shared<egt::ProgressBar>();
        progressbar1->show_label(false);
        grid0->add(egt::expand(progressbar1));

        m_animators.push_back(demo_up_down_animator(spinprogress));
        m_animators.push_back(demo_up_down_animator(progressbar));
        m_animators.push_back(demo_up_down_animator(progressbar1));
    }

    std::vector<std::unique_ptr<egt::AnimationSequence>> m_animators;
};

struct SliderPage : public egt::NotebookTab
{
    SliderPage()
    {
        auto hsizer1 = std::make_shared<egt::BoxSizer>(egt::Orientation::flex);
        add(egt::expand(hsizer1));

        auto slider1 = std::make_shared<egt::Slider>(egt::Rect(0, 0, 200, 80));
        slider1->value(50);
        hsizer1->add(slider1);

        auto slider2 = std::make_shared<egt::Slider>(egt::Rect(0, 0, 80, 200), 0, 100, 0, egt::Orientation::vertical);
        slider2->value(75);
        hsizer1->add(slider2);

        auto slider3 = std::make_shared<egt::Slider>(egt::Rect(0, 0, 200, 80));
        slider3->value(50);
        slider3->slider_flags().set({egt::Slider::SliderFlag::round_handle,
                                     egt::Slider::SliderFlag::show_label});
        hsizer1->add(slider3);

        auto slider4 = std::make_shared<egt::Slider>(egt::Rect(0, 0, 80, 200), 0, 100, 0, egt::Orientation::vertical);
        slider4->value(75);
        slider4->slider_flags().set(egt::Slider::SliderFlag::round_handle);
        slider4->disable();
        hsizer1->add(slider4);

        auto slider5 = std::make_shared<egt::Slider>(egt::Rect(0, 0, 80, 200), 100, 200, 150, egt::Orientation::vertical);
        slider5->value(180);
        slider5->slider_flags().set({egt::Slider::SliderFlag::square_handle, egt::Slider::SliderFlag::show_labels});
        hsizer1->add(slider5);

        auto slider6 = std::make_shared<egt::Slider>(egt::Rect(0, 0, 200, 80), 100, 200, 150);
        slider6->slider_flags().set({egt::Slider::SliderFlag::rectangle_handle,
                                     egt::Slider::SliderFlag::show_labels});
        hsizer1->add(slider6);
    }
};

struct MeterPage : public egt::NotebookTab
{
    MeterPage()
    {
        auto grid0 = std::make_shared<egt::StaticGrid>(egt::StaticGrid::GridSize(2, 2), 10);
        add(egt::expand(grid0));

        auto lp1 = std::make_shared<egt::LevelMeter>();
        lp1->num_bars(10);
        grid0->add(egt::expand(lp1));

        auto am1 = std::make_shared<egt::AnalogMeter>();
        grid0->add(egt::expand(am1));

        auto r1 = std::make_shared<egt::experimental::Radial>();
        auto range0 = std::make_shared<egt::RangeValue<int>>(0, 100, 100);
        r1->add(range0, egt::Palette::grey, 10);
        auto range1 = std::make_shared<egt::RangeValue<int>>(0, 100, 0);
        r1->add(range1, egt::Palette::blue, 5, egt::experimental::Radial::RadialFlag::text_value);
        r1->readonly(true);
        grid0->add(egt::expand(r1));

        m_animators.push_back(demo_up_down_animator(lp1));
        m_animators.push_back(demo_up_down_animator(am1));
        m_animators.push_back(demo_up_down_animator(range1));
    }

    std::vector<std::unique_ptr<egt::AnimationSequence>> m_animators;
};

struct ComboPage : public egt::NotebookTab
{
    ComboPage()
    {
        auto vsizer =
            std::make_shared<egt::BoxSizer>(egt::Orientation::vertical);
        add(egt::expand(vsizer));

        {
            egt::ComboBox::ItemArray combo_items;
            for (auto x = 0; x < 5; x++)
                combo_items.push_back("item " + std::to_string(x));
            auto combo1 = std::make_shared<egt::ComboBox>(combo_items);
            combo1->margin(10);
            vsizer->add(combo1);

            auto combo2 = std::make_shared<egt::ComboBox>(combo_items);
            combo2->margin(10);
            combo2->disable();
            vsizer->add(combo2);
        }

        {
            egt::ComboBox::ItemArray combo_items;
            for (auto x = 0; x < 25; x++)
                combo_items.push_back("item " + std::to_string(x));
            auto combo3 = std::make_shared<egt::ComboBox>(combo_items);
            combo3->margin(10);
            vsizer->add(combo3);
        }
    }
};

struct ListPage : public egt::NotebookTab
{
    ListPage()
    {
        auto hsizer1 = std::make_shared<egt::BoxSizer>(egt::Orientation::horizontal,
                       egt::Justification::justify);
        hsizer1->padding(20);
        add(egt::expand(hsizer1));

        auto list0 = std::make_shared<egt::ListBox>(egt::Rect(0, 0, 200, 0));
        for (auto x = 0; x < 25; x++)
            list0->add_item(std::make_shared<egt::StringItem>("item " + std::to_string(x)));
        list0->align(egt::AlignFlag::expand_vertical);
        hsizer1->add(list0);

        auto list1 = std::make_shared<egt::ListBox>(egt::Rect(0, 0, 200, 300));
        for (auto x = 0; x < 5; x++)
            list1->add_item(std::make_shared<egt::StringItem>("item " + std::to_string(x), egt::Image("icon:ok.png")));
        hsizer1->add(list1);

        auto list2 = std::make_shared<egt::ListBox>(egt::Rect(0, 0, 200, 0));
        list2->add_item(std::make_shared<egt::StringItem>("Help", egt::Image("icon:help.png")));
        list2->add_item(std::make_shared<egt::StringItem>("Info", egt::Image("icon:info.png")));
        list2->add_item(std::make_shared<egt::StringItem>("Warning", egt::Image("icon:warning.png")));
        list2->height((list2->border() * 2) + (40 * list2->item_count()));
        hsizer1->add(list2);
    }
};

struct ScrollwheelPage : public egt::NotebookTab
{
    ScrollwheelPage()
    {
        auto hsizer1 =
            std::make_shared<egt::BoxSizer>(egt::Orientation::horizontal);
        add(egt::expand(hsizer1));

        auto scrollwheel_day =
            std::make_shared<egt::Scrollwheel>(egt::Rect(0, 0, 50, 100), 1, 31, 1);

        std::vector<std::string> months = { "January", "February", "March",
                                            "April", "May", "June", "July",
                                            "August", "September", "October",
                                            "November", "December"
                                          };
        auto scrollwheel_month =
            std::make_shared<egt::Scrollwheel>(egt::Rect(0, 0, 200, 100), months);
        scrollwheel_month->add_item("");

        auto scrollwheel_year =
            std::make_shared<egt::Scrollwheel>(egt::Rect(0, 0, 100, 100), 1900, 2019, 1);
        scrollwheel_year->reversed(true);

        auto label_day =
            std::make_shared<egt::Label>(scrollwheel_day->value(),
                                         egt::Rect(0, 0, 50, 30));

        auto label_month =
            std::make_shared<egt::Label>(scrollwheel_month->value(),
                                         egt::Rect(0, 0, 100, 30));

        auto label_year =
            std::make_shared<egt::Label>(scrollwheel_year->value(),
                                         egt::Rect(0, 0, 75, 30));

        scrollwheel_day->on_value_changed([label_day, scrollwheel_day]()
        {
            label_day->text(scrollwheel_day->value());
        });

        scrollwheel_month->on_value_changed([label_month, scrollwheel_month]()
        {
            label_month->text(scrollwheel_month->value());
        });

        scrollwheel_year->on_value_changed([label_year, scrollwheel_year]()
        {
            label_year->text(scrollwheel_year->value());
        });

        hsizer1->add(scrollwheel_day);
        hsizer1->add(scrollwheel_month);
        hsizer1->add(scrollwheel_year);

        hsizer1->add(label_day);
        hsizer1->add(label_month);
        hsizer1->add(label_year);

        scrollwheel_month->selected(4);
    }
};

struct FormPage : public egt::NotebookTab
{
    FormPage()
    {
        auto form = std::make_shared<egt::experimental::Form>("Information");
        form->align(egt::AlignFlag::expand_vertical);
        form->horizontal_ratio(50);
        add(form);

        form->add_group("Name");
        form->add_option("First Name", std::make_shared<egt::TextBox>());
        form->add_option("Last Name", std::make_shared<egt::TextBox>());
        form->add_group("Settings");
        form->add_option("Admin", std::make_shared<egt::CheckBox>());
        auto toggle1 = std::make_shared<egt::ToggleBox>();
        toggle1->toggle_text("On", "Off");
        form->add_option("Active", toggle1);
        form->add_option(std::make_shared<egt::Button>("Save"));
    }
};

struct ShapesPage : public egt::NotebookTab
{
    ShapesPage()
    {
        auto hsizer1 = std::make_shared<egt::BoxSizer>(egt::Orientation::flex);
        add(egt::expand(hsizer1));

        auto circle = std::make_shared<egt::CircleWidget>(egt::Circle(egt::Point(), 50));
        circle->margin(10);
        hsizer1->add(circle);

        auto hline = std::make_shared<egt::LineWidget>(egt::Size(100, 100));
        hline->margin(10);
        hsizer1->add(hline);

        auto vline = std::make_shared<egt::LineWidget>(egt::Size(100, 100));
        vline->margin(10);
        vline->horizontal(false);
        hsizer1->add(vline);

        auto rect = std::make_shared<egt::RectangleWidget>(egt::Size(100, 100));
        rect->margin(10);
        hsizer1->add(rect);
    }
};

class NoteBookTest : public testing::TestWithParam<int> {};

TEST_P(NoteBookTest, TestWidget)
{
    egt::Application app;
    egt::TopWindow win;
    std::shared_ptr<egt::Notebook> notebook;

    auto type = GetParam();
    if (type == 0)
    {
        EXPECT_NO_THROW(notebook.reset(new egt::Notebook()));
        EXPECT_NO_THROW(win.add(egt::expand(notebook)));
    }
    else
    {
        EXPECT_NO_THROW(notebook.reset(new egt::Notebook(win, egt::Rect(0, 0, 640, 400))));
        EXPECT_NO_THROW(notebook->move_to_center(win.center()));
    }

    auto bp = std::make_shared<ButtonPage>();
    auto tp = std::make_shared<TextPage>();
    auto cp = std::make_shared<CheckBoxPage>();
    auto lp = std::make_shared<LabelPage>();
    auto pp = std::make_shared<ProgressPage>();
    auto sp = std::make_shared<SliderPage>();
    auto mp = std::make_shared<MeterPage>();
    auto cbp = std::make_shared<ComboPage>();
    auto lbp = std::make_shared<ListPage>();
    auto swp = std::make_shared<ScrollwheelPage>();
    auto fp = std::make_shared<FormPage>();
    auto shp = std::make_shared<ShapesPage>();

    EXPECT_NO_THROW(notebook->add(bp));
    EXPECT_NO_THROW(notebook->add(tp));
    EXPECT_NO_THROW(notebook->add(cp));
    EXPECT_NO_THROW(notebook->add(lp));
    EXPECT_NO_THROW(notebook->add(pp));
    EXPECT_NO_THROW(notebook->add(sp));
    EXPECT_NO_THROW(notebook->add(mp));
    EXPECT_NO_THROW(notebook->add(cbp));
    EXPECT_NO_THROW(notebook->add(lbp));
    EXPECT_NO_THROW(notebook->add(swp));
    EXPECT_NO_THROW(notebook->add(fp));
    EXPECT_NO_THROW(notebook->add(shp));

    int scount = 0;
    int count = 0;
    notebook->on_selected_changed([&scount]()
    {
        ++scount;
    });

    egt::PeriodicTimer cputimer(std::chrono::milliseconds(1));
    cputimer.on_timeout([notebook, &cputimer, &app, &count, &scount, &win]()
    {
        auto rm = notebook->count_children();
        if (rm)
        {
            notebook->remove(notebook->get(rm - 1));
            count++;
        }
        else
        {
            EXPECT_EQ(notebook->selected(), -1);
            EXPECT_EQ(scount, count - 2);
            count = 0;
            scount = 0;

            //ASSERT_EXIT(notebook->remove(nullptr),::testing::KilledBySignal(SIGABRT),".*");
            EXPECT_THROW(notebook->add(notebook), std::runtime_error);
            EXPECT_THROW(notebook->add(win), std::invalid_argument);
            EXPECT_NO_THROW(win.remove(notebook.get()));
            cputimer.stop();
            app.quit();
        }
    });

    egt::PeriodicTimer cputimer1(std::chrono::milliseconds(1));
    cputimer1.on_timeout([notebook, &cputimer1, &count, &cputimer]()
    {
        if (count % 2)
        {
            EXPECT_NO_THROW(notebook->selected(count));
        }
        else
        {
            notebook->selected(notebook->child_at(count).get());
        }

        EXPECT_EQ(notebook->selected(), count);
        if (count >= 11)
        {
            cputimer1.stop();
            cputimer.start();
        }
        count++;
    });
    cputimer1.start();

    win.show();
    app.run();
}

INSTANTIATE_TEST_SUITE_P(NoteBookTestGroup, NoteBookTest, Range(0, 2));
