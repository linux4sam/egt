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
#include <map>
#include <vector>

namespace egt
{
inline namespace v1
{

/**
 * @brief Line chart widget.
 *
 */
class LineChart : public Widget
{
public:
    explicit LineChart(const Rect& rect = Rect());

    virtual std::unique_ptr<Widget> clone() override
    {
        return std::unique_ptr<Widget>(make_unique<LineChart>(*this).release());
    }

    // this must mirror struct kpair
    struct data_pair
    {
        double x;
        double y;

        bool operator==(const data_pair& rhs) const
        {
            return (x == rhs.x) && (y == rhs.y);
        }
    };

    virtual void draw(Painter& painter, const Rect& rect) override;

    enum class chart_type
    {
        points,
        marks,
        lines,
        linespoints,
        linesmarks
    };

    using data_array = std::vector<struct data_pair>;

    virtual void add_data(const data_array& data, chart_type type = chart_type::lines)
    {
        data_set d;
        d.type = type;
        d.data = data;
        m_data.push_back(d);
        damage();
    }

    enum
    {
        GRIDX = 0x1,
        GRIDY = 0x2
    };

    void set_grid(uint32_t flags)
    {
        if (m_grid != flags)
        {
            m_grid = flags;
            damage();
        }
    }

    void set_line_width(float width)
    {
        if (m_linewidth != width)
        {
            m_linewidth = width;
            damage();
        }
    }

    virtual void clear()
    {
        m_data.clear();
        damage();
    }

    virtual ~LineChart() = default;

protected:

    struct data_set
    {
        chart_type type;
        data_array data;
    };

    std::vector<data_set> m_data;
    float m_linewidth{1.0};
    uint32_t m_grid{0};
};

/**
 * @brief Pie chart widget.
 */
class PieChart : public Widget
{
public:
    explicit PieChart(const Rect& rect = Rect());

    virtual std::unique_ptr<Widget> clone() override
    {
        return std::unique_ptr<Widget>(make_unique<PieChart>(*this).release());
    }

    virtual void draw(Painter& painter, const Rect& rect) override;

    using data_array = std::map<std::string, float>;

    /**
     * Data is a percentage, from 0.0 to 1.0.
     */
    virtual void data(const data_array& data)
    {
        if (m_data != data)
        {
            m_data = data;
            damage();
        }
    }

    virtual ~PieChart() noexcept = default;

protected:
    data_array m_data;
    std::vector<Color> m_colors;
};

}
}

#endif
