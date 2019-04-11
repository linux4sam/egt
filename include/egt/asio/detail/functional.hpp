//
// detail/functional.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_DETAIL_FUNCTIONAL_HPP
#define EGT_ASIO_DETAIL_FUNCTIONAL_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

#include <functional>

#if !defined(EGT_ASIO_HAS_STD_FUNCTION)
# include <boost/function.hpp>
#endif // !defined(EGT_ASIO_HAS_STD_FUNCTION)

namespace egt {
namespace asio {
namespace detail {

#if defined(EGT_ASIO_HAS_STD_FUNCTION)
using std::function;
#else // defined(EGT_ASIO_HAS_STD_FUNCTION)
using boost::function;
#endif // defined(EGT_ASIO_HAS_STD_FUNCTION)

} // namespace detail
} // namespace asio
} // namespace egt

#endif // EGT_ASIO_DETAIL_FUNCTIONAL_HPP
