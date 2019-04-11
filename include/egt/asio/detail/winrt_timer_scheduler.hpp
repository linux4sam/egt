//
// detail/winrt_timer_scheduler.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_DETAIL_WINRT_TIMER_SCHEDULER_HPP
#define EGT_ASIO_DETAIL_WINRT_TIMER_SCHEDULER_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

#if defined(EGT_ASIO_WINDOWS_RUNTIME)

#include <cstddef>
#include <egt/asio/detail/event.hpp>
#include <egt/asio/detail/limits.hpp>
#include <egt/asio/detail/mutex.hpp>
#include <egt/asio/detail/op_queue.hpp>
#include <egt/asio/detail/thread.hpp>
#include <egt/asio/detail/timer_queue_base.hpp>
#include <egt/asio/detail/timer_queue_set.hpp>
#include <egt/asio/detail/wait_op.hpp>
#include <egt/asio/io_context.hpp>

#if defined(EGT_ASIO_HAS_IOCP)
# include <egt/asio/detail/thread.hpp>
#endif // defined(EGT_ASIO_HAS_IOCP)

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace detail {

class winrt_timer_scheduler
  : public egt::asio::detail::service_base<winrt_timer_scheduler>
{
public:
  // Constructor.
  EGT_ASIO_DECL winrt_timer_scheduler(egt::asio::io_context& io_context);

  // Destructor.
  EGT_ASIO_DECL ~winrt_timer_scheduler();

  // Destroy all user-defined handler objects owned by the service.
  EGT_ASIO_DECL void shutdown();

  // Recreate internal descriptors following a fork.
  EGT_ASIO_DECL void notify_fork(
      egt::asio::io_context::fork_event fork_ev);

  // Initialise the task. No effect as this class uses its own thread.
  EGT_ASIO_DECL void init_task();

  // Add a new timer queue to the reactor.
  template <typename Time_Traits>
  void add_timer_queue(timer_queue<Time_Traits>& queue);

  // Remove a timer queue from the reactor.
  template <typename Time_Traits>
  void remove_timer_queue(timer_queue<Time_Traits>& queue);

  // Schedule a new operation in the given timer queue to expire at the
  // specified absolute time.
  template <typename Time_Traits>
  void schedule_timer(timer_queue<Time_Traits>& queue,
      const typename Time_Traits::time_type& time,
      typename timer_queue<Time_Traits>::per_timer_data& timer, wait_op* op);

  // Cancel the timer operations associated with the given token. Returns the
  // number of operations that have been posted or dispatched.
  template <typename Time_Traits>
  std::size_t cancel_timer(timer_queue<Time_Traits>& queue,
      typename timer_queue<Time_Traits>::per_timer_data& timer,
      std::size_t max_cancelled = (std::numeric_limits<std::size_t>::max)());

  // Move the timer operations associated with the given timer.
  template <typename Time_Traits>
  void move_timer(timer_queue<Time_Traits>& queue,
      typename timer_queue<Time_Traits>::per_timer_data& to,
      typename timer_queue<Time_Traits>::per_timer_data& from);

private:
  // Run the select loop in the thread.
  EGT_ASIO_DECL void run_thread();

  // Entry point for the select loop thread.
  EGT_ASIO_DECL static void call_run_thread(winrt_timer_scheduler* reactor);

  // Helper function to add a new timer queue.
  EGT_ASIO_DECL void do_add_timer_queue(timer_queue_base& queue);

  // Helper function to remove a timer queue.
  EGT_ASIO_DECL void do_remove_timer_queue(timer_queue_base& queue);

  // The io_context implementation used to post completions.
  io_context_impl& io_context_;

  // Mutex used to protect internal variables.
  egt::asio::detail::mutex mutex_;

  // Event used to wake up background thread.
  egt::asio::detail::event event_;

  // The timer queues.
  timer_queue_set timer_queues_;

  // The background thread that is waiting for timers to expire.
  egt::asio::detail::thread* thread_;

  // Does the background thread need to stop.
  bool stop_thread_;

  // Whether the service has been shut down.
  bool shutdown_;
};

} // namespace detail
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#include <egt/asio/detail/impl/winrt_timer_scheduler.hpp>
#if defined(EGT_ASIO_HEADER_ONLY)
# include <egt/asio/detail/impl/winrt_timer_scheduler.ipp>
#endif // defined(EGT_ASIO_HEADER_ONLY)

#endif // defined(EGT_ASIO_WINDOWS_RUNTIME)

#endif // EGT_ASIO_DETAIL_WINRT_TIMER_SCHEDULER_HPP
