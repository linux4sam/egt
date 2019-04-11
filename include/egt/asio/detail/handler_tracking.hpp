//
// detail/handler_tracking.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_DETAIL_HANDLER_TRACKING_HPP
#define EGT_ASIO_DETAIL_HANDLER_TRACKING_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

namespace egt {
namespace asio {

class execution_context;

} // namespace asio
} // namespace egt

#if defined(EGT_ASIO_CUSTOM_HANDLER_TRACKING)
# include EGT_ASIO_CUSTOM_HANDLER_TRACKING
#elif defined(EGT_ASIO_ENABLE_HANDLER_TRACKING)
# include <egt/asio/error_code.hpp>
# include <egt/asio/detail/cstdint.hpp>
# include <egt/asio/detail/static_mutex.hpp>
# include <egt/asio/detail/tss_ptr.hpp>
#endif // defined(EGT_ASIO_ENABLE_HANDLER_TRACKING)

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace detail {

#if defined(EGT_ASIO_CUSTOM_HANDLER_TRACKING)

// The user-specified header must define the following macros:
// - EGT_ASIO_INHERIT_TRACKED_HANDLER
// - EGT_ASIO_ALSO_INHERIT_TRACKED_HANDLER
// - EGT_ASIO_HANDLER_TRACKING_INIT
// - EGT_ASIO_HANDLER_CREATION(args)
// - EGT_ASIO_HANDLER_COMPLETION(args)
// - EGT_ASIO_HANDLER_INVOCATION_BEGIN(args)
// - EGT_ASIO_HANDLER_INVOCATION_END
// - EGT_ASIO_HANDLER_OPERATION(args)
// - EGT_ASIO_HANDLER_REACTOR_REGISTRATION(args)
// - EGT_ASIO_HANDLER_REACTOR_DEREGISTRATION(args)
// - EGT_ASIO_HANDLER_REACTOR_READ_EVENT
// - EGT_ASIO_HANDLER_REACTOR_WRITE_EVENT
// - EGT_ASIO_HANDLER_REACTOR_ERROR_EVENT
// - EGT_ASIO_HANDLER_REACTOR_EVENTS(args)
// - EGT_ASIO_HANDLER_REACTOR_OPERATION(args)

# if !defined(EGT_ASIO_ENABLE_HANDLER_TRACKING)
#  define EGT_ASIO_ENABLE_HANDLER_TRACKING 1
# endif /// !defined(EGT_ASIO_ENABLE_HANDLER_TRACKING)

#elif defined(EGT_ASIO_ENABLE_HANDLER_TRACKING)

class handler_tracking
{
public:
  class completion;

  // Base class for objects containing tracked handlers.
  class tracked_handler
  {
  private:
    // Only the handler_tracking class will have access to the id.
    friend class handler_tracking;
    friend class completion;
    uint64_t id_;

  protected:
    // Constructor initialises with no id.
    tracked_handler() : id_(0) {}

    // Prevent deletion through this type.
    ~tracked_handler() {}
  };

  // Initialise the tracking system.
  EGT_ASIO_DECL static void init();

  // Record the creation of a tracked handler.
  EGT_ASIO_DECL static void creation(
      execution_context& context, tracked_handler& h,
      const char* object_type, void* object,
      uintmax_t native_handle, const char* op_name);

  class completion
  {
  public:
    // Constructor records that handler is to be invoked with no arguments.
    EGT_ASIO_DECL explicit completion(const tracked_handler& h);

    // Destructor records only when an exception is thrown from the handler, or
    // if the memory is being freed without the handler having been invoked.
    EGT_ASIO_DECL ~completion();

    // Records that handler is to be invoked with no arguments.
    EGT_ASIO_DECL void invocation_begin();

    // Records that handler is to be invoked with one arguments.
    EGT_ASIO_DECL void invocation_begin(const egt::asio::error_code& ec);

    // Constructor records that handler is to be invoked with two arguments.
    EGT_ASIO_DECL void invocation_begin(
        const egt::asio::error_code& ec, std::size_t bytes_transferred);

    // Constructor records that handler is to be invoked with two arguments.
    EGT_ASIO_DECL void invocation_begin(
        const egt::asio::error_code& ec, int signal_number);

    // Constructor records that handler is to be invoked with two arguments.
    EGT_ASIO_DECL void invocation_begin(
        const egt::asio::error_code& ec, const char* arg);

    // Record that handler invocation has ended.
    EGT_ASIO_DECL void invocation_end();

  private:
    friend class handler_tracking;
    uint64_t id_;
    bool invoked_;
    completion* next_;
  };

  // Record an operation that is not directly associated with a handler.
  EGT_ASIO_DECL static void operation(execution_context& context,
      const char* object_type, void* object,
      uintmax_t native_handle, const char* op_name);

  // Record that a descriptor has been registered with the reactor.
  EGT_ASIO_DECL static void reactor_registration(execution_context& context,
      uintmax_t native_handle, uintmax_t registration);

  // Record that a descriptor has been deregistered from the reactor.
  EGT_ASIO_DECL static void reactor_deregistration(execution_context& context,
      uintmax_t native_handle, uintmax_t registration);

  // Record a reactor-based operation that is associated with a handler.
  EGT_ASIO_DECL static void reactor_events(execution_context& context,
      uintmax_t registration, unsigned events);

  // Record a reactor-based operation that is associated with a handler.
  EGT_ASIO_DECL static void reactor_operation(
      const tracked_handler& h, const char* op_name,
      const egt::asio::error_code& ec);

  // Record a reactor-based operation that is associated with a handler.
  EGT_ASIO_DECL static void reactor_operation(
      const tracked_handler& h, const char* op_name,
      const egt::asio::error_code& ec, std::size_t bytes_transferred);

  // Write a line of output.
  EGT_ASIO_DECL static void write_line(const char* format, ...);

private:
  struct tracking_state;
  EGT_ASIO_DECL static tracking_state* get_state();
};

# define EGT_ASIO_INHERIT_TRACKED_HANDLER \
  : public egt::asio::detail::handler_tracking::tracked_handler

# define EGT_ASIO_ALSO_INHERIT_TRACKED_HANDLER \
  , public egt::asio::detail::handler_tracking::tracked_handler

# define EGT_ASIO_HANDLER_TRACKING_INIT \
  egt::asio::detail::handler_tracking::init()

# define EGT_ASIO_HANDLER_CREATION(args) \
  egt::asio::detail::handler_tracking::creation args

# define EGT_ASIO_HANDLER_COMPLETION(args) \
  egt::asio::detail::handler_tracking::completion tracked_completion args

# define EGT_ASIO_HANDLER_INVOCATION_BEGIN(args) \
  tracked_completion.invocation_begin args

# define EGT_ASIO_HANDLER_INVOCATION_END \
  tracked_completion.invocation_end()

# define EGT_ASIO_HANDLER_OPERATION(args) \
  egt::asio::detail::handler_tracking::operation args

# define EGT_ASIO_HANDLER_REACTOR_REGISTRATION(args) \
  egt::asio::detail::handler_tracking::reactor_registration args

# define EGT_ASIO_HANDLER_REACTOR_DEREGISTRATION(args) \
  egt::asio::detail::handler_tracking::reactor_deregistration args

# define EGT_ASIO_HANDLER_REACTOR_READ_EVENT 1
# define EGT_ASIO_HANDLER_REACTOR_WRITE_EVENT 2
# define EGT_ASIO_HANDLER_REACTOR_ERROR_EVENT 4

# define EGT_ASIO_HANDLER_REACTOR_EVENTS(args) \
  egt::asio::detail::handler_tracking::reactor_events args

# define EGT_ASIO_HANDLER_REACTOR_OPERATION(args) \
  egt::asio::detail::handler_tracking::reactor_operation args

#else // defined(EGT_ASIO_ENABLE_HANDLER_TRACKING)

# define EGT_ASIO_INHERIT_TRACKED_HANDLER
# define EGT_ASIO_ALSO_INHERIT_TRACKED_HANDLER
# define EGT_ASIO_HANDLER_TRACKING_INIT (void)0
# define EGT_ASIO_HANDLER_CREATION(args) (void)0
# define EGT_ASIO_HANDLER_COMPLETION(args) (void)0
# define EGT_ASIO_HANDLER_INVOCATION_BEGIN(args) (void)0
# define EGT_ASIO_HANDLER_INVOCATION_END (void)0
# define EGT_ASIO_HANDLER_OPERATION(args) (void)0
# define EGT_ASIO_HANDLER_REACTOR_REGISTRATION(args) (void)0
# define EGT_ASIO_HANDLER_REACTOR_DEREGISTRATION(args) (void)0
# define EGT_ASIO_HANDLER_REACTOR_READ_EVENT 0
# define EGT_ASIO_HANDLER_REACTOR_WRITE_EVENT 0
# define EGT_ASIO_HANDLER_REACTOR_ERROR_EVENT 0
# define EGT_ASIO_HANDLER_REACTOR_EVENTS(args) (void)0
# define EGT_ASIO_HANDLER_REACTOR_OPERATION(args) (void)0

#endif // defined(EGT_ASIO_ENABLE_HANDLER_TRACKING)

} // namespace detail
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#if defined(EGT_ASIO_HEADER_ONLY)
# include <egt/asio/detail/impl/handler_tracking.ipp>
#endif // defined(EGT_ASIO_HEADER_ONLY)

#endif // EGT_ASIO_DETAIL_HANDLER_TRACKING_HPP
