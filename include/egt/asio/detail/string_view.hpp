//
// detail/string_view.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_DETAIL_STRING_VIEW_HPP
#define EGT_ASIO_DETAIL_STRING_VIEW_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

#if defined(EGT_ASIO_HAS_STRING_VIEW)

#if defined(EGT_ASIO_HAS_STD_STRING_VIEW)
# include <string_view>
#elif defined(EGT_ASIO_HAS_STD_EXPERIMENTAL_STRING_VIEW)
# include <experimental/string_view>
#else // defined(EGT_ASIO_HAS_STD_EXPERIMENTAL_STRING_VIEW)
# error EGT_ASIO_HAS_STRING_VIEW is set but no string_view is available
#endif // defined(EGT_ASIO_HAS_STD_EXPERIMENTAL_STRING_VIEW)

namespace egt {
namespace asio {

#if defined(EGT_ASIO_HAS_STD_STRING_VIEW)
using std::basic_string_view;
using std::string_view;
#elif defined(EGT_ASIO_HAS_STD_EXPERIMENTAL_STRING_VIEW)
using std::experimental::basic_string_view;
using std::experimental::string_view;
#endif // defined(EGT_ASIO_HAS_STD_EXPERIMENTAL_STRING_VIEW)

} // namespace asio
} // namespace egt

# define EGT_ASIO_STRING_VIEW_PARAM egt::asio::string_view
#else // defined(EGT_ASIO_HAS_STRING_VIEW)
# define EGT_ASIO_STRING_VIEW_PARAM const std::string&
#endif // defined(EGT_ASIO_HAS_STRING_VIEW)

#endif // EGT_ASIO_DETAIL_STRING_VIEW_HPP
