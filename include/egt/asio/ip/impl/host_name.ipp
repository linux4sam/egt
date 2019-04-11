//
// ip/impl/host_name.ipp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_IP_IMPL_HOST_NAME_IPP
#define EGT_ASIO_IP_IMPL_HOST_NAME_IPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>
#include <egt/asio/detail/socket_ops.hpp>
#include <egt/asio/detail/throw_error.hpp>
#include <egt/asio/detail/winsock_init.hpp>
#include <egt/asio/ip/host_name.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace ip {

std::string host_name()
{
  char name[1024];
  egt::asio::error_code ec;
  if (egt::asio::detail::socket_ops::gethostname(name, sizeof(name), ec) != 0)
  {
    egt::asio::detail::throw_error(ec);
    return std::string();
  }
  return std::string(name);
}

std::string host_name(egt::asio::error_code& ec)
{
  char name[1024];
  if (egt::asio::detail::socket_ops::gethostname(name, sizeof(name), ec) != 0)
    return std::string();
  return std::string(name);
}

} // namespace ip
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#endif // EGT_ASIO_IP_IMPL_HOST_NAME_IPP
