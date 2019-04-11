//
// ip/basic_resolver.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_IP_BASIC_RESOLVER_HPP
#define EGT_ASIO_IP_BASIC_RESOLVER_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>
#include <string>
#include <egt/asio/async_result.hpp>
#include <egt/asio/basic_io_object.hpp>
#include <egt/asio/detail/handler_type_requirements.hpp>
#include <egt/asio/detail/string_view.hpp>
#include <egt/asio/detail/throw_error.hpp>
#include <egt/asio/error.hpp>
#include <egt/asio/io_context.hpp>
#include <egt/asio/ip/basic_resolver_iterator.hpp>
#include <egt/asio/ip/basic_resolver_query.hpp>
#include <egt/asio/ip/basic_resolver_results.hpp>
#include <egt/asio/ip/resolver_base.hpp>

#if defined(EGT_ASIO_HAS_MOVE)
# include <utility>
#endif // defined(EGT_ASIO_HAS_MOVE)

#if defined(EGT_ASIO_ENABLE_OLD_SERVICES)
# include <egt/asio/ip/resolver_service.hpp>
#else // defined(EGT_ASIO_ENABLE_OLD_SERVICES)
# if defined(EGT_ASIO_WINDOWS_RUNTIME)
#  include <egt/asio/detail/winrt_resolver_service.hpp>
#  define EGT_ASIO_SVC_T \
    egt::asio::detail::winrt_resolver_service<InternetProtocol>
# else
#  include <egt/asio/detail/resolver_service.hpp>
#  define EGT_ASIO_SVC_T \
    egt::asio::detail::resolver_service<InternetProtocol>
# endif
#endif // defined(EGT_ASIO_ENABLE_OLD_SERVICES)

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace ip {

/// Provides endpoint resolution functionality.
/**
 * The basic_resolver class template provides the ability to resolve a query
 * to a list of endpoints.
 *
 * @par Thread Safety
 * @e Distinct @e objects: Safe.@n
 * @e Shared @e objects: Unsafe.
 */
template <typename InternetProtocol
    EGT_ASIO_SVC_TPARAM_DEF1(= resolver_service<InternetProtocol>)>
class basic_resolver
  : EGT_ASIO_SVC_ACCESS basic_io_object<EGT_ASIO_SVC_T>,
    public resolver_base
{
public:
  /// The type of the executor associated with the object.
  typedef io_context::executor_type executor_type;

  /// The protocol type.
  typedef InternetProtocol protocol_type;

  /// The endpoint type.
  typedef typename InternetProtocol::endpoint endpoint_type;

#if !defined(EGT_ASIO_NO_DEPRECATED)
  /// (Deprecated.) The query type.
  typedef basic_resolver_query<InternetProtocol> query;

  /// (Deprecated.) The iterator type.
  typedef basic_resolver_iterator<InternetProtocol> iterator;
#endif // !defined(EGT_ASIO_NO_DEPRECATED)

  /// The results type.
  typedef basic_resolver_results<InternetProtocol> results_type;

  /// Constructor.
  /**
   * This constructor creates a basic_resolver.
   *
   * @param io_context The io_context object that the resolver will use to
   * dispatch handlers for any asynchronous operations performed on the
   * resolver.
   */
  explicit basic_resolver(egt::asio::io_context& io_context)
    : basic_io_object<EGT_ASIO_SVC_T>(io_context)
  {
  }

#if defined(EGT_ASIO_HAS_MOVE) || defined(GENERATING_DOCUMENTATION)
  /// Move-construct a basic_resolver from another.
  /**
   * This constructor moves a resolver from one object to another.
   *
   * @param other The other basic_resolver object from which the move will
   * occur.
   *
   * @note Following the move, the moved-from object is in the same state as if
   * constructed using the @c basic_resolver(io_context&) constructor.
   */
  basic_resolver(basic_resolver&& other)
    : basic_io_object<EGT_ASIO_SVC_T>(std::move(other))
  {
  }

  /// Move-assign a basic_resolver from another.
  /**
   * This assignment operator moves a resolver from one object to another.
   * Cancels any outstanding asynchronous operations associated with the target
   * object.
   *
   * @param other The other basic_resolver object from which the move will
   * occur.
   *
   * @note Following the move, the moved-from object is in the same state as if
   * constructed using the @c basic_resolver(io_context&) constructor.
   */
  basic_resolver& operator=(basic_resolver&& other)
  {
    basic_io_object<EGT_ASIO_SVC_T>::operator=(std::move(other));
    return *this;
  }
#endif // defined(EGT_ASIO_HAS_MOVE) || defined(GENERATING_DOCUMENTATION)

  /// Destroys the resolver.
  /**
   * This function destroys the resolver, cancelling any outstanding
   * asynchronous wait operations associated with the resolver as if by calling
   * @c cancel.
   */
  ~basic_resolver()
  {
  }

#if defined(EGT_ASIO_ENABLE_OLD_SERVICES)
  // These functions are provided by basic_io_object<>.
#else // defined(EGT_ASIO_ENABLE_OLD_SERVICES)
#if !defined(EGT_ASIO_NO_DEPRECATED)
  /// (Deprecated: Use get_executor().) Get the io_context associated with the
  /// object.
  /**
   * This function may be used to obtain the io_context object that the I/O
   * object uses to dispatch handlers for asynchronous operations.
   *
   * @return A reference to the io_context object that the I/O object will use
   * to dispatch handlers. Ownership is not transferred to the caller.
   */
  egt::asio::io_context& get_io_context()
  {
    return basic_io_object<EGT_ASIO_SVC_T>::get_io_context();
  }

  /// (Deprecated: Use get_executor().) Get the io_context associated with the
  /// object.
  /**
   * This function may be used to obtain the io_context object that the I/O
   * object uses to dispatch handlers for asynchronous operations.
   *
   * @return A reference to the io_context object that the I/O object will use
   * to dispatch handlers. Ownership is not transferred to the caller.
   */
  egt::asio::io_context& get_io_service()
  {
    return basic_io_object<EGT_ASIO_SVC_T>::get_io_service();
  }
#endif // !defined(EGT_ASIO_NO_DEPRECATED)

  /// Get the executor associated with the object.
  executor_type get_executor() EGT_ASIO_NOEXCEPT
  {
    return basic_io_object<EGT_ASIO_SVC_T>::get_executor();
  }
#endif // defined(EGT_ASIO_ENABLE_OLD_SERVICES)

  /// Cancel any asynchronous operations that are waiting on the resolver.
  /**
   * This function forces the completion of any pending asynchronous
   * operations on the host resolver. The handler for each cancelled operation
   * will be invoked with the egt::asio::error::operation_aborted error code.
   */
  void cancel()
  {
    return this->get_service().cancel(this->get_implementation());
  }

#if !defined(EGT_ASIO_NO_DEPRECATED)
  /// (Deprecated.) Perform forward resolution of a query to a list of entries.
  /**
   * This function is used to resolve a query into a list of endpoint entries.
   *
   * @param q A query object that determines what endpoints will be returned.
   *
   * @returns A range object representing the list of endpoint entries. A
   * successful call to this function is guaranteed to return a non-empty
   * range.
   *
   * @throws egt::asio::system_error Thrown on failure.
   */
  results_type resolve(const query& q)
  {
    egt::asio::error_code ec;
    results_type r = this->get_service().resolve(
        this->get_implementation(), q, ec);
    egt::asio::detail::throw_error(ec, "resolve");
    return r;
  }

  /// (Deprecated.) Perform forward resolution of a query to a list of entries.
  /**
   * This function is used to resolve a query into a list of endpoint entries.
   *
   * @param q A query object that determines what endpoints will be returned.
   *
   * @param ec Set to indicate what error occurred, if any.
   *
   * @returns A range object representing the list of endpoint entries. An
   * empty range is returned if an error occurs. A successful call to this
   * function is guaranteed to return a non-empty range.
   */
  results_type resolve(const query& q, egt::asio::error_code& ec)
  {
    return this->get_service().resolve(this->get_implementation(), q, ec);
  }
#endif // !defined(EGT_ASIO_NO_DEPRECATED)

  /// Perform forward resolution of a query to a list of entries.
  /**
   * This function is used to resolve host and service names into a list of
   * endpoint entries.
   *
   * @param host A string identifying a location. May be a descriptive name or
   * a numeric address string. If an empty string and the passive flag has been
   * specified, the resolved endpoints are suitable for local service binding.
   * If an empty string and passive is not specified, the resolved endpoints
   * will use the loopback address.
   *
   * @param service A string identifying the requested service. This may be a
   * descriptive name or a numeric string corresponding to a port number. May
   * be an empty string, in which case all resolved endpoints will have a port
   * number of 0.
   *
   * @returns A range object representing the list of endpoint entries. A
   * successful call to this function is guaranteed to return a non-empty
   * range.
   *
   * @throws egt::asio::system_error Thrown on failure.
   *
   * @note On POSIX systems, host names may be locally defined in the file
   * <tt>/etc/hosts</tt>. On Windows, host names may be defined in the file
   * <tt>c:\\windows\\system32\\drivers\\etc\\hosts</tt>. Remote host name
   * resolution is performed using DNS. Operating systems may use additional
   * locations when resolving host names (such as NETBIOS names on Windows).
   *
   * On POSIX systems, service names are typically defined in the file
   * <tt>/etc/services</tt>. On Windows, service names may be found in the file
   * <tt>c:\\windows\\system32\\drivers\\etc\\services</tt>. Operating systems
   * may use additional locations when resolving service names.
   */
  results_type resolve(EGT_ASIO_STRING_VIEW_PARAM host,
      EGT_ASIO_STRING_VIEW_PARAM service)
  {
    return resolve(host, service, resolver_base::flags());
  }

  /// Perform forward resolution of a query to a list of entries.
  /**
   * This function is used to resolve host and service names into a list of
   * endpoint entries.
   *
   * @param host A string identifying a location. May be a descriptive name or
   * a numeric address string. If an empty string and the passive flag has been
   * specified, the resolved endpoints are suitable for local service binding.
   * If an empty string and passive is not specified, the resolved endpoints
   * will use the loopback address.
   *
   * @param service A string identifying the requested service. This may be a
   * descriptive name or a numeric string corresponding to a port number. May
   * be an empty string, in which case all resolved endpoints will have a port
   * number of 0.
   *
   * @param ec Set to indicate what error occurred, if any.
   *
   * @returns A range object representing the list of endpoint entries. An
   * empty range is returned if an error occurs. A successful call to this
   * function is guaranteed to return a non-empty range.
   *
   * @note On POSIX systems, host names may be locally defined in the file
   * <tt>/etc/hosts</tt>. On Windows, host names may be defined in the file
   * <tt>c:\\windows\\system32\\drivers\\etc\\hosts</tt>. Remote host name
   * resolution is performed using DNS. Operating systems may use additional
   * locations when resolving host names (such as NETBIOS names on Windows).
   *
   * On POSIX systems, service names are typically defined in the file
   * <tt>/etc/services</tt>. On Windows, service names may be found in the file
   * <tt>c:\\windows\\system32\\drivers\\etc\\services</tt>. Operating systems
   * may use additional locations when resolving service names.
   */
  results_type resolve(EGT_ASIO_STRING_VIEW_PARAM host,
      EGT_ASIO_STRING_VIEW_PARAM service, egt::asio::error_code& ec)
  {
    return resolve(host, service, resolver_base::flags(), ec);
  }

  /// Perform forward resolution of a query to a list of entries.
  /**
   * This function is used to resolve host and service names into a list of
   * endpoint entries.
   *
   * @param host A string identifying a location. May be a descriptive name or
   * a numeric address string. If an empty string and the passive flag has been
   * specified, the resolved endpoints are suitable for local service binding.
   * If an empty string and passive is not specified, the resolved endpoints
   * will use the loopback address.
   *
   * @param service A string identifying the requested service. This may be a
   * descriptive name or a numeric string corresponding to a port number. May
   * be an empty string, in which case all resolved endpoints will have a port
   * number of 0.
   *
   * @param resolve_flags A set of flags that determine how name resolution
   * should be performed. The default flags are suitable for communication with
   * remote hosts.
   *
   * @returns A range object representing the list of endpoint entries. A
   * successful call to this function is guaranteed to return a non-empty
   * range.
   *
   * @throws egt::asio::system_error Thrown on failure.
   *
   * @note On POSIX systems, host names may be locally defined in the file
   * <tt>/etc/hosts</tt>. On Windows, host names may be defined in the file
   * <tt>c:\\windows\\system32\\drivers\\etc\\hosts</tt>. Remote host name
   * resolution is performed using DNS. Operating systems may use additional
   * locations when resolving host names (such as NETBIOS names on Windows).
   *
   * On POSIX systems, service names are typically defined in the file
   * <tt>/etc/services</tt>. On Windows, service names may be found in the file
   * <tt>c:\\windows\\system32\\drivers\\etc\\services</tt>. Operating systems
   * may use additional locations when resolving service names.
   */
  results_type resolve(EGT_ASIO_STRING_VIEW_PARAM host,
      EGT_ASIO_STRING_VIEW_PARAM service, resolver_base::flags resolve_flags)
  {
    egt::asio::error_code ec;
    basic_resolver_query<protocol_type> q(static_cast<std::string>(host),
        static_cast<std::string>(service), resolve_flags);
    results_type r = this->get_service().resolve(
        this->get_implementation(), q, ec);
    egt::asio::detail::throw_error(ec, "resolve");
    return r;
  }

  /// Perform forward resolution of a query to a list of entries.
  /**
   * This function is used to resolve host and service names into a list of
   * endpoint entries.
   *
   * @param host A string identifying a location. May be a descriptive name or
   * a numeric address string. If an empty string and the passive flag has been
   * specified, the resolved endpoints are suitable for local service binding.
   * If an empty string and passive is not specified, the resolved endpoints
   * will use the loopback address.
   *
   * @param service A string identifying the requested service. This may be a
   * descriptive name or a numeric string corresponding to a port number. May
   * be an empty string, in which case all resolved endpoints will have a port
   * number of 0.
   *
   * @param resolve_flags A set of flags that determine how name resolution
   * should be performed. The default flags are suitable for communication with
   * remote hosts.
   *
   * @param ec Set to indicate what error occurred, if any.
   *
   * @returns A range object representing the list of endpoint entries. An
   * empty range is returned if an error occurs. A successful call to this
   * function is guaranteed to return a non-empty range.
   *
   * @note On POSIX systems, host names may be locally defined in the file
   * <tt>/etc/hosts</tt>. On Windows, host names may be defined in the file
   * <tt>c:\\windows\\system32\\drivers\\etc\\hosts</tt>. Remote host name
   * resolution is performed using DNS. Operating systems may use additional
   * locations when resolving host names (such as NETBIOS names on Windows).
   *
   * On POSIX systems, service names are typically defined in the file
   * <tt>/etc/services</tt>. On Windows, service names may be found in the file
   * <tt>c:\\windows\\system32\\drivers\\etc\\services</tt>. Operating systems
   * may use additional locations when resolving service names.
   */
  results_type resolve(EGT_ASIO_STRING_VIEW_PARAM host,
      EGT_ASIO_STRING_VIEW_PARAM service, resolver_base::flags resolve_flags,
      egt::asio::error_code& ec)
  {
    basic_resolver_query<protocol_type> q(static_cast<std::string>(host),
        static_cast<std::string>(service), resolve_flags);
    return this->get_service().resolve(this->get_implementation(), q, ec);
  }

  /// Perform forward resolution of a query to a list of entries.
  /**
   * This function is used to resolve host and service names into a list of
   * endpoint entries.
   *
   * @param protocol A protocol object, normally representing either the IPv4 or
   * IPv6 version of an internet protocol.
   *
   * @param host A string identifying a location. May be a descriptive name or
   * a numeric address string. If an empty string and the passive flag has been
   * specified, the resolved endpoints are suitable for local service binding.
   * If an empty string and passive is not specified, the resolved endpoints
   * will use the loopback address.
   *
   * @param service A string identifying the requested service. This may be a
   * descriptive name or a numeric string corresponding to a port number. May
   * be an empty string, in which case all resolved endpoints will have a port
   * number of 0.
   *
   * @returns A range object representing the list of endpoint entries. A
   * successful call to this function is guaranteed to return a non-empty
   * range.
   *
   * @throws egt::asio::system_error Thrown on failure.
   *
   * @note On POSIX systems, host names may be locally defined in the file
   * <tt>/etc/hosts</tt>. On Windows, host names may be defined in the file
   * <tt>c:\\windows\\system32\\drivers\\etc\\hosts</tt>. Remote host name
   * resolution is performed using DNS. Operating systems may use additional
   * locations when resolving host names (such as NETBIOS names on Windows).
   *
   * On POSIX systems, service names are typically defined in the file
   * <tt>/etc/services</tt>. On Windows, service names may be found in the file
   * <tt>c:\\windows\\system32\\drivers\\etc\\services</tt>. Operating systems
   * may use additional locations when resolving service names.
   */
  results_type resolve(const protocol_type& protocol,
      EGT_ASIO_STRING_VIEW_PARAM host, EGT_ASIO_STRING_VIEW_PARAM service)
  {
    return resolve(protocol, host, service, resolver_base::flags());
  }

  /// Perform forward resolution of a query to a list of entries.
  /**
   * This function is used to resolve host and service names into a list of
   * endpoint entries.
   *
   * @param protocol A protocol object, normally representing either the IPv4 or
   * IPv6 version of an internet protocol.
   *
   * @param host A string identifying a location. May be a descriptive name or
   * a numeric address string. If an empty string and the passive flag has been
   * specified, the resolved endpoints are suitable for local service binding.
   * If an empty string and passive is not specified, the resolved endpoints
   * will use the loopback address.
   *
   * @param service A string identifying the requested service. This may be a
   * descriptive name or a numeric string corresponding to a port number. May
   * be an empty string, in which case all resolved endpoints will have a port
   * number of 0.
   *
   * @param ec Set to indicate what error occurred, if any.
   *
   * @returns A range object representing the list of endpoint entries. An
   * empty range is returned if an error occurs. A successful call to this
   * function is guaranteed to return a non-empty range.
   *
   * @note On POSIX systems, host names may be locally defined in the file
   * <tt>/etc/hosts</tt>. On Windows, host names may be defined in the file
   * <tt>c:\\windows\\system32\\drivers\\etc\\hosts</tt>. Remote host name
   * resolution is performed using DNS. Operating systems may use additional
   * locations when resolving host names (such as NETBIOS names on Windows).
   *
   * On POSIX systems, service names are typically defined in the file
   * <tt>/etc/services</tt>. On Windows, service names may be found in the file
   * <tt>c:\\windows\\system32\\drivers\\etc\\services</tt>. Operating systems
   * may use additional locations when resolving service names.
   */
  results_type resolve(const protocol_type& protocol,
      EGT_ASIO_STRING_VIEW_PARAM host, EGT_ASIO_STRING_VIEW_PARAM service,
      egt::asio::error_code& ec)
  {
    return resolve(protocol, host, service, resolver_base::flags(), ec);
  }

  /// Perform forward resolution of a query to a list of entries.
  /**
   * This function is used to resolve host and service names into a list of
   * endpoint entries.
   *
   * @param protocol A protocol object, normally representing either the IPv4 or
   * IPv6 version of an internet protocol.
   *
   * @param host A string identifying a location. May be a descriptive name or
   * a numeric address string. If an empty string and the passive flag has been
   * specified, the resolved endpoints are suitable for local service binding.
   * If an empty string and passive is not specified, the resolved endpoints
   * will use the loopback address.
   *
   * @param service A string identifying the requested service. This may be a
   * descriptive name or a numeric string corresponding to a port number. May
   * be an empty string, in which case all resolved endpoints will have a port
   * number of 0.
   *
   * @param resolve_flags A set of flags that determine how name resolution
   * should be performed. The default flags are suitable for communication with
   * remote hosts.
   *
   * @returns A range object representing the list of endpoint entries. A
   * successful call to this function is guaranteed to return a non-empty
   * range.
   *
   * @throws egt::asio::system_error Thrown on failure.
   *
   * @note On POSIX systems, host names may be locally defined in the file
   * <tt>/etc/hosts</tt>. On Windows, host names may be defined in the file
   * <tt>c:\\windows\\system32\\drivers\\etc\\hosts</tt>. Remote host name
   * resolution is performed using DNS. Operating systems may use additional
   * locations when resolving host names (such as NETBIOS names on Windows).
   *
   * On POSIX systems, service names are typically defined in the file
   * <tt>/etc/services</tt>. On Windows, service names may be found in the file
   * <tt>c:\\windows\\system32\\drivers\\etc\\services</tt>. Operating systems
   * may use additional locations when resolving service names.
   */
  results_type resolve(const protocol_type& protocol,
      EGT_ASIO_STRING_VIEW_PARAM host, EGT_ASIO_STRING_VIEW_PARAM service,
      resolver_base::flags resolve_flags)
  {
    egt::asio::error_code ec;
    basic_resolver_query<protocol_type> q(
        protocol, static_cast<std::string>(host),
        static_cast<std::string>(service), resolve_flags);
    results_type r = this->get_service().resolve(
        this->get_implementation(), q, ec);
    egt::asio::detail::throw_error(ec, "resolve");
    return r;
  }

  /// Perform forward resolution of a query to a list of entries.
  /**
   * This function is used to resolve host and service names into a list of
   * endpoint entries.
   *
   * @param protocol A protocol object, normally representing either the IPv4 or
   * IPv6 version of an internet protocol.
   *
   * @param host A string identifying a location. May be a descriptive name or
   * a numeric address string. If an empty string and the passive flag has been
   * specified, the resolved endpoints are suitable for local service binding.
   * If an empty string and passive is not specified, the resolved endpoints
   * will use the loopback address.
   *
   * @param service A string identifying the requested service. This may be a
   * descriptive name or a numeric string corresponding to a port number. May
   * be an empty string, in which case all resolved endpoints will have a port
   * number of 0.
   *
   * @param resolve_flags A set of flags that determine how name resolution
   * should be performed. The default flags are suitable for communication with
   * remote hosts.
   *
   * @param ec Set to indicate what error occurred, if any.
   *
   * @returns A range object representing the list of endpoint entries. An
   * empty range is returned if an error occurs. A successful call to this
   * function is guaranteed to return a non-empty range.
   *
   * @note On POSIX systems, host names may be locally defined in the file
   * <tt>/etc/hosts</tt>. On Windows, host names may be defined in the file
   * <tt>c:\\windows\\system32\\drivers\\etc\\hosts</tt>. Remote host name
   * resolution is performed using DNS. Operating systems may use additional
   * locations when resolving host names (such as NETBIOS names on Windows).
   *
   * On POSIX systems, service names are typically defined in the file
   * <tt>/etc/services</tt>. On Windows, service names may be found in the file
   * <tt>c:\\windows\\system32\\drivers\\etc\\services</tt>. Operating systems
   * may use additional locations when resolving service names.
   */
  results_type resolve(const protocol_type& protocol,
      EGT_ASIO_STRING_VIEW_PARAM host, EGT_ASIO_STRING_VIEW_PARAM service,
      resolver_base::flags resolve_flags, egt::asio::error_code& ec)
  {
    basic_resolver_query<protocol_type> q(
        protocol, static_cast<std::string>(host),
        static_cast<std::string>(service), resolve_flags);
    return this->get_service().resolve(this->get_implementation(), q, ec);
  }

#if !defined(EGT_ASIO_NO_DEPRECATED)
  /// (Deprecated.) Asynchronously perform forward resolution of a query to a
  /// list of entries.
  /**
   * This function is used to asynchronously resolve a query into a list of
   * endpoint entries.
   *
   * @param q A query object that determines what endpoints will be returned.
   *
   * @param handler The handler to be called when the resolve operation
   * completes. Copies will be made of the handler as required. The function
   * signature of the handler must be:
   * @code void handler(
   *   const egt::asio::error_code& error, // Result of operation.
   *   resolver::results_type results // Resolved endpoints as a range.
   * ); @endcode
   * Regardless of whether the asynchronous operation completes immediately or
   * not, the handler will not be invoked from within this function. Invocation
   * of the handler will be performed in a manner equivalent to using
   * egt::asio::io_context::post().
   *
   * A successful resolve operation is guaranteed to pass a non-empty range to
   * the handler.
   */
  template <typename ResolveHandler>
  EGT_ASIO_INITFN_RESULT_TYPE(ResolveHandler,
      void (egt::asio::error_code, results_type))
  async_resolve(const query& q,
      EGT_ASIO_MOVE_ARG(ResolveHandler) handler)
  {
    // If you get an error on the following line it means that your handler does
    // not meet the documented type requirements for a ResolveHandler.
    EGT_ASIO_RESOLVE_HANDLER_CHECK(
        ResolveHandler, handler, results_type) type_check;

#if defined(EGT_ASIO_ENABLE_OLD_SERVICES)
    return this->get_service().async_resolve(this->get_implementation(), q,
        EGT_ASIO_MOVE_CAST(ResolveHandler)(handler));
#else // defined(EGT_ASIO_ENABLE_OLD_SERVICES)
    egt::asio::async_completion<ResolveHandler,
      void (egt::asio::error_code, results_type)> init(handler);

    this->get_service().async_resolve(
        this->get_implementation(), q, init.completion_handler);

    return init.result.get();
#endif // defined(EGT_ASIO_ENABLE_OLD_SERVICES)
  }
#endif // !defined(EGT_ASIO_NO_DEPRECATED)

  /// Asynchronously perform forward resolution of a query to a list of entries.
  /**
   * This function is used to resolve host and service names into a list of
   * endpoint entries.
   *
   * @param host A string identifying a location. May be a descriptive name or
   * a numeric address string. If an empty string and the passive flag has been
   * specified, the resolved endpoints are suitable for local service binding.
   * If an empty string and passive is not specified, the resolved endpoints
   * will use the loopback address.
   *
   * @param service A string identifying the requested service. This may be a
   * descriptive name or a numeric string corresponding to a port number. May
   * be an empty string, in which case all resolved endpoints will have a port
   * number of 0.
   *
   * @param handler The handler to be called when the resolve operation
   * completes. Copies will be made of the handler as required. The function
   * signature of the handler must be:
   * @code void handler(
   *   const egt::asio::error_code& error, // Result of operation.
   *   resolver::results_type results // Resolved endpoints as a range.
   * ); @endcode
   * Regardless of whether the asynchronous operation completes immediately or
   * not, the handler will not be invoked from within this function. Invocation
   * of the handler will be performed in a manner equivalent to using
   * egt::asio::io_context::post().
   *
   * A successful resolve operation is guaranteed to pass a non-empty range to
   * the handler.
   *
   * @note On POSIX systems, host names may be locally defined in the file
   * <tt>/etc/hosts</tt>. On Windows, host names may be defined in the file
   * <tt>c:\\windows\\system32\\drivers\\etc\\hosts</tt>. Remote host name
   * resolution is performed using DNS. Operating systems may use additional
   * locations when resolving host names (such as NETBIOS names on Windows).
   *
   * On POSIX systems, service names are typically defined in the file
   * <tt>/etc/services</tt>. On Windows, service names may be found in the file
   * <tt>c:\\windows\\system32\\drivers\\etc\\services</tt>. Operating systems
   * may use additional locations when resolving service names.
   */
  template <typename ResolveHandler>
  EGT_ASIO_INITFN_RESULT_TYPE(ResolveHandler,
      void (egt::asio::error_code, results_type))
  async_resolve(EGT_ASIO_STRING_VIEW_PARAM host,
      EGT_ASIO_STRING_VIEW_PARAM service,
      EGT_ASIO_MOVE_ARG(ResolveHandler) handler)
  {
    return async_resolve(host, service, resolver_base::flags(),
        EGT_ASIO_MOVE_CAST(ResolveHandler)(handler));
  }

  /// Asynchronously perform forward resolution of a query to a list of entries.
  /**
   * This function is used to resolve host and service names into a list of
   * endpoint entries.
   *
   * @param host A string identifying a location. May be a descriptive name or
   * a numeric address string. If an empty string and the passive flag has been
   * specified, the resolved endpoints are suitable for local service binding.
   * If an empty string and passive is not specified, the resolved endpoints
   * will use the loopback address.
   *
   * @param service A string identifying the requested service. This may be a
   * descriptive name or a numeric string corresponding to a port number. May
   * be an empty string, in which case all resolved endpoints will have a port
   * number of 0.
   *
   * @param resolve_flags A set of flags that determine how name resolution
   * should be performed. The default flags are suitable for communication with
   * remote hosts.
   *
   * @param handler The handler to be called when the resolve operation
   * completes. Copies will be made of the handler as required. The function
   * signature of the handler must be:
   * @code void handler(
   *   const egt::asio::error_code& error, // Result of operation.
   *   resolver::results_type results // Resolved endpoints as a range.
   * ); @endcode
   * Regardless of whether the asynchronous operation completes immediately or
   * not, the handler will not be invoked from within this function. Invocation
   * of the handler will be performed in a manner equivalent to using
   * egt::asio::io_context::post().
   *
   * A successful resolve operation is guaranteed to pass a non-empty range to
   * the handler.
   *
   * @note On POSIX systems, host names may be locally defined in the file
   * <tt>/etc/hosts</tt>. On Windows, host names may be defined in the file
   * <tt>c:\\windows\\system32\\drivers\\etc\\hosts</tt>. Remote host name
   * resolution is performed using DNS. Operating systems may use additional
   * locations when resolving host names (such as NETBIOS names on Windows).
   *
   * On POSIX systems, service names are typically defined in the file
   * <tt>/etc/services</tt>. On Windows, service names may be found in the file
   * <tt>c:\\windows\\system32\\drivers\\etc\\services</tt>. Operating systems
   * may use additional locations when resolving service names.
   */
  template <typename ResolveHandler>
  EGT_ASIO_INITFN_RESULT_TYPE(ResolveHandler,
      void (egt::asio::error_code, results_type))
  async_resolve(EGT_ASIO_STRING_VIEW_PARAM host,
      EGT_ASIO_STRING_VIEW_PARAM service,
      resolver_base::flags resolve_flags,
      EGT_ASIO_MOVE_ARG(ResolveHandler) handler)
  {
    // If you get an error on the following line it means that your handler does
    // not meet the documented type requirements for a ResolveHandler.
    EGT_ASIO_RESOLVE_HANDLER_CHECK(
        ResolveHandler, handler, results_type) type_check;

    basic_resolver_query<protocol_type> q(static_cast<std::string>(host),
        static_cast<std::string>(service), resolve_flags);

#if defined(EGT_ASIO_ENABLE_OLD_SERVICES)
    return this->get_service().async_resolve(this->get_implementation(), q,
        EGT_ASIO_MOVE_CAST(ResolveHandler)(handler));
#else // defined(EGT_ASIO_ENABLE_OLD_SERVICES)
    egt::asio::async_completion<ResolveHandler,
      void (egt::asio::error_code, results_type)> init(handler);

    this->get_service().async_resolve(
        this->get_implementation(), q, init.completion_handler);

    return init.result.get();
#endif // defined(EGT_ASIO_ENABLE_OLD_SERVICES)
  }

  /// Asynchronously perform forward resolution of a query to a list of entries.
  /**
   * This function is used to resolve host and service names into a list of
   * endpoint entries.
   *
   * @param protocol A protocol object, normally representing either the IPv4 or
   * IPv6 version of an internet protocol.
   *
   * @param host A string identifying a location. May be a descriptive name or
   * a numeric address string. If an empty string and the passive flag has been
   * specified, the resolved endpoints are suitable for local service binding.
   * If an empty string and passive is not specified, the resolved endpoints
   * will use the loopback address.
   *
   * @param service A string identifying the requested service. This may be a
   * descriptive name or a numeric string corresponding to a port number. May
   * be an empty string, in which case all resolved endpoints will have a port
   * number of 0.
   *
   * @param handler The handler to be called when the resolve operation
   * completes. Copies will be made of the handler as required. The function
   * signature of the handler must be:
   * @code void handler(
   *   const egt::asio::error_code& error, // Result of operation.
   *   resolver::results_type results // Resolved endpoints as a range.
   * ); @endcode
   * Regardless of whether the asynchronous operation completes immediately or
   * not, the handler will not be invoked from within this function. Invocation
   * of the handler will be performed in a manner equivalent to using
   * egt::asio::io_context::post().
   *
   * A successful resolve operation is guaranteed to pass a non-empty range to
   * the handler.
   *
   * @note On POSIX systems, host names may be locally defined in the file
   * <tt>/etc/hosts</tt>. On Windows, host names may be defined in the file
   * <tt>c:\\windows\\system32\\drivers\\etc\\hosts</tt>. Remote host name
   * resolution is performed using DNS. Operating systems may use additional
   * locations when resolving host names (such as NETBIOS names on Windows).
   *
   * On POSIX systems, service names are typically defined in the file
   * <tt>/etc/services</tt>. On Windows, service names may be found in the file
   * <tt>c:\\windows\\system32\\drivers\\etc\\services</tt>. Operating systems
   * may use additional locations when resolving service names.
   */
  template <typename ResolveHandler>
  EGT_ASIO_INITFN_RESULT_TYPE(ResolveHandler,
      void (egt::asio::error_code, results_type))
  async_resolve(const protocol_type& protocol,
      EGT_ASIO_STRING_VIEW_PARAM host, EGT_ASIO_STRING_VIEW_PARAM service,
      EGT_ASIO_MOVE_ARG(ResolveHandler) handler)
  {
    return async_resolve(protocol, host, service, resolver_base::flags(),
        EGT_ASIO_MOVE_CAST(ResolveHandler)(handler));
  }

  /// Asynchronously perform forward resolution of a query to a list of entries.
  /**
   * This function is used to resolve host and service names into a list of
   * endpoint entries.
   *
   * @param protocol A protocol object, normally representing either the IPv4 or
   * IPv6 version of an internet protocol.
   *
   * @param host A string identifying a location. May be a descriptive name or
   * a numeric address string. If an empty string and the passive flag has been
   * specified, the resolved endpoints are suitable for local service binding.
   * If an empty string and passive is not specified, the resolved endpoints
   * will use the loopback address.
   *
   * @param service A string identifying the requested service. This may be a
   * descriptive name or a numeric string corresponding to a port number. May
   * be an empty string, in which case all resolved endpoints will have a port
   * number of 0.
   *
   * @param resolve_flags A set of flags that determine how name resolution
   * should be performed. The default flags are suitable for communication with
   * remote hosts.
   *
   * @param handler The handler to be called when the resolve operation
   * completes. Copies will be made of the handler as required. The function
   * signature of the handler must be:
   * @code void handler(
   *   const egt::asio::error_code& error, // Result of operation.
   *   resolver::results_type results // Resolved endpoints as a range.
   * ); @endcode
   * Regardless of whether the asynchronous operation completes immediately or
   * not, the handler will not be invoked from within this function. Invocation
   * of the handler will be performed in a manner equivalent to using
   * egt::asio::io_context::post().
   *
   * A successful resolve operation is guaranteed to pass a non-empty range to
   * the handler.
   *
   * @note On POSIX systems, host names may be locally defined in the file
   * <tt>/etc/hosts</tt>. On Windows, host names may be defined in the file
   * <tt>c:\\windows\\system32\\drivers\\etc\\hosts</tt>. Remote host name
   * resolution is performed using DNS. Operating systems may use additional
   * locations when resolving host names (such as NETBIOS names on Windows).
   *
   * On POSIX systems, service names are typically defined in the file
   * <tt>/etc/services</tt>. On Windows, service names may be found in the file
   * <tt>c:\\windows\\system32\\drivers\\etc\\services</tt>. Operating systems
   * may use additional locations when resolving service names.
   */
  template <typename ResolveHandler>
  EGT_ASIO_INITFN_RESULT_TYPE(ResolveHandler,
      void (egt::asio::error_code, results_type))
  async_resolve(const protocol_type& protocol,
      EGT_ASIO_STRING_VIEW_PARAM host, EGT_ASIO_STRING_VIEW_PARAM service,
      resolver_base::flags resolve_flags,
      EGT_ASIO_MOVE_ARG(ResolveHandler) handler)
  {
    // If you get an error on the following line it means that your handler does
    // not meet the documented type requirements for a ResolveHandler.
    EGT_ASIO_RESOLVE_HANDLER_CHECK(
        ResolveHandler, handler, results_type) type_check;

    basic_resolver_query<protocol_type> q(
        protocol, static_cast<std::string>(host),
        static_cast<std::string>(service), resolve_flags);

#if defined(EGT_ASIO_ENABLE_OLD_SERVICES)
    return this->get_service().async_resolve(this->get_implementation(), q,
        EGT_ASIO_MOVE_CAST(ResolveHandler)(handler));
#else // defined(EGT_ASIO_ENABLE_OLD_SERVICES)
    egt::asio::async_completion<ResolveHandler,
      void (egt::asio::error_code, results_type)> init(handler);

    this->get_service().async_resolve(
        this->get_implementation(), q, init.completion_handler);

    return init.result.get();
#endif // defined(EGT_ASIO_ENABLE_OLD_SERVICES)
  }

  /// Perform reverse resolution of an endpoint to a list of entries.
  /**
   * This function is used to resolve an endpoint into a list of endpoint
   * entries.
   *
   * @param e An endpoint object that determines what endpoints will be
   * returned.
   *
   * @returns A range object representing the list of endpoint entries. A
   * successful call to this function is guaranteed to return a non-empty
   * range.
   *
   * @throws egt::asio::system_error Thrown on failure.
   */
  results_type resolve(const endpoint_type& e)
  {
    egt::asio::error_code ec;
    results_type i = this->get_service().resolve(
        this->get_implementation(), e, ec);
    egt::asio::detail::throw_error(ec, "resolve");
    return i;
  }

  /// Perform reverse resolution of an endpoint to a list of entries.
  /**
   * This function is used to resolve an endpoint into a list of endpoint
   * entries.
   *
   * @param e An endpoint object that determines what endpoints will be
   * returned.
   *
   * @param ec Set to indicate what error occurred, if any.
   *
   * @returns A range object representing the list of endpoint entries. An
   * empty range is returned if an error occurs. A successful call to this
   * function is guaranteed to return a non-empty range.
   */
  results_type resolve(const endpoint_type& e, egt::asio::error_code& ec)
  {
    return this->get_service().resolve(this->get_implementation(), e, ec);
  }

  /// Asynchronously perform reverse resolution of an endpoint to a list of
  /// entries.
  /**
   * This function is used to asynchronously resolve an endpoint into a list of
   * endpoint entries.
   *
   * @param e An endpoint object that determines what endpoints will be
   * returned.
   *
   * @param handler The handler to be called when the resolve operation
   * completes. Copies will be made of the handler as required. The function
   * signature of the handler must be:
   * @code void handler(
   *   const egt::asio::error_code& error, // Result of operation.
   *   resolver::results_type results // Resolved endpoints as a range.
   * ); @endcode
   * Regardless of whether the asynchronous operation completes immediately or
   * not, the handler will not be invoked from within this function. Invocation
   * of the handler will be performed in a manner equivalent to using
   * egt::asio::io_context::post().
   *
   * A successful resolve operation is guaranteed to pass a non-empty range to
   * the handler.
   */
  template <typename ResolveHandler>
  EGT_ASIO_INITFN_RESULT_TYPE(ResolveHandler,
      void (egt::asio::error_code, results_type))
  async_resolve(const endpoint_type& e,
      EGT_ASIO_MOVE_ARG(ResolveHandler) handler)
  {
    // If you get an error on the following line it means that your handler does
    // not meet the documented type requirements for a ResolveHandler.
    EGT_ASIO_RESOLVE_HANDLER_CHECK(
        ResolveHandler, handler, results_type) type_check;

#if defined(EGT_ASIO_ENABLE_OLD_SERVICES)
    return this->get_service().async_resolve(this->get_implementation(), e,
        EGT_ASIO_MOVE_CAST(ResolveHandler)(handler));
#else // defined(EGT_ASIO_ENABLE_OLD_SERVICES)
    egt::asio::async_completion<ResolveHandler,
      void (egt::asio::error_code, results_type)> init(handler);

    this->get_service().async_resolve(
        this->get_implementation(), e, init.completion_handler);

    return init.result.get();
#endif // defined(EGT_ASIO_ENABLE_OLD_SERVICES)
  }
};

} // namespace ip
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#if !defined(EGT_ASIO_ENABLE_OLD_SERVICES)
# undef EGT_ASIO_SVC_T
#endif // !defined(EGT_ASIO_ENABLE_OLD_SERVICES)

#endif // EGT_ASIO_IP_BASIC_RESOLVER_HPP
