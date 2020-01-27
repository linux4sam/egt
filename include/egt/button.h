/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_BUTTON_H
#define EGT_BUTTON_H

/**
 * @file
 * @brief Working with buttons.
 */

#include <egt/buttongroup.h>
#include <egt/detail/meta.h>
#include <egt/detail/textwidget.h>
#include <egt/geometry.h>
#include <egt/image.h>
#include <egt/theme.h>
#include <memory>
#include <string>

namespace egt
{
inline namespace v1
{

class Frame;
class Painter;

/**
 * @defgroup controls Controls
 * User interface control widgets.
 *
 * Control are widgets that usually provide some interaction with the user- even
 * if that interaction is only visual.  For example, displaying a value.
 */

/**
 * Basic button control.
 *
 * This presents a clickable control that can be used to respond to user pointer
 * events.
 *
 * Supported Features:
 * - UTF-8 encoding
 * - Multi-line
 *
 * @b Example
 * @code{.cpp}
 * Button btn("Button");
 * // handle the clicked event of the button
 * btn.on_event([](Event&){
 *     cout << "clicked!" << endl;
 * }, {EventId::pointer_click});
 * @endcode
 *
 * @ingroup controls
 *
 * @image html widget_button.png
 * @image latex widget_button.png "widget_button" width=5cm
 */
class EGT_API Button : public detail::TextWidget
{
public:

    static const AlignFlags DEFAULT_TEXT_ALIGN;

    /**
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     */
    explicit Button(const std::string& text = {},
                    const AlignFlags& text_align = DEFAULT_TEXT_ALIGN) noexcept;

    /**
     * @param[in] text The text to display.
     * @param[in] rect Rectangle for the widget.
     * @param[in] text_align Alignment for the text.
     */
    Button(const std::string& text, const Rect& rect,
           const AlignFlags& text_align = DEFAULT_TEXT_ALIGN) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     */
    explicit Button(Frame& parent, const std::string& text = {},
                    const AlignFlags& text_align = DEFAULT_TEXT_ALIGN) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] text The text to display.
     * @param[in] rect Rectangle for the widget.
     * @param[in] text_align Alignment for the text.
     */
    Button(Frame& parent,
           const std::string& text,
           const Rect& rect,
           const AlignFlags& text_align = DEFAULT_TEXT_ALIGN) noexcept;

    virtual void handle(Event& event) override;

    using TextWidget::text;

    virtual void text(const std::string& text) override;

    virtual void draw(Painter& painter, const Rect& rect) override;

    /**
     * Add an event handler to be called when the widget receives an
     * EventId::pointer_click event.
     *
     * @param handler The callback to invoke on event.
     * @return A handle used to identify the registration.  This can then be
     *         passed to remove_handler().
     *
     * @see detail::Object::on_event()
     */
    virtual RegisterHandle on_click(const EventCallback& handler)
    {
        return on_event(handler, {EventId::pointer_click});
    }

    /**
     * Default draw method for Button.
     */
    static void default_draw(Button& widget, Painter& painter, const Rect& rect);

    using Widget::checked;

    virtual void checked(bool value) override;

    using TextWidget::min_size_hint;

    virtual Size min_size_hint() const override;

    virtual ~Button();

protected:

    virtual void parent(Frame* parent) override;

private:

    /**
     * Pointer to the group this button is a part of.
     * @todo This should be std::shared_ptr<ButtonGroup>
     */
    ButtonGroup* m_group{nullptr};

    friend ButtonGroup;
};

/**
 * Button that also contains an Image.
 *
 * The interesting thing about this Widget is the position of the text
 * relative to the image.  Alignment of text works as usual, however, the image
 * is aligned relative to the position of the text.  If there is no text, the
 * image is aligned relative to the Widget::box() as the text would be.
 *
 * @ingroup controls
 */
class EGT_API ImageButton : public Button
{
public:

    /**
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     */
    explicit ImageButton(const std::string& text = {},
                         const AlignFlags& text_align = DEFAULT_TEXT_ALIGN) noexcept;

    /**
     * @param[in] image The image to display.
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     */
    explicit ImageButton(const Image& image,
                         const std::string& text = {},
                         const AlignFlags& text_align = DEFAULT_TEXT_ALIGN) noexcept;

    /**
     * @param[in] image The image to display.
     * @param[in] text The text to display.
     * @param[in] rect Rectangle for the widget.
     * @param[in] text_align Alignment for the text.
     */
    explicit ImageButton(const Image& image,
                         const std::string& text,
                         const Rect& rect,
                         const AlignFlags& text_align = DEFAULT_TEXT_ALIGN) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] image The image to display.
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     */
    explicit ImageButton(Frame& parent,
                         const Image& image = {},
                         const std::string& text = {},
                         const AlignFlags& text_align = DEFAULT_TEXT_ALIGN) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] image The image to display.
     * @param[in] text The text to display.
     * @param[in] rect Rectangle for the widget.
     * @param[in] text_align Alignment for the text.
     */
    ImageButton(Frame& parent,
                const Image& image,
                const std::string& text,
                const Rect& rect,
                const AlignFlags& text_align = DEFAULT_TEXT_ALIGN) noexcept;

    virtual void draw(Painter& painter, const Rect& rect) override;

    static void default_draw(ImageButton& widget, Painter& painter, const Rect& rect);

    using Button::min_size_hint;

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
     * @param[in] hscale Horizontal scale [0.0 - 1.0], with 1.0 being 100%.
     * @param[in] vscale Vertical scale [0.0 - 1.0], with 1.0 being 100%.
     * @param[in] approximate Approximate the scale to increase image cache
     *            hit efficiency.
     *
     * This scales relative to the original size of the image.  Not the result
     * of any subsequent Image::resize() or Image::scale() call.
     *
     * @see Image::scale().
     *
     * @warning This does not damage the widget.
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
    void image_align(const AlignFlags& align)
    {
        if (detail::change_if_diff<>(m_image_align, align))
            damage();
    }

    /**
     * Get the image alignment.
     */
    inline AlignFlags image_align() const { return m_image_align; }

    /**
     * Enable/disable showing the label text.
     *
     * @param[in] value When true, the label text is shown.
     */
    virtual void show_label(bool value);

    /**
     * Get the show label state.
     */
    inline bool show_label() const { return m_show_label; }

    virtual ~ImageButton() = default;

protected:

    void do_set_image(const Image& image);

    /**
     * The image.
     * Allowed to be empty.
     */
    Image m_image;

    /**
     * When true, the label text is shown.
     */
    bool m_show_label{true};

    /**
     * Alignment of the image relative to the text.
     */
    AlignFlags m_image_align{AlignFlag::left};
};

/**
 * Same as a normal Button, except it manipulates its checked state like a
 * RadioBox or CheckBox.
 */
class CheckButton : public Button
{
    using Button::Button;

    virtual void handle(Event& event) override
    {
        Button::handle(event);

        switch (event.id())
        {
        case EventId::pointer_click:
            checked(!checked());
        default:
            break;
        }
    }
};

/**
 * Experimental namespace.
 *
 * This namespace contains unstable, broken, or otherwise incomplete things.
 */
namespace experimental
{

}

}
}

#endif
