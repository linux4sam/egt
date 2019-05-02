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
#ifndef EGT_DETAIL_PRIORITYQUEUE_H
#define EGT_DETAIL_PRIORITYQUEUE_H

#include <egt/asio.hpp>
#include <functional>
#include <queue>
#include <utility>

namespace egt
{
inline namespace v1
{
namespace detail
{

#ifdef USE_PRIORITY_QUEUE
enum class priorities
{
    low = 0,
    moderate = 50,
    high = 100,
};

class PriorityQueue
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
            m_handlers.pop();
            handler.execute();
        }
    }

    template <typename Handler>
    class WrappedHandler
    {
    public:
        WrappedHandler(PriorityQueue& q, priorities p, Handler h)
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

        PriorityQueue& queue_;
        priorities m_priority;
        Handler handler_;
    };

    template <typename Handler>
    WrappedHandler<Handler> wrap(priorities priority, Handler handler)
    {
        return WrappedHandler<Handler>(*this, priority, handler);
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
                         PriorityQueue::WrappedHandler<Handler>* h)
{
    h->queue_.add(h->m_priority, std::forward<Function>(f));
}

#endif

}
}
}

#endif
