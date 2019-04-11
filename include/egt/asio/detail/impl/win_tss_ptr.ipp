//
// detail/impl/win_tss_ptr.ipp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_DETAIL_IMPL_WIN_TSS_PTR_IPP
#define EGT_ASIO_DETAIL_IMPL_WIN_TSS_PTR_IPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

#if defined(EGT_ASIO_WINDOWS)

#include <egt/asio/detail/throw_error.hpp>
#include <egt/asio/detail/win_tss_ptr.hpp>
#include <egt/asio/error.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace detail {

DWORD win_tss_ptr_create()
{
#if defined(UNDER_CE)
  const DWORD out_of_indexes = 0xFFFFFFFF;
#else
  const DWORD out_of_indexes = TLS_OUT_OF_INDEXES;
#endif

  DWORD tss_key = ::TlsAlloc();
  if (tss_key == out_of_indexes)
  {
    DWORD last_error = ::GetLastError();
    egt::asio::error_code ec(last_error,
        egt::asio::error::get_system_category());
    egt::asio::detail::throw_error(ec, "tss");
  }
  return tss_key;
}

} // namespace detail
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#endif // defined(EGT_ASIO_WINDOWS)

#endif // EGT_ASIO_DETAIL_IMPL_WIN_TSS_PTR_IPP
