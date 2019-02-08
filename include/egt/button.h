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
#include <egt/theme.h>
#include <egt/textwidget.h>

namespace egt
{
inline namespace v1
{

class Font;
class Painter;

/**
 * Basic button widget.
 *
 * @image html widget_button.png
 * @image latex widget_button.png "widget_button" width=5cm
 */
class Button : public TextWidget
{
public:

    Button(const std::string& text = std::string(),
           const Rect& rect = Rect(),
           const Font& font = Font(),
           widgetmask flags = widgetmask::NONE) noexcept;

    Button(Frame& parent,
           const std::string& text,
           const Rect& rect,
           const Font& font = Font(),
           widgetmask flags = widgetmask::NONE) noexcept;

    Button(Frame& parent,
           const std::string& text = std::string(),
           const Font& font = Font(),
           widgetmask flags = widgetmask::NONE) noexcept;

    virtual int handle(eventid event) override;

    virtual void draw(Painter& painter, const Rect& rect) override;

    static void default_draw(Button& widget, Painter& painter, const Rect& rect);

    /**
     * Return the boolean checked state of the a button.
     */
    virtual bool checked() const;

    /**
     * Set checked state of the button.
     */
    virtual void check(bool value);

    virtual ~Button();

protected:

    virtual void set_parent(Frame* parent) override;

    virtual void first_resize();

private:

    bool m_checked{false};

    ButtonGroup* m_group{nullptr};

    friend ButtonGroup;
};

/**
 * A button widget that can have an image and/or a label on it.
 */
class ImageButton : public Button
{
public:
    ImageButton(const Image& image,
                const std::string& text = "",
                const Rect& rect = Rect(),
                widgetmask flags = widgetmask::NONE) noexcept;

    ImageButton(Frame& parent,
                const Image& image,
                const std::string& text = "",
                const Rect& rect = Rect(),
                widgetmask flags = widgetmask::NONE) noexcept;

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

    const Image& image() const { return m_image; }

    Image& image() { return m_image; }

    void set_image_align(alignmask align)
    {
        if (m_image_align != align)
        {
            m_image_align = align;
            damage();
        }
    }

    alignmask image_align() const { return m_image_align; }

    void set_position_image_first(bool value)
    {
        if (m_position_image_first != value)
        {
            m_position_image_first = value;
            damage();
        }
    }

    virtual ~ImageButton();

protected:

    virtual void first_resize() override;

    void do_set_image(const Image& image);

    Image m_image;
    alignmask m_image_align{alignmask::CENTER | alignmask::LEFT};
    bool m_position_image_first{false};
};

namespace experimental
{

/**
 * A special widget that has no content but receives events.
 *
 * This is an invisible widget that can be used to handle events, like
 * input events.
 */
class HotSpot : public Button
{
public:

    HotSpot(const Rect& rect = Rect(),
            widgetmask flags = widgetmask::NO_BACKGROUND) noexcept
        : Button("", rect, Font(), flags)
    {
        hide();
    }

    virtual void damage() override
    {}

    virtual void draw(Painter& painter, const Rect& rect) override
    {
        ignoreparam(painter);
        ignoreparam(rect);
    }

    virtual ~HotSpot() {}

private:
    virtual void show() override {}
};

}

}
}

#endif
