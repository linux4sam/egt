//
// detail/thread.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_DETAIL_THREAD_HPP
#define EGT_ASIO_DETAIL_THREAD_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

#if !defined(EGT_ASIO_HAS_THREADS)
# include <egt/asio/detail/null_thread.hpp>
#elif defined(EGT_ASIO_WINDOWS)
# if defined(UNDER_CE)
#  include <egt/asio/detail/wince_thread.hpp>
# elif defined(EGT_ASIO_WINDOWS_APP)
#  include <egt/asio/detail/winapp_thread.hpp>
# else
#  include <egt/asio/detail/win_thread.hpp>
# endif
#elif defined(EGT_ASIO_HAS_PTHREADS)
# include <egt/asio/detail/posix_thread.hpp>
#elif defined(EGT_ASIO_HAS_STD_THREAD)
# include <egt/asio/detail/std_thread.hpp>
#else
# error Only Windows, POSIX and std::thread are supported!
#endif

namespace egt {
namespace asio {
namespace detail {

#if !defined(EGT_ASIO_HAS_THREADS)
typedef null_thread thread;
#elif defined(EGT_ASIO_WINDOWS)
# if defined(UNDER_CE)
typedef wince_thread thread;
# elif defined(EGT_ASIO_WINDOWS_APP)
typedef winapp_thread thread;
# else
typedef win_thread thread;
# endif
#elif defined(EGT_ASIO_HAS_PTHREADS)
typedef posix_thread thread;
#elif defined(EGT_ASIO_HAS_STD_THREAD)
typedef std_thread thread;
#endif

} // namespace detail
} // namespace asio
} // namespace egt

#endif // EGT_ASIO_DETAIL_THREAD_HPP
