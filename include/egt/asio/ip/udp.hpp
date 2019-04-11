//
// ip/udp.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_IP_UDP_HPP
#define EGT_ASIO_IP_UDP_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>
#include <egt/asio/basic_datagram_socket.hpp>
#include <egt/asio/detail/socket_types.hpp>
#include <egt/asio/ip/basic_endpoint.hpp>
#include <egt/asio/ip/basic_resolver.hpp>
#include <egt/asio/ip/basic_resolver_iterator.hpp>
#include <egt/asio/ip/basic_resolver_query.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace ip {

/// Encapsulates the flags needed for UDP.
/**
 * The egt::asio::ip::udp class contains flags necessary for UDP sockets.
 *
 * @par Thread Safety
 * @e Distinct @e objects: Safe.@n
 * @e Shared @e objects: Safe.
 *
 * @par Concepts:
 * Protocol, InternetProtocol.
 */
class udp
{
public:
  /// The type of a UDP endpoint.
  typedef basic_endpoint<udp> endpoint;

  /// Construct to represent the IPv4 UDP protocol.
  static udp v4()
  {
    return udp(EGT_ASIO_OS_DEF(AF_INET));
  }

  /// Construct to represent the IPv6 UDP protocol.
  static udp v6()
  {
    return udp(EGT_ASIO_OS_DEF(AF_INET6));
  }

  /// Obtain an identifier for the type of the protocol.
  int type() const
  {
    return EGT_ASIO_OS_DEF(SOCK_DGRAM);
  }

  /// Obtain an identifier for the protocol.
  int protocol() const
  {
    return EGT_ASIO_OS_DEF(IPPROTO_UDP);
  }

  /// Obtain an identifier for the protocol family.
  int family() const
  {
    return family_;
  }

  /// The UDP socket type.
  typedef basic_datagram_socket<udp> socket;

  /// The UDP resolver type.
  typedef basic_resolver<udp> resolver;

  /// Compare two protocols for equality.
  friend bool operator==(const udp& p1, const udp& p2)
  {
    return p1.family_ == p2.family_;
  }

  /// Compare two protocols for inequality.
  friend bool operator!=(const udp& p1, const udp& p2)
  {
    return p1.family_ != p2.family_;
  }

private:
  // Construct with a specific family.
  explicit udp(int protocol_family)
    : family_(protocol_family)
  {
  }

  int family_;
};

} // namespace ip
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#endif // EGT_ASIO_IP_UDP_HPP
