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
#include <vector>

static int random_item(int start, int end)
{
    std::random_device r;
    std::default_random_engine e {r()};
    std::uniform_int_distribution<int> dist(start, end);
    return dist(e);
}

static std::shared_ptr<egt::ComboBox> create_grid_combo(std::shared_ptr<egt::ChartBase> chart)
{
    static const std::vector<std::pair<std::string, egt::ChartBase::GridFlag>> combo_items =
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
    PiePage(const egt::Size& size)
    {
        auto sizer = std::make_shared<egt::VerticalBoxSizer>();
        add(expand(sizer));

        auto pie = std::make_shared<egt::PieChart>();
        pie->title("Pie Chart Example");

        egt::PieChart::StringDataArray data;
        std::vector<int> pdata = { 5, 10, 15, 20, 4, 8, 16, 12};
        int count = 0;
        for (auto& i : pdata)
        {
            data.push_back(make_pair(i, ("label" + std::to_string(count++))));
        }
        pie->data(data);
        sizer->add(egt::expand(pie));

        auto csizer = std::make_shared<egt::HorizontalBoxSizer>();
        sizer->add(csizer);

        auto btn1 = std::make_shared<egt::Button>("Add Data");
        csizer->add(btn1);
        btn1->on_click([pie, btn1](egt::Event&)
        {
            if (btn1->text() == "Add Data")
            {
                egt::PieChart::StringDataArray data1;
                static int i = 1;
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
    Points(const egt::Size& size)
    {
        auto sizer = std::make_shared<egt::VerticalBoxSizer>();
        add(expand(sizer));

        auto line = std::make_shared<egt::PointChart>();
        line->label("x-axis", "y-axis", "Point Chart Example");
        sizer->add(expand(line));

        auto csizer = std::make_shared<egt::HorizontalBoxSizer>();
        sizer->add(csizer);

        egt::PointChart::DataArray data;
        for (int i = 0; i < 101; i++)
        {
            data.push_back(std::make_pair(i, random_item(1, 100)));
        }
        line->data(data);

        csizer->add(create_grid_combo(line));

        auto point_type = std::make_shared<egt::Slider>(egt::Size(size.width() * 0.25, size.height() * 0.10), 1, 4, 1);
        point_type->slider_flags().set(egt::Slider::SliderFlag::round_handle);
        point_type->name("point type");
        point_type->margin(5);
        csizer->add(point_type);

        point_type->on_value_changed([line, point_type]()
        {
            switch (point_type->value())
            {
            case 1:
                line->point_type(egt::PointChart::PointType::dot);
                break;
            case 2:
                line->point_type(egt::PointChart::PointType::star);
                break;
            case 3:
                line->point_type(egt::PointChart::PointType::cross);
                break;
            default:
                line->point_type(egt::PointChart::PointType::circle);
                break;
            }
        });

        point_type->value(1);

        auto btn2 = std::make_shared<egt::Button>("Add Data");
        csizer->add(btn2);
        btn2->on_click([line, btn2](egt::Event & event)
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

struct HorizontalBarPage : public egt::NotebookTab
{
    HorizontalBarPage(const egt::Size& size)
    {
        auto sizer = std::make_shared<egt::VerticalBoxSizer>();
        add(expand(sizer));

        auto bar = std::make_shared<egt::HorizontalBarChart>();
        bar->label("Widget Sales (millions)", "---- Months ---- ", "Horizontal BarChart Example");
        sizer->add(expand(bar));

        std::string x[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

        egt::HorizontalBarChart::StringDataArray data;
        for (int i = 0; i < 12; i++)
        {
            data.push_back(std::make_pair(random_item(1, 50), x[i]));
        }
        bar->data(data);

        auto csizer = std::make_shared<egt::HorizontalBoxSizer>();
        sizer->add(csizer);

        auto btn2 = std::make_shared<egt::Button>("Remove Data");
        csizer->add(btn2);
        btn2->on_click([btn2, bar, x](egt::Event & event)
        {
            egt::HorizontalBarChart::StringDataArray data1;
            if (btn2->text() == "Add Data")
            {
                static int i = 0;
                data1.push_back(std::make_pair(random_item(1, 150), x[i++]));
                bar->add_data(data1);

                if (i >= 11)
                    i = 0;

                btn2->text("Remove Data");
            }
            else
            {
                bar->remove_data(1);
                btn2->text("Add Data");
            }

        });

        auto pattern = std::make_shared<egt::Slider>(egt::Size(size.width() * 0.25, size.height() * 0.10), 1, 4, 1);
        pattern->slider_flags().set(egt::Slider::SliderFlag::round_handle);
        pattern->margin(5);
        pattern->name("HorizontalBar pattern");
        csizer->add(pattern);

        pattern->on_value_changed([bar, pattern]()
        {
            switch (pattern->value())
            {
            case 1:
                bar->bar_style(egt::HorizontalBarChart::BarPattern::solid);
                break;
            case 2:
                bar->bar_style(egt::HorizontalBarChart::BarPattern::horizontal_line);
                break;
            case 3:
                bar->bar_style(egt::HorizontalBarChart::BarPattern::vertical_line);
                break;
            default:
                bar->bar_style(egt::HorizontalBarChart::BarPattern::boxes);
                break;
            }
        });

        pattern->value(1);

    }
};

struct VerticalBarPage : public egt::NotebookTab
{
    VerticalBarPage(const egt::Size& size)
    {
        auto sizer = std::make_shared<egt::VerticalBoxSizer>();
        add(expand(sizer));

        auto bar = std::make_shared<egt::BarChart>();
        bar->label("---- Year ---- ", "Widget Sales (millions)", "Vertical BarChart Example");
        sizer->add(expand(bar));

        egt::BarChart::DataArray data;
        for (int i = 0; i < 10; i++)
        {
            data.push_back(std::make_pair((1980 + i), random_item(1, 50)));
        }
        bar->data(data);

        auto csizer = std::make_shared<egt::HorizontalBoxSizer>();
        sizer->add(csizer);

        csizer->add(create_grid_combo(bar));

        auto pattern = std::make_shared<egt::Slider>(egt::Size(size.width() * 0.25, size.height() * 0.10), 1, 4, 1);
        pattern->slider_flags().set(egt::Slider::SliderFlag::round_handle);
        pattern->margin(5);
        pattern->name("Bar pattern");
        csizer->add(pattern);

        pattern->on_value_changed([bar, pattern]()
        {
            switch (pattern->value())
            {
            case 1:
                bar->bar_style(egt::BarChart::BarPattern::solid);
                break;
            case 2:
                bar->bar_style(egt::BarChart::BarPattern::horizontal_line);
                break;
            case 3:
                bar->bar_style(egt::BarChart::BarPattern::vertical_line);
                break;
            default:
                bar->bar_style(egt::BarChart::BarPattern::boxes);
                break;
            }
        });

        pattern->value(1);

        auto btn2 = std::make_shared<egt::Button>("Add Data");
        csizer->add(btn2);
        btn2->on_click([bar, btn2](egt::Event & event)
        {
            if (btn2->text() == "Add Data")
            {
                egt::BarChart::DataArray data1;
                static int year = 1990;
                data1.push_back(std::make_pair(year, random_item(1, 50)));
                bar->add_data(data1);
                year++;
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

struct TanPage : public egt::NotebookTab
{
    TanPage(const egt::Size& size)
    {
        auto sizer = std::make_shared<egt::VerticalBoxSizer>();
        add(expand(sizer));

        auto line = std::make_shared<egt::LineChart>();
        line->label("x-axis", "y-axis", "Tangent Chart Example");
        sizer->add(expand(line));

        egt::LineChart::DataArray data;
        for (float i = 0.; i < M_PI * 16; i += 0.2)
        {
            data.push_back(std::make_pair(i, std::atan(i)));
        }
        line->data(data);

        auto csizer = std::make_shared<egt::HorizontalBoxSizer>();
        sizer->add(csizer);

        auto line_width = std::make_shared<egt::Slider>(egt::Size(size.width() * 0.15, size.height() * 0.10), 1, 5, 1);
        line_width->slider_flags().set(egt::Slider::SliderFlag::round_handle);
        line_width->margin(5);
        line_width->name("line_width");
        csizer->add(line_width);

        line_width->on_value_changed([line, line_width]()
        {
            line->line_width(line_width->value());
        });

        line_width->value(2);

        auto line_pattern = std::make_shared<egt::Slider>(egt::Size(size.width() * 0.15, size.height() * 0.10), 1, 3, 1);
        line_pattern->slider_flags().set(egt::Slider::SliderFlag::round_handle);
        line_pattern->name("line_pattern");
        line_pattern->margin(5);
        csizer->add(line_pattern);

        line_pattern->on_value_changed([line, line_pattern]()
        {
            switch (line_pattern->value())
            {
            case 1:
                line->line_style(egt::LineChart::LinePattern::solid);
                break;
            case 2:
                line->line_style(egt::LineChart::LinePattern::dotted);
                break;
            case 3:
                line->line_style(egt::LineChart::LinePattern::dashes);
                break;
            }
        });

        line_pattern->value(1);

        csizer->add(create_grid_combo(line));

        auto btn2 = std::make_shared<egt::Button>("Add Data");
        csizer->add(btn2);
        btn2->on_click([btn2, line](egt::Event & event)
        {
            if (btn2->text() == "Add Data")
            {
                egt::LineChart::DataArray data1;
                static int tsize = 17;
                for (float i = M_PI * 16; i < M_PI * tsize; i += 0.2)
                {
                    data1.push_back(std::make_pair(i, std::atan(i)));
                }
                line->add_data(data1);
                tsize += 5;
                btn2->text("Remove Data");
            }
            else
            {
                uint32_t count = (10 * M_PI);
                line->remove_data(count);
                btn2->text("Add Data");
            }

        });

    }
};

struct SinePage : public egt::NotebookTab
{
    SinePage(const egt::Size& size)
    {
        auto sizer = std::make_shared<egt::VerticalBoxSizer>();
        add(expand(sizer));

        auto line = std::make_shared<egt::LineChart>();
        line->label("x-axis", "y-axis", "Sine Chart Example");
        sizer->add(expand(line));

        egt::LineChart::DataArray data;
        for (float i = 0.; i < M_PI * 2; i += 0.2)
        {
            data.push_back(std::make_pair(i, std::sin(i)));
        }
        line->data(data);

        auto csizer = std::make_shared<egt::HorizontalBoxSizer>();
        sizer->add(csizer);

        auto line_width = std::make_shared<egt::Slider>(egt::Size(size.width() * 0.15, size.height() * 0.10), 1, 5, 1);
        line_width->slider_flags().set(egt::Slider::SliderFlag::round_handle);
        line_width->name("line_width");
        line_width->margin(5);
        csizer->add(line_width);

        line_width->on_value_changed([line, line_width]()
        {
            line->line_width(line_width->value());
        });

        line_width->value(2);

        auto line_pattern = std::make_shared<egt::Slider>(egt::Size(size.width() * 0.15, size.height() * 0.10), 1, 3, 1);
        line_pattern->slider_flags().set(egt::Slider::SliderFlag::round_handle);
        line_pattern->name("line_pattern");
        line_pattern->margin(5);
        csizer->add(line_pattern);

        line_pattern->on_value_changed([line, line_pattern]()
        {
            switch (line_pattern->value())
            {
            case 1:
                line->line_style(egt::LineChart::LinePattern::solid);
                break;
            case 2:
                line->line_style(egt::LineChart::LinePattern::dotted);
                break;
            case 3:
                line->line_style(egt::LineChart::LinePattern::dashes);
                break;
            }
        });

        line_pattern->value(1);

        csizer->add(create_grid_combo(line));

        auto btn2 = std::make_shared<egt::Button>("Add Data");
        csizer->add(btn2);
        btn2->on_click([btn2, line](egt::Event & event)
        {
            if (btn2->text() == "Add Data")
            {
                egt::LineChart::DataArray data1;
                static int ssize = 4;
                static float i = M_PI * 2;
                for (; i < M_PI * ssize; i += 0.2)
                {
                    data1.push_back(std::make_pair(i, std::sin(i)));
                }
                line->add_data(data1);
                ssize++;
                btn2->text("Remove Data");
            }
            else
            {
                line->remove_data(2 * M_PI);
                btn2->text("Add Data");
            }
        });
    }
};

struct CosinePage : public egt::NotebookTab
{
    CosinePage(const egt::Size& size)
    {
        auto sizer = std::make_shared<egt::VerticalBoxSizer>();
        add(expand(sizer));

        auto line = std::make_shared<egt::LineChart>();
        line->label("x-axis", "y-axis", "Cosine Chart Example");
        sizer->add(egt::expand(line));

        egt::LineChart::DataArray data;
        for (float i = 0.; i < M_PI * 4; i += 0.2)
        {
            data.push_back(std::make_pair(i, std::cos(i)));
        }
        line->data(data);

        auto csizer = std::make_shared<egt::HorizontalBoxSizer>();
        sizer->add(csizer);

        auto line_width = std::make_shared<egt::Slider>(egt::Size(size.width() * 0.15, size.height() * 0.10), 1, 5, 1);
        line_width->slider_flags().set(egt::Slider::SliderFlag::round_handle);
        line_width->name("line_width");
        line_width->margin(5);
        csizer->add(line_width);

        line_width->on_value_changed([line, line_width]()
        {
            line->line_width(line_width->value());
        });
        line_width->value(2);

        auto line_pattern = std::make_shared<egt::Slider>(egt::Size(size.width() * 0.15, size.height() * 0.10), 1, 3, 1);
        line_pattern->slider_flags().set(egt::Slider::SliderFlag::round_handle);
        line_pattern->name("line_pattern");
        line_pattern->margin(5);
        csizer->add(line_pattern);

        line_pattern->on_value_changed([line, line_pattern]()
        {
            switch (line_pattern->value())
            {
            case 1:
                line->line_style(egt::LineChart::LinePattern::solid);
                break;
            case 2:
                line->line_style(egt::LineChart::LinePattern::dotted);
                break;
            case 3:
                line->line_style(egt::LineChart::LinePattern::dashes);
                break;
            }
        });
        line_pattern->value(1);

        csizer->add(create_grid_combo(line));

        auto btn2 = std::make_shared<egt::Button>("Add Data");
        csizer->add(btn2);
        btn2->on_click([btn2, line](egt::Event & event)
        {
            if (btn2->text() == "Add Data")
            {
                egt::LineChart::DataArray data1;
                static float i = M_PI * 4;
                static int csize = 9;
                for (; i < M_PI * csize; i += 0.2)
                {
                    data1.push_back(std::make_pair(i, std::cos(i)));
                }
                line->add_data(data1);
                csize += 3;
                btn2->text("Remove Data");
            }
            else
            {
                line->remove_data(M_PI * 5);
                btn2->text("Add Data");
            }
        });
    }
};

int main(int argc, char** argv)
{
    egt::Application app(argc, argv, "charts");

    egt::TopWindow win;

    egt::VerticalBoxSizer vsizer(win);
    expand(vsizer);

    auto frame = std::make_shared<egt::Frame>(egt::Size(win.width(), win.height() * 0.15));
    vsizer.add(egt::expand_horizontal(frame));

    auto logo = std::make_shared<egt::ImageLabel>(egt::Image("icon:egt_logo_black.png;128"));
    logo->align(egt::AlignFlag::center);
    frame->add(logo);

    std::vector<std::pair<std::string, std::function<std::unique_ptr<egt::Theme>()>>> combo_items =
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
                global_theme(i.second());
                break;
            }
        }
        win.damage();
    });

    egt::BoxSizer hsizer(egt::Orientation::horizontal);
    vsizer.add(expand(hsizer));

    egt::ListBox list(egt::Size(win.width() * 0.20, 0));
    list.align(egt::AlignFlag::expand_vertical | egt::AlignFlag::left);
    list.add_item(std::make_shared<egt::StringItem>("Sine Chart"));
    list.add_item(std::make_shared<egt::StringItem>("Cosine Chart"));
    list.add_item(std::make_shared<egt::StringItem>("Tangent Chart"));
    list.add_item(std::make_shared<egt::StringItem>("Points"));
    list.add_item(std::make_shared<egt::StringItem>("VerticalBarPage"));
    list.add_item(std::make_shared<egt::StringItem>("HorizontalBarPage"));
    list.add_item(std::make_shared<egt::StringItem>("PiePage"));
    hsizer.add(list);

    egt::Notebook notebook;
    hsizer.add(expand(notebook));
    egt::Size size(win.width() * 0.80, win.height() * 0.85);
    notebook.add(std::make_shared<SinePage>(size));
    notebook.add(std::make_shared<CosinePage>(size));
    notebook.add(std::make_shared<TanPage>(size));
    notebook.add(std::make_shared<Points>(size));
    notebook.add(std::make_shared<VerticalBarPage>(size));
    notebook.add(std::make_shared<HorizontalBarPage>(size));
    notebook.add(std::make_shared<PiePage>(size));

    list.on_selected_changed([&]()
    {
        notebook.selected(list.selected());
    });

    win.show();

    return app.run();
}
