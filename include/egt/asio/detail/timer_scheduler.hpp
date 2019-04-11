//
// detail/timer_scheduler.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_DETAIL_TIMER_SCHEDULER_HPP
#define EGT_ASIO_DETAIL_TIMER_SCHEDULER_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>
#include <egt/asio/detail/timer_scheduler_fwd.hpp>

#if defined(EGT_ASIO_WINDOWS_RUNTIME)
# include <egt/asio/detail/winrt_timer_scheduler.hpp>
#elif defined(EGT_ASIO_HAS_IOCP)
# include <egt/asio/detail/win_iocp_io_context.hpp>
#elif defined(EGT_ASIO_HAS_EPOLL)
# include <egt/asio/detail/epoll_reactor.hpp>
#elif defined(EGT_ASIO_HAS_KQUEUE)
# include <egt/asio/detail/kqueue_reactor.hpp>
#elif defined(EGT_ASIO_HAS_DEV_POLL)
# include <egt/asio/detail/dev_poll_reactor.hpp>
#else
# include <egt/asio/detail/select_reactor.hpp>
#endif

#endif // EGT_ASIO_DETAIL_TIMER_SCHEDULER_HPP
