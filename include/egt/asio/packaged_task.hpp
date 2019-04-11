//
// packaged_task.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_PACKAGED_TASK_HPP
#define EGT_ASIO_PACKAGED_TASK_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

#if defined(EGT_ASIO_HAS_STD_FUTURE) \
  || defined(GENERATING_DOCUMENTATION)

#include <future>
#include <egt/asio/async_result.hpp>
#include <egt/asio/detail/type_traits.hpp>
#include <egt/asio/detail/variadic_templates.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {

#if defined(EGT_ASIO_HAS_VARIADIC_TEMPLATES) \
  || defined(GENERATING_DOCUMENTATION)

/// Partial specialisation of @c async_result for @c std::packaged_task.
template <typename Result, typename... Args, typename Signature>
class async_result<std::packaged_task<Result(Args...)>, Signature>
{
public:
  /// The packaged task is the concrete completion handler type.
  typedef std::packaged_task<Result(Args...)> completion_handler_type;

  /// The return type of the initiating function is the future obtained from
  /// the packaged task.
  typedef std::future<Result> return_type;

  /// The constructor extracts the future from the packaged task.
  explicit async_result(completion_handler_type& h)
    : future_(h.get_future())
  {
  }

  /// Returns the packaged task's future.
  return_type get()
  {
    return std::move(future_);
  }

private:
  return_type future_;
};

#else // defined(EGT_ASIO_HAS_VARIADIC_TEMPLATES)
      //   || defined(GENERATING_DOCUMENTATION)

template <typename Result, typename Signature>
struct async_result<std::packaged_task<Result()>, Signature>
{
  typedef std::packaged_task<Result()> completion_handler_type;
  typedef std::future<Result> return_type;

  explicit async_result(completion_handler_type& h)
    : future_(h.get_future())
  {
  }

  return_type get()
  {
    return std::move(future_);
  }

private:
  return_type future_;
};

#define EGT_ASIO_PRIVATE_ASYNC_RESULT_DEF(n) \
  template <typename Result, \
    EGT_ASIO_VARIADIC_TPARAMS(n), typename Signature> \
  class async_result< \
    std::packaged_task<Result(EGT_ASIO_VARIADIC_TARGS(n))>, Signature> \
  { \
  public: \
    typedef std::packaged_task< \
      Result(EGT_ASIO_VARIADIC_TARGS(n))> \
        completion_handler_type; \
  \
    typedef std::future<Result> return_type; \
  \
    explicit async_result(completion_handler_type& h) \
      : future_(h.get_future()) \
    { \
    } \
  \
    return_type get() \
    { \
      return std::move(future_); \
    } \
  \
  private: \
    return_type future_; \
  }; \
  /**/
  EGT_ASIO_VARIADIC_GENERATE(EGT_ASIO_PRIVATE_ASYNC_RESULT_DEF)
#undef EGT_ASIO_PRIVATE_ASYNC_RESULT_DEF

#endif // defined(EGT_ASIO_HAS_VARIADIC_TEMPLATES)
       //   || defined(GENERATING_DOCUMENTATION)

} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#endif // defined(EGT_ASIO_HAS_STD_FUTURE)
       //   || defined(GENERATING_DOCUMENTATION)

#endif // EGT_ASIO_PACKAGED_TASK_HPP
