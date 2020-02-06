/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <map>
#include <string>
#include <vector>

static std::vector<std::pair<egt::EasingFunc, std::string>> easing_functions =
{
    {egt::easing_linear, "linear"},
    {egt::easing_easy, "easy"},
    {egt::easing_easy_slow, "easy slow"},
    {egt::easing_extend, "extend"},
    {egt::easing_drop, "drop"},
    {egt::easing_drop_slow, "drop slow"},
    {egt::easing_snap, "snap"},
    {egt::easing_bounce, "bounce"},
    {egt::easing_bouncy, "bouncy"},
    {egt::easing_rubber, "rubber"},
    {egt::easing_spring, "spring"},
    {egt::easing_boing, "boing"},
    {egt::easing_quadratic_easein, "quadratic easein"},
    {egt::easing_quadratic_easeout, "quadratic easeout"},
    {egt::easing_quadratic_easeinout, "quadratic easeinout"},
    {egt::easing_cubic_easein, "cubic easein"},
    {egt::easing_cubic_easeout, "cubic easeout"},
    {egt::easing_cubic_easeinout, "cubic easeinout"},
    {egt::easing_quartic_easein, "quartic easein"},
    {egt::easing_quartic_easeout, "quartic easeout"},
    {egt::easing_quartic_easeinout, "quartic easeinout"},
    {egt::easing_quintic_easein, "quintic easein"},
    {egt::easing_quintic_easeout, "quintic easeout"},
    {egt::easing_quintic_easeinout, "quintic easeinout"},
    {egt::easing_sine_easein, "sine easein"},
    {egt::easing_sine_easeout, "sine easeout"},
    {egt::easing_sine_easeinout, "sine easeinout"},
    {egt::easing_circular_easein, "circular easein"},
    {egt::easing_circular_easeout, "circular easeout"},
    {egt::easing_circular_easeinout, "circular easeinout"},
    {egt::easing_exponential_easein, "exponential easein"},
    {egt::easing_exponential_easeout, "exponential easeout"},
    {egt::easing_exponential_easeinout, "exponential easeinout"}
};

class MainWindow : public egt::TopWindow
{
public:
    MainWindow()
        : m_animation(height() - 100, 0, std::chrono::seconds(2)),
          m_line(std::make_shared<egt::LineChart>(egt::Rect(0, 0, width(), height())))
    {
        create_board();

        background(egt::Image("file:background.png"));

        egt::ListBox::ItemArray items;
        items.resize(easing_functions.size());
        transform(easing_functions.begin(), easing_functions.end(), items.begin(),
        [](const std::pair<egt::EasingFunc, std::string>& v) { return std::make_shared<egt::StringItem>(v.second);});
        auto list1 = std::make_shared<egt::ListBox>(items, egt::Rect(egt::Point(0, 0), egt::Size(190, 0)));
        list1->align(egt::AlignFlag::expand_vertical | egt::AlignFlag::right);
        add(list1);

        list1->on_selected_changed([this, list1]()
        {
            m_seq.reset();
            m_animation.easing_func(easing_functions[list1->selected()].first);
            m_seq.start();

            egt::LineChart::DataArray data;
            for (float i = 0.; i <= 1.; i += .001)
            {
                data.push_back(std::make_pair(i,
                                              egt::detail::interpolate(easing_functions[list1->selected()].first, i, 0.f, 100.f)));
            }

            m_line->data(data);
            m_line->label("", "", easing_functions[list1->selected()].second);
            m_line->show_ticks(true);
        });

        list1->selected(7);

        auto image = std::make_shared<egt::ImageLabel>(egt::Image("file:ball.png"));
        m_box.resize(image->size());
        m_box.add(image);

        m_box.fill_flags().clear();
        m_box.move(egt::Point(width() / 2 - m_box.width() / 2, -110));
        m_box.flags().set(egt::Widget::Flag::no_layout);
        m_box.show();
        add(m_box);

        m_animation.on_change([this](egt::PropertyAnimator::Value value) { m_box.y(value); });
        m_animation.rounding(true);
        m_seq.add(m_animation);
        m_seq.add(m_delay);
        m_seq.start();

        m_board.zorder_top();
    }

private:

    void create_board()
    {
        auto imgicon = std::make_shared<egt::ImageLabel>(egt::Image("icon:cursor_hand.png"));
        imgicon->margin(5);
        imgicon->resize(egt::Size(egt::SideBoard::HANDLE_WIDTH, egt::SideBoard::HANDLE_WIDTH));
        imgicon->align(egt::AlignFlag::top | egt::AlignFlag::right);
        m_board.add(imgicon);

        add(m_board);
        m_board.show();

        m_board.add(left(expand_vertical(m_line)));
    }

    egt::Window m_box;
    egt::AnimationSequence m_seq{true};
    egt::PropertyAnimator m_animation;
    egt::AnimationDelay m_delay{std::chrono::seconds(1)};
    egt::SideBoard m_board;
    std::shared_ptr<egt::LineChart> m_line;
};

int main(int argc, const char** argv)
{
    egt::Application app(argc, argv, "easing");
    egt::add_search_path(EXAMPLEDATA);

    MainWindow window;

    egt::Label label1("CPU: ----");
    label1.color(egt::Palette::ColorId::label_text, egt::Palette::white);
    label1.color(egt::Palette::ColorId::label_bg, egt::Palette::transparent);
    label1.x(egt::SideBoard::HANDLE_WIDTH);
    window.add(bottom(label1));
    label1.zorder_down();

    egt::ImageLabel logo(egt::Image("icon:egt_logo_white.png;128"));
    logo.x(egt::SideBoard::HANDLE_WIDTH);
    logo.margin(10);
    window.add(top(logo));
    logo.zorder_down();

    egt::experimental::CPUMonitorUsage tools;
    egt::PeriodicTimer cputimer(std::chrono::seconds(1));
    cputimer.on_timeout([&label1, &tools]()
    {
        tools.update();
        std::ostringstream ss;
        ss << "CPU: " << static_cast<int>(tools.usage()) << "%";
        label1.text(ss.str());
    });
    cputimer.start();

    window.show();

    return app.run();
}
