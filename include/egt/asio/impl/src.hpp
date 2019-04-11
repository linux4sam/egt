//
// impl/src.hpp
// ~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_IMPL_SRC_HPP
#define EGT_ASIO_IMPL_SRC_HPP

#define EGT_ASIO_SOURCE

#include <egt/asio/detail/config.hpp>

#if defined(EGT_ASIO_HEADER_ONLY)
# error Do not compile Asio library source with EGT_ASIO_HEADER_ONLY defined
#endif

#include <egt/asio/impl/error.ipp>
#include <egt/asio/impl/error_code.ipp>
#include <egt/asio/impl/execution_context.ipp>
#include <egt/asio/impl/executor.ipp>
#include <egt/asio/impl/handler_alloc_hook.ipp>
#include <egt/asio/impl/io_context.ipp>
#include <egt/asio/impl/serial_port_base.ipp>
#include <egt/asio/impl/system_context.ipp>
#include <egt/asio/impl/thread_pool.ipp>
#include <egt/asio/detail/impl/buffer_sequence_adapter.ipp>
#include <egt/asio/detail/impl/descriptor_ops.ipp>
#include <egt/asio/detail/impl/dev_poll_reactor.ipp>
#include <egt/asio/detail/impl/epoll_reactor.ipp>
#include <egt/asio/detail/impl/eventfd_select_interrupter.ipp>
#include <egt/asio/detail/impl/handler_tracking.ipp>
#include <egt/asio/detail/impl/kqueue_reactor.ipp>
#include <egt/asio/detail/impl/null_event.ipp>
#include <egt/asio/detail/impl/pipe_select_interrupter.ipp>
#include <egt/asio/detail/impl/posix_event.ipp>
#include <egt/asio/detail/impl/posix_mutex.ipp>
#include <egt/asio/detail/impl/posix_thread.ipp>
#include <egt/asio/detail/impl/posix_tss_ptr.ipp>
#include <egt/asio/detail/impl/reactive_descriptor_service.ipp>
#include <egt/asio/detail/impl/reactive_serial_port_service.ipp>
#include <egt/asio/detail/impl/reactive_socket_service_base.ipp>
#include <egt/asio/detail/impl/resolver_service_base.ipp>
#include <egt/asio/detail/impl/scheduler.ipp>
#include <egt/asio/detail/impl/select_reactor.ipp>
#include <egt/asio/detail/impl/service_registry.ipp>
#include <egt/asio/detail/impl/signal_set_service.ipp>
#include <egt/asio/detail/impl/socket_ops.ipp>
#include <egt/asio/detail/impl/socket_select_interrupter.ipp>
#include <egt/asio/detail/impl/strand_executor_service.ipp>
#include <egt/asio/detail/impl/strand_service.ipp>
#include <egt/asio/detail/impl/throw_error.ipp>
#include <egt/asio/detail/impl/timer_queue_ptime.ipp>
#include <egt/asio/detail/impl/timer_queue_set.ipp>
#include <egt/asio/detail/impl/win_iocp_handle_service.ipp>
#include <egt/asio/detail/impl/win_iocp_io_context.ipp>
#include <egt/asio/detail/impl/win_iocp_serial_port_service.ipp>
#include <egt/asio/detail/impl/win_iocp_socket_service_base.ipp>
#include <egt/asio/detail/impl/win_event.ipp>
#include <egt/asio/detail/impl/win_mutex.ipp>
#include <egt/asio/detail/impl/win_object_handle_service.ipp>
#include <egt/asio/detail/impl/win_static_mutex.ipp>
#include <egt/asio/detail/impl/win_thread.ipp>
#include <egt/asio/detail/impl/win_tss_ptr.ipp>
#include <egt/asio/detail/impl/winrt_ssocket_service_base.ipp>
#include <egt/asio/detail/impl/winrt_timer_scheduler.ipp>
#include <egt/asio/detail/impl/winsock_init.ipp>
#include <egt/asio/generic/detail/impl/endpoint.ipp>
#include <egt/asio/ip/impl/address.ipp>
#include <egt/asio/ip/impl/address_v4.ipp>
#include <egt/asio/ip/impl/address_v6.ipp>
#include <egt/asio/ip/impl/host_name.ipp>
#include <egt/asio/ip/impl/network_v4.ipp>
#include <egt/asio/ip/impl/network_v6.ipp>
#include <egt/asio/ip/detail/impl/endpoint.ipp>
#include <egt/asio/local/detail/impl/endpoint.ipp>

#endif // EGT_ASIO_IMPL_SRC_HPP
