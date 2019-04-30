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
 *
 * @ingroup controls
 */
class Label : public TextWidget
{
public:

    static const alignmask default_align;

    /**
     * @param[in] text The text to display.
     * @param[in] align Alignment for the text.
     * @param[in] font The Font to use.
     */
    /// @todo All uses of Font() below are not using the theme font
    Label(const std::string& text = std::string(),
          alignmask align = default_align,
          const Font& font = Font()) noexcept;

    /**
     * @param[in] text The text to display.
     * @param[in] rect Rectangle for the widget.
     * @param[in] align Alignment for the text.
     * @param[in] font The Font to use.
     */
    Label(const std::string& text, const Rect& rect,
          alignmask align = default_align,
          const Font& font = Font()) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] text The text to display.
     * @param[in] align Alignment for the text.
     * @param[in] font The Font to use.
     */
    explicit Label(Frame& parent, const std::string& text = std::string(),
                   alignmask align = default_align,
                   const Font& font = Font()) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] text The text to display.
     * @param[in] rect Rectangle for the widget.
     * @param[in] align Alignment for the text.
     * @param[in] font The Font to use.
     */
    Label(Frame& parent, const std::string& text, const Rect& rect,
          alignmask align = default_align,
          const Font& font = Font()) noexcept;

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

    virtual Size min_size_hint() const override;

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
 *
 * @ingroup controls
 */
class ImageLabel : public Label
{
public:

    /**
     * @param[in] image The image to display.
     * @param[in] text The text to display.
     * @param[in] align Alignment for the text.
     * @param[in] font The Font to use.
     */
    ImageLabel(const Image& image = Image(),
               const std::string& text = std::string(),
               alignmask align = alignmask::right | alignmask::center,
               const Font& font = Font()) noexcept;

    /**
     * @param[in] image The image to display.
     * @param[in] text The text to display.
     * @param[in] rect Rectangle for the widget.
     * @param[in] align Alignment for the text.
     * @param[in] font The Font to use.
     */
    ImageLabel(const Image& image, const std::string& text, const Rect& rect,
               alignmask align = alignmask::right | alignmask::center,
               const Font& font = Font()) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] image The image to display.
     * @param[in] text The text to display.
     * @param[in] align Alignment for the text.
     * @param[in] font The Font to use.
     */
    explicit ImageLabel(Frame& parent, const Image& image = Image(),
                        const std::string& text = std::string(),
                        alignmask align = alignmask::right | alignmask::center,
                        const Font& font = Font()) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] image The image to display.
     * @param[in] text The text to display.
     * @param[in] rect Rectangle for the widget.
     * @param[in] align Alignment for the text.
     * @param[in] font The Font to use.
     */
    ImageLabel(Frame& parent, const Image& image, const std::string& text,
               const Rect& rect,
               alignmask align = alignmask::right | alignmask::center,
               const Font& font = Font()) noexcept;

    virtual std::unique_ptr<Widget> clone() override
    {
        return std::unique_ptr<Widget>(make_unique<ImageLabel>(*this).release());
    }

    virtual void draw(Painter& painter, const Rect& rect) override;

    static void default_draw(ImageLabel& widget, Painter& painter, const Rect& rect);

    /**
     * Set a new Image.
     */
    virtual void set_image(const Image& image);

    const Image& image() const { return m_image; }

    /** @deprecated */
    Image& image() { return m_image; }

    virtual void label_enabled(bool value);

    virtual void set_image_align(alignmask align)
    {
        if (detail::change_if_diff<>(m_image_align, align))
            damage();
    }

    void set_position_image_first(bool value)
    {
        if (detail::change_if_diff<>(m_position_image_first, value))
            damage();
    }

    virtual Size min_size_hint() const override
    {
        // if we are expanding the image, don't use it for min size hint
        if ((m_image_align & alignmask::expand_horizontal) == alignmask::expand_horizontal ||
            (m_image_align & alignmask::expand_vertical) == alignmask::expand_vertical)
            return Widget::min_size_hint();

        return m_image.size() + Widget::min_size_hint();
    }

    virtual ~ImageLabel() = default;

protected:

    /**
     * Scale the image.
     *
     * @param[in] hscale Horizontal scale, with 1.0 being 100%.
     * @param[in] vscale Vertical scale, with 1.0 being 100%.
     * @param[in] approximate Approximate the scale to increase image cache
     *            hit efficiency.
     */
    virtual void scale_image(double hscale, double vscale,
                             bool approximate = false);

    virtual void scale_image(double s, bool approximate = false);

    virtual void first_resize() override;

    Image m_image;
    bool m_show_label{true};
    alignmask m_image_align{alignmask::center | alignmask::left};
    bool m_position_image_first{false};
};

}
}

#endif
