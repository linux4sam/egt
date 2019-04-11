//
// deadline_timer.hpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_DEADLINE_TIMER_HPP
#define EGT_ASIO_DEADLINE_TIMER_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

#if defined(EGT_ASIO_HAS_BOOST_DATE_TIME) \
  || defined(GENERATING_DOCUMENTATION)

#include <egt/asio/detail/socket_types.hpp> // Must come before posix_time.
#include <egt/asio/basic_deadline_timer.hpp>

#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace egt {
namespace asio {

/// Typedef for the typical usage of timer. Uses a UTC clock.
typedef basic_deadline_timer<boost::posix_time::ptime> deadline_timer;

} // namespace asio
} // namespace egt

#endif // defined(EGT_ASIO_HAS_BOOST_DATE_TIME)
       // || defined(GENERATING_DOCUMENTATION)

#endif // EGT_ASIO_DEADLINE_TIMER_HPP
