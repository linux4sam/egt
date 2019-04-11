//
// detail/posix_thread.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_DETAIL_POSIX_THREAD_HPP
#define EGT_ASIO_DETAIL_POSIX_THREAD_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

#if defined(EGT_ASIO_HAS_PTHREADS)

#include <cstddef>
#include <pthread.h>
#include <egt/asio/detail/noncopyable.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace detail {

extern "C"
{
  EGT_ASIO_DECL void* egt_asio_detail_posix_thread_function(void* arg);
}

class posix_thread
  : private noncopyable
{
public:
  // Constructor.
  template <typename Function>
  posix_thread(Function f, unsigned int = 0)
    : joined_(false)
  {
    start_thread(new func<Function>(f));
  }

  // Destructor.
  EGT_ASIO_DECL ~posix_thread();

  // Wait for the thread to exit.
  EGT_ASIO_DECL void join();

  // Get number of CPUs.
  EGT_ASIO_DECL static std::size_t hardware_concurrency();

private:
  friend void* egt_asio_detail_posix_thread_function(void* arg);

  class func_base
  {
  public:
    virtual ~func_base() {}
    virtual void run() = 0;
  };

  struct auto_func_base_ptr
  {
    func_base* ptr;
    ~auto_func_base_ptr() { delete ptr; }
  };

  template <typename Function>
  class func
    : public func_base
  {
  public:
    func(Function f)
      : f_(f)
    {
    }

    virtual void run()
    {
      f_();
    }

  private:
    Function f_;
  };

  EGT_ASIO_DECL void start_thread(func_base* arg);

  ::pthread_t thread_;
  bool joined_;
};

} // namespace detail
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#if defined(EGT_ASIO_HEADER_ONLY)
# include <egt/asio/detail/impl/posix_thread.ipp>
#endif // defined(EGT_ASIO_HEADER_ONLY)

#endif // defined(EGT_ASIO_HAS_PTHREADS)

#endif // EGT_ASIO_DETAIL_POSIX_THREAD_HPP
