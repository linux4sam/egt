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

#include <functional>
#include <memory>
#include <egt/utils.h>
#include <vector>

namespace asio
{
    class io_context;
}

namespace egt
{
    namespace detail
    {
        struct eventloopimpl;
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
         * Wait for an event to occure.
         */
        virtual int wait();

        /**
         * Run the event loop.
         *
         * This will not return until quit is called.
         */
        virtual int run(bool enable_fps = false);

        /**
         * Quit the event loop.
         *
         * This will cause the run() function to return.
         */
        virtual void quit();

        /** @todo Not implemented. */
        void add_idle_callback(event_callback func);

        void paint_to_file(const std::string& filename = std::string());

        virtual ~EventLoop();

    protected:

        virtual void draw();
        void invoke_idle_callbacks();

        std::unique_ptr<detail::eventloopimpl> m_impl;

        std::vector<event_callback> m_idle;
    };

}

#endif
