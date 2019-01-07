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

    ProgressBar::ProgressBar(const Rect& rect,
                             int min, int max, int value) noexcept
        : ValueRangeWidget<int>(rect, min, max, value)
    {
        set_boxtype(Theme::boxtype::rounded_borderfill);
    }

    void ProgressBar::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        draw_box(painter);

        /// @todo this divide by 100 is not respecting min/max range
        theme().draw_rounded_gradient_box(painter,
                                           Rect(x(), y(), w() * value() / 100., h()),
                                          Color::TRANSPARENT,
                                          palette().color(Palette::HIGHLIGHT));

        string text = std::to_string(value()) + "%";
        m_dynamic_font = TextWidget::scale_font(box().size() * 0.75, text, m_dynamic_font);
        painter.draw_text(text, this->box(), this->palette().color(Palette::TEXT),
                          alignmask::CENTER, 0, m_dynamic_font);
    }

    LevelMeter::LevelMeter(const Rect& rect,
                           int min, int max, int value) noexcept
        : ValueRangeWidget<int>(rect, min, max, value)
    {
    }

    void LevelMeter::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        int limit = 20 - value() / 5;
        int barheight = h() / 20;

        for (int i = 0; i < 20; i++)
        {
            if (i > limit)
                painter.set_color(palette().color(Palette::HIGHLIGHT));
            else
                painter.set_color(palette().color(Palette::DARK));

            painter.draw_fill(Rect(x(),  y() + i * barheight, w(), barheight - 2));
        }
    }

    AnalogMeter::AnalogMeter(const Rect& rect)
        : ValueRangeWidget<int>(rect, 0, 100, 0)
    {
    }

    void AnalogMeter::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        auto cr = painter.context();

        painter.set_line_width(1.0);
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
        painter.set_color(Color::RED);
        painter.set_line_width(1.5);

        cairo_move_to(cr.get(), 0.0, 0.0);
        cairo_line_to(cr.get(),
                      -hw * std::cos(M_PI * value * 0.01),
                      -hw * std::sin(M_PI * value * 0.01));
        cairo_stroke(cr.get());
    }

    SpinProgress::SpinProgress(const Rect& rect, int min, int max, int value) noexcept
        : ValueRangeWidget<int>(rect, min, max, value)
    {
    }

    void SpinProgress::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        auto dim = std::min(w(), h());
        float linew = dim / 10;
        float radius = dim / 2 - (linew / 2);
        float angle1 = to_radians<float>(180, 0);
        float angle2 = to_radians<float>(180, value() / 100. * 360.);

        painter.set_color(palette().color(Palette::BG));
        painter.draw_fill(box());
        painter.set_line_width(linew);

        painter.set_color(palette().color(Palette::MID));
        painter.circle(Circle(center(), radius));
        painter.stroke();

        painter.set_color(palette().color(Palette::HIGHLIGHT));
        painter.arc(Arc(center(), radius, angle1, angle2));
        painter.stroke();

        string text = std::to_string(value());
        m_dynamic_font = TextWidget::scale_font(Size(dim,dim) * 0.75, text, m_dynamic_font);
        painter.draw_text(text, this->box(), this->palette().color(Palette::TEXT),
                          alignmask::CENTER, 0, m_dynamic_font);
    }

    Slider::Slider(const Rect& rect, int min, int max, int value,
                   orientation orient) noexcept
        : ValueRangeWidget<int>(rect, min, max, value),
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
        auto ret = Widget::handle(event);

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
                mouse_grab(this);
                return 1;
            }

            break;
        }
        case eventid::MOUSE_UP:
            set_active(false);
            mouse_grab(nullptr);
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

        return ret;
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
            theme().draw_rounded_gradient_box(painter, handle_box(),
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
            theme().draw_rounded_gradient_box(painter, handle_box(),
                                              palette().color(Palette::BORDER),
                                              palette().color(Palette::HIGHLIGHT));
        }
    }

    Slider::~Slider()
    {
    }
}
