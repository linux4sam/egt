//
// detail/descriptor_ops.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_DETAIL_DESCRIPTOR_OPS_HPP
#define EGT_ASIO_DETAIL_DESCRIPTOR_OPS_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

#if !defined(EGT_ASIO_WINDOWS) \
  && !defined(EGT_ASIO_WINDOWS_RUNTIME) \
  && !defined(__CYGWIN__)

#include <cstddef>
#include <egt/asio/error.hpp>
#include <egt/asio/error_code.hpp>
#include <egt/asio/detail/socket_types.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace detail {
namespace descriptor_ops {

// Descriptor state bits.
enum
{
  // The user wants a non-blocking descriptor.
  user_set_non_blocking = 1,

  // The descriptor has been set non-blocking.
  internal_non_blocking = 2,

  // Helper "state" used to determine whether the descriptor is non-blocking.
  non_blocking = user_set_non_blocking | internal_non_blocking,

  // The descriptor may have been dup()-ed.
  possible_dup = 4
};

typedef unsigned char state_type;

template <typename ReturnType>
inline ReturnType error_wrapper(ReturnType return_value,
    egt::asio::error_code& ec)
{
  ec = egt::asio::error_code(errno,
      egt::asio::error::get_system_category());
  return return_value;
}

EGT_ASIO_DECL int open(const char* path, int flags,
    egt::asio::error_code& ec);

EGT_ASIO_DECL int close(int d, state_type& state,
    egt::asio::error_code& ec);

EGT_ASIO_DECL bool set_user_non_blocking(int d,
    state_type& state, bool value, egt::asio::error_code& ec);

EGT_ASIO_DECL bool set_internal_non_blocking(int d,
    state_type& state, bool value, egt::asio::error_code& ec);

typedef iovec buf;

EGT_ASIO_DECL std::size_t sync_read(int d, state_type state, buf* bufs,
    std::size_t count, bool all_empty, egt::asio::error_code& ec);

EGT_ASIO_DECL bool non_blocking_read(int d, buf* bufs, std::size_t count,
    egt::asio::error_code& ec, std::size_t& bytes_transferred);

EGT_ASIO_DECL std::size_t sync_write(int d, state_type state,
    const buf* bufs, std::size_t count, bool all_empty,
    egt::asio::error_code& ec);

EGT_ASIO_DECL bool non_blocking_write(int d,
    const buf* bufs, std::size_t count,
    egt::asio::error_code& ec, std::size_t& bytes_transferred);

EGT_ASIO_DECL int ioctl(int d, state_type& state, long cmd,
    ioctl_arg_type* arg, egt::asio::error_code& ec);

EGT_ASIO_DECL int fcntl(int d, int cmd, egt::asio::error_code& ec);

EGT_ASIO_DECL int fcntl(int d, int cmd,
    long arg, egt::asio::error_code& ec);

EGT_ASIO_DECL int poll_read(int d,
    state_type state, egt::asio::error_code& ec);

EGT_ASIO_DECL int poll_write(int d,
    state_type state, egt::asio::error_code& ec);

EGT_ASIO_DECL int poll_error(int d,
    state_type state, egt::asio::error_code& ec);

} // namespace descriptor_ops
} // namespace detail
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#if defined(EGT_ASIO_HEADER_ONLY)
# include <egt/asio/detail/impl/descriptor_ops.ipp>
#endif // defined(EGT_ASIO_HEADER_ONLY)

#endif // !defined(EGT_ASIO_WINDOWS)
       //   && !defined(EGT_ASIO_WINDOWS_RUNTIME)
       //   && !defined(__CYGWIN__)

#endif // EGT_ASIO_DETAIL_DESCRIPTOR_OPS_HPP
