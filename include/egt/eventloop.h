/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_EVENTLOOP_H
#define EGT_EVENTLOOP_H

/**
 * @file
 * @brief Working with the event loop.
 */

#include <egt/detail/meta.h>
#include <functional>
#include <memory>
#include <vector>

namespace egt
{
namespace asio
{
class io_context;
}

inline namespace v1
{
class Application;

namespace detail
{
class PriorityQueue;
}

/**
 * Event loop interface.
 */
class EGT_API EventLoop
{
public:

    explicit EventLoop(const Application& app) noexcept;
    EventLoop(const EventLoop&) = delete;
    EventLoop& operator=(const EventLoop&) = delete;
    EventLoop(EventLoop&&) = delete;
    EventLoop& operator=(EventLoop&&) = delete;

    /**
     * Get a reference to the internal ASIO io_context object.
     */
    asio::io_context& io();

    /**
     * Perform a draw.
     *
     * @note You do not normally need to call this directly.  It is called by
     * step() and run() automatically.
     */
    void draw();

    /**
     * Run the event loop.
     *
     * This will not return until quit() is called.
     *
     * @return The number of events handled.
     */
    int run();

    /**
     * Single step on the event loop.
     *
     * This is the same as calling:
     * @code{.cpp}
     * auto ret = poll();
     * if (ret)
     *     draw();
     * @endcode
     *
     * @note If calling this manually, this will not invoke any idle callbacks.
     * @return The number of events handled.
     */
    int step();

    /**
     * Run some pending events and return.
     *
     * @note This does not call draw().  If poll() returns a non-zero value,
     * you must manually call draw().
     * @return The number of events handled.
     */
    int poll();

    /**
     * Quit the event loop.
     *
     * This will cause the run() function to return.
     * @note Allow optional non-zero exit value
     */
    void quit(int exit_value = 0);

    /**
     * Event callback function definition.
     */
    using IdleCallback = std::function<void ()>;

    /**
     * Add a callback to be called any time the event loop is idle.
     */
    void add_idle_callback(IdleCallback func);

    /// @private
    detail::PriorityQueue& queue();

    ~EventLoop() noexcept;

protected:

    /// Wait for an event to occur.
    int wait();

    /// Invoke idle callbacks.
    void invoke_idle_callbacks();

    struct EventLoopImpl;

    /// Internal event loop implementation.
    std::unique_ptr<EventLoopImpl> m_impl;

    /// Registered idle callbacks.
    std::vector<IdleCallback> m_idle;

    /// Used internally to determine whether the event loop should exit.
    bool m_do_quit{false};

    /// Return value when application quits
    int m_exit_value;

    /// Application reference.
    const Application& m_app;
};

}
}

#endif
