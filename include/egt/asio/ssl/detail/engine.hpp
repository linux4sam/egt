//
// ssl/detail/engine.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_SSL_DETAIL_ENGINE_HPP
#define EGT_ASIO_SSL_DETAIL_ENGINE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

#include <egt/asio/buffer.hpp>
#include <egt/asio/detail/static_mutex.hpp>
#include <egt/asio/ssl/detail/openssl_types.hpp>
#include <egt/asio/ssl/detail/verify_callback.hpp>
#include <egt/asio/ssl/stream_base.hpp>
#include <egt/asio/ssl/verify_mode.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace ssl {
namespace detail {

class engine
{
public:
  enum want
  {
    // Returned by functions to indicate that the engine wants input. The input
    // buffer should be updated to point to the data. The engine then needs to
    // be called again to retry the operation.
    want_input_and_retry = -2,

    // Returned by functions to indicate that the engine wants to write output.
    // The output buffer points to the data to be written. The engine then
    // needs to be called again to retry the operation.
    want_output_and_retry = -1,

    // Returned by functions to indicate that the engine doesn't need input or
    // output.
    want_nothing = 0,

    // Returned by functions to indicate that the engine wants to write output.
    // The output buffer points to the data to be written. After that the
    // operation is complete, and the engine does not need to be called again.
    want_output = 1
  };

  // Construct a new engine for the specified context.
  EGT_ASIO_DECL explicit engine(SSL_CTX* context);

  // Destructor.
  EGT_ASIO_DECL ~engine();

  // Get the underlying implementation in the native type.
  EGT_ASIO_DECL SSL* native_handle();

  // Set the peer verification mode.
  EGT_ASIO_DECL egt::asio::error_code set_verify_mode(
      verify_mode v, egt::asio::error_code& ec);

  // Set the peer verification depth.
  EGT_ASIO_DECL egt::asio::error_code set_verify_depth(
      int depth, egt::asio::error_code& ec);

  // Set a peer certificate verification callback.
  EGT_ASIO_DECL egt::asio::error_code set_verify_callback(
      verify_callback_base* callback, egt::asio::error_code& ec);

  // Perform an SSL handshake using either SSL_connect (client-side) or
  // SSL_accept (server-side).
  EGT_ASIO_DECL want handshake(
      stream_base::handshake_type type, egt::asio::error_code& ec);

  // Perform a graceful shutdown of the SSL session.
  EGT_ASIO_DECL want shutdown(egt::asio::error_code& ec);

  // Write bytes to the SSL session.
  EGT_ASIO_DECL want write(const egt::asio::const_buffer& data,
      egt::asio::error_code& ec, std::size_t& bytes_transferred);

  // Read bytes from the SSL session.
  EGT_ASIO_DECL want read(const egt::asio::mutable_buffer& data,
      egt::asio::error_code& ec, std::size_t& bytes_transferred);

  // Get output data to be written to the transport.
  EGT_ASIO_DECL egt::asio::mutable_buffer get_output(
      const egt::asio::mutable_buffer& data);

  // Put input data that was read from the transport.
  EGT_ASIO_DECL egt::asio::const_buffer put_input(
      const egt::asio::const_buffer& data);

  // Map an error::eof code returned by the underlying transport according to
  // the type and state of the SSL session. Returns a const reference to the
  // error code object, suitable for passing to a completion handler.
  EGT_ASIO_DECL const egt::asio::error_code& map_error_code(
      egt::asio::error_code& ec) const;

private:
  // Disallow copying and assignment.
  engine(const engine&);
  engine& operator=(const engine&);

  // Callback used when the SSL implementation wants to verify a certificate.
  EGT_ASIO_DECL static int verify_callback_function(
      int preverified, X509_STORE_CTX* ctx);

#if (OPENSSL_VERSION_NUMBER < 0x10000000L)
  // The SSL_accept function may not be thread safe. This mutex is used to
  // protect all calls to the SSL_accept function.
  EGT_ASIO_DECL static egt::asio::detail::static_mutex& accept_mutex();
#endif // (OPENSSL_VERSION_NUMBER < 0x10000000L)

  // Perform one operation. Returns >= 0 on success or error, want_read if the
  // operation needs more input, or want_write if it needs to write some output
  // before the operation can complete.
  EGT_ASIO_DECL want perform(int (engine::* op)(void*, std::size_t),
      void* data, std::size_t length, egt::asio::error_code& ec,
      std::size_t* bytes_transferred);

  // Adapt the SSL_accept function to the signature needed for perform().
  EGT_ASIO_DECL int do_accept(void*, std::size_t);

  // Adapt the SSL_connect function to the signature needed for perform().
  EGT_ASIO_DECL int do_connect(void*, std::size_t);

  // Adapt the SSL_shutdown function to the signature needed for perform().
  EGT_ASIO_DECL int do_shutdown(void*, std::size_t);

  // Adapt the SSL_read function to the signature needed for perform().
  EGT_ASIO_DECL int do_read(void* data, std::size_t length);

  // Adapt the SSL_write function to the signature needed for perform().
  EGT_ASIO_DECL int do_write(void* data, std::size_t length);

  SSL* ssl_;
  BIO* ext_bio_;
};

} // namespace detail
} // namespace ssl
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#if defined(EGT_ASIO_HEADER_ONLY)
# include <egt/asio/ssl/detail/impl/engine.ipp>
#endif // defined(EGT_ASIO_HEADER_ONLY)

#endif // EGT_ASIO_SSL_DETAIL_ENGINE_HPP
