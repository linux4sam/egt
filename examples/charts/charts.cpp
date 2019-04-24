/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <cmath>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>

using namespace std;
using namespace egt;

static LineChart::data_array create_sin_data()
{
    LineChart::data_array data;
    for (float i = 0.; i < M_PI * 4; i += 0.2)
    {
        LineChart::DataPair p;
        p.x = i;
        p.y = std::sin(i);
        data.push_back(p);
    }
    return data;
}

static LineChart::data_array create_cos_data()
{
    LineChart::data_array data;
    for (float i = 0.; i < M_PI * 4; i += 0.2)
    {
        LineChart::DataPair p;
        p.x = i;
        p.y = std::cos(i);
        data.push_back(p);
    }
    return data;
}

static LineChart::data_array create_atan_data()
{
    LineChart::data_array data;
    for (float i = 0.; i < M_PI * 4; i += 0.2)
    {
        LineChart::DataPair p;
        p.x = i;
        p.y = std::atan(i);
        data.push_back(p);
    }
    return data;
}

struct LinePage : public NotebookTab
{
    LinePage()
    {
        auto line = std::make_shared<LineChart>(Rect(50, 10, 600, 400));
        line->set_name("linechart");
        line->instance_palette().set(Palette::ColorId::bg, Palette::GroupId::normal, Palette::black);
        add(line);

        auto sin_checkbox = std::make_shared<CheckBox>("sin", Rect(Point(0, 410), Size(80, 40)));
        sin_checkbox->instance_palette().set(Palette::ColorId::text, Palette::GroupId::normal, Palette::white);
        sin_checkbox->set_name("sin");
        add(sin_checkbox);

        auto cos_checkbox = std::make_shared<CheckBox>("cos", Rect(Point(100, 410), Size(80, 40)));
        cos_checkbox->instance_palette().set(Palette::ColorId::text, Palette::GroupId::normal, Palette::white);
        cos_checkbox->set_name("cos");
        add(cos_checkbox);

        auto atan_checkbox = std::make_shared<CheckBox>("atan", Rect(Point(200, 410), Size(80, 40)));
        atan_checkbox->instance_palette().set(Palette::ColorId::text, Palette::GroupId::normal, Palette::white);
        atan_checkbox->set_name("atan");
        add(atan_checkbox);

        auto handle_checkbox = [line, sin_checkbox, cos_checkbox, atan_checkbox](eventid event)
        {
            if (event == eventid::property_changed)
            {
                line->clear();
                if (sin_checkbox->checked())
                    line->add_data(create_sin_data(), LineChart::chart_type::points);
                if (cos_checkbox->checked())
                    line->add_data(create_cos_data(), LineChart::chart_type::lines);
                if (atan_checkbox->checked())
                    line->add_data(create_atan_data(), LineChart::chart_type::linespoints);
            }

            return 0;
        };

        sin_checkbox->on_event(handle_checkbox);
        cos_checkbox->on_event(handle_checkbox);
        atan_checkbox->on_event(handle_checkbox);

        sin_checkbox->check(true);
        cos_checkbox->check(true);

        auto line_width = std::make_shared<Slider>(Rect(Point(300, 410), Size(200, 40)), 1, 10, 1);
        line_width->set_name("line_width");
        add(line_width);

        line_width->on_event([line, line_width](eventid event)
        {
            if (event == eventid::property_changed)
            {
                line->set_line_width(line_width->value());
            }
            return 0;
        });

        line_width->set_value(2);

        auto gridx_checkbox = std::make_shared<CheckBox>("grid X", Rect(Point(500, 410), Size(80, 40)));
        gridx_checkbox->instance_palette().set(Palette::ColorId::text, Palette::GroupId::normal, Palette::white);
        add(gridx_checkbox);

        auto gridy_checkbox = std::make_shared<CheckBox>("grid Y", Rect(Point(600, 410), Size(80, 40)));
        gridy_checkbox->instance_palette().set(Palette::ColorId::text, Palette::GroupId::normal, Palette::white);
        add(gridy_checkbox);

        auto handle_grid_checkbox = [gridx_checkbox, gridy_checkbox, line](eventid event)
        {
            if (event == eventid::property_changed)
            {
                uint32_t flags = 0;
                if (gridx_checkbox->checked())
                    flags |= LineChart::GRIDX;
                if (gridy_checkbox->checked())
                    flags |= LineChart::GRIDY;
                line->set_grid(flags);
            }
            return 0;
        };

        gridx_checkbox->on_event(handle_grid_checkbox);
        gridy_checkbox->on_event(handle_grid_checkbox);
    }
};

struct PiePage : public NotebookTab
{
    PiePage()
    {
        auto pie = std::make_shared<PieChart>(Rect(10, 10, 600, 400));
        std::map<std::string, float> data;
        data.insert(make_pair("truck", .25));
        data.insert(make_pair("car", .55));
        data.insert(make_pair("bike", .10));
        data.insert(make_pair("motorcycle", .05));
        data.insert(make_pair("plane", .05));
        pie->data(data);

        add(pie);
    }
};

int main(int argc, const char** argv)
{
    Application app(argc, argv, "charts");

    global_theme().palette().set(Palette::ColorId::bg, Palette::GroupId::normal, Palette::black);
    global_theme().palette().set(Palette::ColorId::text, Palette::GroupId::normal, Palette::white);
    global_theme().palette().set(Palette::ColorId::text, Palette::GroupId::active, Palette::black);

    TopWindow win;

    BoxSizer hsizer(orientation::horizontal);
    hsizer.set_align(alignmask::expand);
    win.add(hsizer);

    ListBox list(Size(win.w() * 0.15, 0));
    list.set_align(alignmask::expand_vertical | alignmask::left);
    list.add_item(make_shared<StringItem>("Line"));
    list.add_item(make_shared<StringItem>("Pie"));
    hsizer.add(list);

    Notebook notebook;
    notebook.set_align(alignmask::expand);
    hsizer.add(notebook);
    notebook.add(make_shared<LinePage>());
    notebook.add(make_shared<PiePage>());

    list.on_event([&](eventid event)
    {
        ignoreparam(event);
        notebook.set_select(list.selected());
        return 1;
    }, {eventid::property_changed});

    win.show();

    return app.run();
}
