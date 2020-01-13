/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_VIEW_H
#define EGT_VIEW_H

/**
 * @file
 * @brief View definition.
 */

#include <egt/canvas.h>
#include <egt/detail/meta.h>
#include <egt/frame.h>
#include <egt/slider.h>
#include <memory>

namespace egt
{
inline namespace v1
{

/**
 * A scroll-able view.
 *
 * A ScrolledView is a sort of kitchen window. You can have a large surface
 * area on the other side of the window, but you only see a small portion of
 * it through the window.  The surface can be scrolled, or panned, in a single
 * Orientation to see the rest.
 *
 * This is used internally by Widgets, but can also be used directly.
 */
class EGT_API ScrolledView : public Frame
{
public:

    enum class Policy
    {
        never,
        always,
        as_needed
    };

    /**
     * @param[in] horizontal_policy Horizontal slider policy.
     * @param[in] vertical_policy Vertical slider policy.
     */
    explicit ScrolledView(Policy horizontal_policy = Policy::as_needed,
                          Policy vertical_policy = Policy::as_needed);

    /**
     * @param[in] rect Rectangle for the widget.
     * @param[in] horizontal_policy Horizontal slider policy.
     * @param[in] vertical_policy Vertical slider policy.
     */
    explicit ScrolledView(const Rect& rect,
                          Policy horizontal_policy = Policy::as_needed,
                          Policy vertical_policy = Policy::as_needed);

    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Rectangle for the widget.
     * @param[in] horizontal_policy Horizontal slider policy.
     * @param[in] vertical_policy Vertical slider policy.
     */
    explicit ScrolledView(Frame& parent, const Rect& rect,
                          Policy horizontal_policy = Policy::as_needed,
                          Policy vertical_policy = Policy::as_needed);

    /**
     * @param[in] parent The parent Frame.
     * @param[in] horizontal_policy Horizontal slider policy.
     * @param[in] vertical_policy Vertical slider policy.
     */
    explicit ScrolledView(Frame& parent,
                          Policy horizontal_policy = Policy::as_needed,
                          Policy vertical_policy = Policy::as_needed);

    virtual void handle(Event& event) override;

    virtual void draw(Painter& painter, const Rect& rect) override;

    virtual void resize(const Size& size) override;

    virtual void layout() override;

    virtual void damage() override
    {
        damage(box());
    }

    virtual void damage(const Rect& /*rect*/) override
    {
        Frame::damage(box());
    }

    virtual void damage_from_child(const Rect& rect) override
    {
        damage(rect + m_offset);
    }

    /**
     * Get the current offset.
     *
     * @note The offset moves in the negative direction from zero.
     */
    inline Point offset() const { return m_offset; }

    /**
     * Set the position.
     */
    virtual void offset(Point offset);

    inline void hoffset(int offset)
    {
        this->offset(Point(offset, m_offset.y()));
    }

    inline void voffset(int offset)
    {
        this->offset(Point(m_offset.x(), offset));
    }

    /**
     * Get the slider dimension.
     */
    inline default_dim_type slider_dim() const { return m_slider_dim; }

    /**
     * Set the slider dimension.
     */
    inline void slider_dim(default_dim_type dim)
    {
        if (detail::change_if_diff<>(m_slider_dim, dim))
            damage();
    }

    virtual ~ScrolledView() noexcept = default;

protected:

    inline bool hscrollable() const
    {
        return m_hscrollable;
    }

    inline bool vscrollable() const
    {
        return m_vscrollable;
    }

    void update_scrollable()
    {
        auto super = super_rect();

        m_hscrollable = (m_horizontal_policy == Policy::always) ||
                        (m_horizontal_policy == Policy::as_needed && super.width() > content_area().width());
        m_vscrollable = (m_vertical_policy == Policy::always) ||
                        (m_vertical_policy == Policy::as_needed && super.height() > content_area().height());

        if (super.width() <= content_area().width())
            m_offset.x(0);

        if (super.height() <= content_area().height())
            m_offset.y(0);
    }

    void update_sliders();

    bool m_hscrollable{false};
    bool m_vscrollable{false};

    /**
     * Return the super rectangle that includes all of the child widgets.
     */
    Rect super_rect() const;

    /**
     * Resize the slider whenever the size of this changes.
     */
    void resize_slider();

    /**
     * Current offset of the view.
     */
    Point m_offset;

    /**
     * Slider shown when scroll-able.
     */
    Slider m_hslider;
    Slider m_vslider;

    /**
     * Starting offset for the drag.
     */
    Point m_start_offset;

    Policy m_horizontal_policy{Policy::as_needed};
    Policy m_vertical_policy{Policy::as_needed};

    std::shared_ptr<Canvas> m_canvas;

    /**
     * Width/height of the slider when shown.
     */
    default_dim_type m_slider_dim{8};

    bool m_update{false};
};

}
}

#endif
