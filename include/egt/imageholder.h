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
#include <egt/frame.h>
#include <egt/geometry.h>
#include <egt/image.h>
#include <egt/painter.h>
#include "egt/serialize.h"
#include <egt/text.h>
#include <egt/textwidget.h>
#include <egt/widget.h>

namespace egt
{
inline namespace v1
{

template<class T,
         Palette::ColorId id_bg,
         Palette::ColorId id_border,
         Palette::ColorId id_text>
class EGT_API ImageHolder : public T
{
public:
    /**
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     */
    explicit ImageHolder(const std::string& text = {},
                         const AlignFlags& text_align = T::default_text_align()) noexcept
        : ImageHolder(Image(), text, text_align)
    {}

    /**
     * @param[in] image The image to display.
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     */
    explicit ImageHolder(const Image& image,
                         const std::string& text = {},
                         const AlignFlags& text_align = T::default_text_align()) noexcept
        : ImageHolder(image, text, {}, text_align)
    {}

    /**
     * @param[in] image The image to display.
     * @param[in] text The text to display.
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] text_align Alignment for the text.
     */
    explicit ImageHolder(const Image& image,
                         const std::string& text,
                         const Rect& rect,
                         const AlignFlags& text_align = T::default_text_align()) noexcept
        : T(text, rect, text_align)
    {
        this->name(type() + std::to_string(this->m_widgetid));

        if (text.empty())
        {
            show_label(false);
            image_align(AlignFlag::center | AlignFlag::expand);
        }
        do_set_image(image);
    }

    /**
     * @param[in] parent The parent Frame.
     * @param[in] image The image to display.
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     */
    explicit ImageHolder(Frame& parent,
                         const Image& image = {},
                         const std::string& text = {},
                         const AlignFlags& text_align = T::default_text_align()) noexcept
        : ImageHolder(image, text, text_align)
    {
        parent.add(*this);
    }

    /**
     * @param[in] parent The parent Frame.
     * @param[in] image The image to display.
     * @param[in] text The text to display.
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] text_align Alignment for the text.
     */
    ImageHolder(Frame& parent,
                const Image& image,
                const std::string& text,
                const Rect& rect,
                const AlignFlags& text_align = T::default_text_align()) noexcept
        : ImageHolder(image, text, rect, text_align)
    {
        parent.add(*this);
    }

    /**
     * @param[in] props list of arguments and its properties.
     */
    explicit ImageHolder(Serializer::Properties& props) noexcept
        : ImageHolder(props, false)
    {}

    ImageHolder(const ImageHolder&) noexcept = delete;
    ImageHolder& operator=(const ImageHolder&) noexcept = delete;
    ImageHolder(ImageHolder&&) noexcept = default;
    ImageHolder& operator=(ImageHolder&&) noexcept = default;

    virtual ~ImageHolder() = default;

    EGT_NODISCARD std::string type() const override
    {
        auto t = detail::demangle(typeid(T).name());
        return std::string("Image") + detail::replace_all(t, "egt::v1::", {});
    }

    /**
     * Get the URI of the current image.
     */
    EGT_NODISCARD std::string uri() const
    {
        return m_image.uri();
    }

    /**
     * Load a new Image from an uri.
     *
     * @param[in] uri The URI of the image to load.
     */
    void uri(const std::string& uri)
    {
        m_image.uri(uri);
        refresh();
    }

    /**
     * Reset the URI, therefore clear the current image, if any.
     */
    void reset_uri()
    {
        m_image.reset_uri();
        refresh();
    }

    /**
     * Get the horizontal scale value.
     */
    EGT_NODISCARD float hscale() const { return m_image.hscale(); }

    /**
     * Set the horizontal scale value.
     */
    void hscale(float hscale)
    {
        m_image.scale(hscale, m_image.vscale());
        refresh();
    }

    /**
     * Get the vertical scale value.
     */
    EGT_NODISCARD float vscale() const { return m_image.vscale(); }

    /**
     * Set the vertical scale value.
     */
    void vscale(float vscale)
    {
        m_image.scale(m_image.hscale(), vscale);
        refresh();
    }

    /**
     * Scale the image in both direction with the same ratio.
     */
    void image_scale(float scale)
    {
        m_image.scale(scale);
        refresh();
    }

    /**
     * Scale the image in both direction.
     */
    void image_scale(const SizeF& scale)
    {
        m_image.scale(scale.width(), scale.height());
        refresh();
    }

    /**
     * Get both scale values.
     */
    EGT_NODISCARD SizeF image_scale() const { return SizeF(hscale(), vscale()); }

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
        this->parent_layout();
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
        if (enable)
            m_image_align.set(AlignFlag::expand);
        else
            m_image_align.clear(AlignFlag::expand);
        refresh();
    }

    /**
     * Get the auto scale image state.
     */
    bool auto_scale_image() const
    {
        return m_image_align.is_set(AlignFlag::expand);
    }

    /**
     * Enable/disable ratio preservation while scaling the image.
     *
     * If enabled, the original image ratio won't be changed during the image
     * auto scale operation. By default, set to true.
     */
    void keep_image_ratio(bool enable)
    {
        image().keep_image_ratio(enable);
        refresh();
    }

    /**
     * Get the keep image ratio state.
     */
    bool keep_image_ratio() const
    {
        return image().keep_image_ratio();
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
            this->damage();
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
            refresh();
    }

    /**
     * Get the show label state.
     */
    EGT_NODISCARD bool show_label() const { return m_show_label; }

    void draw(Painter& painter, const Rect& rect) override
    {
        Drawer<ImageHolder>::draw(*this, painter, rect);
    }

    /// Default draw method for the widget.
    static void default_draw(ImageHolder& widget, Painter& painter, const Rect& rect)
    {
        detail::ignoreparam(rect);

        widget.draw_box(painter, id_bg, id_border);

        if (widget.show_label())
        {
            const std::string& text = widget.text();

            if (!widget.image().empty())
            {
                detail::draw_text(painter,
                                  widget.content_area(),
                                  text,
                                  widget.font(),
                                  TextBox::TextFlags({TextBox::TextFlag::multiline, TextBox::TextFlag::word_wrap}),
                                  widget.text_align(),
                                  Justification::middle,
                                  widget.color(id_text),
                                  widget.image_align(),
                                  widget.image());
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
        else if (!widget.image().empty())
        {
            auto target = widget.image().align(widget.content_area(), widget.image_align());
            painter.draw(widget.image(), target.point());
        }
    }

protected:
    explicit ImageHolder(Serializer::Properties& props, bool is_derived) noexcept
        : T(props, true)
    {
        deserialize(props);

        if (!is_derived)
            this->deserialize_leaf(props);
    }

    void refresh()
    {
        this->damage();
        this->layout();
    }

    /// @private
    void do_set_image(const Image& image)
    {
        if (this->size().empty() && !image.empty())
            this->resize(image.size() + Size(this->moat() * 2, this->moat() * 2));

        m_image = image;
        this->damage();
    }

public:
    using T::min_size_hint;

    EGT_NODISCARD Size min_size_hint() const override
    {
        if (!this->m_min_size.empty())
            return this->m_min_size;

        Size min_size_constraint;
        if (this->show_label() && !this->text().empty())
            min_size_constraint = T::min_size_hint();
        else
            min_size_constraint = Widget::min_size_hint();

        if (!show_label() && auto_scale_image())
            return min_size_constraint;

        Rect size = min_size_constraint - Size(this->moat() * 2, this->moat() * 2);

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

        auto res = size.size() + Size(this->moat() * 2, this->moat() * 2);
        return res;
    }

    void serialize(Serializer& serializer) const override
    {
        T::serialize(serializer);

        serializer.add_property("showlabel", show_label());
        if (!m_image.empty())
            m_image.serialize("image", serializer);
        if (!image_align().empty())
            serializer.add_property("image_align", image_align());
    }

protected:
    void deserialize(Serializer::Properties& props)
    {
        // TODO proper loading of all image properties
        m_image_align.clear();
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

    /// The image. Allowed to be empty.
    Image m_image;

    /// When true, the label text is shown.
    bool m_show_label{true};

    /// Alignment of the image relative to the text.
    AlignFlags m_image_align{AlignFlag::left | AlignFlag::expand};
};

}
}

#endif
