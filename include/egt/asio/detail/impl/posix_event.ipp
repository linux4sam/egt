//
// detail/impl/posix_event.ipp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_DETAIL_IMPL_POSIX_EVENT_IPP
#define EGT_ASIO_DETAIL_IMPL_POSIX_EVENT_IPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

#if defined(EGT_ASIO_HAS_PTHREADS)

#include <egt/asio/detail/posix_event.hpp>
#include <egt/asio/detail/throw_error.hpp>
#include <egt/asio/error.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace detail {

posix_event::posix_event()
  : state_(0)
{
#if (defined(__MACH__) && defined(__APPLE__)) \
      || (defined(__ANDROID__) && (__ANDROID_API__ < 21))
  int error = ::pthread_cond_init(&cond_, 0);
#else // (defined(__MACH__) && defined(__APPLE__))
      // || (defined(__ANDROID__) && (__ANDROID_API__ < 21))
  ::pthread_condattr_t attr;
  ::pthread_condattr_init(&attr);
  int error = ::pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
  if (error == 0)
    error = ::pthread_cond_init(&cond_, &attr);
#endif // (defined(__MACH__) && defined(__APPLE__))
       // || (defined(__ANDROID__) && (__ANDROID_API__ < 21))

  egt::asio::error_code ec(error,
      egt::asio::error::get_system_category());
  egt::asio::detail::throw_error(ec, "event");
}

} // namespace detail
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#endif // defined(EGT_ASIO_HAS_PTHREADS)

#endif // EGT_ASIO_DETAIL_IMPL_POSIX_EVENT_IPP
