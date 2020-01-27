/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/themes/coconut.h>
#include <egt/themes/lapis.h>
#include <egt/themes/midnight.h>
#include <egt/themes/shamrock.h>
#include <egt/themes/sky.h>
#include <egt/themes/ultraviolet.h>
#include <egt/ui>
#include <cmath>
#include <iostream>
#include <map>
#include <random>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace egt;


static int random_item(int start, int end)
{
    std::random_device r;
    std::default_random_engine e {r()};
    std::uniform_int_distribution<int> dist(start, end);
    return dist(e);
}

struct PiePage : public NotebookTab
{
    PiePage(Size size)
    {
        auto sizer = std::make_shared<VerticalBoxSizer>();
        add(expand(sizer));

        auto pie = std::make_shared<PieChart>();
        pie->title("Pie Chart Example");

        PieChart::DataArray data;
        const int pdata[] = { 5, 10, 15, 20, 4, 8, 16, 12};
        int len = sizeof(pdata) / pdata[0];
        for (int i = 0; i < len; i++)
        {
            data.push_back(make_pair(pdata[i], "label" + std::to_string(i)));
        }
        pie->data(data);
        sizer->add(expand(pie));

        auto csizer = std::make_shared<HorizontalBoxSizer>();
        sizer->add(csizer);

        auto btn1 = make_shared<Button>("Add Data");
        csizer->add(btn1);
        btn1->on_click([pie, btn1](Event&)
        {
            if (!btn1->text().compare("Add Data"))
            {
                PieChart::DataArray data1;
                static int i = 1;
                data1.push_back(make_pair(random_item(1, 25), "label" + std::to_string(++i)));
                data1.push_back(make_pair(random_item(1, 10), "label" + std::to_string(++i)));
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

struct Points : public NotebookTab
{
    Points(Size size)
    {
        auto sizer = std::make_shared<VerticalBoxSizer>();
        add(expand(sizer));

        auto line = std::make_shared<PointChart>();
        line->label("x-axis", "y-axis", "Point Chart Example");
        sizer->add(expand(line));

        auto csizer = std::make_shared<HorizontalBoxSizer>();
        sizer->add(csizer);

        PointChart::DataArray data;
        for (int i = 0; i < 101; i++)
        {
            data.push_back(make_pair(i, random_item(1, 100)));
        }
        line->data(data);

        auto ticks_checkbox = std::make_shared<CheckBox>("show ticks");
        csizer->add(ticks_checkbox);

        ticks_checkbox->on_checked_changed([line, ticks_checkbox]()
        {
            if (ticks_checkbox->checked())
                line->show_ticks(true);
            else
                line->show_ticks(false);
        });

        ticks_checkbox->checked(true);

        auto gridy_checkbox = std::make_shared<CheckBox>("show grid");
        csizer->add(gridy_checkbox);

        gridy_checkbox->on_checked_changed([line, gridy_checkbox]()
        {
            if (gridy_checkbox->checked())
                line->show_grid(true);
            else
                line->show_grid(false);
        });

        auto point_type = std::make_shared<Slider>(Size(size.width() * 0.25, size.height() * 0.10), 1, 4, 1);
        point_type->slider_flags().set({Slider::SliderFlag::round_handle});
        point_type->name("point type");
        point_type->margin(5);
        csizer->add(point_type);

        point_type->on_value_changed([line, point_type]()
        {
            switch (point_type->value())
            {
            case 1:
                line->point_type(PointChart::PointType::dot);
                break;
            case 2:
                line->point_type(PointChart::PointType::star);
                break;
            case 3:
                line->point_type(PointChart::PointType::cross);
                break;
            default:
                line->point_type(PointChart::PointType::circle);
                break;
            }
        });

        point_type->value(1);

        auto btn2 = make_shared<Button>("Add Data");
        csizer->add(btn2);
        btn2->on_click([line, btn2](Event & event)
        {
            if (!btn2->text().compare("Add Data"))
            {
                PointChart::DataArray data1;
                static int n = 101;
                for (int i = n ; i < (n + 10); i++)
                {
                    data1.push_back(make_pair(i, random_item(1, 125)));
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

struct HorizontalBarPage : public NotebookTab
{
    HorizontalBarPage(Size size)
    {
        auto sizer = std::make_shared<VerticalBoxSizer>();
        add(expand(sizer));

        auto bar = std::make_shared<HorizontalBarChart>();
        bar->label("Widget Sales (millions)", "---- Months ---- ", "Horizontal BarChart Example");
        sizer->add(expand(bar));

        std::string x[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
        HorizontalBarChart::DataArray data;
        for (int i = 0; i < 12; i++)
        {
            data.push_back(make_pair(random_item(1, 50), x[i]));
        }
        bar->data(data);

        auto csizer = std::make_shared<HorizontalBoxSizer>();
        sizer->add(csizer);

        auto btn2 = make_shared<Button>("Remove Data");
        csizer->add(btn2);
        btn2->on_click([btn2, bar, x](Event & event)
        {
            HorizontalBarChart::DataArray data1;
            if (!btn2->text().compare("Add Data"))
            {
                static int i = 0;
                data1.push_back(make_pair(random_item(1, 150), x[i++]));
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

        auto Bar_pattern = std::make_shared<Slider>(Size(size.width() * 0.25, size.height() * 0.10), 1, 4, 1);
        Bar_pattern->slider_flags().set({Slider::SliderFlag::round_handle});
        Bar_pattern->margin(5);
        Bar_pattern->name("HorizontalBar pattern");
        csizer->add(Bar_pattern);

        Bar_pattern->on_value_changed([bar, Bar_pattern]()
        {
            switch (Bar_pattern->value())
            {
            case 1:
                bar->bar_pattern(HorizontalBarChart::BarPattern::soild);
                break;
            case 2:
                bar->bar_pattern(HorizontalBarChart::BarPattern::horizontal_line);
                break;
            case 3:
                bar->bar_pattern(HorizontalBarChart::BarPattern::vertical_line);
                break;
            default:
                bar->bar_pattern(HorizontalBarChart::BarPattern::boxes);
                break;
            }
        });

        Bar_pattern->value(1);

    }
};

struct VerticalBarPage : public NotebookTab
{
    VerticalBarPage(Size size)
    {
        auto sizer = std::make_shared<VerticalBoxSizer>();
        add(expand(sizer));

        auto bar = std::make_shared<BarChart>();
        bar->label("---- Year ---- ", "Widget Sales (millions)", "Vertical BarChart Example");
        sizer->add(expand(bar));

        BarChart::DataArray data;
        for (int i = 0; i < 10; i++)
        {
            data.push_back(make_pair((1980 + i), random_item(1, 50)));
        }
        bar->data(data);

        auto csizer = std::make_shared<HorizontalBoxSizer>();
        sizer->add(csizer);

        auto ticks_checkbox = std::make_shared<CheckBox>("show ticks");
        csizer->add(ticks_checkbox);

        ticks_checkbox->on_checked_changed([bar, ticks_checkbox]()
        {
            if (ticks_checkbox->checked())
                bar->show_ticks(true);
            else
                bar->show_ticks(false);

        });

        ticks_checkbox->checked(true);

        auto gridy_checkbox = std::make_shared<CheckBox>("show grid");
        csizer->add(gridy_checkbox);

        gridy_checkbox->on_checked_changed([bar, gridy_checkbox]()
        {
            if (gridy_checkbox->checked())
                bar->show_grid(true);
            else
                bar->show_grid(false);

        });

        auto Bar_pattern = std::make_shared<Slider>(Size(size.width() * 0.25, size.height() * 0.10), 1, 4, 1);
        Bar_pattern->slider_flags().set({Slider::SliderFlag::round_handle});
        Bar_pattern->margin(5);
        Bar_pattern->name("Bar pattern");
        csizer->add(Bar_pattern);

        Bar_pattern->on_value_changed([bar, Bar_pattern]()
        {
            switch (Bar_pattern->value())
            {
            case 1:
                bar->bar_pattern(BarChart::BarPattern::soild);
                break;
            case 2:
                bar->bar_pattern(BarChart::BarPattern::horizontal_line);
                break;
            case 3:
                bar->bar_pattern(BarChart::BarPattern::vertical_line);
                break;
            default:
                bar->bar_pattern(BarChart::BarPattern::boxes);
                break;
            }
        });

        Bar_pattern->value(1);

        auto btn2 = make_shared<Button>("Add Data");
        csizer->add(btn2);
        btn2->on_click([bar, btn2](Event & event)
        {
            if (!btn2->text().compare("Add Data"))
            {
                BarChart::DataArray data1;
                static int year = 1990;
                data1.push_back(make_pair(year, random_item(1, 50)));
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

struct TanPage : public NotebookTab
{
    TanPage(Size size)
    {
        auto sizer = std::make_shared<VerticalBoxSizer>();
        add(expand(sizer));

        auto line = std::make_shared<LineChart>();
        line->label("x-axis", "y-axis", "Tangent Chart Example");
        sizer->add(expand(line));

        LineChart::DataArray data;
        for (float i = 0.; i < M_PI * 16; i += 0.2)
        {
            data.push_back(make_pair(i, std::atan(i)));
        }
        line->data(data);

        auto csizer = std::make_shared<HorizontalBoxSizer>();
        sizer->add(csizer);

        auto line_width = std::make_shared<Slider>(Size(size.width() * 0.15, size.height() * 0.10), 1, 5, 1);
        line_width->slider_flags().set({Slider::SliderFlag::round_handle});
        line_width->margin(5);
        line_width->name("line_width");
        csizer->add(line_width);

        line_width->on_value_changed([line, line_width]()
        {
            line->line_width(line_width->value());
        });

        line_width->value(2);

        auto line_pattern = std::make_shared<Slider>(Size(size.width() * 0.15, size.height() * 0.10), 1, 3, 1);
        line_pattern->slider_flags().set({Slider::SliderFlag::round_handle});
        line_pattern->name("line_pattern");
        line_pattern->margin(5);
        csizer->add(line_pattern);

        line_pattern->on_value_changed([line, line_pattern]()
        {
            switch (line_pattern->value())
            {
            case 1:
                line->line_pattern(LineChart::LinePattern::solid);
                break;
            case 2:
                line->line_pattern(LineChart::LinePattern::dotted);
                break;
            case 3:
                line->line_pattern(LineChart::LinePattern::dashes);
                break;
            }
        });

        line_pattern->value(1);

        auto ticks_checkbox = std::make_shared<CheckBox>("show ticks");
        csizer->add(ticks_checkbox);

        ticks_checkbox->on_checked_changed([line, ticks_checkbox]()
        {
            if (ticks_checkbox->checked())
                line->show_ticks(true);
            else
                line->show_ticks(false);

        });

        ticks_checkbox->checked(true);

        auto gridy_checkbox = std::make_shared<CheckBox>("show grid");
        csizer->add(gridy_checkbox);

        gridy_checkbox->on_checked_changed([line, gridy_checkbox]()
        {
            if (gridy_checkbox->checked())
                line->show_grid(true);
            else
                line->show_grid(false);

        });

        gridy_checkbox->checked(true);

        auto btn2 = make_shared<Button>("Add Data");
        csizer->add(btn2);
        btn2->on_click([btn2, line](Event & event)
        {
            if (!btn2->text().compare("Add Data"))
            {
                LineChart::DataArray data1;
                static int tsize = 17;
                float i = M_PI * 16;
                for (; i < M_PI * tsize; i += 0.2)
                {
                    data1.push_back(make_pair(i, std::atan(i)));
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

struct SinePage : public NotebookTab
{
    SinePage(Size size)
    {
        auto sizer = std::make_shared<VerticalBoxSizer>();
        add(expand(sizer));

        auto line = std::make_shared<LineChart>();
        line->label("x-axis", "y-axis", "Sine Chart Example");
        sizer->add(expand(line));

        LineChart::DataArray data;
        for (float i = 0.; i < M_PI * 2; i += 0.2)
        {
            data.push_back(make_pair(i, std::sin(i)));
        }
        line->data(data);

        auto csizer = std::make_shared<HorizontalBoxSizer>();
        sizer->add(csizer);

        auto line_width = std::make_shared<Slider>(Size(size.width() * 0.15, size.height() * 0.10), 1, 5, 1);
        line_width->slider_flags().set({Slider::SliderFlag::round_handle});
        line_width->name("line_width");
        line_width->margin(5);
        csizer->add(line_width);

        line_width->on_value_changed([line, line_width]()
        {
            line->line_width(line_width->value());
        });

        line_width->value(2);

        auto line_pattern = std::make_shared<Slider>(Size(size.width() * 0.15, size.height() * 0.10), 1, 3, 1);
        line_pattern->slider_flags().set({Slider::SliderFlag::round_handle});
        line_pattern->name("line_pattern");
        line_pattern->margin(5);
        csizer->add(line_pattern);

        line_pattern->on_value_changed([line, line_pattern]()
        {
            switch (line_pattern->value())
            {
            case 1:
                line->line_pattern(LineChart::LinePattern::solid);
                break;
            case 2:
                line->line_pattern(LineChart::LinePattern::dotted);
                break;
            case 3:
                line->line_pattern(LineChart::LinePattern::dashes);
                break;
            }
        });

        line_pattern->value(1);

        auto ticks_checkbox = std::make_shared<CheckBox>("show ticks");
        csizer->add(ticks_checkbox);

        ticks_checkbox->on_checked_changed([line, ticks_checkbox]()
        {
            if (ticks_checkbox->checked())
                line->show_ticks(true);
            else
                line->show_ticks(false);
        });

        ticks_checkbox->checked(true);

        auto gridy_checkbox = std::make_shared<CheckBox>("show grid");
        csizer->add(gridy_checkbox);

        gridy_checkbox->on_checked_changed([line, gridy_checkbox]()
        {
            if (gridy_checkbox->checked())
                line->show_grid(true);
            else
                line->show_grid(false);
        });

        auto btn2 = make_shared<Button>("Add Data");
        csizer->add(btn2);
        btn2->on_click([btn2, line](Event & event)
        {
            if (!btn2->text().compare("Add Data"))
            {
                LineChart::DataArray data1;
                static int ssize = 4;
                static float i = M_PI * 2;
                for (; i < M_PI * ssize; i += 0.2)
                {
                    data1.push_back(make_pair(i, std::sin(i)));
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

struct CosinePage : public NotebookTab
{
    CosinePage(Size size)
    {
        auto sizer = std::make_shared<VerticalBoxSizer>();
        add(expand(sizer));

        auto line = std::make_shared<LineChart>();
        line->label("x-axis", "y-axis", "Cosine Chart Example");
        sizer->add(expand(line));

        LineChart::DataArray data;
        for (float i = 0.; i < M_PI * 4; i += 0.2)
        {
            data.push_back(make_pair(i, std::cos(i)));
        }
        line->data(data);

        auto csizer = std::make_shared<HorizontalBoxSizer>();
        sizer->add(csizer);

        auto line_width = std::make_shared<Slider>(Size(size.width() * 0.15, size.height() * 0.10), 1, 5, 1);
        line_width->slider_flags().set({Slider::SliderFlag::round_handle});
        line_width->name("line_width");
        line_width->margin(5);
        csizer->add(line_width);

        line_width->on_value_changed([line, line_width]()
        {
            line->line_width(line_width->value());
        });
        line_width->value(2);

        auto line_pattern = std::make_shared<Slider>(Size(size.width() * 0.15, size.height() * 0.10), 1, 3, 1);
        line_pattern->slider_flags().set({Slider::SliderFlag::round_handle});
        line_pattern->name("line_pattern");
        line_pattern->margin(5);
        csizer->add(line_pattern);

        line_pattern->on_value_changed([line, line_pattern]()
        {
            switch (line_pattern->value())
            {
            case 1:
                line->line_pattern(LineChart::LinePattern::solid);
                break;
            case 2:
                line->line_pattern(LineChart::LinePattern::dotted);
                break;
            case 3:
                line->line_pattern(LineChart::LinePattern::dashes);
                break;
            }
        });
        line_pattern->value(1);

        auto ticks_checkbox = std::make_shared<CheckBox>("show ticks");
        csizer->add(ticks_checkbox);

        ticks_checkbox->on_checked_changed([line, ticks_checkbox]()
        {
            if (ticks_checkbox->checked())
                line->show_ticks(true);
            else
                line->show_ticks(false);

        });

        ticks_checkbox->checked(true);

        auto gridy_checkbox = std::make_shared<CheckBox>("show grid");
        csizer->add(gridy_checkbox);

        gridy_checkbox->on_checked_changed([line, gridy_checkbox]()
        {
            if (gridy_checkbox->checked())
                line->show_grid(true);
            else
                line->show_grid(false);

        });

        auto btn2 = make_shared<Button>("Add Data");
        csizer->add(btn2);
        btn2->on_click([btn2, line](Event & event)
        {
            if (!btn2->text().compare("Add Data"))
            {
                LineChart::DataArray data1;
                static float i = M_PI * 4;
                static int csize = 9;
                for (; i < M_PI * csize; i += 0.2)
                {
                    data1.push_back(make_pair(i, std::cos(i)));
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

int main(int argc, const char** argv)
{
    Application app(argc, argv, "charts");

    TopWindow win;

    VerticalBoxSizer vsizer(win);
    expand(vsizer);

    auto frame = make_shared<Frame>(Size(win.width(), win.height() * 0.15));
    vsizer.add(expand_horizontal(frame));

    auto logo = make_shared<ImageLabel>(Image("@128px/egt_logo_black.png"));
    logo->align(AlignFlag::center);
    frame->add(logo);

    vector<std::pair<std::string, std::function<std::unique_ptr<Theme>()>>> combo_items =
    {
        {"Default Theme", []{ return detail::make_unique<Theme>(); }},
        {"Lapis", []{ return detail::make_unique<LapisTheme>(); }},
        {"Midnight", []{ return detail::make_unique<MidnightTheme>(); }},
        {"Sky", []{ return detail::make_unique<SkyTheme>(); }},
        {"Shamrock", []{ return detail::make_unique<ShamrockTheme>(); }},
        {"Coconut", []{ return detail::make_unique<CoconutTheme>(); }},
        {"Ultra Violet", []{ return detail::make_unique<UltraVioletTheme>(); }}
    };

    auto combo = make_shared<ComboBox>();
    for (auto& i : combo_items)
        combo->add_item(i.first);
    combo->align(AlignFlag::center_vertical | AlignFlag::right);
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

    BoxSizer hsizer(Orientation::horizontal);
    vsizer.add(expand(hsizer));

    ListBox list(Size(win.width() * 0.20, 0));
    list.align(AlignFlag::expand_vertical | AlignFlag::left);
    list.add_item(make_shared<StringItem>("Sine Chart"));
    list.add_item(make_shared<StringItem>("Cosine Chart"));
    list.add_item(make_shared<StringItem>("Tangent Chart"));
    list.add_item(make_shared<StringItem>("Points"));
    list.add_item(make_shared<StringItem>("VerticalBarPage"));
    list.add_item(make_shared<StringItem>("HorizontalBarPage"));
    list.add_item(make_shared<StringItem>("PiePage"));
    hsizer.add(list);

    Notebook notebook;
    hsizer.add(expand(notebook));
    Size size(win.width() * 0.80, win.height() * 0.85);
    notebook.add(make_shared<SinePage>(size));
    notebook.add(make_shared<CosinePage>(size));
    notebook.add(make_shared<TanPage>(size));
    notebook.add(make_shared<Points>(size));
    notebook.add(make_shared<VerticalBarPage>(size));
    notebook.add(make_shared<HorizontalBarPage>(size));
    notebook.add(make_shared<PiePage>(size));

    list.on_selected_changed([&]()
    {
        notebook.selected(list.selected());
    });

    win.show();

    return app.run();
}
