/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_LABEL_H
#define EGT_LABEL_H

/**
 * @file
 * @brief Working with labels.
 */

#include <egt/font.h>
#include <egt/image.h>
#include <egt/widget.h>

namespace egt
{

    /**
     * A Label in its basic form is just some text.
     *
     * It handles no events, has a font and alignment, but otherwise is pretty
     * basic.
     *
     * @image html widget_label1.png
     * @image latex widget_label1.png "widget_label1" width=5cm
     * @image html widget_label2.png
     * @image latex widget_label2.png "widget_label2" width=5cm
     * @image html widget_label3.png
     * @image latex widget_label3.png "widget_label3" width=5cm
     * @image html widget_label4.png
     * @image latex widget_label4.png "widget_label4" width=5cm
     */
    class Label : public TextWidget
    {
    public:
        explicit Label(const std::string& text = std::string(),
                       const Rect& rect = Rect(),
                       alignmask align = alignmask::CENTER,
                       const Font& font = Font(),
                       widgetmask flags = widgetmask::NO_BORDER) noexcept;

        explicit Label(Frame& parent,
                       const std::string& text = std::string(),
                       const Rect& rect = Rect(),
                       alignmask align = alignmask::CENTER,
                       const Font& font = Font(),
                       widgetmask flags = widgetmask::NO_BORDER) noexcept;

        /**
         * Set the text of the label.
         */
        virtual void set_text(const std::string& str) override;

        virtual void draw(Painter& painter, const Rect& rect) override;

        virtual ~Label();

    protected:

        void first_resize();
    };

    /**
     * Boolean checkbox.
     *
     * @image html widget_checkbox1.png
     * @image latex widget_checkbox1.png "widget_checkbox1" width=5cm
     * @image html widget_checkbox2.png
     * @image latex widget_checkbox2.png "widget_checkbox2" width=5cm
     *
     * @todo This should be a ValueWidget<bool>.
     */
    class CheckBox : public Label
    {
    public:
        CheckBox(const std::string& text = std::string(),
                 const Rect& rect = Rect());

        /**
         * Return the boolean state of the checkbox.
         */
        inline bool checked() const
        {
            return active();
        }

        /**
         * Set the checked state of the checkbox.
         */
        void check(bool value)
        {
            if (active() != value)
            {
                set_active(value);
                invoke_handlers(eventid::PROPERTY_CHANGED);
            }
        }

        virtual int handle(eventid event) override;

        virtual void draw(Painter& painter, const Rect& rect) override;

        virtual ~CheckBox();
    };

    /**
     * CheckBox with a boolean slider style interface.
     *
     * @image html widget_slidingcheckbox.png
     * @image latex widget_slidingcheckbox.png "widget_slidingcheckbox" width=5cm
     */
    class SlidingCheckBox : public CheckBox
    {
    public:
        explicit SlidingCheckBox(const Rect& rect = Rect());

        virtual void draw(Painter& painter, const Rect& rect) override;

        virtual ~SlidingCheckBox();
    };

    /**
     * A Label widget that also contains an image.
     *
     * The interesting thing about this widget is the position of the text
     * relative to the image.  Alignment of txt usually works relative to the
     * bounding box of the widget.  However, in this case the text is bumped up
     * against the image and aligned relative to the image instead.
     *
     * @image html widget_imagelabel1.png
     * @image latex widget_imagelabel1.png "widget_imagelabel1" width=5cm
     * @image html widget_imagelabel2.png
     * @image latex widget_imagelabel2.png "widget_imagelabel2" width=5cm
     */
    class ImageLabel : public Label
    {
    public:
        ImageLabel(const Image& image,
                   const std::string& text = std::string(),
                   const Rect& rect = Rect(),
                   const Font& font = Font());

        ImageLabel(const Image& image,
                   const std::string& text,
                   const Point& point,
                   const Font& font = Font());

        ImageLabel(const Image& image,
                   const Point& point);

        ImageLabel(Frame& parent,
                   const Image& image,
                   const std::string& text = std::string(),
                   const Rect& rect = Rect(),
                   const Font& font = Font());

        /**
             * Scale the image.
             *
             * Change the size of the widget, similar to calling resize().
             *
             * @param[in] hscale Horizontal scale, with 1.0 being 100%.
             * @param[in] vscale Vertical scale, with 1.0 being 100%.
             * @param[in] approximate Approximate the scale to increase image cache
             *            hit efficiency.
             */
        virtual void scale(double hscale, double vscale,
                           bool approximate = false)
        {
            m_image.scale(hscale, vscale, approximate);
            m_box = Rect(m_box.point(), m_image.size());
        }

        virtual void resize(const Size& size) override
        {
            if (m_text.empty())
            {
                if (this->size() != size)
                {
                    double hs = (double)size.w / (double)m_image.size_orig().w;
                    double vs = (double)size.h / (double)m_image.size_orig().h;
                    scale(hs, vs);
                }
            }
            else
            {
                Widget::resize(size);
            }
        }

        virtual void draw(Painter& painter, const Rect& rect) override;

        virtual void label_enabled(bool value);

        virtual ~ImageLabel();

    protected:
        Image m_image;
        bool m_label{true};
    };

}

#endif
