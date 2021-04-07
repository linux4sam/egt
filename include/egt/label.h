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

    /// Change text align.
    static AlignFlags default_text_align();
    /// Change default text align.
    static void default_text_align(const AlignFlags& align);

    /**
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     */
    explicit Label(const std::string& text = {},
                   const AlignFlags& text_align = default_text_align()) noexcept;

    /**
     * @param[in] text The text to display.
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] text_align Alignment for the text.
     */
    Label(const std::string& text,
          const Rect& rect,
          const AlignFlags& text_align = default_text_align()) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     */
    explicit Label(Frame& parent,
                   const std::string& text = {},
                   const AlignFlags& text_align = default_text_align()) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] text The text to display.
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] text_align Alignment for the text.
     */
    Label(Frame& parent,
          const std::string& text,
          const Rect& rect,
          const AlignFlags& text_align = default_text_align()) noexcept;

    /**
     * @param[in] props list of widget argument and its properties.
     */
    explicit Label(Serializer::Properties& props) noexcept;

    using TextWidget::text;

    void draw(Painter& painter, const Rect& rect) override;

    /// Default draw method for the widget.
    static void default_draw(const Label& widget, Painter& painter, const Rect& rect);

    using TextWidget::min_size_hint;

    EGT_NODISCARD Size min_size_hint() const override;

protected:

    void set_parent(Frame* parent) override;
};

/**
 * Label that also contains an Image.
 *
 * The interesting thing about this Widget is the position of the text
 * relative to the image.  Alignment of text works as usual; however, the image
 * is aligned relative to the position of the text.  If there is no text, the
 * image is aligned relative to the Widget::box() as the text would be.
 *
 * Layout rules:
 * - When text is displayed, the auto scale feature has no effect on the layout.
 * - Unless you disabled the auto resize feature, the size of the button will be
 *   updated to the original size of the image plus extra space for the text.
 * - If you specify a size for the button, and there is no text, the image will
 *   be scaled up or down to fit the button size. You can disable this feature
 *   and you can choose if you want to keep the original ratio of the image
 *   during this operation.
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
                        const AlignFlags& text_align = default_text_align()) noexcept;

    /**
     * @param[in] image The image to display.
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     */
    explicit ImageLabel(const Image& image,
                        const std::string& text = {},
                        const AlignFlags& text_align = default_text_align()) noexcept;

    /**
     * @param[in] image The image to display.
     * @param[in] text The text to display.
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] text_align Alignment for the text.
     */
    ImageLabel(const Image& image,
               const std::string& text,
               const Rect& rect,
               const AlignFlags& text_align = default_text_align()) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] image The image to display.
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     */
    explicit ImageLabel(Frame& parent,
                        const Image& image = {},
                        const std::string& text = {},
                        const AlignFlags& text_align = default_text_align()) noexcept;

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
               const AlignFlags& text_align = default_text_align()) noexcept;

    /**
     * @param[in] props list of widget argument and its properties.
     */
    explicit ImageLabel(Serializer::Properties& props) noexcept;

    void draw(Painter& painter, const Rect& rect) override;

    /// Default draw method for the widget.
    static void default_draw(ImageLabel& widget, Painter& painter, const Rect& rect);

    using TextWidget::min_size_hint;

    EGT_NODISCARD Size min_size_hint() const override;

    /**
     * Set a new Image.
     *
     * @param[in] image The new image to use.
     */
    void image(const Image& image);

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
    void scale_image(float hscale, float vscale,
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
    void scale_image(float scale, bool approximate = false)
    {
        scale_image(scale, scale, approximate);
    }

    /**
     * Enable/disable the auto scale of the image.
     *
     * If enabled, the image will be automatically scaled to fit within the
     * label box. By default, set to true.
     */
    void auto_scale_image(bool enable)
    {
        m_auto_scale_image = enable;
    }

    /**
     * Get the auto scale image state.
     */
    bool auto_scale_image() const
    {
        return m_auto_scale_image;
    }

    /**
     * Enable/disable ratio preservation while scaling the image.
     *
     * If enabled, the original image ratio won't be changed during the image
     * auto scale operation. By default, set to true.
     */
    void keep_image_ratio(bool enable)
    {
        m_keep_image_ratio = enable;
    }

    /**
     * Get the keep image ratio state.
     */
    bool keep_image_ratio() const
    {
        return m_keep_image_ratio;
    }

    /**
     * Get a const reference of the image.
     */
    EGT_NODISCARD const Image& image() const { return m_image; }

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
    EGT_NODISCARD AlignFlags image_align() const { return m_image_align; }

    /**
     * Enable/disable showing the label text.
     *
     * @param[in] value When true, the label text is shown.
     */
    void show_label(bool value);

    /**
     * Get the show label state.
     */
    EGT_NODISCARD bool show_label() const { return m_show_label; }

    void serialize(Serializer& serializer) const override;

protected:

    /// @private
    void do_set_image(const Image& image);

    /// The image. Allowed to be empty.
    Image m_image;

    /// When true, the image is scaled to fit within the label box.
    bool m_auto_scale_image{true};

    /// When true, the image ratio is kept while scaled.
    bool m_keep_image_ratio{true};

    /// When true, the label text is shown.
    bool m_show_label{true};

    /// Alignment of the image relative to the text.
    AlignFlags m_image_align{AlignFlag::left};

private:

    void deserialize(Serializer::Properties& props) override;
};

}
}

#endif
