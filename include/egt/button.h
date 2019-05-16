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
#include <egt/geometry.h>
#include <egt/image.h>
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
 * if that interaction is only visual like display a value.
 */

/**
 * Basic button control.
 *
 * This presents a clickable control that can be used to respond to user pointer
 * events.
 *
 * @ingroup controls
 *
 * @image html widget_button.png
 * @image latex widget_button.png "widget_button" width=5cm
 */
class Button : public TextWidget
{
public:

    /**
     * @param[in] text The text to display.
     */
    explicit Button(const std::string& text = std::string()) noexcept;

    /**
     * @param[in] text The text to display.
     * @param[in] rect Rectangle for the widget.
     */
    Button(const std::string& text, const Rect& rect) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] text The text to display.
     */
    explicit Button(Frame& parent, const std::string& text = std::string()) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] text The text to display.
     * @param[in] rect Rectangle for the widget.
     */
    Button(Frame& parent, const std::string& text, const Rect& rect) noexcept;

    virtual void handle(Event& event) override;

    virtual void draw(Painter& painter, const Rect& rect) override;

    /**
     * Default draw method for the Button.
     */
    static void default_draw(Button& widget, Painter& painter, const Rect& rect);

    /**
     * Return the boolean checked state of the a button.
     */
    virtual bool checked() const;

    /**
     * Set checked state of the button.
     */
    virtual void check(bool value);

    virtual Size min_size_hint() const override;

    virtual ~Button();

protected:

    virtual void set_parent(Frame* parent) override;

    /**
     * Perform a special first resize of the button.
     */
    virtual void first_resize();

private:

    /**
     * Is the button currently checked?
     */
    bool m_checked{false};

    /**
     * Pointer to the group this button is a part of.
     */
    ButtonGroup* m_group{nullptr};

    friend ButtonGroup;
};

/**
 * A button widget that can have an image and/or a label on it.
 *
 * @ingroup controls
 */
class ImageButton : public Button
{
public:
    explicit ImageButton(const Image& image,
                         const std::string& text = "",
                         const Rect& rect = Rect()) noexcept;

    ImageButton(Frame& parent,
                const Image& image,
                const std::string& text = "",
                const Rect& rect = Rect()) noexcept;

    virtual void draw(Painter& painter, const Rect& rect) override;

    static void default_draw(ImageButton& widget, Painter& painter, const Rect& rect);

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

    virtual void scale_image(double s, bool approximate = false)
    {
        scale_image(s, s, approximate);
    }

    inline const Image& image() const { return m_image; }

    inline Image& image() { return m_image; }

    void set_image_align(alignmask align)
    {
        if (detail::change_if_diff<>(m_image_align, align))
            damage();
    }

    inline alignmask image_align() const { return m_image_align; }

    void set_position_image_first(bool value)
    {
        if (detail::change_if_diff<>(m_position_image_first, value))
            damage();
    }

    virtual ~ImageButton() = default;

protected:

    virtual void first_resize() override;

    void do_set_image(const Image& image);

    Image m_image;
    alignmask m_image_align{alignmask::center | alignmask::left};
    bool m_position_image_first{false};
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
