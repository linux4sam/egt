//
// ip/network_v6.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Copyright (c) 2014 Oliver Kowalke (oliver dot kowalke at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_IP_NETWORK_V6_HPP
#define EGT_ASIO_IP_NETWORK_V6_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>
#include <string>
#include <egt/asio/detail/string_view.hpp>
#include <egt/asio/error_code.hpp>
#include <egt/asio/ip/address_v6_range.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace ip {

/// Represents an IPv6 network.
/**
 * The egt::asio::ip::network_v6 class provides the ability to use and
 * manipulate IP version 6 networks.
 *
 * @par Thread Safety
 * @e Distinct @e objects: Safe.@n
 * @e Shared @e objects: Unsafe.
 */
class network_v6
{
public:
  /// Default constructor.
  network_v6() EGT_ASIO_NOEXCEPT
    : address_(),
      prefix_length_(0)
  {
  }

  /// Construct a network based on the specified address and prefix length.
  EGT_ASIO_DECL network_v6(const address_v6& addr,
      unsigned short prefix_len);

  /// Copy constructor.
  network_v6(const network_v6& other) EGT_ASIO_NOEXCEPT
    : address_(other.address_),
      prefix_length_(other.prefix_length_)
  {
  }

#if defined(EGT_ASIO_HAS_MOVE)
  /// Move constructor.
  network_v6(network_v6&& other) EGT_ASIO_NOEXCEPT
    : address_(EGT_ASIO_MOVE_CAST(address_v6)(other.address_)),
      prefix_length_(other.prefix_length_)
  {
  }
#endif // defined(EGT_ASIO_HAS_MOVE)

  /// Assign from another network.
  network_v6& operator=(const network_v6& other) EGT_ASIO_NOEXCEPT
  {
    address_ = other.address_;
    prefix_length_ = other.prefix_length_;
    return *this;
  }

#if defined(EGT_ASIO_HAS_MOVE)
  /// Move-assign from another network.
  network_v6& operator=(network_v6&& other) EGT_ASIO_NOEXCEPT
  {
    address_ = EGT_ASIO_MOVE_CAST(address_v6)(other.address_);
    prefix_length_ = other.prefix_length_;
    return *this;
  }
#endif // defined(EGT_ASIO_HAS_MOVE)

  /// Obtain the address object specified when the network object was created.
  address_v6 address() const EGT_ASIO_NOEXCEPT
  {
    return address_;
  }

  /// Obtain the prefix length that was specified when the network object was
  /// created.
  unsigned short prefix_length() const EGT_ASIO_NOEXCEPT
  {
    return prefix_length_;
  }

  /// Obtain an address object that represents the network address.
  EGT_ASIO_DECL address_v6 network() const EGT_ASIO_NOEXCEPT;

  /// Obtain an address range corresponding to the hosts in the network.
  EGT_ASIO_DECL address_v6_range hosts() const EGT_ASIO_NOEXCEPT;

  /// Obtain the true network address, omitting any host bits.
  network_v6 canonical() const EGT_ASIO_NOEXCEPT
  {
    return network_v6(network(), prefix_length());
  }

  /// Test if network is a valid host address.
  bool is_host() const EGT_ASIO_NOEXCEPT
  {
    return prefix_length_ == 128;
  }

  /// Test if a network is a real subnet of another network.
  EGT_ASIO_DECL bool is_subnet_of(const network_v6& other) const;

  /// Get the network as an address in dotted decimal format.
  EGT_ASIO_DECL std::string to_string() const;

  /// Get the network as an address in dotted decimal format.
  EGT_ASIO_DECL std::string to_string(egt::asio::error_code& ec) const;

  /// Compare two networks for equality.
  friend bool operator==(const network_v6& a, const network_v6& b)
  {
    return a.address_ == b.address_ && a.prefix_length_ == b.prefix_length_;
  }

  /// Compare two networks for inequality.
  friend bool operator!=(const network_v6& a, const network_v6& b)
  {
    return !(a == b);
  }

private:
  address_v6 address_;
  unsigned short prefix_length_;
};

/// Create an IPv6 network from an address and prefix length.
/**
 * @relates address_v6
 */
inline network_v6 make_network_v6(
    const address_v6& addr, unsigned short prefix_len)
{
  return network_v6(addr, prefix_len);
}

/// Create an IPv6 network from a string containing IP address and prefix
/// length.
/**
 * @relates network_v6
 */
EGT_ASIO_DECL network_v6 make_network_v6(const char* str);

/// Create an IPv6 network from a string containing IP address and prefix
/// length.
/**
 * @relates network_v6
 */
EGT_ASIO_DECL network_v6 make_network_v6(
    const char* str, egt::asio::error_code& ec);

/// Create an IPv6 network from a string containing IP address and prefix
/// length.
/**
 * @relates network_v6
 */
EGT_ASIO_DECL network_v6 make_network_v6(const std::string& str);

/// Create an IPv6 network from a string containing IP address and prefix
/// length.
/**
 * @relates network_v6
 */
EGT_ASIO_DECL network_v6 make_network_v6(
    const std::string& str, egt::asio::error_code& ec);

#if defined(EGT_ASIO_HAS_STRING_VIEW) \
  || defined(GENERATING_DOCUMENTATION)

/// Create an IPv6 network from a string containing IP address and prefix
/// length.
/**
 * @relates network_v6
 */
EGT_ASIO_DECL network_v6 make_network_v6(string_view str);

/// Create an IPv6 network from a string containing IP address and prefix
/// length.
/**
 * @relates network_v6
 */
EGT_ASIO_DECL network_v6 make_network_v6(
    string_view str, egt::asio::error_code& ec);

#endif // defined(EGT_ASIO_HAS_STRING_VIEW)
       //  || defined(GENERATING_DOCUMENTATION)

#if !defined(EGT_ASIO_NO_IOSTREAM)

/// Output a network as a string.
/**
 * Used to output a human-readable string for a specified network.
 *
 * @param os The output stream to which the string will be written.
 *
 * @param net The network to be written.
 *
 * @return The output stream.
 *
 * @relates egt::asio::ip::address_v6
 */
template <typename Elem, typename Traits>
std::basic_ostream<Elem, Traits>& operator<<(
    std::basic_ostream<Elem, Traits>& os, const network_v6& net);

#endif // !defined(EGT_ASIO_NO_IOSTREAM)

} // namespace ip
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#include <egt/asio/ip/impl/network_v6.hpp>
#if defined(EGT_ASIO_HEADER_ONLY)
# include <egt/asio/ip/impl/network_v6.ipp>
#endif // defined(EGT_ASIO_HEADER_ONLY)

#endif // EGT_ASIO_IP_NETWORK_V6_HPP
