/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_TIMER_H
#define EGT_TIMER_H

#include <chrono>
#include <egt/asio.hpp>
#include <egt/utils.h>
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

void dump_timers(std::ostream& out);

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
class Timer : public detail::noncopyable
{
public:

    /**
     * Timer callback function definition.
     */
    using timer_callback_t = std::function<void()>;

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
    virtual void start_with_duration(std::chrono::milliseconds duration);

    /**
     * Change the duration of the timer.
     *
     * This will stop the timer, change the duration, and restart the timer
     * with the new duration if the timer was already running.
     */
    virtual void change_duration(std::chrono::milliseconds duration);

    /**
     * Cancel, or stop, the timer.
     */
    virtual void cancel();

    /**
     * Called when the timer times out.
     *
     * This will invoke any callback registered with add_handler(). If you
     * override this you must make sure to call Timer::timeout() or
     * equivalent if you want callbacks to still be called.
     */
    virtual void timeout();

    /**
     * Return the current duration of the timer.
     */
    inline std::chrono::milliseconds duration() const { return m_duration; }

    /**
     * Returns true if the timer is currently running.
     */
    inline bool running() const { return m_running; }

    /**
     * Add a handler callback to be called with the timer times out.
     *
     * This function can be called any number of times to add handlers.
     */
    inline void on_timeout(timer_callback_t callback)
    {
        if (callback)
            m_callbacks.emplace_back(std::move(callback));
    }

    /**
     * Clear all handlers.
     */
    inline void clear_handlers()
    {
        m_callbacks.clear();
    }

    virtual ~Timer() noexcept;

protected:

    /**
     * Invoke any registered handlers.
     */
    virtual void invoke_handlers();

    using callback_array = std::vector<timer_callback_t>;

    /**
     * ASIO timer object.
     */
    asio::steady_timer m_timer;

    /**
     * The duration of the timer.
     */
    std::chrono::milliseconds m_duration{};

    /**
     * Array of registered callbacks.
     */
    callback_array m_callbacks;

    /**
     * When true, currently running.
     */
    bool m_running{false};

private:

    void timer_callback(const asio::error_code& error);
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
class PeriodicTimer : public Timer
{
public:

    /**
     * Construct a periodic timer.
     *
     * The duration of the timer can be specified when calling
     * start_with_duration() instead.
     */
    PeriodicTimer() noexcept = default;

    /**
     * Construct a periodic timer with the specified duration.
     */
    explicit PeriodicTimer(std::chrono::milliseconds interval) noexcept;

    virtual void start() override;

    virtual ~PeriodicTimer() noexcept = default;

private:

    void timer_callback(const asio::error_code& error);
};

}
}

#endif
