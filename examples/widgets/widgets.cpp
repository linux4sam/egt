/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <cmath>
#include <egt/themes/coconut.h>
#include <egt/themes/lapis.h>
#include <egt/themes/midnight.h>
#include <egt/themes/shamrock.h>
#include <egt/themes/sky.h>
#include <egt/themes/ultraviolet.h>
#include <egt/ui>
#include <regex>
#include <string>
#include <vector>

static egt::Size content_size_small(bool landscape, const egt::Size& tab_size)
{
    const auto side_min = std::min<egt::DefaultDim>(tab_size.width(), tab_size.height());
    const auto ratio = static_cast<double>(tab_size.width()) / tab_size.height();
    const auto size = landscape ? tab_size : egt::Size(side_min, side_min * ratio);

    return size * 0.9;
}

static egt::Size content_size_wide(bool landscape, const egt::Size& tab_size)
{
    return landscape ? tab_size * 0.9 : egt::Size(tab_size.width() * 0.9, tab_size.height() * 0.7);
}

struct ButtonPage : public egt::NotebookTab
{
    ButtonPage(bool landscape, const egt::Size& tab_size)
    {
        auto grid0 = std::make_shared<egt::StaticGrid>(egt::StaticGrid::GridSize(3, 3));
        grid0->resize(content_size_small(landscape, tab_size));
        grid0->horizontal_space(5);
        grid0->vertical_space(5);
        add(egt::center(grid0));

        grid0->add(egt::expand(std::make_shared<egt::Button>("Button", egt::Size(100, 40))));

        auto btn2 = std::make_shared<egt::Button>("Disabled");
        btn2->disable();
        grid0->add(egt::expand(btn2));

        grid0->add(expand(std::make_shared<egt::ImageButton>(egt::Image("icon:calculator.png"), "Calculator")));

        auto imagebutton1 = std::make_shared<egt::ImageButton>(egt::Image("icon:calculator.png"), "Calculator");
        grid0->add(expand(imagebutton1));
        imagebutton1->text_align(egt::AlignFlag::center_vertical | egt::AlignFlag::left);

        auto imagebutton2 = std::make_shared<egt::ImageButton>(egt::Image("icon:calculator.png"), "Calculator");
        grid0->add(expand(imagebutton2));
        imagebutton2->text_align(egt::AlignFlag::center_vertical | egt::AlignFlag::right);

        auto imagebutton3 = std::make_shared<egt::ImageButton>(egt::Image("icon:calculator.png"), "Calculator");
        grid0->add(expand(imagebutton3));
        imagebutton3->text_align(egt::AlignFlag::center_horizontal | egt::AlignFlag::top);

        auto imagebutton4 = std::make_shared<egt::ImageButton>(egt::Image("icon:calculator.png"), "Calculator");
        grid0->add(expand(imagebutton4));
        imagebutton4->text_align(egt::AlignFlag::center_horizontal | egt::AlignFlag::bottom);
        imagebutton4->image_align(egt::AlignFlag::right);

        auto imagebutton5 = std::make_shared<egt::ImageButton>(egt::Image("icon:calculator.png"));
        grid0->add(expand(imagebutton5));

        auto button6 = std::make_shared<egt::Button>("Multiline\nButton Text");
        grid0->add(egt::expand(button6));
    }
};

struct CheckBoxPage : public egt::NotebookTab
{
    CheckBoxPage(bool landscape, const egt::Size& tab_size)
    {
        auto grid0 = std::make_shared<egt::StaticGrid>(egt::StaticGrid::GridSize(3, 4));
        grid0->resize(content_size_small(landscape, tab_size));
        grid0->horizontal_space(5);
        grid0->vertical_space(5);
        add(egt::center(grid0));

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
    LabelPage(bool landscape, const egt::Size& tab_size)
    {
        auto grid0 = std::make_shared<egt::StaticGrid>(egt::StaticGrid::GridSize(3, 4));
        grid0->resize(content_size_small(landscape, tab_size));
        grid0->horizontal_space(5);
        grid0->vertical_space(5);
        add(egt::center(grid0));

        auto label1 = std::make_shared<egt::Label>("left align",
                      egt::AlignFlag::left | egt::AlignFlag::center_vertical);
        grid0->add(egt::expand(label1));

        auto label2 = std::make_shared<egt::Label>("right align",
                      egt::AlignFlag::right | egt::AlignFlag::center_vertical);
        grid0->add(egt::expand(label2));

        auto label3 = std::make_shared<egt::Label>("top align",
                      egt::AlignFlag::top | egt::AlignFlag::center_horizontal);
        grid0->add(egt::expand(label3));

        auto label4 = std::make_shared<egt::Label>("bottom align",
                      egt::AlignFlag::bottom | egt::AlignFlag::center_horizontal);
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
        imagelabel2->text_align(egt::AlignFlag::center_vertical | egt::AlignFlag::right);
        imagelabel2->image_align(egt::AlignFlag::right);

        auto imagelabel3 = std::make_shared<egt::ImageLabel>(egt::Image("icon:camera.png"), "Camera");
        grid0->add(expand(imagelabel3));
        imagelabel3->text_align(egt::AlignFlag::center_horizontal | egt::AlignFlag::top);
        imagelabel3->image_align(egt::AlignFlag::bottom);

        auto imagelabel4 = std::make_shared<egt::ImageLabel>(egt::Image("icon:camera.png"), "Camera");
        imagelabel4->fill_flags(egt::Theme::FillFlag::blend);
        imagelabel4->border(1);
        grid0->add(egt::expand(imagelabel4));
        imagelabel4->text_align(egt::AlignFlag::center_horizontal | egt::AlignFlag::bottom);

        auto imagelabel5 = std::make_shared<egt::ImageLabel>(egt::Image("icon:camera.png"));
        imagelabel5->fill_flags(egt::Theme::FillFlag::blend);
        imagelabel5->border(1);
        grid0->add(egt::expand(imagelabel5));
    }
};

struct TextPage : public egt::NotebookTab
{
    TextPage(bool landscape, const egt::Size& tab_size)
    {
        auto sizer = std::make_shared<egt::BoxSizer>();
        egt::Size content_size;
        if (!landscape)
            sizer->orient(egt::Orientation::vertical);
        sizer->resize(content_size_wide(landscape, tab_size));
        add(egt::center(sizer));

        const auto grid_size_cell = landscape ? egt::StaticGrid::GridSize(1, 6) : egt::StaticGrid::GridSize(2, 3);
        auto grid0 = std::make_shared<egt::StaticGrid>(grid_size_cell);
        grid0->margin(5);
        grid0->horizontal_space(5);
        grid0->vertical_space(5);
        sizer->add(egt::expand(grid0));

        auto text1 = std::make_shared<egt::TextBox>("text 1", egt::TextBox::TextFlag::fit_to_width);
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
        text5->add_validator_function([](const std::string&) { return true; });
        text5->input_validation_enabled(true);
        grid0->add(egt::expand(text5));

        auto text6 = std::make_shared<egt::TextBox>("abc123 only");
        text6->add_validator_function([](const std::string & s)
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
        text7->margin(5);
        sizer->add(egt::expand(text7));
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
    ProgressPage(bool landscape, const egt::Size& tab_size)
    {
        auto vsizer = std::make_shared<egt::VerticalBoxSizer>();
        add(egt::expand(vsizer));

        const auto grid_size_cell = landscape ? egt::StaticGrid::GridSize(2, 3) : egt::StaticGrid::GridSize(1, 6);
        auto grid0 = std::make_shared<egt::StaticGrid>(grid_size_cell);
        grid0->resize(content_size_wide(landscape, tab_size));
        grid0->horizontal_space(20);
        grid0->vertical_space(5);
        vsizer->add(egt::center(grid0));

        auto spinprogress = std::make_shared<egt::SpinProgress>();
        grid0->add(egt::expand(spinprogress));

        auto progressbar = std::make_shared<egt::ProgressBar>();
        progressbar->show_label(false);
        grid0->add(egt::expand(progressbar));

        auto progressbar1 = std::make_shared<egt::ProgressBar>();
        progressbar1->style(egt::ProgressBarStyle::left_to_right);
        grid0->add(egt::expand(progressbar1));

        auto progressbar2 = std::make_shared<egt::ProgressBar>();
        progressbar2->style(egt::ProgressBarStyle::right_to_left);
        grid0->add(egt::expand(progressbar2));

        auto progressbar3 = std::make_shared<egt::ProgressBar>();
        progressbar3->style(egt::ProgressBarStyle::top_to_bottom);
        grid0->add(egt::expand(progressbar3));

        auto progressbar4 = std::make_shared<egt::ProgressBar>();
        progressbar4->style(egt::ProgressBarStyle::bottom_to_top);
        grid0->add(egt::expand(progressbar4));

        auto hsizer = std::make_shared<egt::HorizontalBoxSizer>();
        hsizer->margin(20);
        vsizer->add(egt::expand_horizontal(hsizer));

        auto start = std::make_shared<egt::Button>("Start");
        start->margin(5);
        hsizer->add(start);
        start->on_click([this](egt::Event&)
        {
            for (auto& animator : m_animators)
                animator->start();
        });

        auto stop = std::make_shared<egt::Button>("Stop");
        stop->margin(5);
        hsizer->add(stop);
        stop->on_click([this](egt::Event&)
        {
            for (auto& animator : m_animators)
                animator->stop();
        });

        auto resume = std::make_shared<egt::Button>("Resume");
        resume->margin(5);
        hsizer->add(resume);
        resume->on_click([this](egt::Event&)
        {
            for (auto& animator : m_animators)
                animator->resume();
        });

        m_animators.push_back(demo_up_down_animator(spinprogress));
        m_animators.push_back(demo_up_down_animator(progressbar));
        m_animators.push_back(demo_up_down_animator(progressbar1));
        m_animators.push_back(demo_up_down_animator(progressbar2));
        m_animators.push_back(demo_up_down_animator(progressbar3));
        m_animators.push_back(demo_up_down_animator(progressbar4));
    }

    std::vector<std::unique_ptr<egt::AnimationSequence>> m_animators;
};

struct SliderPage : public egt::NotebookTab
{
    SliderPage(bool landscape, const egt::Size& tab_size)
    {
        const auto grid_size_cell = landscape ? egt::StaticGrid::GridSize(3, 3) : egt::StaticGrid::GridSize(2, 5);
        auto grid = std::make_shared<egt::StaticGrid>(grid_size_cell);
        grid->resize(content_size_wide(landscape, tab_size));
        grid->horizontal_space(10);
        grid->vertical_space(10);
        add(egt::center(grid));

        auto slider1 = std::make_shared<egt::Slider>();
        slider1->value(50);
        grid->add(egt::expand(slider1));

        auto slider2 = std::make_shared<egt::Slider>(0, 100, 0, egt::Orientation::vertical);
        slider2->value(75);
        grid->add(egt::expand(slider2));

        auto slider3 = std::make_shared<egt::SliderF>(100, 0);
        slider3->value(50);
        slider3->slider_flags().set({egt::SliderF::SliderFlag::round_handle,
                                     egt::SliderF::SliderFlag::show_label});
        grid->add(egt::expand(slider3));

        auto slider4 = std::make_shared<egt::Slider>(0, 100, 0, egt::Orientation::vertical);
        slider4->value(75);
        slider4->slider_flags().set(egt::Slider::SliderFlag::round_handle);
        slider4->disable();
        grid->add(egt::expand(slider4));

        auto slider5 = std::make_shared<egt::Slider>();
        slider5->orient(egt::Orientation::vertical);
        slider5->live_update(true);
        grid->add(egt::expand(slider5));

        auto slider6 = std::make_shared<egt::Slider>();
        slider6->orient(egt::Orientation::vertical);
        slider6->live_update(true);
        grid->add(egt::expand(slider6));

        slider5->on_value_changed([slider5, slider6]()
        {
            slider6->value(slider5->value());
        });

        slider6->on_value_changed([slider5, slider6]()
        {
            slider5->value(slider6->value());
        });

        auto slider7 = std::make_shared<egt::Slider>(100, 200, 150, egt::Orientation::vertical);
        slider7->value(180);
        slider7->margin(5);
        slider7->slider_flags().set({egt::Slider::SliderFlag::square_handle, egt::Slider::SliderFlag::show_labels});
        grid->add(egt::expand(slider7));

        auto slider8 = std::make_shared<egt::Slider>(100, 200, 150);
        slider8->slider_flags().set({egt::Slider::SliderFlag::rectangle_handle,
                                     egt::Slider::SliderFlag::show_labels});
        grid->add(egt::expand(slider8));

        auto slider9 = std::make_shared<egt::Slider>(0, 100);
        slider9->value(50);
        slider9->slider_flags().set({egt::Slider::SliderFlag::show_label,
                                     egt::Slider::SliderFlag::inverted});
        grid->add(egt::expand(slider9));
    }
};

struct MeterPage : public egt::NotebookTab
{
    MeterPage(bool landscape, const egt::Size& tab_size)
    {
        const auto grid_size_cell = landscape ? egt::StaticGrid::GridSize(2, 2) : egt::StaticGrid::GridSize(1, 3);
        auto grid0 = std::make_shared<egt::StaticGrid>(grid_size_cell);
        grid0->resize(content_size_wide(landscape, tab_size));
        grid0->horizontal_space(20);
        grid0->vertical_space(10);
        add(egt::center(grid0));

        auto lp1 = std::make_shared<egt::LevelMeter>();
        lp1->num_bars(10);
        grid0->add(egt::expand(lp1));

        auto am1 = std::make_shared<egt::AnalogMeter>();
        am1->margin(10);
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
                combo_items.push_back(std::make_shared<egt::StringItem>("item " + std::to_string(x)));
            auto combo1 = std::make_shared<egt::ComboBox>(combo_items);
            combo1->margin(10);
            vsizer->add(combo1);
        }

        {
            egt::ComboBox::ItemArray combo_items;
            for (auto x = 0; x < 5; x++)
                combo_items.push_back(std::make_shared<egt::StringItem>("item " + std::to_string(x)));
            auto combo2 = std::make_shared<egt::ComboBox>(combo_items);
            combo2->margin(10);
            combo2->disable();
            vsizer->add(combo2);
        }

        {
            egt::ComboBox::ItemArray combo_items;
            for (auto x = 0; x < 25; x++)
                combo_items.push_back(std::make_shared<egt::StringItem>("item " + std::to_string(x)));
            auto combo3 = std::make_shared<egt::ComboBox>(combo_items);
            combo3->margin(10);
            vsizer->add(combo3);
        }
    }
};

struct ListPage : public egt::NotebookTab
{
    ListPage(bool landscape, const egt::Size& tab_size)
    {
        egt::Size content_size = content_size_wide(landscape, tab_size);
        auto hsizer1 = std::make_shared<egt::BoxSizer>(egt::Orientation::horizontal,
                       egt::Justification::justify);
        hsizer1->resize(content_size);
        add(egt::center(hsizer1));

        auto list0 = std::make_shared<egt::ListBox>();
        for (auto x = 0; x < 25; x++)
            list0->add_item(std::make_shared<egt::StringItem>("item " + std::to_string(x)));
        list0->resize({content_size.width() / 3, content_size.height() });
        list0->margin(5);
        hsizer1->add(list0);

        auto list1 = std::make_shared<egt::ListBox>(egt::Rect(0, 0, 0, 300));
        for (auto x = 0; x < 5; x++)
            list1->add_item(std::make_shared<egt::StringItem>("item " + std::to_string(x), egt::Image("icon:ok.png")));
        list1->width(content_size.width() / 3);
        list1->margin(5);
        hsizer1->add(list1);

        auto list2 = std::make_shared<egt::ListBox>(egt::Rect(0, 0, 0, 0));
        list2->add_item(std::make_shared<egt::StringItem>("Help", egt::Image("icon:help.png")));
        list2->add_item(std::make_shared<egt::StringItem>("Info", egt::Image("icon:info.png")));
        list2->add_item(std::make_shared<egt::StringItem>("Warning", egt::Image("icon:warning.png")));
        list2->margin(5);
        list2->height((list2->moat() * 2) + (40 * list2->item_count()));
        list2->width(content_size.width() / 3);
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
            std::make_shared<egt::Scrollwheel>(egt::Rect(0, 0, 50, 100), 1, 32, 1);
        scrollwheel_day->remove_item("32");

        const egt::Scrollwheel::ItemArray months =
        {
            "January", "February", "March",
            "April", "May", "June", "July",
            "August", "September", "October",
            "November", "December"
        };

        auto scrollwheel_month =
            std::make_shared<egt::Scrollwheel>(egt::Rect(0, 0, 200, 100), months);
        scrollwheel_month->add_item("");

        auto scrollwheel_year =
            std::make_shared<egt::Scrollwheel>(egt::Rect(0, 0, 100, 100),
                                               1900, 2020, 1);
        scrollwheel_year->selected(120);
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

        auto weak_label_day = std::weak_ptr<egt::Label>(label_day);
        auto weak_scrollwheel_day = std::weak_ptr<egt::Scrollwheel>(scrollwheel_day);
        scrollwheel_day->on_value_changed([weak_label_day, weak_scrollwheel_day]()
        {
            auto label_day = weak_label_day.lock();
            auto scrollwheel_day = weak_scrollwheel_day.lock();
            if (label_day && scrollwheel_day)
                label_day->text(scrollwheel_day->value());
        });

        auto weak_label_month = std::weak_ptr<egt::Label>(label_month);
        auto weak_scrollwheel_month = std::weak_ptr<egt::Scrollwheel>(scrollwheel_month);
        scrollwheel_month->on_value_changed([weak_label_month, weak_scrollwheel_month]()
        {
            auto label_month = weak_label_month.lock();
            auto scrollwheel_month = weak_scrollwheel_month.lock();
            if (label_month && scrollwheel_month)
                label_month->text(scrollwheel_month->value());
        });

        auto weak_label_year = std::weak_ptr<egt::Label>(label_year);
        auto weak_scrollwheel_year = std::weak_ptr<egt::Scrollwheel>(scrollwheel_year);
        scrollwheel_year->on_value_changed([weak_label_year, weak_scrollwheel_year]()
        {
            auto label_year = weak_label_year.lock();
            auto scrollwheel_year = weak_scrollwheel_year.lock();
            if (label_year && scrollwheel_year)
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

struct ShapesPage : public egt::NotebookTab
{
    ShapesPage(bool landscape)
    {
        auto sizer = std::make_shared<egt::BoxSizer>(egt::Orientation::flex);
        if (landscape)
            sizer->orient(egt::Orientation::horizontal);
        else
            sizer->orient(egt::Orientation::vertical);

        add(egt::center(sizer));

        auto circle = std::make_shared<egt::CircleWidget>(egt::Circle(egt::Point(), 50));
        circle->margin(10);
        sizer->add(circle);

        auto hline = std::make_shared<egt::LineWidget>(egt::Size(100, 100));
        hline->margin(10);
        sizer->add(hline);

        auto vline = std::make_shared<egt::LineWidget>(egt::Size(100, 100));
        vline->margin(10);
        vline->horizontal(false);
        sizer->add(vline);

        auto rect = std::make_shared<egt::RectangleWidget>(egt::Size(100, 100));
        rect->margin(10);
        sizer->add(rect);
    }
};

int main(int argc, char** argv)
{
    egt::Application app(argc, argv);

    const auto screen_size = app.screen()->size();
    const auto landscape = screen_size.width() >= screen_size.height();
    auto tab_size = screen_size;

    egt::TopWindow win;

    egt::VerticalBoxSizer vsizer(win);
    egt::expand(vsizer);

    const auto header_height = screen_size.height() * 0.2;
    tab_size -= egt::Size(0, header_height);
    egt::HorizontalBoxSizer header;
    header.resize(egt::Size(0, header_height));
    vsizer.add(egt::expand_horizontal(header));

    egt::ImageLabel logo(egt::Image("icon:egt_logo_black.svg;svg"));
    if (landscape)
        logo.height(header_height * 0.8);
    else
        logo.height(header_height * 0.5);
    logo.align(egt::AlignFlag::center);
    header.add(egt::expand_horizontal(logo));

    const std::pair<std::string, std::function<std::unique_ptr<egt::Theme>()>> combo_items[] =
    {
        {"Default Theme", []{ return std::make_unique<egt::Theme>(); }},
        {"Lapis", []{ return std::make_unique<egt::LapisTheme>(); }},
        {"Midnight", []{ return std::make_unique<egt::MidnightTheme>(); }},
        {"Sky", []{ return std::make_unique<egt::SkyTheme>(); }},
        {"Shamrock", []{ return std::make_unique<egt::ShamrockTheme>(); }},
        {"Coconut", []{ return std::make_unique<egt::CoconutTheme>(); }},
        {"Ultra Violet", []{ return std::make_unique<egt::UltraVioletTheme>(); }}
    };

    egt::ComboBox combo(egt::Rect(egt::Size(screen_size.width() / 4, 25)));
    for (const auto& i : combo_items)
        combo.add_item(std::make_shared<egt::StringItem>(i.first));
    combo.align(egt::AlignFlag::center_vertical | egt::AlignFlag::right);
    combo.margin(5);
    header.add(combo);

    combo.on_selected_changed([&combo_items, &combo, &win]()
    {
        const auto s = combo.item_at(combo.selected());
        const auto i = std::find_if(begin(combo_items),
                                    end(combo_items),
                                    [&s](const auto & element)
        {
            return s->text() == element.first;
        });
        if (i != end(combo_items))
            global_theme(i->second());

        win.damage();
    });

    egt::BoxSizer content_sizer;
    if (!landscape)
        content_sizer.orient(egt::Orientation::vertical);
    vsizer.add(egt::expand(content_sizer));

    egt::ListBox list;
    if (!landscape)
    {
        auto const list_height = std::max<egt::DefaultDim>(50, screen_size.height() * 0.05);
        tab_size -= egt::Size(0, list_height);
        list.resize(egt::Size(0, list_height));
        list.orient(egt::Orientation::horizontal);
    }
    else
    {
        auto const list_width = std::max<egt::DefaultDim>(150, screen_size.width() * 0.2);
        tab_size -= egt::Size(list_width, 0);
        list.resize(egt::Size(list_width, 0));
    }

    egt::Notebook notebook;

    const std::pair<std::string, std::shared_ptr<egt::NotebookTab>> pages[] =
    {
        {"Buttons", std::make_shared<ButtonPage>(landscape, tab_size)},
        {"Text", std::make_shared<TextPage>(landscape, tab_size)},
        {"CheckBox", std::make_shared<CheckBoxPage>(landscape, tab_size)},
        {"Label", std::make_shared<LabelPage>(landscape, tab_size)},
        {"Progress", std::make_shared<ProgressPage>(landscape, tab_size)},
        {"Sliders", std::make_shared<SliderPage>(landscape, tab_size)},
        {"Meters", std::make_shared<MeterPage>(landscape, tab_size)},
        {"ComboBox", std::make_shared<ComboPage>()},
        {"ListBox", std::make_shared<ListPage>(landscape, tab_size)},
        {"Scrollwheel", std::make_shared<ScrollwheelPage>()},
        {"Shapes", std::make_shared<ShapesPage>(landscape)},
    };

    for (const auto& i : pages)
    {
        list.add_item(std::make_shared<egt::StringItem>(i.first));
        notebook.add(i.second);
    }

    if (landscape)
        egt::expand_vertical(list);
    else
        egt::expand_horizontal(list);
    content_sizer.add(list);
    content_sizer.add(egt::expand(notebook));

    list.on_selected_changed([&notebook, &list]()
    {
        notebook.selected(list.selected());
    });

#ifdef EGT_HAS_VIRTUALKEYBOARD
    auto default_keyboard = std::make_shared<egt::VirtualKeyboard>();
    egt::Size keyboard_size;
    if (!landscape)
        keyboard_size = egt::Size(screen_size.width(), screen_size.height() * 0.25);
    egt::PopupVirtualKeyboard popup_keyboard {default_keyboard, keyboard_size};
    win.add(popup_keyboard);
#endif

    win.show();

    return app.run();
}
