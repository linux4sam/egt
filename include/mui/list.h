/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_LIST_H
#define MUI_LIST_H

/**
 * @file
 * @brief ListBox definition.
 */

#include <mui/widget.h>
#include <vector>

namespace mui
{


    /**
     * ListBox that manages a selectable list of items.
     */
    class ListBox : public Widget
    {
    public:
        using item_array = std::vector<std::string>;

        ListBox(const item_array& items,
                const Point& point = Point(),
                const Size& size = Size());

        /**
         * Set the font of the items.
         */
        virtual void font(const Font& font) { m_font = font; }

        virtual int handle(int event) override;

        virtual void draw(Painter& painter, const Rect& rect) override;

        virtual void select(uint32_t index);

        uint32_t selected() const { return m_selected; }

        virtual ~ListBox();

    protected:

        virtual void on_selected(int index) {ignoreparam(index);}

        Rect item_rect(uint32_t index) const;

        item_array m_items;
        uint32_t m_selected = { 0 };
        Font m_font;
    };

}

#endif
