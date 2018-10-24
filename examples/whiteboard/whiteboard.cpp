/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include <mui/ui>
#include <mui/painter.h>
#include <string>
#include <vector>
#include <sstream>

using namespace mui;
using namespace std;

class MainWindow : public Window
{
public:

    MainWindow()
        : m_down(false),
          m_color(Color::RED),
          m_grid(Rect(Size(100, 300)), 1, 4, 5),
          m_red("Red"),
          m_blue("Blue"),
          m_green("Green"),
          m_clear("Clear")
    {
        add(&m_grid);

        palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::WHITE);

        m_grid.add(&m_red, 0, 0);
        m_red.add_handler([this](EventWidget * widget, int event)
        {
            ignoreparam(widget);
            ignoreparam(event);
            if (event == EVT_MOUSE_DOWN)
                m_color = Color::RED;
        });

        m_grid.add(&m_blue, 0, 1);
        m_blue.add_handler([this](EventWidget * widget, int event)
        {
            ignoreparam(widget);
            ignoreparam(event);
            if (event == EVT_MOUSE_DOWN)
                m_color = Color::BLUE;
        });

        m_grid.add(&m_green, 0, 2);
        m_green.add_handler([this](EventWidget * widget, int event)
        {
            ignoreparam(widget);
            ignoreparam(event);
            if (event == EVT_MOUSE_DOWN)
                m_color = Color::GREEN;
        });

        m_grid.add(&m_clear, 0, 3);
        m_clear.add_handler([this](EventWidget * widget, int event)
        {
            ignoreparam(widget);
            ignoreparam(event);
            if (event == EVT_MOUSE_DOWN)
            {
                damage();
            }
        });

        //m_grid.reposition();
    }

    int handle(int event)
    {
        switch (event)
        {
        case EVT_MOUSE_DOWN:
        {
            m_last = mouse_position();
            m_down = true;
            break;
        }
        case EVT_MOUSE_MOVE:
        {
            if (m_down)
            {
                if (m_last != mouse_position())
                {
                    Line line = Line(m_last, mouse_position());

                    Painter painter(screen()->context());
                    painter.set_line_width(2.0);
                    painter.set_color(m_color);
                    painter.line(line.start(), line.end());
                    painter.stroke();

                    Rect r = line.rect();
                    r += Size(4, 4);
                    r -= Point(2, 2);

                    IScreen::damage_array damage;
                    damage.push_back(r);
                    screen()->flip(damage);
                }
            }

            m_last = mouse_position();

            break;
        }
        case EVT_MOUSE_UP:
            m_down = false;
            break;
        }

        return Window::handle(event);
    }

    Point m_last;
    bool m_down;
    Color m_color;
    StaticGrid m_grid;
    Button m_red;
    Button m_blue;
    Button m_green;
    Button m_clear;
};

int main()
{
    Application app;
    MainWindow win;
    win.show();

    return app.run();
}
