//
// detail/win_iocp_wait_op.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_DETAIL_WIN_IOCP_WAIT_OP_HPP
#define EGT_ASIO_DETAIL_WIN_IOCP_WAIT_OP_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

#if defined(EGT_ASIO_HAS_IOCP)

#include <egt/asio/detail/bind_handler.hpp>
#include <egt/asio/detail/buffer_sequence_adapter.hpp>
#include <egt/asio/detail/fenced_block.hpp>
#include <egt/asio/detail/handler_alloc_helpers.hpp>
#include <egt/asio/detail/handler_invoke_helpers.hpp>
#include <egt/asio/detail/memory.hpp>
#include <egt/asio/detail/reactor_op.hpp>
#include <egt/asio/detail/socket_ops.hpp>
#include <egt/asio/error.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace detail {

template <typename Handler>
class win_iocp_wait_op : public reactor_op
{
public:
  EGT_ASIO_DEFINE_HANDLER_PTR(win_iocp_wait_op);

  win_iocp_wait_op(socket_ops::weak_cancel_token_type cancel_token,
      Handler& handler)
    : reactor_op(&win_iocp_wait_op::do_perform,
        &win_iocp_wait_op::do_complete),
      cancel_token_(cancel_token),
      handler_(EGT_ASIO_MOVE_CAST(Handler)(handler))
  {
    handler_work<Handler>::start(handler_);
  }

  static status do_perform(reactor_op*)
  {
    return done;
  }

  static void do_complete(void* owner, operation* base,
      const egt::asio::error_code& result_ec,
      std::size_t /*bytes_transferred*/)
  {
    egt::asio::error_code ec(result_ec);

    // Take ownership of the operation object.
    win_iocp_wait_op* o(static_cast<win_iocp_wait_op*>(base));
    ptr p = { egt::asio::detail::addressof(o->handler_), o, o };
    handler_work<Handler> w(o->handler_);

    EGT_ASIO_HANDLER_COMPLETION((*o));

    // The reactor may have stored a result in the operation object.
    if (o->ec_)
      ec = o->ec_;

    // Map non-portable errors to their portable counterparts.
    if (ec.value() == ERROR_NETNAME_DELETED)
    {
      if (o->cancel_token_.expired())
        ec = egt::asio::error::operation_aborted;
      else
        ec = egt::asio::error::connection_reset;
    }
    else if (ec.value() == ERROR_PORT_UNREACHABLE)
    {
      ec = egt::asio::error::connection_refused;
    }

    // Make a copy of the handler so that the memory can be deallocated before
    // the upcall is made. Even if we're not about to make an upcall, a
    // sub-object of the handler may be the true owner of the memory associated
    // with the handler. Consequently, a local copy of the handler is required
    // to ensure that any owning sub-object remains valid until after we have
    // deallocated the memory here.
    detail::binder1<Handler, egt::asio::error_code>
      handler(o->handler_, ec);
    p.h = egt::asio::detail::addressof(handler.handler_);
    p.reset();

    // Make the upcall if required.
    if (owner)
    {
      fenced_block b(fenced_block::half);
      EGT_ASIO_HANDLER_INVOCATION_BEGIN((handler.arg1_));
      w.complete(handler, handler.handler_);
      EGT_ASIO_HANDLER_INVOCATION_END;
    }
  }

private:
  socket_ops::weak_cancel_token_type cancel_token_;
  Handler handler_;
};

} // namespace detail
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#endif // defined(EGT_ASIO_HAS_IOCP)

#endif // EGT_ASIO_DETAIL_WIN_IOCP_WAIT_OP_HPP
