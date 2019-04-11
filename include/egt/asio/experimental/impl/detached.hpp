//
// experimental/impl/detached.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_EXPERIMENTAL_IMPL_DETACHED_HPP
#define EGT_ASIO_EXPERIMENTAL_IMPL_DETACHED_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>
#include <egt/asio/async_result.hpp>
#include <egt/asio/detail/variadic_templates.hpp>
#include <egt/asio/handler_type.hpp>
#include <egt/asio/system_error.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace experimental {
namespace detail {

  // Class to adapt a detached_t as a completion handler.
  class detached_handler
  {
  public:
    detached_handler(detached_t)
    {
    }

#if defined(EGT_ASIO_HAS_VARIADIC_TEMPLATES)

    template <typename... Args>
    void operator()(Args...)
    {
    }

#else // defined(EGT_ASIO_HAS_VARIADIC_TEMPLATES)

    void operator()()
    {
    }

#define EGT_ASIO_PRIVATE_DETACHED_DEF(n) \
    template <EGT_ASIO_VARIADIC_TPARAMS(n)> \
    void operator()(EGT_ASIO_VARIADIC_BYVAL_PARAMS(n)) \
    { \
    } \
    /**/
    EGT_ASIO_VARIADIC_GENERATE(EGT_ASIO_PRIVATE_DETACHED_DEF)
#undef EGT_ASIO_PRIVATE_DETACHED_DEF

#endif // defined(EGT_ASIO_HAS_VARIADIC_TEMPLATES)
  };

} // namespace detail
} // namespace experimental

#if !defined(GENERATING_DOCUMENTATION)

template <typename Signature>
struct async_result<experimental::detached_t, Signature>
{
  typedef egt::asio::experimental::detail::detached_handler
    completion_handler_type;

  typedef void return_type;

  explicit async_result(completion_handler_type&)
  {
  }

  void get()
  {
  }
};

#endif // !defined(GENERATING_DOCUMENTATION)

} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#endif // EGT_ASIO_EXPERIMENTAL_IMPL_DETACHED_HPP
