/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_TIMER_H
#define MUI_TIMER_H

#include "asio.hpp"
#include <mui/utils.h>
#include <cstdint>
#include <functional>
#include <vector>
#include <chrono>

namespace mui
{
    // TODO: https://gist.github.com/tsaarni/bb0b8d1ca33e3a1bfea1

    //using namespace std::chrono_literals;

    /**
     * Basic one shot timer.
     */
    class Timer : public detail::noncopyable
    {
    public:

        /**
         * Timer callback function definition.
         */
        using timer_callback_t = std::function<void()>;

        /**
         * Construct a one-shot timer with the specified duration.
         */
        //explicit Timer(uint64_t duration = 0) noexcept;

        explicit Timer() noexcept;
        explicit Timer(std::chrono::milliseconds duration) noexcept;

        /*
        template <typename Rep, typename Period>
               explicit Timer(std::chrono::duration<Rep, Period> duration) noexcept
                   : m_duration(std::chrono::duration_cast<decltype(my_duration)>(duration))
               {
               }
               */

        /**
         * Start the timer.
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
         * Cancel the timer.
         */
        virtual void cancel();

        /**
         * Called when the timer times out.
         *
         * This will invoke any callback registered with add_handler(). If you
         * override this you must make sure to call Timer::timeout() or
         * equivelant if you want callbacks to still be called.
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

        void add_handler(timer_callback_t callback)
        {
            m_callbacks.push_back(callback);
        }

        virtual ~Timer();

    protected:

        virtual void invoke_handlers();

        void timer_callback(const asio::error_code& error);

        asio::steady_timer m_timer;
        std::chrono::milliseconds m_duration{};
        //uint64_t m_duration {0};
        std::vector<timer_callback_t> m_callbacks;
        bool m_running { false};
    };

    /**
     * Periodic timer.
     */
    class PeriodicTimer : public Timer
    {
    public:
        //        explicit PeriodicTimer(uint64_t period = 0) noexcept;
        explicit PeriodicTimer() noexcept;
        explicit PeriodicTimer(std::chrono::milliseconds period) noexcept;

        virtual void start();

        virtual ~PeriodicTimer() {}

    protected:

        void timer_callback(const asio::error_code& error);
    };

}

#endif
