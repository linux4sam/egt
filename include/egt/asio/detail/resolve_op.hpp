//
// detail/resolve_op.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_DETAIL_RESOLVE_OP_HPP
#define EGT_ASIO_DETAIL_RESOLVE_OP_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>
#include <egt/asio/error.hpp>
#include <egt/asio/detail/operation.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace detail {

class resolve_op : public operation
{
public:
  // The error code to be passed to the completion handler.
  egt::asio::error_code ec_;

protected:
  resolve_op(func_type complete_func)
    : operation(complete_func)
  {
  }
};

} // namespace detail
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#endif // EGT_ASIO_DETAIL_RESOLVE_OP_HPP
