//
// detail/impl/throw_error.ipp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_DETAIL_IMPL_THROW_ERROR_IPP
#define EGT_ASIO_DETAIL_IMPL_THROW_ERROR_IPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>
#include <egt/asio/detail/throw_error.hpp>
#include <egt/asio/detail/throw_exception.hpp>
#include <egt/asio/system_error.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace detail {

void do_throw_error(const egt::asio::error_code& err)
{
  egt::asio::system_error e(err);
  egt::asio::detail::throw_exception(e);
}

void do_throw_error(const egt::asio::error_code& err, const char* location)
{
  // boostify: non-boost code starts here
#if defined(EGT_ASIO_MSVC) && defined(EGT_ASIO_HAS_STD_SYSTEM_ERROR)
  // Microsoft's implementation of std::system_error is non-conformant in that
  // it ignores the error code's message when a "what" string is supplied. We'll
  // work around this by explicitly formatting the "what" string.
  std::string what_msg = location;
  what_msg += ": ";
  what_msg += err.message();
  egt::asio::system_error e(err, what_msg);
  egt::asio::detail::throw_exception(e);
#else // defined(EGT_ASIO_MSVC) && defined(EGT_ASIO_HAS_STD_SYSTEM_ERROR)
  // boostify: non-boost code ends here
  egt::asio::system_error e(err, location);
  egt::asio::detail::throw_exception(e);
  // boostify: non-boost code starts here
#endif // defined(EGT_ASIO_MSVC) && defined(EGT_ASIO_HAS_STD_SYSTEM_ERROR)
  // boostify: non-boost code ends here
}

} // namespace detail
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#endif // EGT_ASIO_DETAIL_IMPL_THROW_ERROR_IPP
