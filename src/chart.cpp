/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "detail/charts/plplotimpl.h"
#include "detail/egtlog.h"
#include "egt/chart.h"

namespace egt
{
inline namespace v1
{

void ChartBase::draw(Painter& painter, const Rect& rect)
{
    m_impl->draw(painter, rect);
}

void ChartBase::data(const ChartItemArray& data)
{
    m_impl->data(data);
}

void ChartBase::add_data(const ChartItemArray& data)
{
    m_impl->add_data(data);
}

ChartItemArray ChartBase::data() const
{
    return m_impl->data();
}

size_t ChartBase::data_size() const
{
    return m_impl->data_size();
}

void ChartBase::remove_data(uint32_t count)
{
    m_impl->remove_data(count);
}

void ChartBase::clear()
{
    m_impl->clear();
}

void ChartBase::grid_style(ChartBase::GridFlag flag)
{
    m_impl->grid_style(flag);
}

ChartBase::GridFlag ChartBase::grid_style() const
{
    return static_cast<ChartBase::GridFlag>(m_impl->grid_style());
}

void ChartBase::grid_width(const int val)
{
    m_impl->grid_width(val);
}

int ChartBase::grid_width() const
{
    return m_impl->grid_width();
}

void ChartBase::label(const std::string& xlabel, const std::string& ylabel, const std::string& title)
{
    m_impl->label(xlabel, ylabel, title);
}

std::string ChartBase::xlabel() const
{
    return m_impl->xlabel();
}

std::string ChartBase::ylabel() const
{
    return m_impl->ylabel();
}

std::string ChartBase::title() const
{
    return m_impl->title();
}

void ChartBase::xlabel(const std::string& xlabel)
{
    m_impl->label(xlabel, ylabel(), title());
}

void ChartBase::ylabel(const std::string& ylabel)
{
    m_impl->label(xlabel(), ylabel, title());
}

void ChartBase::title(const std::string& title)
{
    m_impl->label(xlabel(), ylabel(), title);
}

void ChartBase::resize(const Size& size)
{
    if (size != this->size())
    {
        m_impl->resize();
        Widget::resize(size);
    }
}

void ChartBase::bank(float bank)
{
    m_impl->bank(bank);
}

float ChartBase::bank() const
{
    return m_impl->bank();
}

void ChartBase::serialize(Serializer& serializer) const
{
    Widget::serialize(serializer);

    serializer.add_property("xlabel", xlabel());

    serializer.add_property("ylabel", ylabel());

    serializer.add_property("title", title());

    auto gflag = grid_style();
    if (gflag == GridFlag::none)
        serializer.add_property("gridflags", "none");
    else if (gflag == GridFlag::box)
        serializer.add_property("gridflags", "box");
    else if (gflag == GridFlag::box_ticks)
        serializer.add_property("gridflags", "box_ticks");
    else if (gflag == GridFlag::box_ticks_coord)
        serializer.add_property("gridflags", "box_ticks_coord");
    else if (gflag == GridFlag::box_major_ticks_coord)
        serializer.add_property("gridflags", "box_major_ticks_coord");
    else if (gflag == GridFlag::box_minor_ticks_coord)
        serializer.add_property("gridflags", "box_minor_ticks_coord");

    serializer.add_property("gridwidth", grid_width());

    ChartItemArray items = data();
    if (!items.get_data().empty())
    {
        for (auto& elem : items.get_data())
        {
            auto value = fmt::format("{},{}", detail::to_string(elem.first), detail::to_string(elem.second));
            serializer.add_property("item", value);
        }
    }
}

void ChartBase::deserialize(Serializer::Properties& props)
{
    props.erase(std::remove_if(props.begin(), props.end(), [&](auto & p)
    {
        auto name = std::get<0>(p);
        auto value = std::get<1>(p);

        switch (detail::hash(name))
        {
        case detail::hash("xlabel"):
        {
            label(value, ylabel(), title());
            break;
        }
        case detail::hash("ylabel"):
        {
            label(xlabel(), value, title());
            break;
        }
        case detail::hash("title"):
        {
            label(xlabel(), ylabel(), value);
            break;
        }
        case detail::hash("item"):
        {
            std::vector<std::string> tokens;
            detail::tokenize(value, ',', tokens);
            if (tokens.size() == 2)
            {
                ChartItemArray data_item;
                data_item.add(std::stod(tokens[0]), std::stod(tokens[1]));
                add_data(data_item);
            }
            else
            {
                egt::detail::warn("unhandled property {} : values {}", name, value);
            }
            break;
        }
        case detail::hash("gridflags"):
        {
            if (value == "none")
                grid_style(GridFlag::none);
            else if (value == "box")
                grid_style(GridFlag::box);
            else if (value == "box_ticks")
                grid_style(GridFlag::box_ticks);
            else if (value == "box_ticks_coord")
                grid_style(GridFlag::box_ticks_coord);
            else if (value == "box_major_ticks_coord")
                grid_style(GridFlag::box_major_ticks_coord);
            else if (value == "box_minor_ticks_coord")
                grid_style(GridFlag::box_minor_ticks_coord);
            else
                egt::detail::warn("unhandled property {}", name);

            break;
        }
        case detail::hash("gridwidth"):
        {
            grid_width(std::stoi(value));
            break;
        }
        default:
            return false;
        }
        return true;
    }), props.end());
}

LineChart::LineChart(const Rect& rect)
    : ChartBase(rect)
{
    name("LineChart" + std::to_string(m_widgetid));

    create_impl();
}

LineChart::LineChart(Serializer::Properties& props, bool is_derived)
    : ChartBase(props, true)
{
    name("LineChart" + std::to_string(m_widgetid));

    create_impl();

    deserialize(props);

    if (!is_derived)
        deserialize_leaf(props);
}

void LineChart::create_impl()
{
    m_impl = std::make_unique<detail::PlPlotLineChart>(*this);
}

void LineChart::line_width(const int val)
{
    m_impl->line_width(val);
}

int LineChart::line_width() const
{
    return m_impl->line_width();
}

void LineChart::line_style(LineChart::LinePattern pattern)
{
    m_impl->line_style(static_cast<int>(pattern));
}

LineChart::LinePattern LineChart::line_style() const
{
    return static_cast<LineChart::LinePattern>(m_impl->line_style());
}

void LineChart::serialize(Serializer& serializer) const
{
    ChartBase::serialize(serializer);

    auto lp = line_style();
    if (lp == egt::LineChart::LinePattern::solid)
        serializer.add_property("linetype", "solid");
    else if (lp == egt::LineChart::LinePattern::dotted)
        serializer.add_property("linetype", "dotted");
    else if (lp == egt::LineChart::LinePattern::dashes)
        serializer.add_property("linetype", "dashes");

    serializer.add_property("linewidth", line_width());
}

void LineChart::deserialize(Serializer::Properties& props)
{
    props.erase(std::remove_if(props.begin(), props.end(), [&](auto & p)
    {
        auto name = std::get<0>(p);
        auto value = std::get<1>(p);

        switch (detail::hash(name))
        {
        case detail::hash("linetype"):
        {
            if (value == "solid")
                line_style(egt::LineChart::LinePattern::solid);
            else if (value == "dotted")
                line_style(egt::LineChart::LinePattern::dotted);
            else if (value == "dashes")
                line_style(egt::LineChart::LinePattern::dashes);
            else
                egt::detail::warn("unhandled property {}", name);
            break;
        }
        case detail::hash("linewidth"):
        {
            line_width(std::stoi(value));
            break;
        }
        default:
            return false;
        }
        return true;
    }), props.end());

    ChartBase::deserialize(props);
}

LineChart::LineChart(LineChart&&) noexcept = default;
LineChart& LineChart::operator=(LineChart&&) noexcept = default;
LineChart::~LineChart() = default;


PointChart::PointChart(const Rect& rect)
    : ChartBase(rect)
{
    name("PointChart" + std::to_string(m_widgetid));

    create_impl();
}

PointChart::PointChart(Serializer::Properties& props, bool is_derived)
    : ChartBase(props, true)
{
    name("PointChart" + std::to_string(m_widgetid));

    create_impl();

    deserialize(props);

    if (!is_derived)
        deserialize_leaf(props);
}

void PointChart::create_impl()
{
    m_impl = std::make_unique<detail::PlPlotPointChart>(*this);
}

void PointChart::point_type(const PointType ptype)
{
    m_impl->point_type(static_cast<int>(ptype));
}

PointChart::PointType PointChart::point_type() const
{
    return static_cast<PointChart::PointType>(m_impl->point_type());
}

void PointChart::serialize(Serializer& serializer) const
{
    ChartBase::serialize(serializer);

    auto pt = point_type();
    if (pt == egt::PointChart::PointType::star)
        serializer.add_property("pointtype", "star");
    else if (pt == egt::PointChart::PointType::dot)
        serializer.add_property("pointtype", "dot");
    else if (pt == egt::PointChart::PointType::cross)
        serializer.add_property("pointtype", "cross");
    else if (pt == egt::PointChart::PointType::circle)
        serializer.add_property("pointtype", "circle");
}

void PointChart::deserialize(Serializer::Properties& props)
{
    props.erase(std::remove_if(props.begin(), props.end(), [&](auto & p)
    {
        auto name = std::get<0>(p);
        auto value = std::get<1>(p);

        switch (detail::hash(name))
        {
        case detail::hash("pointtype"):
        {
            if (value == "star")
                point_type(egt::PointChart::PointType::star);
            else if (value == "dot")
                point_type(egt::PointChart::PointType::dot);
            else if (value == "cross")
                point_type(egt::PointChart::PointType::cross);
            else if (value == "circle")
                point_type(egt::PointChart::PointType::circle);
            else
                egt::detail::warn("unhandled property {}", name);

            break;
        }
        default:
            return false;
        }
        return true;
    }), props.end());

    ChartBase::deserialize(props);
}

PointChart::PointChart(PointChart&&) noexcept = default;
PointChart& PointChart::operator=(PointChart&&) noexcept = default;
PointChart::~PointChart() = default;

BarChart::BarChart(const Rect& rect)
    : ChartBase(rect)
{
    name("BarChart" + std::to_string(m_widgetid));

    create_impl();
}

BarChart::BarChart(Serializer::Properties& props, bool is_derived)
    : ChartBase(props, true)
{
    name("BarChart" + std::to_string(m_widgetid));

    create_impl();

    deserialize(props);

    if (!is_derived)
        deserialize_leaf(props);
}

BarChart::BarChart(const Rect& rect, std::unique_ptr<detail::PlPlotImpl>&& impl)
    : ChartBase(rect)
{
    name("BarChart" + std::to_string(m_widgetid));

    m_impl = std::move(impl);
}

BarChart::BarChart(Serializer::Properties& props, std::unique_ptr<detail::PlPlotImpl>&& impl)
    : ChartBase(props, true)
{
    name("BarChart" + std::to_string(m_widgetid));

    m_impl = std::move(impl);

    deserialize(props);
}

void BarChart::create_impl()
{
    m_impl = std::make_unique<detail::PlPlotBarChart>(*this);
}

void BarChart::bar_style(BarPattern pattern)
{
    m_impl->line_style(static_cast<int>(pattern));
}

BarChart::BarPattern BarChart::bar_style() const
{
    return static_cast<BarChart::BarPattern>(m_impl->line_style());
}

void BarChart::serialize(Serializer& serializer) const
{
    ChartBase::serialize(serializer);

    auto pt = bar_style();
    if (pt == egt::BarChart::BarPattern::solid)
        serializer.add_property("bartype", "solid");
    else if (pt == egt::BarChart::BarPattern::horizontal_line)
        serializer.add_property("bartype", "horizontal_line");
    else if (pt == egt::BarChart::BarPattern::vertical_line)
        serializer.add_property("bartype", "vertical_line");
    else if (pt == egt::BarChart::BarPattern::boxes)
        serializer.add_property("bartype", "boxes");

    ChartItemArray sitems = data();
    if (!sitems.get_sdata().empty())
    {
        for (auto& selem : sitems.get_sdata())
        {
            auto value = fmt::format("{},{}", detail::to_string(selem.first), detail::to_string(selem.second));
            serializer.add_property("item", value);
        }
    }
}

void BarChart::deserialize(Serializer::Properties& props)
{
    props.erase(std::remove_if(props.begin(), props.end(), [&](auto & p)
    {
        auto name = std::get<0>(p);
        auto value = std::get<1>(p);

        switch (detail::hash(name))
        {
        case detail::hash("item"):
        {
            ChartItemArray data;
            std::vector<std::string> tokens;
            detail::tokenize(value, ',', tokens);
            if (tokens.size() == 2)
            {
                try
                {
                    data.add(std::stod(tokens[0]), std::stod(tokens[1]));
                    add_data(data);
                }
                catch (const std::exception& e)
                {
                    data.add(std::stod(tokens[0]), tokens[1]);
                    add_data(data);
                }
            }
            else
            {
                egt::detail::warn("unhandled property {} : values {}", name, value);
            }
            break;
        }
        case detail::hash("bartype"):
        {
            if (value == "solid")
                bar_style(egt::BarChart::BarPattern::solid);
            else if (value == "horizontal_line")
                bar_style(egt::BarChart::BarPattern::horizontal_line);
            else if (value == "vertical_line")
                bar_style(egt::BarChart::BarPattern::vertical_line);
            else if (value == "boxes")
                bar_style(egt::BarChart::BarPattern::boxes);
            else
                egt::detail::warn("unhandled property {}", name);

            break;
        }
        default:
            return false;
        }
        return true;
    }), props.end());

    ChartBase::deserialize(props);
}

BarChart::BarChart(BarChart&&) noexcept = default;
BarChart& BarChart::operator=(BarChart&&) noexcept = default;
BarChart::~BarChart() = default;

HorizontalBarChart::HorizontalBarChart(const Rect& rect)
    : BarChart(rect, std::make_unique<detail::PlPlotHBarChart>(*this))
{
    name("HorizontalBarChart" + std::to_string(m_widgetid));
}

HorizontalBarChart::HorizontalBarChart(Serializer::Properties& props, bool is_derived)
    : BarChart(props, std::make_unique<detail::PlPlotHBarChart>(*this))
{
    name("HorizontalBarChart" + std::to_string(m_widgetid));

    if (!is_derived)
        deserialize_leaf(props);
}

HorizontalBarChart::HorizontalBarChart(HorizontalBarChart&&) noexcept = default;
HorizontalBarChart& HorizontalBarChart::operator=(HorizontalBarChart&&) noexcept = default;

HorizontalBarChart::~HorizontalBarChart() = default;

PieChart::PieChart(const Rect& rect)
    : Widget(rect),
      m_impl(std::make_unique<detail::PlPlotPieChart>(*this))
{
    name("PieChart" + std::to_string(m_widgetid));
}

PieChart::PieChart(Serializer::Properties& props, bool is_derived)
    : Widget(props, true),
      m_impl(std::make_unique<detail::PlPlotPieChart>(*this))
{
    name("PieChart" + std::to_string(m_widgetid));

    deserialize(props);

    if (!is_derived)
        deserialize_leaf(props);
}

void PieChart::draw(Painter& painter, const Rect& rect)
{
    m_impl->draw(painter, rect);
}

void PieChart::title(const std::string& title)
{
    return m_impl->title(title);
}

std::string PieChart::title() const
{
    return m_impl->title();
}

void PieChart::data(const ChartItemArray& data)
{
    m_impl->data(data);
}

ChartItemArray PieChart::data() const
{
    return m_impl->data();
}

size_t PieChart::data_size() const
{
    return m_impl->data_size();
}

void PieChart::add_data(const ChartItemArray& data)
{
    m_impl->add_data(data);
}

void PieChart::remove_data(uint32_t count)
{
    m_impl->remove_data(count);
}

void PieChart::clear()
{
    m_impl->clear();
}

void PieChart::resize(const Size& size)
{
    if (size != this->size())
    {
        m_impl->resize();
        Widget::resize(size);
    }
}

void PieChart::serialize(Serializer& serializer) const
{
    Widget::serialize(serializer);

    serializer.add_property("title", title());

    ChartItemArray items = data();
    if (!items.get_sdata().empty())
    {
        for (auto& elem : items.get_sdata())
        {
            auto value = fmt::format("{},{}", detail::to_string(elem.first), detail::to_string(elem.second));
            serializer.add_property("item", value);
        }
    }
}

void PieChart::deserialize(Serializer::Properties& props)
{
    props.erase(std::remove_if(props.begin(), props.end(), [&](auto & p)
    {
        auto name = std::get<0>(p);
        auto value = std::get<1>(p);

        switch (detail::hash(name))
        {
        case detail::hash("title"):
        {
            title(value);
            break;
        }
        case detail::hash("item"):
        {
            std::vector<std::string> tokens;
            detail::tokenize(value, ',', tokens);
            if (tokens.size() == 2)
            {
                ChartItemArray data_item;
                data_item.add(std::stod(tokens[0]), tokens[1]);
                add_data(data_item);
            }
            else
            {
                egt::detail::warn("unhandled property {} : values {}", name, value);
            }
            break;
        }
        default:
            return false;
        }
        return true;
    }), props.end());
}

PieChart::PieChart(PieChart&&) noexcept = default;
PieChart& PieChart::operator=(PieChart&&) noexcept = default;

PieChart::~PieChart() = default;

}
}
