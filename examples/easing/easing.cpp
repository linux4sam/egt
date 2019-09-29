/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <map>
#include <string>
#include <vector>

using namespace std;
using namespace egt;
using namespace egt::experimental;

static vector<pair<easing_func_t, string>> easing_functions =
{
    {easing_linear, "linear"},
    {easing_easy, "easy"},
    {easing_easy_slow, "easy slow"},
    {easing_extend, "extend"},
    {easing_drop, "drop"},
    {easing_drop_slow, "drop slow"},
    {easing_snap, "snap"},
    {easing_bounce, "bounce"},
    {easing_bouncy, "bouncy"},
    {easing_rubber, "rubber"},
    {easing_spring, "spring"},
    {easing_boing, "boing"},
    {easing_quadratic_easein, "quadratic easein"},
    {easing_quadratic_easeout, "quadratic easeout"},
    {easing_quadratic_easeinout, "quadratic easeinout"},
    {easing_cubic_easein, "cubic easein"},
    {easing_cubic_easeout, "cubic easeout"},
    {easing_cubic_easeinout, "cubic easeinout"},
    {easing_quartic_easein, "quartic easein"},
    {easing_quartic_easeout, "quartic easeout"},
    {easing_quartic_easeinout, "quartic easeinout"},
    {easing_quintic_easein, "quintic easein"},
    {easing_quintic_easeout, "quintic easeout"},
    {easing_quintic_easeinout, "quintic easeinout"},
    {easing_sine_easein, "sine easein"},
    {easing_sine_easeout, "sine easeout"},
    {easing_sine_easeinout, "sine easeinout"},
    {easing_circular_easein, "circular easein"},
    {easing_circular_easeout, "circular easeout"},
    {easing_circular_easeinout, "circular easeinout"},
    {easing_exponential_easein, "exponential easein"},
    {easing_exponential_easeout, "exponential easeout"},
    {easing_exponential_easeinout, "exponential easeinout"}
};

static LineChart::data_array create_data(easing_func_t easing)
{
    LineChart::data_array data;
    for (float i = 0.; i <= 1.; i += .001)
        data.emplace_back(i, detail::interpolate(easing, i, 0.f, 100.f));
    return data;
}

class MainWindow : public TopWindow
{
public:
    MainWindow()
        : m_animation(height() - 100, 0, std::chrono::seconds(2))
    {
        create_board();

        set_background(Image("background.png"));

        ListBox::item_array items;
        items.resize(easing_functions.size());
        transform(easing_functions.begin(), easing_functions.end(), items.begin(),
        [](const std::pair<easing_func_t, string>& v) { return make_shared<StringItem>(v.second);});
        auto list1 = make_shared<ListBox>(items, Rect(Point(0, 0), Size(190, 0)));
        list1->set_align(alignmask::expand_vertical | alignmask::right);
        add(list1);

        list1->on_event([this, list1](Event&)
        {
            m_seq.reset();
            m_animation.set_easing_func(easing_functions[list1->selected()].first);
            m_seq.start();

            m_line.clear();
            m_line.add_data(create_data(easing_functions[list1->selected()].first), LineChart::chart_type::lines);
        }, {eventid::property_changed});

        list1->set_selected(7);

        auto image = std::make_shared<ImageLabel>(Image("ball.png"));
        m_box.resize(image->size());
        m_box.add(image);

        m_box.set_boxtype(Theme::boxtype::none);
        m_box.move(Point(width() / 2 - m_box.width() / 2, -110));
        m_box.flags().set(Widget::flag::no_layout);
        m_box.show();
        add(m_box);

        m_animation.on_change(std::bind(&Window::set_y, &m_box, std::placeholders::_1));
        m_animation.set_rounding(true);
        m_seq.add(m_animation);
        m_seq.add(m_delay);
        m_seq.start();

        m_board.zorder_top();
    }

private:

    void create_board()
    {
        auto imgicon = make_shared<ImageLabel>(Image("@cursor_hand.png"));
        imgicon->set_margin(5);
        imgicon->resize(Size(SideBoard::HANDLE_WIDTH, SideBoard::HANDLE_WIDTH));
        imgicon->set_align(alignmask::top | alignmask::right);
        m_board.add(imgicon);

        add(m_board);
        m_board.show();

        m_line.set_width(m_board.width() - SideBoard::HANDLE_WIDTH);
        m_line.set_color(Palette::ColorId::bg, Palette::black);
        m_line.set_color(Palette::ColorId::border, Palette::white);
        m_board.add(left(expand_vertical(m_line)));
    }

    Window m_box;
    AnimationSequence m_seq{true};
    PropertyAnimator m_animation;
    AnimationDelay m_delay{std::chrono::seconds(1)};
    SideBoard m_board;
    LineChart m_line;
};

int main(int argc, const char** argv)
{
    Application app(argc, argv, "easing");

    MainWindow window;

    Label label1("CPU: ----");
    label1.set_color(Palette::ColorId::label_text, Palette::white);
    label1.set_color(Palette::ColorId::label_bg, Palette::transparent);
    label1.set_x(SideBoard::HANDLE_WIDTH);
    window.add(bottom(label1));

    ImageLabel logo(Image("@128px/egt_logo_white.png"));
    logo.set_x(SideBoard::HANDLE_WIDTH);
    logo.set_margin(10);
    window.add(top(logo));

    experimental::CPUMonitorUsage tools;
    PeriodicTimer cputimer(std::chrono::seconds(1));
    cputimer.on_timeout([&label1, &tools]()
    {
        tools.update();
        ostringstream ss;
        ss << "CPU: " << static_cast<int>(tools.usage(0)) << "%";
        label1.set_text(ss.str());
    });
    cputimer.start();

    window.show();

    return app.run();
}
