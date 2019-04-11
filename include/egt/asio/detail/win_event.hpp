//
// detail/win_event.hpp
// ~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_DETAIL_WIN_EVENT_HPP
#define EGT_ASIO_DETAIL_WIN_EVENT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

#if defined(EGT_ASIO_WINDOWS)

#include <egt/asio/detail/assert.hpp>
#include <egt/asio/detail/noncopyable.hpp>
#include <egt/asio/detail/socket_types.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace detail {

class win_event
  : private noncopyable
{
public:
  // Constructor.
  EGT_ASIO_DECL win_event();

  // Destructor.
  EGT_ASIO_DECL ~win_event();

  // Signal the event. (Retained for backward compatibility.)
  template <typename Lock>
  void signal(Lock& lock)
  {
    this->signal_all(lock);
  }

  // Signal all waiters.
  template <typename Lock>
  void signal_all(Lock& lock)
  {
    EGT_ASIO_ASSERT(lock.locked());
    (void)lock;
    state_ |= 1;
    ::SetEvent(events_[0]);
  }

  // Unlock the mutex and signal one waiter.
  template <typename Lock>
  void unlock_and_signal_one(Lock& lock)
  {
    EGT_ASIO_ASSERT(lock.locked());
    state_ |= 1;
    bool have_waiters = (state_ > 1);
    lock.unlock();
    if (have_waiters)
      ::SetEvent(events_[1]);
  }

  // If there's a waiter, unlock the mutex and signal it.
  template <typename Lock>
  bool maybe_unlock_and_signal_one(Lock& lock)
  {
    EGT_ASIO_ASSERT(lock.locked());
    state_ |= 1;
    if (state_ > 1)
    {
      lock.unlock();
      ::SetEvent(events_[1]);
      return true;
    }
    return false;
  }

  // Reset the event.
  template <typename Lock>
  void clear(Lock& lock)
  {
    EGT_ASIO_ASSERT(lock.locked());
    (void)lock;
    ::ResetEvent(events_[0]);
    state_ &= ~std::size_t(1);
  }

  // Wait for the event to become signalled.
  template <typename Lock>
  void wait(Lock& lock)
  {
    EGT_ASIO_ASSERT(lock.locked());
    while ((state_ & 1) == 0)
    {
      state_ += 2;
      lock.unlock();
#if defined(EGT_ASIO_WINDOWS_APP)
      ::WaitForMultipleObjectsEx(2, events_, false, INFINITE, false);
#else // defined(EGT_ASIO_WINDOWS_APP)
      ::WaitForMultipleObjects(2, events_, false, INFINITE);
#endif // defined(EGT_ASIO_WINDOWS_APP)
      lock.lock();
      state_ -= 2;
    }
  }

  // Timed wait for the event to become signalled.
  template <typename Lock>
  bool wait_for_usec(Lock& lock, long usec)
  {
    EGT_ASIO_ASSERT(lock.locked());
    if ((state_ & 1) == 0)
    {
      state_ += 2;
      lock.unlock();
      DWORD msec = usec > 0 ? (usec < 1000 ? 1 : usec / 1000) : 0;
#if defined(EGT_ASIO_WINDOWS_APP)
      ::WaitForMultipleObjectsEx(2, events_, false, msec, false);
#else // defined(EGT_ASIO_WINDOWS_APP)
      ::WaitForMultipleObjects(2, events_, false, msec);
#endif // defined(EGT_ASIO_WINDOWS_APP)
      lock.lock();
      state_ -= 2;
    }
    return (state_ & 1) != 0;
  }

private:
  HANDLE events_[2];
  std::size_t state_;
};

} // namespace detail
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#if defined(EGT_ASIO_HEADER_ONLY)
# include <egt/asio/detail/impl/win_event.ipp>
#endif // defined(EGT_ASIO_HEADER_ONLY)

#endif // defined(EGT_ASIO_WINDOWS)

#endif // EGT_ASIO_DETAIL_WIN_EVENT_HPP
