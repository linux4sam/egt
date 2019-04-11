//
// detail/impl/posix_mutex.ipp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_DETAIL_IMPL_POSIX_MUTEX_IPP
#define EGT_ASIO_DETAIL_IMPL_POSIX_MUTEX_IPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

#if defined(EGT_ASIO_HAS_PTHREADS)

#include <egt/asio/detail/posix_mutex.hpp>
#include <egt/asio/detail/throw_error.hpp>
#include <egt/asio/error.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace detail {

posix_mutex::posix_mutex()
{
  int error = ::pthread_mutex_init(&mutex_, 0);
  egt::asio::error_code ec(error,
      egt::asio::error::get_system_category());
  egt::asio::detail::throw_error(ec, "mutex");
}

} // namespace detail
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#endif // defined(EGT_ASIO_HAS_PTHREADS)

#endif // EGT_ASIO_DETAIL_IMPL_POSIX_MUTEX_IPP
