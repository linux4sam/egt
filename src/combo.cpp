/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/combo.h"
#include "egt/painter.h"
#include "egt/frame.h"
#include "egt/imagecache.h"

namespace egt
{

ComboBox::ComboBox(const item_array& items,
                           const Rect& rect,
                           alignmask align,
                           const Font& font,
                           widgetmask flags) noexcept
            : TextWidget("", rect, align, font, flags),
              m_items(items)
        {
            flag_set(widgetmask::GRAB_MOUSE);
            palette().set(Palette::BG, Palette::GROUP_NORMAL, palette().color(Palette::LIGHT));
            palette().set(Palette::BG, Palette::GROUP_ACTIVE, palette().color(Palette::LIGHT));
        }

        int ComboBox::handle(eventid event)
        {
            auto ret = Widget::handle(event);

            switch (event)
            {
            case eventid::MOUSE_DOWN:
            {
                Point mouse = from_screen(event_mouse());

                if (Rect::point_inside(mouse, m_down_rect - box().point()))
                {
                    if (m_selected > 0)
                        set_select(m_selected - 1);
                }
                else if (Rect::point_inside(mouse, m_up_rect - box().point()))
                {
                    set_select(m_selected + 1);
                }

                break;
            }
            default:
                break;
            }

            return ret;
        }

        void ComboBox::set_select(uint32_t index)
        {
            if (m_selected != index)
            {
                if (index < m_items.size())
                {
                    m_selected = index;
                    damage();
                    invoke_handlers(eventid::PROPERTY_CHANGED);
                }
            }
        }

        void ComboBox::draw(Painter& painter, const Rect& rect)
        {
            ignoreparam(rect);

            // box
            painter.draw_box(*this, Painter::boxtype::rounded_border);

            // images
            auto OFFSET = 5;

            auto upsize = Size(h() - OFFSET * 2, h() - OFFSET * 2);
            auto downsize = Size(h() - OFFSET * 2, h() - OFFSET * 2);

            auto scale = static_cast<float>(upsize.w) /
                         static_cast<float>(Painter::surface_to_size(detail::image_cache.get("up.png")).w);

            auto up = detail::image_cache.get("up.png", scale, scale);
            auto down = detail::image_cache.get("down.png", scale, scale);

            m_up_rect = Rect(Point(x() + w() - upsize.w - downsize.w - OFFSET * 2,
                                   y() + OFFSET),
                             upsize);
            m_down_rect = Rect(Point(x() + w() - downsize.w - OFFSET,
                                     y() + OFFSET),
                               downsize);

            painter.draw_image(m_up_rect.point(), up);
            painter.draw_image(m_down_rect.point(), down);

            // text
            painter.set_color(palette().color(Palette::TEXT));
            painter.set_font(font());
            auto textbox = box();
            textbox -= Size(m_up_rect.w, 0);
            painter.draw_text(textbox, m_items[m_selected], m_text_align, 5);
        }

        ComboBox::~ComboBox()
        {
        }

}
