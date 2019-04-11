//
// ssl/detail/write_op.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_SSL_DETAIL_WRITE_OP_HPP
#define EGT_ASIO_SSL_DETAIL_WRITE_OP_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

#include <egt/asio/detail/buffer_sequence_adapter.hpp>
#include <egt/asio/ssl/detail/engine.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace ssl {
namespace detail {

template <typename ConstBufferSequence>
class write_op
{
public:
  write_op(const ConstBufferSequence& buffers)
    : buffers_(buffers)
  {
  }

  engine::want operator()(engine& eng,
      egt::asio::error_code& ec,
      std::size_t& bytes_transferred) const
  {
    egt::asio::const_buffer buffer =
      egt::asio::detail::buffer_sequence_adapter<egt::asio::const_buffer,
        ConstBufferSequence>::first(buffers_);

    return eng.write(buffer, ec, bytes_transferred);
  }

  template <typename Handler>
  void call_handler(Handler& handler,
      const egt::asio::error_code& ec,
      const std::size_t& bytes_transferred) const
  {
    handler(ec, bytes_transferred);
  }

private:
  ConstBufferSequence buffers_;
};

} // namespace detail
} // namespace ssl
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#endif // EGT_ASIO_SSL_DETAIL_WRITE_OP_HPP
