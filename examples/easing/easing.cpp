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

static vector<pair<easing_func, string>> easing_functions =
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
    {easing_quadratic_easeinout, "quadratic"},
    {easing_cubic_easeinout, "cubic"},
    {easing_quartic_easeinout, "quartic"},
    {easing_quintic_easeinout, "quintic"},
    {easing_sine_easeinout, "sine"},
    {easing_circular_easeinout, "circular"},
    {easing_exponential_easeinout, "exponential"}
};

class MainWindow : public TopWindow
{
public:
    MainWindow()
        : m_seq(true),
          m_animation(-110, h() - 100, std::chrono::seconds(2)),
          m_delay(std::chrono::seconds(1))
    {
        auto i = Image("background.png");
        auto img = make_shared<ImageLabel>(i);
        img->set_align(alignmask::expand);
        img->set_image_align(alignmask::expand);
        add(img);

        ListBox::item_array items;
        items.resize(easing_functions.size());
        transform(easing_functions.begin(), easing_functions.end(), items.begin(),
        [](const std::pair<easing_func, string>& v) { return make_shared<StringItem>(v.second);});
        auto list1 = make_shared<ListBox>(items, Rect(Point(0, 0), Size(150, 0)));
        list1->set_align(alignmask::expand_vertical | alignmask::right);
        add(list1);
        list1->set_select(7);

        list1->on_event([this, list1](eventid)
        {
            m_seq.reset();
            m_animation.set_easing_func(easing_functions[list1->selected()].first);
            m_seq.start();
            return 0;
        }, {eventid::property_changed});

        auto image = std::make_shared<ImageLabel>(Image("ball.png"));
        image->set_boxtype(Theme::boxtype::none);

        m_box = make_shared<Window>(Size(100, 200));
        m_box->set_boxtype(Theme::boxtype::none);
        m_box->add(image);
        m_box->move(Point(w() / 2 - m_box->w() / 2, -110));
        m_box->show();
        add(m_box);

        m_animation.on_change(std::bind(&Window::set_y, m_box, std::placeholders::_1));
        m_seq.add(m_animation);
        m_seq.add(m_delay);
        m_seq.start();
    }

private:
    shared_ptr<Window> m_box;
    AnimationSequence m_seq;
    PropertyAnimator m_animation;
    AnimationDelay m_delay;
};

int main(int argc, const char** argv)
{
    Application app(argc, argv, "easing");

    MainWindow window;

    Label label1("CPU: ----");
    label1.instance_palette().set(Palette::ColorId::label_text, Palette::white)
    .set(Palette::ColorId::label_bg, Palette::transparent);
    window.add(bottom(left(label1)));

    CPUMonitorUsage tools;
    PeriodicTimer cputimer(std::chrono::seconds(1));
    cputimer.on_timeout([&label1, &tools]()
    {
        tools.update();
        ostringstream ss;
        ss << "CPU: " << (int)tools.usage(0) << "%";
        label1.set_text(ss.str());
    });
    cputimer.start();

    window.show();

    return app.run();
}
