/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <chrono>
#include <cmath>
#include <cstring>
#include <iostream>
#include <map>
#include <egt/ui>
#include <random>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace egt;

class Bubble : public ImageLabel
{
public:
    Bubble(int xspeed, int yspeed, const Point& point) noexcept
        : ImageLabel(Image("smallbubble.png"), point),
          m_xspeed(xspeed),
          m_yspeed(yspeed)
    {}

    Bubble(const Bubble&) = default;
    Bubble(Bubble&&) = default;
    Bubble& operator=(const Bubble&) = default;
    Bubble& operator=(Bubble&&) = default;
    virtual ~Bubble() = default;

    bool animate()
    {
        bool visible = Rect::intersect(Rect(Point(0, 0), main_screen()->size()), box());

        if (visible)
        {
            Point to(box().point());
            to += Point(m_xspeed, m_yspeed);
            move(to);
        }

        return visible;
    }

private:

    Bubble() = delete;

    int m_xspeed;
    int m_yspeed;
};

class MainWindow : public TopWindow
{
public:
    MainWindow()
        : TopWindow(Size()),
          e1(r())
    {
        set_boxtype(Theme::boxtype::none);

        m_background = make_unique<ImageLabel>(Image("water.png"));
        add(m_background);
        if (m_background->h() != h())
        {
            double scale = (double)h() / (double)m_background->h();
            m_background->scale_image(scale);
        }

        m_label = make_unique<Label>("Objects: 0",
                                     Rect(Point(10, 10),
                                          Size(150, 40)),
                                     alignmask::LEFT | alignmask::CENTER);
        m_label->palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::WHITE)
        .set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);
        add(m_label);

        m_sprite = make_unique<Sprite>(Image("diver.png"), Size(390, 312), 16, Point(0, 0));
        add(m_sprite);
        m_sprite->show();
    }

    int handle(eventid event) override
    {
        switch (event)
        {
        case eventid::RAW_POINTER_MOVE:
            spawn(event_mouse());
            break;
        default:
            break;
        }

        return TopWindow::handle(event);
    }

    void spawn(const Point& p)
    {
        int xspeed = 0;
        int yspeed = speed_dist(e1);
        int offset = offset_dist(e1);
        float size = size_dist(e1);

        // has to move at some speed
        if (yspeed == 0)
            yspeed = 1;

        m_images.emplace_back(xspeed, yspeed, p);
        auto& image = m_images.back();
        add(image);
        image.scale_image(size, true);
        image.move(Point(p.x - image.box().w / 2 + offset,
                         p.y - image.box().h / 2 + offset));
        objects_changed();
    }

    void animate()
    {
        for (auto x = m_images.begin(); x != m_images.end();)
        {
            auto& image = *x;
            if (!image.animate())
            {
                x = m_images.erase(x);
                objects_changed();
            }
            else
            {
                x++;
            }
        }
    }

    void objects_changed()
    {
        ostringstream ss;
        ss << "Objects: " << m_images.size();
        m_label->set_text(ss.str());
    }

    vector<Bubble> m_images;
    unique_ptr<ImageLabel> m_background;
    unique_ptr<Label> m_label;
    unique_ptr<Sprite> m_sprite;

    std::random_device r;
    std::default_random_engine e1;
    std::uniform_int_distribution<int> speed_dist{-20, -1};
    std::uniform_int_distribution<int> offset_dist{-20, 20};
    std::uniform_real_distribution<float> size_dist{0.1, 1.0};
};

int main(int argc, const char** argv)
{
    Application app(argc, argv, "water");

    MainWindow win;

    vector<Sprite*> sprites;

#define SPRITE1
#ifdef SPRITE1
    Sprite sprite1(Image("fish.png"), Size(252, 209), 8, Point(0, 0));
    win.add(&sprite1);
    sprite1.show();
    sprites.push_back(&sprite1);
#endif

#define SPRITE2
#ifdef SPRITE2
    Sprite sprite2(Image("fish2.png"), Size(100, 87), 6, Point(0, 0));
    win.add(&sprite2);
    sprite2.show();
    sprites.push_back(&sprite2);
#endif

    sprites.push_back(win.m_sprite.get());

    PeriodicTimer animatetimer(std::chrono::milliseconds(30));
    animatetimer.on_timeout([&win]()
    {
        win.animate();
    });
    animatetimer.start();

    PeriodicTimer animatetimer2(std::chrono::milliseconds(100));
    animatetimer2.on_timeout([&sprites]()
    {
        for (auto& sprite : sprites)
            sprite->advance();
    });
    animatetimer2.start();

    PeriodicTimer spawntimer(std::chrono::seconds(1));
    spawntimer.on_timeout([&win]()
    {
        if (win.m_images.size() > 30)
            return;

        static std::uniform_int_distribution<int> xoffset_dist(-win.w() / 2, win.w() / 2);
        int offset = xoffset_dist(win.e1);

        static std::uniform_int_distribution<int> count_dist(1, 10);
        int count = count_dist(win.e1);

        Point p(win.box().center());
        p.y = win.box().h;
        p.x += offset;

        while (count--)
            win.spawn(p);
    });
    spawntimer.start();

#ifdef SPRITE1
    PropertyAnimator a1(-sprite1.size().w, main_screen()->size().w,
                        std::chrono::milliseconds(10000),
                        easing_linear);
    a1.on_change(std::bind(&Sprite::set_x, std::ref(sprite1), std::placeholders::_1));
    a1.start();

    PeriodicTimer floattimer(std::chrono::milliseconds(1000 * 12));
    floattimer.on_timeout([&a1, &sprite1, &win]()
    {

        static std::uniform_int_distribution<int> yoffset_dist(0, win.h() - sprite1.size().h);
        int y = yoffset_dist(win.e1);

        sprite1.move(Point(-sprite1.size().w, y));
        a1.start();
    });
    floattimer.start();
#endif

#ifdef SPRITE2
    PropertyAnimator a2(-sprite2.size().w, main_screen()->size().w,
                        std::chrono::milliseconds(12000),
                        easing_linear);
    a2.on_change(std::bind(&Sprite::set_x, std::ref(sprite2), std::placeholders::_1));
    a2.start();

    PeriodicTimer floattimer2(std::chrono::milliseconds(1000 * 15));
    floattimer2.on_timeout([&a2, &sprite2, &win]()
    {
        static std::uniform_int_distribution<int> yoffset_dist(0, win.h() - sprite2.size().h);
        int y = yoffset_dist(win.e1);

        sprite2.move(Point(-sprite2.size().w, y));
        a2.start();
    });
    floattimer2.start();
#endif

    Label label1("CPU: 0%",
                 Rect(Point(10, win.size().h - 40),
                      Size(100, 40)));
    label1.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::WHITE)
    .set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);
    win.add(&label1);

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

    win.show();

    return app.run();
}
