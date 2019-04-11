//
// ip/address.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_IP_ADDRESS_HPP
#define EGT_ASIO_IP_ADDRESS_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>
#include <string>
#include <egt/asio/detail/throw_exception.hpp>
#include <egt/asio/detail/string_view.hpp>
#include <egt/asio/detail/type_traits.hpp>
#include <egt/asio/error_code.hpp>
#include <egt/asio/ip/address_v4.hpp>
#include <egt/asio/ip/address_v6.hpp>
#include <egt/asio/ip/bad_address_cast.hpp>

#if !defined(EGT_ASIO_NO_IOSTREAM)
# include <iosfwd>
#endif // !defined(EGT_ASIO_NO_IOSTREAM)

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace ip {

/// Implements version-independent IP addresses.
/**
 * The egt::asio::ip::address class provides the ability to use either IP
 * version 4 or version 6 addresses.
 *
 * @par Thread Safety
 * @e Distinct @e objects: Safe.@n
 * @e Shared @e objects: Unsafe.
 */
class address
{
public:
  /// Default constructor.
  EGT_ASIO_DECL address();

  /// Construct an address from an IPv4 address.
  EGT_ASIO_DECL address(const egt::asio::ip::address_v4& ipv4_address);

  /// Construct an address from an IPv6 address.
  EGT_ASIO_DECL address(const egt::asio::ip::address_v6& ipv6_address);

  /// Copy constructor.
  EGT_ASIO_DECL address(const address& other);

#if defined(EGT_ASIO_HAS_MOVE)
  /// Move constructor.
  EGT_ASIO_DECL address(address&& other);
#endif // defined(EGT_ASIO_HAS_MOVE)

  /// Assign from another address.
  EGT_ASIO_DECL address& operator=(const address& other);

#if defined(EGT_ASIO_HAS_MOVE)
  /// Move-assign from another address.
  EGT_ASIO_DECL address& operator=(address&& other);
#endif // defined(EGT_ASIO_HAS_MOVE)

  /// Assign from an IPv4 address.
  EGT_ASIO_DECL address& operator=(
      const egt::asio::ip::address_v4& ipv4_address);

  /// Assign from an IPv6 address.
  EGT_ASIO_DECL address& operator=(
      const egt::asio::ip::address_v6& ipv6_address);

  /// Get whether the address is an IP version 4 address.
  bool is_v4() const
  {
    return type_ == ipv4;
  }

  /// Get whether the address is an IP version 6 address.
  bool is_v6() const
  {
    return type_ == ipv6;
  }

  /// Get the address as an IP version 4 address.
  EGT_ASIO_DECL egt::asio::ip::address_v4 to_v4() const;

  /// Get the address as an IP version 6 address.
  EGT_ASIO_DECL egt::asio::ip::address_v6 to_v6() const;

  /// Get the address as a string.
  EGT_ASIO_DECL std::string to_string() const;

#if !defined(EGT_ASIO_NO_DEPRECATED)
  /// (Deprecated: Use other overload.) Get the address as a string.
  EGT_ASIO_DECL std::string to_string(egt::asio::error_code& ec) const;

  /// (Deprecated: Use make_address().) Create an address from an IPv4 address
  /// string in dotted decimal form, or from an IPv6 address in hexadecimal
  /// notation.
  static address from_string(const char* str);

  /// (Deprecated: Use make_address().) Create an address from an IPv4 address
  /// string in dotted decimal form, or from an IPv6 address in hexadecimal
  /// notation.
  static address from_string(const char* str, egt::asio::error_code& ec);

  /// (Deprecated: Use make_address().) Create an address from an IPv4 address
  /// string in dotted decimal form, or from an IPv6 address in hexadecimal
  /// notation.
  static address from_string(const std::string& str);

  /// (Deprecated: Use make_address().) Create an address from an IPv4 address
  /// string in dotted decimal form, or from an IPv6 address in hexadecimal
  /// notation.
  static address from_string(
      const std::string& str, egt::asio::error_code& ec);
#endif // !defined(EGT_ASIO_NO_DEPRECATED)

  /// Determine whether the address is a loopback address.
  EGT_ASIO_DECL bool is_loopback() const;

  /// Determine whether the address is unspecified.
  EGT_ASIO_DECL bool is_unspecified() const;

  /// Determine whether the address is a multicast address.
  EGT_ASIO_DECL bool is_multicast() const;

  /// Compare two addresses for equality.
  EGT_ASIO_DECL friend bool operator==(const address& a1, const address& a2);

  /// Compare two addresses for inequality.
  friend bool operator!=(const address& a1, const address& a2)
  {
    return !(a1 == a2);
  }

  /// Compare addresses for ordering.
  EGT_ASIO_DECL friend bool operator<(const address& a1, const address& a2);

  /// Compare addresses for ordering.
  friend bool operator>(const address& a1, const address& a2)
  {
    return a2 < a1;
  }

  /// Compare addresses for ordering.
  friend bool operator<=(const address& a1, const address& a2)
  {
    return !(a2 < a1);
  }

  /// Compare addresses for ordering.
  friend bool operator>=(const address& a1, const address& a2)
  {
    return !(a1 < a2);
  }

private:
  // The type of the address.
  enum { ipv4, ipv6 } type_;

  // The underlying IPv4 address.
  egt::asio::ip::address_v4 ipv4_address_;

  // The underlying IPv6 address.
  egt::asio::ip::address_v6 ipv6_address_;
};

/// Create an address from an IPv4 address string in dotted decimal form,
/// or from an IPv6 address in hexadecimal notation.
/**
 * @relates address
 */
EGT_ASIO_DECL address make_address(const char* str);

/// Create an address from an IPv4 address string in dotted decimal form,
/// or from an IPv6 address in hexadecimal notation.
/**
 * @relates address
 */
EGT_ASIO_DECL address make_address(
    const char* str, egt::asio::error_code& ec);

/// Create an address from an IPv4 address string in dotted decimal form,
/// or from an IPv6 address in hexadecimal notation.
/**
 * @relates address
 */
EGT_ASIO_DECL address make_address(const std::string& str);

/// Create an address from an IPv4 address string in dotted decimal form,
/// or from an IPv6 address in hexadecimal notation.
/**
 * @relates address
 */
EGT_ASIO_DECL address make_address(
    const std::string& str, egt::asio::error_code& ec);

#if defined(EGT_ASIO_HAS_STRING_VIEW) \
  || defined(GENERATING_DOCUMENTATION)

/// Create an address from an IPv4 address string in dotted decimal form,
/// or from an IPv6 address in hexadecimal notation.
/**
 * @relates address
 */
EGT_ASIO_DECL address make_address(string_view str);

/// Create an address from an IPv4 address string in dotted decimal form,
/// or from an IPv6 address in hexadecimal notation.
/**
 * @relates address
 */
EGT_ASIO_DECL address make_address(
    string_view str, egt::asio::error_code& ec);

#endif // defined(EGT_ASIO_HAS_STRING_VIEW)
       //  || defined(GENERATING_DOCUMENTATION)

#if !defined(EGT_ASIO_NO_IOSTREAM)

/// Output an address as a string.
/**
 * Used to output a human-readable string for a specified address.
 *
 * @param os The output stream to which the string will be written.
 *
 * @param addr The address to be written.
 *
 * @return The output stream.
 *
 * @relates egt::asio::ip::address
 */
template <typename Elem, typename Traits>
std::basic_ostream<Elem, Traits>& operator<<(
    std::basic_ostream<Elem, Traits>& os, const address& addr);

#endif // !defined(EGT_ASIO_NO_IOSTREAM)

} // namespace ip
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#include <egt/asio/ip/impl/address.hpp>
#if defined(EGT_ASIO_HEADER_ONLY)
# include <egt/asio/ip/impl/address.ipp>
#endif // defined(EGT_ASIO_HEADER_ONLY)

#endif // EGT_ASIO_IP_ADDRESS_HPP
