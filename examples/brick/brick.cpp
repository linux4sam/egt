/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "mui/ui.h"
#include <iostream>
#include <random>
#include <sstream>
#include <vector>

using namespace std;
using namespace mui;

class MyWindow : public SimpleWindow
{
public:
    static constexpr int ROWS = 2;

    MyWindow()
        : m_grid1(0, 30, KMSScreen::instance()->size().w, 80,
                  KMSScreen::instance()->size().w / 100, ROWS, 5),
          m_grid2(0, 30 + 80 + 30, KMSScreen::instance()->size().w, 80,
                  KMSScreen::instance()->size().w / 100, ROWS, 5),
          m_ball("small_ball.png"),
          m_paddle("paddle.png"),
          m_running(false),
          e1(r())
#ifdef INCLUDE_ANIMATION
        , m_sprite1("firework.png", 100, 95, 46, 0, 0, 0, 0),
          m_animatetimer(33)
#endif
    {
        Image* background = new Image("brick_background.png");
        add(background);
        if (background->w() != w() || background->h() != h())
        {
            double hscale = (double)w() / (double)background->w();
            double vscale = (double)h() / (double)background->h();
            background->scale(hscale, vscale);
        }

        for (int c = 0; c < KMSScreen::instance()->size().w / 100; c++)
        {
            for (int r = 0; r < ROWS; r++)
            {
                ostringstream ss;
                ss << "brick" << r << ".png";
                Image* block = new Image(ss.str());
                m_blocks.push_back(block);
                add(block);
                m_grid1.add(block, c, r);
            }
        }

        for (int c = 0; c < KMSScreen::instance()->size().w / 100; c++)
        {
            for (int r = 0; r < ROWS; r++)
            {
                ostringstream ss;
                ss << "brick" << r + 2 << ".png";
                Image* block = new Image(ss.str());
                m_blocks.push_back(block);
                add(block);
                m_grid2.add(block, c, r);
            }
        }

        add(&m_paddle);
        add(&m_ball);
        double hscale = (double)w() / (double)background->w() * 0.5;
        double vscale = (double)h() / (double)background->h() * 0.5;
        m_ball.scale(hscale, vscale);

        m_label = new Label("-",
                            Point(5, 2),
                            Size(100, 40),
                            Widget::ALIGN_LEFT | Widget::ALIGN_CENTER);
        m_label->fgcolor(Color::WHITE);
        add(m_label);

        m_grid1.reposition();
        m_grid2.reposition();

        reset_game();

#ifdef INCLUDE_ANIMATION
        m_animatetimer.add_handler([this]()
        {
            if (m_sprite1.is_last_frame())
            {
                m_animatetimer.cancel();
                m_sprite1.hide();
                return;
            }

            m_sprite1.advance();
        });
#endif
    }

    int handle(int event)
    {
        switch (event)
        {
        case EVT_KEY_REPEAT:
        case EVT_KEY_DOWN:
        {
            if (key_position() == KEY_LEFT || key_position() == KEY_RIGHT)
            {
                int x;
                m_running = true;
                if (key_position() == KEY_LEFT)
                    x = m_paddle.x() - (event == EVT_KEY_REPEAT ? 15 : 10);
                else
                    x = m_paddle.x() + (event == EVT_KEY_REPEAT ? 15 : 10);

                if (x > -m_paddle.w() && x < w())
                    m_paddle.move(x, m_paddle.y());

                return 1;
            }
            else if (key_position() == KEY_UP)
            {
                m_xspeed *= 1.5;
                m_yspeed *= 1.5;
                return 1;
            }

            else if (key_position() == KEY_DOWN)
            {
                m_xspeed *= .5;
                m_yspeed *= .5;
                return 1;
            }

            break;
        }
        case EVT_MOUSE_DOWN:
            m_running = true;
        case EVT_MOUSE_MOVE:
            m_paddle.move(mouse_position().x - m_paddle.w() / 2, m_paddle.y());
            return 1;
        }

        return SimpleWindow::handle(event);
    }

    void reset_game()
    {
        for (auto block : m_blocks)
            block->show();

        m_ball.move(w() / 2 - m_ball.w() / 2, h() - m_paddle.h() - 25 - m_ball.h() - 100);
        m_paddle.move(w() / 2 - m_paddle.w() / 2, h() - m_paddle.h() - 25);

        std::uniform_real_distribution<float> speed_dist(2.0, 5.0);
        m_xspeed = speed_dist(e1);
        m_yspeed = speed_dist(e1);
        m_points = 0;

        ostringstream ss;
        ss << "Points: " << m_points;
        m_label->text(ss.str());

        m_running = false;
    }

    void add_points(int points)
    {
        m_points += points;

        ostringstream ss;
        ss << "Points: " << m_points;
        m_label->text(ss.str());
    }

    void animate()
    {
        if (!m_running)
            return;

        // advance ball
        Point to(m_ball.box().point());
        to += Point(m_xspeed, m_yspeed);
        m_ball.move(to);

        // hit block
        for (auto block : m_blocks)
        {
            if (!block->visible())
                continue;

            if (Rect::is_intersect(m_ball.box(), block->box()))
            {
                block->hide();
                m_yspeed *= -1.0;
                add_points(1);

#ifdef INCLUDE_ANIMATION
                m_sprite1.move(block->box().center().x - m_sprite1.w() / 2,
                               block->box().center().y - m_sprite1.h() / 2);
                m_sprite1.show();
                m_animatetimer.start();
#endif

                break;
            }
        }

        // hit paddle
        if (Rect::is_intersect(m_ball.box(), m_paddle.box()))
        {
            m_yspeed *= -1.0;
        }

        // bounce off walls
        if (m_ball.x() + m_ball.w() > w() || m_ball.x() < 0)
        {
            m_xspeed *= -1.0;
        }

        // below paddle
        if (m_ball.y() + m_ball.h() > h())
        {
            reset_game();
        }

        // win
        if (m_ball.y() < 0)
        {
            reset_game();
        }
    }

private:

    StaticGrid m_grid1;
    StaticGrid m_grid2;
    Image m_ball;
    Image m_paddle;
    float m_xspeed;
    float m_yspeed;
    vector<Image*> m_blocks;
    Label* m_label;
    unsigned int m_points;
    bool m_running;
    std::random_device r;
    std::default_random_engine e1;
#ifdef INCLUDE_ANIMATION
    HardwareSprite m_sprite1;
    PeriodicTimer m_animatetimer;
#endif
};

int main()
{
#ifdef HAVE_TSLIB
#ifdef HAVE_LIBPLANES
    KMSScreen screen;
    InputTslib input0("/dev/input/touchscreen0");
    InputEvDev input1("/dev/input/event2");
#else
    FrameBuffer fb("/dev/fb0");
#endif
#else
    X11Screen screen(Size(800, 480));
#endif

    MyWindow win;
    win.show();

    PeriodicTimer animatetimer(33);
    animatetimer.add_handler([&win]()
    {
        win.animate();
    });
    animatetimer.start();

    return EventLoop::run();
}
