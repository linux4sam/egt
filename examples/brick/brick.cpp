/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "egt/ui"
#include <iostream>
#include <random>
#include <sstream>
#include <vector>

using namespace std;
using namespace egt;

class GameWindow : public Window
{
public:
    static constexpr int ROWS = 2;

    GameWindow()
        : m_grid1(Rect(Point(0, 30), Size(KMSScreen::instance()->size().w, 80)),
                  KMSScreen::instance()->size().w / 100, ROWS, 5),
          m_grid2(Rect(Point(0, 30 + 80 + 30), Size(KMSScreen::instance()->size().w, 80)),
                  KMSScreen::instance()->size().w / 100, ROWS, 5),
          m_ball("small_ball.png"),
          m_paddle("paddle.png"),
          m_running(false),
          e1(r())
    {
        Image* background = new Image("brick_background.png");
        add(background);
        if (background->w() != w() || background->h() != h())
        {
            double hscale = (double)w() / (double)background->w();
            double vscale = (double)h() / (double)background->h();
            background->scale(hscale, vscale);
        }

        add(&m_grid1);
        add(&m_grid2);

        for (int c = 0; c < KMSScreen::instance()->size().w / 100; c++)
        {
            for (int r = 0; r < ROWS; r++)
            {
                ostringstream ss;
                ss << "brick" << r << ".png";
                Image* block = new Image(ss.str());
                m_blocks.push_back(block);
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
                m_grid2.add(block, c, r);
            }
        }

        add(&m_paddle);
        add(&m_ball);
        double hscale = (double)w() / (double)background->w() * 0.5;
        double vscale = (double)h() / (double)background->h() * 0.5;
        m_ball.scale(hscale, vscale);

        m_label = new Label("-",
                            Rect(Point(5, 2),
                                 Size(100, 40)),
                            alignmask::LEFT | alignmask::CENTER);
        m_label->palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::WHITE)
        .set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);
        add(m_label);

        m_grid1.reposition();
        m_grid2.reposition();

        reset_game();
    }

    int handle(eventid event) override
    {
        switch (event)
        {
        case eventid::KEYBOARD_REPEAT:
        case eventid::KEYBOARD_DOWN:
        {
            if (event_key() == KEY_LEFT || event_key() == KEY_RIGHT)
            {
                int x;
                m_running = true;
                if (event_key() == KEY_LEFT)
                    x = m_paddle.x() - (event == eventid::KEYBOARD_REPEAT ? 15 : 10);
                else
                    x = m_paddle.x() + (event == eventid::KEYBOARD_REPEAT ? 15 : 10);

                if (x > -m_paddle.w() && x < w())
                    m_paddle.move(Point(x, m_paddle.y()));

                return 1;
            }
            else if (event_key() == KEY_UP)
            {
                m_xspeed *= 1.5;
                m_yspeed *= 1.5;
                return 1;
            }

            else if (event_key() == KEY_DOWN)
            {
                m_xspeed *= .5;
                m_yspeed *= .5;
                return 1;
            }

            break;
        }
        case eventid::MOUSE_DOWN:
            m_running = true;
        case eventid::MOUSE_MOVE:
            m_paddle.move(Point(event_mouse().x - m_paddle.w() / 2, m_paddle.y()));
            return 1;
        default:
            break;
        }

        return Window::handle(event);
    }

    void reset_game()
    {
        for (auto block : m_blocks)
            block->show();

        m_ball.move(Point(w() / 2 - m_ball.w() / 2, h() - m_paddle.h() - 25 - m_ball.h() - 100));
        m_paddle.move(Point(w() / 2 - m_paddle.w() / 2, h() - m_paddle.h() - 25));

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

            if (Rect::intersect(m_ball.box(), block->box()))
            {
                block->hide();
                m_yspeed *= -1.0;
                add_points(1);
                break;
            }
        }

        // hit paddle
        if (Rect::intersect(m_ball.box(), m_paddle.box()))
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
};

int main()
{
    Application app;

    set_image_path("../share/egt/examples/brick/");

    GameWindow win;

    PeriodicTimer animatetimer(std::chrono::milliseconds(30));
    animatetimer.on_timeout([&win]()
    {
        win.animate();
    });
    animatetimer.start();

    win.show();

    return app.run();
}
