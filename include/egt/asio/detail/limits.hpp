//
// detail/limits.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_DETAIL_LIMITS_HPP
#define EGT_ASIO_DETAIL_LIMITS_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

#if defined(EGT_ASIO_HAS_BOOST_LIMITS)
# include <boost/limits.hpp>
#else // defined(EGT_ASIO_HAS_BOOST_LIMITS)
# include <limits>
#endif // defined(EGT_ASIO_HAS_BOOST_LIMITS)

#endif // EGT_ASIO_DETAIL_LIMITS_HPP
