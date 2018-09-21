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

#include <cairo.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_KPLOT
#include <kplot.h>
#endif
#include <mui/widget.h>
#include <vector>
#include <map>

namespace mui
{

#ifdef HAVE_KPLOT
    /**
     * Implements a basic line chart widget.
     */
    class Chart : public Widget
    {
    public:
        Chart(const Point& point = Point(),
              const Size& size = Size());

        virtual void draw(const Rect& rect);

        virtual void data(std::vector<struct kpair> data)
        {
            if (m_data != data)
            {
                m_data = data;
                damage();
            }
        }
    protected:

        std::vector<struct kpair> m_data;
    };
#endif

    /**
     * Pie chart widget.
     */
    class PieChart : public Widget
    {
    public:
        PieChart(const Point& point = Point(),
                 const Size& size = Size());

        virtual void draw(const Rect& rect);

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
