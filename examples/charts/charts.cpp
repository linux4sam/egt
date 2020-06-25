/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <cmath>
#include <egt/themes/coconut.h>
#include <egt/themes/lapis.h>
#include <egt/themes/midnight.h>
#include <egt/themes/shamrock.h>
#include <egt/themes/sky.h>
#include <egt/themes/ultraviolet.h>
#include <egt/ui>
#include <random>
#include <string>

static int random_item(int start, int end)
{
    std::random_device r;
    std::default_random_engine e {r()};
    std::uniform_int_distribution<int> dist(start, end);
    return dist(e);
}

static std::shared_ptr<egt::ComboBox> create_grid_combo(const std::shared_ptr<egt::ChartBase>& chart)
{
    static const std::pair<const char*, egt::ChartBase::GridFlag> combo_items[] =
    {
        {"None", egt::ChartBase::GridFlag::none },
        {"Box", egt::ChartBase::GridFlag::box },
        {"Ticks", egt::ChartBase::GridFlag::box_ticks },
        {"Coordinates", egt::ChartBase::GridFlag::box_ticks_coord },
        {"Major Ticks", egt::ChartBase::GridFlag::box_major_ticks_coord },
        {"Minor Ticks", egt::ChartBase::GridFlag::box_minor_ticks_coord },
    };

    auto combo = std::make_shared<egt::ComboBox>();
    for (auto& i : combo_items)
        combo->add_item(i.first);
    combo->margin(5);

    combo->on_selected_changed([combo, chart]()
    {
        auto s = combo->item_at(combo->selected());
        for (auto& i : combo_items)
        {
            if (s == i.first)
            {
                chart->grid_style(i.second);
                break;
            }
        }
    });
    combo->selected(1);

    return combo;
}

struct PiePage : public egt::NotebookTab
{
    PiePage()
    {
        auto sizer = std::make_shared<egt::VerticalBoxSizer>();
        add(expand(sizer));

        auto pie = std::make_shared<egt::PieChart>();
        pie->title("Pie Chart");

        egt::PieChart::StringDataArray data;
        const int pdata[] = { 5, 10, 15, 20, 4, 8, 16, 12};
        size_t count = 0;
        for (auto& i : pdata)
        {
            data.push_back(make_pair(i, ("label" + std::to_string(count++))));
        }
        pie->data(data);
        sizer->add(egt::expand(pie));

        auto csizer = std::make_shared<egt::HorizontalBoxSizer>();
        sizer->add(expand_horizontal(csizer));

        auto btn1 = std::make_shared<egt::Button>("Add Data");
        csizer->add(btn1);
        btn1->on_click([pie, btn1](egt::Event&)
        {
            if (btn1->text() == "Add Data")
            {
                egt::PieChart::StringDataArray data1;
                static size_t i = 1;
                data1.push_back(std::make_pair(random_item(1, 25), "label" + std::to_string(++i)));
                data1.push_back(std::make_pair(random_item(1, 10), "label" + std::to_string(++i)));
                pie->add_data(data1);
                btn1->text("Remove Data");
            }
            else
            {
                pie->remove_data(2);
                btn1->text("Add Data");
            }
        });
    }
};

struct Points : public egt::NotebookTab
{
    explicit Points()
    {
        auto sizer = std::make_shared<egt::VerticalBoxSizer>();
        add(expand(sizer));

        auto line = std::make_shared<egt::PointChart>();
        line->label("x-axis", "y-axis", "Point Chart");
        sizer->add(expand(line));

        auto csizer = std::make_shared<egt::HorizontalBoxSizer>();
        sizer->add(expand_horizontal(csizer));

        egt::PointChart::DataArray data;
        for (int i = 0; i < 101; i++)
        {
            data.push_back(std::make_pair(i, random_item(1, 100)));
        }
        line->data(data);

        csizer->add(create_grid_combo(line));

        static const std::pair<std::string, egt::PointChart::PointType> point_items[] =
        {
            {"PointType: Dot", egt::PointChart::PointType::dot},
            {"PointType: Star", egt::PointChart::PointType::star },
            {"PointType: Circle", egt::PointChart::PointType::circle },
            {"PointType: Cross", egt::PointChart::PointType::cross },
        };

        auto combo = std::make_shared<egt::ComboBox>();
        for (auto& i : point_items)
            combo->add_item(i.first);
        combo->margin(5);
        csizer->add(combo);

        combo->on_selected_changed([combo, line]()
        {
            auto s = combo->item_at(combo->selected());
            for (auto& i : point_items)
            {
                if (s == i.first)
                {
                    line->point_type(i.second);
                    break;
                }
            }
        });
        combo->selected(0);

        auto btn2 = std::make_shared<egt::Button>("Add Data");
        csizer->add(btn2);
        btn2->on_click([line, btn2](egt::Event&)
        {
            if (btn2->text() == "Add Data")
            {
                egt::PointChart::DataArray data1;
                static int n = 101;
                for (int i = n ; i < (n + 10); i++)
                {
                    data1.push_back(std::make_pair(i, random_item(1, 125)));
                }
                line->add_data(data1);
                n += 10;
                btn2->text("Remove Data");
            }
            else
            {
                line->remove_data(15);
                btn2->text("Add Data");
            }
        });
    }
};

static std::shared_ptr<egt::ComboBox> create_bar_pattern(const std::shared_ptr<egt::BarChart>& chart)
{
    static const std::pair<std::string, egt::BarChart::BarPattern> bar_pattern[] =
    {
        {"BarPattern: Solid", egt::BarChart::BarPattern::solid},
        {"BarPattern: Horizontal", egt::BarChart::BarPattern::horizontal_line},
        {"BarPattern: Vertical", egt::BarChart::BarPattern::vertical_line},
        {"BarPattern: Boxes", egt::BarChart::BarPattern::boxes},
    };

    auto combo = std::make_shared<egt::ComboBox>();
    for (auto& i : bar_pattern)
        combo->add_item(i.first);
    combo->margin(5);

    combo->on_selected_changed([combo, chart]()
    {
        auto s = combo->item_at(combo->selected());
        for (auto& i : bar_pattern)
        {
            if (s == i.first)
            {
                chart->bar_style(i.second);
                break;
            }
        }
    });
    combo->selected(0);

    return combo;
}

struct HorizontalBarPage : public egt::NotebookTab
{
    explicit HorizontalBarPage(bool sdata)
    {
        auto sizer = std::make_shared<egt::VerticalBoxSizer>();
        add(expand(sizer));

        auto bar = std::make_shared<egt::HorizontalBarChart>();
        sizer->add(expand(bar));

        const char* months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

        if (sdata)
        {
            bar->label("Sales", "Months", "Horizontal BarChart-2");
            egt::HorizontalBarChart::StringDataArray str_data;
            for (const auto& i : months)
            {
                str_data.push_back(std::make_pair(random_item(1, 50), i));
            }
            bar->data(str_data);
        }
        else
        {
            bar->label("Sales", "Years", "Horizontal Bar Chart");
            egt::HorizontalBarChart::DataArray data;
            for (int i = 0; i < 10; i++)
            {
                data.push_back(std::make_pair(random_item(1, 50), (2010 + i)));
            }
            bar->data(data);
        }

        auto csizer = std::make_shared<egt::HorizontalBoxSizer>();
        sizer->add(expand_horizontal(csizer));

        csizer->add(create_grid_combo(bar));

        csizer->add(create_bar_pattern(bar));

        auto btn2 = std::make_shared<egt::Button>("Remove Data");
        csizer->add(btn2);
        btn2->on_click([btn2, bar, months, sdata](egt::Event&)
        {
            if (btn2->text() == "Add Data")
            {
                if (sdata)
                {
                    egt::HorizontalBarChart::StringDataArray data1;
                    static size_t i = 0;
                    data1.push_back(std::make_pair(random_item(1, 150), months[i++]));
                    bar->add_data(data1);

                    if (i >= egt::detail::size(months))
                        i = 0;
                }
                else
                {
                    egt::BarChart::DataArray data1;
                    static int year = 2010;
                    data1.push_back(std::make_pair(random_item(1, 50), year));
                    bar->add_data(data1);
                    year++;
                }
                btn2->text("Remove Data");
            }
            else
            {
                bar->remove_data(1);
                btn2->text("Add Data");
            }
        });
    }
};

struct VerticalBarPage : public egt::NotebookTab
{
    explicit VerticalBarPage(bool sdata)
    {
        auto sizer = std::make_shared<egt::VerticalBoxSizer>();
        add(expand(sizer));

        auto bar = std::make_shared<egt::BarChart>();
        sizer->add(expand(bar));

        const char* months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
        if (sdata)
        {
            bar->label("Months", "Sales", "Vertical Bar Chart-2");
            egt::BarChart::StringDataArray str_data;
            for (const auto& i : months)
            {
                str_data.push_back(std::make_pair(random_item(1, 50), i));
            }
            bar->data(str_data);
        }
        else
        {
            bar->label("Year", "Sales", "Vertical Bar Chart");
            egt::BarChart::DataArray data;
            for (int i = 0; i < 10; i++)
            {
                data.push_back(std::make_pair((1980 + i), random_item(1, 50)));
            }
            bar->data(data);
        }

        auto csizer = std::make_shared<egt::HorizontalBoxSizer>();
        sizer->add(expand_horizontal(csizer));

        csizer->add(create_grid_combo(bar));

        csizer->add(create_bar_pattern(bar));

        auto btn2 = std::make_shared<egt::Button>("Remove Data");
        csizer->add(btn2);
        btn2->on_click([bar, btn2, sdata, months](egt::Event&)
        {
            if (btn2->text() == "Add Data")
            {
                if (sdata)
                {
                    static size_t i = 0;
                    egt::BarChart::StringDataArray str_data1;
                    str_data1.push_back(std::make_pair(random_item(1, 50), months[i++]));
                    bar->add_data(str_data1);

                    if (i >= egt::detail::size(months))
                        i = 0;
                }
                else
                {
                    egt::BarChart::DataArray data1;
                    static int year = 1990;
                    data1.push_back(std::make_pair(year, random_item(1, 50)));
                    bar->add_data(data1);
                    year++;
                }
                btn2->text("Remove Data");
            }
            else
            {
                bar->remove_data(1);
                btn2->text("Add Data");
            }
        });
    }
};

static std::shared_ptr<egt::HorizontalBoxSizer> create_line_combo(const std::shared_ptr<egt::LineChart>& chart)
{
    auto hsizer = std::make_shared<egt::HorizontalBoxSizer>();

    static const std::pair<std::string, egt::LineChart::LinePattern> line_patterns[] =
    {
        {"LineStyle: Solid", egt::LineChart::LinePattern::solid},
        {"LineStyle: Dotted", egt::LineChart::LinePattern::dotted },
        {"LineStyle: Dashes", egt::LineChart::LinePattern::dashes },
    };

    auto pattern = std::make_shared<egt::ComboBox>();
    for (auto& i : line_patterns)
        pattern->add_item(i.first);
    pattern->margin(5);
    hsizer->add(pattern);

    pattern->on_selected_changed([pattern, chart]()
    {
        auto s = pattern->item_at(pattern->selected());
        for (auto& i : line_patterns)
        {
            if (s == i.first)
            {
                chart->line_style(i.second);
                break;
            }
        }
    });
    pattern->selected(0);

    auto line_width = std::make_shared<egt::ComboBox>();
    for (int i = 1; i < 10; i++)
    {
        line_width->add_item("LineWidth: " + std::to_string(i));
    }
    line_width->margin(5);
    hsizer->add(line_width);

    line_width->on_selected_changed([line_width, chart]()
    {
        auto s = line_width->item_at(line_width->selected());
        chart->line_width(strtoul(s.substr(11, 1).c_str(), nullptr, 10));
    });
    line_width->selected(0);

    return hsizer;
}

enum LineChartType
{
    Sine = 1,
    Cosine,
    Tangent,
};

struct LineChartPage : public egt::NotebookTab
{
    explicit LineChartPage(LineChartType type)
    {
        auto sizer = std::make_shared<egt::VerticalBoxSizer>();
        add(expand(sizer));

        auto line = std::make_shared<egt::LineChart>();
        sizer->add(egt::expand(line));

        egt::LineChart::DataArray data;
        for (auto i = 0; i < 6283; i += 200)
        {
            if (type == LineChartType::Cosine)
            {
                line->label("x-axis", "y-axis", "Cosine Chart");
                data.push_back(std::make_pair(i / 1000., std::cos(i / 1000.)));
            }
            else if (type == LineChartType::Sine)
            {
                line->label("x-axis", "y-axis", "Sine Chart");
                data.push_back(std::make_pair(i / 1000., std::sin(i / 1000.)));
            }
        }
        line->data(data);

        auto csizer = std::make_shared<egt::HorizontalBoxSizer>(egt::Justification::start);
        sizer->add(csizer);

        csizer->add(create_line_combo(line));

        csizer->add(create_grid_combo(line));

        auto btn2 = std::make_shared<egt::Button>("Add Data");
        csizer->add(btn2);
        btn2->on_click([btn2, line, type](egt::Event&)
        {
            if (btn2->text() == "Add Data")
            {
                egt::LineChart::DataArray data1;
                if (type == LineChartType::Cosine)
                {
                    static int ci = 6283;
                    static int csize = 4;
                    for (; ci < 3141 * csize; ci += 200)
                    {
                        data1.push_back(std::make_pair(ci / 1000., std::cos(ci / 1000.)));
                    }
                    line->add_data(data1);
                    csize ++;
                }
                else if (type == LineChartType::Sine)
                {
                    static int si = 6283;
                    static int ssize = 4;
                    for (; si < 3141 * ssize; si += 200)
                    {
                        data1.push_back(std::make_pair(si / 1000., std::sin(si / 1000.)));
                    }
                    line->add_data(data1);
                    ssize ++;
                }
                btn2->text("RM Data");
            }
            else
            {
                line->remove_data(M_PI * 2);
                btn2->text("Add Data");
            }
        });
    }
};

int main(int argc, char** argv)
{
    egt::Application app(argc, argv);
    egt::TopWindow win;

    egt::VerticalBoxSizer vsizer(win);
    expand(vsizer);

    auto frame = std::make_shared<egt::Frame>(egt::Size(win.width(), win.height() * 0.1));
    frame->color(egt::Palette::ColorId::bg, egt::Palette::transparent);
    vsizer.add(egt::expand_horizontal(frame));

    auto logo = std::make_shared<egt::ImageLabel>(egt::Image("icon:egt_logo_black.png;128"));
    logo->align(egt::AlignFlag::left | egt::AlignFlag::center);
    logo->margin(5);
    frame->add(logo);

    const std::pair<const char*, std::function<std::unique_ptr<egt::Theme>()>> combo_items[] =
    {
        {"Default Theme", []{ return std::make_unique<egt::Theme>(); }},
        {"Lapis", []{ return std::make_unique<egt::LapisTheme>(); }},
        {"Midnight", []{ return std::make_unique<egt::MidnightTheme>(); }},
        {"Sky", []{ return std::make_unique<egt::SkyTheme>(); }},
        {"Shamrock", []{ return std::make_unique<egt::ShamrockTheme>(); }},
        {"Coconut", []{ return std::make_unique<egt::CoconutTheme>(); }},
        {"Ultra Violet", []{ return std::make_unique<egt::UltraVioletTheme>(); }}
    };

    auto combo = std::make_shared<egt::ComboBox>();
    for (auto& i : combo_items)
        combo->add_item(i.first);
    combo->align(egt::AlignFlag::center_vertical | egt::AlignFlag::right);
    combo->margin(5);
    frame->add(combo);

    combo->on_selected_changed([&]()
    {
        auto s = combo->item_at(combo->selected());
        for (auto& i : combo_items)
        {
            if (s == i.first)
            {
                egt::global_theme(i.second());

                if ((s == "Midnight") || (s == "Lapis"))
                    logo->image(egt::Image("icon:egt_logo_white.png;128"));
                else
                    logo->image(egt::Image("icon:egt_logo_black.png;128"));

                break;
            }
        }
        win.damage();
    });
    combo->selected(0);

    const std::pair<const char*, std::function<std::shared_ptr<egt::NotebookTab>()>> charts_items[] =
    {
        {"Sine Chart", []{ return std::make_shared<LineChartPage>(LineChartType::Sine); }},
        {"Cosine Chart", []{ return std::make_shared<LineChartPage>(LineChartType::Cosine); }},
        {"Points", []{ return std::make_shared<Points>(); }},
        {"Vertical Bar", []{ return std::make_shared<VerticalBarPage>(false); }},
        {"Vertical Bar-2", []{ return std::make_shared<VerticalBarPage>(true); }},
        {"Horizontal Bar", []{ return std::make_shared<HorizontalBarPage>(false); }},
        {"Horizontal Bar-2", []{ return std::make_shared<HorizontalBarPage>(true); }},
        {"Pie Chart", []{ return std::make_shared<PiePage>(); }}
    };

    auto chart = std::make_shared<egt::ComboBox>();
    for (auto& i : charts_items)
        chart->add_item(i.first);
    chart->align(egt::AlignFlag::center_vertical | egt::AlignFlag::center);
    chart->margin(5);
    frame->add(chart);

    egt::BoxSizer hsizer(egt::Orientation::horizontal);
    hsizer.margin(5);
    vsizer.add(expand(hsizer));

    chart->on_selected_changed([&]()
    {
        hsizer.remove_all();
        auto s = chart->item_at(chart->selected());
        for (auto& i : charts_items)
        {
            if (s == i.first)
            {
                hsizer.add(expand(i.second()));
            }
        }
    });
    chart->selected(1);

    win.show();

    return app.run();
}
