/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_APP_H
#define EGT_APP_H

#include <clocale>
#include <egt/asio.hpp>
#include <egt/detail/meta.h>
#include <egt/eventloop.h>
#include <egt/object.h>
#include <iosfwd>
#include <memory>
#include <ostream>
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
 * EGT API Version 1 inline namespace.
 */
inline namespace v1
{

class Screen;
class Input;
class Window;
class Timer;

/**
 * Application definition.
 *
 * This is basically just a helper class that does standard setup for
 * inputs, outputs, the event loop, and more. It also acts as sort of a
 * global access to get to these things because of this.
 */
class EGT_API Application
{
public:
    /**
     * @param[in] argc Application argument count.
     * @param[in] argv Application argument array.
     * @param[in] primary Is this the primary display application.
     * @param[in] name Application name.  This is used for several things,
     *             including identifying the text domain used by gettext().
     */
    explicit Application(int argc = 0, char** argv = nullptr,
                         const std::string& name = {}, bool primary = true);

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    /**
     * Reference to the main Application instance.
     *
     * @throws std::runtime_error If there is no application instance.
     */
    static Application& instance();

    /**
     * Check if there is an available instance.
     */
    static bool check_instance();

    /**
     * Reset the resources search paths optionally adding extra paths.
     *
     * @param[in] extra_paths An optional list of extra search paths.
     *
     * The current search paths list is cleared then filled with:
     * 1 - paths from the EGT_SEARCH_PATH env variable
     * 2 - paths from extra_paths
     * 3 - the current working directory
     * 4 - the application directory
     */
    static void setup_search_paths(const std::vector<std::string>& extra_paths = {});

    /**
     * Check whether the application runs under the Microchip Graphic Composer.
     */
    EGT_NODISCARD bool is_composer() const;

    /**
     * Run the application.
     *
     * This will initialize the application and start running the event loop.
     * This function will block until the event loop is told to exit by calling
     * quit().
     */
    virtual int run();

    /**
     * Calls EventLoop::quit() by default.
     * Allow non-zero exit value
     */
    virtual void quit(const int exit_value = 0);

    /**
     * Get a reference to the application event loop instance.
     */
    EventLoop& event() { return m_event; }

    /**
     * Get a pointer to the Screen instance.
     */
    EGT_NODISCARD Screen* screen() const { return m_screen.get(); }

    /**
     * Get a pointer to the main Window.
     */
    EGT_NODISCARD Window* main_window() const { return m_main_window; }

    /**
     * Get a pointer to the modal Window.
     *
     * The modal window is a single window that will receive all events. Only
     * one window can be modal at any given time.
     */
    EGT_NODISCARD Window* modal_window() const { return m_modal_window; }

    /// @private
    void set_modal_window(Window* window)
    {
        m_modal_window = window;
    }

    /**
     * Get the list of all currently allocated Windows.
     */
    EGT_NODISCARD const std::vector<Window*>& windows() const { return m_windows; }

    /**
     * Paint the entire Screen to a file.
     */
    void paint_to_file(const std::string& filename = {});

    /**
     * Dump the widget hierarchy and properties to the specified std::ostream.
     *
     * Example:
     * @code{.cpp}
     * app.dump(cout);
     * @endcode
     */
    void dump(std::ostream& out) const;

    /**
     * Dump all allocated timers to the specified std::ostream.
     *
     * Example:
     * @code{.cpp}
     * app.dump_timers(cout);
     * @endcode
     */
    void dump_timers(std::ostream& out) const;

    /**
     * Get a list of input devices configured with the EGT_INPUT_DEVICES
     * environment variable.
     */
    const std::vector<std::pair<std::string, std::string>>& get_input_devices();

    /**
     * Get the argc value passed in.
     */
    EGT_NODISCARD inline int argc() const { return m_argc; }

    /**
     * Get the argv value passed in.
     */
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    EGT_NODISCARD inline const char** argv() const { return const_cast<const char**>(m_argv); }

    virtual ~Application() noexcept;

protected:

    /// @private
    static void setup_info();
    /// @private
    static void setup_logging();
    /// @private
    static void setup_locale(const std::string& name);
    /// @private
    void setup_backend(bool primary);
    /// @private
    void setup_inputs();
    /// @private
    void setup_events();

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
    char** m_argv{nullptr};

    /**
     * Signal instance for registered signal handles by the Application.
     */
    asio::signal_set m_signals;

    /**
     * List of configured input devices.
     */
    std::vector<std::pair<std::string, std::string>> m_input_devices;

private:

    /// Signal handler to handle some default signals to the application.
    void signal_handler(const asio::error_code& error, int signum);

    /// The screen instance.
    std::unique_ptr<Screen> m_screen;

    /// Array of inputs.
    std::vector<std::unique_ptr<Input>> m_inputs;

    /// Internal registration handle
    Object::RegisterHandle m_handle{0};

    /// All allocated windpws.
    std::vector<Window*> m_windows;

    /// The main window.
    Window* m_main_window{nullptr};

    /// The modal window.
    Window* m_modal_window{nullptr};

    /// All allocated timers.
    std::vector<Timer*> m_timers;

    friend class Window;
    friend class Timer;
};

}
}

#endif
