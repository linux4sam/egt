/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_APP_H
#define EGT_APP_H

#include <clocale>
#include <egt/asio.hpp>
#include <egt/eventloop.h>
#include <egt/utils.h>
#include <iosfwd>
#include <libintl.h>
#include <string>

/**
 * @file
 * @brief Working with app.
 */

/**
 * EGT framework namespace.
 */
namespace egt
{
/**
 * EGT API version 1 namespace.
 */
inline namespace v1
{

/**
 * Application definition.
 *
 * This is basically just a helper class that does standard setup for
 * inputs, outputs, the event loop, and more. It also acts as sort of a
 * global access to get to these things because of this.
 */
class Application : public detail::noncopyable
{
public:
    /**
     * @param[in] argc Application argument count.
     * @param[in] argv Application argument array.
     * @param[in] primary Is this the primary display application.
     * @param[in] name Application name.  This is used for several things,
     *             including identifying the text domain used by gettext().
     */
    explicit Application(int argc = 0, const char** argv = nullptr,
                         const std::string& name = "egt", bool primary = true);

    /**
     * Reference to the main Application instance.
     */
    static Application& instance();

    /**
     * Run the application.
     *
     * This will initialize the application and start running the event loop.
     * This function will block until the event loop is told to exit, possibly
     * with a call to EventLoop::quit().
     */
    virtual int run();

    /**
     * Get a reference to the internal EventLoop of the application.
     */
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

    std::vector<std::pair<std::string, std::string>> get_input_devices();

    virtual ~Application() = default;

protected:

    EventLoop m_event;
    int m_argc{0};
    const char** m_argv{nullptr};
    asio::signal_set m_signals;
    std::vector<std::pair<std::string, std::string>> m_input_devices;

private:

    /**
     * Signal handler to handle some default signals to the application.
     */
    void signal_handler(const asio::error_code& error, int signum);
};

/**
 * This is a wrapper around gettext.
 */
#define _(String) gettext(String)

}
}

#endif
