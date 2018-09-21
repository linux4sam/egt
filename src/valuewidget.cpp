/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "valuewidget.h"

using namespace std;

namespace mui
{

    Radial::Radial(const Point& point, const Size& size)
        : ValueWidget<float>(point, size, -1.0)
    {}

    static int norm(float angle)
    {
        int a = (int)(angle / (M_PI / 180.0));
        if (a > 180)
            a -= 180;
        else
            a += 180;
        if (a == 360)
            a = 0;
        return a;
    }

    int Radial::handle(int event)
    {
        switch (event)
        {
        case EVT_MOUSE_DOWN:
            active(true);
            return 1;
            break;
        case EVT_MOUSE_UP:
            active(false);
            return 1;
            break;
        case EVT_MOUSE_MOVE:
            if (active())
            {
                Point c = center();
                float angle = atan2f(screen_to_window(mouse_position()).y - c.y,
                                     screen_to_window(mouse_position()).x - c.x);
                angle = angle * (180.0 / M_PI);
                angle = (angle > 0.0 ? angle : (360.0 + angle));
                angle = 180 - angle;

                value(angle);
                return 1;
            }
            break;
        }

        return Widget::handle(event);
    }

    static float radians(float zero, float degrees)
    {
        degrees = 360 - degrees - zero;
        return degrees * (M_PI / 180.0);
    }

    void Radial::draw(const Rect& rect)
    {
        float linew = 40;

        Color color1(Color::LIGHTGRAY);
        color1.alpha(0x55);

        float radius = w() / 2 - (linew / 2);
        double angle1 = radians(180, 0);
        double angle2 = radians(180, value());

        int a = norm(angle2);

        Color color2(Color::OLIVE);
        if (a > 180)
            color2 = Color::ORANGE;

        Point c = center();

        auto cr = screen()->context();

        cairo_save(cr.get());

        // bottom full circle
        cairo_set_source_rgba(cr.get(),
                              color1.redf(),
                              color1.greenf(),
                              color1.bluef(),
                              color1.alphaf());
        cairo_set_line_width(cr.get(), linew);

        cairo_arc(cr.get(), c.x, c.y, radius, 0, 2 * M_PI);
        cairo_stroke(cr.get());

        // top position arc
        cairo_set_source_rgb(cr.get(),
                             color2.redf(),
                             color2.greenf(),
                             color2.bluef());
        cairo_set_line_width(cr.get(), linew - (linew / 3));
        cairo_arc(cr.get(), c.x, c.y, radius, angle1, angle2);
        cairo_stroke(cr.get());

        string text = std::to_string(a) + m_label;
        draw_text(text, box(), color2, ALIGN_CENTER, 0, Font(72));

        cairo_restore(cr.get());
    }

    ProgressBar::ProgressBar(const Point& point, const Size& size)
        : ValueRangeWidget<int>(point, size, 0, 100, 0)
    {
    }

    void ProgressBar::draw(const Rect& rect)
    {
        auto cr = screen()->context();

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

    LevelMeter::LevelMeter(const Point& point, const Size& size)
        : ValueRangeWidget<int>(point, size, 0, 100, 0)
    {
    }

    void LevelMeter::draw(const Rect& rect)
    {
        auto cr = screen()->context();

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

    AnalogMeter::AnalogMeter(const Point& point, const Size& size)
        : ValueRangeWidget<int>(point, size, 0, 100, 0)
    {
    }

    void AnalogMeter::draw(const Rect& rect)
    {
        auto cr = screen()->context();

        cairo_save(cr.get());

        cairo_set_source_rgb(cr.get(), 0, 0, 0);
        cairo_set_line_width(cr.get(), 1.0);

        // Set origin as middle of bottom edge of the drawing area.
        // Window is not resizable so "magic numbers" will work here.
        cairo_translate(cr.get(), x() + w() / 2, y() + h());

        float hw = w() / 2.0 - 40.0;

        // Draw the black radial scale marks and labels
        for (double marks = 0.0; marks <= 100.0; marks += 10.0)
        {
            // Draw the radial marks
            cairo_move_to(cr.get(),
                          hw * cos(M_PI * marks * 0.01),
                          -hw * sin(M_PI * marks * 0.01));
            cairo_line_to(cr.get(),
                          (hw + 10.0) * cos(M_PI * marks * 0.01),
                          -(hw + 10.0) * sin(M_PI * marks * 0.01));

            // Set the text to print
            char text[10];
            sprintf(text, "%2.0f", marks);

            cairo_text_extents_t textext;
            cairo_select_font_face(cr.get(), "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
            cairo_set_font_size(cr.get(), 16);
            cairo_text_extents(cr.get(), text, &textext);

            int width = textext.width;
            int height = textext.height;
            // Position the text at the end of the radial marks
            cairo_move_to(cr.get(),
                          (-(hw + 30.0) * cos(M_PI * marks * 0.01)) - ((double)width / 2.0),
                          (-(hw + 30.0) * sin(M_PI * marks * 0.01)) + ((double)height / 2.0));

            cairo_show_text(cr.get(), text);
        }
        cairo_stroke(cr.get());

        // Retrieve the new slider value
        float value = this->value();

        cairo_set_source_rgb(cr.get(), 1.0, 0, 0);
        cairo_set_line_width(cr.get(), 1.5);

        // Draw the meter pointer
        cairo_move_to(cr.get(), 0.0, 0.0);
        cairo_line_to(cr.get(),
                      -hw * cos(M_PI * value * 0.01),
                      -hw * sin(M_PI * value * 0.01));
        cairo_stroke(cr.get());

        cairo_restore(cr.get());
    }

    SpinProgress::SpinProgress(const Point& point, const Size& size)
        : ValueRangeWidget<int>(point, size, 0, 100, 0)
    {
    }

    void SpinProgress::draw(const Rect& rect)
    {
        float linew = 5;

        Color color2(Color::ORANGE);

        float radius = w() / 2 - (linew / 2);
        double angle1 = radians(180, 0);
        double angle2 = radians(180, value() / 100. * 360.);

        Point c = center();

        auto cr = screen()->context();

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

}
