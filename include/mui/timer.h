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

namespace mui
{
    // TODO: https://gist.github.com/tsaarni/bb0b8d1ca33e3a1bfea1

    /**
     * Timer callback function definition.
     */
    typedef std::function<void()> timer_callback_t;

    /**
     * Basic one shot timer.
     */
    class Timer : public detail::noncopyable
    {
    public:
        explicit Timer(uint64_t duration = 0) noexcept;

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
        virtual void start_with_duration(uint64_t duration);

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
        inline uint64_t duration() const { return m_duration; }

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

        void timer_callback(const asio::error_code& error);

        asio::steady_timer m_timer;
        //asio::high_resolution_timer m_timer;
        uint64_t m_duration;
        std::vector<timer_callback_t> m_callbacks;
        bool m_running { false};
    };

    /**
     * Periodic timer.
     */
    class PeriodicTimer : public Timer
    {
    public:
        explicit PeriodicTimer(uint64_t period = 0) noexcept;

        virtual void start();
	/*virtual void start_with_duration(uint64_t duration);
        {
            Timer::start(duration);
	    }*/
        virtual ~PeriodicTimer() {}

    protected:

        void timer_callback(const asio::error_code& error);
    };

}

#endif
