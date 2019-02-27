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
        palette().set(Palette::ColorId::bg, Palette::GroupId::normal, Palette::black);

        add(m_grid);

        const vector<Color> colors =
        {
            Palette::red,
            Palette::green,
            Palette::blue,
            Palette::yellow,
            Palette::cyan,
            Palette::magenta,
            Palette::silver,
            Palette::gray,
            Palette::lightgray,
            Palette::maroon,
            Palette::olive,
            Palette::purple,
            Palette::teal,
            Palette::navy,
            Palette::orange,
            Palette::black,
            Color::CSS("#F012BE"),
            Color::CSS("#7FDBFF"),
            Color::CSS("#01FF70"),
            Color::CSS("#AA0070"),
        };

        for (auto& c : colors)
        {
            auto color = make_shared<Label>("");
            color->palette().set(Palette::ColorId::bg, Palette::GroupId::normal, c);

            m_grid.add(expand(color));
            int column = m_grid.last_add_column();
            int row = m_grid.last_add_row();

            if (c == m_color)
                m_grid.select(column, row);

            color->on_event([this, column, row](eventid event)
            {
                if (event == eventid::pointer_click)
                {
                    m_grid.select(column, row);
                    m_color = m_grid.get(m_grid.selected())->palette().color(Palette::ColorId::bg);
                    this->hide();
                }
                return 0;
            });
        }
    }

    const Color& color() const { return m_color; }

protected:
    SelectableGrid m_grid;
    Color m_color{Palette::red};
};

class WidthPickerWindow : public Popup
{
public:

    explicit WidthPickerWindow(int width)
        : Popup(main_screen()->size() / 2),
          m_grid(Rect(Point(0, 0), main_screen()->size() / 2), 4, 1, 10),
          m_width(width)
    {
        palette().set(Palette::ColorId::bg, Palette::GroupId::normal, Palette::black);

        add(m_grid);

        const vector<int> widths =
        {
            1,
            2,
            5,
            10
        };

        for (auto& w : widths)
        {
            auto width = make_shared<Label>(std::to_string(w));

            m_grid.add(expand(width));
            int column = m_grid.last_add_column();
            int row = m_grid.last_add_row();

            if (w == m_width)
                m_grid.select(column, row);

            width->on_event([this, column, row](eventid event)
            {
                if (event == eventid::pointer_click)
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
          m_penpicker(Palette::blue),
          m_fillpicker(Palette::white),
          m_widthpicker(2),
          m_canvas(screen()->size(), CAIRO_FORMAT_ARGB32)
    {
        // don't draw background, we'll do it in draw()
        set_boxtype(Theme::boxtype::none);

        m_grid.palette().set(Palette::ColorId::border, Palette::GroupId::normal, Palette::transparent);

        m_colorbtn.set_boxtype(Theme::boxtype::none);
        m_fillbtn.set_boxtype(Theme::boxtype::none);
        m_widthbtn.set_boxtype(Theme::boxtype::none);
        m_clearbtn.set_boxtype(Theme::boxtype::none);

        add(m_grid);
        add(m_penpicker);
        add(m_fillpicker);
        add(m_widthpicker);

        m_grid.add(expand(m_colorbtn));
        m_colorbtn.on_event([this](eventid)
        {
            m_penpicker.show_modal(true);
            return 1;
        }, {eventid::pointer_click});

        m_colorbtn.on_event([this](eventid)
        {
            m_penpicker.show_modal(true);
            return 1;
        }, {eventid::pointer_click});

        m_colorbtn.on_event([this](eventid)
        {
            m_penpicker.show_modal(true);
            return 1;
        }, {eventid::pointer_click});

        m_colorbtn.on_event([this](eventid)
        {
            m_penpicker.show_modal(true);
            return 1;
        }, {eventid::pointer_click});

        m_grid.add(expand(m_fillbtn));
        m_fillbtn.on_event([this](eventid)
        {
            m_fillpicker.show_modal(true);
            return 1;
        }, {eventid::pointer_click});

        m_grid.add(expand(m_widthbtn));
        m_widthbtn.on_event([this](eventid)
        {
            m_widthpicker.show_modal(true);
            return 1;
        }, {eventid::pointer_click});

        m_grid.add(expand(m_clearbtn));
        m_clearbtn.on_event([this](eventid)
        {
            clear();
            damage();
            return 1;
        }, {eventid::pointer_click});

        m_fillpicker.on_event([this](eventid)
        {
            palette().set(Palette::ColorId::bg, Palette::GroupId::normal, m_fillpicker.color());
            damage();
            return 1;
        }, {eventid::hide});

        auto logo = make_shared<ImageLabel>(Image("@microchip_logo_black.png"));
        logo->set_align(alignmask::right | alignmask::top, 10);
        add(logo);

        clear();
    }

    void clear()
    {
        Painter painter(m_canvas.context());
        cairo_set_operator(painter.context().get(), CAIRO_OPERATOR_SOURCE);
        painter.set_color(Palette::transparent);
        painter.paint();
    }

    int handle(eventid event) override
    {
        auto ret = TopWindow::handle(event);
        if (ret)
            return ret;

        switch (event)
        {
        case eventid::pointer_drag_start:
            m_last = from_display(event::pointer().point);
            break;
        case eventid::pointer_drag:
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
        painter.set_color(palette().color(Palette::ColorId::bg, Palette::GroupId::normal));
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
