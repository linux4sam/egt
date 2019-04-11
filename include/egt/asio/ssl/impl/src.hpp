//
// impl/ssl/src.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_SSL_IMPL_SRC_HPP
#define EGT_ASIO_SSL_IMPL_SRC_HPP

#define EGT_ASIO_SOURCE

#include <egt/asio/detail/config.hpp>

#if defined(EGT_ASIO_HEADER_ONLY)
# error Do not compile Asio library source with EGT_ASIO_HEADER_ONLY defined
#endif

#include <egt/asio/ssl/impl/context.ipp>
#include <egt/asio/ssl/impl/error.ipp>
#include <egt/asio/ssl/detail/impl/engine.ipp>
#include <egt/asio/ssl/detail/impl/openssl_init.ipp>
#include <egt/asio/ssl/impl/rfc2818_verification.ipp>

#endif // EGT_ASIO_SSL_IMPL_SRC_HPP
