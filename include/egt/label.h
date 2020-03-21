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

#include <egt/detail/meta.h>
#include <egt/image.h>
#include <egt/textwidget.h>
#include <memory>
#include <string>

namespace egt
{
inline namespace v1
{
class Frame;
class Painter;

/**
 * A Label that displays text.
 *
 * It handles no events, has a text alignment, but otherwise is pretty
 * basic.
 *
 * Supported Features:
 * - UTF-8 encoding
 * - Multi-line
 *
 * @ingroup controls
 */
class EGT_API Label : public TextWidget
{
public:

    /// Default value used for text alignment.
    static const AlignFlags DEFAULT_TEXT_ALIGN;

    /**
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     */
    explicit Label(const std::string& text = {},
                   const AlignFlags& text_align = DEFAULT_TEXT_ALIGN) noexcept;

    /**
     * @param[in] text The text to display.
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] text_align Alignment for the text.
     */
    Label(const std::string& text,
          const Rect& rect,
          const AlignFlags& text_align = DEFAULT_TEXT_ALIGN) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     */
    explicit Label(Frame& parent,
                   const std::string& text = {},
                   const AlignFlags& text_align = DEFAULT_TEXT_ALIGN) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] text The text to display.
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] text_align Alignment for the text.
     */
    Label(Frame& parent,
          const std::string& text,
          const Rect& rect,
          const AlignFlags& text_align = DEFAULT_TEXT_ALIGN) noexcept;

    using TextWidget::text;

    virtual void draw(Painter& painter, const Rect& rect) override;

    /// Default draw method for the widget.
    static void default_draw(Label& widget, Painter& painter, const Rect& rect);

    using TextWidget::min_size_hint;

    virtual Size min_size_hint() const override;

    virtual ~Label() = default;

protected:

    using TextWidget::parent;

    virtual void parent(Frame* parent) override;
};

/**
 * Label that also contains an Image.
 *
 * The interesting thing about this Widget is the position of the text
 * relative to the image.  Alignment of text works as usual, however, the image
 * is aligned relative to the position of the text.  If there is no text, the
 * image is aligned relative to the Widget::box() as the text would be.
 *
 * @image html widget_imagelabel1.png
 * @image latex widget_imagelabel1.png "widget_imagelabel1" width=5cm
 * @image html widget_imagelabel2.png
 * @image latex widget_imagelabel2.png "widget_imagelabel2" width=5cm
 *
 * @ingroup controls
 */
class EGT_API ImageLabel : public Label
{
public:

    /**
    * @param[in] text The text to display.
    * @param[in] text_align Alignment for the text.
    */
    explicit ImageLabel(const std::string& text = {},
                        const AlignFlags& text_align = DEFAULT_TEXT_ALIGN) noexcept;

    /**
     * @param[in] image The image to display.
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     */
    explicit ImageLabel(const Image& image,
                        const std::string& text = {},
                        const AlignFlags& text_align = DEFAULT_TEXT_ALIGN) noexcept;

    /**
     * @param[in] image The image to display.
     * @param[in] text The text to display.
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] text_align Alignment for the text.
     */
    ImageLabel(const Image& image,
               const std::string& text,
               const Rect& rect,
               const AlignFlags& text_align = DEFAULT_TEXT_ALIGN) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] image The image to display.
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     */
    explicit ImageLabel(Frame& parent,
                        const Image& image = {},
                        const std::string& text = {},
                        const AlignFlags& text_align = DEFAULT_TEXT_ALIGN) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] image The image to display.
     * @param[in] text The text to display.
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] text_align Alignment for the text.
     */
    ImageLabel(Frame& parent,
               const Image& image,
               const std::string& text,
               const Rect& rect,
               const AlignFlags& text_align = DEFAULT_TEXT_ALIGN) noexcept;

    virtual void draw(Painter& painter, const Rect& rect) override;

    /// Default draw method for the widget.
    static void default_draw(ImageLabel& widget, Painter& painter, const Rect& rect);

    using TextWidget::min_size_hint;

    virtual Size min_size_hint() const override;

    /**
     * Set a new Image.
     *
     * @param[in] image The new image to use.
     */
    virtual void image(const Image& image);

    /**
     * Scale the image.
     *
     * Change the scale of the image.
     *
     * This scales relative to the original size of the image.  Not the result
     * of any subsequent Image::resize() or Image::scale() call.
     *
     * @see Image::scale().
     *
     * @param[in] hscale Horizontal scale [0.0 - 1.0], with 1.0 being 100%.
     * @param[in] vscale Vertical scale [0.0 - 1.0], with 1.0 being 100%.
     * @param[in] approximate Approximate the scale to increase image cache
     *            hit efficiency.
     */
    virtual void scale_image(float hscale, float vscale,
                             bool approximate = false)
    {
        m_image.scale(hscale, vscale, approximate);
        parent_layout();
    }

    /**
     * Scale the image.
     *
     * @param[in] scale Vertical and horizontal scale [0.0 - 1.0], with 1.0 being 100%.
     * @param[in] approximate Approximate the scale to increase image cache
     *            hit efficiency.
     *
     * @warning This does not damage the widget.
     */
    virtual void scale_image(float scale, bool approximate = false)
    {
        scale_image(scale, scale, approximate);
    }

    /**
     * Get a const reference of the image.
     */
    const Image& image() const { return m_image; }

    /**
     * Get a non-const reference to the image.
     */
    Image& image() { return m_image; }

    /**
     * Set the alignment of the image relative to the text.
     *
     * @param[in] align Only left, right, top, and bottom alignments are supported.
     */
    void image_align(const AlignFlags& align)
    {
        if (detail::change_if_diff<>(m_image_align, align))
            damage();
    }

    /**
     * Get the image alignment.
     */
    AlignFlags image_align() const { return m_image_align; }

    /**
     * Enable/disable showing the label text.
     *
     * @param[in] value When true, the label text is shown.
     */
    virtual void show_label(bool value);

    /**
     * Get the show label state.
     */
    bool show_label() const { return m_show_label; }

    virtual void serialize(Serializer& serializer) const override;

    virtual void deserialize(const std::string& name, const std::string& value,
                             const Serializer::Attributes& attrs) override;

    virtual ~ImageLabel() = default;

protected:

    /// @private
    void do_set_image(const Image& image);

    /// The image. Allowed to be empty.
    Image m_image;

    /// When true, the label text is shown.
    bool m_show_label{true};

    /// Alignment of the image relative to the text.
    AlignFlags m_image_align{AlignFlag::left};
};

}
}

#endif
