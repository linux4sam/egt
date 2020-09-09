/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/combo.h"
#include "egt/detail/imagecache.h"
#include "egt/frame.h"
#include "egt/image.h"
#include "egt/painter.h"
#include "egt/window.h"
#include <memory>

namespace egt
{
inline namespace v1
{

static constexpr auto DEFAULT_COMBOBOX_SIZE = Size(200, 30);

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

    /// ListBox of ComboBox items.
    std::shared_ptr<ListBox> m_list;

    /// Parent ComboBox.
    ComboBox& m_parent;

    friend class egt::ComboBox;
};

ComboBoxPopup::ComboBoxPopup(ComboBox& parent)
    : Popup(Size(parent.size().width(), 40)),
      m_list(std::make_shared<ListBox>()),
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

    m_list->align(AlignFlag::expand);

    add(m_list);

    m_list->on_selected_changed([this]()
    {
        // ?? how to stop event
        //event.stop();
        m_parent.selected(m_list->selected());
    });
}

void ComboBoxPopup::smart_pos()
{
    if (Application::instance().screen())
    {
        const auto ss = Application::instance().screen()->size();
        auto height =
            std::min(static_cast<DefaultDim>(ss.height() / 40),
                     static_cast<DefaultDim>(m_list->item_count())) * 40;

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
    m_list->clear();
    for (const auto& item : m_parent.m_items)
    {
        auto p = std::make_shared<StringItem>(item);
        m_list->add_item(p);
    }

    if (!m_parent.m_items.empty())
        m_list->selected(m_parent.selected());

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

ComboBox::ComboBox(ItemArray items,
                   const Rect& rect) noexcept
    : Widget(rect),
      m_items(std::move(items)),
      m_popup(std::make_shared<detail::ComboBoxPopup>(*this))
{
    name("ComboBox" + std::to_string(m_widgetid));

    fill_flags(Theme::FillFlag::blend);
    border_radius(4.0);
    padding(5);
    border(theme().default_border());

    grab_mouse(true);

    // automatically select the first item
    if (!m_items.empty())
        m_selected = 0;
}

ComboBox::ComboBox(Frame& parent, const ItemArray& items, const Rect& rect) noexcept
    : ComboBox(items, rect)
{
    parent.add(*this);
}

void ComboBox::add_item(const std::string& item)
{
    m_items.push_back(item);

    if (m_items.size() == 1)
        m_selected = 0;

    damage();
}

bool ComboBox::remove_item(const std::string& item)
{
    const auto it = std::find(m_items.begin(), m_items.end(), item);
    if (it != m_items.end())
    {
        m_items.erase(it);
        damage();
        return true;
    }
    return false;
}

void ComboBox::clear()
{
    if (!m_items.empty())
    {
        m_items.clear();
        damage();
    }
}

void ComboBox::set_parent(Frame* parent)
{
    Widget::set_parent(parent);

    if (!Application::instance().main_window())
        throw std::runtime_error("no main window");

    Application::instance().main_window()->add(m_popup);

    m_popup->special_child_draw_callback(parent->special_child_draw_callback());
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
    if (m_selected != static_cast<ssize_t>(index))
    {
        if (index < m_items.size())
        {
            m_selected = index;
            damage();
            on_selected_changed.invoke();
        }
    }
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

Size ComboBox::min_size_hint() const
{
    if (!m_min_size.empty())
        return m_min_size;

    return DEFAULT_COMBOBOX_SIZE + Widget::min_size_hint();
}

void ComboBox::draw(Painter& painter, const Rect& rect)
{
    Drawer<ComboBox>::draw(*this, painter, rect);
}

void ComboBox::default_draw(ComboBox& widget, Painter& painter, const Rect& /*rect*/)
{
    widget.draw_box(painter, Palette::ColorId::bg, Palette::ColorId::border);

    const auto b = widget.content_area();
    const auto handle_dim = std::min(b.width(), b.height());
    const auto handle = Rect(b.top_right() - Point(handle_dim, 0), Size(handle_dim, handle_dim));
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

    if (widget.selected() >= 0 && widget.selected() < static_cast<ssize_t>(widget.item_count()))
    {
        // text
        painter.set(widget.color(Palette::ColorId::text));
        painter.set(widget.font());
        const auto size = painter.text_size(widget.item_at(widget.selected()));
        const auto target = detail::align_algorithm(size,
                            text,
                            widget.text_align());
        painter.draw(target.point());
        painter.draw(widget.item_at(widget.selected()));
    }
}

void ComboBox::serialize(Serializer& serializer) const
{
    Widget::serialize(serializer);

    auto index = 0;
    for (const auto& item : m_items)
    {
        if (index == selected())
            serializer.add_property("item", item, {{"selected", "true"}});
        else
            serializer.add_property("item", item);

        ++index;
    }
}

void ComboBox::deserialize(const std::string& name, const std::string& value,
                           const Serializer::Attributes& attrs)
{
    if (name == "item")
    {
        add_item(value);
        for (const auto& i : attrs)
        {
            if (i.first == "selected" && detail::from_string(i.second))
                selected(item_count() - 1);
        }
    }
    else
        Widget::deserialize(name, value, attrs);
}

}
}
