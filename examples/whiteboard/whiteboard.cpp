/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <egt/painter.h>
#include <string>
#include <vector>
#include <sstream>

using namespace egt;
using namespace std;

class MainWindow : public Window
{
public:

    MainWindow()
        : m_down(false),
          m_color(Color::RED),
          m_grid(Rect(Size(100, 400)), 1, 4, 5),
          m_red("Red"),
          m_blue("Blue"),
          m_green("Green"),
          m_clear("clear.png")
    {
        add(&m_grid);

        palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::WHITE);

        m_grid.add(&m_red, 0, 0);
        m_red.on_event([this](eventid event)
        {
            if (event == eventid::MOUSE_DOWN)
                m_color = Color::RED;
        });

        m_grid.add(&m_blue, 0, 1);
        m_blue.on_event([this](eventid event)
        {
            ignoreparam(event);
            if (event == eventid::MOUSE_DOWN)
                m_color = Color::BLUE;
        });

        m_grid.add(&m_green, 0, 2);
        m_green.on_event([this](eventid event)
        {
            ignoreparam(event);
            if (event == eventid::MOUSE_DOWN)
                m_color = Color::GREEN;
        });

        m_grid.add(&m_clear, 0, 3);
        m_clear.on_event([this](eventid event)
        {
            ignoreparam(event);
            if (event == eventid::MOUSE_DOWN)
            {
                damage();
            }
        });

        //m_grid.reposition();
    }

    int handle(eventid event) override
    {
        switch (event)
        {
        case eventid::MOUSE_DOWN:
        {
            m_last = event_mouse();
            m_down = true;
            break;
        }
        case eventid::MOUSE_MOVE:
        {
            if (m_down)
            {
                if (m_last != event_mouse())
                {
                    Line line = Line(m_last, event_mouse());

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

            m_last = event_mouse();

            break;
        }
        case eventid::MOUSE_UP:
            m_down = false;
            break;
        default:
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
    ImageButton m_clear;
};

int main()
{
    Application app;

    set_image_path("../share/egt/examples/whiteboard/");

    MainWindow win;
    win.show();

    return app.run();
}
