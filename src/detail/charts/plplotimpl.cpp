/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/charts/plplotimpl.h"
#include "egt/app.h"
#include "egt/canvas.h"
#include "egt/screen.h"
#include <cmath>
#include <spdlog/spdlog.h>
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

    SPDLOG_DEBUG("xmin {} xmax {} ymin {} ymax {}",
                 m_xmin, m_xmax, m_ymin, m_ymax);
}

void PlPlotImpl::plplot_font(const Font& font)
{
    const std::string& face = font.face();
    SPDLOG_TRACE("font face = {}", face);
    PLINT font_family = 0;
    if (face.find("Sans") != std::string::npos)
    {
        font_family = 0;
    }
    else if (face.find("Serif") != std::string::npos)
    {
        font_family = 1;
    }
    else if (face.find("Mono") != std::string::npos)
    {
        font_family = 2;
    }
    else if (face.find("Script") != std::string::npos)
    {
        font_family = 3;
    }
    else if (face.find("Symbol") != std::string::npos)
    {
        font_family = 4;
    }

    auto slant = font.slant();
    PLINT font_style;
    if (slant == Font::Slant::normal)
        font_style = 0;
    else if (slant == Font::Slant::italic)
        font_style = 1;
    else
        font_style = 2;

    PLINT font_weight;
    if (font.weight() == Font::Weight::normal)
        font_weight = 0;
    else
        font_weight = 1;

    m_plstream->sfont(font_family, font_style, font_weight);
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
        m_plstream->spage(0, 0, b.width(), b.height(), 0, 0);
        m_plstream->init();
        plplot_font(m_interface.font());
        m_initalize = true;
    }
    auto cr = painter.context();

    if (b.x() || b.y())
        cairo_translate(cr.get(), b.x(), b.y());

    m_plstream->cmd(PLESC_DEVINIT, cr.get());

    painter.set(m_interface.font());

    plplot_color(m_interface.color(Palette::ColorId::button_bg).color());

    if (m_xdata.size() > 1 && m_ydata.size() > 1)
    {
        m_plstream->width(m_grid_width);

        // set env
        m_plstream->env(m_xmin, m_xmax, m_ymin, m_ymax, 0, axis());

        //set line style
        m_plstream->lsty(m_pattern <= 0 ? 1 : m_pattern);

        // set line width
        m_plstream->width(m_line_width);

        plplot_color(m_interface.color(Palette::ColorId::button_fg).color());

        // plot
        m_plstream->line(m_xdata.size(), m_xdata.data(), m_ydata.data());
    }
    else
    {
        m_plstream->width(m_grid_width);
        m_plstream->env(m_xmin, m_xmax, m_ymin, m_ymax, 0, axis());
    }

    // set text color
    plplot_color(m_interface.color(Palette::ColorId::label_text).color());

    //set text or label
    m_plstream->lab(m_xlabel.c_str(), m_ylabel.c_str(), m_title.c_str());
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
        m_plstream->spage(0, 0, b.width(), b.height(), 0, 0);
        m_plstream->init();
        plplot_font(m_interface.font());
        m_initalize = true;
    }

    auto cr = painter.context();

    if (b.x() || b.y())
        cairo_translate(cr.get(), b.x(), b.y());

    m_plstream->cmd(PLESC_DEVINIT, cr.get());

    //set axis color
    plplot_color(m_interface.color(Palette::ColorId::button_bg).color());

    if (!m_xdata.empty() && !m_ydata.empty())
    {
        m_plstream->width(m_grid_width);

        // set env
        m_plstream->env(m_xmin, m_xmax, m_ymin, m_ymax, 0, axis());

        plplot_color(m_interface.color(Palette::ColorId::button_fg).color());

        // draw points
        m_plstream->poin(m_xdata.size(), m_xdata.data(), m_ydata.data(), m_pointtype);
    }
    else
    {
        // set env
        m_plstream->env(0, 1, 0, 1, 0, axis());
    }

    // set label color
    plplot_color(m_interface.color(Palette::ColorId::label_text).color());

    // set label
    m_plstream->lab(m_xlabel.c_str(), m_ylabel.c_str(), m_title.c_str());
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
        m_plstream->spage(0, 0, b.width(), b.height(), 0, 0);
        m_plstream->init();
        plplot_font(m_interface.font());
        m_initalize = true;
    }

    auto cr = painter.context();

    if (b.x() || b.y())
        cairo_translate(cr.get(), b.x(), b.y());

    m_plstream->cmd(PLESC_DEVINIT, cr.get());

    m_plstream->adv(0);
    m_plstream->vsta();

    //set axis color.
    plplot_color(m_interface.color(Palette::ColorId::button_bg).color());

    // Draw axis
    if ((!m_ydata.empty()) && (!m_sdata.empty()))
    {
        std::size_t xmax = m_sdata.size() * 2;
        m_plstream->wind(0.0, xmax, 0.0, m_ymax);
        if (axis() == 2)
            m_plstream->box("bgit", 0, 0, "bginvst", 0, 0);
        else if (axis() == 0)
            m_plstream->box("bit", 0, 0, "binvst", 0, 0);
        else
            m_plstream->box("bi", 0, 0, "binvs", 0, 0);
    }
    else
    {
        m_plstream->width(m_grid_width);
        m_plstream->env(m_xmin, m_xmax, m_ymin, m_ymax, 0, axis());
    }

    if (!m_ydata.empty())
    {
        std::size_t n = m_ydata.size() - 1;
        for (std::size_t i = 0; i <= n; i++)
        {
            // set Bar color : using default colors from plplot
            m_plstream->col0(i % 16);

            // set Bar patterns : solid color or line patterns
            m_plstream->psty(m_pattern);

            if (!m_sdata.empty())
            {
                SPDLOG_TRACE("m_ydata[{}] = {} ", i, m_ydata.at(i));
                plfbox((i * 2), m_ydata.at(i));

                if (axis() != -1)
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

    // set labels color
    plplot_color(m_interface.color(Palette::ColorId::label_text).color());

    // set labels
    m_plstream->lab(m_xlabel.c_str(), m_ylabel.c_str(), m_title.c_str());
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
        m_plstream->spage(0, 0, b.width(), b.height(), 0, 0);
        m_plstream->init();
        plplot_font(m_interface.font());
        m_initalize = true;
    }

    auto cr = painter.context();

    if (b.x() || b.y())
        cairo_translate(cr.get(), b.x(), b.y());

    m_plstream->cmd(PLESC_DEVINIT, cr.get());

    m_plstream->adv(0);
    m_plstream->vsta();

    if ((!m_sdata.empty()) && (!m_ydata.empty()))
    {
        m_plstream->width(m_grid_width);

        //set axis color.
        plplot_color(m_interface.color(Palette::ColorId::button_bg).color());

        m_plstream->wind(0.0, m_ymax, 0.0, (m_sdata.size() * 2));
        if (axis() == 2)
            m_plstream->box("bginvst", 0, 0, "bgit", 0, 0);
        else if (axis() == 0)
            m_plstream->box("bit", 0, 0, "bitnvs", 0, 0);
        else
            m_plstream->box("bi", 0, 0, "binvs", 0, 0);

        std::size_t n = m_ydata.size();
        int i = 0;
        for (auto& data : m_ydata)
        {
            // set HBar color
            m_plstream->col0(i % 16);

            // Set HBar pattern
            m_plstream->psty(m_pattern);

            // Draw HBar
            plfHbox(data, (i * 2));

            //Set Y-axis text
            if (axis() != -1)
            {
                auto pos = static_cast<PLFLT>(i) / static_cast<PLFLT>(n);
                m_plstream->mtex("lv", 1.0, pos, 1, m_sdata.at(i).c_str());
            }
            ++i;
        }
    }
    else
    {
        m_plstream->width(m_grid_width);

        m_plstream->env(m_xmin, m_xmax, m_ymin, m_ymax, 0, axis());
    }

    // set labels color
    plplot_color(m_interface.color(Palette::ColorId::label_text).color());

    // set labels
    m_plstream->lab(m_xlabel.c_str(), m_ylabel.c_str(), m_title.c_str());
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
        m_plstream->spage(0, 0, b.width(), b.height(), 0, 0);
        m_plstream->init();
        plplot_font(m_interface.font());
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

    // set title
    m_plstream->lab("", "", m_title.c_str());

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
}

PlPlotPieChart::~PlPlotPieChart() = default;

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt
