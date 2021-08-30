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
#include <egt/geometry.h>
#include <egt/image.h>
#include <egt/imageholder.h>
#include <egt/textwidget.h>
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
class EGT_API Button : public TextWidget
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
    explicit Button(const std::string& text = {},
                    const AlignFlags& text_align = default_text_align()) noexcept;

    /**
     * @param[in] text The text to display.
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] text_align Alignment for the text.
     */
    Button(const std::string& text, const Rect& rect,
           const AlignFlags& text_align = default_text_align()) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     */
    explicit Button(Frame& parent, const std::string& text = {},
                    const AlignFlags& text_align = default_text_align()) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] text The text to display.
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] text_align Alignment for the text.
     */
    Button(Frame& parent,
           const std::string& text,
           const Rect& rect,
           const AlignFlags& text_align = default_text_align()) noexcept;
    /**
     * @param[in] props list of widget argument and its properties.
     */
    explicit Button(Serializer::Properties& props) noexcept;

    Button(const Button&) = delete;
    Button& operator=(const Button&) = delete;
    Button(Button&&) noexcept = default;
    Button& operator=(Button&&) noexcept = default;

    void handle(Event& event) override;

    using TextWidget::text;

    void text(const std::string& text) override;

    void draw(Painter& painter, const Rect& rect) override;

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
    RegisterHandle on_click(const EventCallback& handler)
    {
        return on_event(handler, {EventId::pointer_click});
    }

    /// Default draw method for the widget.
    static void default_draw(const Button& widget, Painter& painter, const Rect& rect);

    using Widget::checked;

    void checked(bool value) override;

    /// Default button size.
    static Size default_size();
    /// Change default button size.
    static void default_size(const Size& size);

    using TextWidget::min_size_hint;

    EGT_NODISCARD Size min_size_hint() const override;

    ~Button() noexcept override;

protected:

    void set_parent(Frame* parent) override;

private:

    static Size default_button_size_value;

    static AlignFlags default_text_align_value;

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
 * Layout rules:
 * - When text is displayed, the auto scale feature has no effect on the layout.
 * - Unless you disabled the auto resize feature, the size of the button will be
 *   updated to the original size of the image plus extra space for the text.
 * - If you specify a size for the button, and there is no text, the image will
 *   be scaled up or down to fit the button size. You can disable this feature
 *   and you can choose if you want to keep the original ratio of the image
 *   during this operation.
 *
 * @ingroup controls
 */
class EGT_API ImageButton : public Button, public ImageHolder<ImageButton, Button>
{
public:

    /**
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     */
    explicit ImageButton(const std::string& text = {},
                         const AlignFlags& text_align = default_text_align()) noexcept;

    /**
     * @param[in] image The image to display.
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     */
    explicit ImageButton(const Image& image,
                         const std::string& text = {},
                         const AlignFlags& text_align = default_text_align()) noexcept;

    /**
     * @param[in] image The image to display.
     * @param[in] text The text to display.
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] text_align Alignment for the text.
     */
    explicit ImageButton(const Image& image,
                         const std::string& text,
                         const Rect& rect,
                         const AlignFlags& text_align = default_text_align()) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] image The image to display.
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     */
    explicit ImageButton(Frame& parent,
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
    ImageButton(Frame& parent,
                const Image& image,
                const std::string& text,
                const Rect& rect,
                const AlignFlags& text_align = default_text_align()) noexcept;

    /**
     * @param[in] props list of arguments and its properties.
     */
    explicit ImageButton(Serializer::Properties& props) noexcept;

    void draw(Painter& painter, const Rect& rect) override;

    /// Default draw method for the widget.
    static void default_draw(ImageButton& widget, Painter& painter, const Rect& rect);

    using Button::min_size_hint;

    EGT_NODISCARD Size min_size_hint() const override;

    void serialize(Serializer& serializer) const override;

private:

    void deserialize(Serializer::Properties& props) override;
};

/**
 * Same as a normal Button, except it manipulates its checked state like a
 * RadioBox or CheckBox.
 */
class CheckButton : public Button
{
    using Button::Button;

    void handle(Event& event) override
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
