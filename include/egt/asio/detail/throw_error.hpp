//
// detail/throw_error.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_DETAIL_THROW_ERROR_HPP
#define EGT_ASIO_DETAIL_THROW_ERROR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>
#include <egt/asio/error_code.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace detail {

EGT_ASIO_DECL void do_throw_error(const egt::asio::error_code& err);

EGT_ASIO_DECL void do_throw_error(const egt::asio::error_code& err,
    const char* location);

inline void throw_error(const egt::asio::error_code& err)
{
  if (err)
    do_throw_error(err);
}

inline void throw_error(const egt::asio::error_code& err,
    const char* location)
{
  if (err)
    do_throw_error(err, location);
}

} // namespace detail
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#if defined(EGT_ASIO_HEADER_ONLY)
# include <egt/asio/detail/impl/throw_error.ipp>
#endif // defined(EGT_ASIO_HEADER_ONLY)

#endif // EGT_ASIO_DETAIL_THROW_ERROR_HPP
