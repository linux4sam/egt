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

class ColorPickerWindow : public Popup
{
public:

    explicit ColorPickerWindow(const Color& color)
        : Popup(main_screen()->size() / 2),
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
                if (event == eventid::POINTER_CLICK)
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

class WidthPickerWindow : public Popup
{
public:

    explicit WidthPickerWindow(int width)
        : Popup(main_screen()->size() / 2),
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
                if (event == eventid::POINTER_CLICK)
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

class MainWindow : public TopWindow
{
public:

    MainWindow()
        : m_grid(Rect(Size(100, 250)), 1, 4, 5),
          m_colorbtn(Image("palette.png")),
          m_fillbtn(Image("fill.png")),
          m_widthbtn(Image("width.png")),
          m_clearbtn(Image("clear.png")),
          m_penpicker(Color::BLUE),
          m_fillpicker(Color::WHITE),
          m_widthpicker(2),
          m_canvas(screen()->size(), CAIRO_FORMAT_ARGB32)
    {
        // don't draw background, we'll do it in draw()
        set_boxtype(Theme::boxtype::none);

        m_grid.palette().set(Palette::BORDER, Palette::GROUP_NORMAL, Color::TRANSPARENT);

        m_colorbtn.set_boxtype(Theme::boxtype::none);
        m_fillbtn.set_boxtype(Theme::boxtype::none);
        m_widthbtn.set_boxtype(Theme::boxtype::none);
        m_clearbtn.set_boxtype(Theme::boxtype::none);

        add(&m_grid);
        add(&m_penpicker);
        add(&m_fillpicker);
        add(&m_widthpicker);

        m_grid.add(&m_colorbtn);
        m_colorbtn.on_event([this](eventid)
        {
            m_penpicker.show_modal(true);
            return 1;
        }, {eventid::POINTER_CLICK});

        m_colorbtn.on_event([this](eventid)
        {
            m_penpicker.show_modal(true);
            return 1;
        }, {eventid::POINTER_CLICK});

        m_colorbtn.on_event([this](eventid)
        {
            m_penpicker.show_modal(true);
            return 1;
        }, {eventid::POINTER_CLICK});

        m_colorbtn.on_event([this](eventid)
        {
            m_penpicker.show_modal(true);
            return 1;
        }, {eventid::POINTER_CLICK});

        m_grid.add(&m_fillbtn);
        m_fillbtn.on_event([this](eventid)
        {
            m_fillpicker.show_modal(true);
            return 1;
        }, {eventid::POINTER_CLICK});

        m_grid.add(&m_widthbtn);
        m_widthbtn.on_event([this](eventid)
        {
            m_widthpicker.show_modal(true);
            return 1;
        }, {eventid::POINTER_CLICK});

        m_grid.add(&m_clearbtn);
        m_clearbtn.on_event([this](eventid)
        {
            clear();
            damage();
            return 1;
        }, {eventid::POINTER_CLICK});

        m_fillpicker.on_event([this](eventid)
        {
            palette().set(Palette::BG, Palette::GROUP_NORMAL, m_fillpicker.color());
            damage();
            return 1;
        }, {eventid::HIDE});

        auto logo = new ImageLabel(Image("@microchip_logo_black.png"));
        add(logo)->set_align(alignmask::RIGHT | alignmask::TOP, 10);

        clear();
    }

    void clear()
    {
        Painter painter(m_canvas.context());
        cairo_set_operator(painter.context().get(), CAIRO_OPERATOR_SOURCE);
        painter.set_color(Color::TRANSPARENT);
        painter.paint();
    }

    int handle(eventid event) override
    {
        auto ret = TopWindow::handle(event);
        if (ret)
            return ret;

        switch (event)
        {
        case eventid::POINTER_DRAG_START:
            m_last = from_display(event::pointer().point);
            break;
        case eventid::POINTER_DRAG:
        {
            auto mouse = from_display(event::pointer().point);

            if (m_last != mouse)
            {
                int width = m_widthpicker.width();

                Line line(m_last, mouse);
                Painter painter(m_canvas.context());
                painter.set_line_width(width);
                auto cr = painter.context();
                cairo_set_line_cap(cr.get(), CAIRO_LINE_CAP_ROUND);
                painter.set_color(m_penpicker.color());
                painter.line(line.start(), line.end());
                painter.stroke();

                // damage only the rectangle containing the new line
                Rect r = line.rect();
                r += Size(width * 2, width * 2);
                r -= Point(width, width);
                damage(r);
            }

            m_last = mouse;

            break;
        }
        default:
            break;
        }

        return 0;
    }

    void draw(Painter& painter, const Rect& rect) override
    {
        painter.set_color(palette().color(Palette::BG, Palette::GROUP_NORMAL));
        painter.draw_fill(rect);

        painter.draw_image(rect, rect.point(), Image(m_canvas.surface()));

        TopWindow::draw(painter, rect);
    }

    Point m_last;
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
    Application app(argc, argv, "whiteboard");

    MainWindow win;
    win.show_cursor(Image("@pencil.png"));
    win.show();

    return app.run();
}
