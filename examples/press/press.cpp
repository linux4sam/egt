/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/painter.h>
#include <egt/ui>

class MainWindow : public egt::TopWindow
{
public:

    MainWindow()
        : m_clearbtn(egt::Image("icon:warning.png")),
          m_canvas(screen()->size(), egt::PixelFormat::argb8888)
    {
        // don't draw background, we'll do it in draw()
        fill_flags().clear();
        color(egt::Palette::ColorId::bg, egt::Palette::white);

        auto logo = std::make_shared<egt::ImageLabel>(egt::Image("icon:egt_logo_black.png;128"));
        logo->align(egt::AlignFlag::left | egt::AlignFlag::top);
        logo->margin(10);
        add(logo);

        clear();

        m_clearbtn.fill_flags().clear();
        m_clearbtn.min_size_hint(egt::Size());
        m_clearbtn.align(egt::AlignFlag::bottom | egt::AlignFlag::right);
        add(m_clearbtn);

        m_clearbtn.on_click([this](egt::Event&)
        {
            clear();
            damage();
        });
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

        static const auto dim = 30;

        switch (event.id())
        {
        case egt::EventId::raw_pointer_down:
        {
            auto mouse = display_to_local(event.pointer().point);
            egt::Painter painter(m_canvas.context());
            painter.set(egt::Palette::red);
            painter.draw(egt::Line(egt::Point(mouse.x() - dim, mouse.y()),
                                   egt::Point(mouse.x() + dim, mouse.y())));
            painter.draw(egt::Line(egt::Point(mouse.x(), mouse.y() - dim),
                                   egt::Point(mouse.x(), mouse.y() + dim)));
            painter.stroke();
            damage();
            break;
        }
        case egt::EventId::raw_pointer_up:
        {
            auto mouse = display_to_local(event.pointer().point);
            egt::Painter painter(m_canvas.context());
            painter.set(egt::Palette::blue);
            painter.draw(egt::Line(egt::Point(mouse.x() - dim, mouse.y()),
                                   egt::Point(mouse.x() + dim, mouse.y())));
            painter.draw(egt::Line(egt::Point(mouse.x(), mouse.y() - dim),
                                   egt::Point(mouse.x(), mouse.y() + dim)));
            painter.stroke();
            damage();
            break;
        }
        case egt::EventId::pointer_click:
        {
            auto mouse = display_to_local(event.pointer().point);
            egt::Painter painter(m_canvas.context());
            painter.set(egt::Palette::green);
            egt::Circle circle(mouse, dim);
            painter.draw(circle);
            painter.stroke();
            damage();
            break;
        }
        case egt::EventId::pointer_dblclick:
        {
            auto mouse = display_to_local(event.pointer().point);
            egt::Painter painter(m_canvas.context());
            painter.set(egt::Palette::orange);
            egt::Circle circle(mouse, dim);
            painter.draw(circle);
            painter.fill();
            damage();
            break;
        }
        case egt::EventId::pointer_drag:
        {
            auto mouse = display_to_local(event.pointer().point);
            egt::Painter painter(m_canvas.context());
            painter.set(egt::Palette::black);
            egt::Rect rect(egt::Size(2, 2));
            rect.move_to_center(mouse);
            painter.draw(rect);
            painter.fill();
            damage();
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

        painter.draw(rect.point());
        painter.draw(rect, egt::Image(m_canvas.surface()));

        egt::TopWindow::draw(painter, rect);
    }

    egt::ImageButton m_clearbtn;
    egt::Canvas m_canvas;
};

static int run(int argc, char** argv)
{
    egt::Application app(argc, argv);

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
