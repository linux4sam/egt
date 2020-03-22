/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_FORM_H
#define EGT_FORM_H

/**
 * @file
 * @brief Working with forms.
 */

#include <egt/detail/meta.h>
#include <egt/frame.h>
#include <egt/sizer.h>
#include <memory>
#include <string>
#include <vector>

namespace egt
{
inline namespace v1
{

namespace experimental
{

/**
 * Utility class to help with standard Form layout of widgets.
 */
class EGT_API Form : public Frame
{
public:

    /**
     * @param[in] title Optional title of the form.
     */
    explicit Form(const std::string& title = {}) noexcept;

    /**
     * @param[in] parent Parent Frame of the Widget.
     * @param[in] title Optional title of the form.
     */
    explicit Form(Frame& parent, const std::string& title = {}) noexcept;

    /**
     * Add a group to the Form.
     */
    virtual void add_group(const std::string& caption);

    /**
     * Add an option to the form with a name.
     */
    template<class T>
    void add_option(const std::string& name, const std::shared_ptr<T>& widget)
    {
        auto p = std::dynamic_pointer_cast<Widget>(widget);
        add_option(name, p);
    }

    /**
     * Add an option to the form with a name.
     */
    virtual void add_option(const std::string& name, const std::shared_ptr<Widget>& widget);

    /**
     * Add an option to the Form without a name.
     */
    template<class T>
    void add_option(const std::shared_ptr<T>& widget)
    {
        auto p = std::dynamic_pointer_cast<Widget>(widget);
        add_option(p);
    }

    /**
     * Add an option to the Form without a name.
     */
    virtual void add_option(const std::shared_ptr<Widget>& widget);

    /**
     * Remove all options from the Form.
     */
    virtual void remove_options();

    /**
     * Set default option name text alignment.
     */
    virtual void name_align(const AlignFlags& align)
    {
        if (detail::change_if_diff<>(m_name_align, align))
            parent_layout();
    }

    /**
     * Set group name text alignment.
     */
    virtual void group_align(const AlignFlags& align)
    {
        if (detail::change_if_diff<>(m_group_align, align))
            parent_layout();
    }

    ~Form() override = default;

protected:

    /**
     * Minimum option height.
     */
    virtual DefaultDim min_option_height() const
    {
        return 50;
    }

    /// Internal BoxSizer used for layout.
    BoxSizer m_vsizer;

    /**
     * Array of options.
     *
     * @todo This will keep pointers around to deleted child widgets.
     */
    std::vector<Widget*> m_options;

    /// Option name text alignment.
    AlignFlags m_name_align{AlignFlag::center | AlignFlag::left};

    /// Group name text alignment.
    AlignFlags m_group_align{AlignFlag::bottom | AlignFlag::left};
};

}
}
}

#endif
