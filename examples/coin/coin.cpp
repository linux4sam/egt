/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include <iostream>
#include <mui/ui>
#include <planes/plane.h>
#include <random>
#include <sstream>
#include <vector>

using namespace std;
using namespace mui;

static bool alpha_collision(const Rect& lhs, shared_cairo_surface_t limage,
                            const Rect& rhs, shared_cairo_surface_t rimage)
{
    if (Rect::intersect(lhs, rhs))
    {
        unsigned int* ldata = reinterpret_cast<unsigned int*>(cairo_image_surface_get_data(limage.get()));
        unsigned int* rdata = reinterpret_cast<unsigned int*>(cairo_image_surface_get_data(rimage.get()));

        Rect i = Rect::intersection(lhs, rhs);

        for (int y = i.top(); y < i.bottom(); y++)
        {
            for (int x = i.left(); x < i.right(); x++)
            {
                unsigned int l = ldata[(x - lhs.left()) + (y - lhs.top()) * lhs.w];
                unsigned int r = rdata[(x - rhs.left()) + (y - rhs.top()) * rhs.w];

                if ((l >> 24 & 0xff) && (r >> 24 & 0xff))
                {
                    return true;
                }
            }
        }
    }

    return false;
}


/**
 * Layer that contains the obstacles.
 *
 * It is the size of the screen and scrolls from right off-stage to left off-stage.
 */
class ObstacleLayer : public PlaneWindow
{
public:
    ObstacleLayer(int speed)
        : PlaneWindow(Size(800, 480), widgetmask::WINDOW_DEFAULT, DRM_FORMAT_ARGB8888/*RGB565*/),
          m_speed(speed),
          m_x(0),
          e1(r()),
          m_grid1(Rect(Point(0, 0), Size(200, 200)), 4, 4, 0),
          m_grid2(Rect(Point(0, 0), Size(132, 130)), 3, 10, 0)
    {
        add(&m_grid1);
        add(&m_grid2);
        reset();
    }

    bool advance()
    {
        bool bounce = false;

        m_x -= m_speed;

        if (m_x <= -w())
        {
            m_x = w();
            bounce = true;
        }

        move(Point(m_x, 0));

        return bounce;
    }

    void restart()
    {
        m_x = w();
        move(Point(m_x, 0));
    }

    bool collide(const Rect& rect, shared_cairo_surface_t image, uint32_t& points)
    {
        Rect orect = rect;
        orect.x -= m_x;

        // hit coin
        for (auto coin : m_coins)
        {
            if (!coin->visible())
                continue;

            if (alpha_collision(orect, image, coin->box(), coin->surface()))
            {
                coin->hide();
                points += 10;
            }
        }

        // hit brick
        for (auto brick : m_bricks)
        {
            if (!brick->visible())
                continue;

            if (alpha_collision(orect, image, brick->box(), brick->surface()))
            {
                return false;
            }
        }

        return true;
    }

    void reset()
    {
        for (auto coin : m_coins)
        {
            remove(coin);
            delete coin;
        }

        for (auto brick : m_bricks)
        {
            remove(brick);
            delete brick;
        }

        m_coins.clear();
        m_bricks.clear();

        add_coins();
        add_bricks();
    }

    void add_coins()
    {
        for (int c = 0; c < 4; c++)
        {
            for (int r = 0; r < 4; r++)
            {
                m_grid1.add(0, c, r);
            }
        }

        std::uniform_int_distribution<int> cdist(1, 4);
        int columns = cdist(e1);
        int rows = cdist(e1);
        for (int c = 0; c < columns; c++)
        {
            for (int r = 0; r < rows; r++)
            {
                Image* block = new Image("coin_coin.png");
                m_coins.push_back(block);
                //add(block);
                m_grid1.add(block, c, r);
            }
        }

        std::uniform_int_distribution<int> xdist(0, 600);
        std::uniform_int_distribution<int> ydist(0, 200);
        m_grid1.move(Point(xdist(e1), ydist(e1)));
    }

    void add_bricks()
    {
        for (int c = 0; c < 3; c++)
        {
            for (int r = 0; r < 10; r++)
            {
                m_grid2.add(0, c, r);
            }
        }

        std::uniform_int_distribution<int> cdist(1, 3);
        std::uniform_int_distribution<int> rdist(1, 10);
        int columns = cdist(e1);
        int rows = rdist(e1);
        for (int c = 0; c < columns; c++)
        {
            for (int r = 0; r < rows; r++)
            {
                Image* block = new Image("coin_brick.png");
                m_bricks.push_back(block);
                //add(block);
                m_grid2.add(block, c, r);
            }
        }

        std::uniform_int_distribution<int> xdist(0, 600);

        const int ids[] = { 0, 100, 200, 300 };
        std::uniform_int_distribution<int> ydist(0, 3);

        bool conflict = false;
        do
        {
            conflict = false;

            m_grid2.move(Point(xdist(e1), ids[ydist(e1)]));

            for (auto coin : m_coins)
            {
                for (auto brick : m_bricks)
                {
                    if (Rect::intersect(coin->box(), brick->box()))
                    {
                        conflict = true;
                        break;
                    }
                }

                if (conflict)
                    break;
            }


            if (conflict)
                cout << "bad placement" << endl;
        } while (conflict);
    }

protected:

    int m_speed;
    int m_x;
    std::random_device r;
    std::default_random_engine e1;
    std::vector<Image*> m_coins;
    std::vector<Image*> m_bricks;
    StaticGrid m_grid1;
    StaticGrid m_grid2;
};

class SceneLayer : public PlaneWindow
{
public:

    SceneLayer(Image& image, const Point& pos, const Size& size, int speed)
        : PlaneWindow(image.size(), widgetmask::WINDOW_DEFAULT, DRM_FORMAT_RGB565),
          m_image(image),
          m_speed(speed),
          m_x(0)
    {
        add(&image);

        move(pos);

        if (speed < 0)
            m_x = m_image.w() / 2;

        KMSOverlayScreen* screen = reinterpret_cast<KMSOverlayScreen*>(m_screen);
        screen->set_pan_size(size);
        screen->set_pan_pos(Point(m_x, 0));
        screen->apply();
    }

    void reset()
    {
    }

    void restart()
    {
        m_x = 0;
    }

    virtual bool advance()
    {
        bool bounce = false;

        m_x += m_speed;

        if (m_x >= m_image.w() / 2)
        {
            m_x = 0;
            bounce = true;
        }

        KMSOverlayScreen* screen = reinterpret_cast<KMSOverlayScreen*>(m_screen);
        screen->set_pan_pos(Point(m_x, 0));
        screen->apply();

        return bounce;
    }

    virtual ~SceneLayer()
    {}

protected:
    Image& m_image;
    int m_speed;
    int m_x;
};

class MyWindow : public Window
{
public:

    MyWindow()
        : m_running(false),
          m_points(0)
    {
        Image* back0 = new Image("coin_layer0.png");
        add(back0);

        Image* back1 = new Image("coin_layer1.png");
        m_background = new SceneLayer(*back1, Point(0, 180), Size(w(), 300), 6);
        m_background->show();

        m_obstacle = new ObstacleLayer(8);
        m_obstacle->show();

        m_mouse = new HardwareSprite("coin_mouse.png", 120, 97, 2, 0, 0, Point(w() / 2 - 120 / 2, h() / 2 - 97 / 2));
        m_mouse->add_strip(8, 0, 97);
        m_mouse->add_strip(3, 240, 97);
        add(m_mouse);
        m_mouse->show();

        m_label = new Label("",
                            Rect(Point(5, 2),
                                 Size(100, 40)),
                            alignmask::LEFT | alignmask::CENTER);
        m_label->palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::WHITE);
        add(m_label);

        m_go = new Label("GAME OVER",
                         Rect(Point(0, 0),
                              Size(800, 480)),
                         alignmask::CENTER | alignmask::TOP,
                         Font(32));
        m_go->palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::RED);
        add(m_go);

        reset_game();
    }

    void update_points(uint32_t points)
    {
        ostringstream ss;
        ss << "Points: " << points;
        m_label->text(ss.str());
    }

    int handle(int event)
    {
        switch (event)
        {
        case EVT_KEY_REPEAT:
        case EVT_KEY_DOWN:
        {
            m_running = true;

            if (key_value() == KEY_LEFT || key_value() == KEY_RIGHT)
            {
            }
            else if (key_value() == KEY_UP)
            {
                int y = m_mouse->y() - (event == EVT_KEY_REPEAT ? 10 : 5);
                if (y > 0 && y < h() + m_mouse->h())
                    m_mouse->move(Point(m_mouse->x(), y));

                return 1;
            }
            else if (key_value() == KEY_DOWN)
            {
                int y = m_mouse->y() + (event == EVT_KEY_REPEAT ? 10 : 5);
                if (y > 0 && y < h() + m_mouse->h())
                    m_mouse->move(Point(m_mouse->x(), y));

                return 1;
            }

            break;
        }
        case EVT_MOUSE_MOVE:

            m_running = true;

            if (mouse_position().y > m_mouse->box().center().y)
            {
                int delta = mouse_position().y - m_mouse->box().center().y;
                if (delta > 10)
                    delta = 10;
                int y = m_mouse->y() + delta;
                if (y > 0 && y < h() + m_mouse->h())
                    m_mouse->move(Point(m_mouse->x(), y));
            }
            else if (mouse_position().y < m_mouse->box().center().y)
            {
                int delta = m_mouse->box().center().y - mouse_position().y;
                if (delta > 10)
                    delta = 10;
                int y = m_mouse->y() - delta;
                if (y > 0 && y < h() + m_mouse->h())
                    m_mouse->move(Point(m_mouse->x(), y));
            }

            return 1;
        }

        return Window::handle(event);
    }

    void reset_game()
    {
        m_go->show();
        m_points = 0;
        update_points(m_points);
        m_mouse->move(Point(w() / 2 - 120 / 2, h() / 2 - 97 / 2));
        m_mouse->set_strip(0);
        m_background->restart();
        m_background->reset();
        m_obstacle->restart();
        m_obstacle->reset();
        m_running = false;
    }

    void animate()
    {
        if (!m_running)
            return;

        m_go->hide();

        // advance mouse
        static int mouse = 0;
        if (++mouse % 4 == 0)
        {
            if (m_mouse->is_last_frame())
                m_mouse->set_strip(0);
            m_mouse->advance();
        }

        // advance background layer
        m_background->advance();

        // advance obstacle layer
        if (m_obstacle->advance())
        {
            m_obstacle->reset();
        }

        uint32_t points = m_points;
        // perform collision detection
        if (!m_obstacle->collide(m_mouse->box(), m_mouse->surface(), m_points))
        {
            m_mouse->set_strip(2);
            m_mouse->advance();
            reset_game();
        }
        else
        {
            if (points != m_points)
            {
                update_points(m_points);
                m_mouse->set_strip(1);
            }
        }
    }

private:

    SceneLayer* m_background;
    ObstacleLayer* m_obstacle;
    HardwareSprite* m_mouse;
    bool m_running;
    Label* m_label;
    uint32_t m_points;
    Label* m_go;
};

int main()
{
    Application app;

    set_image_path("/root/mui/share/mui/examples/coin/");

    MyWindow win;
    win.show();

    PeriodicTimer animatetimer(std::chrono::milliseconds(30));
    animatetimer.on_timeout([&win]()
    {
        win.animate();
    });
    animatetimer.start();

    return app.run();
}
