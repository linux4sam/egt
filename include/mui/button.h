/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_BUTTON_H
#define MUI_BUTTON_H

/**
 * @file
 * @brief Working with buttons.
 */

#include <mui/label.h>

namespace mui
{

    /**
     * Basic button widget.
     *
     * @image html widget_button.png
     * @image latex widget_button.png "widget_button" height=10cm
     */
    class Button : public Label
    {
    public:

        Button(const std::string& text = std::string(),
               const Rect& rect = Rect(),
               widgetmask flags = widgetmask::NONE) noexcept;

        virtual int handle(int event) override;

        virtual void draw(Painter& painter, const Rect& rect) override;

        virtual ~Button();
    };

    /**
     * A button widget that can have an image and/or a label on it.
     */
    class ImageButton : public Button
    {
    public:
        ImageButton(const std::string& image,
                    const std::string& text = "",
                    const Rect& rect = Rect(),
                    widgetmask flags = widgetmask::NONE) noexcept;

        virtual void draw(Painter& painter, const Rect& rect) override;

        void set_image(const std::string& image);

        void set_image_align(alignmask align)
        {
            m_image_align = align;
        }

        void set_label_align(alignmask align)
        {
            Label::text_align(align);
        }

        virtual ~ImageButton();

    protected:

        void do_set_image(const std::string& image);

        shared_cairo_surface_t m_image;
        alignmask m_image_align;
    };

    namespace experimental
    {

        /**
         * A special widget that has no content but receives events.
         *
         * This is an invisible widget that can be used to handle events, like
         * input events.
         */
        class HotSpot : public Button
        {
        public:

            HotSpot(const Rect& rect = Rect(),
                    widgetmask flags = widgetmask::NO_BACKGROUND | widgetmask::NO_BORDER) noexcept
                : Button("", rect, flags)
            {
                hide();
            }

            virtual void draw(Painter& painter, const Rect& rect) override
            {
                ignoreparam(painter);
                ignoreparam(rect);
            }

            virtual ~HotSpot() {}

        private:
            virtual void show() {}
        };

    }


}

#endif
