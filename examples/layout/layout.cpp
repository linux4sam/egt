/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <egt/detail/serialize.h>
#include <egt/form.h>
#include <egt/shapes.h>
#include <egt/themes/sky.h>
#include <egt/themes/midnight.h>
#include <egt/themes/shamrock.h>
#include <egt/ui>
#include <memory>

using namespace egt;
using namespace egt::experimental;
using namespace egt::detail;
using namespace std;

using create_function = std::function<shared_ptr<Widget>()>;

struct widget_types
{
    std::string name;
    std::string icon;
    create_function create;
};

template <class T>
shared_ptr<Widget> create_widget()
{
    auto instance = make_shared<T>();
    return static_pointer_cast<Widget>(instance);
}

template <>
shared_ptr<Widget> create_widget<Button>()
{
    auto instance = make_shared<Button>("Button");
    return static_pointer_cast<Widget>(instance);
}

template <>
shared_ptr<Widget> create_widget<Label>()
{
    auto instance = make_shared<Label>("Label");
    return static_pointer_cast<Widget>(instance);
}

template <>
shared_ptr<Widget> create_widget<CheckBox>()
{
    auto instance = make_shared<CheckBox>("CheckBox");
    return static_pointer_cast<Widget>(instance);
}

template <>
shared_ptr<Widget> create_widget<TextBox>()
{
    auto instance = make_shared<TextBox>("Text");
    return static_pointer_cast<Widget>(instance);
}

template <>
shared_ptr<Widget> create_widget<RadioBox>()
{
    auto instance = make_shared<RadioBox>("RadioBox");
    return static_pointer_cast<Widget>(instance);
}

template <>
shared_ptr<Widget> create_widget<ComboBox>()
{
    ComboBox::item_array combo_items;
    for (auto x = 0; x < 5; x++)
        combo_items.push_back("item " + std::to_string(x));

    auto instance = make_shared<ComboBox>(combo_items);
    return static_pointer_cast<Widget>(instance);
}

template <>
shared_ptr<Widget> create_widget<ListBox>()
{
    auto instance = make_shared<ListBox>();

    for (auto x = 0; x < 5; x++)
        instance->add_item(std::make_shared<StringItem>("item " + std::to_string(x)));

    instance->resize(Size(100, 200));

    return static_pointer_cast<Widget>(instance);
}

template <>
shared_ptr<Widget> create_widget<ImageLabel>()
{
    auto instance = make_shared<ImageLabel>(Image("icons/image_default.png"));
    instance->set_image_align(alignmask::expand);
    return static_pointer_cast<Widget>(instance);
}

template <>
shared_ptr<Widget> create_widget<ScrolledView>()
{
    auto instance = make_shared<ScrolledView>();
    instance->resize(Size(200, 200));
    return static_pointer_cast<Widget>(instance);
}

template <>
shared_ptr<Widget> create_widget<StaticGrid>()
{
    auto instance = make_shared<StaticGrid>(Tuple(2, 2), 1);
    instance->resize(Size(200, 200));
    instance->set_border(instance->theme().default_border());
    return static_pointer_cast<Widget>(instance);
}

template <>
shared_ptr<Widget> create_widget<SelectableGrid>()
{
    auto instance = make_shared<SelectableGrid>(Tuple(2, 2), 1);
    instance->resize(Size(200, 200));
    instance->set_border(instance->theme().default_border());
    return static_pointer_cast<Widget>(instance);
}

template <>
shared_ptr<Widget> create_widget<Slider>()
{
    auto instance = make_shared<Slider>();
    instance->set_value(10);
    return static_pointer_cast<Widget>(instance);
}

template <>
shared_ptr<Widget> create_widget<PieChart>()
{
    auto instance = make_shared<PieChart>();

    std::map<std::string, float> data;
    data.insert(make_pair("truck", .25));
    data.insert(make_pair("car", .55));
    data.insert(make_pair("bike", .10));
    data.insert(make_pair("motorcycle", .05));
    data.insert(make_pair("plane", .05));
    instance->data(data);

    return static_pointer_cast<Widget>(instance);
}

static vector<widget_types> widgets =
{
    { "frame", "icons/frame.png", create_widget<Frame> },
    { "vsizer", "icons/vertical.png", create_widget<VerticalBoxSizer> },
    { "hsizer", "icons/horizontal.png", create_widget<HorizontalBoxSizer> },
    { "flexsizer", "icons/flex.png", create_widget<FlexBoxSizer> },
    { "grid", "icons/grid.png", create_widget<StaticGrid>},
    { "grid", "icons/selectgrid.png", create_widget<SelectableGrid>},
    { "view", "icons/view.png", create_widget<ScrolledView>},
    { "circle", "icons/circle.png", create_widget<CircleWidget>},
    { "line", "icons/line.png", create_widget<LineWidget>},
    { "square", "icons/square.png", create_widget<RectangleWidget>},
    { "buttton", "icons/button.png", create_widget<Button> },
    { "label", "icons/label.png", create_widget<Label> },
    { "textbox", "icons/textbox.png", create_widget<TextBox>},
    { "listbox", "icons/listbox.png", create_widget<ListBox>},
    { "imagelabel", "icons/imagelabel.png", create_widget<ImageLabel>},
    { "radiobox", "icons/radiobox.png", create_widget<RadioBox>},
    { "combobox", "icons/combobox.png", create_widget<ComboBox>},
    { "checkbox", "icons/checkbox.png", create_widget<CheckBox>},
    { "togglebox", "icons/togglebox.png", create_widget<ToggleBox>},
    { "slider", "icons/slider.png", create_widget<Slider>},
    { "progressbar", "icons/progress.png", create_widget<ProgressBar>},
    { "spinprogress", "icons/spinprogress.png", create_widget<SpinProgress>},
    { "levelmeter", "icons/levelmeter.png", create_widget<LevelMeter>},
    { "analogmeter", "icons/analogmeter.png", create_widget<AnalogMeter>},
    { "linechart", "icons/linechart.png", create_widget<LineChart>},
    { "piechart", "icons/piechart.png", create_widget<PieChart>},
    { "radial", "icons/radial.png", create_widget<RadialType<int>>},
};

template<class T>
shared_ptr<Widget> create_option(std::function<void(T)> set,
                                 std::function<T()> get)
{
    auto x = make_shared<TextBox>(std::to_string(get()));
    x->on_event([x, set](Event&)
    {
        if (!x->text().empty())
            set(std::stoi(x->text()));
    }, {eventid::property_changed});
    return static_pointer_cast<Widget>(x);
}

template<>
shared_ptr<Widget> create_option(std::function<void(string)> set,
                                 std::function<string()> get)
{
    auto x = make_shared<TextBox>(get());
    x->on_event([x, set](Event&)
    {
        if (!x->text().empty())
            set(x->text());
    }, {eventid::property_changed});
    return static_pointer_cast<Widget>(x);
}

class MainWindow : public TopWindow
{
public:

    MainWindow()
    {
        auto vsizer = make_shared<BoxSizer>(orientation::vertical);
        add(expand(vsizer));

        auto frame = make_shared<Frame>(Size(0, 60));
        vsizer->add(expand_horizontal(frame));

        auto logo = make_shared<ImageLabel>(Image("@128px/egt_logo_black.png"));
        logo->set_margin(1);
        logo->set_align(alignmask::center);
        frame->add(logo);

        ComboBox::item_array combo_items =
        {
            "Default",
            "Midnight",
            "Sky",
            "Shamrock"
        };
        auto combo = make_shared<ComboBox>(combo_items);
        combo->set_align(alignmask::center_vertical | alignmask::right);
        combo->set_margin(5);
        frame->add(combo);

        combo->on_event([combo, this](Event&)
        {
            auto s = combo->item_at(combo->selected());

            if (s == "Default")
                set_global_theme(new Theme());
            else if (s == "Midnight")
                set_global_theme(new MidnightTheme());
            else if (s == "Sky")
                set_global_theme(new SkyTheme());
            else if (s == "Shamrock")
                set_global_theme(new ShamrockTheme());

            this->damage();
        }, {eventid::property_changed});

        auto hsizer = make_shared<BoxSizer>(orientation::horizontal);
        vsizer->add(expand(hsizer));

        m_grid = make_shared<SelectableGrid>(Tuple(3, 10), 5);
        m_grid->set_horizontal_ratio(15);
        m_grid->resize(Size(200, 0));
        hsizer->add(expand_vertical(m_grid));

        for (auto& w : widgets)
        {
            auto btn = make_shared<ImageButton>(Image(w.icon));
            btn->set_boxtype(Theme::boxtype::none);
            m_grid->add(egt::center(btn));
        }

        m_canvas = make_shared<Frame>();
        m_canvas->set_border(global_theme().default_border());
        m_canvas->set_boxtype(Theme::boxtype::blank);
        m_canvas->set_align(alignmask::expand);
        m_canvas->set_horizontal_ratio(80);
        m_canvas->show();
        hsizer->add(m_canvas);

        m_canvas->set_special_child_draw_callback([](Painter & painter, Widget * widget)
        {
            Painter::AutoSaveRestore sr3(painter);
            auto cr = painter.context().get();
            auto b = widget->box();
            b += Point(2 / 2., 2 / 2.);
            b -= Size(2, 2);
            painter.draw(b);
            painter.set(Palette::blue);
            static const double dashed[] = {1.0};
            cairo_set_dash(cr, dashed, 1, 0);
            painter.set_line_width(2);
            cairo_stroke(cr);
        });

        m_form = make_shared<Form>("Properties");
        m_form->resize(Size(200, 0));
        m_form->set_align(alignmask::expand_vertical);
        m_form->set_horizontal_ratio(100);
        hsizer->add(m_form);

        Input::global_input().on_event([this](Event & event)
        {
            switch (event.key().keycode)
            {
            case EKEY_DELETE:
                if (m_selected)
                {
                    m_selected->detach();
                    m_selected = nullptr;
                    m_form->remove_options();
                }
                break;
            default:
                break;
            }
        }, {eventid::keyboard_down});
    }

    void handle(Event& event) override
    {
        TopWindow::handle(event);

        switch (event.id())
        {
        case eventid::pointer_click:
        {
            if (event.pointer().btn == Pointer::button::right)
            {
                Point pos = display_to_local(event.pointer().point);
                if (Rect::point_inside(pos, m_canvas->box()))
                {
                    auto parent = dynamic_cast<Frame*>(m_canvas->hit_test(event.pointer().point));
                    if (parent && !parent->flags().is_set(Widget::flag::frame))
                        parent = nullptr;
                    auto p = m_grid->selected();
                    auto index = p.x() + 3 * p.y();
                    auto widget = widgets[index].create();
                    add_widget(widget, event.pointer().point, parent);
                    event.stop();
                }
            }
            break;
        }
        default:
            break;
        }
    }

    template<class T>
    void show_properties(T* widget)
    {
        m_form->remove_options();

        m_form->add_group("Widget");
        m_form->add_option("Name",
                           create_option<string>(std::bind(&Widget::set_name, widget, std::placeholders::_1),
                                   std::bind(&Widget::name, widget)));
        m_form->add_option("X",
                           create_option<int>(std::bind(&Widget::set_x, widget, std::placeholders::_1),
                                              std::bind(&Widget::x, widget)));
        m_form->add_option("Y",
                           create_option<int>(std::bind(&Widget::set_y, widget, std::placeholders::_1),
                                              std::bind(&Widget::y, widget)));
        m_form->add_option("Width",
                           create_option<int>(std::bind(&Widget::set_width, widget, std::placeholders::_1),
                                              std::bind(&Widget::width, widget)));
        m_form->add_option("Height",
                           create_option<int>(std::bind(&Widget::set_height, widget, std::placeholders::_1),
                                              std::bind(&Widget::height, widget)));
        m_form->add_option("Border",
                           create_option<int>(std::bind(&Widget::set_border, widget, std::placeholders::_1),
                                              std::bind(&Widget::border, widget)));
        m_form->add_option("Margin",
                           create_option<int>(std::bind(&Widget::set_margin, widget, std::placeholders::_1),
                                              std::bind(&Widget::margin, widget)));
        m_form->add_option("Padding",
                           create_option<int>(std::bind(&Widget::set_padding, widget, std::placeholders::_1),
                                              std::bind(&Widget::padding, widget)));
        m_form->add_option("X Ratio",
                           create_option<int>(std::bind(&Widget::set_xratio, widget, std::placeholders::_1),
                                              std::bind(&Widget::xratio, widget)));
        m_form->add_option("Y Ratio",
                           create_option<int>(std::bind(&Widget::set_yratio, widget, std::placeholders::_1),
                                              std::bind(&Widget::yratio, widget)));
        m_form->add_option("H Ratio",
                           create_option<int>(std::bind(&Widget::set_horizontal_ratio, widget, std::placeholders::_1),
                                              std::bind(&Widget::horizontal_ratio, widget)));
        m_form->add_option("V Ratio",
                           create_option<int>(std::bind(&Widget::set_vertical_ratio, widget, std::placeholders::_1),
                                              std::bind(&Widget::vertical_ratio, widget)));

        auto left = make_shared<CheckBox>("left");
        if ((widget->align() & alignmask::left) == alignmask::left)
            left->set_checked(true);
        left->on_event([widget, left](Event&)
        {
            if (left->checked())
                widget->set_align(widget->align() | alignmask::left);
            else
                widget->set_align(widget->align() & ~alignmask::left);
        }, {eventid::property_changed});
        m_form->add_option(left);

        auto right = make_shared<CheckBox>("right");
        if ((widget->align() & alignmask::right) == alignmask::right)
            right->set_checked(true);
        right->on_event([widget, right](Event&)
        {
            if (right->checked())
                widget->set_align(widget->align() | alignmask::right);
            else
                widget->set_align(widget->align() & ~alignmask::right);
        }, {eventid::property_changed});
        m_form->add_option(right);

        auto center = make_shared<CheckBox>("center");
        if ((widget->align() & alignmask::center) == alignmask::center)
            center->set_checked(true);
        center->on_event([widget, center](Event&)
        {
            if (center->checked())
                widget->set_align(widget->align() | alignmask::center);
            else
                widget->set_align(widget->align() & ~alignmask::center);
        }, {eventid::property_changed});
        m_form->add_option(center);

        auto top = make_shared<CheckBox>("top");
        if ((widget->align() & alignmask::top) == alignmask::top)
            top->set_checked(true);
        top->on_event([widget, top](Event&)
        {
            if (top->checked())
                widget->set_align(widget->align() | alignmask::top);
            else
                widget->set_align(widget->align() & ~alignmask::top);
        }, {eventid::property_changed});
        m_form->add_option(top);

        auto bottom = make_shared<CheckBox>("bottom");
        if ((widget->align() & alignmask::bottom) == alignmask::bottom)
            bottom->set_checked(true);
        bottom->on_event([widget, bottom](Event&)
        {
            if (bottom->checked())
                widget->set_align(widget->align() | alignmask::bottom);
            else
                widget->set_align(widget->align() & ~alignmask::bottom);
        }, {eventid::property_changed});
        m_form->add_option(bottom);

        auto expand_horizontal = make_shared<CheckBox>("expand horizontal");
        if ((widget->align() & alignmask::expand_horizontal) == alignmask::expand_horizontal)
            expand_horizontal->set_checked(true);
        expand_horizontal->on_event([widget, expand_horizontal](Event&)
        {
            if (expand_horizontal->checked())
                widget->set_align(widget->align() | alignmask::expand_horizontal);
            else
                widget->set_align(widget->align() & ~alignmask::expand_horizontal);
        }, {eventid::property_changed});
        m_form->add_option(expand_horizontal);

        auto expand_vertical = make_shared<CheckBox>("expand vertical");
        if ((widget->align() & alignmask::expand_vertical) == alignmask::expand_vertical)
            expand_vertical->set_checked(true);
        expand_vertical->on_event([widget, expand_vertical](Event&)
        {
            if (expand_vertical->checked())
                widget->set_align(widget->align() | alignmask::expand_vertical);
            else
                widget->set_align(widget->align() & ~alignmask::expand_vertical);
        }, {eventid::property_changed});
        m_form->add_option(expand_vertical);
    }

    void add_widget(const shared_ptr<Widget>& widget, const DisplayPoint& point, Frame* parent)
    {
        if (!parent)
            return;

        cout << "Adding widget at " << point << " to " << parent->name() << endl;

        parent->add(widget);
        widget->move(parent->display_to_local(point));

        if (widget->size().empty())
            widget->resize(widget->min_size_hint());

        if (widget->size().empty())
            widget->resize(Size(100, 100));

        widget->on_event([this, widget](Event & event)
        {
            /// @todo drag is broken if you drag an item over one it is under

            static Point m_start_point;
            switch (event.id())
            {
            case eventid::raw_pointer_down:
                if (event.pointer().btn == Pointer::button::left)
                {
                    show_properties(widget.get());
                    m_selected = widget.get();
                }
                break;
            case eventid::pointer_drag_start:
                m_start_point = widget->box().point();
                event.stop();
                break;
            case eventid::pointer_drag:
            {
                auto diff = event.pointer().drag_start - event.pointer().point;
                widget->move(m_start_point - Point(diff.x(), diff.y()));
                break;
            }
            case eventid::pointer_drag_stop:
                widget->set_align(alignmask::none);
                show_properties(widget.get());
                m_selected = widget.get();
                break;
            default:
                break;
            }
        });

        m_selected = widget.get();

        show_properties(widget.get());
    }

private:

    shared_ptr<Frame> m_canvas;
    shared_ptr<Form> m_form;
    shared_ptr<SelectableGrid> m_grid;
    Widget* m_selected{nullptr};
};

int main(int argc, const char** argv)
{
    Application app(argc, argv, "layout");

    MainWindow win;
    win.show();

    OstreamWidgetSerializer s(cout);
    win.walk(std::bind(&OstreamWidgetSerializer::add, std::ref(s),
                       std::placeholders::_1, std::placeholders::_2));

    XmlWidgetSerializer xml;
    win.walk(std::bind(&XmlWidgetSerializer::add, std::ref(xml),
                       std::placeholders::_1, std::placeholders::_2));
    xml.write("tree.xml");
    xml.write(cout);

    return app.run();
}
