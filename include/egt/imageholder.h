/*
 * Copyright (C) 2021 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_IMAGE_HOLDER_H
#define EGT_IMAGE_HOLDER_H

/**
 * @file
 * @brief Helper class for widgets holding an image.
 */

#include "egt/detail/alignment.h"
#include <egt/detail/meta.h>
#include <egt/geometry.h>
#include <egt/image.h>
#include <egt/painter.h>
#include "egt/serialize.h"
#include <egt/text.h>
#include <egt/widget.h>

namespace egt
{
inline namespace v1
{

template<class T, class U>
class ImageHolder
{
public:
    ImageHolder(const ImageHolder& rhs)
        : m_widget(*static_cast<T*>(this)),
          m_image(rhs.m_image),
          m_auto_scale_image(rhs.m_auto_scale_image),
          m_keep_image_ratio(rhs.m_keep_image_ratio),
          m_show_label(rhs.m_show_label),
          m_image_align(rhs.m_image_align)
    {}

    ImageHolder(ImageHolder&& rhs)
        : m_widget(*static_cast<T*>(this)),
          m_image(std::move(rhs.m_image)),
          m_auto_scale_image(rhs.m_auto_scale_image),
          m_keep_image_ratio(rhs.m_keep_image_ratio),
          m_show_label(rhs.m_show_label),
          m_image_align(std::move(rhs.m_image_align))
    {}

    ImageHolder& operator=(const ImageHolder& rhs)
    {
        if (this != &rhs)
        {
            // Keep m_widget unchanged
            m_image = rhs.m_image;
            m_auto_scale_image = rhs.m_auto_scale_image;
            m_keep_image_ratio = rhs.m_keep_image_ratio;
            m_show_label = rhs.m_show_label;
            m_image_align = rhs.m_image_align;
        }
        return *this;
    }

    ImageHolder& operator=(ImageHolder&& rhs)
    {
        if (this != &rhs)
        {
            // Keep m_widget unchanged
            m_image = std::move(rhs.m_image);
            m_auto_scale_image = rhs.m_auto_scale_image;
            m_keep_image_ratio = rhs.m_keep_image_ratio;
            m_show_label = rhs.m_show_label;
            m_image_align = std::move(rhs.m_image_align);
        }
        return *this;
    }

    virtual ~ImageHolder() = default;

    /**
     * Set a new Image.
     *
     * @param[in] image The new image to use.
     */
    void image(const Image& image)
    {
        do_set_image(image);
    }

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
    void scale_image(float hscale, float vscale,
                     bool approximate = false)
    {
        m_image.scale(hscale, vscale, approximate);
        m_widget.parent_layout();
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
     * button box. By default, set to true.
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
            m_widget.damage();
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
    void show_label(bool value)
    {
        if (detail::change_if_diff<>(m_show_label, value))
            m_widget.damage();
    }

    /**
     * Get the show label state.
     */
    EGT_NODISCARD bool show_label() const { return m_show_label; }

    void default_draw(Painter& painter,
                      const Rect& rect,
                      Palette::ColorId id_bg,
                      Palette::ColorId id_border,
                      Palette::ColorId id_text)
    {
        T& widget = static_cast<T&>(m_widget);

        detail::ignoreparam(rect);

        widget.draw_box(painter, id_bg, id_border);

        if (!widget.text().empty())
        {
            std::string text;
            if (show_label())
                text = widget.text();

            if (!image().empty())
            {
                detail::draw_text(painter,
                                  widget.content_area(),
                                  text,
                                  widget.font(),
                                  TextBox::TextFlags({TextBox::TextFlag::multiline, TextBox::TextFlag::word_wrap}),
                                  widget.text_align(),
                                  Justification::middle,
                                  widget.color(id_text),
                                  image_align(),
                                  image());
            }
            else
            {
                detail::draw_text(painter,
                                  widget.content_area(),
                                  text,
                                  widget.font(),
                                  TextBox::TextFlags({TextBox::TextFlag::multiline, TextBox::TextFlag::word_wrap}),
                                  widget.text_align(),
                                  Justification::middle,
                                  widget.color(id_text));
            }
        }
        else if (!image().empty())
        {
            auto max_image_size = image().size();

            if (auto_scale_image())
            {
                /*
                 * If auto scale is enabled, we need to give the expected size of
                 * the image to the align algorithm.
                 */
                max_image_size.width(widget.content_area().width());
                max_image_size.height(widget.content_area().height());
            }

            auto target = detail::align_algorithm(max_image_size,
                                                  widget.content_area(),
                                                  image_align());

            /*
             * Better to do it with target rect. Depending on the align flags, the
             * target size can be different from the max_image_size.
             */
            if (auto_scale_image())
            {
                const auto hs = static_cast<float>(target.width()) /
                                static_cast<float>(image().size_orig().width());
                const auto vs = static_cast<float>(target.height()) /
                                static_cast<float>(image().size_orig().height());

                // This check avoid rounding issues.
                if (image().size() != target.size())
                {
                    if (keep_image_ratio())
                    {
                        if (hs < vs)
                            image().scale(hs);
                        else
                            image().scale(vs);
                        /*
                         * Need to update the alignment as the image size is
                         * probably no longer the max size due to the scaling with
                         * ratio preservation.
                         */
                        target = detail::align_algorithm(image().size(),
                                                         widget.content_area(),
                                                         image_align());
                    }
                    else
                    {
                        image().scale(hs, vs);
                    }
                }
            }

            painter.draw(target.point());
            painter.draw(image());
        }
    }

protected:
    explicit ImageHolder(Widget& widget) noexcept
        : m_widget(widget)
    {}

    /// @private
    void do_set_image(const Image& image)
    {
        if (m_widget.size().empty() && !image.empty())
            m_widget.resize(image.size() + Size(m_widget.moat() * 2, m_widget.moat() * 2));

        m_image = image;
        m_widget.damage();
    }

    Size min_size_hint() const
    {
        if (!m_widget.m_min_size.empty())
            return m_widget.m_min_size;

        Rect size = static_cast<U&>(m_widget).U::min_size_hint() -
                    Size(m_widget.moat() * 2, m_widget.moat() * 2);

        if (!m_image.size().empty())
        {
            if (image_align().is_set(AlignFlag::left) ||
                image_align().is_set(AlignFlag::right))
            {
                size += Size(m_image.width(), 0);
            }
            else if (image_align().is_set(AlignFlag::top) ||
                     image_align().is_set(AlignFlag::bottom))
            {
                size += Size(0, m_image.height());
            }

            size = Rect::merge(size, m_image.size());
        }

        auto res = size.size() + Size(m_widget.moat() * 2, m_widget.moat() * 2);
        return res;
    }

    void serialize(Serializer& serializer) const
    {
        static_cast<U&>(m_widget).U::serialize(serializer);

        serializer.add_property("showlabel", show_label());
        if (!m_image.empty())
            m_image.serialize("image", serializer);
        if (!image_align().empty())
            serializer.add_property("image_align", image_align());
    }

    void deserialize(Serializer::Properties& props)
    {
        // TODO proper loading of all image properties
        props.erase(std::remove_if(props.begin(), props.end(), [&](auto & p)
        {
            switch (detail::hash(std::get<0>(p)))
            {
            case detail::hash("showlabel"):
                show_label(egt::detail::from_string(std::get<1>(p)));
                break;
            case detail::hash("image"):
                m_image.deserialize(std::get<0>(p), std::get<1>(p), std::get<2>(p));
                break;
            case detail::hash("image_align"):
                image_align(AlignFlags(std::get<1>(p)));
                break;
            default:
                return false;
            }
            return true;
        }), props.end());
    }

    ///
    Widget& m_widget;

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
};

}
}

#endif
