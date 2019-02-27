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

#include <egt/image.h>
#include <egt/textwidget.h>

namespace egt
{
inline namespace v1
{
class Font;
class Frame;
class Painter;

/**
 * @brief A Label of text.
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
    Label(const std::string& text = std::string(),
          const Rect& rect = Rect(),
          alignmask align = alignmask::center,
          const Font& font = Font(),
          const Widget::flags_type& flags = Widget::flags_type()) noexcept;

    explicit Label(Frame& parent,
                   const std::string& text,
                   const Rect& rect = Rect(),
                   alignmask align = alignmask::center,
                   const Font& font = Font(),
                   const Widget::flags_type& flags = Widget::flags_type()) noexcept;

    explicit Label(Frame& parent,
                   const std::string& text = std::string(),
                   alignmask align = alignmask::center,
                   const Font& font = Font(),
                   const Widget::flags_type& flags = Widget::flags_type()) noexcept;

    virtual std::unique_ptr<Widget> clone() override
    {
        return std::unique_ptr<Widget>(make_unique<Label>(*this).release());
    }

    /**
     * Set the text of the label.
     */
    virtual void set_text(const std::string& str) override;

    virtual void draw(Painter& painter, const Rect& rect) override;

    static void default_draw(Label& widget, Painter& painter, const Rect& rect);

    virtual ~Label() = default;

protected:

    virtual void set_parent(Frame* parent) override;

    virtual void first_resize();
};

/**
 * @brief Label that also contains an Image.
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
               const Font& font = Font()) noexcept;

    ImageLabel(const Image& image,
               const std::string& text,
               const Point& point,
               const Font& font = Font()) noexcept;

    ImageLabel(const Image& image,
               const Point& point) noexcept;

    ImageLabel(Frame& parent,
               const Image& image,
               const std::string& text = std::string(),
               const Rect& rect = Rect(),
               const Font& font = Font()) noexcept;

    virtual std::unique_ptr<Widget> clone() override
    {
        return std::unique_ptr<Widget>(make_unique<ImageLabel>(*this).release());
    }

    virtual void draw(Painter& painter, const Rect& rect) override;

    static void default_draw(ImageLabel& widget, Painter& painter, const Rect& rect);

    virtual void set_image(const Image& image);

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
    virtual void scale_image(double hscale, double vscale,
                             bool approximate = false);

    virtual void scale_image(double s, bool approximate = false);

#if 0
    virtual void resize(const Size& size) override;
#endif

    const Image& image() const { return m_image; }

    Image& image() { return m_image; }

    virtual void label_enabled(bool value);

    virtual void set_image_align(alignmask align)
    {
        if (m_image_align != align)
        {
            m_image_align = align;
            damage();
        }
    }

    void set_position_image_first(bool value)
    {
        if (m_position_image_first != value)
        {
            m_position_image_first = value;
            damage();
        }
    }

    virtual ~ImageLabel() = default;

protected:
    virtual void first_resize() override;

    Image m_image;
    bool m_show_label{true};
    alignmask m_image_align{alignmask::center | alignmask::left};
    bool m_position_image_first{false};
};

}
}

#endif
