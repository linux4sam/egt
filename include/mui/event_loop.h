/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_EVENT_LOOP_H
#define MUI_EVENT_LOOP_H

/**
 * @file
 * @brief Working with event loops.
 */

#include <functional>
#include <memory>
#include <mui/utils.h>
#include <vector>

namespace asio
{
    class io_context;
}

namespace mui
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
     *
     */
    class EventLoop : public detail::noncopyable
    {
    public:

        EventLoop() noexcept;

        asio::io_context& io();

        virtual int wait();

        virtual int run();

        virtual void quit();

        virtual void close();

        /** @todo Not implemented. */
        void add_idle_callback(event_callback func);

        virtual ~EventLoop();

    protected:

        virtual void draw();
        void invoke_idle_callbacks();

        std::unique_ptr<detail::eventloopimpl> m_impl;

        std::vector<event_callback> m_idle;
    };

}

#endif
