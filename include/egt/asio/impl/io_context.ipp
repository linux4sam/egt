//
// impl/io_context.ipp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_IMPL_IO_CONTEXT_IPP
#define EGT_ASIO_IMPL_IO_CONTEXT_IPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>
#include <egt/asio/io_context.hpp>
#include <egt/asio/detail/concurrency_hint.hpp>
#include <egt/asio/detail/limits.hpp>
#include <egt/asio/detail/scoped_ptr.hpp>
#include <egt/asio/detail/service_registry.hpp>
#include <egt/asio/detail/throw_error.hpp>

#if defined(EGT_ASIO_HAS_IOCP)
# include <egt/asio/detail/win_iocp_io_context.hpp>
#else
# include <egt/asio/detail/scheduler.hpp>
#endif

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {

io_context::io_context()
  : impl_(add_impl(new impl_type(*this, EGT_ASIO_CONCURRENCY_HINT_DEFAULT)))
{
}

io_context::io_context(int concurrency_hint)
  : impl_(add_impl(new impl_type(*this, concurrency_hint == 1
          ? EGT_ASIO_CONCURRENCY_HINT_1 : concurrency_hint)))
{
}

io_context::impl_type& io_context::add_impl(io_context::impl_type* impl)
{
  egt::asio::detail::scoped_ptr<impl_type> scoped_impl(impl);
  egt::asio::add_service<impl_type>(*this, scoped_impl.get());
  return *scoped_impl.release();
}

io_context::~io_context()
{
}

io_context::count_type io_context::run()
{
  egt::asio::error_code ec;
  count_type s = impl_.run(ec);
  egt::asio::detail::throw_error(ec);
  return s;
}

#if !defined(EGT_ASIO_NO_DEPRECATED)
io_context::count_type io_context::run(egt::asio::error_code& ec)
{
  return impl_.run(ec);
}
#endif // !defined(EGT_ASIO_NO_DEPRECATED)

io_context::count_type io_context::run_one()
{
  egt::asio::error_code ec;
  count_type s = impl_.run_one(ec);
  egt::asio::detail::throw_error(ec);
  return s;
}

#if !defined(EGT_ASIO_NO_DEPRECATED)
io_context::count_type io_context::run_one(egt::asio::error_code& ec)
{
  return impl_.run_one(ec);
}
#endif // !defined(EGT_ASIO_NO_DEPRECATED)

io_context::count_type io_context::poll()
{
  egt::asio::error_code ec;
  count_type s = impl_.poll(ec);
  egt::asio::detail::throw_error(ec);
  return s;
}

#if !defined(EGT_ASIO_NO_DEPRECATED)
io_context::count_type io_context::poll(egt::asio::error_code& ec)
{
  return impl_.poll(ec);
}
#endif // !defined(EGT_ASIO_NO_DEPRECATED)

io_context::count_type io_context::poll_one()
{
  egt::asio::error_code ec;
  count_type s = impl_.poll_one(ec);
  egt::asio::detail::throw_error(ec);
  return s;
}

#if !defined(EGT_ASIO_NO_DEPRECATED)
io_context::count_type io_context::poll_one(egt::asio::error_code& ec)
{
  return impl_.poll_one(ec);
}
#endif // !defined(EGT_ASIO_NO_DEPRECATED)

void io_context::stop()
{
  impl_.stop();
}

bool io_context::stopped() const
{
  return impl_.stopped();
}

void io_context::restart()
{
  impl_.restart();
}

io_context::service::service(egt::asio::io_context& owner)
  : execution_context::service(owner)
{
}

io_context::service::~service()
{
}

void io_context::service::shutdown()
{
#if !defined(EGT_ASIO_NO_DEPRECATED)
  shutdown_service();
#endif // !defined(EGT_ASIO_NO_DEPRECATED)
}

#if !defined(EGT_ASIO_NO_DEPRECATED)
void io_context::service::shutdown_service()
{
}
#endif // !defined(EGT_ASIO_NO_DEPRECATED)

void io_context::service::notify_fork(io_context::fork_event ev)
{
#if !defined(EGT_ASIO_NO_DEPRECATED)
  fork_service(ev);
#else // !defined(EGT_ASIO_NO_DEPRECATED)
  (void)ev;
#endif // !defined(EGT_ASIO_NO_DEPRECATED)
}

#if !defined(EGT_ASIO_NO_DEPRECATED)
void io_context::service::fork_service(io_context::fork_event)
{
}
#endif // !defined(EGT_ASIO_NO_DEPRECATED)

} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#endif // EGT_ASIO_IMPL_IO_CONTEXT_IPP
