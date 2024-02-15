/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/egtlog.h"
#include "egt/canvas.h"
#include "egt/detail/alignment.h"
#include "egt/detail/enum.h"
#include "egt/detail/math.h"
#include "egt/detail/string.h"
#include "egt/frame.h"
#include "egt/geometry.h"
#include "egt/input.h"
#include "egt/painter.h"
#include "egt/screen.h"
#include "egt/serialize.h"
#include "egt/types.h"
#include "egt/widget.h"
#include <cassert>
#include <ostream>
#include <string>

#include "detail/dump.h"

namespace egt
{
inline namespace v1
{

template<>
const std::pair<Widget::Flag, char const*> detail::EnumStrings<Widget::Flag>::data[] =
{
    {Widget::Flag::plane_window, "plane_window"},
    {Widget::Flag::window, "window"},
    {Widget::Flag::frame, "frame"},
    {Widget::Flag::disabled, "disabled"},
    {Widget::Flag::readonly, "readonly"},
    {Widget::Flag::active, "active"},
    {Widget::Flag::invisible, "invisible"},
    {Widget::Flag::grab_mouse, "grab_mouse"},
    {Widget::Flag::no_clip, "no_clip"},
    {Widget::Flag::no_layout, "no_layout"},
    {Widget::Flag::no_autoresize, "no_autoresize"},
    {Widget::Flag::checked, "checked"},
    {Widget::Flag::component, "component"},
    {Widget::Flag::user_drag, "user_drag"},
    {Widget::Flag::user_track_drag, "user_track_drag"},
};

std::ostream& operator<<(std::ostream& os, const Widget::Flags& flags)
{
    return os << flags.to_string();
}

std::ostream& operator<<(std::ostream& os, const Widget::Flag& flag)
{
    return os << detail::enum_to_string(flag);
}

static Widget::WidgetId global_widget_id{0};

// NOLINTNEXTLINE(modernize-pass-by-value)
Widget::Widget(const Rect& rect, const Widget::Flags& flags) noexcept
    : m_box(rect),
      m_user_requested_box(rect),
      m_widgetid(global_widget_id++),
      m_widget_flags(flags)
{
    init();
}

Widget::Widget(Serializer::Properties& props, bool is_derived) noexcept
    : m_widgetid(global_widget_id++)
{
    deserialize(props);

    m_user_requested_box = m_box;

    init();

    if (!is_derived)
        deserialize_leaf(props);
}

Widget::Widget(Frame& parent, const Rect& rect, const Widget::Flags& flags) noexcept
    : Widget(rect, flags)
{
    parent.add(*this);
}

void Widget::handle(Event& event)
{
    if (event.quit())
        return;

    EGTLOG_TRACE("{} handle {}", name(), event);

    switch (event.id())
    {
    case EventId::raw_pointer_down:
        if (flags().is_set(Widget::Flag::grab_mouse))
        {
            active(true);
            event.grab(this);
        }
        break;
    case EventId::raw_pointer_up:
        active(false);
        break;
    default:
        break;
    }

    invoke_handlers(event);

    if (!m_subordinates.empty())
    {
        switch (event.id())
        {
        case EventId::raw_pointer_down:
        case EventId::raw_pointer_up:
        case EventId::raw_pointer_move:
        case EventId::pointer_click:
        case EventId::pointer_dblclick:
        case EventId::pointer_hold:
        case EventId::pointer_drag_start:
        {
            for (auto& subordinate : detail::reverse_iterate(m_subordinates))
            {
                if (!subordinate->can_handle_event())
                    continue;

                if (subordinate->hit(event.pointer().point))
                {
                    subordinate->handle(event);
                    if (event.postponed_quit())
                        event.stop();
                    break;
                }
            }

            break;
        }

        case EventId::keyboard_down:
        case EventId::keyboard_up:
        case EventId::keyboard_repeat:
        {
            for (auto& subordinate : detail::reverse_iterate(m_subordinates))
            {
                if (!subordinate->can_handle_event())
                    continue;

                subordinate->handle(event);
                if (event.postponed_quit())
                    event.stop();
                if (event.quit())
                    break;
            }

            break;
        }

        default:
            break;
        }
    }

    if (event.quit())
        return;

    /*
     * start_drag() must be executed only now to give a chance to subordinates
     * catch the 'pointer_drag_start' event.
     */
    if (event.id() == EventId::pointer_drag_start)
        start_drag(event);
}

void Widget::start_drag(Event& event)
{
    /* Do nothing if this widget doesn't accept drag events anyway. */
    if (!accept_drag())
        return;

    /* Accept this event only it hits our widget box. */
    if (!hit(event.pointer().drag_start))
        return;

    /*
     * Don't stop the event immediately if an overridden handle() method still
     * needs to process it, but reschedule the stop() for when this
     * overridden handle() method completes.
     */
    event.postpone_stop();
    detail::dragged(this);
}

void Widget::continue_drag(Event& event)
{
    auto pos = display_to_local(event.pointer().point);
    const auto b = local_box();

    if (track_drag() || b.intersect(pos))
    {
        if (event.id() == EventId::pointer_drag_stop)
            detail::dragged(nullptr);
    }
    else
    {
        /* Make sure the pointer is within the box. */
        pos.x(detail::clamp(pos.x(), 0, b.width()));
        pos.y(detail::clamp(pos.y(), 0, b.height()));

        event.id(EventId::pointer_drag_stop);
        event.pointer().point = local_to_display(pos);
        detail::dragged(nullptr);
    }

    handle(event);
}

void Widget::move_to_center(const Point& point)
{
    if (center() != point)
    {
        Point pos(point.x() - width() / 2,
                  point.y() - height() / 2);

        move(pos);
    }
}

void Widget::move_to_center()
{
    if (!m_parent)
        return;

    move_to_center(m_parent->center());
}

void Widget::resize(const Size& size)
{
    if (size != this->size())
    {
        damage();
        m_box.size(size);
        damage();

        // If resize comes from the user
        if (!parent_in_layout() && !in_layout())
            m_user_requested_box.size(size);

        parent_layout();

        if (!m_subordinates.empty())
            layout();
    }
}

void Widget::resize_by_ratio(DefaultDim hratio, DefaultDim vratio)
{
    Size size(static_cast<float>(width()) * (static_cast<float>(hratio) / 100.0f),
              static_cast<float>(height()) * (static_cast<float>(vratio) / 100.0f));
    resize(size);
}

void Widget::move(const Point& point)
{
    if (point != box().point())
    {
        damage();
        m_box.point(point);
        damage();

        // If move comes from the user
        if (!parent_in_layout())
            m_user_requested_box.point(point);

        parent_layout();
    }
}

void Widget::box(const Rect& rect)
{
    move(rect.point());
    resize(rect.size());
}

void Widget::hide()
{
    if (flags().is_set(Widget::Flag::invisible))
        return;
    // careful attention to ordering
    damage();
    flags().set(Widget::Flag::invisible);
    on_hide.invoke();
}

void Widget::show()
{
    if (!flags().is_set(Widget::Flag::invisible))
        return;
    // careful attention to ordering
    flags().clear(Widget::Flag::invisible);
    damage();
    on_show.invoke();
}

void Widget::visible(bool value)
{
    if (visible() != value)
    {
        if (value)
            show();
        else
            hide();
    }
}

bool Widget::active() const
{
    return flags().is_set(Widget::Flag::active);
}

void Widget::active(bool value)
{
    if (flags().is_set(Widget::Flag::active) != value)
    {
        if (value)
            flags().set(Widget::Flag::active);
        else
            flags().clear(Widget::Flag::active);
        damage();
    }
}

void Widget::readonly(bool value)
{
    if (flags().is_set(Widget::Flag::readonly) != value)
    {
        if (value)
        {
            flags().set(Widget::Flag::readonly);

            if (detail::keyboard_focus() == this)
                detail::keyboard_focus(nullptr);
        }
        else
            flags().clear(Widget::Flag::readonly);
        damage();
    }
}

void Widget::disable()
{
    if (flags().is_set(Widget::Flag::disabled))
        return;
    damage();
    flags().set(Widget::Flag::disabled);

    if (detail::keyboard_focus() == this)
        detail::keyboard_focus(nullptr);
}

void Widget::enable()
{
    if (!flags().is_set(Widget::Flag::disabled))
        return;
    damage();
    flags().clear(Widget::Flag::disabled);
}

bool Widget::plane_window() const
{
    return flags().is_set(Widget::Flag::plane_window);
}

bool Widget::frame() const
{
    return flags().is_set(Widget::Flag::frame);
}

void Widget::autoresize(bool value)
{
    if (flags().is_set(Widget::Flag::no_autoresize) == value)
    {
        if (value)
        {
            flags().clear(Widget::Flag::no_autoresize);
            layout();
        }
        else
        {
            flags().set(Widget::Flag::no_autoresize);
        }
    }
}

bool Widget::autoresize() const
{
    return !flags().is_set(Widget::Flag::no_autoresize);
}

bool Widget::clip() const
{
    return !flags().is_set(Widget::Flag::no_clip);
}

void Widget::no_layout(bool value)
{
    if (flags().is_set(Widget::Flag::no_layout) != value)
    {
        if (value)
            flags().set(Widget::Flag::no_layout);
        else
            flags().clear(Widget::Flag::no_layout);
    }
}

bool Widget::no_layout() const
{
    return flags().is_set(Widget::Flag::no_layout);
}

void Widget::grab_mouse(bool value)
{
    if (flags().is_set(Widget::Flag::grab_mouse) != value)
    {
        if (value)
            flags().set(Widget::Flag::grab_mouse);
        else
            flags().clear(Widget::Flag::grab_mouse);
    }
}

bool Widget::grab_mouse() const
{
    return flags().is_set(Widget::Flag::grab_mouse);
}

void Widget::alpha(float alpha)
{
    alpha = detail::clamp<>(alpha, 0.f, 1.f);

    if (detail::change_if_diff<float>(m_alpha, alpha))
        damage();
}

void Widget::damage()
{
    damage(box());
}

void Widget::damage(const Rect& rect)
{
    if (egt_unlikely(rect.empty()))
        return;

    // don't damage if not even visible
    if (!visible())
        return;

    // damage propagates up to widget with screen
    if (!has_screen())
    {
        if (parent())
            parent()->damage_from_subordinate(to_parent(rect));

        // have no parent or screen - nowhere to put damage
        return;
    }

    add_damage(rect);
}

void Widget::add_damage(const Rect& rect)
{
    // if we get here, we must have a screen
    assert(has_screen());
    if (!has_screen())
        return;

    if (egt_unlikely(rect.empty()))
        return;

    // not allowed to damage() in draw()
    assert(!m_in_draw);
    if (m_in_draw)
        return;

    EGTLOG_TRACE("{} damage:{}", name(), rect);

    // No damage outside of our box().  There are cases where this is expected,
    // for example, when a widget is halfway off the screen. So, we truncate the
    // to just the part we care about.
    auto r = Rect::intersection(rect, to_subordinate(box()));

    Screen::damage_algorithm(m_damage, r);
}

void Widget::palette(const Palette& palette)
{
    m_palette = std::make_unique<Palette>(palette);
    damage();
}

void Widget::reset_palette()
{
    if (m_palette)
    {
        m_palette.reset();
        damage();
    }
}

const Pattern& Widget::color(Palette::ColorId id) const
{
    Palette::GroupId group = Palette::GroupId::normal;
    if (disabled())
        group = Palette::GroupId::disabled;
    else if (active())
        group = Palette::GroupId::active;
    else if (checked())
        group = Palette::GroupId::checked;

    return color(id, group);
}

const Pattern& Widget::color(Palette::ColorId id, Palette::GroupId group) const
{
    if (m_palette)
    {
        const Pattern* color;
        if (m_palette->exists(id, group, &color))
            return *color;
    }

    if (parent())
        return parent()->color(id, group);

    if (global_palette())
        return global_palette()->color(id, group);

    return global_theme().palette().color(id, group);
}

void Widget::color(Palette::ColorId id,
                   const Pattern& color,
                   Palette::GroupId group)
{
    if (!m_palette)
        m_palette = std::make_unique<Palette>();

    /*
     * Performance improvement: do not update the color if there is no change,
     * otherwise it can cause unexpected redraws.
     */
    const Pattern* current_color;
    if (!m_palette->exists(id, group, &current_color) || (color != *current_color))
    {
        m_palette->set(id, group, color);
        damage();
    }
}

const Palette& Widget::palette() const
{
    if (m_palette)
        return *m_palette;

    if (parent())
        return parent()->palette();

    if (global_palette())
        return *global_palette();

    return global_theme().palette();
}

Widget* Widget::parent()
{
    return m_parent;
}

const Widget* Widget::parent() const
{
    return m_parent;
}

Screen* Widget::screen() const
{
    assert(m_parent);
    return parent()->screen();
}

void Widget::align(const AlignFlags& a)
{
    m_align = a;
}

Point Widget::to_parent(const Point& r) const
{
    if (parent())
        return r + parent()->point();

    return r;
}

DisplayPoint Widget::display_origin()
{
    DisplayPoint p(x(), y());

    auto par = parent();
    while (par)
    {
        p += DisplayPoint(par->x(), par->y());
        par = par->parent();
    }

    return p;
}

Size Widget::min_size_hint() const
{
    if (!m_min_size.empty())
        return m_min_size;

    return {static_cast<DefaultDim>(moat() * 2.),
            static_cast<DefaultDim>(moat() * 2.)};
}

void Widget::paint(Painter& painter)
{
    Painter::AutoSaveRestore sr(painter);

    // move origin
    painter.translate(-point());

    draw(painter, box());
}

void Widget::paint_to_file(const std::string& filename)
{
#if CAIRO_HAS_PNG_FUNCTIONS == 1
    std::string name = filename;
    if (name.empty())
        name = fmt::format("{}.png", this->name());

    auto surface = shared_cairo_surface_t(
                       cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                               width(), height()),
                       cairo_surface_destroy);

    auto cr = shared_cairo_t(cairo_create(surface.get()), cairo_destroy);

    Painter painter(cr);
    paint(painter);
    cairo_surface_write_to_png(surface.get(), name.c_str());
#else
    detail::ignoreparam(filename);
    detail::error("png support not available");
#endif
}

void Widget::walk(const WalkCallback& callback, int level)
{
    callback(this, level);
}

void Widget::draw_box(Painter& painter, Palette::ColorId bg,
                      Palette::ColorId border) const
{
    theme().draw_box(painter, *this, bg, border);
}

void Widget::draw_circle(Painter& painter, Palette::ColorId bg,
                         Palette::ColorId border) const
{
    theme().draw_circle(painter, *this, bg, border);
}

const Theme& Widget::theme() const
{
    return global_theme();
}

void Widget::zorder_down()
{
    if (m_parent)
        m_parent->zorder_down(this);
}

void Widget::zorder_up()
{
    if (m_parent)
        m_parent->zorder_up(this);
}

void Widget::zorder_bottom()
{
    if (m_parent)
        m_parent->zorder_bottom(this);
}

void Widget::zorder_top()
{
    if (m_parent)
        m_parent->zorder_top(this);
}

size_t Widget::zorder() const
{
    if (m_parent)
        return m_parent->zorder(this);

    return 0;
}

void Widget::zorder(size_t rank)
{
    if (m_parent)
        m_parent->zorder(this, rank);
}

void Widget::zorder_down(const Widget* widget)
{
    auto& range = range_from_widget(*widget);
    auto begin = range.begin();
    auto end = range.end();

    auto i = std::find_if(begin, end,
                          [widget](const auto & ptr)
    {
        return ptr.get() == widget;
    });
    if (i != end && i != begin)
    {
        auto to = std::prev(i);
        (*i)->damage();
        (*to)->damage();
        std::iter_swap(i, to);
        if (to == begin)
        {
            if (widget->component())
                m_components_begin = to;
            update_subordinates_ranges();
        }
        layout();
    }
}

void Widget::zorder_up(const Widget* widget)
{
    auto& range = range_from_widget(*widget);
    auto begin = range.begin();
    auto end = range.end();

    auto i = std::find_if(begin, end,
                          [widget](const auto & ptr)
    {
        return ptr.get() == widget;
    });
    if (i != end)
    {
        auto to = std::next(i);
        if (to != end)
        {
            (*i)->damage();
            (*to)->damage();
            std::iter_swap(i, to);
            if (i == begin)
            {
                if (widget->component())
                    m_components_begin = i;
                update_subordinates_ranges();
            }
            layout();
        }
    }
}

void Widget::zorder_bottom(const Widget* widget)
{
    auto& range = range_from_widget(*widget);
    auto begin = range.begin();
    auto end = range.end();

    if (std::distance(begin, end) <= 1)
        return;

    auto i = std::find_if(begin, end,
                          [widget](const auto & ptr)
    {
        return ptr.get() == widget;
    });

    if (i != end && i != begin)
    {
        m_subordinates.splice(begin, m_subordinates, i, std::next(i));
        if (widget->component())
            m_components_begin = i;
        update_subordinates_ranges();
        layout();
    }
}

void Widget::zorder_top(const Widget* widget)
{
    auto& range = range_from_widget(*widget);
    auto begin = range.begin();
    auto end = range.end();

    if (std::distance(begin, end) <= 1)
        return;

    auto i = std::find_if(begin, end,
                          [widget](const auto & ptr)
    {
        return ptr.get() == widget;
    });
    if (i != end && i != std::prev(end))
    {
        m_subordinates.splice(end, m_subordinates, i, std::next(i));
        if (i == begin)
        {
            if (widget->component())
                m_components_begin = std::next(i);
            update_subordinates_ranges();
        }
        layout();
    }
}

size_t Widget::zorder(const Widget* widget) const
{
    auto i = std::find_if(children().begin(), children().end(),
                          [widget](const auto & ptr)
    {
        return ptr.get() == widget;
    });
    if (i != children().end())
    {
        return std::distance(children().begin(), i);
    }

    return 0;
}

void Widget::zorder(const Widget* widget, size_t rank)
{
    auto& range = range_from_widget(*widget);
    auto begin = range.begin();
    auto end = range.end();

    auto i = std::find_if(begin, end,
                          [widget](const auto & ptr)
    {
        return ptr.get() == widget;
    });
    if (i != end)
    {
        size_t old_rank = std::distance(begin, i);
        rank = std::min(rank, (size_t)std::distance(begin, end) - 1);
        if (rank != old_rank)
        {
            auto j = std::next(begin, rank);
            if (rank > old_rank)
                std::advance(j, 1);

            m_subordinates.splice(j, m_subordinates, i, std::next(i));
            if (i == begin || j == begin)
            {
                if (widget->component())
                    m_components_begin = i;
                update_subordinates_ranges();
            }
            layout();
        }
    }
}

void Widget::detach()
{
    if (m_parent)
    {
        if (component())
            m_parent->remove_component(this);
        else
        {
            auto frame = dynamic_cast<Frame*>(m_parent);
            frame->remove(this);
        }
        m_parent = nullptr;
    }
}

size_t Widget::moat() const
{
    return margin() + padding() + border();
}

Rect Widget::content_area() const
{
    auto m = moat();
    auto b = box();
    b += Point(m, m);
    b -= Size(2. * m, 2. * m);
    // don't return a negative size
    if (b.empty())
        return Rect(point(), Size());
    return b;
}

void Widget::layout()
{
    if (m_subordinates.empty())
    {
        if (!flags().is_set(Widget::Flag::no_autoresize))
        {
            m_in_layout = true;
            // cppcheck-suppress unreadVariable
            auto reset = detail::on_scope_exit([this]() { m_in_layout = false; });
            auto s = size();
            auto m = min_size_hint();
            if (s.width() < m.width())
                s.width(m.width());
            if (s.height() < m.height())
                s.height(m.height());
            resize(s);
        }
    }
    else
    {
        if (!visible())
            return;

        // we cannot layout with no space
        if (size().empty())
            return;

        if (m_in_layout)
            return;

        m_in_layout = true;
        auto reset = detail::on_scope_exit([this]() { m_in_layout = false; });

        auto area = content_area();

        for (auto& subordinate : m_subordinates)
        {
            auto bounding = to_subordinate(area);
            if (bounding.empty())
                continue;

            subordinate->layout();

            auto r = detail::align_algorithm(subordinate->box(),
                                             bounding,
                                             subordinate->align(),
                                             0,
                                             subordinate->horizontal_ratio(),
                                             subordinate->vertical_ratio(),
                                             subordinate->xratio(),
                                             subordinate->yratio());
            subordinate->box(r);
        }
    }
}

void Widget::checked(bool value)
{
    if (flags().is_set(Widget::Flag::checked) != value)
    {
        if (value)
            flags().set(Widget::Flag::checked);
        else
            flags().clear(Widget::Flag::checked);
        damage();
    }
}

void Widget::focus(bool value)
{
    if (focus() != value)
    {
        if (value)
            detail::keyboard_focus(this);
        else
            detail::keyboard_focus(nullptr);
    }
}

std::string Widget::type() const
{
    auto t = detail::demangle(typeid(*this).name());
    // for now, remove egt/v1 namespace only.
    return detail::replace_all(t, "egt::v1::", {});
}

void Widget::init(void)
{
    m_components_begin = m_subordinates.begin();
    update_subordinates_ranges();

    m_align.on_change([this]()
    {
        parent_layout();
    });

    on_gain_focus([this]()
    {
        m_focus = true;
    });

    on_lost_focus([this]()
    {
        m_focus = false;
    });
}

void Widget::serialize(Serializer& serializer) const
{
    serializer.add_property("show", visible());
    if (x())
        serializer.add_property("x", x());
    if (y())
        serializer.add_property("y", y());
    if (width())
        serializer.add_property("width", width());
    if (height())
        serializer.add_property("height", height());
    if (!align().empty())
        serializer.add_property("align", align());
    if (!border_flags().empty())
        serializer.add_property("borderflags", border_flags().to_string());
    if (!autoresize())
        serializer.add_property("autoresize", autoresize());
    if (checked())
        serializer.add_property("checked", checked());
    if (disabled())
        serializer.add_property("disabled", disabled());
    if (grab_mouse())
        serializer.add_property("grab_mouse", grab_mouse());
    if (no_layout())
        serializer.add_property("no_layout", no_layout());
    if (padding())
        serializer.add_property("padding", padding());
    if (margin())
        serializer.add_property("margin", margin());
    if (border())
        serializer.add_property("border", border());
    if (!detail::float_equal(border_radius(), 0))
        serializer.add_property("border_radius", border_radius());
    if (xratio())
        serializer.add_property("ratio:x", xratio());
    if (yratio())
        serializer.add_property("ratio:y", yratio());
    if (horizontal_ratio())
        serializer.add_property("ratio:horizontal", horizontal_ratio());
    if (vertical_ratio())
        serializer.add_property("ratio:vertical", vertical_ratio());
    if (!fill_flags().empty())
        serializer.add_property("fillflags", fill_flags().to_string());
    if (m_font)
        m_font->serialize("font", serializer);
    /**
     * widget color can be set by theme and using m_palette object.
     * during draw first m_palette is checked and if m_palette not
     * available, then theme is used.
     */
    if (m_palette)
    {
        m_palette->serialize("color", serializer);
    }
}

void Widget::deserialize_leaf(Serializer::Properties& props)
{
    props.erase(std::remove_if(props.begin(), props.end(), [&](auto & p)
    {
        if (std::get<0>(p) == "show")
        {
            auto enable =  detail::from_string(std::get<1>(p));
            if (enable)
                show();
            else
                hide();
            return true;
        }
        return false;
    }), props.end());
}

void Widget::deserialize(Serializer::Properties& props)
{
    props.erase(std::remove_if(props.begin(), props.end(), [&](auto & p)
    {
        bool ret = true;
        auto value = std::get<1>(p);
        switch (detail::hash(std::get<0>(p)))
        {
        case detail::hash("width"):
            width(std::stoi(value));
            break;
        case detail::hash("height"):
            height(std::stoi(value));
            break;
        case detail::hash("x"):
            x(std::stoi(value));
            break;
        case detail::hash("y"):
            y(std::stoi(value));
            break;
        case detail::hash("align"):
            align(AlignFlags(value));
            break;
        case detail::hash("borderflags"):
            border_flags(Theme::BorderFlags(value));
            break;
        case detail::hash("autoresize"):
            autoresize(egt::detail::from_string(value));
            break;
        case detail::hash("checked"):
            checked(egt::detail::from_string(value));
            break;
        case detail::hash("disabled"):
            disabled(egt::detail::from_string(value));
            break;
        case detail::hash("grab_mouse"):
            grab_mouse(egt::detail::from_string(value));
            break;
        case detail::hash("no_layout"):
            no_layout(egt::detail::from_string(value));
            break;
        case detail::hash("alpha"):
            alpha(std::stoi(value));
            break;
        case detail::hash("padding"):
            padding(std::stoi(value));
            break;
        case detail::hash("margin"):
            margin(std::stoi(value));
            break;
        case detail::hash("border"):
            border(std::stoi(value));
            break;
        case detail::hash("border_radius"):
            border_radius(std::stof(value));
            break;
        case detail::hash("fillflags"):
            m_fill_flags.from_string(value);
            break;
        case detail::hash("ratio:x"):
            xratio(std::stoi(value));
            break;
        case detail::hash("ratio:y"):
            yratio(std::stoi(value));
            break;
        case detail::hash("ratio:horizontal"):
            horizontal_ratio(std::stoi(value));
            break;
        case detail::hash("ratio:vertical"):
            vertical_ratio(std::stoi(value));
            break;
        case detail::hash("font"):
        {
            Font f;
            f.deserialize(std::get<0>(p), value, std::get<2>(p));
            font(f);
            break;
        }
        /**
         * widget color can be set by theme and using m_palette object.
         * during draw first m_palette is checked and if m_palette not
         * available, then theme is used.
         */
        case detail::hash("color"):
        {
            if (!m_palette)
                m_palette = std::make_unique<Palette>();
            m_palette->deserialize(std::get<0>(p), value, std::get<2>(p));
            break;
        }
        default:
            ret = false;
            break;
        }
        return ret;
    }), props.end());
}

Widget::~Widget() noexcept
{
    for (auto& i : components())
        remove_component(i.get());
    detach();

    if (detail::mouse_grab() == this)
        detail::mouse_grab(nullptr);

    if (detail::keyboard_focus() == this)
        detail::keyboard_focus(nullptr);

    if (detail::dragged() == this)
        detail::dragged(nullptr);
}

void Widget::set_parent(Widget* parent)
{
    // cannot already have a parent
    assert(!m_parent);
    if (m_parent)
        throw std::runtime_error("widget already has a parent");

    if (parent == this)
        throw std::runtime_error("cannot add a widget to itself");

    m_parent = parent;
    damage();
}

bool Widget::parent_in_layout()
{
    if (parent())
        return parent()->in_layout();
    else
        return false;
}

void Widget::parent_layout()
{
    if (!visible())
        return;

    if (flags().is_set(Widget::Flag::no_layout))
        return;

    if (parent())
        parent()->layout();
}

DisplayPoint Widget::local_to_display(const Point& p) const
{
    DisplayPoint p2(p.x(), p.y());

    auto sub = this;
    auto par = parent();
    while (par)
    {
        auto par_point = par->point_from_subordinate(*sub);
        p2 += DisplayPoint(par_point.x(), par_point.y());
        sub = par;
        par = par->parent();
    }

    return p2 + DisplayPoint(x(), y());
}

Point Widget::display_to_local(const DisplayPoint& p) const
{
    Point p2(p.x(), p.y());

    auto sub = this;
    auto par = parent();
    while (par)
    {
        p2 -= par->point_from_subordinate(*sub);
        sub = par;
        par = par->parent();
    }

    return p2 - point();
}

const Font& Widget::font() const
{
    if (m_font)
        return *m_font;

    if (parent())
        return parent()->font();

    if (global_font())
        return *global_font();

    return global_theme().font();
}

void Widget::on_screen_resized()
{
    if (m_font)
    {
        m_font->on_screen_resized();
        damage();
        layout();
        parent_layout();
    }
}

void Widget::draw(Painter& painter, const Rect& rect)
{
    EGTLOG_TRACE("{} draw {}", name(), rect);

    Painter::AutoSaveRestore sr(painter);

    // child rect
    auto crect = rect;

    // if this widget does not have a screen, it means the damage rect is in
    // coordinates of some parent widget, so we have to adjust the physical origin
    // and take it into account when looking at children, who's coordinates are
    // respective of this widget
    if (!has_screen())
    {
        const auto& origin = point();
        painter.translate(origin);

        // adjust our child rect for comparison's below
        crect -= origin;
    }

    if (clip())
    {
        // clip the damage rectangle, otherwise we will draw this whole widget
        // and then only draw the children inside the actual damage rect, which
        // will cover them
        painter.draw(crect);
        painter.clip();
    }

    // draw our widget box, but now that the physical origin has possibly changed
    // and our box() is relative to our parent, we have to adjust to our local
    // origin
    if (!fill_flags().empty() || border())
    {
        Palette::GroupId group = Palette::GroupId::normal;
        if (disabled())
            group = Palette::GroupId::disabled;
        else if (active())
            group = Palette::GroupId::active;

        theme().draw_box(painter,
                         fill_flags(),
                         to_subordinate(box()),
                         color(Palette::ColorId::border, group),
                         color(Palette::ColorId::bg, group),
                         border(),
                         margin(),
                         border_radius(),
                         border_flags());
    }
    else if (Application::instance().is_composer())
    {
        constexpr static Color composer_border = Palette::black;
        constexpr static Color composer_bg = Color(0x00000020);

        theme().draw_box(painter,
        {Theme::FillFlag::blend},
        to_subordinate(box()),
        composer_border,
        composer_bg,
        1,
        0,
        0,
        {});
    }

    if (m_subordinates.empty())
        return;

    // keep the crect inside our content area
    crect = Rect::intersection(crect, to_subordinate(content_area()));

    for (auto& subordinate : m_subordinates)
    {
        if (!subordinate->visible())
            continue;

        // don't draw plane widget as child - this is
        // specifically handled by event loop
        if (subordinate->plane_window())
            continue;

        draw_subordinate(painter, crect, subordinate.get());
    }
}

static inline bool time_subordinate_draw_enabled()
{
    static int value = 0;
    if (value == 0)
    {
        if (std::getenv("EGT_TIME_DRAW"))
            value += 1;
        else
            value -= 1;
    }
    return value == 1;
}

void Widget::draw_subordinate(Painter& painter, const Rect& crect, Widget* subordinate)
{
    if (subordinate->box().intersect(crect))
    {
        // don't give a child a rectangle that is outside of its own box
        auto r = Rect::intersection(crect, subordinate->box());
        if (r.empty())
            return;

        if (detail::float_equal(subordinate->alpha(), 1.f))
        {
            Painter::AutoSaveRestore sr2(painter);

            // no matter what the child draws, clip the output to only the
            // rectangle we care about updating
            if (clip())
            {
                painter.draw(r);
                painter.clip();
            }

            detail::code_timer(time_subordinate_draw_enabled(), subordinate->name() + " draw: ", [subordinate, &painter, &r]()
            {
                subordinate->draw(painter, r);
            });
        }
        else
        {
            {
                Painter::AutoGroup group(painter);

                // no matter what the child draws, clip the output to only the
                // rectangle we care about updating
                if (clip())
                {
                    painter.draw(r);
                    painter.clip();
                }

                detail::code_timer(time_subordinate_draw_enabled(), subordinate->name() + " draw: ", [subordinate, &painter, &r]()
                {
                    subordinate->draw(painter, r);
                });
            }

            // we pushed a group for the child to draw into it, now paint that
            // child with its alpha component
            painter.paint(subordinate->alpha());
        }

        special_child_draw(painter, subordinate);
    }
}

Point Widget::to_panel(const Point& p)
{
    if (has_screen())
        return p - point();

    if (parent())
        return parent()->to_panel(p - point());

    return p;
}

void Widget::remove_component(Widget* widget)
{
    if (!widget)
        return;

    auto i = std::find_if(components().begin(), components().end(),
                          [widget](const auto & ptr)
    {
        return ptr.get() == widget;
    });
    if (i != components().end())
    {
        // note order here - damage and then unset parent
        (*i)->damage();
        (*i)->m_parent = nullptr;
        (*i)->component(false);
        if (i == m_components_begin)
            m_components_begin = std::next(m_components_begin);
        m_subordinates.erase(i);
        update_subordinates_ranges();
    }
    else if (widget->m_parent == this)
    {
        widget->m_parent = nullptr;
    }
}

void Widget::add_component(Widget& widget)
{
    // Nasty, but it gets the job done.  If a widget is passed in as a
    // reference, we don't own it, so create a "pointless" shared_ptr that
    // will not delete it.
    auto w = std::shared_ptr<Widget>(&widget, [](Widget*) {});

    bool first_component = components().empty();

    w->set_parent(this);
    w->component(true);
    m_subordinates.emplace_back(w);

    if (first_component)
        m_components_begin = std::prev(m_subordinates.end());
    update_subordinates_ranges();
}

void Widget::component(bool value)
{
    if (flags().is_set(Widget::Flag::component) != value)
    {
        if (value)
            flags().set(Widget::Flag::component);
        else
            flags().clear(Widget::Flag::component);
    }
}

bool Widget::component() const
{
    return flags().is_set(Widget::Flag::component);
}

}
}
