//
// detail/null_global.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_DETAIL_NULL_GLOBAL_HPP
#define EGT_ASIO_DETAIL_NULL_GLOBAL_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace detail {

template <typename T>
struct null_global_impl
{
  null_global_impl()
    : ptr_(0)
  {
  }

  // Destructor automatically cleans up the global.
  ~null_global_impl()
  {
    delete ptr_;
  }

  static null_global_impl instance_;
  T* ptr_;
};

template <typename T>
null_global_impl<T> null_global_impl<T>::instance_;

template <typename T>
T& null_global()
{
  if (null_global_impl<T>::instance_.ptr_ == 0)
    null_global_impl<T>::instance_.ptr_ = new T;
  return *null_global_impl<T>::instance_.ptr_;
}

} // namespace detail
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#endif // EGT_ASIO_DETAIL_NULL_GLOBAL_HPP
