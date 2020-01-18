/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_PLPLOTIMPL_H
#define EGT_DETAIL_PLPLOTIMPL_H

#include <egt/chart.h>
#include <egt/painter.h>
#include <memory>
#include <plstream.h>
#include <string>

namespace egt
{
inline namespace v1
{
namespace detail
{

class PlPlotImpl
{
public:

    PlPlotImpl() = delete;

    explicit PlPlotImpl(const Rect& rect);

    virtual void title(const std::string& title);

    virtual void label(const std::string& xlabel, const std::string& ylabel, const std::string& title);

    virtual void draw(Painter& painter, const Rect& rect) = 0;

    using DataArray = std::vector<std::pair<double, double>>;
    virtual void data(const DataArray& data);

    virtual void add_data(const DataArray& data);

    using StringDataArray = std::vector<std::pair<double, std::string>>;
    virtual void data(const StringDataArray& data);

    virtual void add_data(const StringDataArray& data);

    virtual void remove_data(uint32_t count);

    virtual void clear();

    virtual void show_ticks(bool enable);

    virtual void show_grid(bool enable);

    virtual void width(int val);

    virtual void pattern(int val);

    virtual void point_type(int ptype);

    virtual void resize(const Size& size);

    virtual ~PlPlotImpl();

protected:
    std::unique_ptr<plstream> m_plstream;
    bool m_initalize{false};
    std::string m_xlabel;
    std::string m_ylabel;
    std::string m_title;

    PLFLT m_xmin{0.};
    PLFLT m_xmax{0.};
    PLFLT m_ymin{0.};
    PLFLT m_ymax{0.};

    PLINT m_pattern{0};
    PLINT m_pointtype{17}; //code 17 represents to dot.

    PLINT m_line_width{1};

    Font m_font;

    /** set axis value
     *  -2: draw no box, no tick marks, no numeric tick labels, no axes.
     *  -1: draw box only.
     *   0: draw box, ticks, and numeric tick labels.
     *   1: also draw coordinate axes at x=0 and y=0.
     *   2: also draw a grid at major tick positions in both coordinates.
     */
    PLINT m_axis{-1};
    PLINT m_axis_old{-1};

    bool m_clearsurface{false};

    std::vector<PLFLT> m_xdata;
    std::vector<PLFLT> m_ydata;
    std::vector<std::string> m_sdata;

    void plplot_color(Color& color);

    void plplot_font(const Font& font);
};

class PlPlotLineChart: public PlPlotImpl
{
public:
    explicit PlPlotLineChart(LineChart& interface, const Rect& rect);

    virtual void draw(Painter& painter, const Rect& rect) override;

    virtual ~PlPlotLineChart();

protected:
    LineChart& m_interface;
};

class PlPlotPointChart: public PlPlotImpl
{
public:
    explicit PlPlotPointChart(PointChart& interface, const Rect& rect);

    virtual void draw(Painter& painter, const Rect& rect) override;

    virtual ~PlPlotPointChart();

protected:
    PointChart& m_interface;
};

class PlPlotBarChart: public PlPlotImpl
{
public:
    explicit PlPlotBarChart(BarChart& interface, const Rect& rect);

    virtual void draw(Painter& painter, const Rect& rect) override;

    virtual ~PlPlotBarChart();

protected:
    void plfbox(PLFLT x0, PLFLT y0);

protected:
    BarChart& m_interface;
};

class PlPlotHBarChart: public PlPlotImpl
{
public:
    explicit PlPlotHBarChart(HorizontalBarChart& interface, const Rect& rect);

    virtual void draw(Painter& painter, const Rect& rect) override;

    virtual ~PlPlotHBarChart();

protected:
    void plfHbox(PLFLT x0, PLFLT y0);

protected:
    HorizontalBarChart& m_interface;
};

class PlPlotPieChart: public PlPlotImpl
{
public:

    explicit PlPlotPieChart(PieChart& interface, const Rect& rect);

    virtual void draw(Painter& painter, const Rect& rect) override;

    virtual ~PlPlotPieChart();

protected:
    PieChart& m_interface;
};

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt

#endif
