/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_APP_H
#define EGT_APP_H

#include <egt/eventloop.h>
#include <egt/utils.h>
#include <iosfwd>
#include <libintl.h>
#include <locale.h>
#include <string>

/**
 * @file
 * @brief Working with app.
 */

namespace egt
{
inline namespace v1
{

/**
 * Defines an application.
 *
 * This is basically just a helper class that does standard setup for
 * inputs, outputs, the event loop, and more. It also acts as sort of a
 * global accessor to get to these things because of this.
 */
class Application : public detail::noncopyable
{
public:
    /**
     * @param[in] argc Aplication argument count.
     * @param[in] argv Application argument array.
     * @param[in] primary Is this the primary display application.
     * @param[in] name Application name.  This is used for several things,
     *             including identifying the text domain used by gettext().
     */
    Application(int argc = 0, const char** argv = nullptr,
                const std::string& name = "egt", bool primary = true);

    virtual int run();

    inline EventLoop& event() { return m_event; }

    /**
     * Paint the entire screen to a file.
     */
    void paint_to_file(const std::string& filename = std::string());

    /**
     * Dump the widget hierarchy and properties to the specified ostream.
     *
     * Example:
     * @code
     * app.dump(cout);
     * @endcode
     */
    void dump(std::ostream& out);

    virtual ~Application();

protected:
    EventLoop m_event;
    int m_argc{0};
    const char** m_argv{nullptr};
};

/**
 * Reference to the main Application instance.
 */
Application& main_app();

/**
 * This is a wrapper around gettext.
 */
#define _(String) gettext(String)

}
}

#endif
