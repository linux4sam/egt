/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/utils.h"
#include <atomic>
#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>
#include <spdlog/spdlog.h>
#include <thread>

namespace egt
{
inline namespace v1
{
namespace detail
{

/**
 * Flip thread queue.
 *
 * This creates a flip queue used for queuing up flip calls when using more
 * than one buffer.
 */
struct FlipThread : public detail::noncopyable
{
    explicit FlipThread(uint32_t max_queue = 0)
        : m_stop(false),
          m_max_queue(max_queue)
    {
        m_thread = std::thread(&FlipThread::run, this);
    }

    void run()
    {
        std::function<void()> task;
        while (true)
        {
            std::unique_lock<std::mutex> lock(m_mutex);

            while (!m_stop && m_queue.empty())
                m_condition.wait(lock);

            if (m_stop)
                return;

            task = m_queue.front();
            m_queue.pop_front();
            m_condition.notify_one();
            lock.unlock();

            task();
        }
    }

    void enqueue(std::function<void()> job)
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        while (m_max_queue && m_queue.size() >= m_max_queue)
        {
            SPDLOG_DEBUG("flip thread blocked");
            m_condition.wait(lock);
        }

        m_queue.emplace_back(std::move(job));
        m_condition.notify_one();
    }

    ~FlipThread()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_stop = true;
        m_condition.notify_all();
        lock.unlock();
        m_thread.join();
    }

    std::thread m_thread;
    std::deque<std::function<void()>> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    std::atomic<bool> m_stop;
    uint32_t m_max_queue{0};
};

}
}
}
