//
// detail/select_interrupter.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_DETAIL_SELECT_INTERRUPTER_HPP
#define EGT_ASIO_DETAIL_SELECT_INTERRUPTER_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

#if !defined(EGT_ASIO_WINDOWS_RUNTIME)

#if defined(EGT_ASIO_WINDOWS) || defined(__CYGWIN__) || defined(__SYMBIAN32__)
# include <egt/asio/detail/socket_select_interrupter.hpp>
#elif defined(EGT_ASIO_HAS_EVENTFD)
# include <egt/asio/detail/eventfd_select_interrupter.hpp>
#else
# include <egt/asio/detail/pipe_select_interrupter.hpp>
#endif

namespace egt {
namespace asio {
namespace detail {

#if defined(EGT_ASIO_WINDOWS) || defined(__CYGWIN__) || defined(__SYMBIAN32__)
typedef socket_select_interrupter select_interrupter;
#elif defined(EGT_ASIO_HAS_EVENTFD)
typedef eventfd_select_interrupter select_interrupter;
#else
typedef pipe_select_interrupter select_interrupter;
#endif

} // namespace detail
} // namespace asio
} // namespace egt

#endif // !defined(EGT_ASIO_WINDOWS_RUNTIME)

#endif // EGT_ASIO_DETAIL_SELECT_INTERRUPTER_HPP
