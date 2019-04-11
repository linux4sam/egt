//
// detail/handler_invoke_helpers.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_DETAIL_HANDLER_INVOKE_HELPERS_HPP
#define EGT_ASIO_DETAIL_HANDLER_INVOKE_HELPERS_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>
#include <egt/asio/detail/memory.hpp>
#include <egt/asio/handler_invoke_hook.hpp>

#include <egt/asio/detail/push_options.hpp>

// Calls to asio_handler_invoke must be made from a namespace that does not
// contain overloads of this function. The egt_asio_handler_invoke_helpers
// namespace is defined here for that purpose.
namespace egt_asio_handler_invoke_helpers {

template <typename Function, typename Context>
inline void invoke(Function& function, Context& context)
{
#if !defined(EGT_ASIO_HAS_HANDLER_HOOKS)
  Function tmp(function);
  tmp();
#else
  using egt::asio::asio_handler_invoke;
  asio_handler_invoke(function, egt::asio::detail::addressof(context));
#endif
}

template <typename Function, typename Context>
inline void invoke(const Function& function, Context& context)
{
#if !defined(EGT_ASIO_HAS_HANDLER_HOOKS)
  Function tmp(function);
  tmp();
#else
  using egt::asio::asio_handler_invoke;
  asio_handler_invoke(function, egt::asio::detail::addressof(context));
#endif
}

} // namespace egt_asio_handler_invoke_helpers

#include <egt/asio/detail/pop_options.hpp>

#endif // EGT_ASIO_DETAIL_HANDLER_INVOKE_HELPERS_HPP
