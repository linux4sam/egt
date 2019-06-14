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

#include <egt/detail/textwidget.h>
#include <egt/image.h>
#include <memory>
#include <string>

namespace egt
{
inline namespace v1
{
class Frame;
class Painter;

/**
 * A Label of text.
 *
 * It handles no events, has a text alignment, but otherwise is pretty
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
 *
 * @ingroup controls
 */
class Label : public detail::TextWidget
{
public:

    static const alignmask default_align;

    /**
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     */
    Label(const std::string& text = {},
          alignmask text_align = default_align) noexcept;

    /**
     * @param[in] text The text to display.
     * @param[in] rect Rectangle for the widget.
     * @param[in] text_align Alignment for the text.
     */
    Label(const std::string& text, const Rect& rect,
          alignmask text_align = default_align) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     */
    explicit Label(Frame& parent, const std::string& text = {},
                   alignmask text_align = default_align) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] text The text to display.
     * @param[in] rect Rectangle for the widget.
     * @param[in] text_align Alignment for the text.
     */
    Label(Frame& parent, const std::string& text, const Rect& rect,
          alignmask text_align = default_align) noexcept;

    /**
     * Set the text of the label.
     */
    virtual void set_text(const std::string& str) override;

    virtual void draw(Painter& painter, const Rect& rect) override;

    static void default_draw(Label& widget, Painter& painter, const Rect& rect);

    virtual Size min_size_hint() const override;

    virtual ~Label() = default;

protected:

    virtual void set_parent(Frame* parent) override;

    virtual void first_resize();
};

/**
 * Label that also contains an Image.
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
 *
 * @ingroup controls
 */
class ImageLabel : public Label
{
public:

    /**
    * @param[in] text The text to display.
    * @param[in] text_align Alignment for the text.
    */
    ImageLabel(const std::string& text = {},
               alignmask text_align = default_align) noexcept;

    /**
     * @param[in] image The image to display.
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     */
    ImageLabel(const Image& image,
               const std::string& text = {},
               alignmask text_align = default_align) noexcept;

    /**
     * @param[in] image The image to display.
     * @param[in] text The text to display.
     * @param[in] rect Rectangle for the widget.
     * @param[in] text_align Alignment for the text.
     */
    ImageLabel(const Image& image, const std::string& text, const Rect& rect,
               alignmask text_align = default_align) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] image The image to display.
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     */
    explicit ImageLabel(Frame& parent, const Image& image = {},
                        const std::string& text = {},
                        alignmask text_align = default_align) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] image The image to display.
     * @param[in] text The text to display.
     * @param[in] rect Rectangle for the widget.
     * @param[in] text_align Alignment for the text.
     */
    ImageLabel(Frame& parent, const Image& image, const std::string& text,
               const Rect& rect,
               alignmask text_align = default_align) noexcept;

    virtual void draw(Painter& painter, const Rect& rect) override;

    virtual Size min_size_hint() const override;

    static void default_draw(ImageLabel& widget, Painter& painter, const Rect& rect);

    /**
     * Set a new Image.
     *
     * @param image The new image to use.
     */
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
                             bool approximate = false)
    {
        m_image.scale(hscale, vscale, approximate);
        m_box = Rect(m_box.point(), m_image.size());
    }

    /**
     * Scale the image.
     *
     * @param[in] s Vertical and horizontal scale, with 1.0 being 100%.
     * @param[in] approximate Approximate the scale to increase image cache
     *            hit efficiency.
     */
    virtual void scale_image(double s, bool approximate = false)
    {
        scale_image(s, s, approximate);
    }

    /**
     * Get a const reference of the image.
     */
    inline const Image& image() const { return m_image; }

    /**
     * Get a non-const reference to the image.
     */
    inline Image& image() { return m_image; }

    /**
     * Set the alignment of the image relative to the text.
     *
     * @param[in] align Only left, right, top, and bottom alignments are supported.
     */
    virtual void set_image_align(alignmask align)
    {
        if (detail::change_if_diff<>(m_image_align, align))
            damage();
    }

    /**
     * Get the image alignment.
     */
    inline alignmask image_align() const { return m_image_align; }

    /**
     * Enable/disable showing the label text.
     *
     * @param[in] value When true, the label text is shown.
     */
    virtual void set_show_label(bool value);

    /**
    * Get the show label state.
    */
    inline bool show_label() const { return m_show_label; }

    virtual ~ImageLabel() = default;

protected:

    virtual void first_resize() override;

    Image m_image;
    bool m_show_label{true};
    alignmask m_image_align{alignmask::left};
};

}
}

#endif
