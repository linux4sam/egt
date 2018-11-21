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

class ColorPickerWindow : public Popup<PlaneWindow>
{
public:
    ColorPickerWindow()
        : Popup<PlaneWindow>(main_screen()->size() / 2),
          m_grid(Rect(Point(0, 0), main_screen()->size() / 2), 4, 5, 10)
    {
        palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::BLACK);

        add(&m_grid);

        const vector<Color> colors =
        {
            Color::RED,
            Color::GREEN,
            Color::BLUE,
            Color::YELLOW,
            Color::CYAN,
            Color::MAGENTA,
            Color::SILVER,
            Color::GRAY,
            Color::LIGHTGRAY,
            Color::MAROON,
            Color::OLIVE,
            Color::PURPLE,
            Color::TEAL,
            Color::NAVY,
            Color::ORANGE,
            Color::BLACK,
            Color::CSS("#F012BE"),
            Color::CSS("#7FDBFF"),
            Color::CSS("#01FF70"),
            Color::CSS("#AA0070"),
        };

        for (auto& c : colors)
        {
            auto color = new Label("");
            color->palette().set(Palette::BG, Palette::GROUP_NORMAL, c);

            m_grid.add(color);
            int column = m_grid.last_add_column();
            int row = m_grid.last_add_row();

            color->on_event([this, column, row](eventid event)
            {
                if (event == eventid::MOUSE_UP)
                {
                    m_grid.select(column, row);
                    m_color = m_grid.get(m_grid.selected())->palette().color(Palette::BG);
                    this->hide();
                    return 1;
                }
                else if (event == eventid::MOUSE_DOWN)
                    return 1;
                return 0;
            });
        }
    }

    Color m_color{Color::RED};

protected:
    SelectableGrid m_grid;
};

class MainWindow : public Window
{
public:

    MainWindow()
        : m_down(false),
          m_grid(Rect(Size(100, 200)), 1, 2, 5),
          m_colorbtn("palette.png"),
          m_clearbtn("clear.png")
    {
        add(&m_grid);
        add(&m_colorpicker);

        palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::WHITE);

        m_grid.add(&m_colorbtn, 0, 0);
        m_colorbtn.on_event([this](eventid event)
        {
            if (event == eventid::MOUSE_DOWN)
            {
                m_colorpicker.show(true);
                return 1;
            }
            return 0;
        });

        m_grid.add(&m_clearbtn, 0, 1);
        m_clearbtn.on_event([this](eventid event)
        {
            ignoreparam(event);
            if (event == eventid::MOUSE_DOWN)
            {
                damage();
                return 1;
            }
            return 0;
        });
    }

    int handle(eventid event) override
    {
        auto ret = Window::handle(event);
        if (ret)
            return ret;

        switch (event)
        {
        case eventid::MOUSE_DOWN:
        {
            m_last = event_mouse();
            m_down = true;
            return 1;
        }
        case eventid::MOUSE_MOVE:
        {
            if (m_down)
            {
                if (m_last != event_mouse())
                {
#if 0
                    for (auto& i : m_children)
                    {
                        if (alpha_collision(i->box(), i->surface(), event_mouse()))
                            return 0;
                    }
#endif

                    Line line = Line(m_last, event_mouse());

                    Painter painter(screen()->context());
                    painter.set_line_width(2.0);
                    painter.set_color(m_colorpicker.m_color);
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

            return 1;
        }
        case eventid::MOUSE_UP:
            m_down = false;
            return 1;
        default:
            break;
        }

        return 0;
    }

    Point m_last;
    bool m_down;
    StaticGrid m_grid;
    ImageButton m_colorbtn;
    ImageButton m_clearbtn;
    ColorPickerWindow m_colorpicker;
};

int main(int argc, const char** argv)
{
    Application app(argc, argv);

    set_image_path("../share/egt/examples/whiteboard/");

    MainWindow win;
    win.show();

    return app.run();
}
