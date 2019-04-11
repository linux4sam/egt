//
// ts/executor.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_TS_EXECUTOR_HPP
#define EGT_ASIO_TS_EXECUTOR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/handler_type.hpp>
#include <egt/asio/async_result.hpp>
#include <egt/asio/associated_allocator.hpp>
#include <egt/asio/execution_context.hpp>
#include <egt/asio/is_executor.hpp>
#include <egt/asio/associated_executor.hpp>
#include <egt/asio/bind_executor.hpp>
#include <egt/asio/executor_work_guard.hpp>
#include <egt/asio/system_executor.hpp>
#include <egt/asio/executor.hpp>
#include <egt/asio/dispatch.hpp>
#include <egt/asio/post.hpp>
#include <egt/asio/defer.hpp>
#include <egt/asio/strand.hpp>
#include <egt/asio/packaged_task.hpp>
#include <egt/asio/use_future.hpp>

#endif // EGT_ASIO_TS_EXECUTOR_HPP
