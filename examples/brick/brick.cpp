/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/detail/screen/kmsscreen.h>
#include <egt/ui>
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
        : m_grid1(*this, Rect(Point(0, 30), Size(w(), 80)),
                  Tuple(w() / 100, ROWS), 5),
          m_grid2(*this, Rect(Point(0, 30 + 80 + 30), Size(w(), 80)),
                  Tuple(w() / 100, ROWS), 5),
          m_ball(Image("small_ball.png")),
          m_paddle(Image("paddle.png")),
          m_running(false),
          e1(r())
    {
        auto background = make_shared<ImageLabel>(Image("brick_background.png"));
        add(background);
        if (background->w() != w() || background->h() != h())
        {
            double hscale = (double)w() / (double)background->w();
            double vscale = (double)h() / (double)background->h();
            background->scale_image(hscale, vscale);
        }

        m_grid1.palette().set(Palette::ColorId::border, Palette::GroupId::normal, Palette::transparent);
        m_grid2.palette().set(Palette::ColorId::border, Palette::GroupId::normal, Palette::transparent);

        for (int c = 0; c < detail::KMSScreen::instance()->size().w / 100; c++)
        {
            for (int r = 0; r < ROWS; r++)
            {
                ostringstream ss;
                ss << "brick" << r << ".png";
                auto block = make_shared<ImageLabel>(Image(ss.str()));
                m_blocks.push_back(block);
                m_grid1.add(expand(block), c, r);
            }
        }

        for (int c = 0; c < detail::KMSScreen::instance()->size().w / 100; c++)
        {
            for (int r = 0; r < ROWS; r++)
            {
                ostringstream ss;
                ss << "brick" << r + 2 << ".png";
                auto block = make_shared<ImageLabel>(Image(ss.str()));
                m_blocks.push_back(block);
                m_grid2.add(expand(block), c, r);
            }
        }

        add(m_paddle);
        add(m_ball);
        double hscale = (double)w() / (double)background->w() * 0.5;
        double vscale = (double)h() / (double)background->h() * 0.5;
        m_ball.scale_image(hscale, vscale);

        m_label = Label("-",
                        Rect(Point(5, 2),
                             Size(100, 40)),
                        alignmask::left | alignmask::center);
        m_label.palette().set(Palette::ColorId::text, Palette::GroupId::normal, Palette::white)
        .set(Palette::ColorId::bg, Palette::GroupId::normal, Palette::transparent);
        add(m_label);

        reset_game();
    }

    // TODO: convert this to use POINTER_DRAG events
    int handle(eventid event) override
    {
        switch (event)
        {
        case eventid::keyboard_repeat:
        case eventid::keyboard_down:
        {
            if (event::keys().key == KEY_LEFT || event::keys().key == KEY_RIGHT)
            {
                int x;
                m_running = true;
                if (event::keys().key == KEY_LEFT)
                    x = m_paddle.x() - (event == eventid::keyboard_repeat ? 15 : 10);
                else
                    x = m_paddle.x() + (event == eventid::keyboard_repeat ? 15 : 10);

                if (x > -m_paddle.w() && x < w())
                    m_paddle.move(Point(x, m_paddle.y()));

                return 1;
            }
            else if (event::keys().key == KEY_UP)
            {
                m_xspeed *= 1.5;
                m_yspeed *= 1.5;
                return 1;
            }

            else if (event::keys().key == KEY_DOWN)
            {
                m_xspeed *= .5;
                m_yspeed *= .5;
                return 1;
            }

            break;
        }
        case eventid::raw_pointer_down:
            m_running = true;
            break;
        case eventid::raw_pointer_move:
            m_paddle.move(Point(event::pointer().point.x - m_paddle.w() / 2, m_paddle.y()));
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
        m_label.set_text(ss.str());

        m_running = false;
    }

    void add_points(int points)
    {
        m_points += points;

        ostringstream ss;
        ss << "Points: " << m_points;
        m_label.set_text(ss.str());
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

            if (Rect::intersect(m_ball.box(), block->to_parent(block->box())))
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
    ImageLabel m_ball;
    ImageLabel m_paddle;
    float m_xspeed;
    float m_yspeed;
    vector<shared_ptr<ImageLabel>> m_blocks;
    Label m_label;
    unsigned int m_points;
    bool m_running;
    std::random_device r;
    std::default_random_engine e1;
};

int main(int argc, const char** argv)
{
    Application app(argc, argv, "brick");

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
