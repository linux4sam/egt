/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include <chrono>
#include <cmath>
#include <cstring>
#include <iostream>
#include <map>
#include <mui/ui>
#include <random>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace mui;

class Bubble : public Image
{
public:
    Bubble(int xspeed, int yspeed, const Point& point)
        : Image("smallbubble.png", point),
          m_xspeed(xspeed),
          m_yspeed(yspeed)
    {}

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
    int m_xspeed;
    int m_yspeed;
    float m_angle;
};

class MainWindow : public Window
{
public:
    MainWindow()
        : Window(Size(), widgetmask::WINDOW_DEFAULT | widgetmask::NO_BACKGROUND),
          /*PlaneWindow(Size(KMSScreen::instance()->size().w,
                                     KMSScreen::instance()->size().h),
                                FLAG_WINDOW_DEFAULT | FLAG_NO_BACKGROUND, DRM_FORMAT_XRGB8888),*/
          e1(r())
    {
        Image* img = new Image("water_1080.png");
        add(img);
        if (img->h() != h())
        {
            double scale = (double)h() / (double)img->h();
            img->scale(scale, scale);
        }

        m_label = new Label("Objects: 0",
                            Rect(Point(10, 10),
                                 Size(150, 40)),
                            alignmask::LEFT | alignmask::CENTER);
        m_label->palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::WHITE)
        .set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);
        add(m_label);
    }

    int handle(int event)
    {
        switch (event)
        {
        case EVT_MOUSE_MOVE:
            spawn(mouse_position());
            break;
        }

        return Window::handle(event);
    }

    void spawn(const Point& p)
    {
        static std::uniform_int_distribution<int> speed_dist(-20, -1);
        int xspeed = 0;
        int yspeed = speed_dist(e1);

        static std::uniform_int_distribution<int> offset_dist(-20, 20);
        int offset = offset_dist(e1);

        static std::uniform_real_distribution<float> size_dist(0.1, 1.0);
        float size = size_dist(e1);

        // has to move at some speed
        if (yspeed == 0)
            yspeed = 1;

        Bubble* image = new Bubble(xspeed, yspeed, p);
        add(image);
        image->scale(size, size, true);
        image->move(Point(p.x - image->box().w / 2 + offset,
                          p.y - image->box().h / 2 + offset));
        m_images.push_back(image);
        objects_changed();
    }

    void animate()
    {
        for (auto x = m_images.begin(); x != m_images.end();)
        {
            if (!(*x)->animate())
            {
                remove(*x);
                delete *x;
                x = m_images.erase(x);
                objects_changed();
            }
            else
            {
                x++;
            }
        }
    }

    //private:

    void objects_changed()
    {
        ostringstream ss;
        ss << "Objects: " << m_images.size();
        m_label->text(ss.str());
    }

    vector<Bubble*> m_images;
    std::random_device r;
    std::default_random_engine e1;
    Label* m_label;
};


int main()
{
    Application app/*(false)*/;

    set_image_path("../share/mui/examples/water/");

    MainWindow win;
    win.show();

    vector<ISpriteBase*> sprites;

#ifdef SPRITE1
    HardwareSprite sprite1("fish.png", 252, 209, 8, 0, 0, 0, 0);
    win.add(&sprite1);
    sprites.push_back(&sprite1);
#endif

#ifdef SPRITE2
    HardwareSprite sprite2("fish2.png", 100, 87, 6, 0, 0, 0, 0);
    win.add(&sprite2);
    sprites.push_back(&sprite2);
#endif

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
    WidgetPositionAnimator a1 = WidgetPositionAnimator({&sprite1},
                                WidgetPositionAnimator::CORD_X,
                                -sprite1.size().w,
                                main_screen()->size().w,
                                10000,
                                easing_linear);
    a1.start();

    PeriodicTimer floattimer(1000 * 12);
    floattimer.on_timeout([&a1, &sprite1, &win]()
    {

        static std::uniform_int_distribution<int> yoffset_dist(0, win.h() - sprite1.size().h);
        int y = yoffset_dist(win.e1);

        sprite1.move(-sprite1.size().w, y);
        a1.start();
    });
    floattimer.start();
#endif

#ifdef SPRITE2
    WidgetPositionAnimator a2 = WidgetPositionAnimator({&sprite2},
                                WidgetPositionAnimator::CORD_X,
                                -sprite2.size().w, main_screen()->size().w,
                                12000,
                                easing_linear);
    a2.start();

    PeriodicTimer floattimer2(1000 * 15);
    floattimer2.on_timeout([&a2, &sprite2, &win]()
    {
        static std::uniform_int_distribution<int> yoffset_dist(0, win.h() - sprite2.size().h);
        int y = yoffset_dist(win.e1);

        sprite2.move(-sprite2.size().w, y);
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
        label1.text(ss.str());
    });
    cputimer.start();

    return app.run();
}
