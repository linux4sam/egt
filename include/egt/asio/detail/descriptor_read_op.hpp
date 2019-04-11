//
// detail/descriptor_read_op.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_DETAIL_DESCRIPTOR_READ_OP_HPP
#define EGT_ASIO_DETAIL_DESCRIPTOR_READ_OP_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

#if !defined(EGT_ASIO_WINDOWS) && !defined(__CYGWIN__)

#include <egt/asio/detail/bind_handler.hpp>
#include <egt/asio/detail/buffer_sequence_adapter.hpp>
#include <egt/asio/detail/descriptor_ops.hpp>
#include <egt/asio/detail/fenced_block.hpp>
#include <egt/asio/detail/handler_work.hpp>
#include <egt/asio/detail/memory.hpp>
#include <egt/asio/detail/reactor_op.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace detail {

template <typename MutableBufferSequence>
class descriptor_read_op_base : public reactor_op
{
public:
  descriptor_read_op_base(int descriptor,
      const MutableBufferSequence& buffers, func_type complete_func)
    : reactor_op(&descriptor_read_op_base::do_perform, complete_func),
      descriptor_(descriptor),
      buffers_(buffers)
  {
  }

  static status do_perform(reactor_op* base)
  {
    descriptor_read_op_base* o(static_cast<descriptor_read_op_base*>(base));

    buffer_sequence_adapter<egt::asio::mutable_buffer,
        MutableBufferSequence> bufs(o->buffers_);

    status result = descriptor_ops::non_blocking_read(o->descriptor_,
        bufs.buffers(), bufs.count(), o->ec_, o->bytes_transferred_)
      ? done : not_done;

    EGT_ASIO_HANDLER_REACTOR_OPERATION((*o, "non_blocking_read",
          o->ec_, o->bytes_transferred_));

    return result;
  }

private:
  int descriptor_;
  MutableBufferSequence buffers_;
};

template <typename MutableBufferSequence, typename Handler>
class descriptor_read_op
  : public descriptor_read_op_base<MutableBufferSequence>
{
public:
  EGT_ASIO_DEFINE_HANDLER_PTR(descriptor_read_op);

  descriptor_read_op(int descriptor,
      const MutableBufferSequence& buffers, Handler& handler)
    : descriptor_read_op_base<MutableBufferSequence>(
        descriptor, buffers, &descriptor_read_op::do_complete),
      handler_(EGT_ASIO_MOVE_CAST(Handler)(handler))
  {
    handler_work<Handler>::start(handler_);
  }

  static void do_complete(void* owner, operation* base,
      const egt::asio::error_code& /*ec*/,
      std::size_t /*bytes_transferred*/)
  {
    // Take ownership of the handler object.
    descriptor_read_op* o(static_cast<descriptor_read_op*>(base));
    ptr p = { egt::asio::detail::addressof(o->handler_), o, o };
    handler_work<Handler> w(o->handler_);

    EGT_ASIO_HANDLER_COMPLETION((*o));

    // Make a copy of the handler so that the memory can be deallocated before
    // the upcall is made. Even if we're not about to make an upcall, a
    // sub-object of the handler may be the true owner of the memory associated
    // with the handler. Consequently, a local copy of the handler is required
    // to ensure that any owning sub-object remains valid until after we have
    // deallocated the memory here.
    detail::binder2<Handler, egt::asio::error_code, std::size_t>
      handler(o->handler_, o->ec_, o->bytes_transferred_);
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

#endif // !defined(EGT_ASIO_WINDOWS) && !defined(__CYGWIN__)

#endif // EGT_ASIO_DETAIL_DESCRIPTOR_READ_OP_HPP
