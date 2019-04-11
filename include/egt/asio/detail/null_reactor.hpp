//
// detail/null_reactor.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_DETAIL_NULL_REACTOR_HPP
#define EGT_ASIO_DETAIL_NULL_REACTOR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

#if defined(EGT_ASIO_HAS_IOCP) || defined(EGT_ASIO_WINDOWS_RUNTIME)

#include <egt/asio/detail/scheduler_operation.hpp>
#include <egt/asio/execution_context.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace detail {

class null_reactor
  : public execution_context_service_base<null_reactor>
{
public:
  // Constructor.
  null_reactor(egt::asio::execution_context& ctx)
    : execution_context_service_base<null_reactor>(ctx)
  {
  }

  // Destructor.
  ~null_reactor()
  {
  }

  // Destroy all user-defined handler objects owned by the service.
  void shutdown()
  {
  }

  // No-op because should never be called.
  void run(long /*usec*/, op_queue<scheduler_operation>& /*ops*/)
  {
  }

  // No-op.
  void interrupt()
  {
  }
};

} // namespace detail
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#endif // defined(EGT_ASIO_HAS_IOCP) || defined(EGT_ASIO_WINDOWS_RUNTIME)

#endif // EGT_ASIO_DETAIL_NULL_REACTOR_HPP
