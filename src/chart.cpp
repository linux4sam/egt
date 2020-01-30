/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "detail/charts/plplotimpl.h"
#include "egt/chart.h"

namespace egt
{
inline namespace v1
{

LineChart::LineChart(const Rect& rect)
    : Widget(rect)
{
    name("LineChart" + std::to_string(m_widgetid));

    fill_flags(Theme::FillFlag::blend);

    m_impl = detail::make_unique<detail::PlPlotLineChart>(*this, rect);
}

void LineChart::draw(Painter& painter, const Rect& rect)
{
    return m_impl->draw(painter, rect);
}

void LineChart::data(const DataArray& data)
{
    m_impl->data(data);
    damage();
}

size_t LineChart::data_size() const
{
    return m_impl->data_size();
}

void LineChart::add_data(const DataArray& data)
{
    m_impl->add_data(data);
    damage();
}

void LineChart::remove_data(uint32_t count)
{
    m_impl->remove_data(count);
    damage();
}

void LineChart::clear()
{
    m_impl->clear();
    damage();
}

void LineChart::line_width(const int val)
{
    m_impl->width(val);
    damage();
}

void LineChart::line_pattern(LinePattern pattern)
{
    m_impl->pattern(static_cast<int>(pattern));
    damage();
}

void LineChart::label(const std::string& xlabel, const std::string& ylabel, const std::string& title)
{
    m_impl->label(xlabel, ylabel, title);
    damage();
}

void LineChart::show_ticks(bool enable)
{
    m_impl->show_ticks(enable);
    damage();
}

void LineChart::show_grid(bool enable)
{
    m_impl->show_grid(enable);
    damage();
}

void LineChart::resize(const Size& size)
{
    m_impl->resize(size);
    Widget::resize(size);
}

LineChart::~LineChart() = default;

PointChart::PointChart(const Rect& rect)
    : Widget(rect)
{
    name("PointChart" + std::to_string(m_widgetid));

    fill_flags(Theme::FillFlag::blend);

    m_impl = detail::make_unique<detail::PlPlotPointChart>(*this, rect);
}

void PointChart::draw(Painter& painter, const Rect& rect)
{
    m_impl->draw(painter, rect);
}

void PointChart::data(const DataArray& data)
{
    m_impl->data(data);
    damage();
}

size_t PointChart::data_size() const
{
    return m_impl->data_size();
}

void PointChart::add_data(const DataArray& data)
{
    m_impl->add_data(data);
    damage();
}

void PointChart::remove_data(uint32_t count)
{
    m_impl->remove_data(count);
    damage();
}

void PointChart::clear()
{
    m_impl->clear();
    damage();
}

void PointChart::point_type(const PointType ptype)
{
    m_impl->point_type(static_cast<int>(ptype));
    damage();
}

void PointChart::label(const std::string& xlabel, const std::string& ylabel, const std::string& title)
{
    m_impl->label(xlabel, ylabel, title);
    damage();
}

void PointChart::show_ticks(bool enable)
{
    m_impl->show_ticks(enable);
    damage();
}

void PointChart::resize(const Size& size)
{
    m_impl->resize(size);
    Widget::resize(size);
}

void PointChart::show_grid(bool enable)
{
    m_impl->show_grid(enable);
    damage();
}

PointChart::~PointChart() = default;

BarChart::BarChart(const Rect& rect)
    : Widget(rect)
{
    name("BarChart" + std::to_string(m_widgetid));

    fill_flags(Theme::FillFlag::blend);

    m_impl = detail::make_unique<detail::PlPlotBarChart>(*this, rect);
}

void BarChart::draw(Painter& painter, const Rect& rect)
{
    m_impl->draw(painter, rect);
}

void BarChart::bar_pattern(BarPattern pattern)
{
    m_impl->pattern(static_cast<int>(pattern));
    damage();
}

void BarChart::label(const std::string& xlabel, const std::string& ylabel, const std::string& title)
{
    m_impl->label(xlabel, ylabel, title);
    damage();
}

void BarChart::data(const DataArray& data)
{
    m_impl->data(data);
    damage();
}

size_t BarChart::data_size() const
{
    return m_impl->data_size();
}

void BarChart::add_data(const DataArray& data)
{
    m_impl->add_data(data);
    damage();
}

void BarChart::data(const StringDataArray& data)
{
    m_impl->data(data);
    damage();
}

void BarChart::add_data(const StringDataArray& data)
{
    m_impl->add_data(data);
    damage();
}

void BarChart::remove_data(uint32_t count)
{
    m_impl->remove_data(count);
    damage();
}

void BarChart::clear()
{
    m_impl->clear();
    damage();
}

void BarChart::show_ticks(bool enable)
{
    m_impl->show_ticks(enable);
    damage();
}

void BarChart::show_grid(bool enable)
{
    m_impl->show_grid(enable);
    damage();
}

void BarChart::resize(const Size& size)
{
    m_impl->resize(size);
    Widget::resize(size);
}

BarChart::~BarChart() = default;

HorizontalBarChart::HorizontalBarChart(const Rect& rect)
    : Widget(rect)
{
    name("HorizontalBarChart" + std::to_string(m_widgetid));

    fill_flags(Theme::FillFlag::blend);

    m_impl = detail::make_unique<detail::PlPlotHBarChart>(*this, rect);
}

void HorizontalBarChart::draw(Painter& painter, const Rect& rect)
{
    m_impl->draw(painter, rect);
}

void HorizontalBarChart::data(const DataArray& data)
{
    m_impl->data(data);
    damage();
}

size_t HorizontalBarChart::data_size() const
{
    return m_impl->data_size();
}

void HorizontalBarChart::add_data(const DataArray& data)
{
    m_impl->add_data(data);
    damage();
}

void HorizontalBarChart::remove_data(uint32_t count)
{
    m_impl->remove_data(count);
    damage();
}

void HorizontalBarChart::clear()
{
    m_impl->clear();
    damage();
}

void HorizontalBarChart::bar_pattern(BarPattern pattern)
{
    m_impl->pattern(static_cast<int>(pattern));
    damage();
}

void HorizontalBarChart::label(const std::string& xlabel, const std::string& ylabel, const std::string& title)
{
    m_impl->label(xlabel, ylabel, title);
    damage();
}

void HorizontalBarChart::show_ticks(bool enable)
{
    m_impl->show_ticks(enable);
    damage();
}

void HorizontalBarChart::show_grid(bool enable)
{
    m_impl->show_grid(enable);
    damage();
}

void HorizontalBarChart::resize(const Size& size)
{
    m_impl->resize(size);
    Widget::resize(size);
}

HorizontalBarChart::~HorizontalBarChart() = default;

PieChart::PieChart(const Rect& rect)
    : Widget(rect)
{
    name("PieChart" + std::to_string(m_widgetid));

    fill_flags(Theme::FillFlag::blend);

    m_impl = detail::make_unique<detail::PlPlotPieChart>(*this, rect);
}

void PieChart::draw(Painter& painter, const Rect& rect)
{
    m_impl->draw(painter, rect);
}

void PieChart::title(const std::string& title)
{
    return m_impl->title(title);
    damage();
}

void PieChart::data(const DataArray& data)
{
    m_impl->data(data);
    damage();
}

size_t PieChart::data_size() const
{
    return m_impl->data_size();
}

void PieChart::add_data(const DataArray& data)
{
    m_impl->add_data(data);
    damage();
}

void PieChart::remove_data(uint32_t count)
{
    m_impl->remove_data(count);
    damage();
}

void PieChart::clear()
{
    m_impl->clear();
    damage();
}

void PieChart::resize(const Size& size)
{
    m_impl->resize(size);
    Widget::resize(size);
}

PieChart::~PieChart() = default;

}

}
