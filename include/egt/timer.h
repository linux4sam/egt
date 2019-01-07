/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_TIMER_H
#define EGT_TIMER_H

#include <asio.hpp>
#include <egt/utils.h>
#include <vector>
#include <chrono>

namespace egt
{
    inline namespace v1
    {
        // TODO: https://gist.github.com/tsaarni/bb0b8d1ca33e3a1bfea1

        /**
         * Basic one shot timer.
         *
         * This is a timer that will fire once after the specified duration.
         * To handle the timeout, call on_timeout with a callback.
         */
        class Timer : public detail::noncopyable
        {
        public:

            /**
             * Timer callback function definition.
             */
            using timer_callback_t = std::function<void()>;

            /**
             * Construct a one-shot timer.
             *
             * The duration of the timer can be specified when calling
             * start_with_duration() when using this constructor.
             */
            explicit Timer() noexcept;

            /**
             * Construct a one-shot timer with the specified duration.
             */
            explicit Timer(std::chrono::milliseconds duration) noexcept;

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
             * Change the duration of the timer.
             *
             * This will stop the timer, change the duration, and restart the timer
             * with the new duration if the timer was alrady running.
             */
            virtual void change_duration(std::chrono::milliseconds duration);

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

            /**
             * Add a callback to be called with the timer times out.
             */
            void on_timeout(timer_callback_t callback)
            {
                m_callbacks.push_back(callback);
            }

            virtual ~Timer();

        protected:

            /**
             * Invoke any registered handlers.
             */
            virtual void invoke_handlers();

            using callback_array = std::vector<timer_callback_t>;

            asio::steady_timer m_timer;
            std::chrono::milliseconds m_duration{};
            callback_array m_callbacks;
            bool m_running{false};

        private:

            void timer_callback(const asio::error_code& error);
            void do_cancel();
        };

        /**
         * Periodic timer.
         *
         * This is a timer that will keep firing at the duration interval until it
         * is stopped.
         */
        class PeriodicTimer : public Timer
        {
        public:
            /**
             * Construct a periodic timer.
             *
             * The duration of the timer can be specified when calling
             * start_with_duration() instead.
             */
            explicit PeriodicTimer() noexcept;

            /**
             * Construct a periodic timer with the specified duration.
             */
            explicit PeriodicTimer(std::chrono::milliseconds interval) noexcept;

            virtual void start() override;

            virtual ~PeriodicTimer() {}

        private:

            void timer_callback(const asio::error_code& error);
        };

    }
}

#endif
