//
// experimental/redirect_error.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_EXPERIMENTAL_REDIRECT_ERROR_HPP
#define EGT_ASIO_EXPERIMENTAL_REDIRECT_ERROR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>
#include <egt/asio/detail/type_traits.hpp>
#include <egt/asio/error_code.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace experimental {

/// Completion token type used to specify that an error produced by an
/// asynchronous operation is captured to an error_code variable.
/**
 * The redirect_error_t class is used to indicate that any error_code produced
 * by an asynchronous operation is captured to a specified variable.
 */
template <typename CompletionToken>
class redirect_error_t
{
public:
  /// Constructor. 
  template <typename T>
  redirect_error_t(EGT_ASIO_MOVE_ARG(T) completion_token,
      egt::asio::error_code& ec)
    : token_(EGT_ASIO_MOVE_CAST(T)(completion_token)),
      ec_(ec)
  {
  }

//private:
  CompletionToken token_;
  egt::asio::error_code& ec_;
};

/// Create a completion token to capture error_code values to a variable.
template <typename CompletionToken>
inline redirect_error_t<typename decay<CompletionToken>::type> redirect_error(
    CompletionToken&& completion_token, egt::asio::error_code& ec)
{
  return redirect_error_t<typename decay<CompletionToken>::type>(
      EGT_ASIO_MOVE_CAST(CompletionToken)(completion_token), ec);
}

} // namespace experimental
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#include <egt/asio/experimental/impl/redirect_error.hpp>

#endif // EGT_ASIO_EXPERIMENTAL_REDIRECT_ERROR_HPP
