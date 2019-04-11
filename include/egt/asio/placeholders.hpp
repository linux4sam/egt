//
// placeholders.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_PLACEHOLDERS_HPP
#define EGT_ASIO_PLACEHOLDERS_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

#if defined(EGT_ASIO_HAS_BOOST_BIND)
# include <boost/bind/arg.hpp>
#endif // defined(EGT_ASIO_HAS_BOOST_BIND)

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace placeholders {

#if defined(GENERATING_DOCUMENTATION)

/// An argument placeholder, for use with boost::bind(), that corresponds to
/// the error argument of a handler for any of the asynchronous functions.
unspecified error;

/// An argument placeholder, for use with boost::bind(), that corresponds to
/// the bytes_transferred argument of a handler for asynchronous functions such
/// as egt::asio::basic_stream_socket::async_write_some or
/// egt::asio::async_write.
unspecified bytes_transferred;

/// An argument placeholder, for use with boost::bind(), that corresponds to
/// the iterator argument of a handler for asynchronous functions such as
/// egt::asio::async_connect.
unspecified iterator;

/// An argument placeholder, for use with boost::bind(), that corresponds to
/// the results argument of a handler for asynchronous functions such as
/// egt::asio::basic_resolver::async_resolve.
unspecified results;

/// An argument placeholder, for use with boost::bind(), that corresponds to
/// the results argument of a handler for asynchronous functions such as
/// egt::asio::async_connect.
unspecified endpoint;

/// An argument placeholder, for use with boost::bind(), that corresponds to
/// the signal_number argument of a handler for asynchronous functions such as
/// egt::asio::signal_set::async_wait.
unspecified signal_number;

#elif defined(EGT_ASIO_HAS_BOOST_BIND)
# if defined(__BORLANDC__) || defined(__GNUC__)

inline boost::arg<1> error()
{
  return boost::arg<1>();
}

inline boost::arg<2> bytes_transferred()
{
  return boost::arg<2>();
}

inline boost::arg<2> iterator()
{
  return boost::arg<2>();
}

inline boost::arg<2> results()
{
  return boost::arg<2>();
}

inline boost::arg<2> endpoint()
{
  return boost::arg<2>();
}

inline boost::arg<2> signal_number()
{
  return boost::arg<2>();
}

# else

namespace detail
{
  template <int Number>
  struct placeholder
  {
    static boost::arg<Number>& get()
    {
      static boost::arg<Number> result;
      return result;
    }
  };
}

#  if defined(EGT_ASIO_MSVC) && (EGT_ASIO_MSVC < 1400)

static boost::arg<1>& error
  = egt::asio::placeholders::detail::placeholder<1>::get();
static boost::arg<2>& bytes_transferred
  = egt::asio::placeholders::detail::placeholder<2>::get();
static boost::arg<2>& iterator
  = egt::asio::placeholders::detail::placeholder<2>::get();
static boost::arg<2>& results
  = egt::asio::placeholders::detail::placeholder<2>::get();
static boost::arg<2>& endpoint
  = egt::asio::placeholders::detail::placeholder<2>::get();
static boost::arg<2>& signal_number
  = egt::asio::placeholders::detail::placeholder<2>::get();

#  else

namespace
{
  boost::arg<1>& error
    = egt::asio::placeholders::detail::placeholder<1>::get();
  boost::arg<2>& bytes_transferred
    = egt::asio::placeholders::detail::placeholder<2>::get();
  boost::arg<2>& iterator
    = egt::asio::placeholders::detail::placeholder<2>::get();
  boost::arg<2>& results
    = egt::asio::placeholders::detail::placeholder<2>::get();
  boost::arg<2>& endpoint
    = egt::asio::placeholders::detail::placeholder<2>::get();
  boost::arg<2>& signal_number
    = egt::asio::placeholders::detail::placeholder<2>::get();
} // namespace

#  endif
# endif
#endif

} // namespace placeholders
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#endif // EGT_ASIO_PLACEHOLDERS_HPP
