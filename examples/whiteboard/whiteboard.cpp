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
          m_grid(egt::SelectableGrid::GridSize(4, 5)),
          m_color(color)
    {
        expand(m_grid);
        m_grid.margin(10);
        m_grid.horizontal_space(10);
        m_grid.vertical_space(10);
        m_grid.selection_highlight(5);
        egt::Popup::color(egt::Palette::ColorId::bg, egt::Palette::black);
        add(m_grid);

        static const std::array<egt::Pattern, 20> colors =
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
            const auto column = m_grid.last_add_column();
            const auto row = m_grid.last_add_row();

            if (c == m_color)
                m_grid.selected(column, row);

            color_label->on_event([this](egt::Event&)
            {
                m_color = m_grid.get(m_grid.selected())->color(egt::Palette::ColorId::button_bg);
                this->hide();
            }, egt::EventId::pointer_click);
        }
    }

    const egt::Pattern& selected_color() const { return m_color; }

protected:
    egt::SelectableGrid m_grid;
    egt::Pattern m_color{egt::Palette::red};
};

class WidthPickerWindow : public egt::Popup
{
public:

    explicit WidthPickerWindow(int width)
        : egt::Popup(egt::Application::instance().screen()->size() / 4),
          m_grid(egt::SelectableGrid::GridSize(4, 1)),
          m_width(width)
    {
        expand(m_grid);
        m_grid.margin(10);
        m_grid.horizontal_space(10);
        m_grid.vertical_space(10);
        m_grid.selection_highlight(5);
        color(egt::Palette::ColorId::bg, egt::Palette::black);
        add(m_grid);

        static constexpr std::array<int, 4> widths =
        {
            1,
            2,
            5,
            10
        };

        for (const auto& w : widths)
        {
            auto width_label = std::make_shared<egt::Label>(std::to_string(w), egt::AlignFlag::center);
            width_label->color(egt::Palette::ColorId::label_text, egt::Palette::white);

            m_grid.add(egt::expand(width_label));
            const auto column = m_grid.last_add_column();
            const auto row = m_grid.last_add_row();

            if (w == m_width)
                m_grid.selected(column, row);

            width_label->on_event([this](egt::Event&)
            {
                m_width = std::stoi(reinterpret_cast<egt::Label*>(m_grid.get(m_grid.selected()))->text());
                this->hide();
            }, {egt::EventId::pointer_click});
        }
    }

    int width() const { return m_width; }

protected:
    egt::SelectableGrid m_grid;
    int m_width{2};
};

class MainWindow : public egt::TopWindow
{
public:

    MainWindow()
        : m_colorbtn(egt::Image("file:palette.png")),
          m_fillbutton(egt::Image("file:fill.png")),
          m_widthbtn(egt::Image("file:width.png")),
          m_clearbtn(egt::Image("file:clear.png")),
          m_snapshotbtn(egt::Image("file:screenshot.png")),
          m_penpicker(egt::Palette::blue),
          m_fillpicker(egt::Palette::red),
          m_widthpicker(2),
          m_canvas(screen()->size(), egt::PixelFormat::argb8888)
    {
        // don't draw background, we'll do it in draw()
        fill_flags().clear();
        color(egt::Palette::ColorId::bg, egt::Palette::white);

        m_sizer = std::make_unique<egt::VerticalBoxSizer>(*this);
        top(left(*m_sizer));

        m_colorbtn.auto_scale_image(false);
        m_fillbutton.auto_scale_image(false);
        m_widthbtn.auto_scale_image(false);
        m_clearbtn.auto_scale_image(false);
        m_snapshotbtn.auto_scale_image(false);

        m_colorbtn.fill_flags().clear();
        m_fillbutton.fill_flags().clear();
        m_widthbtn.fill_flags().clear();
        m_clearbtn.fill_flags().clear();
        m_snapshotbtn.fill_flags().clear();

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

        auto logo = std::make_shared<egt::ImageLabel>(egt::Image("icon:mgs_logo_black.png;128"));
        logo->align(egt::AlignFlag::right | egt::AlignFlag::top);
        logo->margin(10);
        add(logo);

        clear();
    }

    void clear()
    {
        egt::Painter painter(m_canvas.context());
        painter.alpha_blending(false);
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
            const auto mouse = display_to_local(event.pointer().point);
            egt::Painter painter(m_canvas.context());
            painter.antialias(egt::Painter::AntiAlias::none);
            painter.flood(mouse, m_fillpicker.selected_color());
            damage();
            break;
        }
        case egt::EventId::pointer_drag_start:
            m_last = display_to_local(event.pointer().point);
            event.grab(this);
            break;
        case egt::EventId::pointer_drag:
        {
            const auto mouse = display_to_local(event.pointer().point);

            if (m_last != mouse)
            {
                const auto width = m_widthpicker.width();

                egt::Line line(m_last, mouse);
                egt::Painter painter(m_canvas.context());
                painter.antialias(egt::Painter::AntiAlias::none);
                painter.line_width(width);
                painter.line_cap(egt::Painter::LineCap::round);
                painter.set(m_penpicker.selected_color());
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
        painter.set(color(egt::Palette::ColorId::bg));
        painter.draw(rect);
        painter.fill();

        painter.draw(m_canvas.surface(), {}, rect);

        egt::TopWindow::draw(painter, rect);
    }

protected:

    bool internal_drag() const override { return true; }

    egt::Point m_last;
    std::unique_ptr<egt::VerticalBoxSizer> m_sizer;
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

static int run(int argc, char** argv)
{
    egt::Application app(argc, argv, "whiteboard");
#ifdef EXAMPLEDATA
    egt::add_search_path(EXAMPLEDATA);
#endif

    MainWindow win;
    win.show_cursor(egt::Image("icon:cursor_pencil.png;16"));
    win.show();

    return app.run();
}

int main(int argc, char** argv)
{
    auto res = run(argc, argv);

    // cleanup any font allocations
    egt::Font::shutdown_fonts();

    return res;
}
