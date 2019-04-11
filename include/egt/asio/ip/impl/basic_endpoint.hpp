//
// ip/impl/basic_endpoint.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_IP_IMPL_BASIC_ENDPOINT_HPP
#define EGT_ASIO_IP_IMPL_BASIC_ENDPOINT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#if !defined(EGT_ASIO_NO_IOSTREAM)

#include <egt/asio/detail/throw_error.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace ip {

template <typename Elem, typename Traits, typename InternetProtocol>
std::basic_ostream<Elem, Traits>& operator<<(
    std::basic_ostream<Elem, Traits>& os,
    const basic_endpoint<InternetProtocol>& endpoint)
{
  egt::asio::ip::detail::endpoint tmp_ep(endpoint.address(), endpoint.port());
  return os << tmp_ep.to_string().c_str();
}

} // namespace ip
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#endif // !defined(EGT_ASIO_NO_IOSTREAM)

#endif // EGT_ASIO_IP_IMPL_BASIC_ENDPOINT_HPP
