//
// ssl/error.hpp
// ~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_SSL_ERROR_HPP
#define EGT_ASIO_SSL_ERROR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>
#include <egt/asio/error_code.hpp>
#include <egt/asio/ssl/detail/openssl_types.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace error {

enum ssl_errors
{
  // Error numbers are those produced by openssl.
};

extern EGT_ASIO_DECL
const egt::asio::error_category& get_ssl_category();

static const egt::asio::error_category&
  ssl_category EGT_ASIO_UNUSED_VARIABLE
  = egt::asio::error::get_ssl_category();

} // namespace error
namespace ssl {
namespace error {

enum stream_errors
{
#if defined(GENERATING_DOCUMENTATION)
  /// The underlying stream closed before the ssl stream gracefully shut down.
  stream_truncated
#elif (OPENSSL_VERSION_NUMBER < 0x10100000L) && !defined(OPENSSL_IS_BORINGSSL)
  stream_truncated = ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SHORT_READ)
#else
  stream_truncated = 1
#endif
};

extern EGT_ASIO_DECL
const egt::asio::error_category& get_stream_category();

static const egt::asio::error_category&
  stream_category EGT_ASIO_UNUSED_VARIABLE
  = egt::asio::ssl::error::get_stream_category();

} // namespace error
} // namespace ssl
} // namespace asio
} // namespace egt

#if defined(EGT_ASIO_HAS_STD_SYSTEM_ERROR)
namespace std {

template<> struct is_error_code_enum<egt::asio::error::ssl_errors>
{
  static const bool value = true;
};

template<> struct is_error_code_enum<egt::asio::ssl::error::stream_errors>
{
  static const bool value = true;
};

} // namespace std
#endif // defined(EGT_ASIO_HAS_STD_SYSTEM_ERROR)

namespace egt {
namespace asio {
namespace error {

inline egt::asio::error_code make_error_code(ssl_errors e)
{
  return egt::asio::error_code(
      static_cast<int>(e), get_ssl_category());
}

} // namespace error
namespace ssl {
namespace error {

inline egt::asio::error_code make_error_code(stream_errors e)
{
  return egt::asio::error_code(
      static_cast<int>(e), get_stream_category());
}

} // namespace error
} // namespace ssl
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#if defined(EGT_ASIO_HEADER_ONLY)
# include <egt/asio/ssl/impl/error.ipp>
#endif // defined(EGT_ASIO_HEADER_ONLY)

#endif // EGT_ASIO_SSL_ERROR_HPP
