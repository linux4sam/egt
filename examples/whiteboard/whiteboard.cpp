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
          m_grid(Tuple(4, 5), 10),
          m_color(color)
    {
        expand(m_grid);
        set_color(Palette::ColorId::bg, Palette::black);
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
            Color::css("#F012BE"),
            Color::css("#7FDBFF"),
            Color::css("#01FF70"),
            Color::css("#AA0070"),
        };

        for (auto& c : colors)
        {
            auto color_label = make_shared<RectangleWidget>();

            color_label->set_color(Palette::ColorId::button_bg, c);

            m_grid.add(expand(color_label));
            int column = m_grid.last_add_column();
            int row = m_grid.last_add_row();

            if (c == m_color)
                m_grid.set_selected(column, row);

            color_label->on_event([this, column, row](Event&)
            {
                m_color = m_grid.get(m_grid.selected())->color(Palette::ColorId::button_bg).color();
                this->hide();
            }, {eventid::pointer_click});
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
        : Popup(main_screen()->size() / 4),
          m_grid(Tuple(4, 1), 10),
          m_width(width)
    {
        expand(m_grid);
        set_color(Palette::ColorId::bg, Palette::black);
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
            auto width_label = make_shared<Label>(std::to_string(w), alignmask::center);
            width_label->set_color(Palette::ColorId::label_text, Palette::white);

            m_grid.add(expand(width_label));
            int column = m_grid.last_add_column();
            int row = m_grid.last_add_row();

            if (w == m_width)
                m_grid.set_selected(column, row);

            width_label->on_event([this, column, row](Event&)
            {
                m_width = std::stoi(reinterpret_cast<Label*>(m_grid.get(m_grid.selected()))->text());
                this->hide();
            }, {eventid::pointer_click});
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
        : m_colorbtn(Image("palette.png")),
          m_fillbutton(Image("fill.png")),
          m_widthbtn(Image("width.png")),
          m_clearbtn(Image("clear.png")),
          m_snapshotbtn(Image("screenshot.png")),
          m_penpicker(Palette::blue),
          m_fillpicker(Palette::red),
          m_widthpicker(2),
          m_canvas(screen()->size(), pixel_format::argb8888)
    {
        // don't draw background, we'll do it in draw()
        set_boxtype(Theme::boxtype::none);
        set_color(Palette::ColorId::bg, Palette::white);

        m_sizer = make_shared<VerticalBoxSizer>(*this);
        top(left(m_sizer));

        m_colorbtn.set_boxtype(Theme::boxtype::none);
        m_fillbutton.set_boxtype(Theme::boxtype::none);
        m_widthbtn.set_boxtype(Theme::boxtype::none);
        m_clearbtn.set_boxtype(Theme::boxtype::none);
        m_snapshotbtn.set_boxtype(Theme::boxtype::none);

        m_colorbtn.set_image_align(alignmask::expand);
        m_fillbutton.set_image_align(alignmask::expand);
        m_widthbtn.set_image_align(alignmask::expand);
        m_clearbtn.set_image_align(alignmask::expand);
        m_snapshotbtn.set_image_align(alignmask::expand);

        add(m_penpicker);
        add(m_fillpicker);
        add(m_widthpicker);

        m_sizer->add(m_colorbtn);
        m_sizer->add(m_fillbutton);
        m_sizer->add(m_widthbtn);
        m_sizer->add(m_clearbtn);
        m_sizer->add(m_snapshotbtn);

        m_colorbtn.on_event([this](Event&)
        {
            m_penpicker.show_modal(true);
        }, {eventid::pointer_click});

        m_fillbutton.on_event([this](Event&)
        {
            m_fillpicker.show_modal(true);
        }, {eventid::pointer_click});

        m_widthbtn.on_event([this](Event&)
        {
            m_widthpicker.show_modal(true);
        }, {eventid::pointer_click});

        m_clearbtn.on_event([this](Event&)
        {
            clear();
            damage();
        }, {eventid::pointer_click});

        m_snapshotbtn.on_event([this](Event&)
        {
            paint_to_file();
        }, {eventid::pointer_click});

        auto logo = make_shared<ImageLabel>(Image("@128px/egt_logo_black.png"));
        logo->set_align(alignmask::right | alignmask::top);
        logo->set_margin(10);
        add(logo);

        clear();
    }

    void clear()
    {
        Painter painter(m_canvas.context());
        cairo_set_operator(painter.context().get(), CAIRO_OPERATOR_SOURCE);
        painter.set(Palette::transparent);
        painter.paint();
    }

    void handle(Event& event) override
    {
        TopWindow::handle(event);

        switch (event.id())
        {
        case eventid::pointer_click:
        {
            auto mouse = display_to_local(event.pointer().point);
            Painter painter(m_canvas.context());
            cairo_set_antialias(painter.context().get(), CAIRO_ANTIALIAS_NONE);
            painter.flood(mouse, m_fillpicker.color());
            damage();
            break;
        }
        case eventid::pointer_drag_start:
            m_last = display_to_local(event.pointer().point);
            event.grab(this);
            break;
        case eventid::pointer_drag:
        {
            auto mouse = display_to_local(event.pointer().point);

            if (m_last != mouse)
            {
                int width = m_widthpicker.width();

                Line line(m_last, mouse);
                Painter painter(m_canvas.context());
                cairo_set_antialias(painter.context().get(), CAIRO_ANTIALIAS_NONE);
                painter.set_line_width(width);
                auto cr = painter.context();
                cairo_set_line_cap(cr.get(), CAIRO_LINE_CAP_ROUND);
                painter.set(m_penpicker.color());
                painter.draw(line.start(), line.end());
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
    }

    void draw(Painter& painter, const Rect& rect) override
    {
        painter.set(color(Palette::ColorId::bg).color());
        painter.draw(rect);
        painter.fill();

        painter.draw(rect.point());
        painter.draw(rect, Image(m_canvas.surface()));

        TopWindow::draw(painter, rect);
    }

    Point m_last;
    shared_ptr<VerticalBoxSizer> m_sizer;
    ImageButton m_colorbtn;
    ImageButton m_fillbutton;
    ImageButton m_widthbtn;
    ImageButton m_clearbtn;
    ImageButton m_snapshotbtn;
    ColorPickerWindow m_penpicker;
    ColorPickerWindow m_fillpicker;
    WidthPickerWindow m_widthpicker;
    Canvas m_canvas;
};

int main(int argc, const char** argv)
{
    Application app(argc, argv, "whiteboard");

    MainWindow win;
    win.show_cursor(Image("@cursor_pencil.png"));
    win.show();

    return app.run();
}
