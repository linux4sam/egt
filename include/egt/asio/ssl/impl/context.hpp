//
// ssl/impl/context.hpp
// ~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2005 Voipster / Indrek dot Juhani at voipster dot com
// Copyright (c) 2005-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_SSL_IMPL_CONTEXT_HPP
#define EGT_ASIO_SSL_IMPL_CONTEXT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

#include <egt/asio/detail/throw_error.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace ssl {

template <typename VerifyCallback>
void context::set_verify_callback(VerifyCallback callback)
{
  egt::asio::error_code ec;
  this->set_verify_callback(callback, ec);
  egt::asio::detail::throw_error(ec, "set_verify_callback");
}

template <typename VerifyCallback>
EGT_ASIO_SYNC_OP_VOID context::set_verify_callback(
    VerifyCallback callback, egt::asio::error_code& ec)
{
  do_set_verify_callback(
      new detail::verify_callback<VerifyCallback>(callback), ec);
  EGT_ASIO_SYNC_OP_VOID_RETURN(ec);
}

template <typename PasswordCallback>
void context::set_password_callback(PasswordCallback callback)
{
  egt::asio::error_code ec;
  this->set_password_callback(callback, ec);
  egt::asio::detail::throw_error(ec, "set_password_callback");
}

template <typename PasswordCallback>
EGT_ASIO_SYNC_OP_VOID context::set_password_callback(
    PasswordCallback callback, egt::asio::error_code& ec)
{
  do_set_password_callback(
      new detail::password_callback<PasswordCallback>(callback), ec);
  EGT_ASIO_SYNC_OP_VOID_RETURN(ec);
}

} // namespace ssl
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#endif // EGT_ASIO_SSL_IMPL_CONTEXT_HPP
