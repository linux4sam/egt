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

#include <cstdint>
#include <functional>
#include <mui/utils.h>
#include <memory>

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
    typedef std::function<void (int fd, uint32_t mask, void* data)> event_callback;

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

        std::unique_ptr<detail::eventloopimpl> m_impl;

    };

}

#endif
