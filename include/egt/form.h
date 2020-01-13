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

    explicit Form(const std::string& title = {}) noexcept;

    explicit Form(Frame& parent, const std::string& title = {}) noexcept;

    /**
     * Add a group to the Form.
     */
    virtual void add_group(const std::string& caption);

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
     * Set option name text alignment.
     */
    inline void set_name_align(AlignFlags align)
    {
        m_name_align = align;
    }

    /**
     * Set group name text alignment.
     */
    inline void set_group_align(AlignFlags align)
    {
        m_group_align = align;
    }

    virtual ~Form() = default;

protected:

    inline default_dim_type min_option_height() const
    {
        return 50;
    }

    /**
     * Internal BoxSizer used for layout.
     */
    BoxSizer m_vsizer;

    /**
     * Array of options.
     *
     * @todo This will keep pointers around to deleted child widgets.
     */
    std::vector<Widget*> m_options;

    /**
     * Option name text alignment.
     */
    AlignFlags m_name_align{AlignFlag::center | AlignFlag::left};

    /**
     * Group name text alignment.
     */
    AlignFlags m_group_align{AlignFlag::bottom | AlignFlag::left};
};

}
}
}

#endif
