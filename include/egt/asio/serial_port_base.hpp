//
// serial_port_base.hpp
// ~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Copyright (c) 2008 Rep Invariant Systems, Inc. (info@repinvariant.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_SERIAL_PORT_BASE_HPP
#define EGT_ASIO_SERIAL_PORT_BASE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

#if defined(EGT_ASIO_HAS_SERIAL_PORT) \
  || defined(GENERATING_DOCUMENTATION)

#if !defined(EGT_ASIO_WINDOWS) && !defined(__CYGWIN__)
# include <termios.h>
#endif // !defined(EGT_ASIO_WINDOWS) && !defined(__CYGWIN__)

#include <egt/asio/detail/socket_types.hpp>
#include <egt/asio/error_code.hpp>

#if defined(GENERATING_DOCUMENTATION)
# define EGT_ASIO_OPTION_STORAGE implementation_defined
#elif defined(EGT_ASIO_WINDOWS) || defined(__CYGWIN__)
# define EGT_ASIO_OPTION_STORAGE DCB
#else
# define EGT_ASIO_OPTION_STORAGE termios
#endif

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {

/// The serial_port_base class is used as a base for the basic_serial_port class
/// template so that we have a common place to define the serial port options.
class serial_port_base
{
public:
  /// Serial port option to permit changing the baud rate.
  /**
   * Implements changing the baud rate for a given serial port.
   */
  class baud_rate
  {
  public:
    explicit baud_rate(unsigned int rate = 0);
    unsigned int value() const;
    EGT_ASIO_DECL EGT_ASIO_SYNC_OP_VOID store(
        EGT_ASIO_OPTION_STORAGE& storage,
        egt::asio::error_code& ec) const;
    EGT_ASIO_DECL EGT_ASIO_SYNC_OP_VOID load(
        const EGT_ASIO_OPTION_STORAGE& storage,
        egt::asio::error_code& ec);
  private:
    unsigned int value_;
  };

  /// Serial port option to permit changing the flow control.
  /**
   * Implements changing the flow control for a given serial port.
   */
  class flow_control
  {
  public:
    enum type { none, software, hardware };
    EGT_ASIO_DECL explicit flow_control(type t = none);
    type value() const;
    EGT_ASIO_DECL EGT_ASIO_SYNC_OP_VOID store(
        EGT_ASIO_OPTION_STORAGE& storage,
        egt::asio::error_code& ec) const;
    EGT_ASIO_DECL EGT_ASIO_SYNC_OP_VOID load(
        const EGT_ASIO_OPTION_STORAGE& storage,
        egt::asio::error_code& ec);
  private:
    type value_;
  };

  /// Serial port option to permit changing the parity.
  /**
   * Implements changing the parity for a given serial port.
   */
  class parity
  {
  public:
    enum type { none, odd, even };
    EGT_ASIO_DECL explicit parity(type t = none);
    type value() const;
    EGT_ASIO_DECL EGT_ASIO_SYNC_OP_VOID store(
        EGT_ASIO_OPTION_STORAGE& storage,
        egt::asio::error_code& ec) const;
    EGT_ASIO_DECL EGT_ASIO_SYNC_OP_VOID load(
        const EGT_ASIO_OPTION_STORAGE& storage,
        egt::asio::error_code& ec);
  private:
    type value_;
  };

  /// Serial port option to permit changing the number of stop bits.
  /**
   * Implements changing the number of stop bits for a given serial port.
   */
  class stop_bits
  {
  public:
    enum type { one, onepointfive, two };
    EGT_ASIO_DECL explicit stop_bits(type t = one);
    type value() const;
    EGT_ASIO_DECL EGT_ASIO_SYNC_OP_VOID store(
        EGT_ASIO_OPTION_STORAGE& storage,
        egt::asio::error_code& ec) const;
    EGT_ASIO_DECL EGT_ASIO_SYNC_OP_VOID load(
        const EGT_ASIO_OPTION_STORAGE& storage,
        egt::asio::error_code& ec);
  private:
    type value_;
  };

  /// Serial port option to permit changing the character size.
  /**
   * Implements changing the character size for a given serial port.
   */
  class character_size
  {
  public:
    EGT_ASIO_DECL explicit character_size(unsigned int t = 8);
    unsigned int value() const;
    EGT_ASIO_DECL EGT_ASIO_SYNC_OP_VOID store(
        EGT_ASIO_OPTION_STORAGE& storage,
        egt::asio::error_code& ec) const;
    EGT_ASIO_DECL EGT_ASIO_SYNC_OP_VOID load(
        const EGT_ASIO_OPTION_STORAGE& storage,
        egt::asio::error_code& ec);
  private:
    unsigned int value_;
  };

protected:
  /// Protected destructor to prevent deletion through this type.
  ~serial_port_base()
  {
  }
};

} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#undef EGT_ASIO_OPTION_STORAGE

#include <egt/asio/impl/serial_port_base.hpp>
#if defined(EGT_ASIO_HEADER_ONLY)
# include <egt/asio/impl/serial_port_base.ipp>
#endif // defined(EGT_ASIO_HEADER_ONLY)

#endif // defined(EGT_ASIO_HAS_SERIAL_PORT)
       //   || defined(GENERATING_DOCUMENTATION)

#endif // EGT_ASIO_SERIAL_PORT_BASE_HPP
