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
 * Utility class to help with standard form layout of widgets.
 */
class Form : public Frame
{
public:

    explicit Form(const std::string& title = {}) noexcept;

    explicit Form(Frame& parent, const std::string& title = {}) noexcept;

    virtual void add_group(const std::string& caption);

    template<class T>
    void add_option(const std::string& name, const std::shared_ptr<T>& widget)
    {
        auto p = std::dynamic_pointer_cast<Widget>(widget);
        add_option(name, p);
    }

    virtual void add_option(const std::string& name, const std::shared_ptr<Widget>& widget);

    virtual void add_option(const std::shared_ptr<Widget>& widget);

    virtual void remove_options();

    virtual ~Form() = default;

protected:

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
};

}
}
}

#endif
