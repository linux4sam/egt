//
// ip/impl/address_v4.ipp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_IP_IMPL_ADDRESS_V4_IPP
#define EGT_ASIO_IP_IMPL_ADDRESS_V4_IPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>
#include <climits>
#include <limits>
#include <stdexcept>
#include <egt/asio/error.hpp>
#include <egt/asio/detail/socket_ops.hpp>
#include <egt/asio/detail/throw_error.hpp>
#include <egt/asio/detail/throw_exception.hpp>
#include <egt/asio/ip/address_v4.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace ip {

address_v4::address_v4(const address_v4::bytes_type& bytes)
{
#if UCHAR_MAX > 0xFF
  if (bytes[0] > 0xFF || bytes[1] > 0xFF
      || bytes[2] > 0xFF || bytes[3] > 0xFF)
  {
    std::out_of_range ex("address_v4 from bytes_type");
    egt::asio::detail::throw_exception(ex);
  }
#endif // UCHAR_MAX > 0xFF

  using namespace std; // For memcpy.
  memcpy(&addr_.s_addr, bytes.data(), 4);
}

address_v4::address_v4(address_v4::uint_type addr)
{
  if ((std::numeric_limits<uint_type>::max)() > 0xFFFFFFFF)
  {
    std::out_of_range ex("address_v4 from unsigned integer");
    egt::asio::detail::throw_exception(ex);
  }

  addr_.s_addr = egt::asio::detail::socket_ops::host_to_network_long(
      static_cast<egt::asio::detail::u_long_type>(addr));
}

address_v4::bytes_type address_v4::to_bytes() const
{
  using namespace std; // For memcpy.
  bytes_type bytes;
#if defined(EGT_ASIO_HAS_STD_ARRAY)
  memcpy(bytes.data(), &addr_.s_addr, 4);
#else // defined(EGT_ASIO_HAS_STD_ARRAY)
  memcpy(bytes.elems, &addr_.s_addr, 4);
#endif // defined(EGT_ASIO_HAS_STD_ARRAY)
  return bytes;
}

address_v4::uint_type address_v4::to_uint() const
{
  return egt::asio::detail::socket_ops::network_to_host_long(addr_.s_addr);
}

#if !defined(EGT_ASIO_NO_DEPRECATED)
unsigned long address_v4::to_ulong() const
{
  return egt::asio::detail::socket_ops::network_to_host_long(addr_.s_addr);
}
#endif // !defined(EGT_ASIO_NO_DEPRECATED)

std::string address_v4::to_string() const
{
  egt::asio::error_code ec;
  char addr_str[egt::asio::detail::max_addr_v4_str_len];
  const char* addr =
    egt::asio::detail::socket_ops::inet_ntop(
        EGT_ASIO_OS_DEF(AF_INET), &addr_, addr_str,
        egt::asio::detail::max_addr_v4_str_len, 0, ec);
  if (addr == 0)
    egt::asio::detail::throw_error(ec);
  return addr;
}

#if !defined(EGT_ASIO_NO_DEPRECATED)
std::string address_v4::to_string(egt::asio::error_code& ec) const
{
  char addr_str[egt::asio::detail::max_addr_v4_str_len];
  const char* addr =
    egt::asio::detail::socket_ops::inet_ntop(
        EGT_ASIO_OS_DEF(AF_INET), &addr_, addr_str,
        egt::asio::detail::max_addr_v4_str_len, 0, ec);
  if (addr == 0)
    return std::string();
  return addr;
}
#endif // !defined(EGT_ASIO_NO_DEPRECATED)

bool address_v4::is_loopback() const
{
  return (to_uint() & 0xFF000000) == 0x7F000000;
}

bool address_v4::is_unspecified() const
{
  return to_uint() == 0;
}

#if !defined(EGT_ASIO_NO_DEPRECATED)
bool address_v4::is_class_a() const
{
  return (to_uint() & 0x80000000) == 0;
}

bool address_v4::is_class_b() const
{
  return (to_uint() & 0xC0000000) == 0x80000000;
}

bool address_v4::is_class_c() const
{
  return (to_uint() & 0xE0000000) == 0xC0000000;
}
#endif // !defined(EGT_ASIO_NO_DEPRECATED)

bool address_v4::is_multicast() const
{
  return (to_uint() & 0xF0000000) == 0xE0000000;
}

#if !defined(EGT_ASIO_NO_DEPRECATED)
address_v4 address_v4::broadcast(const address_v4& addr, const address_v4& mask)
{
  return address_v4(addr.to_uint() | (mask.to_uint() ^ 0xFFFFFFFF));
}

address_v4 address_v4::netmask(const address_v4& addr)
{
  if (addr.is_class_a())
    return address_v4(0xFF000000);
  if (addr.is_class_b())
    return address_v4(0xFFFF0000);
  if (addr.is_class_c())
    return address_v4(0xFFFFFF00);
  return address_v4(0xFFFFFFFF);
}
#endif // !defined(EGT_ASIO_NO_DEPRECATED)

address_v4 make_address_v4(const char* str)
{
  egt::asio::error_code ec;
  address_v4 addr = make_address_v4(str, ec);
  egt::asio::detail::throw_error(ec);
  return addr;
}

address_v4 make_address_v4(
    const char* str, egt::asio::error_code& ec)
{
  address_v4::bytes_type bytes;
  if (egt::asio::detail::socket_ops::inet_pton(
        EGT_ASIO_OS_DEF(AF_INET), str, &bytes, 0, ec) <= 0)
    return address_v4();
  return address_v4(bytes);
}

address_v4 make_address_v4(const std::string& str)
{
  return make_address_v4(str.c_str());
}

address_v4 make_address_v4(
    const std::string& str, egt::asio::error_code& ec)
{
  return make_address_v4(str.c_str(), ec);
}

#if defined(EGT_ASIO_HAS_STRING_VIEW)

address_v4 make_address_v4(string_view str)
{
  return make_address_v4(static_cast<std::string>(str));
}

address_v4 make_address_v4(string_view str,
    egt::asio::error_code& ec)
{
  return make_address_v4(static_cast<std::string>(str), ec);
}

#endif // defined(EGT_ASIO_HAS_STRING_VIEW)

} // namespace ip
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#endif // EGT_ASIO_IP_IMPL_ADDRESS_V4_IPP
