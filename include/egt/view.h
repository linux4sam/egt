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
 * A scrollable view.
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

    /**
     * Scrollbar policy.
     */
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
                          Policy vertical_policy = Policy::as_needed) noexcept;

    /**
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] horizontal_policy Horizontal slider policy.
     * @param[in] vertical_policy Vertical slider policy.
     */
    explicit ScrolledView(const Rect& rect,
                          Policy horizontal_policy = Policy::as_needed,
                          Policy vertical_policy = Policy::as_needed) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] horizontal_policy Horizontal slider policy.
     * @param[in] vertical_policy Vertical slider policy.
     */
    explicit ScrolledView(Frame& parent, const Rect& rect,
                          Policy horizontal_policy = Policy::as_needed,
                          Policy vertical_policy = Policy::as_needed) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] horizontal_policy Horizontal slider policy.
     * @param[in] vertical_policy Vertical slider policy.
     */
    explicit ScrolledView(Frame& parent,
                          Policy horizontal_policy = Policy::as_needed,
                          Policy vertical_policy = Policy::as_needed) noexcept;

    /**
     * @param[in] props list of widget argument and its properties.
     */
    explicit ScrolledView(Serializer::Properties& props) noexcept
        : ScrolledView(props, false)
    {
    }

protected:

    explicit ScrolledView(Serializer::Properties& props, bool is_derived) noexcept;

public:

    void handle(Event& event) override;

    void draw(Painter& painter, const Rect& rect) override;

    void resize(const Size& size) override;

    void layout() override;

    using Frame::damage;

    void damage(const Rect& /*rect*/) override
    {
        Frame::damage(box());
    }

    void damage_from_child(const Rect& rect) override
    {
        damage(rect + m_offset);
    }

    /**
     * Get the current offset.
     *
     * @note The offset moves in the negative direction from zero.
     */
    EGT_NODISCARD Point offset() const { return m_offset; }

    /**
     * Set the position.
     *
     * @note The offset moves in the negative direction from zero.
     */
    void offset(Point offset);

    /**
     * Get the maximum offset currently possible.
     *
     * @note The offset moves in the negative direction from zero.
     */
    EGT_NODISCARD Point offset_max() const;

    /**
     * Set the horizontal offset.
     *
     * @note The offset moves in the negative direction from zero.
     */
    void hoffset(int offset)
    {
        this->offset(Point(offset, m_offset.y()));
    }

    /**
     * Set the vertical offset.
     *
     * @note The offset moves in the negative direction from zero.
     */
    void voffset(int offset)
    {
        this->offset(Point(m_offset.x(), offset));
    }

    /**
     * Get the slider dimension.
     */
    EGT_NODISCARD DefaultDim slider_dim() const { return m_slider_dim; }

    /**
     * Set the slider dimension.
     */
    void slider_dim(DefaultDim dim)
    {
        if (detail::change_if_diff<>(m_slider_dim, dim))
            damage();
    }

protected:

    /// Horizontal scrollable
    EGT_NODISCARD bool hscrollable() const
    {
        return m_hscrollable;
    }

    /// Vertical scrollable
    EGT_NODISCARD bool vscrollable() const
    {
        return m_vscrollable;
    }

    /// Update scrollable settings based on current size
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

    /// Update properties of the sliders.
    void update_sliders();

    /// Return the super rectangle that includes all of the child widgets.
    EGT_NODISCARD Rect super_rect() const;

    /// Resize the slider whenever the size of this changes.
    void resize_slider();

    /// Horizontal scrollable
    bool m_hscrollable{false};

    /// Vertical scrollable
    bool m_vscrollable{false};

    /// Current offset of the view.
    Point m_offset;

    /// Horizontal slider shown when scrollable.
    Slider m_hslider;

    /// Vertical slider shown when scrollable.
    Slider m_vslider;

    /// Starting offset for the drag.
    Point m_start_offset;

    /// Horizontal scrollbar policy
    Policy m_horizontal_policy{Policy::as_needed};

    /// Vertical scrollbar policy
    Policy m_vertical_policy{Policy::as_needed};

    /// @private
    std::unique_ptr<Canvas> m_canvas;

    /// Width/height of the slider when shown.
    DefaultDim m_slider_dim{8};
};

}
}

#endif
