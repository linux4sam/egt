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

LineChart::LineChart(const Rect& rect)
    : ChartBase(rect),
      m_impl(std::make_unique<detail::PlPlotLineChart>(*this))
{
    name("LineChart" + std::to_string(m_widgetid));
}

void LineChart::draw(Painter& painter, const Rect& rect)
{
    return m_impl->draw(painter, rect);
}

void LineChart::data(const DataArray& data)
{
    m_impl->data(data);
}

ChartBase::DataArray LineChart::data() const
{
    return m_impl->data();
}

size_t LineChart::data_size() const
{
    return m_impl->data_size();
}

void LineChart::add_data(const DataArray& data)
{
    m_impl->add_data(data);
}

void LineChart::remove_data(uint32_t count)
{
    m_impl->remove_data(count);
}

void LineChart::clear()
{
    m_impl->clear();
}

void LineChart::grid_style(GridFlag flag)
{
    m_impl->grid_style(flag);
}

ChartBase::GridFlag LineChart::grid_style() const
{
    return static_cast<ChartBase::GridFlag>(m_impl->grid_style());
}

void LineChart::grid_width(const int val)
{
    m_impl->grid_width(val);
}

int LineChart::grid_width() const
{
    return m_impl->grid_width();
}

void LineChart::line_width(const int val)
{
    m_impl->line_width(val);
}

int LineChart::line_width() const
{
    return m_impl->line_width();
}

void LineChart::line_style(LinePattern pattern)
{
    m_impl->line_style(static_cast<int>(pattern));
}

LineChart::LinePattern LineChart::line_style() const
{
    return static_cast<LineChart::LinePattern>(m_impl->line_style());
}

void LineChart::label(const std::string& xlabel, const std::string& ylabel, const std::string& title)
{
    m_impl->label(xlabel, ylabel, title);
}

std::string LineChart::xlabel() const
{
    return m_impl->xlabel();
}

std::string LineChart::ylabel() const
{
    return m_impl->ylabel();
}

std::string LineChart::title() const
{
    return m_impl->title();
}

void LineChart::resize(const Size& size)
{
    if (size != this->size())
    {
        m_impl->resize();
        Widget::resize(size);
    }
}

void LineChart::bank(float bank)
{
    m_impl->bank(bank);
}

void LineChart::serialize(Serializer& serializer) const
{
    ChartBase::serialize(serializer);

    serializer.add_property("xlabel", xlabel());

    serializer.add_property("ylabel", ylabel());

    serializer.add_property("title", title());

    auto gflag = grid_style();
    if (gflag == egt::ChartBase::GridFlag::none)
        serializer.add_property("gridflags", "none");
    else if (gflag == egt::ChartBase::GridFlag::box)
        serializer.add_property("gridflags", "box");
    else if (gflag == egt::ChartBase::GridFlag::box_ticks)
        serializer.add_property("gridflags", "box_ticks");
    else if (gflag == egt::ChartBase::GridFlag::box_ticks_coord)
        serializer.add_property("gridflags", "box_ticks_coord");
    else if (gflag == egt::ChartBase::GridFlag::box_major_ticks_coord)
        serializer.add_property("gridflags", "box_major_ticks_coord");
    else if (gflag == egt::ChartBase::GridFlag::box_minor_ticks_coord)
        serializer.add_property("gridflags", "box_minor_ticks_coord");

    auto lp = line_style();
    if (lp == egt::LineChart::LinePattern::solid)
        serializer.add_property("linetype", "solid");
    else if (lp == egt::LineChart::LinePattern::dotted)
        serializer.add_property("linetype", "dotted");
    else if (lp == egt::LineChart::LinePattern::dashes)
        serializer.add_property("linetype", "dashes");

    serializer.add_property("linewidth", line_width());

    serializer.add_property("gridwidth", grid_width());

    ChartBase::DataArray items = data();
    if (!items.empty())
    {
        for (auto& elem : items)
        {
            auto value = fmt::format("{},{}", detail::to_string(elem.first), detail::to_string(elem.second));
            serializer.add_property("item", value);
        }
    }
}

void LineChart::deserialize(const std::string& name, const std::string& value,
                            const Serializer::Attributes& attrs)
{
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
            egt::ChartBase::DataArray data_item;
            data_item.push_back(std::make_pair(std::stod(tokens[0]), std::stod(tokens[1])));
            add_data(data_item);
        }
        else
        {
            egt::detail::warn("unhandled property {} values", name, value);
        }
        break;
    }
    case detail::hash("gridflags"):
    {
        if (value == "none")
            grid_style(egt::ChartBase::GridFlag::none);
        else if (value == "box")
            grid_style(egt::ChartBase::GridFlag::box);
        else if (value == "box_ticks")
            grid_style(egt::ChartBase::GridFlag::box_ticks);
        else if (value == "box_ticks_coord")
            grid_style(egt::ChartBase::GridFlag::box_ticks_coord);
        else if (value == "box_major_ticks_coord")
            grid_style(egt::ChartBase::GridFlag::box_major_ticks_coord);
        else if (value == "box_minor_ticks_coord")
            grid_style(egt::ChartBase::GridFlag::box_minor_ticks_coord);
        else
            egt::detail::warn("unhandled property {}", name);

        break;
    }
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
    case detail::hash("gridwidth"):
    {
        grid_width(std::stoi(value));
        break;
    }
    default:
        ChartBase::deserialize(name, value, attrs);
        break;
    }
}

LineChart::LineChart(LineChart&&) noexcept = default;
LineChart& LineChart::operator=(LineChart&&) noexcept = default;

LineChart::~LineChart() = default;

PointChart::PointChart(const Rect& rect)
    : ChartBase(rect),
      m_impl(std::make_unique<detail::PlPlotPointChart>(*this))
{
    name("PointChart" + std::to_string(m_widgetid));
}

void PointChart::draw(Painter& painter, const Rect& rect)
{
    m_impl->draw(painter, rect);
}

void PointChart::data(const DataArray& data)
{
    m_impl->data(data);
}

ChartBase::DataArray PointChart::data() const
{
    return m_impl->data();
}

size_t PointChart::data_size() const
{
    return m_impl->data_size();
}

void PointChart::add_data(const DataArray& data)
{
    m_impl->add_data(data);
}

void PointChart::remove_data(uint32_t count)
{
    m_impl->remove_data(count);
}

void PointChart::clear()
{
    m_impl->clear();
}

void PointChart::point_type(const PointType ptype)
{
    m_impl->point_type(static_cast<int>(ptype));
}

PointChart::PointType PointChart::point_type() const
{
    return static_cast<PointChart::PointType>(m_impl->point_type());
}

void PointChart::label(const std::string& xlabel, const std::string& ylabel, const std::string& title)
{
    m_impl->label(xlabel, ylabel, title);
}

std::string PointChart::xlabel() const
{
    return m_impl->xlabel();
}

std::string PointChart::ylabel() const
{
    return m_impl->ylabel();
}

std::string PointChart::title() const
{
    return m_impl->title();
}

void PointChart::resize(const Size& size)
{
    if (size != this->size())
    {
        m_impl->resize();
        Widget::resize(size);
    }
}

void PointChart::bank(float bank)
{
    m_impl->bank(bank);
}

void PointChart::grid_style(GridFlag flag)
{
    m_impl->grid_style(flag);
}

ChartBase::GridFlag PointChart::grid_style() const
{
    return static_cast<ChartBase::GridFlag>(m_impl->grid_style());
}

void PointChart::grid_width(const int val)
{
    m_impl->grid_width(val);
}

int PointChart::grid_width() const
{
    return m_impl->grid_width();
}

void PointChart::serialize(Serializer& serializer) const
{
    ChartBase::serialize(serializer);

    serializer.add_property("xlabel", xlabel());

    serializer.add_property("ylabel", ylabel());

    serializer.add_property("title", title());

    auto gflag = grid_style();
    if (gflag == egt::ChartBase::GridFlag::none)
        serializer.add_property("gridflags", "none");
    else if (gflag == egt::ChartBase::GridFlag::box)
        serializer.add_property("gridflags", "box");
    else if (gflag == egt::ChartBase::GridFlag::box_ticks)
        serializer.add_property("gridflags", "box_ticks");
    else if (gflag == egt::ChartBase::GridFlag::box_ticks_coord)
        serializer.add_property("gridflags", "box_ticks_coord");
    else if (gflag == egt::ChartBase::GridFlag::box_major_ticks_coord)
        serializer.add_property("gridflags", "box_major_ticks_coord");
    else if (gflag == egt::ChartBase::GridFlag::box_minor_ticks_coord)
        serializer.add_property("gridflags", "box_minor_ticks_coord");

    auto pt = point_type();
    if (pt == egt::PointChart::PointType::star)
        serializer.add_property("linetype", "star");
    else if (pt == egt::PointChart::PointType::dot)
        serializer.add_property("linetype", "dot");
    else if (pt == egt::PointChart::PointType::cross)
        serializer.add_property("linetype", "cross");
    else if (pt == egt::PointChart::PointType::circle)
        serializer.add_property("linetype", "circle");

    serializer.add_property("gridwidth", grid_width());

    ChartBase::DataArray items = data();
    if (!items.empty())
    {
        for (auto& elem : items)
        {
            auto value = fmt::format("{},{}", detail::to_string(elem.first), detail::to_string(elem.second));
            serializer.add_property("item", value);
        }
    }
}

void PointChart::deserialize(const std::string& name, const std::string& value,
                             const Serializer::Attributes& attrs)
{
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
            egt::ChartBase::DataArray data_item;
            data_item.push_back(std::make_pair(std::stod(tokens[0]), std::stod(tokens[1])));
            add_data(data_item);
        }
        else
        {
            egt::detail::warn("unhandled property {} values", name, value);
        }
        break;
    }
    case detail::hash("gridflags"):
    {
        if (value == "none")
            grid_style(egt::ChartBase::GridFlag::none);
        else if (value == "box")
            grid_style(egt::ChartBase::GridFlag::box);
        else if (value == "box_ticks")
            grid_style(egt::ChartBase::GridFlag::box_ticks);
        else if (value == "box_ticks_coord")
            grid_style(egt::ChartBase::GridFlag::box_ticks_coord);
        else if (value == "box_major_ticks_coord")
            grid_style(egt::ChartBase::GridFlag::box_major_ticks_coord);
        else if (value == "box_minor_ticks_coord")
            grid_style(egt::ChartBase::GridFlag::box_minor_ticks_coord);
        else
            egt::detail::warn("unhandled property {}", name);

        break;
    }
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
    case detail::hash("gridwidth"):
    {
        grid_width(std::stoi(value));
        break;
    }
    default:
        ChartBase::deserialize(name, value, attrs);
        break;
    }
}

PointChart::PointChart(PointChart&&) noexcept = default;
PointChart& PointChart::operator=(PointChart&&) noexcept = default;
PointChart::~PointChart() = default;

BarChart::BarChart(const Rect& rect)
    : ChartBase(rect),
      m_impl(std::make_unique<detail::PlPlotBarChart>(*this))
{
    name("BarChart" + std::to_string(m_widgetid));
}

BarChart::BarChart(const Rect& rect, std::unique_ptr<detail::PlPlotImpl>&& impl)
    : ChartBase(rect),
      m_impl(std::move(impl))
{
    name("BarChart" + std::to_string(m_widgetid));
}

void BarChart::draw(Painter& painter, const Rect& rect)
{
    m_impl->draw(painter, rect);
}

void BarChart::bar_style(BarPattern pattern)
{
    m_impl->line_style(static_cast<int>(pattern));
}

BarChart::BarPattern BarChart::bar_style() const
{
    return static_cast<BarChart::BarPattern>(m_impl->line_style());
}

void BarChart::label(const std::string& xlabel, const std::string& ylabel, const std::string& title)
{
    m_impl->label(xlabel, ylabel, title);
}

std::string BarChart::xlabel() const
{
    return m_impl->xlabel();
}

std::string BarChart::ylabel() const
{
    return m_impl->ylabel();
}

std::string BarChart::title() const
{
    return m_impl->title();
}

void BarChart::data(const DataArray& data)
{
    m_impl->data(data);
}

ChartBase::DataArray BarChart::data() const
{
    return m_impl->data();
}

size_t BarChart::data_size() const
{
    return m_impl->data_size();
}

void BarChart::add_data(const DataArray& data)
{
    m_impl->add_data(data);
}

void BarChart::data(const StringDataArray& data)
{
    m_impl->data(data);
}

ChartBase::StringDataArray BarChart::sdata() const
{
    return m_impl->sdata();
}

void BarChart::add_data(const StringDataArray& data)
{
    m_impl->add_data(data);
}

void BarChart::remove_data(uint32_t count)
{
    m_impl->remove_data(count);
}

void BarChart::clear()
{
    m_impl->clear();
}

void BarChart::grid_style(GridFlag flag)
{
    m_impl->grid_style(flag);
}

ChartBase::GridFlag BarChart::grid_style() const
{
    return static_cast<ChartBase::GridFlag>(m_impl->grid_style());
}

void BarChart::grid_width(const int val)
{
    m_impl->grid_width(val);
}

int BarChart::grid_width() const
{
    return m_impl->grid_width();
}

void BarChart::resize(const Size& size)
{
    if (size != this->size())
    {
        m_impl->resize();
        Widget::resize(size);
    }
}

void BarChart::bank(float bank)
{
    m_impl->bank(bank);
}


void BarChart::serialize(Serializer& serializer) const
{
    ChartBase::serialize(serializer);

    serializer.add_property("xlabel", xlabel());

    serializer.add_property("ylabel", ylabel());

    serializer.add_property("title", title());

    auto gflag = grid_style();
    if (gflag == egt::ChartBase::GridFlag::none)
        serializer.add_property("gridflags", "none");
    else if (gflag == egt::ChartBase::GridFlag::box)
        serializer.add_property("gridflags", "box");
    else if (gflag == egt::ChartBase::GridFlag::box_ticks)
        serializer.add_property("gridflags", "box_ticks");
    else if (gflag == egt::ChartBase::GridFlag::box_ticks_coord)
        serializer.add_property("gridflags", "box_ticks_coord");
    else if (gflag == egt::ChartBase::GridFlag::box_major_ticks_coord)
        serializer.add_property("gridflags", "box_major_ticks_coord");
    else if (gflag == egt::ChartBase::GridFlag::box_minor_ticks_coord)
        serializer.add_property("gridflags", "box_minor_ticks_coord");

    auto pt = bar_style();
    if (pt == egt::BarChart::BarPattern::solid)
        serializer.add_property("bartype", "solid");
    else if (pt == egt::BarChart::BarPattern::horizontal_line)
        serializer.add_property("bartype", "horizontal_line");
    else if (pt == egt::BarChart::BarPattern::vertical_line)
        serializer.add_property("bartype", "vertical_line");
    else if (pt == egt::BarChart::BarPattern::boxes)
        serializer.add_property("bartype", "boxes");

    serializer.add_property("gridwidth", grid_width());

    ChartBase::DataArray items = data();
    if (!items.empty())
    {
        for (auto& elem : items)
        {
            auto value = fmt::format("{},{}", detail::to_string(elem.first), detail::to_string(elem.second));
            serializer.add_property("item", value);
        }
    }
    else
    {
        ChartBase::StringDataArray sitems = sdata();
        if (!sitems.empty())
        {
            for (auto& selem : sitems)
            {
                auto value = fmt::format("{},{}", detail::to_string(selem.first), detail::to_string(selem.second));
                serializer.add_property("item", value);
            }
        }
    }
}

void BarChart::deserialize(const std::string& name, const std::string& value,
                           const Serializer::Attributes& attrs)
{
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
            try
            {
                egt::ChartBase::DataArray data_item;
                data_item.push_back(std::make_pair(std::stod(tokens[0]), std::stod(tokens[1])));
                add_data(data_item);
            }
            catch (const std::exception& e)
            {
                ChartBase::StringDataArray sdata_item;
                sdata_item.push_back(std::make_pair(std::stod(tokens[0]), tokens[1]));
                add_data(sdata_item);
            }
        }
        else
        {
            egt::detail::warn("unhandled property {} values", name, value);
        }
        break;
    }
    case detail::hash("gridflags"):
    {
        if (value == "none")
            grid_style(egt::ChartBase::GridFlag::none);
        else if (value == "box")
            grid_style(egt::ChartBase::GridFlag::box);
        else if (value == "box_ticks")
            grid_style(egt::ChartBase::GridFlag::box_ticks);
        else if (value == "box_ticks_coord")
            grid_style(egt::ChartBase::GridFlag::box_ticks_coord);
        else if (value == "box_major_ticks_coord")
            grid_style(egt::ChartBase::GridFlag::box_major_ticks_coord);
        else if (value == "box_minor_ticks_coord")
            grid_style(egt::ChartBase::GridFlag::box_minor_ticks_coord);
        else
            egt::detail::warn("unhandled property {}", name);

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
    case detail::hash("gridwidth"):
    {
        grid_width(std::stoi(value));
        break;
    }
    default:
        ChartBase::deserialize(name, value, attrs);
        break;
    }
}

BarChart::BarChart(BarChart&&) noexcept = default;
BarChart& BarChart::operator=(BarChart&&) noexcept = default;
BarChart::~BarChart() = default;

HorizontalBarChart::HorizontalBarChart(const Rect& rect)
    : BarChart(rect, std::make_unique<detail::PlPlotHBarChart>(*this))
{
    name("HorizontalBarChart" + std::to_string(m_widgetid));
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

void PieChart::data(const StringDataArray& data)
{
    m_impl->data(data);
}

PieChart::StringDataArray PieChart::sdata() const
{
    return m_impl->sdata();
}

size_t PieChart::data_size() const
{
    return m_impl->data_size();
}

void PieChart::add_data(const StringDataArray& data)
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

PieChart::PieChart(PieChart&&) noexcept = default;
PieChart& PieChart::operator=(PieChart&&) noexcept = default;

PieChart::~PieChart() = default;

}
}
