//
// impl/thread_pool.hpp
// ~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_IMPL_THREAD_POOL_HPP
#define EGT_ASIO_IMPL_THREAD_POOL_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/executor_op.hpp>
#include <egt/asio/detail/fenced_block.hpp>
#include <egt/asio/detail/recycling_allocator.hpp>
#include <egt/asio/detail/type_traits.hpp>
#include <egt/asio/execution_context.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {

inline thread_pool::executor_type
thread_pool::get_executor() EGT_ASIO_NOEXCEPT
{
  return executor_type(*this);
}

inline thread_pool&
thread_pool::executor_type::context() const EGT_ASIO_NOEXCEPT
{
  return pool_;
}

inline void
thread_pool::executor_type::on_work_started() const EGT_ASIO_NOEXCEPT
{
  pool_.scheduler_.work_started();
}

inline void thread_pool::executor_type::on_work_finished()
const EGT_ASIO_NOEXCEPT
{
  pool_.scheduler_.work_finished();
}

template <typename Function, typename Allocator>
void thread_pool::executor_type::dispatch(
    EGT_ASIO_MOVE_ARG(Function) f, const Allocator& a) const
{
  typedef typename decay<Function>::type function_type;

  // Invoke immediately if we are already inside the thread pool.
  if (pool_.scheduler_.can_dispatch())
  {
    // Make a local, non-const copy of the function.
    function_type tmp(EGT_ASIO_MOVE_CAST(Function)(f));

    detail::fenced_block b(detail::fenced_block::full);
    egt_asio_handler_invoke_helpers::invoke(tmp, tmp);
    return;
  }

  // Allocate and construct an operation to wrap the function.
  typedef detail::executor_op<function_type, Allocator> op;
  typename op::ptr p = { detail::addressof(a), op::ptr::allocate(a), 0 };
  p.p = new (p.v) op(EGT_ASIO_MOVE_CAST(Function)(f), a);

  EGT_ASIO_HANDLER_CREATION((pool_, *p.p,
        "thread_pool", &this->context(), 0, "dispatch"));

  pool_.scheduler_.post_immediate_completion(p.p, false);
  p.v = p.p = 0;
}

template <typename Function, typename Allocator>
void thread_pool::executor_type::post(
    EGT_ASIO_MOVE_ARG(Function) f, const Allocator& a) const
{
  typedef typename decay<Function>::type function_type;

  // Allocate and construct an operation to wrap the function.
  typedef detail::executor_op<function_type, Allocator> op;
  typename op::ptr p = { detail::addressof(a), op::ptr::allocate(a), 0 };
  p.p = new (p.v) op(EGT_ASIO_MOVE_CAST(Function)(f), a);

  EGT_ASIO_HANDLER_CREATION((pool_, *p.p,
        "thread_pool", &this->context(), 0, "post"));

  pool_.scheduler_.post_immediate_completion(p.p, false);
  p.v = p.p = 0;
}

template <typename Function, typename Allocator>
void thread_pool::executor_type::defer(
    EGT_ASIO_MOVE_ARG(Function) f, const Allocator& a) const
{
  typedef typename decay<Function>::type function_type;

  // Allocate and construct an operation to wrap the function.
  typedef detail::executor_op<function_type, Allocator> op;
  typename op::ptr p = { detail::addressof(a), op::ptr::allocate(a), 0 };
  p.p = new (p.v) op(EGT_ASIO_MOVE_CAST(Function)(f), a);

  EGT_ASIO_HANDLER_CREATION((pool_, *p.p,
        "thread_pool", &this->context(), 0, "defer"));

  pool_.scheduler_.post_immediate_completion(p.p, true);
  p.v = p.p = 0;
}

inline bool
thread_pool::executor_type::running_in_this_thread() const EGT_ASIO_NOEXCEPT
{
  return pool_.scheduler_.can_dispatch();
}

} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#endif // EGT_ASIO_IMPL_THREAD_POOL_HPP
