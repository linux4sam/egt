//
// detail/resolver_service_base.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_DETAIL_RESOLVER_SERVICE_BASE_HPP
#define EGT_ASIO_DETAIL_RESOLVER_SERVICE_BASE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>
#include <egt/asio/error.hpp>
#include <egt/asio/executor_work_guard.hpp>
#include <egt/asio/io_context.hpp>
#include <egt/asio/detail/mutex.hpp>
#include <egt/asio/detail/noncopyable.hpp>
#include <egt/asio/detail/resolve_op.hpp>
#include <egt/asio/detail/socket_ops.hpp>
#include <egt/asio/detail/socket_types.hpp>
#include <egt/asio/detail/scoped_ptr.hpp>
#include <egt/asio/detail/thread.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace detail {

class resolver_service_base
{
public:
  // The implementation type of the resolver. A cancellation token is used to
  // indicate to the background thread that the operation has been cancelled.
  typedef socket_ops::shared_cancel_token_type implementation_type;

  // Constructor.
  EGT_ASIO_DECL resolver_service_base(egt::asio::io_context& io_context);

  // Destructor.
  EGT_ASIO_DECL ~resolver_service_base();

  // Destroy all user-defined handler objects owned by the service.
  EGT_ASIO_DECL void base_shutdown();

  // Perform any fork-related housekeeping.
  EGT_ASIO_DECL void base_notify_fork(
      egt::asio::io_context::fork_event fork_ev);

  // Construct a new resolver implementation.
  EGT_ASIO_DECL void construct(implementation_type& impl);

  // Destroy a resolver implementation.
  EGT_ASIO_DECL void destroy(implementation_type&);

  // Move-construct a new resolver implementation.
  EGT_ASIO_DECL void move_construct(implementation_type& impl,
      implementation_type& other_impl);

  // Move-assign from another resolver implementation.
  EGT_ASIO_DECL void move_assign(implementation_type& impl,
      resolver_service_base& other_service,
      implementation_type& other_impl);

  // Cancel pending asynchronous operations.
  EGT_ASIO_DECL void cancel(implementation_type& impl);

protected:
  // Helper function to start an asynchronous resolve operation.
  EGT_ASIO_DECL void start_resolve_op(resolve_op* op);

#if !defined(EGT_ASIO_WINDOWS_RUNTIME)
  // Helper class to perform exception-safe cleanup of addrinfo objects.
  class auto_addrinfo
    : private egt::asio::detail::noncopyable
  {
  public:
    explicit auto_addrinfo(egt::asio::detail::addrinfo_type* ai)
      : ai_(ai)
    {
    }

    ~auto_addrinfo()
    {
      if (ai_)
        socket_ops::freeaddrinfo(ai_);
    }

    operator egt::asio::detail::addrinfo_type*()
    {
      return ai_;
    }

  private:
    egt::asio::detail::addrinfo_type* ai_;
  };
#endif // !defined(EGT_ASIO_WINDOWS_RUNTIME)

  // Helper class to run the work io_context in a thread.
  class work_io_context_runner;

  // Start the work thread if it's not already running.
  EGT_ASIO_DECL void start_work_thread();

  // The io_context implementation used to post completions.
  io_context_impl& io_context_impl_;

private:
  // Mutex to protect access to internal data.
  egt::asio::detail::mutex mutex_;

  // Private io_context used for performing asynchronous host resolution.
  egt::asio::detail::scoped_ptr<egt::asio::io_context> work_io_context_;

  // The work io_context implementation used to post completions.
  io_context_impl& work_io_context_impl_;

  // Work for the private io_context to perform.
  egt::asio::executor_work_guard<
      egt::asio::io_context::executor_type> work_;

  // Thread used for running the work io_context's run loop.
  egt::asio::detail::scoped_ptr<egt::asio::detail::thread> work_thread_;
};

} // namespace detail
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#if defined(EGT_ASIO_HEADER_ONLY)
# include <egt/asio/detail/impl/resolver_service_base.ipp>
#endif // defined(EGT_ASIO_HEADER_ONLY)

#endif // EGT_ASIO_DETAIL_RESOLVER_SERVICE_BASE_HPP
