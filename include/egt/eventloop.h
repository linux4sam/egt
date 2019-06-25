/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_EVENTLOOP_H
#define EGT_EVENTLOOP_H

/**
 * @file
 * @brief Working with event loops.
 */

#include <egt/detail/priorityqueue.h>
#include <egt/utils.h>
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

namespace detail
{
struct EventLoopImpl;
}

/**
 * Event callback function defitiion.
 */
using event_callback = std::function<void ()>;

/**
 * Event loop interface.
 */
class EventLoop : public detail::noncopyable
{
public:

    EventLoop() noexcept;

    /**
     * Get a reference to the internal ASIO io_context object.
     */
    asio::io_context& io();

    /**
     * Wait for an event to occur.
     */
    virtual int wait();

    /**
     * Run the event loop.
     *
     * This will not return until quit is called.
     */
    virtual int run(bool enable_fps = false);

    /**
     * Single step on the event loop.
     */
    virtual int step();

    /**
     * Quit the event loop.
     *
     * This will cause the run() function to return.
     */
    virtual void quit();

    /**
     * Add a callback to be called any time the event loop is idle.
     *
     * This is useful for executing long running tasks that should otherwise
     * not impact any other event handling performance.
     */
    void add_idle_callback(event_callback func);

#ifdef USE_PRIORITY_QUEUE
    inline detail::PriorityQueue& queue() { return m_queue; }
#endif

    virtual ~EventLoop();

protected:

    virtual void draw();

    /**
     * Called to invoke idle callbacks.
     */
    void invoke_idle_callbacks();

    /**
     * Internal event loop implementation.
     */
    std::unique_ptr<detail::EventLoopImpl> m_impl;

    /**
     * Registered idle callbacks.
     */
    std::vector<event_callback> m_idle;

#ifdef USE_PRIORITY_QUEUE
    detail::PriorityQueue m_queue;
#endif
};


}
}

#endif
