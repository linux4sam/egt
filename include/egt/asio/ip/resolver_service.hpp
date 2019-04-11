//
// ip/resolver_service.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_IP_RESOLVER_SERVICE_HPP
#define EGT_ASIO_IP_RESOLVER_SERVICE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

#if defined(EGT_ASIO_ENABLE_OLD_SERVICES)

#include <egt/asio/async_result.hpp>
#include <egt/asio/error_code.hpp>
#include <egt/asio/io_context.hpp>
#include <egt/asio/ip/basic_resolver_iterator.hpp>
#include <egt/asio/ip/basic_resolver_query.hpp>
#include <egt/asio/ip/basic_resolver_results.hpp>

#if defined(EGT_ASIO_WINDOWS_RUNTIME)
# include <egt/asio/detail/winrt_resolver_service.hpp>
#else
# include <egt/asio/detail/resolver_service.hpp>
#endif

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace ip {

/// Default service implementation for a resolver.
template <typename InternetProtocol>
class resolver_service
#if defined(GENERATING_DOCUMENTATION)
  : public egt::asio::io_context::service
#else
  : public egt::asio::detail::service_base<
      resolver_service<InternetProtocol> >
#endif
{
public:
#if defined(GENERATING_DOCUMENTATION)
  /// The unique service identifier.
  static egt::asio::io_context::id id;
#endif

  /// The protocol type.
  typedef InternetProtocol protocol_type;

  /// The endpoint type.
  typedef typename InternetProtocol::endpoint endpoint_type;

  /// The query type.
  typedef basic_resolver_query<InternetProtocol> query_type;

  /// The iterator type.
  typedef basic_resolver_iterator<InternetProtocol> iterator_type;

  /// The results type.
  typedef basic_resolver_results<InternetProtocol> results_type;

private:
  // The type of the platform-specific implementation.
#if defined(EGT_ASIO_WINDOWS_RUNTIME)
  typedef egt::asio::detail::winrt_resolver_service<InternetProtocol>
    service_impl_type;
#else
  typedef egt::asio::detail::resolver_service<InternetProtocol>
    service_impl_type;
#endif

public:
  /// The type of a resolver implementation.
#if defined(GENERATING_DOCUMENTATION)
  typedef implementation_defined implementation_type;
#else
  typedef typename service_impl_type::implementation_type implementation_type;
#endif

  /// Construct a new resolver service for the specified io_context.
  explicit resolver_service(egt::asio::io_context& io_context)
    : egt::asio::detail::service_base<
        resolver_service<InternetProtocol> >(io_context),
      service_impl_(io_context)
  {
  }

  /// Construct a new resolver implementation.
  void construct(implementation_type& impl)
  {
    service_impl_.construct(impl);
  }

#if defined(EGT_ASIO_HAS_MOVE) || defined(GENERATING_DOCUMENTATION)
  /// Move-construct a new resolver implementation.
  void move_construct(implementation_type& impl,
      implementation_type& other_impl)
  {
    service_impl_.move_construct(impl, other_impl);
  }

  /// Move-assign from another resolver implementation.
  void move_assign(implementation_type& impl,
      resolver_service& other_service,
      implementation_type& other_impl)
  {
    service_impl_.move_assign(impl, other_service.service_impl_, other_impl);
  }
#endif // defined(EGT_ASIO_HAS_MOVE) || defined(GENERATING_DOCUMENTATION)

  /// Destroy a resolver implementation.
  void destroy(implementation_type& impl)
  {
    service_impl_.destroy(impl);
  }

  /// Cancel pending asynchronous operations.
  void cancel(implementation_type& impl)
  {
    service_impl_.cancel(impl);
  }

  /// Resolve a query to a list of entries.
  results_type resolve(implementation_type& impl, const query_type& query,
      egt::asio::error_code& ec)
  {
    return service_impl_.resolve(impl, query, ec);
  }

  /// Asynchronously resolve a query to a list of entries.
  template <typename ResolveHandler>
  EGT_ASIO_INITFN_RESULT_TYPE(ResolveHandler,
      void (egt::asio::error_code, results_type))
  async_resolve(implementation_type& impl, const query_type& query,
      EGT_ASIO_MOVE_ARG(ResolveHandler) handler)
  {
    egt::asio::async_completion<ResolveHandler,
      void (egt::asio::error_code, results_type)> init(handler);

    service_impl_.async_resolve(impl, query, init.completion_handler);

    return init.result.get();
  }

  /// Resolve an endpoint to a list of entries.
  results_type resolve(implementation_type& impl,
      const endpoint_type& endpoint, egt::asio::error_code& ec)
  {
    return service_impl_.resolve(impl, endpoint, ec);
  }

  /// Asynchronously resolve an endpoint to a list of entries.
  template <typename ResolveHandler>
  EGT_ASIO_INITFN_RESULT_TYPE(ResolveHandler,
      void (egt::asio::error_code, results_type))
  async_resolve(implementation_type& impl, const endpoint_type& endpoint,
      EGT_ASIO_MOVE_ARG(ResolveHandler) handler)
  {
    egt::asio::async_completion<ResolveHandler,
      void (egt::asio::error_code, results_type)> init(handler);

    service_impl_.async_resolve(impl, endpoint, init.completion_handler);

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
  service_impl_type service_impl_;
};

} // namespace ip
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#endif // defined(EGT_ASIO_ENABLE_OLD_SERVICES)

#endif // EGT_ASIO_IP_RESOLVER_SERVICE_HPP
