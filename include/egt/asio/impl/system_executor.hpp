//
// impl/system_executor.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_IMPL_SYSTEM_EXECUTOR_HPP
#define EGT_ASIO_IMPL_SYSTEM_EXECUTOR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/executor_op.hpp>
#include <egt/asio/detail/global.hpp>
#include <egt/asio/detail/recycling_allocator.hpp>
#include <egt/asio/detail/type_traits.hpp>
#include <egt/asio/system_context.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {

inline system_context& system_executor::context() const EGT_ASIO_NOEXCEPT
{
  return detail::global<system_context>();
}

template <typename Function, typename Allocator>
void system_executor::dispatch(
    EGT_ASIO_MOVE_ARG(Function) f, const Allocator&) const
{
  typename decay<Function>::type tmp(EGT_ASIO_MOVE_CAST(Function)(f));
  egt_asio_handler_invoke_helpers::invoke(tmp, tmp);
}

template <typename Function, typename Allocator>
void system_executor::post(
    EGT_ASIO_MOVE_ARG(Function) f, const Allocator& a) const
{
  typedef typename decay<Function>::type function_type;

  system_context& ctx = detail::global<system_context>();

  // Allocate and construct an operation to wrap the function.
  typedef detail::executor_op<function_type, Allocator> op;
  typename op::ptr p = { detail::addressof(a), op::ptr::allocate(a), 0 };
  p.p = new (p.v) op(EGT_ASIO_MOVE_CAST(Function)(f), a);

  EGT_ASIO_HANDLER_CREATION((ctx, *p.p,
        "system_executor", &this->context(), 0, "post"));

  ctx.scheduler_.post_immediate_completion(p.p, false);
  p.v = p.p = 0;
}

template <typename Function, typename Allocator>
void system_executor::defer(
    EGT_ASIO_MOVE_ARG(Function) f, const Allocator& a) const
{
  typedef typename decay<Function>::type function_type;

  system_context& ctx = detail::global<system_context>();

  // Allocate and construct an operation to wrap the function.
  typedef detail::executor_op<function_type, Allocator> op;
  typename op::ptr p = { detail::addressof(a), op::ptr::allocate(a), 0 };
  p.p = new (p.v) op(EGT_ASIO_MOVE_CAST(Function)(f), a);

  EGT_ASIO_HANDLER_CREATION((ctx, *p.p,
        "system_executor", &this->context(), 0, "defer"));

  ctx.scheduler_.post_immediate_completion(p.p, true);
  p.v = p.p = 0;
}

} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#endif // EGT_ASIO_IMPL_SYSTEM_EXECUTOR_HPP
