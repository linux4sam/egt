/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
 *
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef EGT_PRIORITYQUEUE_H
#define EGT_PRIORITYQUEUE_H

#include <asio.hpp>
#include <functional>
#include <queue>

namespace egt
{
inline namespace v1
{
namespace detail
{

enum class priorities
{
    low = 0,
    moderate = 50,
    high = 100,
};

class handler_priority_queue
{
public:

    void add(priorities priority, std::function<void()> function)
    {
        m_handlers.push(queued_handler(priority, function));
    }

    void execute_all()
    {
        while (!m_handlers.empty())
        {
            queued_handler handler = m_handlers.top();
            handler.execute();
            m_handlers.pop();

        }
    }

    template <typename Handler>
    class wrapped_handler
    {
    public:
        wrapped_handler(handler_priority_queue& q, priorities p, Handler h)
            : queue_(q), m_priority(p), handler_(h)
        {
        }

        void operator()()
        {
            handler_();
        }

        template <typename Arg1>
        void operator()(Arg1 arg1)
        {
            handler_(arg1);
        }

        template <typename Arg1, typename Arg2>
        void operator()(Arg1 arg1, Arg2 arg2)
        {
            handler_(arg1, arg2);
        }

        handler_priority_queue& queue_;
        priorities m_priority;
        Handler handler_;
    };

    template <typename Handler>
    wrapped_handler<Handler> wrap(priorities priority, Handler handler)
    {
        return wrapped_handler<Handler>(*this, priority, handler);
    }

private:

    class queued_handler
    {
    public:
        queued_handler(priorities p, std::function<void()> f)
            : m_priority(p), m_function(f)
        {
        }

        void execute()
        {
            m_function();
        }

        friend bool operator<(const queued_handler& a,
                              const queued_handler& b)
        {
            return a.m_priority < b.m_priority;
        }

    private:
        priorities m_priority;
        std::function<void()> m_function;
    };

    std::priority_queue<queued_handler> m_handlers;
};

template <typename Function, typename Handler>
void asio_handler_invoke(Function f,
                         handler_priority_queue::wrapped_handler<Handler>* h)
{
    h->queue_.add(h->m_priority, std::forward<Function>(f));
}

}
}
}

#endif
