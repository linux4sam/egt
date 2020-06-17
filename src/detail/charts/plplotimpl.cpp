/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/egtlog.h"
#include "detail/charts/plplotimpl.h"
#include "egt/app.h"
#include "egt/canvas.h"
#include "egt/screen.h"
#include <cmath>
#include <type_traits>

namespace egt
{
inline namespace v1
{
namespace detail
{

PlPlotImpl::PlPlotImpl()
    : m_plstream(std::make_unique<plstream>())
{
    m_plstream->sdev("extcairo");
    plplot_verify_viewport();
}

template<class T1, class T2, class T3>
inline bool change_if_diff(T1& first, T2& second, const T3& to)
{
    if (to.size() != first.size() || to.size() != second.size())
    {
        first.clear();
        second.clear();

        for (const auto& elem : to)
        {
            first.push_back(elem.first);
            second.push_back(elem.second);
        }

        return true;
    }

    bool diff = false;
    auto f = first.begin();
    auto s = second.begin();
    for (const auto& elem : to)
    {
        if (detail::change_if_diff<typename std::remove_reference<decltype(*f)>::type>(*f, elem.first))
            diff = true;

        if (detail::change_if_diff<typename std::remove_reference<decltype(*s)>::type>(*s, elem.second))
            diff = true;

        ++s;
        ++f;
    }

    return diff;
}

void PlPlotImpl::data(const ChartBase::DataArray& data)
{
    if (change_if_diff(m_xdata, m_ydata, data))
    {
        m_sdata.clear();

        plplot_verify_viewport();

        invoke_damage();
    }
}

void PlPlotImpl::add_data(const ChartBase::DataArray& data)
{
    if (!data.empty())
    {
        /**
         * clear m_sdata both m_sdata and m_xdata cannot
         * exist together
         */
        m_sdata.clear();

        for (auto& elem : data)
        {
            m_xdata.push_back(elem.first);
            m_ydata.push_back(elem.second);
        }

        plplot_verify_viewport();

        invoke_damage();
    }
}

void PlPlotImpl::data(const ChartBase::StringDataArray& data)
{
    if (change_if_diff(m_ydata, m_sdata, data))
    {
        m_xdata.clear();

        plplot_verify_viewport();

        invoke_damage();
    }
}

size_t PlPlotImpl::data_size() const
{
    if (!m_xdata.empty())
        return m_xdata.size();

    if (!m_sdata.empty())
        return m_sdata.size();

    return 0;
}

void PlPlotImpl::add_data(const ChartBase::StringDataArray& data)
{
    if (!data.empty())
    {
        /**
         * clear m_xdata both m_sdata and m_xdata cannot
         * exist together
         */
        m_xdata.clear();

        for (auto& elem : data)
        {
            m_sdata.push_back(elem.second);
            m_ydata.push_back(elem.first);
        }

        plplot_verify_viewport();

        invoke_damage();
    }
}

void PlPlotImpl::remove_data(uint32_t count)
{
    if (!count)
        return;

    bool damage = false;

    if (!m_sdata.empty())
    {
        damage = true;
        if (count <= m_sdata.size())
            m_sdata.erase(m_sdata.begin(), m_sdata.begin() + count);
        else
            m_sdata.clear();
    }

    if (!m_xdata.empty())
    {
        damage = true;
        if (count <= m_xdata.size())
            m_xdata.erase(m_xdata.begin(), m_xdata.begin() + count);
        else
            m_xdata.clear();
    }

    if (!m_ydata.empty())
    {
        damage = true;
        if (count <= m_ydata.size())
            m_ydata.erase(m_ydata.begin(), m_ydata.begin() + count);
        else
            m_ydata.clear();
    }

    if (damage)
    {
        plplot_verify_viewport();
        invoke_damage();
    }
}

void PlPlotImpl::clear()
{
    if (!m_ydata.empty() ||
        !m_xdata.empty() ||
        !m_sdata.empty())
    {
        m_xdata.clear();
        m_sdata.clear();
        m_ydata.clear();
        invoke_damage();
    }
}

void PlPlotImpl::title(const std::string& title)
{
    if (detail::change_if_diff<>(m_title, title))
        invoke_damage();
}

void PlPlotImpl::label(const std::string& xlabel, const std::string& ylabel, const std::string& title)
{
    bool damage_flag = false;

    if (detail::change_if_diff<>(m_xlabel, xlabel))
    {
        damage_flag = true;
    }
    if (detail::change_if_diff<>(m_ylabel, ylabel))
    {
        damage_flag = true;
    }
    if (detail::change_if_diff<>(m_title, title))
    {
        damage_flag = true;
    }

    if (damage_flag)
        invoke_damage();
}

void PlPlotImpl::grid_style(ChartBase::GridFlag flag)
{
    if (detail::change_if_diff<>(m_grid, flag))
        invoke_damage();
}

void PlPlotImpl::grid_width(int val)
{
    m_grid_width = val;
}

void PlPlotImpl::line_width(int val)
{
    if (detail::change_if_diff<>(m_line_width, val))
        invoke_damage();
}

void PlPlotImpl::line_style(int val)
{
    if (detail::change_if_diff<>(m_pattern, val))
        invoke_damage();
}

void PlPlotImpl::point_type(int ptype)
{
    if (detail::change_if_diff<>(m_pointtype, ptype))
        invoke_damage();
}

/**
 * Plplot reports error while creating a viewport
 * if x_min & x_max and y_min & y_max values are
 * same. as workaround modifying these value's.
 */
void PlPlotImpl::plplot_verify_viewport()
{
    if (!m_xdata.empty())
    {
        m_xmin = std::round(*std::min_element(m_xdata.begin(), m_xdata.end()));
        m_xmax = std::round(*std::max_element(m_xdata.begin(), m_xdata.end()));
    }

    if (!m_ydata.empty())
    {
        m_ymin = std::round(*std::min_element(m_ydata.begin(), m_ydata.end()));
        m_ymax = std::round(*std::max_element(m_ydata.begin(), m_ydata.end()));
    }

    if (!detail::float_equal(m_bank, 0.0f))
    {
        auto xdiff = (m_xmax - m_xmin) * m_bank;
        m_xmin -= xdiff;
        m_xmax += xdiff;

        auto ydiff = (m_ymax - m_ymin) * m_bank;
        m_ymin -= ydiff;
        m_ymax += ydiff;
    }

    if (detail::float_equal(m_xmin, m_xmax))
    {
        auto xdiff = m_xmax * 0.1;
        if (detail::float_equal(xdiff, 0.0))
            xdiff = 1;
        m_xmin -= xdiff;
        m_xmax += xdiff;
    }

    if (detail::float_equal(m_ymin, m_ymax))
    {
        auto ydiff = m_ymax * 0.1;
        if (detail::float_equal(ydiff, 0.0))
            ydiff = 1;
        m_ymin -= ydiff;
        m_ymax += ydiff;
    }

    EGTLOG_DEBUG("xmin {} xmax {} ymin {} ymax {}",
                 m_xmin, m_xmax, m_ymin, m_ymax);
}

void PlPlotImpl::plplot_color(const Color& color)
{
    auto r = static_cast<PLINT>(color.red());
    auto g = static_cast<PLINT>(color.green());
    auto b = static_cast<PLINT>(color.blue());
    auto a = static_cast<PLFLT>(color.alpha() / 255.0);
    m_plstream->scmap1n(1);
    m_plstream->scmap1a(&r, &g, &b, &a, 1);
    m_plstream->col1(1);
}

void PlPlotImpl::resize()
{
    m_plstream = std::make_unique<plstream>();
    m_initalize = false;
    m_plstream->sdev("extcairo");
}

void PlPlotImpl::bank(float bank)
{
    if (detail::change_if_diff<float>(m_bank, bank))
        invoke_damage();
}

void PlPlotImpl::plplot_box(bool xtick_label, bool ytick_label)
{
    PLINT val = axis();
    std::string xopt;
    std::string yopt;

    if (val == -1)
    {
        xopt = "bc";
        yopt = "bc";
    }
    else if (val == 0)
    {
        xopt = "bcist";
        yopt = "bcistv";
    }
    else if (val == 1)
    {
        xopt = "abcist";
        yopt = "abcistv";
    }
    else if (val == 2)
    {
        xopt = "abcgist";
        yopt = "abcgistv";
    }
    else if (val == 3)
    {
        xopt = "abcghist";
        yopt = "abcghistv";
    }

    if (xtick_label && (val >= 0))
        xopt += "n";

    if (ytick_label && (val >= 0))
        yopt += "n";

    m_plstream->box(xopt.c_str(), 0, 0, yopt.c_str(), 0, 0);
}

void PlPlotImpl::plplot_label(const shared_cairo_t& cr, Rect b, const Font& font, const Color& color)
{
    if (axis() >= 0)
    {
        // set font face, slant and weight
        auto slant = static_cast<cairo_font_slant_t>(font.slant());
        auto weight = static_cast<cairo_font_weight_t>(font.weight());
        cairo_select_font_face(cr.get(), font.face().c_str(), slant, weight);

        // set font size
        cairo_set_font_size(cr.get(), font.size());

        // set text color
        cairo_set_source_rgb(cr.get(), color.redf(), color.greenf(), color.bluef());

        // reverse mirroring the font
        cairo_matrix_t font_reflection_matrix;
        cairo_get_font_matrix(cr.get(), &font_reflection_matrix);
        font_reflection_matrix.yy = font_reflection_matrix.yy * -1;
        cairo_set_font_matrix(cr.get(), &font_reflection_matrix);

        cairo_text_extents_t te;
        double x;
        double y;
        double offset = b.width() * 0.5;
        if (!m_xlabel.empty())
        {
            cairo_text_extents(cr.get(), m_xlabel.c_str(), &te);
            x = (b.x() + offset) - (te.x_bearing + (te.width * 0.5));
            y = b.y();

            cairo_move_to(cr.get(), x, y);
            cairo_rotate(cr.get(), 0);
            cairo_show_text(cr.get(), m_xlabel.c_str());
        }

        if (!m_title.empty())
        {
            cairo_text_extents(cr.get(), m_title.c_str(), &te);
            x = (b.x() + offset) - (te.x_bearing + (te.width * 0.5));
            y =  b.y() + b.height() - font.size();

            cairo_move_to(cr.get(), x, y);
            cairo_rotate(cr.get(), 0);
            cairo_show_text(cr.get(), m_title.c_str());
        }

        if (!m_ylabel.empty())
        {
            cairo_text_extents(cr.get(), m_ylabel.c_str(), &te);
            x = b.x() + font.size();
            y = (b.y() + (b.height() * 0.5)) - (te.x_bearing + (te.width * 0.5));

            cairo_move_to(cr.get(), x, y);
            cairo_rotate(cr.get(), detail::pi<float>() * 2.5);
            cairo_show_text(cr.get(), m_ylabel.c_str());
        }
    }
}

void PlPlotImpl::plplot_viewport(PLFLT size)
{
    PLFLT xmin;
    PLFLT xmax;
    PLFLT ymin;
    PLFLT ymax;

    // get the size of the current subpage in millimeters.
    m_plstream->gspa(xmin, xmax, ymin, ymax);

    PLFLT def_ht;
    PLFLT scale_ht;

    // get character default height and current scaled height.
    m_plstream->gchr(def_ht, scale_ht);

    if (axis() < 0)
    {
        // leave a margin space on 4 sides of default character size.
        m_plstream->svpa(xmin + def_ht, xmax - def_ht, ymin + def_ht, ymax - def_ht);
    }
    else
    {
        // leave a margin space on 4 sides
        // for x-axis, y-axis and title, coordinate font size(i.e def_ht * 3) + label font size
        PLFLT ch_ht = ((size / 96.0) * 25.4);
        PLFLT x_margin = def_ht * 6;
        PLFLT ymin_margin = def_ht * 3;
        PLFLT ymax_margin = def_ht * 3;
        if (!m_ylabel.empty())
        {
            x_margin += ch_ht;
        }
        if (!m_xlabel.empty())
        {
            ymin_margin += ch_ht;
        }
        if (!m_title.empty())
        {
            ymax_margin += ch_ht;
        }
        // set viewport size in millimeters.
        m_plstream->svpa(xmin + x_margin, xmax - def_ht, ymin + ymin_margin, ymax - ymax_margin);
    }
}

PlPlotImpl::~PlPlotImpl() = default;

PlPlotLineChart::PlPlotLineChart(LineChart& interface)
    : m_interface(interface)
{
}

void PlPlotLineChart::draw(Painter& painter, const Rect& rect)
{
    detail::ignoreparam(rect);

    m_interface.draw_box(painter, Palette::ColorId::bg,
                         Palette::ColorId::border);

    auto b = m_interface.content_area();

    if (!m_initalize)
    {
        m_plstream->spage(0, 0, b.width(), b.height(),  b.x(), b.y());
        m_plstream->init();
        m_initalize = true;
    }
    auto cr = painter.context();

    if (b.x() || b.y())
        cairo_translate(cr.get(), b.x(), b.y());

    m_plstream->cmd(PLESC_DEVINIT, cr.get());

    m_plstream->adv(0);

    plplot_color(m_interface.color(Palette::ColorId::button_bg).color());

    m_plstream->width(m_grid_width);

    m_plstream->lsty(1);

    auto size = static_cast<PLFLT>(m_interface.font().size());

    plplot_viewport(size);

    m_plstream->wind(m_xmin, m_xmax, m_ymin, m_ymax);

    plplot_box(true, true);

    if (m_xdata.size() > 1 && m_ydata.size() > 1)
    {
        //set line style
        m_plstream->lsty(m_pattern <= 0 ? 1 : m_pattern);

        // set line width
        m_plstream->width(m_line_width);

        plplot_color(m_interface.color(Palette::ColorId::button_fg).color());

        // plot
        m_plstream->line(m_xdata.size(), m_xdata.data(), m_ydata.data());
    }

    plplot_label(cr, b, m_interface.font(), m_interface.color(Palette::ColorId::label_text).color());

}

PlPlotLineChart::~PlPlotLineChart() = default;

PlPlotPointChart::PlPlotPointChart(PointChart& interface)
    : m_interface(interface)
{
}

void PlPlotPointChart::draw(Painter& painter, const Rect& rect)
{
    detail::ignoreparam(rect);

    m_interface.draw_box(painter, Palette::ColorId::bg,
                         Palette::ColorId::border);

    auto b = m_interface.content_area();

    if (!m_initalize)
    {
        m_plstream->spage(0, 0, b.width(), b.height(),  b.x(), b.y());
        m_plstream->init();
        m_initalize = true;
    }

    auto cr = painter.context();

    if (b.x() || b.y())
        cairo_translate(cr.get(), b.x(), b.y());

    m_plstream->cmd(PLESC_DEVINIT, cr.get());

    m_plstream->adv(0);

    plplot_color(m_interface.color(Palette::ColorId::button_bg).color());

    m_plstream->width(m_grid_width);

    m_plstream->lsty(1);

    plplot_viewport(m_interface.font().size());

    m_plstream->wind(m_xmin, m_xmax, m_ymin, m_ymax);

    plplot_box(true, true);

    if (!m_xdata.empty() && !m_ydata.empty())
    {
        plplot_color(m_interface.color(Palette::ColorId::button_fg).color());

        // draw points
        m_plstream->poin(m_xdata.size(), m_xdata.data(), m_ydata.data(), m_pointtype);
    }

    plplot_label(cr, b, m_interface.font(), m_interface.color(Palette::ColorId::label_text).color());
}

PlPlotPointChart::~PlPlotPointChart() = default;

PlPlotBarChart::PlPlotBarChart(BarChart& interface)
    : m_interface(interface)
{
}

void PlPlotBarChart::plfbox(PLFLT x0, PLFLT y0)
{
    PLFLT x[4];
    PLFLT y[4];

    x[0] = x0;
    x[1] = x0;
    x[2] = x0 + 1.;
    x[3] = x0 + 1.;
    y[0] = 0.;
    y[1] = y0;
    y[2] = y0;
    y[3] = 0.;
    m_plstream->fill(4, x, y);
    m_plstream->width(m_line_width);
    plplot_color(m_interface.color(Palette::ColorId::button_bg).color());
    m_plstream->lsty(1);
    m_plstream->line(4, x, y);
}

void PlPlotBarChart::draw(Painter& painter, const Rect& rect)
{
    detail::ignoreparam(rect);

    m_interface.draw_box(painter, Palette::ColorId::bg,
                         Palette::ColorId::border);

    auto b = m_interface.content_area();

    if (!m_initalize)
    {
        m_plstream->spage(0, 0, b.width(), b.height(),  b.x(), b.y());
        m_plstream->init();
        m_initalize = true;
    }

    auto cr = painter.context();

    if (b.x() || b.y())
        cairo_translate(cr.get(), b.x(), b.y());

    m_plstream->cmd(PLESC_DEVINIT, cr.get());

    m_plstream->adv(0);

    //set axis color.
    plplot_color(m_interface.color(Palette::ColorId::button_bg).color());

    m_plstream->width(m_grid_width);

    plplot_viewport(m_interface.font().size());

    if ((!m_ydata.empty()) && (!m_sdata.empty()))
    {
        std::size_t xmax = m_sdata.size() * 2;
        m_plstream->wind(0.0, xmax, 0.0, m_ymax);

        plplot_box(false, true);
    }
    else
    {
        m_plstream->wind(m_xmin, m_xmax, m_ymin, m_ymax);
        plplot_box(true, true);
    }

    if (!m_ydata.empty())
    {
        std::size_t n = m_ydata.size();
        for (std::size_t i = 0; i < n; i++)
        {
            // set Bar color : using default colors from plplot
            m_plstream->col0(i % 16);

            // set Bar patterns : solid color or line patterns
            m_plstream->psty(m_pattern);

            if (!m_sdata.empty())
            {
                EGTLOG_TRACE("m_ydata[{}] = {} ", i, m_ydata.at(i));
                plfbox((i * 2), m_ydata.at(i));

                if (axis() >= 0)
                {
                    plplot_color(m_interface.color(Palette::ColorId::button_bg).color());
                    auto pos = static_cast<PLFLT>(i) / static_cast<PLFLT>(n);
                    m_plstream->mtex("b", 1.0, pos, 0, m_sdata.at(i).c_str());
                }
            }
            else if (!m_xdata.empty())
            {
                plfbox(m_xdata.at(i), m_ydata.at(i));
            }
        }
    }

    plplot_label(cr, b, m_interface.font(), m_interface.color(Palette::ColorId::label_text).color());
}

PlPlotBarChart::~PlPlotBarChart() = default;

PlPlotHBarChart::PlPlotHBarChart(HorizontalBarChart& interface)
    : m_interface(interface)
{
}

void PlPlotHBarChart::plfHbox(PLFLT x0, PLFLT y0)
{
    PLFLT x[4];
    PLFLT y[4];

    x[0] = 0;
    x[1] = x0;
    x[2] = x0;
    x[3] = 0;

    y[0] = y0;
    y[1] = y0;
    y[2] = y0 + 1.;
    y[3] = y0 + 1.;

    m_plstream->fill(4, x, y);
    m_plstream->width(m_line_width);
    plplot_color(m_interface.color(Palette::ColorId::button_bg).color());
    m_plstream->lsty(1);
    m_plstream->line(4, x, y);
}

void PlPlotHBarChart::draw(Painter& painter, const Rect& rect)
{
    detail::ignoreparam(rect);

    m_interface.draw_box(painter, Palette::ColorId::bg,
                         Palette::ColorId::border);

    auto b = m_interface.content_area();

    if (!m_initalize)
    {
        m_plstream->spage(0, 0, b.width(), b.height(),  b.x(), b.y());
        m_plstream->init();
        m_initalize = true;
    }

    auto cr = painter.context();

    if (b.x() || b.y())
        cairo_translate(cr.get(), b.x(), b.y());

    m_plstream->cmd(PLESC_DEVINIT, cr.get());

    m_plstream->adv(0);

    //set axis color.
    plplot_color(m_interface.color(Palette::ColorId::button_bg).color());

    m_plstream->width(m_grid_width);

    plplot_viewport(m_interface.font().size());

    if ((!m_sdata.empty()) && (!m_ydata.empty()))
    {
        m_plstream->wind(0.0, m_ymax, 0.0, (m_sdata.size() * 2));
        plplot_box(true, false);
    }
    else
    {
        m_plstream->wind(m_xmin, m_xmax, m_ymin, m_ymax);
        plplot_box(true, true);
    }

    std::size_t n = m_ydata.size();
    int i = 0;
    for (auto& data : m_ydata)
    {
        // set HBar color
        m_plstream->col0(i % 16);

        // Set HBar pattern
        m_plstream->psty(m_pattern);

        if (!m_sdata.empty())
        {
            // Draw HBar
            plfHbox(data, (i * 2));

            //Set Y-axis text
            if (axis() >= 0)
            {
                auto pos = static_cast<PLFLT>(i) / static_cast<PLFLT>(n);
                m_plstream->mtex("lv", 1.0, pos, 1, m_sdata.at(i).c_str());
            }
        }
        else
        {
            // Draw HBar
            plfHbox(m_xdata.at(i), data);
        }
        ++i;
    }

    plplot_label(cr, b, m_interface.font(), m_interface.color(Palette::ColorId::label_text).color());
}

PlPlotHBarChart::~PlPlotHBarChart() = default;

PlPlotPieChart::PlPlotPieChart(PieChart& interface)
    : m_interface(interface)
{
}

void PlPlotPieChart::draw(Painter& painter, const Rect& rect)
{
    detail::ignoreparam(rect);

    m_interface.draw_box(painter, Palette::ColorId::bg,
                         Palette::ColorId::border);

    auto b = m_interface.content_area();

    if (!m_initalize)
    {
        m_plstream->spage(0, 0, b.width(), b.height(),  b.x(), b.y());
        m_plstream->init();
        m_initalize = true;
    }

    auto cr = painter.context();

    if (b.x() || b.y())
        cairo_translate(cr.get(), b.x(), b.y());

    m_plstream->cmd(PLESC_DEVINIT, cr.get());

    m_plstream->adv(0);
    // Ensure window has aspect ratio of one so plotted as a circle.
    m_plstream->vasp(1.0);
    m_plstream->wind(0.1, 10.0, 0.1, 10.0);

    // get labels color & set title color
    plplot_color(m_interface.color(Palette::ColorId::label_text).color());

    if ((!m_ydata.empty()) && (!m_sdata.empty()))
    {
        int theta = 0;
        int theta0 = 0;
        int color = 1;
        int size = m_ydata.size();
        for (int i = 0; i < size; i++)
        {
            std::vector<PLFLT> x;
            std::vector<PLFLT> y;
            x.push_back(5.);
            y.push_back(5.);

            int theta1 = theta0 + 5 * m_ydata.at(i);
            if (i == size)
                theta1 = 500;

            for (theta = theta0; theta <= theta1; theta += 1)
            {
                x.push_back(5 + 3 * cos((2. * M_PI / 500.) * theta));
                y.push_back(5 + 3 * sin((2. * M_PI / 500.) * theta));
            }

            m_plstream->col0(color++ % 16);
            m_plstream->psty(m_pattern);
            m_plstream->fill(x.size(), x.data(), y.data());

            //Draw pie border
            m_plstream->width(m_line_width);
            // get line color
            plplot_color(m_interface.color(Palette::ColorId::button_bg).color());
            m_plstream->line(x.size(), x.data(), y.data());

            PLFLT just = (2. * M_PI / 500.) * (theta0 + theta1) / 2.;
            PLFLT dx = .25 * cos(just);
            PLFLT dy = .25 * sin(just);

            if ((theta0 + theta1) < 250 || (theta0 + theta1) > 750)
                just = 0.;
            else
                just = 1.;

            // set pie label color
            plplot_color(m_interface.color(Palette::ColorId::label_text).color());

            //set pie label
            m_plstream->ptex((x[x.size() / 2] + dx), (y[y.size() / 2] + dy), 1.0, 0.0, just, m_sdata.at(i).c_str());
            theta0 = theta - 1;
        }
    }

    plplot_label(cr, b, m_interface.font(), m_interface.color(Palette::ColorId::label_text).color());

}

PlPlotPieChart::~PlPlotPieChart() = default;

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt
