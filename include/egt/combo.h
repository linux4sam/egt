/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_COMBO_H
#define EGT_COMBO_H

/**
 * @file
 * @brief ComboBox definition.
 */

#include <egt/widget.h>
#include <vector>
#include <string>

namespace egt
{

    /**
             * Combo box widget.
             */
    class ComboBox : public TextWidget
    {
    public:
        using item_array = std::vector<std::string>;

        ComboBox(const item_array& items = item_array(),
                 const Rect& rect = Rect(),
                 alignmask align = alignmask::CENTER | alignmask::LEFT,
                 const Font& font = Font(),
                 widgetmask flags = widgetmask::NO_BORDER) noexcept;

        virtual int handle(eventid event) override;

        virtual void set_select(uint32_t index);

        virtual uint32_t selected() const { return m_selected; }

        virtual void draw(Painter& painter, const Rect& rect) override;

        virtual ~ComboBox();

    protected:
        item_array m_items;
        uint32_t m_selected{0};
        Rect m_up_rect;
        Rect m_down_rect;
    };

}

#endif
