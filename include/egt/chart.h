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
class PlPlotImpl;
class PlPlotPieChart;
}

class ChartItemArray
{
public:
    ChartItemArray()
    {}

    using DataArray = std::deque<std::pair<double, double>>;

    using StringDataArray = std::deque<std::pair<double, std::string>>;

    bool add(const double& data, const double& data1)
    {
        /**
         * both m_sdata and m_data cannot exist together
         */
        if (m_sdata.empty())
        {
            m_data.emplace_back(std::make_pair(data, data1));
            return true;
        }
        return false;
    }

    bool add(const double& data, const std::string& data1)
    {
        /**
         * both m_sdata and m_data cannot exist together
         */
        if (m_data.empty())
        {
            m_sdata.emplace_back(std::make_pair(data, data1));
            return true;
        }
        return false;
    }

    void data(DataArray& data)
    {
        m_data = data;
    }

    void data(StringDataArray& data)
    {
        m_sdata = data;
    }

    DataArray get_data()
    {
        return m_data;
    }

    StringDataArray get_sdata()
    {
        return m_sdata;
    }

    DataArray get_data() const
    {
        return m_data;
    }

    StringDataArray get_sdata() const
    {
        return m_sdata;
    }

    EGT_NODISCARD bool IsStringArray() const
    {
        if (!m_sdata.empty())
            return true;

        return false;
    }

    void clear()
    {
        m_data.clear();
        m_sdata.clear();
    }

    ~ChartItemArray() = default;

private:
    DataArray m_data;
    StringDataArray m_sdata;
};

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

    void draw(Painter& painter, const Rect& rect) override;

    virtual void create_impl() = 0;

    /**
     * Set a new set of data to Chart.
     *
     * @param[in] data is a data items for Chart.
     */
    void data(const ChartItemArray& data);

    /**
     * Add data items to an existing array.
     *
     * @param[in] data is a data items for Chart.
     */
    void add_data(const ChartItemArray& data);

    /**
     * Get the data points set for Chart.
     *
     * @return DataArray set for Chart.
     */
    ChartItemArray data() const;

    /**
    * Get the number of data points.
    */
    size_t data_size() const;

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
    void grid_style(GridFlag flag);

    /**
     * Get chart grid style.
     *
     * @return GridFlag
     */
    GridFlag grid_style() const;

    /**
     * Set grid width
     *
     * @param[in] val size of a grid width.
     */
    void grid_width(const int val);

    /**
     * Get chart grid width.
     *
     * @return grid width
     */
    int grid_width() const;

    /**
     * Set chart labels.
     *
     * @param[in] xlabel is x-axis label.
     * @param[in] ylabel is y-axis label.
     * @param[in] title is Chart title.
     */
    void label(const std::string& xlabel, const std::string& ylabel, const std::string& title);

    /**
     * Get chart x-axis label.
     */
    std::string xlabel() const;
    /**
     * Get chart y-axis label.
     */
    std::string ylabel() const;

    /**
     * Get chart title.
     */
    std::string title() const;

    /**
     * Set chart x-axis label.
     */
    void xlabel(const std::string& xlabel);
    /**
     * Set chart y-axis label.
     */
    void ylabel(const std::string& ylabel);

    /**
     * Set chart title.
     */
    void title(const std::string& title);

    /**
     * Resize the Chart.
     *
     * Change width and height of the Chart.
     *
     * @param[in] size The new size of the Chart.
     */
    void resize(const Size& size) override;

    /**
     * Add a percent bank beyond the min and maximum values.
     */
    void bank(float bank);

    /**
     * Get bank percent set beyond min and maximum values.
     */
    float bank() const;

    void serialize(Serializer& serializer) const;

    void deserialize(Serializer::Properties& props);

protected:
    /// @private
    std::unique_ptr<detail::PlPlotImpl> m_impl;
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
    LineChart(Serializer::Properties& props)
        : LineChart(props, false)
    {
    }

    LineChart(const LineChart&) = delete;
    LineChart& operator=(const LineChart&) = delete;
    LineChart(LineChart&&) noexcept = default;
    LineChart& operator=(LineChart&&) noexcept = default;

    void create_impl() override;

    /**
     * Set line width
     *
     * @param[in] val size of a line width.     *
     */
    void line_width(const int val);

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

    void serialize(Serializer& serializer) const override;

    ~LineChart() override;

protected:

    LineChart(Serializer::Properties& props, bool is_derived);

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
    PointChart(Serializer::Properties& props)
        : PointChart(props, false)
    {
    }

    PointChart(const PointChart&) = delete;
    PointChart& operator=(const PointChart&) = delete;
    PointChart(PointChart&&) noexcept = default;
    PointChart& operator=(PointChart&&) noexcept = default;

    void create_impl() override;

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

    void serialize(Serializer& serializer) const override;

    ~PointChart() override;

protected:

    PointChart(Serializer::Properties& props, bool is_derived);

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
    BarChart(Serializer::Properties& props)
        : BarChart(props, false)
    {
    }

public:

    BarChart(const BarChart&) = delete;
    BarChart& operator=(const BarChart&) = delete;
    BarChart(BarChart&&) noexcept = default;
    BarChart& operator=(BarChart&&) noexcept = default;

    void create_impl() override;

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

    void serialize(Serializer& serializer) const override;

    ~BarChart() override;

protected:

    BarChart(Serializer::Properties& props, bool is_derived);

    /// @private
    BarChart(const Rect& rect, std::unique_ptr<detail::PlPlotImpl>&& impl);

    /// @private
    BarChart(Serializer::Properties& props, std::unique_ptr<detail::PlPlotImpl>&& impl);

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
    HorizontalBarChart(Serializer::Properties& props)
        : HorizontalBarChart(props, false)
    {
    }

protected:

    HorizontalBarChart(Serializer::Properties& props, bool is_derived);

public:

    HorizontalBarChart(const HorizontalBarChart&) = delete;
    HorizontalBarChart& operator=(const HorizontalBarChart&) = delete;
    HorizontalBarChart(HorizontalBarChart&&) noexcept = default;
    HorizontalBarChart& operator=(HorizontalBarChart&&) noexcept = default;

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
    PieChart(Serializer::Properties& props)
        : PieChart(props, false)
    {
    }

    PieChart(const PieChart&) = delete;
    PieChart& operator=(const PieChart&) = delete;
    PieChart(PieChart&&) noexcept = default;
    PieChart& operator=(PieChart&&) noexcept = default;

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
    void data(const ChartItemArray& data);

    /**
     * Get the data points set for PieChart.
     *
     * @return StringDataArray set for PieChart.
     */
    ChartItemArray data() const;

    /**
     * Get the number of data points.
     */
    EGT_NODISCARD size_t data_size() const;

    /**
     * Add data items to an existing array.
     *
     * @param[in] data is a data items for PieChart.
     */
    void add_data(const ChartItemArray& data);

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
    PieChart(Serializer::Properties& props, bool is_derived);

    /// @private
    std::unique_ptr<detail::PlPlotPieChart> m_impl;

private:

    void deserialize(Serializer::Properties& props);
};

}

}

#endif
