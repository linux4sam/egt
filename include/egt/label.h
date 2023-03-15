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
#include <egt/imageholder.h>
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
    explicit Label(Serializer::Properties& props) noexcept
        : Label(props, false)
    {
    }

protected:

    explicit Label(Serializer::Properties& props, bool is_derived) noexcept;

public:

    Label(const Label&) = delete;
    Label& operator=(const Label&) = delete;
    Label(Label&&) noexcept = default;
    Label& operator=(Label&&) noexcept = default;

    using TextWidget::text;

    void draw(Painter& painter, const Rect& rect) override;

    /// Default draw method for the widget.
    static void default_draw(const Label& widget, Painter& painter, const Rect& rect);

    using TextWidget::min_size_hint;

    EGT_NODISCARD Size min_size_hint() const override;

protected:

    void set_parent(Widget* parent) override;
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
using ImageLabel = ImageHolder<Label, Palette::ColorId::label_bg, Palette::ColorId::border, Palette::ColorId::label_text>;

}
}

#endif
