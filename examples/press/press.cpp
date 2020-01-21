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

class MainWindow : public TopWindow
{
public:

    MainWindow()
        : m_clearbtn(Image("@warning.png")),
          m_canvas(screen()->size(), PixelFormat::argb8888)
    {
        // don't draw background, we'll do it in draw()
        boxtype().clear();
        color(Palette::ColorId::bg, Palette::white);

        auto logo = make_shared<ImageLabel>(Image("@128px/egt_logo_black.png"));
        logo->align(AlignFlag::left | AlignFlag::top);
        logo->margin(10);
        add(logo);

        clear();

        m_clearbtn.boxtype().clear();
        m_clearbtn.min_size_hint(Size());
        m_clearbtn.align(AlignFlag::bottom | AlignFlag::right);
        add(m_clearbtn);

        m_clearbtn.on_click([this](Event&)
        {
            clear();
            damage();
        });
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

        static const auto dim = 30;

        switch (event.id())
        {
        case EventId::raw_pointer_down:
        {
            auto mouse = display_to_local(event.pointer().point);
            Painter painter(m_canvas.context());
            painter.set(Palette::red);
            painter.draw(Line(Point(mouse.x() - dim, mouse.y()), Point(mouse.x() + dim, mouse.y())));
            painter.draw(Line(Point(mouse.x(), mouse.y() - dim), Point(mouse.x(), mouse.y() + dim)));
            painter.stroke();
            damage();
            break;
        }
        case EventId::raw_pointer_up:
        {
            auto mouse = display_to_local(event.pointer().point);
            Painter painter(m_canvas.context());
            painter.set(Palette::blue);
            painter.draw(Line(Point(mouse.x() - dim, mouse.y()), Point(mouse.x() + dim, mouse.y())));
            painter.draw(Line(Point(mouse.x(), mouse.y() - dim), Point(mouse.x(), mouse.y() + dim)));
            painter.stroke();
            damage();
            break;
        }
        case EventId::pointer_click:
        {
            auto mouse = display_to_local(event.pointer().point);
            Painter painter(m_canvas.context());
            painter.set(Palette::green);
            Circle circle(mouse, dim);
            painter.draw(circle);
            painter.stroke();
            damage();
            break;
        }
        case EventId::pointer_dblclick:
        {
            auto mouse = display_to_local(event.pointer().point);
            Painter painter(m_canvas.context());
            painter.set(Palette::orange);
            Circle circle(mouse, dim);
            painter.draw(circle);
            painter.fill();
            damage();
            break;
        }
        case EventId::pointer_drag:
        {
            auto mouse = display_to_local(event.pointer().point);
            Painter painter(m_canvas.context());
            painter.set(Palette::black);
            Rect rect(Size(2, 2));
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

    void draw(Painter& painter, const Rect& rect) override
    {
        painter.set(color(Palette::ColorId::bg).color());
        painter.draw(rect);
        painter.fill();

        painter.draw(rect.point());
        painter.draw(rect, Image(m_canvas.surface()));

        TopWindow::draw(painter, rect);
    }

    ImageButton m_clearbtn;
    Canvas m_canvas;
};

static int run(int argc, const char** argv)
{
    Application app(argc, argv, "press");

    MainWindow win;
    win.show();

    return app.run();
}

int main(int argc, const char** argv)
{
    auto res = run(argc, argv);

    // cleanup any font allocations
    Font::shutdown_fonts();

    return res;
}
