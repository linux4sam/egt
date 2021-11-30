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

    using Widget::Widget;

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

    /**
     * Get chart grid style.
     */
    virtual GridFlag grid_style() const = 0;

    /// A data pair array.
    using DataArray = std::deque<std::pair<double, double>>;

    /// A data pair array for strings.
    using StringDataArray = std::deque<std::pair<double, std::string>>;
};

/**
 * LineChart Widget is used for displaying information as a
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

    /**
     * Construct a Linechart.
     *
     * @param[in] props list of widget argument and its properties.
     */
    LineChart(Serializer::Properties& props);

    LineChart(const LineChart&) = delete;
    LineChart& operator=(const LineChart&) = delete;
    LineChart(LineChart&&) noexcept;
    LineChart& operator=(LineChart&&) noexcept;

    void draw(Painter& painter, const Rect& rect) override;

    /**
     * Set a new set of data to LineChart.
     *
     * @param[in] data is a data items for LineChart.
     */
    void data(const DataArray& data);

    /**
     * Get the data points set for LineChart.
     *
     * @return DataArray set for LineChart.
     */
    ChartBase::DataArray data() const;

    /**
     * Get the number of data points.
     */
    EGT_NODISCARD size_t data_size() const;

    /**
     * Add data items to an existing array.
     *
     * @param[in] data is a data items for LineChart.
     */
    void add_data(const DataArray& data);

    /**
     * Remove data items from top in an array.
     *
     * @param[in] count is the number of items to remove.
     */
    void remove_data(uint32_t count);

    /**
     * Remove all data items.
     */
    void clear();

    /**
     * Set the grid style.
     */
    void grid_style(GridFlag flag) override;

    /**
     * Get chart grid style.
     *
     * @return GridFlag
     */
    GridFlag grid_style() const override;

    /**
     * Set grid width
     *
     * @param[in] val size of a grid width.
     */
    void grid_width(int val);

    /**
     * Get chart grid width.
     *
     * @return grid width
     */
    int grid_width() const;

    /**
     * Set line width
     *
     * @param[in] val size of a line width.     *
     */
    void line_width(int val);

    /**
     * Get chart line width.
     *
     * @return line width
     */
    int line_width() const;

    /**
     * Sets line style
     *
     *  @param[in] pattern is one of the enum LinePattern
     */
    void line_style(LinePattern pattern);

    /**
     * Get Linechart style.
     *
     * @return LinePattern
     */
    LinePattern line_style() const;

    /**
     * Set chart labels.
     *
     * @param[in] xlabel is x-axis label.
     * @param[in] ylabel is y-axis label.
     * @param[in] title is Chart title.
     */
    void label(const std::string& xlabel,
               const std::string& ylabel,
               const std::string& title);

    /**
     * Get chart x-axis label.
     */
    EGT_NODISCARD std::string xlabel() const;

    /**
     * Get chart y-axis label.
     */
    EGT_NODISCARD std::string ylabel() const;

    /**
     * Get chart title.
     */
    EGT_NODISCARD std::string title() const;

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
    void bank(float bank);

    void serialize(Serializer& serializer) const override;

    ~LineChart() override;

protected:

    /// @private
    std::unique_ptr<detail::PlPlotLineChart> m_impl;

private:

    void deserialize(Serializer::Properties& props);
};

/**
 * PointChart Widget is used for displaying information as a
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
        star = 3,
        circle,
        cross,
        dot = 17,
    };

    /**
     * Construct a PointChart with the specified size.
     *
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit PointChart(const Rect& rect = {});

    /**
     * Construct a Pointchart.
     *
     * @param[in] props list of widget argument and its properties.
     */
    PointChart(Serializer::Properties& props);

    PointChart(const PointChart&) = delete;
    PointChart& operator=(const PointChart&) = delete;
    PointChart(PointChart&&) noexcept;
    PointChart& operator=(PointChart&&) noexcept;

    void draw(Painter& painter, const Rect& rect) override;

    /**
     * Set a new set of data to PointChart.
     *
     * @param[in] data is a data items for PointChart.
     */
    void data(const DataArray& data);

    /**
     * Get the data points set for PointChart.
     *
     * @return DataArray set for PointChart.
     */
    ChartBase::DataArray data() const;

    /**
    * Get the number of data points.
    */
    EGT_NODISCARD size_t data_size() const;

    /**
     * Add data items to an existing array.
     *
     * @param[in] data is a data items for PointChart.
     */
    void add_data(const DataArray& data);

    /**
     * Remove data items from top in an array.
     *
     * @param[in] count is the number of items to remove.
     */
    void remove_data(uint32_t count);

    /**
     * Remove all data items.
     */
    void clear();

    /**
     * Select the point type.
     *
     * @param[in] ptype one of the enum point_type
     */
    void point_type(PointType ptype);

    /**
     * get the chart point type.
     *
     * @return PointType enum
     */
    EGT_NODISCARD PointChart::PointType point_type() const;

    /**
     * Set chart labels.
     *
     * @param[in] xlabel is x-axis label.
     * @param[in] ylabel is y-axis label.
     * @param[in] title is Chart title.
     */
    void label(const std::string& xlabel,
               const std::string& ylabel,
               const std::string& title);

    /**
     * Get chart x-axis label.
     */
    EGT_NODISCARD std::string xlabel() const;

    /**
     * Get chart y-axis label.
     */
    EGT_NODISCARD std::string ylabel() const;

    /**
     * Get chart title.
     */
    EGT_NODISCARD std::string title() const;

    /**
    * Set the grid style.
    */
    void grid_style(GridFlag flag) override;

    /**
     * Get chart grid style.
     *
     * @return GridFlag
     */
    GridFlag grid_style() const override;

    /**
     * Set grid width
     *
     * @param[in] val size of a grid width.
     */
    void grid_width(int val);

    /**
     * Get chart grid width.
     *
     * @return grid width
     */
    int grid_width() const;

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
    void bank(float bank);

    void serialize(Serializer& serializer) const override;

    ~PointChart() override;

protected:

    /// @private
    std::unique_ptr<detail::PlPlotPointChart> m_impl;

private:

    void deserialize(Serializer::Properties& props);
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

    /**
     * Construct a Barchart.
     *
     * @param[in] props list of widget argument and its properties.
     */
    BarChart(Serializer::Properties& props);

    BarChart(const BarChart&) = delete;
    BarChart& operator=(const BarChart&) = delete;
    BarChart(BarChart&&) noexcept;
    BarChart& operator=(BarChart&&) noexcept;

    void draw(Painter& painter, const Rect& rect) override;


    /**
     * Set a new set of data to BarChart.
     *
     * @param[in] data is a data items for BarChart.
     */
    void data(const DataArray& data);

    /**
     * Get the data points set for BarChart.
     *
     * @return DataArray set for BarChart.
     */
    DataArray data() const;

    /**
    * Get the number of data points.
    */
    EGT_NODISCARD size_t data_size() const;

    /**
     * Add data items to an existing array.
     *
     * @param[in] data is a data items for BarChart.
     */
    void add_data(const DataArray& data);

    /**
     * Set a new set of data to BarChart.
     *
     * @param[in] data is a data items for BarChart.
     */
    void data(const StringDataArray& data);

    /**
     * Get the data points set for BarChart.
     *
     * @return StringDataArray set for BarChart.
     */
    StringDataArray sdata() const;

    /**
     * Add data items to an existing array.
     *
     * @param[in] data is a data items for BarChart.
     */
    void add_data(const StringDataArray& data);

    /**
     * Remove data items from top in an array.
     *
     * @param[in] count is the number of items to remove.
     */
    void remove_data(uint32_t count);

    /**
     * Remove all data items.
     */
    void clear();

    /**
     * Set chart labels.
     *
     * @param[in] xlabel is x-axis label.
     * @param[in] ylabel is y-axis label.
     * @param[in] title is Chart title.
     */
    void label(const std::string& xlabel,
               const std::string& ylabel,
               const std::string& title);

    /**
     * Get chart x-axis label.
     */
    EGT_NODISCARD std::string xlabel() const;

    /**
     * Get chart y-axis label.
     */
    EGT_NODISCARD std::string ylabel() const;

    /**
     * Get chart title.
     */
    EGT_NODISCARD std::string title() const;

    /**
     * Set bar style.
     *
     * @param[in] pattern is one of the enum BarPattern
     */
    void bar_style(BarPattern pattern);

    /**
     * get the Bar chart bar type.
     *
     * @return PointType enum
     */
    EGT_NODISCARD BarPattern bar_style() const;

    /**
    * Set the grid style.
    */
    void grid_style(GridFlag flag) override;

    /**
     * Get chart grid style.
     *
     * @return GridFlag
     */
    GridFlag grid_style() const override;

    /**
     * Set grid width
     *
     * @param[in] val size of a grid width.
     */
    void grid_width(int val);

    /**
     * Get chart grid width.
     *
     * @return grid width
     */
    int grid_width() const;

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
    void bank(float bank);

    void serialize(Serializer& serializer) const override;

    ~BarChart() override;

protected:

    /// @private
    BarChart(const Rect& rect, std::unique_ptr<detail::PlPlotImpl>&& impl);

    /// @private
    BarChart(Serializer::Properties& props, std::unique_ptr<detail::PlPlotImpl>&& impl);

    /// @private
    std::unique_ptr<detail::PlPlotImpl> m_impl;

private:

    void deserialize(Serializer::Properties& props);
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

    /**
     * Construct a HorizontalBarchart.
     *
     * @param[in] props list of widget argument and its properties.
     */
    HorizontalBarChart(Serializer::Properties& props);

    HorizontalBarChart(const HorizontalBarChart&) = delete;
    HorizontalBarChart& operator=(const HorizontalBarChart&) = delete;
    HorizontalBarChart(HorizontalBarChart&&) noexcept;
    HorizontalBarChart& operator=(HorizontalBarChart&&) noexcept;

    ~HorizontalBarChart() override;
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

    /**
     * Construct a Piechart.
     *
     * @param[in] props list of widget argument and its properties.
     */
    PieChart(Serializer::Properties& props);

    PieChart(const PieChart&) = delete;
    PieChart& operator=(const PieChart&) = delete;
    PieChart(PieChart&&) noexcept;
    PieChart& operator=(PieChart&&) noexcept;

    void draw(Painter& painter, const Rect& rect) override;

    /**
     * Set PieChart title
     *
     * @param[in] title is PieChart title.
     */
    void title(const std::string& title);

    /**
     * Get chart title.
     */
    EGT_NODISCARD std::string title() const;

    /**
     * Set a new set of data to PieChart.
     *
     * @param[in] data is a data items for PieChart.
     */
    void data(const StringDataArray& data);

    /**
     * Get the data points set for PieChart.
     *
     * @return StringDataArray set for PieChart.
     */
    StringDataArray sdata() const;

    /**
     * Get the number of data points.
     */
    EGT_NODISCARD size_t data_size() const;

    /**
     * Add data items to an existing array.
     *
     * @param[in] data is a data items for PieChart.
     */
    void add_data(const StringDataArray& data);

    /**
     * Remove data items from top in an array.
     *
     * @param[in] count is the number of items to remove.
     */
    void remove_data(uint32_t count);

    /**
     * Remove all data items.
     */
    void clear();

    /**
     * Resize the PieChart.
     *
     * Change width and height of the PieChart.
     *
     * @param[in] size The new size of the PieChart.
     */
    void resize(const Size& size) override;

    void serialize(Serializer& serializer) const override;

    ~PieChart() override;

protected:

    /// @private
    std::unique_ptr<detail::PlPlotPieChart> m_impl;

private:

    void deserialize(Serializer::Properties& props);
};

}

}

#endif
