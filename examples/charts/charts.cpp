/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#define _USE_MATH_DEFINES
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

static std::shared_ptr<egt::ListBox> chart_axis_type(const std::shared_ptr<egt::ChartBase>& chart)
{
    static const std::pair<const char*, egt::ChartBase::GridFlag> view_items[] =
    {
        {"None", egt::ChartBase::GridFlag::none },
        {"Box", egt::ChartBase::GridFlag::box },
        {"Ticks", egt::ChartBase::GridFlag::box_ticks },
        {"Coordinates", egt::ChartBase::GridFlag::box_ticks_coord },
        {"Major Ticks", egt::ChartBase::GridFlag::box_major_ticks_coord },
        {"Minor Ticks", egt::ChartBase::GridFlag::box_minor_ticks_coord },
    };

    auto cvlist = std::make_shared<egt::ListBox>();
    for (auto& i : view_items)
        cvlist->add_item(std::make_shared<egt::StringItem>(i.first));
    cvlist->margin(5);

    cvlist->on_selected([cvlist, chart](size_t index)
    {
        auto s = cvlist->item_at(index);
        for (auto& i : view_items)
        {
            if (s->text() == i.first)
            {
                chart->grid_style(i.second);
                break;
            }
        }
        cvlist->parent()->hide();
    });

    return cvlist;
}

struct PiePage : public egt::NotebookTab
{
    PiePage()
    {
        auto sizer = std::make_shared<egt::VerticalBoxSizer>();
        add(expand(sizer));

        auto pie = std::make_shared<egt::PieChart>();
        pie->title("Pie Chart");

        egt::ChartItemArray data;
        const int pdata[] = { 5, 10, 15, 20, 4, 8, 16, 12};
        size_t count = 0;
        for (auto& i : pdata)
        {
            data.add(i, ("label" + std::to_string(count++)));
        }
        pie->data(data);
        sizer->add(egt::expand(pie));

        auto csizer = std::make_shared<egt::HorizontalBoxSizer>();
        sizer->add(expand_horizontal(csizer));

        auto btn1 = std::make_shared<egt::ImageButton>(egt::Image("file:add.png"));
        btn1->fill_flags().clear();
        btn1->align(egt::AlignFlag::right | egt::AlignFlag::center_vertical);
        sizer->add(btn1);
        btn1->on_click([pie, btn1](egt::Event&)
        {
            static bool btn_flag = true;
            if (btn_flag)
            {
                egt::ChartItemArray data1;
                static size_t i = 1;
                data1.add(random_item(1, 25), "label" + std::to_string(++i));
                data1.add(random_item(1, 10), "label" + std::to_string(++i));
                pie->add_data(data1);
                btn1->image(egt::Image("file:rm.png"));
                btn_flag = false;
            }
            else
            {
                pie->remove_data(2);
                btn1->image(egt::Image("file:add.png"));
                btn_flag = true;
            }
        });
    }
};

static std::shared_ptr<egt::ListBox> point_chart_type(const std::shared_ptr<egt::PointChart>& point)
{
    static const std::pair<std::string, egt::PointChart::PointType> point_items[] =
    {
        {"PointType: Dot", egt::PointChart::PointType::dot},
        {"PointType: Star", egt::PointChart::PointType::star },
        {"PointType: Circle", egt::PointChart::PointType::circle },
        {"PointType: Cross", egt::PointChart::PointType::cross },
    };

    auto plist = std::make_shared<egt::ListBox>();
    for (auto& i : point_items)
        plist->add_item(std::make_shared<egt::StringItem>(i.first));
    plist->margin(5);;

    plist->on_selected([plist, point](size_t index)
    {
        auto s = plist->item_at(index);
        for (auto& i : point_items)
        {
            if (s->text() == i.first)
            {
                point->point_type(i.second);
                break;
            }
        }
        plist->parent()->hide();
    });

    return plist;
}

struct Points : public egt::NotebookTab
{
    explicit Points(const std::shared_ptr<egt::PointChart>& point)
    {
        auto sizer = std::make_shared<egt::VerticalBoxSizer>();
        add(expand(sizer));

        point->label("x-axis", "y-axis", "Point Chart");
        sizer->add(expand(point));

        auto csizer = std::make_shared<egt::HorizontalBoxSizer>();
        sizer->add(expand_horizontal(csizer));

        egt::ChartItemArray data;
        for (int i = 0; i < 101; i++)
        {
            data.add(i, random_item(1, 100));
        }
        point->data(data);

        auto btn2 = std::make_shared<egt::ImageButton>(egt::Image("file:add.png"));
        btn2->fill_flags().clear();
        btn2->align(egt::AlignFlag::right | egt::AlignFlag::center_vertical);
        sizer->add(btn2);
        btn2->on_click([point, btn2](egt::Event&)
        {
            static bool btn_flag = true;
            if (btn_flag)
            {
                egt::ChartItemArray data1;
                static int n = 101;
                for (int i = n ; i < (n + 10); i++)
                {
                    data1.add(i, random_item(1, 125));
                }
                point->add_data(data1);
                n += 10;
                btn2->image(egt::Image("file:rm.png"));
                btn_flag = false;
            }
            else
            {
                point->remove_data(15);
                btn2->image(egt::Image("file:add.png"));
                btn_flag = true;
            }
        });
    }
};

static std::shared_ptr<egt::ListBox> bar_chart_pattern(const std::shared_ptr<egt::BarChart>& chart)
{
    static const std::pair<std::string, egt::BarChart::BarPattern> bar_pattern[] =
    {
        {"BarPattern: Solid", egt::BarChart::BarPattern::solid},
        {"BarPattern: Horizontal", egt::BarChart::BarPattern::horizontal_line},
        {"BarPattern: Vertical", egt::BarChart::BarPattern::vertical_line},
        {"BarPattern: Boxes", egt::BarChart::BarPattern::boxes},
    };

    auto blist = std::make_shared<egt::ListBox>();
    for (auto& i : bar_pattern)
        blist->add_item(std::make_shared<egt::StringItem>(i.first));
    blist->margin(5);

    blist->on_selected([blist, chart](size_t index)
    {
        auto s = blist->item_at(index);
        for (auto& i : bar_pattern)
        {
            if (s->text() == i.first)
            {
                chart->bar_style(i.second);
                break;
            }
        }
        blist->parent()->hide();
    });

    return blist;
}

struct HorizontalBarPage : public egt::NotebookTab
{
    explicit HorizontalBarPage(const std::shared_ptr<egt::HorizontalBarChart>& bar, bool sdata)
    {
        auto sizer = std::make_shared<egt::VerticalBoxSizer>();
        add(expand(sizer));

        sizer->add(expand(bar));

        const char* months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

        if (sdata)
        {
            bar->label("Sales", "Months", "Horizontal BarChart-2");
            egt::ChartItemArray str_data;
            for (const auto& i : months)
            {
                str_data.add(random_item(1, 50), i);
            }
            bar->data(str_data);
        }
        else
        {
            bar->label("Sales", "Years", "Horizontal Bar Chart");
            egt::ChartItemArray data;
            for (int i = 0; i < 10; i++)
            {
                data.add(random_item(1, 50), (2010 + i));
            }
            bar->data(data);
        }

        auto btn2 = std::make_shared<egt::ImageButton>(egt::Image("file:add.png"));
        btn2->fill_flags().clear();
        btn2->align(egt::AlignFlag::right | egt::AlignFlag::center_vertical);
        sizer->add(btn2);
        btn2->on_click([bar, btn2, sdata, months](egt::Event&)
        {
            static bool btn_flag = true;
            if (btn_flag)
            {
                if (sdata)
                {
                    egt::ChartItemArray data1;
                    static size_t i = 0;
                    data1.add(random_item(1, 150), months[i++]);
                    bar->add_data(data1);

                    if (i >= egt::detail::size(months))
                        i = 0;
                }
                else
                {
                    egt::ChartItemArray data1;
                    static int year = 2010;
                    data1.add(random_item(1, 50), year);
                    bar->add_data(data1);
                    year++;
                }
                btn2->image(egt::Image("file:rm.png"));
                btn_flag = false;
            }
            else
            {
                bar->remove_data(1);
                btn2->image(egt::Image("file:add.png"));
                btn_flag = true;
            }
        });
    }
};

struct VerticalBarPage : public egt::NotebookTab
{
    explicit VerticalBarPage(const std::shared_ptr<egt::BarChart>& bar, bool sdata)
    {
        auto sizer = std::make_shared<egt::VerticalBoxSizer>();
        add(expand(sizer));

        sizer->add(expand(bar));

        const char* months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
        if (sdata)
        {
            bar->label("Months", "Sales", "Vertical Bar Chart-2");
            egt::ChartItemArray str_data;
            for (const auto& i : months)
            {
                str_data.add(random_item(1, 50), i);
            }
            bar->data(str_data);
        }
        else
        {
            bar->label("Year", "Sales", "Vertical Bar Chart");
            egt::ChartItemArray data;
            for (int i = 0; i < 10; i++)
            {
                data.add((1980 + i), random_item(1, 50));
            }
            bar->data(data);
        }

        auto btn2 = std::make_shared<egt::ImageButton>(egt::Image("file:add.png"));
        btn2->fill_flags().clear();
        btn2->align(egt::AlignFlag::right | egt::AlignFlag::center_vertical);
        sizer->add(btn2);
        btn2->on_click([bar, btn2, sdata, months](egt::Event&)
        {
            static bool btn_flag = true;
            if (btn_flag)
            {
                if (sdata)
                {
                    static size_t i = 0;
                    egt::ChartItemArray str_data1;
                    str_data1.add(random_item(1, 50), months[i++]);
                    bar->add_data(str_data1);

                    if (i >= egt::detail::size(months))
                        i = 0;
                }
                else
                {
                    egt::ChartItemArray data1;
                    static int year = 1990;
                    data1.add(year, random_item(1, 50));
                    bar->add_data(data1);
                    year++;
                }
                btn2->image(egt::Image("file:rm.png"));
                btn_flag = false;
            }
            else
            {
                bar->remove_data(1);
                btn2->image(egt::Image("file:add.png"));
                btn_flag = true;
            }
        });
    }
};

static std::shared_ptr<egt::ListBox> line_chart_style(const std::shared_ptr<egt::LineChart>& chart)
{
    static const std::pair<std::string, egt::LineChart::LinePattern> line_patterns[] =
    {
        {"LineStyle: Solid", egt::LineChart::LinePattern::solid },
        {"LineStyle: Dotted", egt::LineChart::LinePattern::dotted },
        {"LineStyle: Dashes", egt::LineChart::LinePattern::dashes },
    };

    auto pattern = std::make_shared<egt::ListBox>();
    for (auto& i : line_patterns)
        pattern->add_item(std::make_shared<egt::StringItem>(i.first));
    pattern->margin(5);

    pattern->on_selected([pattern, chart](size_t index)
    {
        auto s = pattern->item_at(index);
        for (auto& i : line_patterns)
        {
            if (s->text() == i.first)
            {
                chart->line_style(i.second);
                break;
            }
        }
        pattern->parent()->hide();
    });

    return pattern;
}

static std::shared_ptr<egt::ListBox> line_chart_width(const std::shared_ptr<egt::LineChart>& chart)
{
    auto line_width = std::make_shared<egt::ListBox>();
    for (int i = 1; i < 10; i++)
    {
        line_width->add_item(std::make_shared<egt::StringItem>("LineWidth: " + std::to_string(i)));
    }
    line_width->margin(5);

    line_width->on_selected([line_width, chart](size_t index)
    {
        auto s = line_width->item_at(index);
        chart->line_width(strtoul(s->text().substr(11, 1).c_str(), nullptr, 10));
        line_width->parent()->hide();
    });

    return line_width;
}

enum LineChartType
{
    Sine = 1,
    Cosine,
};

struct LineChartPage : public egt::NotebookTab
{
    explicit LineChartPage(const std::shared_ptr<egt::LineChart>& line, LineChartType type)
    {
        auto sizer = std::make_shared<egt::VerticalBoxSizer>();
        add(expand(sizer));

        sizer->add(egt::expand(line));

        egt::ChartItemArray data;
        for (auto i = 0; i < 6283; i += 200)
        {
            if (type == LineChartType::Cosine)
            {
                line->label("x-axis", "y-axis", "Cosine Chart");
                data.add((i / 1000.), std::cos(i / 1000.));
            }
            else if (type == LineChartType::Sine)
            {
                line->label("x-axis", "y-axis", "Sine Chart");
                data.add((i / 1000.), std::sin(i / 1000.));
            }
        }
        line->data(data);

        auto btn2 = std::make_shared<egt::ImageButton>(egt::Image("file:add.png"));
        btn2->fill_flags().clear();
        btn2->align(egt::AlignFlag::right | egt::AlignFlag::center_vertical);
        sizer->add(btn2);
        btn2->on_click([btn2, line, type](egt::Event&)
        {
            static bool btn_flag = true;
            if (btn_flag)
            {
                egt::ChartItemArray data1;
                if (type == LineChartType::Cosine)
                {
                    static int ci = 6283;
                    static int csize = 4;
                    for (; ci < 3141 * csize; ci += 200)
                    {
                        data1.add((ci / 1000.), std::cos(ci / 1000.));
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
                        data1.add((si / 1000.), std::sin(si / 1000.));
                    }
                    line->add_data(data1);
                    ssize ++;
                }
                btn2->image(egt::Image("file:rm.png"));
                btn_flag = false;
            }
            else
            {
                line->remove_data(M_PI * 2);
                btn2->image(egt::Image("file:add.png"));
                btn_flag = true;
            }
        });
    }
};

int main(int argc, char** argv)
{
    egt::Application app(argc, argv);
    egt::TopWindow win;

#ifdef EXAMPLEDATA
    egt::add_search_path(EXAMPLEDATA);
#endif

    egt::VerticalBoxSizer vsizer(win);
    expand(vsizer);

    auto frame = std::make_shared<egt::Frame>(egt::Size(win.width(), win.height() * 0.1));
    frame->color(egt::Palette::ColorId::bg, egt::Palette::transparent);
    vsizer.add(egt::expand_horizontal(frame));

    auto logo = std::make_shared<egt::ImageLabel>(egt::Image("icon:egt_logo_black.png;128"));
    logo->align(egt::AlignFlag::left | egt::AlignFlag::center_vertical);
    logo->margin(5);
    frame->add(logo);

    auto setting = std::make_shared<egt::ImageButton>(egt::Image("icon:settings.png"));
    setting->fill_flags().clear();
    setting->align(egt::AlignFlag::right | egt::AlignFlag::center_vertical);
    frame->add(setting);

    auto spopup = std::make_shared<egt::Popup>(egt::Size(win.width() * 0.50, 40));
    auto slist = std::make_shared<egt::ListBox>();
    slist->add_item(std::make_shared<egt::StringItem>("Themes"));
    spopup->add(egt::expand(slist));
    win.add(spopup);

    auto tpopup = std::make_shared<egt::Popup>(egt::Size(win.size() * 0.50));
    const std::pair<const char*, std::function<std::unique_ptr<egt::Theme>()>> theme_items[] =
    {
        {"Default Theme", []{ return std::make_unique<egt::Theme>(); }},
        {"Lapis", []{ return std::make_unique<egt::LapisTheme>(); }},
        {"Midnight", []{ return std::make_unique<egt::MidnightTheme>(); }},
        {"Sky", []{ return std::make_unique<egt::SkyTheme>(); }},
        {"Shamrock", []{ return std::make_unique<egt::ShamrockTheme>(); }},
        {"Coconut", []{ return std::make_unique<egt::CoconutTheme>(); }},
        {"Ultra Violet", []{ return std::make_unique<egt::UltraVioletTheme>(); }}
    };

    auto tlist = std::make_shared<egt::ListBox>();
    for (auto& i : theme_items)
        tlist->add_item(std::make_shared<egt::StringItem>(i.first));
    tlist->margin(5);

    tlist->on_selected([&](size_t index)
    {
        auto s = tlist->item_at(index);
        for (auto& i : theme_items)
        {
            if (s->text() == i.first)
            {
                egt::global_theme(i.second());

                if ((s->text() == "Midnight") || (s->text() == "Lapis"))
                    logo->image(egt::Image("icon:egt_logo_white.png;128"));
                else
                    logo->image(egt::Image("icon:egt_logo_black.png;128"));

                break;
            }
        }
        tpopup->hide();
        win.damage();
    });
    tlist->selected(0);

    auto line = std::make_shared<egt::LineChart>();
    auto pchart = std::make_shared<egt::PointChart>();
    auto vbar = std::make_shared<egt::BarChart>();
    auto hbar = std::make_shared<egt::HorizontalBarChart>();

    const std::pair<const char*, std::function<std::shared_ptr<egt::NotebookTab>()>> charts_items[] =
    {
        {"Sine Chart", [line] { return std::make_shared<LineChartPage>(line, LineChartType::Sine);}},
        {"Cosine Chart", [line] { return std::make_shared<LineChartPage>(line, LineChartType::Cosine);}},
        {"Points", [pchart] { return std::make_shared<Points>(pchart);}},
        {"Vertical Bar",  [vbar] { return std::make_shared<VerticalBarPage>(vbar, false);}},
        {"Vertical Bar-2", [vbar] { return std::make_shared<VerticalBarPage>(vbar, true);}},
        {"Horizontal Bar", [hbar] { return std::make_shared<HorizontalBarPage>(hbar, false);}},
        {"Horizontal Bar-2", [hbar] { return std::make_shared<HorizontalBarPage>(hbar, true);}},
        {"Pie Chart", []{ return std::make_shared<PiePage>(); }}
    };

    auto chart = std::make_shared<egt::ComboBox>();
    for (auto& i : charts_items)
        chart->add_item(std::make_shared<egt::StringItem>(i.first));
    chart->align(egt::AlignFlag::center);
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
            if (s->text() == i.first)
            {
                hsizer.add(expand(i.second()));
            }
        }
    });
    chart->selected(1);

    setting->on_click([&](egt::Event&)
    {
        auto s = chart->item_at(chart->selected());
        slist->clear();
        if ((s->text() == "Sine Chart") || (s->text() == "Cosine Chart"))
        {
            slist->add_item(std::make_shared<egt::StringItem>("Line Style"));
            slist->add_item(std::make_shared<egt::StringItem>("Line Width"));
        }
        else if (s->text() == "Points")
        {
            slist->add_item(std::make_shared<egt::StringItem>("Point Type"));
        }
        else if ((s->text() == "Vertical Bar") || (s->text() == "Vertical Bar-2")
                 || (s->text() == "Horizontal Bar") || (s->text() == "Horizontal Bar-2"))
        {
            slist->add_item(std::make_shared<egt::StringItem>("Bar Type"));
        }

        if (s->text() != "Pie Chart")
            slist->add_item(std::make_shared<egt::StringItem>("Chart Axis"));

        slist->add_item(std::make_shared<egt::StringItem>("Themes"));

        spopup->resize(egt::Size(win.width() * 0.50, slist->item_count() * 40));
        spopup->show_modal(true);
    });

    tpopup->on_hide([&]()
    {
        tpopup->remove_all();
        win.remove(tpopup.get());
    });

    slist->on_selected([&](size_t index)
    {
        auto s = slist->item_at(index);
        spopup->hide();
        if (s->text() == "Line Style")
        {
            auto pw = line_chart_style(line);
            tpopup->add(egt::expand(pw));
            tpopup->resize(egt::Size(win.width() * 0.50, pw->item_count() * 40));
        }
        else if (s->text() == "Line Width")
        {
            auto pw = line_chart_width(line);
            tpopup->add(egt::expand(pw));
        }
        else if (s->text() == "Point Type")
        {
            auto pw = point_chart_type(pchart);
            tpopup->add(egt::expand(pw));
            tpopup->resize(egt::Size(win.width() * 0.50, pw->item_count() * 40));
        }
        else if (s->text() == "Bar Type")
        {
            auto ctype = chart->item_at(chart->selected());
            if ((ctype->text() == "Vertical Bar") || (ctype->text() == "Vertical Bar-2"))
            {
                auto pw = bar_chart_pattern(vbar);
                tpopup->add(egt::expand(pw));
                tpopup->resize(egt::Size(win.width() * 0.50, pw->item_count() * 40));
            }
            else if ((ctype->text() == "Horizontal Bar") || (ctype->text() == "Horizontal Bar-2"))
            {
                auto pw = bar_chart_pattern(hbar);
                tpopup->add(egt::expand(pw));
                tpopup->resize(egt::Size(win.width() * 0.50, pw->item_count() * 40));
            }
        }
        else if (s->text() == "Chart Axis")
        {
            auto ctype = chart->item_at(chart->selected());
            if ((ctype->text() == "Sine Chart") || (ctype->text() == "Cosine Chart"))
            {
                auto pw = chart_axis_type(line);
                tpopup->add(egt::expand(pw));
            }
            else if (ctype->text() == "Points")
            {
                auto pw = chart_axis_type(pchart);
                tpopup->add(egt::expand(pw));
            }
            else if ((ctype->text() == "Vertical Bar") || (ctype->text() == "Vertical Bar-2"))
            {
                auto pw = chart_axis_type(vbar);
                tpopup->add(egt::expand(pw));
            }
            else if ((ctype->text() == "Horizontal Bar") || (ctype->text() == "Horizontal Bar-2"))
            {
                auto pw = chart_axis_type(hbar);
                tpopup->add(egt::expand(pw));
            }
        }
        else if (s->text() == "Themes")
        {
            tpopup->add(egt::expand(tlist));
        }
        win.add(tpopup);
        tpopup->show_modal(true);
    });

    win.show();

    return app.run();
}
