//
// detail/assert.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_DETAIL_ASSERT_HPP
#define EGT_ASIO_DETAIL_ASSERT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

#if defined(EGT_ASIO_HAS_BOOST_ASSERT)
# include <boost/assert.hpp>
#else // defined(EGT_ASIO_HAS_BOOST_ASSERT)
# include <cassert>
#endif // defined(EGT_ASIO_HAS_BOOST_ASSERT)

#if defined(EGT_ASIO_HAS_BOOST_ASSERT)
# define EGT_ASIO_ASSERT(expr) BOOST_ASSERT(expr)
#else // defined(EGT_ASIO_HAS_BOOST_ASSERT)
# define EGT_ASIO_ASSERT(expr) assert(expr)
#endif // defined(EGT_ASIO_HAS_BOOST_ASSERT)

#endif // EGT_ASIO_DETAIL_ASSERT_HPP
