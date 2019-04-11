//
// asio.hpp
// ~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//  See www.egt.org/libs/asio for documentation.
//

#ifndef EGT_ASIO_HPP
#define EGT_ASIO_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/associated_allocator.hpp>
#include <egt/asio/associated_executor.hpp>
#include <egt/asio/async_result.hpp>
#include <egt/asio/basic_datagram_socket.hpp>
#include <egt/asio/basic_deadline_timer.hpp>
#include <egt/asio/basic_io_object.hpp>
#include <egt/asio/basic_raw_socket.hpp>
#include <egt/asio/basic_seq_packet_socket.hpp>
#include <egt/asio/basic_serial_port.hpp>
#include <egt/asio/basic_signal_set.hpp>
#include <egt/asio/basic_socket_acceptor.hpp>
#include <egt/asio/basic_socket_iostream.hpp>
#include <egt/asio/basic_socket_streambuf.hpp>
#include <egt/asio/basic_stream_socket.hpp>
#include <egt/asio/basic_streambuf.hpp>
#include <egt/asio/basic_waitable_timer.hpp>
#include <egt/asio/bind_executor.hpp>
#include <egt/asio/buffer.hpp>
#include <egt/asio/buffered_read_stream_fwd.hpp>
#include <egt/asio/buffered_read_stream.hpp>
#include <egt/asio/buffered_stream_fwd.hpp>
#include <egt/asio/buffered_stream.hpp>
#include <egt/asio/buffered_write_stream_fwd.hpp>
#include <egt/asio/buffered_write_stream.hpp>
#include <egt/asio/buffers_iterator.hpp>
#include <egt/asio/completion_condition.hpp>
#include <egt/asio/connect.hpp>
#include <egt/asio/coroutine.hpp>
#include <egt/asio/datagram_socket_service.hpp>
#include <egt/asio/deadline_timer_service.hpp>
#include <egt/asio/deadline_timer.hpp>
#include <egt/asio/defer.hpp>
#include <egt/asio/dispatch.hpp>
#include <egt/asio/error.hpp>
#include <egt/asio/error_code.hpp>
#include <egt/asio/execution_context.hpp>
#include <egt/asio/executor.hpp>
#include <egt/asio/executor_work_guard.hpp>
#include <egt/asio/generic/basic_endpoint.hpp>
#include <egt/asio/generic/datagram_protocol.hpp>
#include <egt/asio/generic/raw_protocol.hpp>
#include <egt/asio/generic/seq_packet_protocol.hpp>
#include <egt/asio/generic/stream_protocol.hpp>
#include <egt/asio/handler_alloc_hook.hpp>
#include <egt/asio/handler_continuation_hook.hpp>
#include <egt/asio/handler_invoke_hook.hpp>
#include <egt/asio/handler_type.hpp>
#include <egt/asio/high_resolution_timer.hpp>
#include <egt/asio/io_context.hpp>
#include <egt/asio/io_context_strand.hpp>
#include <egt/asio/io_service.hpp>
#include <egt/asio/io_service_strand.hpp>
#include <egt/asio/ip/address.hpp>
#include <egt/asio/ip/address_v4.hpp>
#include <egt/asio/ip/address_v4_iterator.hpp>
#include <egt/asio/ip/address_v4_range.hpp>
#include <egt/asio/ip/address_v6.hpp>
#include <egt/asio/ip/address_v6_iterator.hpp>
#include <egt/asio/ip/address_v6_range.hpp>
#include <egt/asio/ip/bad_address_cast.hpp>
#include <egt/asio/ip/basic_endpoint.hpp>
#include <egt/asio/ip/basic_resolver.hpp>
#include <egt/asio/ip/basic_resolver_entry.hpp>
#include <egt/asio/ip/basic_resolver_iterator.hpp>
#include <egt/asio/ip/basic_resolver_query.hpp>
#include <egt/asio/ip/host_name.hpp>
#include <egt/asio/ip/icmp.hpp>
#include <egt/asio/ip/multicast.hpp>
#include <egt/asio/ip/resolver_base.hpp>
#include <egt/asio/ip/resolver_query_base.hpp>
#include <egt/asio/ip/resolver_service.hpp>
#include <egt/asio/ip/tcp.hpp>
#include <egt/asio/ip/udp.hpp>
#include <egt/asio/ip/unicast.hpp>
#include <egt/asio/ip/v6_only.hpp>
#include <egt/asio/is_executor.hpp>
#include <egt/asio/is_read_buffered.hpp>
#include <egt/asio/is_write_buffered.hpp>
#include <egt/asio/local/basic_endpoint.hpp>
#include <egt/asio/local/connect_pair.hpp>
#include <egt/asio/local/datagram_protocol.hpp>
#include <egt/asio/local/stream_protocol.hpp>
#include <egt/asio/packaged_task.hpp>
#include <egt/asio/placeholders.hpp>
#include <egt/asio/posix/basic_descriptor.hpp>
#include <egt/asio/posix/basic_stream_descriptor.hpp>
#include <egt/asio/posix/descriptor.hpp>
#include <egt/asio/posix/descriptor_base.hpp>
#include <egt/asio/posix/stream_descriptor.hpp>
#include <egt/asio/posix/stream_descriptor_service.hpp>
#include <egt/asio/post.hpp>
#include <egt/asio/raw_socket_service.hpp>
#include <egt/asio/read.hpp>
#include <egt/asio/read_at.hpp>
#include <egt/asio/read_until.hpp>
#include <egt/asio/seq_packet_socket_service.hpp>
#include <egt/asio/serial_port.hpp>
#include <egt/asio/serial_port_base.hpp>
#include <egt/asio/serial_port_service.hpp>
#include <egt/asio/signal_set.hpp>
#include <egt/asio/signal_set_service.hpp>
#include <egt/asio/socket_acceptor_service.hpp>
#include <egt/asio/socket_base.hpp>
#include <egt/asio/steady_timer.hpp>
#include <egt/asio/strand.hpp>
#include <egt/asio/stream_socket_service.hpp>
#include <egt/asio/streambuf.hpp>
#include <egt/asio/system_context.hpp>
#include <egt/asio/system_error.hpp>
#include <egt/asio/system_executor.hpp>
#include <egt/asio/system_timer.hpp>
#include <egt/asio/thread.hpp>
#include <egt/asio/thread_pool.hpp>
#include <egt/asio/time_traits.hpp>
#include <egt/asio/use_future.hpp>
#include <egt/asio/uses_executor.hpp>
#include <egt/asio/version.hpp>
#include <egt/asio/wait_traits.hpp>
#include <egt/asio/waitable_timer_service.hpp>
#include <egt/asio/windows/basic_handle.hpp>
#include <egt/asio/windows/basic_object_handle.hpp>
#include <egt/asio/windows/basic_random_access_handle.hpp>
#include <egt/asio/windows/basic_stream_handle.hpp>
#include <egt/asio/windows/object_handle.hpp>
#include <egt/asio/windows/object_handle_service.hpp>
#include <egt/asio/windows/overlapped_handle.hpp>
#include <egt/asio/windows/overlapped_ptr.hpp>
#include <egt/asio/windows/random_access_handle.hpp>
#include <egt/asio/windows/random_access_handle_service.hpp>
#include <egt/asio/windows/stream_handle.hpp>
#include <egt/asio/windows/stream_handle_service.hpp>
#include <egt/asio/write.hpp>
#include <egt/asio/write_at.hpp>

#endif // EGT_ASIO_HPP
