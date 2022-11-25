/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_NOTEBOOK_H
#define EGT_NOTEBOOK_H

/**
 * @file
 * @brief Working with notebooks.
 */

#include <egt/detail/meta.h>
#include <egt/frame.h>
#include <egt/signal.h>
#include <memory>
#include <string>
#include <vector>

namespace egt
{
inline namespace v1
{

class Notebook;
/**
 * A single layer of a Notebook.
 */
class EGT_API NotebookTab : public Frame
{
public:

    NotebookTab()
    {
        name("NotebookTab" + std::to_string(m_widgetid));

        // tabs are not transparent by default
        fill_flags(Theme::FillFlag::solid);
    }

    explicit NotebookTab(Serializer::Properties& props) noexcept
        : NotebookTab(props, false)
    {
    }

    using Frame::add;
    void add(const std::shared_ptr<Widget>& widget) override
    {
        if (!widget)
            return;

        if (widget.get() == this)
            throw std::runtime_error("cannot add a widget to itself");

        assert(!widget->parent() && "widget already has parent!");

        auto cell = std::dynamic_pointer_cast<NotebookTab>(widget);
        if (cell)
            throw std::invalid_argument("cannot have nested NotebookTab");

        Frame::add(widget);
    }

protected:

    explicit NotebookTab(Serializer::Properties& props, bool is_derived) noexcept
        : Frame(props, true)
    {
        if (!is_derived)
            deserialize_leaf(props);
    }

public:

    /**
     * @return true if allowed to leave, false otherwise.
     */
    virtual bool leave()
    {
        return true;
    }

    /**
     * Called when this tab is entered and before show() is invoked.
     */
    virtual void enter()
    {
    }

    /**
     * Select this tab.
     */
    void select();
};

/**
 * Allows a collection of NotebookTab widgets to be shown one at a time.
 *
 * @ingroup controls
 */
class EGT_API Notebook : public Frame
{
public:

    /**
     * Event signal.
     * @{
     */
    /**
     * Invoked when a notebook tab changes.
     */
    Signal<> on_selected_changed;
    /** @} */

    /**
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit Notebook(const Rect& rect = {}) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit Notebook(Frame& parent, const Rect& rect = {}) noexcept;

    /**
     * @param[in] props list of widget argument and its properties.
     */
    explicit Notebook(Serializer::Properties& props) noexcept
        : Notebook(props, false)
    {
    }

protected:

    explicit Notebook(Serializer::Properties& props, bool is_derived) noexcept;

public:

    using Frame::add;

    void add(const std::shared_ptr<Widget>& widget) override;

    void remove(Widget* widget) override;

    /**
     * Set the selected widget by index.
     */
    void selected(size_t index);

    /**
     * Set the selected widget by widget.
     */
    void selected(Widget* widget);

    /**
     * Get the currently selected index.
     */
    EGT_NODISCARD ssize_t selected() const { return m_selected; }

    /**
     * Get a widget at the specified index.
     *
     * @param index The index of the widget.
     */
    EGT_NODISCARD NotebookTab* get(size_t index) const;

    void serialize(Serializer& serializer) const override;

    /**
     * Resume deserializing of the widget after its children have been deserialized.
     */
    void post_deserialize(Serializer::Properties& props) override;

protected:

    /// Type of array of notebook tabs.
    using CellArray = std::vector<std::weak_ptr<NotebookTab>>;

    /// Array of notebook tabs.
    CellArray m_cells;

    /// Currently selected index.
    ssize_t m_selected{-1};
};

}
}

#endif
