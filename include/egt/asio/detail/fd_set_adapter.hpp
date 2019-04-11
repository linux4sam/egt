//
// detail/fd_set_adapter.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_DETAIL_FD_SET_ADAPTER_HPP
#define EGT_ASIO_DETAIL_FD_SET_ADAPTER_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

#if !defined(EGT_ASIO_WINDOWS_RUNTIME)

#include <egt/asio/detail/posix_fd_set_adapter.hpp>
#include <egt/asio/detail/win_fd_set_adapter.hpp>

namespace egt {
namespace asio {
namespace detail {

#if defined(EGT_ASIO_WINDOWS) || defined(__CYGWIN__)
typedef win_fd_set_adapter fd_set_adapter;
#else
typedef posix_fd_set_adapter fd_set_adapter;
#endif

} // namespace detail
} // namespace asio
} // namespace egt

#endif // !defined(EGT_ASIO_WINDOWS_RUNTIME)

#endif // EGT_ASIO_DETAIL_FD_SET_ADAPTER_HPP
