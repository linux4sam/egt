//
// io_service.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_IO_SERVICE_HPP
#define EGT_ASIO_IO_SERVICE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/io_context.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {

#if !defined(EGT_ASIO_NO_DEPRECATED)
/// Typedef for backwards compatibility.
typedef io_context io_service;
#endif // !defined(EGT_ASIO_NO_DEPRECATED)

} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#endif // EGT_ASIO_IO_SERVICE_HPP
