//
// signal_set_service.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_SIGNAL_SET_SERVICE_HPP
#define EGT_ASIO_SIGNAL_SET_SERVICE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

#if defined(EGT_ASIO_ENABLE_OLD_SERVICES)

#include <egt/asio/async_result.hpp>
#include <egt/asio/detail/signal_set_service.hpp>
#include <egt/asio/error.hpp>
#include <egt/asio/io_context.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {

/// Default service implementation for a signal set.
class signal_set_service
#if defined(GENERATING_DOCUMENTATION)
  : public egt::asio::io_context::service
#else
  : public egt::asio::detail::service_base<signal_set_service>
#endif
{
public:
#if defined(GENERATING_DOCUMENTATION)
  /// The unique service identifier.
  static egt::asio::io_context::id id;
#endif

public:
  /// The type of a signal set implementation.
#if defined(GENERATING_DOCUMENTATION)
  typedef implementation_defined implementation_type;
#else
  typedef detail::signal_set_service::implementation_type implementation_type;
#endif

  /// Construct a new signal set service for the specified io_context.
  explicit signal_set_service(egt::asio::io_context& io_context)
    : egt::asio::detail::service_base<signal_set_service>(io_context),
      service_impl_(io_context)
  {
  }

  /// Construct a new signal set implementation.
  void construct(implementation_type& impl)
  {
    service_impl_.construct(impl);
  }

  /// Destroy a signal set implementation.
  void destroy(implementation_type& impl)
  {
    service_impl_.destroy(impl);
  }

  /// Add a signal to a signal_set.
  EGT_ASIO_SYNC_OP_VOID add(implementation_type& impl,
      int signal_number, egt::asio::error_code& ec)
  {
    service_impl_.add(impl, signal_number, ec);
    EGT_ASIO_SYNC_OP_VOID_RETURN(ec);
  }

  /// Remove a signal to a signal_set.
  EGT_ASIO_SYNC_OP_VOID remove(implementation_type& impl,
      int signal_number, egt::asio::error_code& ec)
  {
    service_impl_.remove(impl, signal_number, ec);
    EGT_ASIO_SYNC_OP_VOID_RETURN(ec);
  }

  /// Remove all signals from a signal_set.
  EGT_ASIO_SYNC_OP_VOID clear(implementation_type& impl,
      egt::asio::error_code& ec)
  {
    service_impl_.clear(impl, ec);
    EGT_ASIO_SYNC_OP_VOID_RETURN(ec);
  }

  /// Cancel all operations associated with the signal set.
  EGT_ASIO_SYNC_OP_VOID cancel(implementation_type& impl,
      egt::asio::error_code& ec)
  {
    service_impl_.cancel(impl, ec);
    EGT_ASIO_SYNC_OP_VOID_RETURN(ec);
  }

  // Start an asynchronous operation to wait for a signal to be delivered.
  template <typename SignalHandler>
  EGT_ASIO_INITFN_RESULT_TYPE(SignalHandler,
      void (egt::asio::error_code, int))
  async_wait(implementation_type& impl,
      EGT_ASIO_MOVE_ARG(SignalHandler) handler)
  {
    async_completion<SignalHandler,
      void (egt::asio::error_code, int)> init(handler);

    service_impl_.async_wait(impl, init.completion_handler);

    return init.result.get();
  }

private:
  // Destroy all user-defined handler objects owned by the service.
  void shutdown()
  {
    service_impl_.shutdown();
  }

  // Perform any fork-related housekeeping.
  void notify_fork(egt::asio::io_context::fork_event event)
  {
    service_impl_.notify_fork(event);
  }

  // The platform-specific implementation.
  detail::signal_set_service service_impl_;
};

} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#endif // defined(EGT_ASIO_ENABLE_OLD_SERVICES)

#endif // EGT_ASIO_SIGNAL_SET_SERVICE_HPP
