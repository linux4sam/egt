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

    explicit ColorPickerWindow(const Color& color)
        : Popup<PlaneWindow>(main_screen()->size() / 2),
          m_grid(Rect(Point(0, 0), main_screen()->size() / 2), 4, 5, 10),
          m_color(color)
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

            if (c == m_color)
                m_grid.select(column, row);

            color->on_event([this, column, row](eventid event)
            {
                if (event == eventid::MOUSE_DOWN)
                {
                    m_grid.select(column, row);
                    m_color = m_grid.get(m_grid.selected())->palette().color(Palette::BG);
                    this->hide();
                }
                return 0;
            });
        }
    }

    const Color& color() const { return m_color; }

protected:
    SelectableGrid m_grid;
    Color m_color{Color::RED};
};

class WidthPickerWindow : public Popup<Window>
{
public:

    explicit WidthPickerWindow(int width)
        : Popup<Window>(main_screen()->size() / 2),
          m_grid(Rect(Point(0, 0), main_screen()->size() / 2), 4, 1, 10),
          m_width(width)
    {
        palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::BLACK);

        add(&m_grid);

        const vector<int> widths =
        {
            1,
            2,
            5,
            10
        };

        for (auto& w : widths)
        {
            auto width = new Label(std::to_string(w));

            m_grid.add(width);
            int column = m_grid.last_add_column();
            int row = m_grid.last_add_row();

            if (w == m_width)
                m_grid.select(column, row);

            width->on_event([this, column, row](eventid event)
            {
                if (event == eventid::MOUSE_DOWN)
                {
                    m_grid.select(column, row);
                    m_width = std::stoi(reinterpret_cast<Label*>(m_grid.get(m_grid.selected()))->text());
                    this->hide();
                }
                return 0;
            });
        }
    }

    int width() const { return m_width; }

protected:
    SelectableGrid m_grid;
    int m_width{2};
};

class MainWindow : public Window
{
public:

    MainWindow()
        : m_down(false),
          m_grid(Rect(Size(100, 250)), 1, 4, 5),
          m_colorbtn("palette.png"),
          m_fillbtn("fill.png"),
          m_widthbtn("width.png"),
          m_clearbtn("clear.png"),
          m_penpicker(Color::BLUE),
          m_fillpicker(Color::WHITE),
          m_widthpicker(2),
          m_canvas(screen()->size(), CAIRO_FORMAT_ARGB32)
    {
        // don't draw background, we'll do it in draw()
        flag_set(widgetmask::NO_BACKGROUND);

        add(&m_grid);
        add(&m_penpicker);
        add(&m_fillpicker);
        add(&m_widthpicker);

        m_grid.add(&m_colorbtn);
        m_colorbtn.on_event([this](eventid event)
        {
            if (event == eventid::MOUSE_DOWN)
            {
                m_penpicker.show_modal(true);
                return 1;
            }
            return 0;
        });

        m_grid.add(&m_fillbtn);
        m_fillbtn.on_event([this](eventid event)
        {
            if (event == eventid::MOUSE_DOWN)
            {
                m_fillpicker.show_modal(true);
                return 1;
            }
            return 0;
        });

        m_grid.add(&m_widthbtn);
        m_widthbtn.on_event([this](eventid event)
        {
            if (event == eventid::MOUSE_DOWN)
            {
                m_widthpicker.show_modal(true);
                return 1;
            }
            return 0;
        });

        m_grid.add(&m_clearbtn);
        m_clearbtn.on_event([this](eventid event)
        {
            ignoreparam(event);
            if (event == eventid::MOUSE_DOWN)
            {
                clear();
                damage();
                return 1;
            }
            return 0;
        });

        m_fillpicker.on_event([this](eventid event)
        {
            if (event == eventid::HIDE)
            {
                palette().set(Palette::BG, Palette::GROUP_NORMAL, m_fillpicker.color());
                damage();
            }
            return 0;
        });

        clear();
    }

    void clear()
    {
        Painter painter(m_canvas.context());
        cairo_set_operator(painter.context().get(), CAIRO_OPERATOR_SOURCE);
        painter.set_color(Color::TRANSPARENT);
        painter.draw_fill(box());
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
                    int width = m_widthpicker.width();

                    Line line(m_last, event_mouse());
                    Painter painter(m_canvas.context());
                    painter.set_line_width(width);
                    painter.set_color(m_penpicker.color());
                    painter.line(line.start(), line.end());
                    painter.stroke();

                    // damage only the rectangle containing the new line
                    Rect r = line.rect();
                    r += Size(width * 2, width * 2);
                    r -= Point(width, width);
                    damage(r);
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

    void draw(Painter& painter, const Rect& rect) override
    {
        painter.set_color(palette().color(Palette::BG, Palette::GROUP_NORMAL));
        painter.draw_fill(rect);

        painter.draw_image(rect, rect.point(), m_canvas.surface());

        Window::draw(painter, rect);
    }

    Point m_last;
    bool m_down;
    StaticGrid m_grid;
    ImageButton m_colorbtn;
    ImageButton m_fillbtn;
    ImageButton m_widthbtn;
    ImageButton m_clearbtn;
    ColorPickerWindow m_penpicker;
    ColorPickerWindow m_fillpicker;
    WidthPickerWindow m_widthpicker;
    Canvas m_canvas;
};

int main(int argc, const char** argv)
{
    Application app(argc, argv);

    set_image_path("../share/egt/examples/whiteboard/");

    MainWindow win;
    win.show();

    return app.run();
}
