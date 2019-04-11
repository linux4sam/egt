//
// detail/impl/posix_tss_ptr.ipp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_DETAIL_IMPL_POSIX_TSS_PTR_IPP
#define EGT_ASIO_DETAIL_IMPL_POSIX_TSS_PTR_IPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

#if defined(EGT_ASIO_HAS_PTHREADS)

#include <egt/asio/detail/posix_tss_ptr.hpp>
#include <egt/asio/detail/throw_error.hpp>
#include <egt/asio/error.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace detail {

void posix_tss_ptr_create(pthread_key_t& key)
{
  int error = ::pthread_key_create(&key, 0);
  egt::asio::error_code ec(error,
      egt::asio::error::get_system_category());
  egt::asio::detail::throw_error(ec, "tss");
}

} // namespace detail
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#endif // defined(EGT_ASIO_HAS_PTHREADS)

#endif // EGT_ASIO_DETAIL_IMPL_POSIX_TSS_PTR_IPP
