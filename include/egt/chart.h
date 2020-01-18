/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_CHART_H
#define EGT_CHART_H

/**
 * @file
 * @brief Working with charts.
 */

#include <egt/widget.h>

namespace egt
{
inline namespace v1
{

namespace detail
{
class PlPlotLineChart;
class PlPlotPointChart;
class PlPlotBarChart;
class PlPlotHBarChart;
class PlPlotPieChart;
}

/**
 * LineChart Widget is used for displaying informations as a
 * series of data points.
 *
 * The LineChart widget is using plplot library api's.
 */
class EGT_API LineChart: public Widget
{
public:

    enum LinePattern
    {
        solid = 1,
        dotted,
        dashes,
    };

    /**
     * Construct a LineChart with the specified size.
     *
     * @param[in] rect size of frame with offset x & y.
     */
    explicit LineChart(const Rect& rect = {});

    virtual void draw(Painter& painter, const Rect& rect) override;

    /**
     * A data pair array for LineChart
     */
    using DataArray = std::vector<std::pair<double, double>>;

    /**
     * Set a new set of data to LineChart.
     *
     * @param[in] data is a data items for LineChart.
     */
    virtual void data(const DataArray& data);

    /**
     * Add data items to an existing array.
     *
     * @param[in] data is a data items for LineChart.
     */
    virtual void add_data(const DataArray& data);

    /**
     * Remove data items from top in an array.
     *
     * @param[in] count is the number of items to remove.
     */
    virtual void remove_data(uint32_t count);

    /**
     * Remove all data items.
     */
    virtual void clear();

    /**
     * Set line width
     *
     * @param[in] val size of a line width.
     *
     */
    virtual void line_width(int val);

    /**
     * Sets line style
     *
     *  @param[in] pattern is one of the enum LinePattern
     */
    virtual void line_pattern(LinePattern pattern);

    /**
     * Set chart labels.
     *
     * @param[in] xlabel is x-axis label.
     * @param[in] ylabel is y-axis label.
     * @param[in] title is Chart title.
     */
    virtual void label(const std::string& xlabel,
                       const std::string& ylabel,
                       const std::string& title);

    /**
     * Show x & y axis major ticks.
     *
     * @param[in] enable to show/hide ticks.
     */
    virtual void show_ticks(bool enable);

    /**
     * Show grid at major tick positions for x & y axis
     *
     * @param[in] enable to show/hide grid.
     */
    virtual void show_grid(bool enable);

    /**
     * Resize the LineChart.
     *
     * Change width and height of the LineChart.
     *
     * @param[in] size The new size of the LineChart.
     */
    virtual void resize(const Size& size) override;

    virtual ~LineChart();

protected:
    std::unique_ptr<detail::PlPlotLineChart> m_chartimpl;
};

/**
 * PointChart Widget is used for displaying informations as a
 * series of data points.
 *
 * The PointChart widget is using plplot library api's.
 */
class EGT_API PointChart: public Widget
{
public:

    enum PointType
    {
        dot = 1,
        star,
        cross,
        circle,
    };

    /**
     * Construct a PointChart with the specified size.
     *
     * @param[in] rect size of frame with offset x & y.
     */
    explicit PointChart(const Rect& rect = {});

    virtual void draw(Painter& painter, const Rect& rect) override;

    /**
     * A data pair array for PointChart.
     */
    using DataArray = std::vector<std::pair<double, double>>;

    /**
     * Set a new set of data to PointChart.
     *
     * @param[in] data is a data items for PointChart.
     */
    virtual void data(const DataArray& data);

    /**
     * Add data items to an existing array.
     *
     * @param[in] data is a data items for PointChart.
     */
    virtual void add_data(const DataArray& data);

    /**
     * Remove data items from top in an array.
     *
     * @param[in] count is the number of items to remove.
     */
    virtual void remove_data(uint32_t count);

    /**
     * Remove all data items.
     */
    virtual void clear();

    /**
     * Select the point type.
     *
     * @param[in] ptype one of the enum point_type
     */
    virtual void point_type(PointType ptype);

    /**
     * Set chart labels.
     *
     * @param[in] xlabel is x-axis label.
     * @param[in] ylabel is y-axis label.
     * @param[in] title is Chart title.
     */
    virtual void label(const std::string& xlabel,
                       const std::string& ylabel,
                       const std::string& title);

    /**
     * Show x & y axis major ticks.
     *
     * @param[in] enable to show/hide ticks.
     */
    virtual void show_ticks(bool enable);

    /**
     * Show grid at major tick positions for x & y axis
     *
     * @param[in] enable to show/hide grid.
     */
    virtual void show_grid(bool enable);

    /**
     * Resize the PointChart.
     *
     * Change width and height of the PointChart.
     *
     * @param[in] size The new size of the PointChart.
     */
    virtual void resize(const Size& size) override;

    virtual ~PointChart();

protected:
    std::unique_ptr<detail::PlPlotPointChart> m_chartimpl;
};

/**
 * Vertical BarChart Widget is used for displaying a
 * set of categorical data.
 *
 * The Vertical BarChart widget is using plplot library api's.
 */
class EGT_API BarChart: public Widget
{
public:

    enum BarPattern
    {
        soild = 0,
        horizontal_line = 1,
        vertical_line = 2,
        boxes = 7,
    };

    /**
     * Construct a BarChart with the specified size.
     *
     * @param[in] rect Size of frame with offset x & y.
     */
    explicit BarChart(const Rect& rect = {});

    virtual void draw(Painter& painter, const Rect& rect) override;

    /**
     * A data pair array for BarChart.
     */
    using DataArray = std::vector<std::pair<double, double>>;
    using StringDataArray = std::vector<std::pair<double, std::string>>;

    /**
     * Set a new set of data to BarChart.
     *
     * @param[in] data is a data items for BarChart.
     */
    virtual void data(const DataArray& data);

    /**
     * Add data items to an existing array.
     *
     * @param[in] data is a data items for BarChart.
     */
    virtual void add_data(const DataArray& data);

    /**
     * Set a new set of data to BarChart.
     *
     * @param[in] data is a data items for BarChart.
     */
    virtual void data(const StringDataArray& data);

    /**
     * Add data items to an existing array.
     *
     * @param[in] data is a data items for BarChart.
     */
    virtual void add_data(const StringDataArray& data);

    /**
     * Remove data items from top in an array.
     *
     * @param[in] count is the number of items to remove.
     */
    virtual void remove_data(uint32_t count);

    /**
     * Remove all data items.
     */
    virtual void clear();

    /**
     * Set chart labels.
     *
     * @param[in] xlabel is x-axis label.
     * @param[in] ylabel is y-axis label.
     * @param[in] title is Chart title.
     */
    virtual void label(const std::string& xlabel,
                       const std::string& ylabel,
                       const std::string& title);

    /**
     * Sets Bar style
     *
     * @param[in] pattern is one of the enum BarPattern
     */
    virtual void bar_pattern(BarPattern pattern);

    /**
     * Show x & y axis major ticks.
     *
     * @param[in] enable to show/hide ticks.
     */
    virtual void show_ticks(bool enable);

    /**
     * Show grid at major tick positions for x & y axis
     *
     * @param[in] enable to show/hide grid.
     */
    virtual void show_grid(bool enable);

    /**
     * Resize the BarChart.
     *
     * Change width and height of the BarChart.
     *
     * @param[in] size The new size of the BarChart.
     */
    virtual void resize(const Size& size) override;

    virtual ~BarChart();

protected:
    std::unique_ptr<detail::PlPlotBarChart> m_chartimpl;
};


/**
 * Horizontal BarChart Widget is used for displaying a set
 * of categorical data.
 *
 * The Horizontal BarChart Widget is using plplot library api's.
 */
class EGT_API HorizontalBarChart: public Widget
{
public:

    enum BarPattern
    {
        soild = 0,
        horizontal_line = 1,
        vertical_line = 2,
        boxes = 7,
    };

    /**
     * Construct a HorizontalBarChart with the specified size.
     *
     * @param[in] rect Size of frame with offset x & y.
     */
    explicit HorizontalBarChart(const Rect& rect = {});

    virtual void draw(Painter& painter, const Rect& rect) override;

    /**
     * A data pair array for HorizontalBarChart.
     */
    using DataArray = std::vector<std::pair<double, std::string>>;

    /**
     * Set a new set of data to HorizontalBarChart.
     *
     * @param[in] data is a data items for HorizontalBarChart.
     */
    virtual void data(const DataArray& data);

    /**
     * Add data items to an existing array.
     *
     * @param[in] data is a data items for HorizontalBarChart.
     */
    virtual void add_data(const DataArray& data);

    /**
     * Remove data items from top in an array.
     *
     * @param[in] count is the number of items to remove.
     */
    virtual void remove_data(uint32_t count);

    /**
     * Remove all data items.
     */
    virtual void clear();

    /**
     * Set chart labels.
     *
     * @param[in] xlabel is x-axis label.
     * @param[in] ylabel is y-axis label.
     * @param[in] title is Chart title.
     */
    virtual void label(const std::string& xlabel,
                       const std::string& ylabel,
                       const std::string& title);

    /**
     * sets the Bar style
     *
     * @param[in] pattern is one of the enum Bar_pattern
     */
    virtual void bar_pattern(BarPattern pattern);

    /**
     * Show x & y axis major ticks.
     *
     * @param[in] enable to show/hide ticks.
     */
    virtual void show_ticks(bool enable);

    /**
     * Show grid at major tick positions for x & y axis
     *
     * @param[in] enable to show/hide grid.
     */
    virtual void show_grid(bool enable);

    /**
     * Resize the HorizontalBarChart.
     *
     * Change width and height of the HorizontalBarChart.
     *
     * @param[in] size The new size of the HorizontalBarChart.
     */
    virtual void resize(const Size& size) override;

    virtual ~HorizontalBarChart();

protected:
    std::unique_ptr<detail::PlPlotHBarChart> m_chartimpl;
};


/**
 * PieChart Widget is a circular statistical graphic, which
 * is divided into slices to illustrate numerical proportion
 *
 * The PieChart Widget is using plplot library api's.
 */
class EGT_API PieChart: public Widget
{
public:

    /**
     * Construct a PieChart with the specified size.
     *
     * @param[in] rect Size of frame with offset x & y.
     */
    explicit PieChart(const Rect& rect = {});

    virtual void draw(Painter& painter, const Rect& rect) override;

    /**
     * Set PieChart title
     *
     * @param[in] title is PieChart title.
     */
    virtual void title(const std::string& title);

    /**
     * A data pair array for PieChart.
     */
    using DataArray = std::vector<std::pair<double, std::string>>;

    /**
     * Set a new set of data to PieChart.
     *
     * @param[in] data is a data items for PieChart.
     */
    virtual void data(const DataArray& data);

    /**
     * Add data items to an existing array.
     *
     * @param[in] data is a data items for PieChart.
     */
    virtual void add_data(const DataArray& data);

    /**
     * Remove data items from top in an array.
     *
     * @param[in] count is the number of items to remove.
     */
    virtual void remove_data(uint32_t count);

    /**
     * Remove all data items.
     */
    virtual void clear();

    /**
     * Resize the PieChart.
     *
     * Change width and height of the PieChart.
     *
     * @param[in] size The new size of the PieChart.
     */
    virtual void resize(const Size& size) override;

    virtual ~PieChart();

protected:
    std::unique_ptr<detail::PlPlotPieChart> m_chartimpl;
};

} //End of namespace v1

} //End of egt.

#endif
