/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_TIMER_H
#define EGT_TIMER_H

/**
 * @file
 * @brief Timers.
 */

#include <chrono>
#include <egt/asio.hpp>
#include <egt/detail/meta.h>
#include <iosfwd>
#include <vector>

namespace egt
{
inline namespace v1
{
/**
 * @defgroup timers Timers
 * Timer related widgets.
 */

/**
 * Basic one shot timer.
 *
 * This is a timer that will fire once after the specified duration.
 * To handle the timeout, call on_timeout with a callback.
 *
 * There are two main use cases for this class.  One, is you can derive from
 * this class and overload the Timer::timeout() member function.
 *
 * @code{.cpp}
 * struct MyTimer : public Timer
 * {
 *     MyTimer()
 *         : Timer(std::chrono::milliseconds(100))
 *     {}
 *
 *     void timeout() override
 *     {
 *         Timer::timeout();
 *         ...
 *     }
 * };
 * MyTimer timer;
 * timer.start();
 * @endcode
 *
 * The other way is to create an instance of this class and connect a
 * handler by calling Timer::on_timeout().
 *
 * @code{.cpp}
 * Timer timer(std::chrono::milliseconds(100));
 * timer.on_timeout([]() {
 *     ...
 * });
 * timer.start();
 * @endcode
 *
 * @ingroup timers
 * @see PeriodicTimer
 */
class EGT_API Timer
{
public:

    /**
     * Timer callback function definition.
     */
    using TimerCallback = std::function<void()>;

    /**
     * Construct a one-shot timer.
     *
     * The duration of the timer can be specified when calling
     * start_with_duration() when using this constructor.
     */
    Timer() noexcept;

    /**
     * Construct a one-shot timer with the specified duration.
     */
    explicit Timer(std::chrono::milliseconds duration) noexcept;

    EGT_OPS_NOCOPY_MOVE_EXCEPT(Timer);
    virtual ~Timer() noexcept;

    /**
     * Start the timer.
     *
     * It is safe to call start() on an already running timer.  It will simply
     * reset and start again.
     *
     * @note Assumes a duration is set already.
     */
    virtual void start();

    /**
     * Start the timer with the specified duration.  This overwrites any
     * duration specified previously.
     */
    void start_with_duration(std::chrono::milliseconds duration);

    /**
     * Change the duration of the timer.
     *
     * This will stop the timer, change the duration, and restart the timer
     * with the new duration if the timer was already running.
     */
    void change_duration(std::chrono::milliseconds duration);

    /**
     * Cancel, or stop, the timer.
     */
    void cancel();

    /**
     * Alias for cancel().
     */
    void stop() { cancel(); }

    /**
     * Called when the timer times out.
     *
     * This will invoke any callback registered with add_handler(). If you
     * override this you must make sure to call Timer::timeout() or
     * equivalent if you want callbacks to still be called.
     */
    void timeout();

    /**
     * Return the current duration of the timer.
     */
    std::chrono::milliseconds duration() const { return m_duration; }

    /**
     * Returns true if the timer is currently running.
     */
    bool running() const { return m_running; }

    /// Handle type.
    using RegisterHandle = uint64_t;

    /**
     * Add a handler callback to be called with the timer times out.
     *
     * This function can be called any number of times to add handlers.
     *
     * @param callback The callback to invoke on event.
     * @return A handle used to identify the registration.  This can then be
     *         passed to remove_handler().
     */
    RegisterHandle on_timeout(TimerCallback callback);

    /**
     * Clear all handlers.
     */
    void clear_handlers();

    /**
     * Remove an event handler.
     *
     * @param handle The handle returned from on_timeout().
     */
    void remove_handler(RegisterHandle handle);

    /**
    * Get the name of the Timer.
    */
    const std::string& name() const { return m_name; }

    /**
     * Set the name of the Timer.
     *
     * Assigns a human readable name to a Timer that can then be used to
     * find timers by name or debug.
     *
     * @param[in] name Name to set for the Object.
     */
    void name(const std::string& name) { m_name = name; }

protected:

    /**
     * Counter used to generate unique handles for each callback registration.
     */
    RegisterHandle m_handle_counter{0};

    /**
     * Invoke any registered handlers.
     */
    void invoke_handlers();

    /**
     * Manages metadata about a registered callback.
     * @private
     */
    struct CallbackMeta
    {
        CallbackMeta(TimerCallback c,
                     RegisterHandle h) noexcept
            : callback(std::move(c)),
              handle(h)
        {}

        TimerCallback callback;
        RegisterHandle handle{0};
    };

    /// Type for array of registered callbacks.
    using CallbackArray = std::vector<CallbackMeta>;

    /// Asio timer object.
    asio::steady_timer m_timer;

    /// The duration of the timer.
    std::chrono::milliseconds m_duration{};

    /// Array of registered callbacks.
    CallbackArray m_callbacks;

    /// When true, currently running.
    bool m_running{false};

    /// A user defined name for the Timer.
    std::string m_name;

private:

    void internal_timer_callback(const asio::error_code& error);
    void do_cancel();
};

/**
 * Periodic timer.
 *
 * This is a timer that will keep firing at the duration interval until it
 * is stopped by calling cancel().
 *
 * @b Example
 * @code{.cpp}
 * PeriodicTimer timer(std::chrono::seconds(1));
 * timer.on_timeout([]()
 * {
 *     // called every 1 second
 * });
 * timer.start();
 * @endcode
 *
 * @see Timer
 *
 * @ingroup timers
 */
class EGT_API PeriodicTimer : public Timer
{
public:

    using Timer::Timer;

    void start() override;

private:

    void internal_timer_callback(const asio::error_code& error);
};

/**
 * Dump debug information about all timers to the specified ostream.
 */
void dump_timers(std::ostream& out);

}
}

#endif
