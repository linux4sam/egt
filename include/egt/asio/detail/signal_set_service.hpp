//
// detail/signal_set_service.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_DETAIL_SIGNAL_SET_SERVICE_HPP
#define EGT_ASIO_DETAIL_SIGNAL_SET_SERVICE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

#include <cstddef>
#include <signal.h>
#include <egt/asio/error.hpp>
#include <egt/asio/io_context.hpp>
#include <egt/asio/detail/handler_alloc_helpers.hpp>
#include <egt/asio/detail/memory.hpp>
#include <egt/asio/detail/op_queue.hpp>
#include <egt/asio/detail/signal_handler.hpp>
#include <egt/asio/detail/signal_op.hpp>
#include <egt/asio/detail/socket_types.hpp>

#if !defined(EGT_ASIO_WINDOWS) && !defined(__CYGWIN__)
# include <egt/asio/detail/reactor.hpp>
#endif // !defined(EGT_ASIO_WINDOWS) && !defined(__CYGWIN__)

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace detail {

#if defined(NSIG) && (NSIG > 0)
enum { max_signal_number = NSIG };
#else
enum { max_signal_number = 128 };
#endif

extern EGT_ASIO_DECL struct signal_state* get_signal_state();

extern "C" EGT_ASIO_DECL void egt_asio_signal_handler(int signal_number);

class signal_set_service :
  public service_base<signal_set_service>
{
public:
  // Type used for tracking an individual signal registration.
  class registration
  {
  public:
    // Default constructor.
    registration()
      : signal_number_(0),
        queue_(0),
        undelivered_(0),
        next_in_table_(0),
        prev_in_table_(0),
        next_in_set_(0)
    {
    }

  private:
    // Only this service will have access to the internal values.
    friend class signal_set_service;

    // The signal number that is registered.
    int signal_number_;

    // The waiting signal handlers.
    op_queue<signal_op>* queue_;

    // The number of undelivered signals.
    std::size_t undelivered_;

    // Pointers to adjacent registrations in the registrations_ table.
    registration* next_in_table_;
    registration* prev_in_table_;

    // Link to next registration in the signal set.
    registration* next_in_set_;
  };

  // The implementation type of the signal_set.
  class implementation_type
  {
  public:
    // Default constructor.
    implementation_type()
      : signals_(0)
    {
    }

  private:
    // Only this service will have access to the internal values.
    friend class signal_set_service;

    // The pending signal handlers.
    op_queue<signal_op> queue_;

    // Linked list of registered signals.
    registration* signals_;
  };

  // Constructor.
  EGT_ASIO_DECL signal_set_service(egt::asio::io_context& io_context);

  // Destructor.
  EGT_ASIO_DECL ~signal_set_service();

  // Destroy all user-defined handler objects owned by the service.
  EGT_ASIO_DECL void shutdown();

  // Perform fork-related housekeeping.
  EGT_ASIO_DECL void notify_fork(
      egt::asio::io_context::fork_event fork_ev);

  // Construct a new signal_set implementation.
  EGT_ASIO_DECL void construct(implementation_type& impl);

  // Destroy a signal_set implementation.
  EGT_ASIO_DECL void destroy(implementation_type& impl);

  // Add a signal to a signal_set.
  EGT_ASIO_DECL egt::asio::error_code add(implementation_type& impl,
      int signal_number, egt::asio::error_code& ec);

  // Remove a signal to a signal_set.
  EGT_ASIO_DECL egt::asio::error_code remove(implementation_type& impl,
      int signal_number, egt::asio::error_code& ec);

  // Remove all signals from a signal_set.
  EGT_ASIO_DECL egt::asio::error_code clear(implementation_type& impl,
      egt::asio::error_code& ec);

  // Cancel all operations associated with the signal set.
  EGT_ASIO_DECL egt::asio::error_code cancel(implementation_type& impl,
      egt::asio::error_code& ec);

  // Start an asynchronous operation to wait for a signal to be delivered.
  template <typename Handler>
  void async_wait(implementation_type& impl, Handler& handler)
  {
    // Allocate and construct an operation to wrap the handler.
    typedef signal_handler<Handler> op;
    typename op::ptr p = { egt::asio::detail::addressof(handler),
      op::ptr::allocate(handler), 0 };
    p.p = new (p.v) op(handler);

    EGT_ASIO_HANDLER_CREATION((io_context_.context(),
          *p.p, "signal_set", &impl, 0, "async_wait"));

    start_wait_op(impl, p.p);
    p.v = p.p = 0;
  }

  // Deliver notification that a particular signal occurred.
  EGT_ASIO_DECL static void deliver_signal(int signal_number);

private:
  // Helper function to add a service to the global signal state.
  EGT_ASIO_DECL static void add_service(signal_set_service* service);

  // Helper function to remove a service from the global signal state.
  EGT_ASIO_DECL static void remove_service(signal_set_service* service);

  // Helper function to create the pipe descriptors.
  EGT_ASIO_DECL static void open_descriptors();

  // Helper function to close the pipe descriptors.
  EGT_ASIO_DECL static void close_descriptors();

  // Helper function to start a wait operation.
  EGT_ASIO_DECL void start_wait_op(implementation_type& impl, signal_op* op);

  // The io_context instance used for dispatching handlers.
  io_context_impl& io_context_;

#if !defined(EGT_ASIO_WINDOWS) \
  && !defined(EGT_ASIO_WINDOWS_RUNTIME) \
  && !defined(__CYGWIN__)
  // The type used for registering for pipe reactor notifications.
  class pipe_read_op;

  // The reactor used for waiting for pipe readiness.
  reactor& reactor_;

  // The per-descriptor reactor data used for the pipe.
  reactor::per_descriptor_data reactor_data_;
#endif // !defined(EGT_ASIO_WINDOWS)
       //   && !defined(EGT_ASIO_WINDOWS_RUNTIME)
       //   && !defined(__CYGWIN__)

  // A mapping from signal number to the registered signal sets.
  registration* registrations_[max_signal_number];

  // Pointers to adjacent services in linked list.
  signal_set_service* next_;
  signal_set_service* prev_;
};

} // namespace detail
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#if defined(EGT_ASIO_HEADER_ONLY)
# include <egt/asio/detail/impl/signal_set_service.ipp>
#endif // defined(EGT_ASIO_HEADER_ONLY)

#endif // EGT_ASIO_DETAIL_SIGNAL_SET_SERVICE_HPP
