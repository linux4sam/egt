//
// impl/system_context.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_IMPL_SYSTEM_CONTEXT_HPP
#define EGT_ASIO_IMPL_SYSTEM_CONTEXT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/system_executor.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {

inline system_context::executor_type
system_context::get_executor() EGT_ASIO_NOEXCEPT
{
  return system_executor();
}

} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#endif // EGT_ASIO_IMPL_SYSTEM_CONTEXT_HPP
