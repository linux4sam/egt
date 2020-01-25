/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/charts/plplotimpl.h"
#include "egt/screen.h"
#include <egt/app.h>
#include <math.h>
#include <spdlog/spdlog.h>
#include <sstream>

namespace egt
{
inline namespace v1
{
namespace detail
{

PlPlotImpl::PlPlotImpl(const Rect& rect)
    : m_plstream(detail::make_unique<plstream>())
{
    m_plstream->sdev("extcairo");
}

void PlPlotImpl::data(const DataArray& data)
{
    clear();

    if (!data.empty())
    {
        for (auto& elem : data)
        {
            m_xdata.push_back(elem.first);
            m_ydata.push_back(elem.second);
        }

        m_xmin = std::round(*std::min_element(m_xdata.begin(), m_xdata.end()));
        m_xmax = std::round(*std::max_element(m_xdata.begin(), m_xdata.end()));
        m_ymin = std::round(*std::min_element(m_ydata.begin(), m_ydata.end()));
        m_ymax = std::round(*std::max_element(m_ydata.begin(), m_ydata.end()));

        SPDLOG_DEBUG("m_xmin {} m_xmax {}  m_ymin {} m_ymax {}", m_xmin, m_xmax, m_ymin, m_ymax);
    }
}

void PlPlotImpl::add_data(const DataArray& data)
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

        m_xmin = std::round(*std::min_element(m_xdata.begin(), m_xdata.end()));
        m_xmax = std::round(*std::max_element(m_xdata.begin(), m_xdata.end()));
        m_ymin = std::round(*std::min_element(m_ydata.begin(), m_ydata.end()));
        m_ymax = std::round(*std::max_element(m_ydata.begin(), m_ydata.end()));

        SPDLOG_DEBUG("In: {} : m_xmin {} m_xmax {}  m_ymin {} m_ymax {} \n", __func__, m_xmin, m_xmax, m_ymin, m_ymax);
    }
}

void PlPlotImpl::data(const StringDataArray& data)
{
    clear();
    if (!data.empty())
    {
        for (auto& elem : data)
        {
            m_sdata.push_back(elem.second);
            m_ydata.push_back(elem.first);
        }

        m_ymin = std::round(*std::min_element(m_ydata.begin(), m_ydata.end()));
        m_ymax = std::round(*std::max_element(m_ydata.begin(), m_ydata.end()));

        SPDLOG_DEBUG("m_ymin {} m_ymax {}", m_ymin, m_ymax);
    }
}

void PlPlotImpl::add_data(const StringDataArray& data)
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

        m_ymin = std::round(*std::min_element(m_ydata.begin(), m_ydata.end()));
        m_ymax = std::round(*std::max_element(m_ydata.begin(), m_ydata.end()));

        SPDLOG_DEBUG("m_ymin {} m_ymax {}", m_ymin, m_ymax);
    }
}

void PlPlotImpl::remove_data(uint32_t count)
{
    if (!m_sdata.empty())
    {
        if (count <= m_sdata.size())
        {
            m_sdata.erase(m_sdata.begin(), m_sdata.begin() + count);
        }
        else
        {
            clear();
        }
    }

    if (!m_xdata.empty())
    {
        if (count <= m_xdata.size())
        {
            m_xdata.erase(m_xdata.begin(), m_xdata.begin() + count);
            m_xmin = std::round(*std::min_element(m_xdata.begin(), m_xdata.end()));
            m_xmax = std::round(*std::max_element(m_xdata.begin(), m_xdata.end()));
        }
        else
        {
            clear();
        }
    }

    if (!m_ydata.empty())
    {
        if (count <= m_ydata.size())
        {
            m_ydata.erase(m_ydata.begin(), m_ydata.begin() + count);
            m_ymin = std::round(*std::min_element(m_ydata.begin(), m_ydata.end()));
            m_ymax = std::round(*std::max_element(m_ydata.begin(), m_ydata.end()));
        }
        else
        {
            clear();
        }
    }
}

void PlPlotImpl::clear()
{
    m_xdata.clear();
    m_sdata.clear();
    m_ydata.clear();

    m_ymin = m_ymax = m_xmin = m_xmax = 0.;

    m_clearsurface = true;
}

void PlPlotImpl::title(const std::string& title)
{
    m_title = title;
}

void PlPlotImpl::label(const std::string& xlabel, const std::string& ylabel, const std::string& title)
{
    m_xlabel = xlabel;
    m_ylabel = ylabel;
    m_title = title;
}

void PlPlotImpl::show_ticks(bool enable)
{
    m_ticks = enable;

    if (m_ticks && m_grid)
        m_axis = 2;
    else if (m_ticks)
        m_axis = 0;
    else
        m_axis = -1;
}

void PlPlotImpl::show_grid(bool enable)
{
    m_grid = enable;
    /**
     * if Grid is enable, then enable ticks to show
     * grid at major tick positions
     */
    if (m_grid)
        m_ticks = true;

    if (m_ticks && m_grid)
        m_axis = 2;
    else if (m_ticks)
        m_axis = 0;
    else
        m_axis = -1;
}

void PlPlotImpl::width(int val)
{
    m_line_width = val;
}

void PlPlotImpl::pattern(int val)
{
    m_pattern = val;
}

void PlPlotImpl::point_type(int ptype)
{
    m_pointtype = ptype;
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
    PLINT font_style = slant == Font::Slant::normal ? 0 : (slant == Font::Slant::italic ? 1 : 2);
    PLINT font_weight = font.weight() == Font::Weight::normal ? 0 : 1;

    m_plstream->sfont(font_family, font_style, font_weight);

    // Todo add setting font size. (i.e. need to convert font size to pixel size)

}

void PlPlotImpl::plplot_color(Color& color)
{
    auto r = static_cast<PLINT>(color.red());
    auto g = static_cast<PLINT>(color.green());
    auto b = static_cast<PLINT>(color.blue());
    auto a = static_cast<PLFLT>(color.alpha() / 255.0);
    m_plstream->scmap1n(1);
    m_plstream->scmap1a(&r, &g, &b, &a, 1);
    m_plstream->col1(1);
}

void PlPlotImpl::resize(const Size& size)
{
    m_plstream.reset(new plstream());
    m_initalize = false;
    m_clearsurface = true;
    m_plstream->sdev("extcairo");
}

PlPlotImpl::~PlPlotImpl() = default;


PlPlotLineChart::PlPlotLineChart(LineChart& interface, const Rect& rect)
    : PlPlotImpl(rect),
      m_interface(interface)
{
    m_font = m_interface.font();
    plplot_font(m_font);
}

void PlPlotLineChart::draw(Painter& painter, const Rect& rect)
{
    auto b = m_interface.content_area();

    if (!m_initalize)
    {
        m_plstream->spage(0, 0, b.width(), b.height(), b.x(), b.y());
        m_plstream->init();
        m_initalize = true;
    }

    m_plstream->cmd(PLESC_DEVINIT, painter.context().get());

    if (m_clearsurface)
    {
        m_plstream->clear();
        m_clearsurface = false;
    }

    plplot_color(m_interface.color(Palette::ColorId::button_bg).color());


    if ((!m_xdata.empty()) && (!m_ydata.empty()))
    {
        // set env
        m_plstream->env(m_xmin, m_xmax, m_ymin, m_ymax, 0, m_axis);

        //set line style
        m_plstream->lsty(m_pattern <= 0 ? 1 : m_pattern);

        // set line width
        m_plstream->width(m_line_width);

        // plot
        m_plstream->line(m_xdata.size(), m_xdata.data(), m_ydata.data());
    }

    Font tfont = m_interface.font();
    if (m_font != tfont)
        plplot_font(tfont);

    // set text color
    plplot_color(m_interface.color(Palette::ColorId::label_text).color());

    //set text or label
    m_plstream->lab(m_xlabel.c_str(), m_ylabel.c_str(), m_title.c_str());
}

PlPlotLineChart::~PlPlotLineChart() = default;

PlPlotPointChart::PlPlotPointChart(PointChart& interface, const Rect& rect)
    : PlPlotImpl(rect),
      m_interface(interface)
{
    m_font = m_interface.font();
    plplot_font(m_font);
}

void PlPlotPointChart::draw(Painter& painter, const Rect& rect)
{
    auto b = m_interface.content_area();

    if (!m_initalize)
    {
        m_plstream->spage(b.x(), b.y(), b.width(), b.height(), 0, 0);
        m_plstream->init();
        m_initalize = true;
    }

    m_plstream->cmd(PLESC_DEVINIT, painter.context().get());

    if (m_clearsurface)
    {
        m_plstream->clear();
        m_clearsurface = false;
    }

    //set axis color
    plplot_color(m_interface.color(Palette::ColorId::button_bg).color());

    if ((!m_xdata.empty()) && (!m_ydata.empty()))
    {
        // set env
        m_plstream->env(m_xmin, m_xmax, m_ymin, m_ymax, 0, m_axis);

        // draw points
        m_plstream->poin(m_xdata.size(), m_xdata.data(), m_ydata.data(), m_pointtype);
    }

    // set font
    Font tfont = m_interface.font();
    if (m_font != tfont)
        plplot_font(tfont);

    // set label color
    plplot_color(m_interface.color(Palette::ColorId::label_text).color());

    // set label
    m_plstream->lab(m_xlabel.c_str(), m_ylabel.c_str(), m_title.c_str());
}

PlPlotPointChart::~PlPlotPointChart() = default;

PlPlotBarChart::PlPlotBarChart(BarChart& interface, const Rect& rect)
    : PlPlotImpl(rect),
      m_interface(interface)
{
    m_font = m_interface.font();
    plplot_font(m_font);
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
    auto b = m_interface.content_area();

    if (!m_initalize)
    {
        m_plstream->spage(b.x(), b.y(), b.width(), b.height(), 0, 0);
        m_plstream->init();
        m_initalize = true;
    }

    m_plstream->cmd(PLESC_DEVINIT, painter.context().get());

    if (m_clearsurface)
    {
        m_plstream->clear();
        m_clearsurface = false;
    }

    m_plstream->adv(0);
    m_plstream->vsta();

    //set axis color.
    plplot_color(m_interface.color(Palette::ColorId::button_bg).color());

    // Draw axis
    if ((!m_ydata.empty()) && (!m_sdata.empty()))
    {
        std::size_t xmax = m_sdata.size() * 2;
        m_plstream->wind(0.0, xmax, 0.0, m_ymax);
        if (m_axis == 2)
            m_plstream->box("bgit", 0, 0, "bginvst", 0, 0);
        else if (m_axis == 0)
            m_plstream->box("bit", 0, 0, "binvst", 0, 0);
        else
            m_plstream->box("bi", 0, 0, "binvs", 0, 0);
    }
    else if ((!m_ydata.empty()) && (!m_xdata.empty()))
    {
        m_plstream->env(m_xmin, m_xmax, m_ymin, m_ymax, 0, m_axis);
    }

    // set font
    Font tfont = m_interface.font();
    if (m_font != tfont)
        plplot_font(tfont);

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

                if (m_axis != -1)
                {
                    plplot_color(m_interface.color(Palette::ColorId::button_bg).color());
                    auto pos = static_cast<float>(i) / static_cast<float>(n);
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

PlPlotHBarChart::PlPlotHBarChart(HorizontalBarChart& interface, const Rect& rect)
    : PlPlotImpl(rect),
      m_interface(interface)
{
    m_font = m_interface.font();
    plplot_font(m_font);
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
    auto b = m_interface.content_area();

    if (!m_initalize)
    {
        m_plstream->spage(b.x(), b.y(), b.width(), b.height(), 0, 0);
        m_plstream->init();
        m_initalize = true;
    }

    m_plstream->cmd(PLESC_DEVINIT, painter.context().get());

    if (m_clearsurface)
    {
        m_plstream->clear();
        m_clearsurface = false;
    }

    m_plstream->adv(0);
    m_plstream->vsta();

    // set font
    Font tfont = m_interface.font();
    if (m_font != tfont)
        plplot_font(tfont);

    if ((!m_sdata.empty()) && (!m_ydata.empty()))
    {
        //set axis color.
        plplot_color(m_interface.color(Palette::ColorId::button_bg).color());

        m_plstream->wind(0.0, m_ymax, 0.0, (m_sdata.size() * 2));
        if (m_axis == 2)
            m_plstream->box("bginvst", 0, 0, "bgit", 0, 0);
        else if (m_axis == 0)
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
            if (m_axis != -1)
            {
                auto pos = static_cast<float>(i) / static_cast<float>(n);
                m_plstream->mtex("lv", 1.0, pos, 1, m_sdata.at(i).c_str());
            }
            ++i;
        }
    }

    // set labels color
    plplot_color(m_interface.color(Palette::ColorId::label_text).color());

    // set labels
    m_plstream->lab(m_xlabel.c_str(), m_ylabel.c_str(), m_title.c_str());
}

PlPlotHBarChart::~PlPlotHBarChart() = default;

PlPlotPieChart::PlPlotPieChart(PieChart& interface, const Rect& rect)
    : PlPlotImpl(rect),
      m_interface(interface)
{
    m_font = m_interface.font();
    plplot_font(m_font);
}

void PlPlotPieChart::draw(Painter& painter, const Rect& rect)
{
    auto b = m_interface.content_area();

    if (!m_initalize)
    {
        m_plstream->spage(b.x(), b.y(), b.width(), b.height(), 0, 0);
        m_plstream->init();
        m_initalize = true;
    }

    m_plstream->cmd(PLESC_DEVINIT, painter.context().get());

    if (m_clearsurface)
    {
        m_plstream->clear();
        m_clearsurface = false;
    }

    m_plstream->adv(0);
    // Ensure window has aspect ratio of one so plotted as a circle.
    m_plstream->vasp(1.0);

    m_plstream->wind(0.1, 10.0, 0.1, 10.0);

    // set font
    Font tfont = m_interface.font();
    if (m_font != tfont)
        plplot_font(tfont);

    // get labels color & set title color
    plplot_color(m_interface.color(Palette::ColorId::label_text).color());

    // set title
    m_plstream->lab("", "", m_title.c_str());

    if ((!m_ydata.empty()) && (!m_sdata.empty()))
    {
        int theta, theta0 = 0;
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
