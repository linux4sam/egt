//
// detail/timer_queue_set.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_DETAIL_TIMER_QUEUE_SET_HPP
#define EGT_ASIO_DETAIL_TIMER_QUEUE_SET_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>
#include <egt/asio/detail/timer_queue_base.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace detail {

class timer_queue_set
{
public:
  // Constructor.
  EGT_ASIO_DECL timer_queue_set();

  // Add a timer queue to the set.
  EGT_ASIO_DECL void insert(timer_queue_base* q);

  // Remove a timer queue from the set.
  EGT_ASIO_DECL void erase(timer_queue_base* q);

  // Determine whether all queues are empty.
  EGT_ASIO_DECL bool all_empty() const;

  // Get the wait duration in milliseconds.
  EGT_ASIO_DECL long wait_duration_msec(long max_duration) const;

  // Get the wait duration in microseconds.
  EGT_ASIO_DECL long wait_duration_usec(long max_duration) const;

  // Dequeue all ready timers.
  EGT_ASIO_DECL void get_ready_timers(op_queue<operation>& ops);

  // Dequeue all timers.
  EGT_ASIO_DECL void get_all_timers(op_queue<operation>& ops);

private:
  timer_queue_base* first_;
};

} // namespace detail
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#if defined(EGT_ASIO_HEADER_ONLY)
# include <egt/asio/detail/impl/timer_queue_set.ipp>
#endif // defined(EGT_ASIO_HEADER_ONLY)

#endif // EGT_ASIO_DETAIL_TIMER_QUEUE_SET_HPP
