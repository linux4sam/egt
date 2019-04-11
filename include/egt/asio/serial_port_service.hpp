//
// serial_port_service.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_SERIAL_PORT_SERVICE_HPP
#define EGT_ASIO_SERIAL_PORT_SERVICE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

#if defined(EGT_ASIO_ENABLE_OLD_SERVICES)

#if defined(EGT_ASIO_HAS_SERIAL_PORT) \
  || defined(GENERATING_DOCUMENTATION)

#include <cstddef>
#include <string>
#include <egt/asio/async_result.hpp>
#include <egt/asio/detail/reactive_serial_port_service.hpp>
#include <egt/asio/detail/win_iocp_serial_port_service.hpp>
#include <egt/asio/error.hpp>
#include <egt/asio/io_context.hpp>
#include <egt/asio/serial_port_base.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {

/// Default service implementation for a serial port.
class serial_port_service
#if defined(GENERATING_DOCUMENTATION)
  : public egt::asio::io_context::service
#else
  : public egt::asio::detail::service_base<serial_port_service>
#endif
{
public:
#if defined(GENERATING_DOCUMENTATION)
  /// The unique service identifier.
  static egt::asio::io_context::id id;
#endif

private:
  // The type of the platform-specific implementation.
#if defined(EGT_ASIO_HAS_IOCP)
  typedef detail::win_iocp_serial_port_service service_impl_type;
#else
  typedef detail::reactive_serial_port_service service_impl_type;
#endif

public:
  /// The type of a serial port implementation.
#if defined(GENERATING_DOCUMENTATION)
  typedef implementation_defined implementation_type;
#else
  typedef service_impl_type::implementation_type implementation_type;
#endif

  /// The native handle type.
#if defined(GENERATING_DOCUMENTATION)
  typedef implementation_defined native_handle_type;
#else
  typedef service_impl_type::native_handle_type native_handle_type;
#endif

  /// Construct a new serial port service for the specified io_context.
  explicit serial_port_service(egt::asio::io_context& io_context)
    : egt::asio::detail::service_base<serial_port_service>(io_context),
      service_impl_(io_context)
  {
  }

  /// Construct a new serial port implementation.
  void construct(implementation_type& impl)
  {
    service_impl_.construct(impl);
  }

#if defined(EGT_ASIO_HAS_MOVE) || defined(GENERATING_DOCUMENTATION)
  /// Move-construct a new serial port implementation.
  void move_construct(implementation_type& impl,
      implementation_type& other_impl)
  {
    service_impl_.move_construct(impl, other_impl);
  }

  /// Move-assign from another serial port implementation.
  void move_assign(implementation_type& impl,
      serial_port_service& other_service,
      implementation_type& other_impl)
  {
    service_impl_.move_assign(impl, other_service.service_impl_, other_impl);
  }
#endif // defined(EGT_ASIO_HAS_MOVE) || defined(GENERATING_DOCUMENTATION)

  /// Destroy a serial port implementation.
  void destroy(implementation_type& impl)
  {
    service_impl_.destroy(impl);
  }

  /// Open a serial port.
  EGT_ASIO_SYNC_OP_VOID open(implementation_type& impl,
      const std::string& device, egt::asio::error_code& ec)
  {
    service_impl_.open(impl, device, ec);
    EGT_ASIO_SYNC_OP_VOID_RETURN(ec);
  }

  /// Assign an existing native handle to a serial port.
  EGT_ASIO_SYNC_OP_VOID assign(implementation_type& impl,
      const native_handle_type& handle, egt::asio::error_code& ec)
  {
    service_impl_.assign(impl, handle, ec);
    EGT_ASIO_SYNC_OP_VOID_RETURN(ec);
  }

  /// Determine whether the handle is open.
  bool is_open(const implementation_type& impl) const
  {
    return service_impl_.is_open(impl);
  }

  /// Close a serial port implementation.
  EGT_ASIO_SYNC_OP_VOID close(implementation_type& impl,
      egt::asio::error_code& ec)
  {
    service_impl_.close(impl, ec);
    EGT_ASIO_SYNC_OP_VOID_RETURN(ec);
  }

  /// Get the native handle implementation.
  native_handle_type native_handle(implementation_type& impl)
  {
    return service_impl_.native_handle(impl);
  }

  /// Cancel all asynchronous operations associated with the handle.
  EGT_ASIO_SYNC_OP_VOID cancel(implementation_type& impl,
      egt::asio::error_code& ec)
  {
    service_impl_.cancel(impl, ec);
    EGT_ASIO_SYNC_OP_VOID_RETURN(ec);
  }

  /// Set a serial port option.
  template <typename SettableSerialPortOption>
  EGT_ASIO_SYNC_OP_VOID set_option(implementation_type& impl,
      const SettableSerialPortOption& option, egt::asio::error_code& ec)
  {
    service_impl_.set_option(impl, option, ec);
    EGT_ASIO_SYNC_OP_VOID_RETURN(ec);
  }

  /// Get a serial port option.
  template <typename GettableSerialPortOption>
  EGT_ASIO_SYNC_OP_VOID get_option(const implementation_type& impl,
      GettableSerialPortOption& option, egt::asio::error_code& ec) const
  {
    service_impl_.get_option(impl, option, ec);
    EGT_ASIO_SYNC_OP_VOID_RETURN(ec);
  }

  /// Send a break sequence to the serial port.
  EGT_ASIO_SYNC_OP_VOID send_break(implementation_type& impl,
      egt::asio::error_code& ec)
  {
    service_impl_.send_break(impl, ec);
    EGT_ASIO_SYNC_OP_VOID_RETURN(ec);
  }

  /// Write the given data to the stream.
  template <typename ConstBufferSequence>
  std::size_t write_some(implementation_type& impl,
      const ConstBufferSequence& buffers, egt::asio::error_code& ec)
  {
    return service_impl_.write_some(impl, buffers, ec);
  }

  /// Start an asynchronous write.
  template <typename ConstBufferSequence, typename WriteHandler>
  EGT_ASIO_INITFN_RESULT_TYPE(WriteHandler,
      void (egt::asio::error_code, std::size_t))
  async_write_some(implementation_type& impl,
      const ConstBufferSequence& buffers,
      EGT_ASIO_MOVE_ARG(WriteHandler) handler)
  {
    async_completion<WriteHandler,
      void (egt::asio::error_code, std::size_t)> init(handler);

    service_impl_.async_write_some(impl, buffers, init.completion_handler);

    return init.result.get();
  }

  /// Read some data from the stream.
  template <typename MutableBufferSequence>
  std::size_t read_some(implementation_type& impl,
      const MutableBufferSequence& buffers, egt::asio::error_code& ec)
  {
    return service_impl_.read_some(impl, buffers, ec);
  }

  /// Start an asynchronous read.
  template <typename MutableBufferSequence, typename ReadHandler>
  EGT_ASIO_INITFN_RESULT_TYPE(ReadHandler,
      void (egt::asio::error_code, std::size_t))
  async_read_some(implementation_type& impl,
      const MutableBufferSequence& buffers,
      EGT_ASIO_MOVE_ARG(ReadHandler) handler)
  {
    async_completion<ReadHandler,
      void (egt::asio::error_code, std::size_t)> init(handler);

    service_impl_.async_read_some(impl, buffers, init.completion_handler);

    return init.result.get();
  }

private:
  // Destroy all user-defined handler objects owned by the service.
  void shutdown()
  {
    service_impl_.shutdown();
  }

  // The platform-specific implementation.
  service_impl_type service_impl_;
};

} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#endif // defined(EGT_ASIO_HAS_SERIAL_PORT)
       //   || defined(GENERATING_DOCUMENTATION)

#endif // defined(EGT_ASIO_ENABLE_OLD_SERVICES)

#endif // EGT_ASIO_SERIAL_PORT_SERVICE_HPP
