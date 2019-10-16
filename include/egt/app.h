/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_APP_H
#define EGT_APP_H

#include <clocale>
#include <egt/asio.hpp>
#include <egt/detail/object.h>
#include <egt/eventloop.h>
#include <egt/utils.h>
#include <iosfwd>
#include <libintl.h>
#include <memory>
#include <string>
#include <vector>

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

class Screen;
class Input;

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
     * Calls EventLoop::quit() by default.
     */
    virtual void quit();

    /**
     * Get a reference to the application event loop instance.
     */
    inline EventLoop& event() { return m_event; }

    inline Screen* screen() const { return m_screen.get(); }

    /**
     * Paint the entire screen to a file.
     */
    void paint_to_file(const std::string& filename = {});

    /**
     * Dump the widget hierarchy and properties to the specified ostream.
     *
     * Example:
     * @code{.cpp}
     * app.dump(cout);
     * @endcode
     */
    void dump(std::ostream& out = std::cout);

    /**
     * Get a list of input devices configured with the EGT_INPUT_DEVICES
     * environment variable.
     */
    std::vector<std::pair<std::string, std::string>> get_input_devices();

    virtual ~Application();

protected:

    /**
     * The event loop instance.
     */
    EventLoop m_event;

    /**
     * Argument count.
     */
    int m_argc{0};

    /**
     * Argument list.
     */
    const char** m_argv{nullptr};

    /**
     * Signal instance for registered signal handles by the Application.
     */
    asio::signal_set m_signals;

    /**
     * List of configured input devices.
     */
    std::vector<std::pair<std::string, std::string>> m_input_devices;

private:

    /**
     * Signal handler to handle some default signals to the application.
     */
    void signal_handler(const asio::error_code& error, int signum);

    /**
     * The screen instance.
     */
    std::unique_ptr<Screen> m_screen;

    /**
     * Array of inputs.
     */
    std::vector<std::unique_ptr<Input>> m_inputs;

    detail::Object::handle_t m_handle{0};
};

/**
 * This is a wrapper around gettext().
 */
#define _(String) gettext(String)

}
}

#endif
