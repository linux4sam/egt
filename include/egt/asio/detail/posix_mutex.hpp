//
// detail/posix_mutex.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_DETAIL_POSIX_MUTEX_HPP
#define EGT_ASIO_DETAIL_POSIX_MUTEX_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

#if defined(EGT_ASIO_HAS_PTHREADS)

#include <pthread.h>
#include <egt/asio/detail/noncopyable.hpp>
#include <egt/asio/detail/scoped_lock.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace detail {

class posix_event;

class posix_mutex
  : private noncopyable
{
public:
  typedef egt::asio::detail::scoped_lock<posix_mutex> scoped_lock;

  // Constructor.
  EGT_ASIO_DECL posix_mutex();

  // Destructor.
  ~posix_mutex()
  {
    ::pthread_mutex_destroy(&mutex_); // Ignore EBUSY.
  }

  // Lock the mutex.
  void lock()
  {
    (void)::pthread_mutex_lock(&mutex_); // Ignore EINVAL.
  }

  // Unlock the mutex.
  void unlock()
  {
    (void)::pthread_mutex_unlock(&mutex_); // Ignore EINVAL.
  }

private:
  friend class posix_event;
  ::pthread_mutex_t mutex_;
};

} // namespace detail
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#if defined(EGT_ASIO_HEADER_ONLY)
# include <egt/asio/detail/impl/posix_mutex.ipp>
#endif // defined(EGT_ASIO_HEADER_ONLY)

#endif // defined(EGT_ASIO_HAS_PTHREADS)

#endif // EGT_ASIO_DETAIL_POSIX_MUTEX_HPP
