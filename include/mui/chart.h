/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_CHART_H
#define MUI_CHART_H

/**
 * @file
 * @brief Working with charts.
 */

#include <map>
#include <mui/widget.h>
#include <vector>

namespace mui
{

#ifdef HAVE_KPLOT
    /**
     * Implements a basic line chart widget.
     *
     */
    class LineChart : public Widget
    {
    public:

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

        explicit LineChart(const Rect& rect = Rect());

        virtual void draw(Painter& painter, const Rect& rect) override;

        virtual void data(const std::vector<struct data_pair>& data)
        {
            if (m_data != data)
            {
                m_data = data;
                damage();
            }
        }

        virtual ~LineChart()
        {}

    protected:

        std::vector<struct data_pair> m_data;
    };
#endif

    /**
     * Pie chart widget.
     */
    class PieChart : public Widget
    {
    public:
        explicit PieChart(const Rect& rect = Rect());

        virtual void draw(Painter& painter, const Rect& rect) override;

        virtual void data(const std::map<std::string, float>& data)
        {
            if (m_data != data)
            {
                m_data = data;
                damage();
            }
        }

    protected:
        std::map<std::string, float> m_data;
    };

}

#endif
