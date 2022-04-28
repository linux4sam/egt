/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/app.h"
#include "egt/combo.h"
#include "egt/detail/imagecache.h"
#include "egt/detail/screen/composerscreen.h"
#include "egt/frame.h"
#include "egt/image.h"
#include "egt/painter.h"
#include "egt/string.h"
#include "egt/window.h"
#include <memory>

namespace egt
{
inline namespace v1
{

Size ComboBox::default_combobox_size_value;
Signal<>::RegisterHandle ComboBox::default_combobox_size_handle = Signal<>::INVALID_HANDLE;

namespace detail
{

/**
* Popup class used by ComboBox.
*/
class EGT_API ComboBoxPopup : public Popup
{
public:

    /// Construct a ComboBoxPopup with associated ComboBox
    explicit ComboBoxPopup(ComboBox& parent);

    /// @ref PopupType::handle()
    void handle(Event& event) override;

    /// @ref PopupType::show()
    void show() override;

protected:

    /// Position algorithm.
    void smart_pos();

    /// Parent ComboBox.
    ComboBox& m_parent;

    friend class egt::ComboBox;
};

ComboBoxPopup::ComboBoxPopup(ComboBox& parent)
    : Popup(Size(parent.size().width(), 40)),
      m_parent(parent)
{
    name("ComboBoxPopup" + std::to_string(m_widgetid));
    border(20);
    if (!plane_window())
        fill_flags(Theme::FillFlag::blend);
    border_flags(Theme::BorderFlag::drop_shadow);
    auto black = Palette::gray;
    black.alpha(0x30);
    color(egt::Palette::ColorId::border, black);
}

void ComboBoxPopup::smart_pos()
{
    if (Application::instance().screen())
    {
        const auto ss = Application::instance().screen()->size();
        auto height =
            std::min(static_cast<DefaultDim>(ss.height() / 40),
                     static_cast<DefaultDim>(m_parent.m_list.item_count())) * 40;

        // hack because list child area is smaller by this much
        height += 2.0 * 3.0;

        resize(Size(m_parent.size().width(), height));
    }
    else
    {
        resize(Size(m_parent.size().width(), 100));
    }

    const auto ss = Application::instance().screen()->size() / 2;
    move_to_center(Point(ss.width(), ss.height()));
}

void ComboBoxPopup::show()
{
    smart_pos();
    Popup::show();
}

void ComboBoxPopup::handle(Event& event)
{
    Popup::handle(event);

    /// @todo How is this widget going to get this event?
    switch (event.id())
    {
    case EventId::pointer_click:
    {
        /// Force a redraw to avoid a glitch in some circumstances when showing
        /// the popup again.
        begin_draw();
        // if any mouse click happens, hide
        hide();
        break;
    }
    default:
        break;
    }
}

}

ComboBox::ComboBox(const ItemArray& items) noexcept
    : ComboBox(items, {})
{
}

ComboBox::ComboBox(const Rect& rect) noexcept
    : ComboBox(ItemArray(), rect)
{
}

/**
 * @todo ComboBox needs the ability to update list, and this needs to
 * be propagated to ComboBoxPopup, which right now won't because it only
 * happens in constructor.
 */

ComboBox::ComboBox(const ItemArray& items,
                   const Rect& rect) noexcept
    : Widget(rect),
      m_popup(std::make_shared<detail::ComboBoxPopup>(*this))
{
    name("ComboBox" + std::to_string(m_widgetid));

    for (auto& i : items)
    {
        m_list.add_item(i);
    }

    initialize();
}

ComboBox::ComboBox(Frame& parent, const ItemArray& items, const Rect& rect) noexcept
    : ComboBox(items, rect)
{
    parent.add(*this);
}

ComboBox::ComboBox(Serializer::Properties& props, bool is_derived) noexcept
    : Widget(props, true),
      m_popup(std::make_shared<detail::ComboBoxPopup>(*this))
{
    name("ComboBox" + std::to_string(m_widgetid));
    initialize();

    deserialize(props);

    if (!is_derived)
        deserialize_leaf(props);
}

void ComboBox::initialize()
{
    fill_flags(Theme::FillFlag::blend);
    border_radius(4.0);
    padding(5);
    border(theme().default_border());

    grab_mouse(true);

    m_popup->add(m_list);

    m_list.align(AlignFlag::expand);

    // automatically select the first item
    if (m_list.item_count())
        m_list.selected(0);

    m_list.on_selected_changed([this]()
    {
        damage();
        on_selected_changed.invoke();
    });
}

void ComboBox::add_item(const std::shared_ptr<StringItem>& item)
{
    m_list.add_item(item);

    if (m_list.item_count() == 1)
        m_list.selected(0);

    damage();
}

void ComboBox::remove_item(StringItem* item)
{
    return m_list.remove_item(item);
}

void ComboBox::clear()
{
    if (m_list.item_count())
    {
        m_list.clear();
        damage();
    }
}

void ComboBox::set_parent(Frame* parent)
{
    Widget::set_parent(parent);

    if (!Application::instance().main_window())
        throw std::runtime_error("no main window");

    if (!m_popup->parent())
    {
        Application::instance().main_window()->add(m_popup);

        m_popup->special_child_draw_callback(parent->special_child_draw_callback());
    }
}

void ComboBox::handle(Event& event)
{
    Widget::handle(event);

    switch (event.id())
    {
    case EventId::pointer_click:
    {
        const auto mouse = display_to_local(event.pointer().point);
        if (local_box().intersect(mouse))
        {
            m_popup->show_modal();
        }

        break;
    }
    default:
        break;
    }
}

void ComboBox::selected(size_t index)
{
    m_list.selected(index);
}

void ComboBox::resize(const Size& s)
{
    Widget::resize(s);

    if (m_popup->visible())
        m_popup->smart_pos();
}

void ComboBox::move(const Point& point)
{
    Widget::move(point);

    if (m_popup->visible())
        m_popup->smart_pos();
}

void ComboBox::register_handler()
{
    if (default_combobox_size_handle == Signal<>::INVALID_HANDLE)
    {
        default_combobox_size_handle = detail::ComposerScreen::register_screen_resize_hook([]()
            {
                default_combobox_size_value.clear();
            });
    }
}

void ComboBox::unregister_handler()
{
    detail::ComposerScreen::unregister_screen_resize_hook(default_combobox_size_handle);
    default_combobox_size_handle = Signal<>::INVALID_HANDLE;
}

Size ComboBox::default_size()
{
    if (default_combobox_size_value.empty())
    {
        register_handler();
        auto ss = egt::Application::instance().screen()->size();
        default_combobox_size_value = Size(ss.width() * 0.25, ss.height() * 0.05);
    }

    return default_combobox_size_value;
}

void ComboBox::default_size(const Size& size)
{
    if (!size.empty())
        unregister_handler();

    default_combobox_size_value = size;
}

Size ComboBox::min_size_hint() const
{
    if (!m_min_size.empty())
        return m_min_size;

    return default_size() + Widget::min_size_hint();
}

void ComboBox::show_popup() const
{
    m_popup->show_modal();
}

void ComboBox::hide_popup() const
{
    m_popup->hide();
}

void ComboBox::draw(Painter& painter, const Rect& rect)
{
    Drawer<ComboBox>::draw(*this, painter, rect);
}

void ComboBox::default_draw(ComboBox& widget, Painter& painter, const Rect& /*rect*/)
{
    widget.draw_box(painter, Palette::ColorId::bg, Palette::ColorId::border);

    const auto b = widget.content_area();
    const auto handle_dim = b.width() * 0.15;
    const auto pos = ((b.top_right() + b.bottom_right()) / 2.0);
    const auto y = pos.y() - (handle_dim / 2.0);
    const auto x = pos.x() - handle_dim;
    const auto handle = Rect(Point(x, y), Size(handle_dim, handle_dim));
    const auto text = Rect(b.point(), b.size() - Size(handle.size().width(), 0));

    // draw a down arrow
    painter.set(widget.color(Palette::ColorId::button_fg));
    auto cr = painter.context().get();
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    painter.draw(Point(handle.x(), handle.y() + handle.height() / 3.),
                 Point(handle.x() + handle.width() / 2., handle.y() + (handle.height() / 3. * 2.)));
    painter.draw(Point(handle.x() + handle.width() / 2., handle.y() + (handle.height() / 3. * 2.)),
                 Point(handle.x() + handle.width(), handle.y() + handle.height() / 3.));
    painter.line_width(widget.theme().default_border());
    painter.stroke();

    if (widget.m_list.selected() >= 0 && widget.m_list.selected() < static_cast<ssize_t>(widget.m_list.item_count()))
    {
        // text
        painter.set(widget.color(Palette::ColorId::text));
        painter.set(widget.font());
        const auto item = widget.m_list.item_at(widget.m_list.selected());
        const auto size = painter.text_size(item->text());
        const auto target = detail::align_algorithm(size,
                            text,
                            item->text_align());
        painter.draw(target.point());
        painter.draw(item->text());
    }
}

void ComboBox::serialize(Serializer& serializer) const
{
    Widget::serialize(serializer);

    for (size_t i = 0; i < m_list.item_count(); i++)
    {
        Serializer::Attributes attrs;
        auto item = m_list.item_at(i);
        if (item)
        {
            if (!item->image().empty())
                attrs.push_back({"image", item->image().uri()});

            if (!item->text().empty())
                attrs.push_back({"text_align", item->text_align().to_string()});

            if (static_cast<ssize_t>(i) == selected())
                attrs.push_back({"selected", "true"});

            serializer.add_property("item", item->text(), attrs);
        }
    }
}

void ComboBox::deserialize(Serializer::Properties& props)
{
    props.erase(std::remove_if(props.begin(), props.end(), [&](auto & p)
    {
        if (std::get<0>(p) == "item")
        {
            std::string text;
            Image image;
            AlignFlags text_align;
            auto attrs = std::get<2>(p);
            bool is_selected = false;
            for (const auto& i : attrs)
            {
                if (i.first == "image")
                    image = Image(i.second);

                if (i.first == "text_align")
                    text_align = AlignFlags(i.second);

                if (i.first == "selected" && detail::from_string(i.second))
                    is_selected = true;
            }

            add_item(std::make_shared<StringItem>(std::get<1>(p), image, Rect(), text_align));
            if (is_selected)
                selected(item_count() - 1);
            return true;
        }
        return false;
    }), props.end());
}

}
}
