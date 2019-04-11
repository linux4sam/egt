//
// detail/null_thread.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_DETAIL_NULL_THREAD_HPP
#define EGT_ASIO_DETAIL_NULL_THREAD_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

#if !defined(EGT_ASIO_HAS_THREADS)

#include <egt/asio/detail/noncopyable.hpp>
#include <egt/asio/detail/throw_error.hpp>
#include <egt/asio/error.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace detail {

class null_thread
  : private noncopyable
{
public:
  // Constructor.
  template <typename Function>
  null_thread(Function, unsigned int = 0)
  {
    egt::asio::detail::throw_error(
        egt::asio::error::operation_not_supported, "thread");
  }

  // Destructor.
  ~null_thread()
  {
  }

  // Wait for the thread to exit.
  void join()
  {
  }

  // Get number of CPUs.
  static std::size_t hardware_concurrency()
  {
    return 1;
  }
};

} // namespace detail
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#endif // !defined(EGT_ASIO_HAS_THREADS)

#endif // EGT_ASIO_DETAIL_NULL_THREAD_HPP
