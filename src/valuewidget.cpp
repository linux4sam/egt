/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/valuewidget.h"
#include "egt/painter.h"

using namespace std;

namespace egt
{

    ProgressBar::ProgressBar(const Rect& rect)
        : ValueRangeWidget<int>(rect, 0, 100, 0)
    {
    }

    void ProgressBar::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        auto cr = painter.context();

        cairo_save(cr.get());

        Color bg = palette().color(Palette::BG);
        cairo_set_source_rgba(cr.get(),
                              bg.redf(),
                              bg.greenf(),
                              bg.bluef(),
                              bg.alphaf());
        cairo_rectangle(cr.get(), x(), y(), w(), h());
        cairo_fill(cr.get());

        Color color = palette().color(Palette::HIGHLIGHT);
        cairo_set_source_rgb(cr.get(), color.redf(), color.greenf(), color.bluef());
        cairo_rectangle(cr.get(), x(),  y(), w() * value() / 100., h());
        cairo_fill(cr.get());

        cairo_rectangle(cr.get(), x(), y(), w(), h());
        Color border = palette().color(Palette::BORDER);
        cairo_set_source_rgba(cr.get(),
                              border.redf(),
                              border.greenf(),
                              border.bluef(),
                              border.alphaf());
        cairo_set_line_width(cr.get(), 1.0);
        cairo_stroke(cr.get());

        cairo_restore(cr.get());
    }

    LevelMeter::LevelMeter(const Rect& rect)
        : ValueRangeWidget<int>(rect, 0, 100, 0)
    {
    }

    void LevelMeter::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        auto cr = painter.context();

        cairo_save(cr.get());

        int limit = 20 - value() / 5;
        int barheight = h() / 20;

        for (int i = 0; i < 20; i++)
        {
            if (i > limit)
                cairo_set_source_rgb(cr.get(), 0.6, 1.0, 0);
            else
                cairo_set_source_rgb(cr.get(), 0.2, 0.4, 0);

            cairo_rectangle(cr.get(), x(),  y() + i * barheight, w(), barheight - 2);
            cairo_fill(cr.get());
        }

        cairo_restore(cr.get());
    }

    AnalogMeter::AnalogMeter(const Rect& rect)
        : ValueRangeWidget<int>(rect, 0, 100, 0)
    {
    }

    void AnalogMeter::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        auto cr = painter.context();

        cairo_set_line_width(cr.get(), 1.0);

        painter.set_font(m_font);

        cairo_text_extents_t textext;
        cairo_text_extents(cr.get(), "999", &textext);

        cairo_translate(cr.get(), x() + w() / 2, y() + h() - textext.height);

        float hw = w() / 2.0 - (textext.width * 2);

        // scale marks and labels
        for (double marks = 0.0; marks <= 100.0; marks += 10.0)
        {
            painter.set_color(palette().color(Palette::HIGHLIGHT));

            cairo_move_to(cr.get(),
                          hw * std::cos(M_PI * marks * 0.01),
                          -hw * std::sin(M_PI * marks * 0.01));
            cairo_line_to(cr.get(),
                          (hw + 10.0) * std::cos(M_PI * marks * 0.01),
                          -(hw + 10.0) * std::sin(M_PI * marks * 0.01));

            cairo_stroke(cr.get());

            painter.set_color(palette().color(Palette::TEXT));

            char text[10];
            sprintf(text, "%2.0f", marks);

            cairo_text_extents(cr.get(), text, &textext);

            int width = textext.width;
            int height = textext.height;
            cairo_move_to(cr.get(),
                          (-(hw + 30.0) * std::cos(M_PI * marks * 0.01)) - ((double)width / 2.0),
                          (-(hw + 30.0) * std::sin(M_PI * marks * 0.01)) + ((double)height / 2.0));

            cairo_show_text(cr.get(), text);
            cairo_stroke(cr.get());
        }

        float value = this->value();
        cairo_set_source_rgb(cr.get(), Color::RED.redf(),
                             Color::RED.greenf(), Color::RED.bluef());
        cairo_set_line_width(cr.get(), 1.5);

        cairo_move_to(cr.get(), 0.0, 0.0);
        cairo_line_to(cr.get(),
                      -hw * std::cos(M_PI * value * 0.01),
                      -hw * std::sin(M_PI * value * 0.01));
        cairo_stroke(cr.get());
    }

    SpinProgress::SpinProgress(const Rect& rect)
        : ValueRangeWidget<int>(rect, 0, 100, 0)
    {
    }

    void SpinProgress::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        float linew = 5;

        Color color2(Color::ORANGE);

        float radius = w() / 2 - (linew / 2);
        double angle1 = to_radians<float>(180, 0);
        double angle2 = to_radians<float>(180, value() / 100. * 360.);

        Point c = center();

        auto cr = painter.context();

        cairo_save(cr.get());

        cairo_set_source_rgba(cr.get(), 0, 0, 0, 0);
        cairo_rectangle(cr.get(), x(),  y(), w(), h());
        cairo_fill(cr.get());

        cairo_set_source_rgb(cr.get(),
                             color2.redf(),
                             color2.greenf(),
                             color2.bluef());
        cairo_set_line_width(cr.get(), linew - (linew / 3));
        cairo_arc(cr.get(), c.x, c.y, radius, angle1, angle2);
        cairo_stroke(cr.get());

        cairo_restore(cr.get());
    }

    Slider::Slider(int min, int max, const Rect& rect, orientation orient)
        : ValueRangeWidget<int>(rect, min, max, min),
          m_orientation(orient)
    {
    }

    int Slider::handle_dim() const
    {
        if (m_orientation == orientation::HORIZONTAL)
        {
            auto dim = w() / 6;
            if (dim > h())
                dim = h();
            return dim;
        }
        else
        {
            auto dim = h() / 6;
            if (dim > w())
                dim = w();
            return dim;
        }

        assert(0);

        return 0;
    }

    Rect Slider::handle_box() const
    {
        if (m_orientation == orientation::HORIZONTAL)
        {
            auto dim = handle_dim();
            return Rect(x() + normalize(m_value) + 1,
                        y() + h() / 2 - dim / 2 + 1,
                        dim - 2,
                        dim - 2);
        }
        else
        {
            auto dim = handle_dim();
            return Rect(x() + w() / 2 - dim / 2 + 1,
                        y() + normalize(m_value) + 1,
                        dim - 2,
                        dim - 2);
        }

        assert(0);

        return Rect();
    }

    int Slider::handle(eventid event)
    {
        switch (event)
        {
        case eventid::MOUSE_DOWN:
        {
            if (Rect::point_inside(from_screen(event_mouse()), handle_box() - box().point()))
            {
                if (m_orientation == orientation::HORIZONTAL)
                    m_moving_offset = from_screen(event_mouse()).x;
                else
                    m_moving_offset = from_screen(event_mouse()).y;
                m_start_pos = value();
                set_active(true);
                return 1;
            }

            break;
        }
        case eventid::MOUSE_UP:
            set_active(false);
            if (m_invoke_pending)
            {
                m_invoke_pending = false;
                this->invoke_handlers(eventid::PROPERTY_CHANGED);
            }
            return 1;
        case eventid::MOUSE_MOVE:
            if (active())
            {
                if (m_orientation == orientation::HORIZONTAL)
                {
                    auto diff = from_screen(event_mouse()).x - m_moving_offset;
                    set_value(m_start_pos + denormalize(diff));
                }
                else
                {
                    auto diff = from_screen(event_mouse()).y - m_moving_offset;
                    set_value(m_start_pos - denormalize(diff));
                }
                return 1;
            }
            break;
        default:
            break;
        }

        return Widget::handle(event);
    }

    void Slider::draw(Painter& painter, const Rect& rect)
    {
        DBG(name() << " " << __PRETTY_FUNCTION__ << " " << rect);

        auto dim = handle_dim();

        painter.set_color(palette().color(Palette::HIGHLIGHT));
        painter.set_line_width(dim / 5.0);

        if (m_orientation == orientation::HORIZONTAL)
        {
            // line
            painter.line(Point(x(), y() + h() / 2),
                         Point(x() + normalize(m_value), y() + h() / 2));
            painter.stroke();
            painter.set_color(palette().color(Palette::BORDER));
            painter.line(Point(x() + normalize(m_value) + 1, y() + h() / 2),
                         Point(x() + w(), y() + h() / 2));
            painter.stroke();

            // handle
            painter.draw_rounded_gradient_box(handle_box(),
                                              palette().color(Palette::BORDER),
                                              palette().color(Palette::HIGHLIGHT));
        }
        else
        {
            // line
            painter.line(Point(x() + w() / 2, y() + h()),
                         Point(x() + w() / 2, y() + normalize(m_value)));
            painter.stroke();
            painter.set_color(palette().color(Palette::BORDER));
            painter.line(Point(x() + w() / 2, y() + normalize(m_value) + 1),
                         Point(x() + w() / 2, y()));
            painter.stroke();

            // handle
            painter.draw_rounded_gradient_box(handle_box(),
                                              palette().color(Palette::BORDER),
                                              palette().color(Palette::HIGHLIGHT));
        }
    }

    Slider::~Slider()
    {
    }
}
