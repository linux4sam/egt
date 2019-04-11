//
// detail/signal_handler.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_DETAIL_SIGNAL_HANDLER_HPP
#define EGT_ASIO_DETAIL_SIGNAL_HANDLER_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>
#include <egt/asio/detail/bind_handler.hpp>
#include <egt/asio/detail/fenced_block.hpp>
#include <egt/asio/detail/handler_alloc_helpers.hpp>
#include <egt/asio/detail/handler_invoke_helpers.hpp>
#include <egt/asio/detail/handler_work.hpp>
#include <egt/asio/detail/memory.hpp>
#include <egt/asio/detail/signal_op.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace detail {

template <typename Handler>
class signal_handler : public signal_op
{
public:
  EGT_ASIO_DEFINE_HANDLER_PTR(signal_handler);

  signal_handler(Handler& h)
    : signal_op(&signal_handler::do_complete),
      handler_(EGT_ASIO_MOVE_CAST(Handler)(h))
  {
    handler_work<Handler>::start(handler_);
  }

  static void do_complete(void* owner, operation* base,
      const egt::asio::error_code& /*ec*/,
      std::size_t /*bytes_transferred*/)
  {
    // Take ownership of the handler object.
    signal_handler* h(static_cast<signal_handler*>(base));
    ptr p = { egt::asio::detail::addressof(h->handler_), h, h };
    handler_work<Handler> w(h->handler_);

    EGT_ASIO_HANDLER_COMPLETION((*h));

    // Make a copy of the handler so that the memory can be deallocated before
    // the upcall is made. Even if we're not about to make an upcall, a
    // sub-object of the handler may be the true owner of the memory associated
    // with the handler. Consequently, a local copy of the handler is required
    // to ensure that any owning sub-object remains valid until after we have
    // deallocated the memory here.
    detail::binder2<Handler, egt::asio::error_code, int>
      handler(h->handler_, h->ec_, h->signal_number_);
    p.h = egt::asio::detail::addressof(handler.handler_);
    p.reset();

    // Make the upcall if required.
    if (owner)
    {
      fenced_block b(fenced_block::half);
      EGT_ASIO_HANDLER_INVOCATION_BEGIN((handler.arg1_, handler.arg2_));
      w.complete(handler, handler.handler_);
      EGT_ASIO_HANDLER_INVOCATION_END;
    }
  }

private:
  Handler handler_;
};

} // namespace detail
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#endif // EGT_ASIO_DETAIL_SIGNAL_HANDLER_HPP
