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

#include <deque>
#include <egt/widget.h>

namespace egt
{
inline namespace v1
{

namespace detail
{
class PlPlotBarChart;
class PlPlotHBarChart;
class PlPlotImpl;
class PlPlotLineChart;
class PlPlotPieChart;
class PlPlotPointChart;
}

/**
 * Abstract base chart class.
 */
class EGT_API ChartBase: public Widget
{
public:

    /**
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit ChartBase(const Rect& rect)
        : Widget(rect)
    {}

    /**
     * Flags used to control how a chart grid is drawn.
     */
    enum class GridFlag
    {
        /// draw no box, no tick marks, no numeric tick labels, no axes
        none = -2,
        /// draw box only
        box = -1,
        /// draw box, ticks, and numeric tick labels
        box_ticks = 0,
        /// also draw coordinate axes at x=0 and y=0
        box_ticks_coord = 1,
        /// also draw a grid at major tick positions in both coordinates
        box_major_ticks_coord = 2,
        /// also draw a grid at minor tick positions in both coordinates
        box_minor_ticks_coord = 3,
    };

    /**
     * Set chart grid style.
     */
    virtual void grid_style(GridFlag flag) = 0;

    /// A data pair array.
    using DataArray = std::deque<std::pair<double, double>>;

    /// A data pair array for strings.
    using StringDataArray = std::deque<std::pair<double, std::string>>;
};

/**
 * LineChart Widget is used for displaying informations as a
 * series of data points.
 *
 * The LineChart widget is using plplot library api's.
 */
class EGT_API LineChart: public ChartBase
{
public:

    /// Line style patterns.
    enum LinePattern
    {
        solid = 1,
        dotted,
        dashes,
    };

    /**
     * Construct a LineChart with the specified size.
     *
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit LineChart(const Rect& rect = {});

    EGT_OPS_NOCOPY_MOVE(LineChart);
    ~LineChart() noexcept override;

    void draw(Painter& painter, const Rect& rect) override;

    /**
     * Set a new set of data to LineChart.
     *
     * @param[in] data is a data items for LineChart.
     */
    virtual void data(const DataArray& data);

    /**
     * Get the number of data points.
     */
    virtual size_t data_size() const;

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
     * Set the grid style.
     */
    void grid_style(GridFlag flag) override;

    /**
     * Set grid width
     *
     * @param[in] val size of a grid width.
     */
    virtual void grid_width(int val);

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
    virtual void line_style(LinePattern pattern);

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
     * Resize the LineChart.
     *
     * Change width and height of the LineChart.
     *
     * @param[in] size The new size of the LineChart.
     */
    void resize(const Size& size) override;

    /**
     * Add a percent bank beyond the min and maximum values.
     */
    virtual void bank(float bank);

protected:

    /// @private
    std::unique_ptr<detail::PlPlotLineChart> m_impl;
};

/**
 * PointChart Widget is used for displaying informations as a
 * series of data points.
 *
 * The PointChart widget is using plplot library api's.
 */
class EGT_API PointChart: public ChartBase
{
public:

    /// Point styles.
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
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit PointChart(const Rect& rect = {});

    EGT_OPS_NOCOPY_MOVE(PointChart);
    ~PointChart() noexcept override;

    void draw(Painter& painter, const Rect& rect) override;

    /**
     * Set a new set of data to PointChart.
     *
     * @param[in] data is a data items for PointChart.
     */
    virtual void data(const DataArray& data);

    /**
    * Get the number of data points.
    */
    virtual size_t data_size() const;

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
    * Set the grid style.
    */
    void grid_style(GridFlag flag) override;

    /**
     * Set grid width
     *
     * @param[in] val size of a grid width.
     */
    virtual void grid_width(int val);

    /**
     * Resize the PointChart.
     *
     * Change width and height of the PointChart.
     *
     * @param[in] size The new size of the PointChart.
     */
    void resize(const Size& size) override;

    /**
     * Add a percent bank beyond the min and maximum values.
     */
    virtual void bank(float bank);

protected:

    /// @private
    std::unique_ptr<detail::PlPlotPointChart> m_impl;
};

/**
 * Vertical BarChart Widget is used for displaying a
 * set of categorical data.
 *
 * The Vertical BarChart widget is using plplot library api's.
 */
class EGT_API BarChart: public ChartBase
{
public:

    /// Bar pattern styles
    enum class BarPattern
    {
        solid = 0,
        horizontal_line = 1,
        vertical_line = 2,
        boxes = 7,
    };

    /**
     * Construct a BarChart with the specified size.
     *
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit BarChart(const Rect& rect = {});

    EGT_OPS_NOCOPY_MOVE(BarChart);
    ~BarChart() noexcept override;

    void draw(Painter& painter, const Rect& rect) override;


    /**
     * Set a new set of data to BarChart.
     *
     * @param[in] data is a data items for BarChart.
     */
    virtual void data(const DataArray& data);

    /**
    * Get the number of data points.
    */
    virtual size_t data_size() const;

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
     * Set bar style.
     *
     * @param[in] pattern is one of the enum BarPattern
     */
    virtual void bar_style(BarPattern pattern);

    /**
    * Set the grid style.
    */
    void grid_style(GridFlag flag) override;

    /**
     * Set grid width
     *
     * @param[in] val size of a grid width.
     */
    virtual void grid_width(int val);

    /**
     * Resize the BarChart.
     *
     * Change width and height of the BarChart.
     *
     * @param[in] size The new size of the BarChart.
     */
    void resize(const Size& size) override;

    /**
     * Add a percent bank beyond the min and maximum values.
     */
    virtual void bank(float bank);

protected:

    /// @private
    BarChart(const Rect& rect, std::unique_ptr<detail::PlPlotImpl>&& impl);

    /// @private
    std::unique_ptr<detail::PlPlotImpl> m_impl;
};


/**
 * Horizontal BarChart Widget is used for displaying a set
 * of categorical data.
 *
 * The Horizontal BarChart Widget is using plplot library api's.
 */
class EGT_API HorizontalBarChart: public BarChart
{
public:

    /**
     * Construct a HorizontalBarChart with the specified size.
     *
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit HorizontalBarChart(const Rect& rect = {});

    EGT_OPS_NOCOPY_MOVE(HorizontalBarChart);
    ~HorizontalBarChart() noexcept override;
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

    /// String data array type
    using StringDataArray = ChartBase::StringDataArray;

    /**
     * Construct a PieChart with the specified size.
     *
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit PieChart(const Rect& rect = {});

    EGT_OPS_NOCOPY_MOVE(PieChart);
    ~PieChart() noexcept override;

    void draw(Painter& painter, const Rect& rect) override;

    /**
     * Set PieChart title
     *
     * @param[in] title is PieChart title.
     */
    virtual void title(const std::string& title);

    /**
     * Set a new set of data to PieChart.
     *
     * @param[in] data is a data items for PieChart.
     */
    virtual void data(const StringDataArray& data);

    /**
    * Get the number of data points.
    */
    virtual size_t data_size() const;

    /**
     * Add data items to an existing array.
     *
     * @param[in] data is a data items for PieChart.
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
     * Resize the PieChart.
     *
     * Change width and height of the PieChart.
     *
     * @param[in] size The new size of the PieChart.
     */
    void resize(const Size& size) override;

protected:

    /// @private
    std::unique_ptr<detail::PlPlotPieChart> m_impl;
};

}

}

#endif
