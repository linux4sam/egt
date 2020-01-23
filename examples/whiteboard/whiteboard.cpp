/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <array>
#include <egt/painter.h>
#include <egt/ui>
#include <memory>
#include <sstream>
#include <string>

class ColorPickerWindow : public egt::Popup
{
public:

    explicit ColorPickerWindow(const egt::Color& color)
        : egt::Popup(egt::Application::instance().screen()->size() / 2),
          m_grid(std::make_tuple(4, 5), 10),
          m_color(color)
    {
        expand(m_grid);
        egt::Popup::color(egt::Palette::ColorId::bg, egt::Palette::black);
        add(m_grid);

        const std::array<egt::Pattern, 20> colors =
        {
            egt::Palette::red,
            egt::Palette::green,
            egt::Palette::blue,
            egt::Palette::yellow,
            egt::Palette::cyan,
            egt::Palette::magenta,
            egt::Palette::silver,
            egt::Palette::gray,
            egt::Palette::lightgray,
            egt::Palette::maroon,
            egt::Palette::olive,
            egt::Palette::purple,
            egt::Palette::teal,
            egt::Palette::navy,
            egt::Palette::orange,
            egt::Palette::black,
            egt::Color::css("#F012BE"),
            egt::Color::css("#7FDBFF"),
            egt::Color::css("#01FF70"),
            egt::Color::css("#AA0070"),
        };

        for (auto& c : colors)
        {
            auto color_label = std::make_shared<egt::RectangleWidget>();

            color_label->color(egt::Palette::ColorId::button_bg, c);

            m_grid.add(expand(color_label));
            int column = m_grid.last_add_column();
            int row = m_grid.last_add_row();

            if (c == m_color)
                m_grid.selected(column, row);

            color_label->on_event([this](egt::Event&)
            {
                m_color = m_grid.get(m_grid.selected())->color(egt::Palette::ColorId::button_bg).color();
                this->hide();
            }, egt::EventId::pointer_click);
        }
    }

    const egt::Color& color() const { return m_color; }

protected:
    egt::SelectableGrid m_grid;
    egt::Color m_color{egt::Palette::red};
};

class WidthPickerWindow : public egt::Popup
{
public:

    explicit WidthPickerWindow(int width)
        : egt::Popup(egt::Application::instance().screen()->size() / 4),
          m_grid(std::make_tuple(4, 1), 10),
          m_width(width)
    {
        expand(m_grid);
        color(egt::Palette::ColorId::bg, egt::Palette::black);
        add(m_grid);

        const std::array<int, 4> widths =
        {
            1,
            2,
            5,
            10
        };

        for (auto& w : widths)
        {
            auto width_label = std::make_shared<egt::Label>(std::to_string(w), egt::AlignFlag::center);
            width_label->color(egt::Palette::ColorId::label_text, egt::Palette::white);

            m_grid.add(egt::expand(width_label));
            int column = m_grid.last_add_column();
            int row = m_grid.last_add_row();

            if (w == m_width)
                m_grid.selected(column, row);

            width_label->on_event([this](egt::Event&)
            {
                m_width = std::stoi(reinterpret_cast<egt::Label*>(m_grid.get(m_grid.selected()))->text());
                this->hide();
            }, {egt::EventId::pointer_click});
        }
    }

    inline int width() const { return m_width; }

protected:
    egt::SelectableGrid m_grid;
    int m_width{2};
};


class MainWindow : public egt::TopWindow
{
public:

    MainWindow()
        : m_colorbtn(egt::Image("palette.png")),
          m_fillbutton(egt::Image("fill.png")),
          m_widthbtn(egt::Image("width.png")),
          m_clearbtn(egt::Image("clear.png")),
          m_snapshotbtn(egt::Image("screenshot.png")),
          m_penpicker(egt::Palette::blue),
          m_fillpicker(egt::Palette::red),
          m_widthpicker(2),
          m_canvas(screen()->size(), egt::PixelFormat::argb8888)
    {
        // don't draw background, we'll do it in draw()
        boxtype().clear();
        color(egt::Palette::ColorId::bg, egt::Palette::white);

        m_sizer = std::make_shared<egt::VerticalBoxSizer>(*this);
        top(left(m_sizer));

        m_colorbtn.boxtype().clear();
        m_fillbutton.boxtype().clear();
        m_widthbtn.boxtype().clear();
        m_clearbtn.boxtype().clear();
        m_snapshotbtn.boxtype().clear();

        m_colorbtn.image_align(egt::AlignFlag::expand);
        m_fillbutton.image_align(egt::AlignFlag::expand);
        m_widthbtn.image_align(egt::AlignFlag::expand);
        m_clearbtn.image_align(egt::AlignFlag::expand);
        m_snapshotbtn.image_align(egt::AlignFlag::expand);

        add(m_penpicker);
        add(m_fillpicker);
        add(m_widthpicker);

        m_sizer->add(m_colorbtn);
        m_sizer->add(m_fillbutton);
        m_sizer->add(m_widthbtn);
        m_sizer->add(m_clearbtn);
        m_sizer->add(m_snapshotbtn);

        m_colorbtn.on_click([this](egt::Event&)
        {
            m_penpicker.show_modal(true);
        });

        m_fillbutton.on_click([this](egt::Event&)
        {
            m_fillpicker.show_modal(true);
        });

        m_widthbtn.on_click([this](egt::Event&)
        {
            m_widthpicker.show_modal(true);
        });

        m_clearbtn.on_click([this](egt::Event&)
        {
            clear();
            damage();
        });

        m_snapshotbtn.on_click([this](egt::Event&)
        {
            paint_to_file();
        });

        auto logo = std::make_shared<egt::ImageLabel>(egt::Image("icon:128px/egt_logo_black.png"));
        logo->align(egt::AlignFlag::right | egt::AlignFlag::top);
        logo->margin(10);
        add(logo);

        clear();
    }

    void clear()
    {
        egt::Painter painter(m_canvas.context());
        cairo_set_operator(painter.context().get(), CAIRO_OPERATOR_SOURCE);
        painter.set(egt::Palette::transparent);
        painter.paint();
    }

    void handle(egt::Event& event) override
    {
        egt::TopWindow::handle(event);

        switch (event.id())
        {
        case egt::EventId::pointer_click:
        {
            auto mouse = display_to_local(event.pointer().point);
            egt::Painter painter(m_canvas.context());
            cairo_set_antialias(painter.context().get(), CAIRO_ANTIALIAS_NONE);
            painter.flood(mouse, m_fillpicker.color());
            damage();
            break;
        }
        case egt::EventId::pointer_drag_start:
            m_last = display_to_local(event.pointer().point);
            event.grab(this);
            break;
        case egt::EventId::pointer_drag:
        {
            auto mouse = display_to_local(event.pointer().point);

            if (m_last != mouse)
            {
                int width = m_widthpicker.width();

                egt::Line line(m_last, mouse);
                egt::Painter painter(m_canvas.context());
                cairo_set_antialias(painter.context().get(), CAIRO_ANTIALIAS_NONE);
                painter.line_width(width);
                auto cr = painter.context();
                cairo_set_line_cap(cr.get(), CAIRO_LINE_CAP_ROUND);
                painter.set(m_penpicker.color());
                painter.draw(line.start(), line.end());
                painter.stroke();

                // damage only the rectangle containing the new line
                auto r = line.rect();
                r += egt::Size(width * 2, width * 2);
                r -= egt::Point(width, width);
                damage(r);
            }

            m_last = mouse;

            break;
        }
        default:
            break;
        }
    }

    void draw(egt::Painter& painter, const egt::Rect& rect) override
    {
        painter.set(color(egt::Palette::ColorId::bg).color());
        painter.draw(rect);
        painter.fill();

        painter.draw(rect.point());
        painter.draw(rect, egt::Image(m_canvas.surface()));

        egt::TopWindow::draw(painter, rect);
    }

    egt::Point m_last;
    std::shared_ptr<egt::VerticalBoxSizer> m_sizer;
    egt::ImageButton m_colorbtn;
    egt::ImageButton m_fillbutton;
    egt::ImageButton m_widthbtn;
    egt::ImageButton m_clearbtn;
    egt::ImageButton m_snapshotbtn;
    ColorPickerWindow m_penpicker;
    ColorPickerWindow m_fillpicker;
    WidthPickerWindow m_widthpicker;
    egt::Canvas m_canvas;
};

static int run(int argc, const char** argv)
{
    egt::Application app(argc, argv, "whiteboard");

    MainWindow win;
    win.show_cursor(egt::Image("icon:cursor_pencil.png"));
    win.show();

    return app.run();
}

int main(int argc, const char** argv)
{
    auto res = run(argc, argv);

    // cleanup any font allocations
    egt::Font::shutdown_fonts();

    return res;
}
