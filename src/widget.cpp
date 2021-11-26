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

Widget::Widget(Serializer::Properties& props, bool is_derived) noexcept
    : m_widgetid(global_widget_id++)
{
    deserialize(props);

    m_user_requested_box = m_box;

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
            flags().clear(Widget::Flag::no_autoresize);
        else
            flags().set(Widget::Flag::no_autoresize);
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

    // damage propagates to top level frame
    if (m_parent)
        m_parent->damage_from_child(to_parent(rect));
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

    return default_palette().color(id, group);
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

const Palette& Widget::default_palette() const
{
    return theme().palette();
}

Frame* Widget::parent()
{
    return m_parent;
}

const Frame* Widget::parent() const
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
    cairo_translate(painter.context().get(),
                    -x(),
                    -y());

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

void Widget::theme(const Theme& theme)
{
    m_theme = std::make_unique<Theme>(theme);
    damage();
}

void Widget::reset_theme()
{
    if (m_theme)
    {
        m_theme.reset();
        damage();
    }
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
    if (m_theme)
        return *m_theme;

    if (parent())
        return parent()->theme();

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

void Widget::detach()
{
    if (m_parent)
    {
        m_parent->remove(this);
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
    if (!flags().is_set(Widget::Flag::no_autoresize))
    {
        m_in_layout = true;
        // cppcheck-suppress unreadVariable
        auto reset = detail::on_scope_exit([this]() { m_in_layout = false; });
        auto s = size();
        if (s.width() < min_size_hint().width())
            s.width(min_size_hint().width());
        if (s.height() < min_size_hint().height())
            s.height(min_size_hint().height());
        resize(s);
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

std::string Widget::type() const
{
    auto t = detail::demangle(typeid(*this).name());
    // for now, remove egt/v1 namespace only.
    return detail::replace_all(t, "egt::v1::", {});
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
    if (m_theme)
    {
        // add new node theme
        serializer.add_node("theme");
        m_theme->serialize(serializer);

        // return back widget node to add other widget properties.
        serializer.previous_node();

        /*
         * widget font can be set either through theme or through m_font
         * if m_font is set ignore theme font.
         */
        if (!m_font)
            m_theme->font().serialize("font", serializer);
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
    detach();

    if (detail::mouse_grab() == this)
        detail::mouse_grab(nullptr);

    if (detail::keyboard_focus() == this)
        detail::keyboard_focus(nullptr);
}

void Widget::set_parent(Frame* parent)
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

DisplayPoint Widget::local_to_display(const Point& p)
{
    DisplayPoint p2(p.x(), p.y());

    auto par = parent();
    while (par)
    {
        p2 += DisplayPoint(par->point().x(), par->point().y());
        par = par->parent();
    }

    return p2 + DisplayPoint(x(), y());
}

Point Widget::display_to_local(const DisplayPoint& p)
{
    Point p2(p.x(), p.y());

    auto par = parent();
    while (par)
    {
        p2 -= par->point();
        par = par->parent();
    }

    return p2 - point();
}

const Font& Widget::font() const
{
    if (m_font)
        return *m_font;

    if (m_theme)
        return m_theme->font();

    if (parent())
        return parent()->font();

    return global_theme().font();
}

}
}
