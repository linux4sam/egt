//
// impl/read.hpp
// ~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_IMPL_READ_HPP
#define EGT_ASIO_IMPL_READ_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <algorithm>
#include <egt/asio/associated_allocator.hpp>
#include <egt/asio/associated_executor.hpp>
#include <egt/asio/buffer.hpp>
#include <egt/asio/completion_condition.hpp>
#include <egt/asio/detail/array_fwd.hpp>
#include <egt/asio/detail/base_from_completion_cond.hpp>
#include <egt/asio/detail/bind_handler.hpp>
#include <egt/asio/detail/consuming_buffers.hpp>
#include <egt/asio/detail/dependent_type.hpp>
#include <egt/asio/detail/handler_alloc_helpers.hpp>
#include <egt/asio/detail/handler_cont_helpers.hpp>
#include <egt/asio/detail/handler_invoke_helpers.hpp>
#include <egt/asio/detail/handler_type_requirements.hpp>
#include <egt/asio/detail/throw_error.hpp>
#include <egt/asio/error.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {

namespace detail
{
  template <typename SyncReadStream, typename MutableBufferSequence,
      typename MutableBufferIterator, typename CompletionCondition>
  std::size_t read_buffer_sequence(SyncReadStream& s,
      const MutableBufferSequence& buffers, const MutableBufferIterator&,
      CompletionCondition completion_condition, egt::asio::error_code& ec)
  {
    ec = egt::asio::error_code();
    egt::asio::detail::consuming_buffers<mutable_buffer,
        MutableBufferSequence, MutableBufferIterator> tmp(buffers);
    while (!tmp.empty())
    {
      if (std::size_t max_size = detail::adapt_completion_condition_result(
            completion_condition(ec, tmp.total_consumed())))
        tmp.consume(s.read_some(tmp.prepare(max_size), ec));
      else
        break;
    }
    return tmp.total_consumed();;
  }
} // namespace detail

template <typename SyncReadStream, typename MutableBufferSequence,
    typename CompletionCondition>
std::size_t read(SyncReadStream& s, const MutableBufferSequence& buffers,
    CompletionCondition completion_condition, egt::asio::error_code& ec,
    typename enable_if<
      is_mutable_buffer_sequence<MutableBufferSequence>::value
    >::type*)
{
  return detail::read_buffer_sequence(s, buffers,
      egt::asio::buffer_sequence_begin(buffers), completion_condition, ec);
}

template <typename SyncReadStream, typename MutableBufferSequence>
inline std::size_t read(SyncReadStream& s, const MutableBufferSequence& buffers,
    typename enable_if<
      is_mutable_buffer_sequence<MutableBufferSequence>::value
    >::type*)
{
  egt::asio::error_code ec;
  std::size_t bytes_transferred = read(s, buffers, transfer_all(), ec);
  egt::asio::detail::throw_error(ec, "read");
  return bytes_transferred;
}

template <typename SyncReadStream, typename MutableBufferSequence>
inline std::size_t read(SyncReadStream& s, const MutableBufferSequence& buffers,
    egt::asio::error_code& ec,
    typename enable_if<
      is_mutable_buffer_sequence<MutableBufferSequence>::value
    >::type*)
{
  return read(s, buffers, transfer_all(), ec);
}

template <typename SyncReadStream, typename MutableBufferSequence,
    typename CompletionCondition>
inline std::size_t read(SyncReadStream& s, const MutableBufferSequence& buffers,
    CompletionCondition completion_condition,
    typename enable_if<
      is_mutable_buffer_sequence<MutableBufferSequence>::value
    >::type*)
{
  egt::asio::error_code ec;
  std::size_t bytes_transferred = read(s, buffers, completion_condition, ec);
  egt::asio::detail::throw_error(ec, "read");
  return bytes_transferred;
}

template <typename SyncReadStream, typename DynamicBuffer,
    typename CompletionCondition>
std::size_t read(SyncReadStream& s,
    EGT_ASIO_MOVE_ARG(DynamicBuffer) buffers,
    CompletionCondition completion_condition, egt::asio::error_code& ec,
    typename enable_if<
      is_dynamic_buffer<typename decay<DynamicBuffer>::type>::value
    >::type*)
{
  typename decay<DynamicBuffer>::type b(
      EGT_ASIO_MOVE_CAST(DynamicBuffer)(buffers));

  ec = egt::asio::error_code();
  std::size_t total_transferred = 0;
  std::size_t max_size = detail::adapt_completion_condition_result(
        completion_condition(ec, total_transferred));
  std::size_t bytes_available = std::min<std::size_t>(
        std::max<std::size_t>(512, b.capacity() - b.size()),
        std::min<std::size_t>(max_size, b.max_size() - b.size()));
  while (bytes_available > 0)
  {
    std::size_t bytes_transferred = s.read_some(b.prepare(bytes_available), ec);
    b.commit(bytes_transferred);
    total_transferred += bytes_transferred;
    max_size = detail::adapt_completion_condition_result(
          completion_condition(ec, total_transferred));
    bytes_available = std::min<std::size_t>(
          std::max<std::size_t>(512, b.capacity() - b.size()),
          std::min<std::size_t>(max_size, b.max_size() - b.size()));
  }
  return total_transferred;
}

template <typename SyncReadStream, typename DynamicBuffer>
inline std::size_t read(SyncReadStream& s,
    EGT_ASIO_MOVE_ARG(DynamicBuffer) buffers,
    typename enable_if<
      is_dynamic_buffer<typename decay<DynamicBuffer>::type>::value
    >::type*)
{
  egt::asio::error_code ec;
  std::size_t bytes_transferred = read(s,
      EGT_ASIO_MOVE_CAST(DynamicBuffer)(buffers), transfer_all(), ec);
  egt::asio::detail::throw_error(ec, "read");
  return bytes_transferred;
}

template <typename SyncReadStream, typename DynamicBuffer>
inline std::size_t read(SyncReadStream& s,
    EGT_ASIO_MOVE_ARG(DynamicBuffer) buffers,
    egt::asio::error_code& ec,
    typename enable_if<
      is_dynamic_buffer<typename decay<DynamicBuffer>::type>::value
    >::type*)
{
  return read(s, EGT_ASIO_MOVE_CAST(DynamicBuffer)(buffers),
      transfer_all(), ec);
}

template <typename SyncReadStream, typename DynamicBuffer,
    typename CompletionCondition>
inline std::size_t read(SyncReadStream& s,
    EGT_ASIO_MOVE_ARG(DynamicBuffer) buffers,
    CompletionCondition completion_condition,
    typename enable_if<
      is_dynamic_buffer<typename decay<DynamicBuffer>::type>::value
    >::type*)
{
  egt::asio::error_code ec;
  std::size_t bytes_transferred = read(s,
      EGT_ASIO_MOVE_CAST(DynamicBuffer)(buffers),
      completion_condition, ec);
  egt::asio::detail::throw_error(ec, "read");
  return bytes_transferred;
}

#if !defined(EGT_ASIO_NO_EXTENSIONS)
#if !defined(EGT_ASIO_NO_IOSTREAM)

template <typename SyncReadStream, typename Allocator,
    typename CompletionCondition>
inline std::size_t read(SyncReadStream& s,
    egt::asio::basic_streambuf<Allocator>& b,
    CompletionCondition completion_condition, egt::asio::error_code& ec)
{
  return read(s, basic_streambuf_ref<Allocator>(b), completion_condition, ec);
}

template <typename SyncReadStream, typename Allocator>
inline std::size_t read(SyncReadStream& s,
    egt::asio::basic_streambuf<Allocator>& b)
{
  return read(s, basic_streambuf_ref<Allocator>(b));
}

template <typename SyncReadStream, typename Allocator>
inline std::size_t read(SyncReadStream& s,
    egt::asio::basic_streambuf<Allocator>& b,
    egt::asio::error_code& ec)
{
  return read(s, basic_streambuf_ref<Allocator>(b), ec);
}

template <typename SyncReadStream, typename Allocator,
    typename CompletionCondition>
inline std::size_t read(SyncReadStream& s,
    egt::asio::basic_streambuf<Allocator>& b,
    CompletionCondition completion_condition)
{
  return read(s, basic_streambuf_ref<Allocator>(b), completion_condition);
}

#endif // !defined(EGT_ASIO_NO_IOSTREAM)
#endif // !defined(EGT_ASIO_NO_EXTENSIONS)

namespace detail
{
  template <typename AsyncReadStream, typename MutableBufferSequence,
      typename MutableBufferIterator, typename CompletionCondition,
      typename ReadHandler>
  class read_op
    : detail::base_from_completion_cond<CompletionCondition>
  {
  public:
    read_op(AsyncReadStream& stream, const MutableBufferSequence& buffers,
        CompletionCondition completion_condition, ReadHandler& handler)
      : detail::base_from_completion_cond<
          CompletionCondition>(completion_condition),
        stream_(stream),
        buffers_(buffers),
        start_(0),
        handler_(EGT_ASIO_MOVE_CAST(ReadHandler)(handler))
    {
    }

#if defined(EGT_ASIO_HAS_MOVE)
    read_op(const read_op& other)
      : detail::base_from_completion_cond<CompletionCondition>(other),
        stream_(other.stream_),
        buffers_(other.buffers_),
        start_(other.start_),
        handler_(other.handler_)
    {
    }

    read_op(read_op&& other)
      : detail::base_from_completion_cond<CompletionCondition>(other),
        stream_(other.stream_),
        buffers_(other.buffers_),
        start_(other.start_),
        handler_(EGT_ASIO_MOVE_CAST(ReadHandler)(other.handler_))
    {
    }
#endif // defined(EGT_ASIO_HAS_MOVE)

    void operator()(const egt::asio::error_code& ec,
        std::size_t bytes_transferred, int start = 0)
    {
      std::size_t max_size;
      switch (start_ = start)
      {
        case 1:
        max_size = this->check_for_completion(ec, buffers_.total_consumed());
        do
        {
          stream_.async_read_some(buffers_.prepare(max_size),
              EGT_ASIO_MOVE_CAST(read_op)(*this));
          return; default:
          buffers_.consume(bytes_transferred);
          if ((!ec && bytes_transferred == 0) || buffers_.empty())
            break;
          max_size = this->check_for_completion(ec, buffers_.total_consumed());
        } while (max_size > 0);

        handler_(ec, buffers_.total_consumed());
      }
    }

  //private:
    AsyncReadStream& stream_;
    egt::asio::detail::consuming_buffers<mutable_buffer,
        MutableBufferSequence, MutableBufferIterator> buffers_;
    int start_;
    ReadHandler handler_;
  };

  template <typename AsyncReadStream, typename MutableBufferSequence,
      typename MutableBufferIterator, typename CompletionCondition,
      typename ReadHandler>
  inline void* asio_handler_allocate(std::size_t size,
      read_op<AsyncReadStream, MutableBufferSequence, MutableBufferIterator,
        CompletionCondition, ReadHandler>* this_handler)
  {
    return egt_asio_handler_alloc_helpers::allocate(
        size, this_handler->handler_);
  }

  template <typename AsyncReadStream, typename MutableBufferSequence,
      typename MutableBufferIterator, typename CompletionCondition,
      typename ReadHandler>
  inline void asio_handler_deallocate(void* pointer, std::size_t size,
      read_op<AsyncReadStream, MutableBufferSequence, MutableBufferIterator,
        CompletionCondition, ReadHandler>* this_handler)
  {
    egt_asio_handler_alloc_helpers::deallocate(
        pointer, size, this_handler->handler_);
  }

  template <typename AsyncReadStream, typename MutableBufferSequence,
      typename MutableBufferIterator, typename CompletionCondition,
      typename ReadHandler>
  inline bool asio_handler_is_continuation(
      read_op<AsyncReadStream, MutableBufferSequence, MutableBufferIterator,
        CompletionCondition, ReadHandler>* this_handler)
  {
    return this_handler->start_ == 0 ? true
      : egt_asio_handler_cont_helpers::is_continuation(
          this_handler->handler_);
  }

  template <typename Function, typename AsyncReadStream,
      typename MutableBufferSequence, typename MutableBufferIterator,
      typename CompletionCondition, typename ReadHandler>
  inline void asio_handler_invoke(Function& function,
      read_op<AsyncReadStream, MutableBufferSequence, MutableBufferIterator,
        CompletionCondition, ReadHandler>* this_handler)
  {
    egt_asio_handler_invoke_helpers::invoke(
        function, this_handler->handler_);
  }

  template <typename Function, typename AsyncReadStream,
      typename MutableBufferSequence, typename MutableBufferIterator,
      typename CompletionCondition, typename ReadHandler>
  inline void asio_handler_invoke(const Function& function,
      read_op<AsyncReadStream, MutableBufferSequence, MutableBufferIterator,
        CompletionCondition, ReadHandler>* this_handler)
  {
    egt_asio_handler_invoke_helpers::invoke(
        function, this_handler->handler_);
  }

  template <typename AsyncReadStream, typename MutableBufferSequence,
      typename MutableBufferIterator, typename CompletionCondition,
      typename ReadHandler>
  inline void start_read_buffer_sequence_op(AsyncReadStream& stream,
      const MutableBufferSequence& buffers, const MutableBufferIterator&,
      CompletionCondition completion_condition, ReadHandler& handler)
  {
    detail::read_op<AsyncReadStream, MutableBufferSequence,
      MutableBufferIterator, CompletionCondition, ReadHandler>(
        stream, buffers, completion_condition, handler)(
          egt::asio::error_code(), 0, 1);
  }
} // namespace detail

#if !defined(GENERATING_DOCUMENTATION)

template <typename AsyncReadStream, typename MutableBufferSequence,
    typename MutableBufferIterator, typename CompletionCondition,
    typename ReadHandler, typename Allocator>
struct associated_allocator<
    detail::read_op<AsyncReadStream, MutableBufferSequence,
      MutableBufferIterator, CompletionCondition, ReadHandler>,
    Allocator>
{
  typedef typename associated_allocator<ReadHandler, Allocator>::type type;

  static type get(
      const detail::read_op<AsyncReadStream, MutableBufferSequence,
        MutableBufferIterator, CompletionCondition, ReadHandler>& h,
      const Allocator& a = Allocator()) EGT_ASIO_NOEXCEPT
  {
    return associated_allocator<ReadHandler, Allocator>::get(h.handler_, a);
  }
};

template <typename AsyncReadStream, typename MutableBufferSequence,
    typename MutableBufferIterator, typename CompletionCondition,
    typename ReadHandler, typename Executor>
struct associated_executor<
    detail::read_op<AsyncReadStream, MutableBufferSequence,
      MutableBufferIterator, CompletionCondition, ReadHandler>,
    Executor>
{
  typedef typename associated_executor<ReadHandler, Executor>::type type;

  static type get(
      const detail::read_op<AsyncReadStream, MutableBufferSequence,
        MutableBufferIterator, CompletionCondition, ReadHandler>& h,
      const Executor& ex = Executor()) EGT_ASIO_NOEXCEPT
  {
    return associated_executor<ReadHandler, Executor>::get(h.handler_, ex);
  }
};

#endif // !defined(GENERATING_DOCUMENTATION)

template <typename AsyncReadStream, typename MutableBufferSequence,
    typename CompletionCondition, typename ReadHandler>
inline EGT_ASIO_INITFN_RESULT_TYPE(ReadHandler,
    void (egt::asio::error_code, std::size_t))
async_read(AsyncReadStream& s, const MutableBufferSequence& buffers,
    CompletionCondition completion_condition,
    EGT_ASIO_MOVE_ARG(ReadHandler) handler,
    typename enable_if<
      is_mutable_buffer_sequence<MutableBufferSequence>::value
    >::type*)
{
  // If you get an error on the following line it means that your handler does
  // not meet the documented type requirements for a ReadHandler.
  EGT_ASIO_READ_HANDLER_CHECK(ReadHandler, handler) type_check;

  async_completion<ReadHandler,
    void (egt::asio::error_code, std::size_t)> init(handler);

  detail::start_read_buffer_sequence_op(s, buffers,
      egt::asio::buffer_sequence_begin(buffers), completion_condition,
      init.completion_handler);

  return init.result.get();
}

template <typename AsyncReadStream, typename MutableBufferSequence,
    typename ReadHandler>
inline EGT_ASIO_INITFN_RESULT_TYPE(ReadHandler,
    void (egt::asio::error_code, std::size_t))
async_read(AsyncReadStream& s, const MutableBufferSequence& buffers,
    EGT_ASIO_MOVE_ARG(ReadHandler) handler,
    typename enable_if<
      is_mutable_buffer_sequence<MutableBufferSequence>::value
    >::type*)
{
  // If you get an error on the following line it means that your handler does
  // not meet the documented type requirements for a ReadHandler.
  EGT_ASIO_READ_HANDLER_CHECK(ReadHandler, handler) type_check;

  async_completion<ReadHandler,
    void (egt::asio::error_code, std::size_t)> init(handler);

  detail::start_read_buffer_sequence_op(s, buffers,
      egt::asio::buffer_sequence_begin(buffers), transfer_all(),
      init.completion_handler);

  return init.result.get();
}

namespace detail
{
  template <typename AsyncReadStream, typename DynamicBuffer,
      typename CompletionCondition, typename ReadHandler>
  class read_dynbuf_op
    : detail::base_from_completion_cond<CompletionCondition>
  {
  public:
    template <typename BufferSequence>
    read_dynbuf_op(AsyncReadStream& stream,
        EGT_ASIO_MOVE_ARG(BufferSequence) buffers,
        CompletionCondition completion_condition, ReadHandler& handler)
      : detail::base_from_completion_cond<
          CompletionCondition>(completion_condition),
        stream_(stream),
        buffers_(EGT_ASIO_MOVE_CAST(BufferSequence)(buffers)),
        start_(0),
        total_transferred_(0),
        handler_(EGT_ASIO_MOVE_CAST(ReadHandler)(handler))
    {
    }

#if defined(EGT_ASIO_HAS_MOVE)
    read_dynbuf_op(const read_dynbuf_op& other)
      : detail::base_from_completion_cond<CompletionCondition>(other),
        stream_(other.stream_),
        buffers_(other.buffers_),
        start_(other.start_),
        total_transferred_(other.total_transferred_),
        handler_(other.handler_)
    {
    }

    read_dynbuf_op(read_dynbuf_op&& other)
      : detail::base_from_completion_cond<CompletionCondition>(other),
        stream_(other.stream_),
        buffers_(EGT_ASIO_MOVE_CAST(DynamicBuffer)(other.buffers_)),
        start_(other.start_),
        total_transferred_(other.total_transferred_),
        handler_(EGT_ASIO_MOVE_CAST(ReadHandler)(other.handler_))
    {
    }
#endif // defined(EGT_ASIO_HAS_MOVE)

    void operator()(const egt::asio::error_code& ec,
        std::size_t bytes_transferred, int start = 0)
    {
      std::size_t max_size, bytes_available;
      switch (start_ = start)
      {
        case 1:
        max_size = this->check_for_completion(ec, total_transferred_);
        bytes_available = std::min<std::size_t>(
              std::max<std::size_t>(512,
                buffers_.capacity() - buffers_.size()),
              std::min<std::size_t>(max_size,
                buffers_.max_size() - buffers_.size()));
        for (;;)
        {
          stream_.async_read_some(buffers_.prepare(bytes_available),
              EGT_ASIO_MOVE_CAST(read_dynbuf_op)(*this));
          return; default:
          total_transferred_ += bytes_transferred;
          buffers_.commit(bytes_transferred);
          max_size = this->check_for_completion(ec, total_transferred_);
          bytes_available = std::min<std::size_t>(
                std::max<std::size_t>(512,
                  buffers_.capacity() - buffers_.size()),
                std::min<std::size_t>(max_size,
                  buffers_.max_size() - buffers_.size()));
          if ((!ec && bytes_transferred == 0) || bytes_available == 0)
            break;
        }

        handler_(ec, static_cast<const std::size_t&>(total_transferred_));
      }
    }

  //private:
    AsyncReadStream& stream_;
    DynamicBuffer buffers_;
    int start_;
    std::size_t total_transferred_;
    ReadHandler handler_;
  };

  template <typename AsyncReadStream, typename DynamicBuffer,
      typename CompletionCondition, typename ReadHandler>
  inline void* asio_handler_allocate(std::size_t size,
      read_dynbuf_op<AsyncReadStream, DynamicBuffer,
        CompletionCondition, ReadHandler>* this_handler)
  {
    return egt_asio_handler_alloc_helpers::allocate(
        size, this_handler->handler_);
  }

  template <typename AsyncReadStream, typename DynamicBuffer,
      typename CompletionCondition, typename ReadHandler>
  inline void asio_handler_deallocate(void* pointer, std::size_t size,
      read_dynbuf_op<AsyncReadStream, DynamicBuffer,
        CompletionCondition, ReadHandler>* this_handler)
  {
    egt_asio_handler_alloc_helpers::deallocate(
        pointer, size, this_handler->handler_);
  }

  template <typename AsyncReadStream, typename DynamicBuffer,
      typename CompletionCondition, typename ReadHandler>
  inline bool asio_handler_is_continuation(
      read_dynbuf_op<AsyncReadStream, DynamicBuffer,
        CompletionCondition, ReadHandler>* this_handler)
  {
    return this_handler->start_ == 0 ? true
      : egt_asio_handler_cont_helpers::is_continuation(
          this_handler->handler_);
  }

  template <typename Function, typename AsyncReadStream,
      typename DynamicBuffer, typename CompletionCondition,
      typename ReadHandler>
  inline void asio_handler_invoke(Function& function,
      read_dynbuf_op<AsyncReadStream, DynamicBuffer,
        CompletionCondition, ReadHandler>* this_handler)
  {
    egt_asio_handler_invoke_helpers::invoke(
        function, this_handler->handler_);
  }

  template <typename Function, typename AsyncReadStream,
      typename DynamicBuffer, typename CompletionCondition,
      typename ReadHandler>
  inline void asio_handler_invoke(const Function& function,
      read_dynbuf_op<AsyncReadStream, DynamicBuffer,
        CompletionCondition, ReadHandler>* this_handler)
  {
    egt_asio_handler_invoke_helpers::invoke(
        function, this_handler->handler_);
  }
} // namespace detail

#if !defined(GENERATING_DOCUMENTATION)

template <typename AsyncReadStream, typename DynamicBuffer,
    typename CompletionCondition, typename ReadHandler, typename Allocator>
struct associated_allocator<
    detail::read_dynbuf_op<AsyncReadStream,
      DynamicBuffer, CompletionCondition, ReadHandler>,
    Allocator>
{
  typedef typename associated_allocator<ReadHandler, Allocator>::type type;

  static type get(
      const detail::read_dynbuf_op<AsyncReadStream,
        DynamicBuffer, CompletionCondition, ReadHandler>& h,
      const Allocator& a = Allocator()) EGT_ASIO_NOEXCEPT
  {
    return associated_allocator<ReadHandler, Allocator>::get(h.handler_, a);
  }
};

template <typename AsyncReadStream, typename DynamicBuffer,
    typename CompletionCondition, typename ReadHandler, typename Executor>
struct associated_executor<
    detail::read_dynbuf_op<AsyncReadStream,
      DynamicBuffer, CompletionCondition, ReadHandler>,
    Executor>
{
  typedef typename associated_executor<ReadHandler, Executor>::type type;

  static type get(
      const detail::read_dynbuf_op<AsyncReadStream,
        DynamicBuffer, CompletionCondition, ReadHandler>& h,
      const Executor& ex = Executor()) EGT_ASIO_NOEXCEPT
  {
    return associated_executor<ReadHandler, Executor>::get(h.handler_, ex);
  }
};

#endif // !defined(GENERATING_DOCUMENTATION)

template <typename AsyncReadStream,
    typename DynamicBuffer, typename ReadHandler>
inline EGT_ASIO_INITFN_RESULT_TYPE(ReadHandler,
    void (egt::asio::error_code, std::size_t))
async_read(AsyncReadStream& s,
    EGT_ASIO_MOVE_ARG(DynamicBuffer) buffers,
    EGT_ASIO_MOVE_ARG(ReadHandler) handler,
    typename enable_if<
      is_dynamic_buffer<typename decay<DynamicBuffer>::type>::value
    >::type*)
{
  return async_read(s,
      EGT_ASIO_MOVE_CAST(DynamicBuffer)(buffers),
      transfer_all(), EGT_ASIO_MOVE_CAST(ReadHandler)(handler));
}

template <typename AsyncReadStream, typename DynamicBuffer,
    typename CompletionCondition, typename ReadHandler>
inline EGT_ASIO_INITFN_RESULT_TYPE(ReadHandler,
    void (egt::asio::error_code, std::size_t))
async_read(AsyncReadStream& s,
    EGT_ASIO_MOVE_ARG(DynamicBuffer) buffers,
    CompletionCondition completion_condition,
    EGT_ASIO_MOVE_ARG(ReadHandler) handler,
    typename enable_if<
      is_dynamic_buffer<typename decay<DynamicBuffer>::type>::value
    >::type*)
{
  // If you get an error on the following line it means that your handler does
  // not meet the documented type requirements for a ReadHandler.
  EGT_ASIO_READ_HANDLER_CHECK(ReadHandler, handler) type_check;

  async_completion<ReadHandler,
    void (egt::asio::error_code, std::size_t)> init(handler);

  detail::read_dynbuf_op<AsyncReadStream,
    typename decay<DynamicBuffer>::type,
      CompletionCondition, EGT_ASIO_HANDLER_TYPE(
        ReadHandler, void (egt::asio::error_code, std::size_t))>(
          s, EGT_ASIO_MOVE_CAST(DynamicBuffer)(buffers),
            completion_condition, init.completion_handler)(
              egt::asio::error_code(), 0, 1);

  return init.result.get();
}

#if !defined(EGT_ASIO_NO_EXTENSIONS)
#if !defined(EGT_ASIO_NO_IOSTREAM)

template <typename AsyncReadStream, typename Allocator, typename ReadHandler>
inline EGT_ASIO_INITFN_RESULT_TYPE(ReadHandler,
    void (egt::asio::error_code, std::size_t))
async_read(AsyncReadStream& s, basic_streambuf<Allocator>& b,
    EGT_ASIO_MOVE_ARG(ReadHandler) handler)
{
  return async_read(s, basic_streambuf_ref<Allocator>(b),
      EGT_ASIO_MOVE_CAST(ReadHandler)(handler));
}

template <typename AsyncReadStream, typename Allocator,
    typename CompletionCondition, typename ReadHandler>
inline EGT_ASIO_INITFN_RESULT_TYPE(ReadHandler,
    void (egt::asio::error_code, std::size_t))
async_read(AsyncReadStream& s, basic_streambuf<Allocator>& b,
    CompletionCondition completion_condition,
    EGT_ASIO_MOVE_ARG(ReadHandler) handler)
{
  return async_read(s, basic_streambuf_ref<Allocator>(b),
      completion_condition, EGT_ASIO_MOVE_CAST(ReadHandler)(handler));
}

#endif // !defined(EGT_ASIO_NO_IOSTREAM)
#endif // !defined(EGT_ASIO_NO_EXTENSIONS)

} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#endif // EGT_ASIO_IMPL_READ_HPP
