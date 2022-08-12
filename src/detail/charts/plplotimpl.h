/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SRC_DETAIL_CHARTS_PLPLOTIMPL_H
#define EGT_SRC_DETAIL_CHARTS_PLPLOTIMPL_H

#include "egt/chart.h"
#include "egt/painter.h"
#include <memory>
#include <plstream.h>
#include <string>
#include <vector>

namespace egt
{
inline namespace v1
{
namespace detail
{

class PlPlotImpl
{
public:

    PlPlotImpl();

    // special functions deleted because they are never used
    PlPlotImpl(const PlPlotImpl&) = delete;
    PlPlotImpl& operator=(const PlPlotImpl&) = delete;
    PlPlotImpl(PlPlotImpl&&) = delete;
    PlPlotImpl& operator=(PlPlotImpl&&) = delete;

    void title(const std::string& title);

    void label(const std::string& xlabel, const std::string& ylabel, const std::string& title);

    virtual void draw(Painter& painter, const Rect& rect) = 0;

    void data(const ChartItemArray& data);

    void add_data(const ChartItemArray& data);

    ChartItemArray data() const;

    size_t data_size() const;

    void remove_data(uint32_t count);

    void clear();

    void grid_style(ChartBase::GridFlag flag);

    ChartBase::GridFlag grid_style() const
    {
        return m_grid;
    }

    void grid_width(int val);

    int grid_width()
    {
        return m_grid_width;
    }

    void line_width(int val);

    int line_width() const
    {
        return m_line_width;
    }

    void line_style(int val);

    int line_style()
    {
        return m_pattern;
    }

    std::string xlabel() const
    {
        return m_xlabel;
    }

    std::string ylabel() const
    {
        return m_ylabel;
    }

    std::string title() const
    {
        return m_title;
    }

    void point_type(int ptype);

    int point_type() const
    {
        return m_pointtype;
    }

    virtual void resize();

    void bank(float bank);

    float bank() const
    {
        return m_bank;
    }

    virtual void invoke_damage() = 0;

    virtual ~PlPlotImpl();

protected:

    inline PLINT axis() const
    {
        return static_cast<PLINT>(m_grid);
    }

    ChartBase::GridFlag m_grid{ChartBase::GridFlag::box_ticks};

    std::unique_ptr<plstream> m_plstream;
    bool m_initalize{false};
    std::string m_xlabel;
    std::string m_ylabel;
    std::string m_title;

    /*
     * plenv
     * m_xmin and m_xmax cannot be same
     * m_ymin and m_ymax cannot be same
     */
    PLFLT m_xmin{0};
    PLFLT m_xmax{0};
    PLFLT m_ymin{0};
    PLFLT m_ymax{0};

    PLINT m_pattern{0};
    PLINT m_pointtype{17}; //code 17 represents to dot.

    PLINT m_grid_width{1};
    PLINT m_line_width{2};

    std::vector<PLFLT> m_xdata;
    std::vector<PLFLT> m_ydata;
    std::vector<std::string> m_sdata;

    void plplot_color(const Color& color);

    void plplot_verify_viewport();

    void plplot_viewport(PLFLT size);

    void plplot_box(bool xtick_label, bool ytick_label);

    void plplot_label(const shared_cairo_t& cr, Rect b, const Font& font, const Color& color);

    float m_bank{0};
};

class PlPlotLineChart: public PlPlotImpl
{
public:
    explicit PlPlotLineChart(LineChart& iface);

    void draw(Painter& painter, const Rect& rect) override;

    void invoke_damage() override
    {
        m_interface.damage();
    }

protected:
    LineChart& m_interface;
};

class PlPlotPointChart: public PlPlotImpl
{
public:
    explicit PlPlotPointChart(PointChart& iface);

    void draw(Painter& painter, const Rect& rect) override;

    void invoke_damage() override
    {
        m_interface.damage();
    }

protected:
    PointChart& m_interface;
};

class PlPlotBarChart: public PlPlotImpl
{
public:
    explicit PlPlotBarChart(BarChart& iface);

    void draw(Painter& painter, const Rect& rect) override;

    void invoke_damage() override
    {
        m_interface.damage();
    }

protected:
    void plfbox(PLFLT x0, PLFLT y0);

protected:
    BarChart& m_interface;
};

class PlPlotHBarChart: public PlPlotImpl
{
public:
    explicit PlPlotHBarChart(HorizontalBarChart& iface);

    void draw(Painter& painter, const Rect& rect) override;

    void invoke_damage() override
    {
        m_interface.damage();
    }

protected:
    void plfHbox(PLFLT x0, PLFLT y0);

protected:
    HorizontalBarChart& m_interface;
};

class PlPlotPieChart: public PlPlotImpl
{
public:

    explicit PlPlotPieChart(PieChart& iface);

    void draw(Painter& painter, const Rect& rect) override;

    void invoke_damage() override
    {
        m_interface.damage();
    }

protected:
    PieChart& m_interface;
};

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt

#endif
