//
// impl/write.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_IMPL_WRITE_HPP
#define EGT_ASIO_IMPL_WRITE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

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

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {

namespace detail
{
  template <typename SyncWriteStream, typename ConstBufferSequence,
      typename ConstBufferIterator, typename CompletionCondition>
  std::size_t write_buffer_sequence(SyncWriteStream& s,
      const ConstBufferSequence& buffers, const ConstBufferIterator&,
      CompletionCondition completion_condition, egt::asio::error_code& ec)
  {
    ec = egt::asio::error_code();
    egt::asio::detail::consuming_buffers<const_buffer,
        ConstBufferSequence, ConstBufferIterator> tmp(buffers);
    while (!tmp.empty())
    {
      if (std::size_t max_size = detail::adapt_completion_condition_result(
            completion_condition(ec, tmp.total_consumed())))
        tmp.consume(s.write_some(tmp.prepare(max_size), ec));
      else
        break;
    }
    return tmp.total_consumed();;
  }
} // namespace detail

template <typename SyncWriteStream, typename ConstBufferSequence,
    typename CompletionCondition>
inline std::size_t write(SyncWriteStream& s, const ConstBufferSequence& buffers,
    CompletionCondition completion_condition, egt::asio::error_code& ec,
    typename enable_if<
      is_const_buffer_sequence<ConstBufferSequence>::value
    >::type*)
{
  return detail::write_buffer_sequence(s, buffers,
      egt::asio::buffer_sequence_begin(buffers), completion_condition, ec);
}

template <typename SyncWriteStream, typename ConstBufferSequence>
inline std::size_t write(SyncWriteStream& s, const ConstBufferSequence& buffers,
    typename enable_if<
      is_const_buffer_sequence<ConstBufferSequence>::value
    >::type*)
{
  egt::asio::error_code ec;
  std::size_t bytes_transferred = write(s, buffers, transfer_all(), ec);
  egt::asio::detail::throw_error(ec, "write");
  return bytes_transferred;
}

template <typename SyncWriteStream, typename ConstBufferSequence>
inline std::size_t write(SyncWriteStream& s, const ConstBufferSequence& buffers,
    egt::asio::error_code& ec,
    typename enable_if<
      is_const_buffer_sequence<ConstBufferSequence>::value
    >::type*)
{
  return write(s, buffers, transfer_all(), ec);
}

template <typename SyncWriteStream, typename ConstBufferSequence,
    typename CompletionCondition>
inline std::size_t write(SyncWriteStream& s, const ConstBufferSequence& buffers,
    CompletionCondition completion_condition,
    typename enable_if<
      is_const_buffer_sequence<ConstBufferSequence>::value
    >::type*)
{
  egt::asio::error_code ec;
  std::size_t bytes_transferred = write(s, buffers, completion_condition, ec);
  egt::asio::detail::throw_error(ec, "write");
  return bytes_transferred;
}

template <typename SyncWriteStream, typename DynamicBuffer,
    typename CompletionCondition>
std::size_t write(SyncWriteStream& s,
    EGT_ASIO_MOVE_ARG(DynamicBuffer) buffers,
    CompletionCondition completion_condition, egt::asio::error_code& ec,
    typename enable_if<
      is_dynamic_buffer<typename decay<DynamicBuffer>::type>::value
    >::type*)
{
  typename decay<DynamicBuffer>::type b(
      EGT_ASIO_MOVE_CAST(DynamicBuffer)(buffers));

  std::size_t bytes_transferred = write(s, b.data(), completion_condition, ec);
  b.consume(bytes_transferred);
  return bytes_transferred;
}

template <typename SyncWriteStream, typename DynamicBuffer>
inline std::size_t write(SyncWriteStream& s,
    EGT_ASIO_MOVE_ARG(DynamicBuffer) buffers,
    typename enable_if<
      is_dynamic_buffer<typename decay<DynamicBuffer>::type>::value
    >::type*)
{
  egt::asio::error_code ec;
  std::size_t bytes_transferred = write(s,
      EGT_ASIO_MOVE_CAST(DynamicBuffer)(buffers),
      transfer_all(), ec);
  egt::asio::detail::throw_error(ec, "write");
  return bytes_transferred;
}

template <typename SyncWriteStream, typename DynamicBuffer>
inline std::size_t write(SyncWriteStream& s,
    EGT_ASIO_MOVE_ARG(DynamicBuffer) buffers,
    egt::asio::error_code& ec,
    typename enable_if<
      is_dynamic_buffer<typename decay<DynamicBuffer>::type>::value
    >::type*)
{
  return write(s, EGT_ASIO_MOVE_CAST(DynamicBuffer)(buffers),
      transfer_all(), ec);
}

template <typename SyncWriteStream, typename DynamicBuffer,
    typename CompletionCondition>
inline std::size_t write(SyncWriteStream& s,
    EGT_ASIO_MOVE_ARG(DynamicBuffer) buffers,
    CompletionCondition completion_condition,
    typename enable_if<
      is_dynamic_buffer<typename decay<DynamicBuffer>::type>::value
    >::type*)
{
  egt::asio::error_code ec;
  std::size_t bytes_transferred = write(s,
      EGT_ASIO_MOVE_CAST(DynamicBuffer)(buffers),
      completion_condition, ec);
  egt::asio::detail::throw_error(ec, "write");
  return bytes_transferred;
}

#if !defined(EGT_ASIO_NO_EXTENSIONS)
#if !defined(EGT_ASIO_NO_IOSTREAM)

template <typename SyncWriteStream, typename Allocator,
    typename CompletionCondition>
inline std::size_t write(SyncWriteStream& s,
    egt::asio::basic_streambuf<Allocator>& b,
    CompletionCondition completion_condition, egt::asio::error_code& ec)
{
  return write(s, basic_streambuf_ref<Allocator>(b), completion_condition, ec);
}

template <typename SyncWriteStream, typename Allocator>
inline std::size_t write(SyncWriteStream& s,
    egt::asio::basic_streambuf<Allocator>& b)
{
  return write(s, basic_streambuf_ref<Allocator>(b));
}

template <typename SyncWriteStream, typename Allocator>
inline std::size_t write(SyncWriteStream& s,
    egt::asio::basic_streambuf<Allocator>& b,
    egt::asio::error_code& ec)
{
  return write(s, basic_streambuf_ref<Allocator>(b), ec);
}

template <typename SyncWriteStream, typename Allocator,
    typename CompletionCondition>
inline std::size_t write(SyncWriteStream& s,
    egt::asio::basic_streambuf<Allocator>& b,
    CompletionCondition completion_condition)
{
  return write(s, basic_streambuf_ref<Allocator>(b), completion_condition);
}

#endif // !defined(EGT_ASIO_NO_IOSTREAM)
#endif // !defined(EGT_ASIO_NO_EXTENSIONS)

namespace detail
{
  template <typename AsyncWriteStream, typename ConstBufferSequence,
      typename ConstBufferIterator, typename CompletionCondition,
      typename WriteHandler>
  class write_op
    : detail::base_from_completion_cond<CompletionCondition>
  {
  public:
    write_op(AsyncWriteStream& stream, const ConstBufferSequence& buffers,
        CompletionCondition completion_condition, WriteHandler& handler)
      : detail::base_from_completion_cond<
          CompletionCondition>(completion_condition),
        stream_(stream),
        buffers_(buffers),
        start_(0),
        handler_(EGT_ASIO_MOVE_CAST(WriteHandler)(handler))
    {
    }

#if defined(EGT_ASIO_HAS_MOVE)
    write_op(const write_op& other)
      : detail::base_from_completion_cond<CompletionCondition>(other),
        stream_(other.stream_),
        buffers_(other.buffers_),
        start_(other.start_),
        handler_(other.handler_)
    {
    }

    write_op(write_op&& other)
      : detail::base_from_completion_cond<CompletionCondition>(other),
        stream_(other.stream_),
        buffers_(other.buffers_),
        start_(other.start_),
        handler_(EGT_ASIO_MOVE_CAST(WriteHandler)(other.handler_))
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
          stream_.async_write_some(buffers_.prepare(max_size),
              EGT_ASIO_MOVE_CAST(write_op)(*this));
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
    AsyncWriteStream& stream_;
    egt::asio::detail::consuming_buffers<const_buffer,
        ConstBufferSequence, ConstBufferIterator> buffers_;
    int start_;
    WriteHandler handler_;
  };

  template <typename AsyncWriteStream, typename ConstBufferSequence,
      typename ConstBufferIterator, typename CompletionCondition,
      typename WriteHandler>
  inline void* asio_handler_allocate(std::size_t size,
      write_op<AsyncWriteStream, ConstBufferSequence, ConstBufferIterator,
        CompletionCondition, WriteHandler>* this_handler)
  {
    return egt_asio_handler_alloc_helpers::allocate(
        size, this_handler->handler_);
  }

  template <typename AsyncWriteStream, typename ConstBufferSequence,
      typename ConstBufferIterator, typename CompletionCondition,
      typename WriteHandler>
  inline void asio_handler_deallocate(void* pointer, std::size_t size,
      write_op<AsyncWriteStream, ConstBufferSequence, ConstBufferIterator,
        CompletionCondition, WriteHandler>* this_handler)
  {
    egt_asio_handler_alloc_helpers::deallocate(
        pointer, size, this_handler->handler_);
  }

  template <typename AsyncWriteStream, typename ConstBufferSequence,
      typename ConstBufferIterator, typename CompletionCondition,
      typename WriteHandler>
  inline bool asio_handler_is_continuation(
      write_op<AsyncWriteStream, ConstBufferSequence, ConstBufferIterator,
        CompletionCondition, WriteHandler>* this_handler)
  {
    return this_handler->start_ == 0 ? true
      : egt_asio_handler_cont_helpers::is_continuation(
          this_handler->handler_);
  }

  template <typename Function, typename AsyncWriteStream,
      typename ConstBufferSequence, typename ConstBufferIterator,
      typename CompletionCondition, typename WriteHandler>
  inline void asio_handler_invoke(Function& function,
      write_op<AsyncWriteStream, ConstBufferSequence, ConstBufferIterator,
        CompletionCondition, WriteHandler>* this_handler)
  {
    egt_asio_handler_invoke_helpers::invoke(
        function, this_handler->handler_);
  }

  template <typename Function, typename AsyncWriteStream,
      typename ConstBufferSequence, typename ConstBufferIterator,
      typename CompletionCondition, typename WriteHandler>
  inline void asio_handler_invoke(const Function& function,
      write_op<AsyncWriteStream, ConstBufferSequence, ConstBufferIterator,
        CompletionCondition, WriteHandler>* this_handler)
  {
    egt_asio_handler_invoke_helpers::invoke(
        function, this_handler->handler_);
  }

  template <typename AsyncWriteStream, typename ConstBufferSequence,
      typename ConstBufferIterator, typename CompletionCondition,
      typename WriteHandler>
  inline void start_write_buffer_sequence_op(AsyncWriteStream& stream,
      const ConstBufferSequence& buffers, const ConstBufferIterator&,
      CompletionCondition completion_condition, WriteHandler& handler)
  {
    detail::write_op<AsyncWriteStream, ConstBufferSequence,
      ConstBufferIterator, CompletionCondition, WriteHandler>(
        stream, buffers, completion_condition, handler)(
          egt::asio::error_code(), 0, 1);
  }

} // namespace detail

#if !defined(GENERATING_DOCUMENTATION)

template <typename AsyncWriteStream, typename ConstBufferSequence,
    typename ConstBufferIterator, typename CompletionCondition,
    typename WriteHandler, typename Allocator>
struct associated_allocator<
    detail::write_op<AsyncWriteStream, ConstBufferSequence,
      ConstBufferIterator, CompletionCondition, WriteHandler>,
    Allocator>
{
  typedef typename associated_allocator<WriteHandler, Allocator>::type type;

  static type get(
      const detail::write_op<AsyncWriteStream, ConstBufferSequence,
        ConstBufferIterator, CompletionCondition, WriteHandler>& h,
      const Allocator& a = Allocator()) EGT_ASIO_NOEXCEPT
  {
    return associated_allocator<WriteHandler, Allocator>::get(h.handler_, a);
  }
};

template <typename AsyncWriteStream, typename ConstBufferSequence,
    typename ConstBufferIterator, typename CompletionCondition,
    typename WriteHandler, typename Executor>
struct associated_executor<
    detail::write_op<AsyncWriteStream, ConstBufferSequence,
      ConstBufferIterator, CompletionCondition, WriteHandler>,
    Executor>
{
  typedef typename associated_executor<WriteHandler, Executor>::type type;

  static type get(
      const detail::write_op<AsyncWriteStream, ConstBufferSequence,
        ConstBufferIterator, CompletionCondition, WriteHandler>& h,
      const Executor& ex = Executor()) EGT_ASIO_NOEXCEPT
  {
    return associated_executor<WriteHandler, Executor>::get(h.handler_, ex);
  }
};

#endif // !defined(GENERATING_DOCUMENTATION)

template <typename AsyncWriteStream, typename ConstBufferSequence,
  typename CompletionCondition, typename WriteHandler>
inline EGT_ASIO_INITFN_RESULT_TYPE(WriteHandler,
    void (egt::asio::error_code, std::size_t))
async_write(AsyncWriteStream& s, const ConstBufferSequence& buffers,
    CompletionCondition completion_condition,
    EGT_ASIO_MOVE_ARG(WriteHandler) handler,
    typename enable_if<
      is_const_buffer_sequence<ConstBufferSequence>::value
    >::type*)
{
  // If you get an error on the following line it means that your handler does
  // not meet the documented type requirements for a WriteHandler.
  EGT_ASIO_WRITE_HANDLER_CHECK(WriteHandler, handler) type_check;

  async_completion<WriteHandler,
    void (egt::asio::error_code, std::size_t)> init(handler);

  detail::start_write_buffer_sequence_op(s, buffers,
      egt::asio::buffer_sequence_begin(buffers), completion_condition,
      init.completion_handler);

  return init.result.get();
}

template <typename AsyncWriteStream, typename ConstBufferSequence,
    typename WriteHandler>
inline EGT_ASIO_INITFN_RESULT_TYPE(WriteHandler,
    void (egt::asio::error_code, std::size_t))
async_write(AsyncWriteStream& s, const ConstBufferSequence& buffers,
    EGT_ASIO_MOVE_ARG(WriteHandler) handler,
    typename enable_if<
      is_const_buffer_sequence<ConstBufferSequence>::value
    >::type*)
{
  // If you get an error on the following line it means that your handler does
  // not meet the documented type requirements for a WriteHandler.
  EGT_ASIO_WRITE_HANDLER_CHECK(WriteHandler, handler) type_check;

  async_completion<WriteHandler,
    void (egt::asio::error_code, std::size_t)> init(handler);

  detail::start_write_buffer_sequence_op(s, buffers,
      egt::asio::buffer_sequence_begin(buffers), transfer_all(),
      init.completion_handler);

  return init.result.get();
}

namespace detail
{
  template <typename AsyncWriteStream, typename DynamicBuffer,
      typename CompletionCondition, typename WriteHandler>
  class write_dynbuf_op
  {
  public:
    template <typename BufferSequence>
    write_dynbuf_op(AsyncWriteStream& stream,
        EGT_ASIO_MOVE_ARG(BufferSequence) buffers,
        CompletionCondition completion_condition, WriteHandler& handler)
      : stream_(stream),
        buffers_(EGT_ASIO_MOVE_CAST(BufferSequence)(buffers)),
        completion_condition_(
          EGT_ASIO_MOVE_CAST(CompletionCondition)(completion_condition)),
        handler_(EGT_ASIO_MOVE_CAST(WriteHandler)(handler))
    {
    }

#if defined(EGT_ASIO_HAS_MOVE)
    write_dynbuf_op(const write_dynbuf_op& other)
      : stream_(other.stream_),
        buffers_(other.buffers_),
        completion_condition_(other.completion_condition_),
        handler_(other.handler_)
    {
    }

    write_dynbuf_op(write_dynbuf_op&& other)
      : stream_(other.stream_),
        buffers_(EGT_ASIO_MOVE_CAST(DynamicBuffer)(other.buffers_)),
        completion_condition_(
          EGT_ASIO_MOVE_CAST(CompletionCondition)(
            other.completion_condition_)),
        handler_(EGT_ASIO_MOVE_CAST(WriteHandler)(other.handler_))
    {
    }
#endif // defined(EGT_ASIO_HAS_MOVE)

    void operator()(const egt::asio::error_code& ec,
        std::size_t bytes_transferred, int start = 0)
    {
      switch (start)
      {
        case 1:
        async_write(stream_, buffers_.data(), completion_condition_,
            EGT_ASIO_MOVE_CAST(write_dynbuf_op)(*this));
        return; default:
        buffers_.consume(bytes_transferred);
        handler_(ec, static_cast<const std::size_t&>(bytes_transferred));
      }
    }

  //private:
    AsyncWriteStream& stream_;
    DynamicBuffer buffers_;
    CompletionCondition completion_condition_;
    WriteHandler handler_;
  };

  template <typename AsyncWriteStream, typename DynamicBuffer,
      typename CompletionCondition, typename WriteHandler>
  inline void* asio_handler_allocate(std::size_t size,
      write_dynbuf_op<AsyncWriteStream, DynamicBuffer,
        CompletionCondition, WriteHandler>* this_handler)
  {
    return egt_asio_handler_alloc_helpers::allocate(
        size, this_handler->handler_);
  }

  template <typename AsyncWriteStream, typename DynamicBuffer,
      typename CompletionCondition, typename WriteHandler>
  inline void asio_handler_deallocate(void* pointer, std::size_t size,
      write_dynbuf_op<AsyncWriteStream, DynamicBuffer,
        CompletionCondition, WriteHandler>* this_handler)
  {
    egt_asio_handler_alloc_helpers::deallocate(
        pointer, size, this_handler->handler_);
  }

  template <typename AsyncWriteStream, typename DynamicBuffer,
      typename CompletionCondition, typename WriteHandler>
  inline bool asio_handler_is_continuation(
      write_dynbuf_op<AsyncWriteStream, DynamicBuffer,
        CompletionCondition, WriteHandler>* this_handler)
  {
    return egt_asio_handler_cont_helpers::is_continuation(
        this_handler->handler_);
  }

  template <typename Function, typename AsyncWriteStream,
      typename DynamicBuffer, typename CompletionCondition,
      typename WriteHandler>
  inline void asio_handler_invoke(Function& function,
      write_dynbuf_op<AsyncWriteStream, DynamicBuffer,
        CompletionCondition, WriteHandler>* this_handler)
  {
    egt_asio_handler_invoke_helpers::invoke(
        function, this_handler->handler_);
  }

  template <typename Function, typename AsyncWriteStream,
      typename DynamicBuffer, typename CompletionCondition,
      typename WriteHandler>
  inline void asio_handler_invoke(const Function& function,
      write_dynbuf_op<AsyncWriteStream, DynamicBuffer,
        CompletionCondition, WriteHandler>* this_handler)
  {
    egt_asio_handler_invoke_helpers::invoke(
        function, this_handler->handler_);
  }
} // namespace detail

#if !defined(GENERATING_DOCUMENTATION)

template <typename AsyncWriteStream, typename DynamicBuffer,
    typename CompletionCondition, typename WriteHandler, typename Allocator>
struct associated_allocator<
    detail::write_dynbuf_op<AsyncWriteStream,
      DynamicBuffer, CompletionCondition, WriteHandler>,
    Allocator>
{
  typedef typename associated_allocator<WriteHandler, Allocator>::type type;

  static type get(
      const detail::write_dynbuf_op<AsyncWriteStream,
        DynamicBuffer, CompletionCondition, WriteHandler>& h,
      const Allocator& a = Allocator()) EGT_ASIO_NOEXCEPT
  {
    return associated_allocator<WriteHandler, Allocator>::get(h.handler_, a);
  }
};

template <typename AsyncWriteStream, typename DynamicBuffer,
    typename CompletionCondition, typename WriteHandler, typename Executor>
struct associated_executor<
    detail::write_dynbuf_op<AsyncWriteStream,
      DynamicBuffer, CompletionCondition, WriteHandler>,
    Executor>
{
  typedef typename associated_executor<WriteHandler, Executor>::type type;

  static type get(
      const detail::write_dynbuf_op<AsyncWriteStream,
        DynamicBuffer, CompletionCondition, WriteHandler>& h,
      const Executor& ex = Executor()) EGT_ASIO_NOEXCEPT
  {
    return associated_executor<WriteHandler, Executor>::get(h.handler_, ex);
  }
};

#endif // !defined(GENERATING_DOCUMENTATION)

template <typename AsyncWriteStream,
    typename DynamicBuffer, typename WriteHandler>
inline EGT_ASIO_INITFN_RESULT_TYPE(WriteHandler,
    void (egt::asio::error_code, std::size_t))
async_write(AsyncWriteStream& s,
    EGT_ASIO_MOVE_ARG(DynamicBuffer) buffers,
    EGT_ASIO_MOVE_ARG(WriteHandler) handler,
    typename enable_if<
      is_dynamic_buffer<typename decay<DynamicBuffer>::type>::value
    >::type*)
{
  return async_write(s,
      EGT_ASIO_MOVE_CAST(DynamicBuffer)(buffers),
      transfer_all(), EGT_ASIO_MOVE_CAST(WriteHandler)(handler));
}

template <typename AsyncWriteStream, typename DynamicBuffer,
    typename CompletionCondition, typename WriteHandler>
inline EGT_ASIO_INITFN_RESULT_TYPE(WriteHandler,
    void (egt::asio::error_code, std::size_t))
async_write(AsyncWriteStream& s,
    EGT_ASIO_MOVE_ARG(DynamicBuffer) buffers,
    CompletionCondition completion_condition,
    EGT_ASIO_MOVE_ARG(WriteHandler) handler,
    typename enable_if<
      is_dynamic_buffer<typename decay<DynamicBuffer>::type>::value
    >::type*)
{
  // If you get an error on the following line it means that your handler does
  // not meet the documented type requirements for a WriteHandler.
  EGT_ASIO_WRITE_HANDLER_CHECK(WriteHandler, handler) type_check;

  async_completion<WriteHandler,
    void (egt::asio::error_code, std::size_t)> init(handler);

  detail::write_dynbuf_op<AsyncWriteStream,
    typename decay<DynamicBuffer>::type,
      CompletionCondition, EGT_ASIO_HANDLER_TYPE(
        WriteHandler, void (egt::asio::error_code, std::size_t))>(
          s, EGT_ASIO_MOVE_CAST(DynamicBuffer)(buffers),
            completion_condition, init.completion_handler)(
              egt::asio::error_code(), 0, 1);

  return init.result.get();
}

#if !defined(EGT_ASIO_NO_EXTENSIONS)
#if !defined(EGT_ASIO_NO_IOSTREAM)

template <typename AsyncWriteStream, typename Allocator, typename WriteHandler>
inline EGT_ASIO_INITFN_RESULT_TYPE(WriteHandler,
    void (egt::asio::error_code, std::size_t))
async_write(AsyncWriteStream& s,
    egt::asio::basic_streambuf<Allocator>& b,
    EGT_ASIO_MOVE_ARG(WriteHandler) handler)
{
  return async_write(s, basic_streambuf_ref<Allocator>(b),
      EGT_ASIO_MOVE_CAST(WriteHandler)(handler));
}

template <typename AsyncWriteStream, typename Allocator,
    typename CompletionCondition, typename WriteHandler>
inline EGT_ASIO_INITFN_RESULT_TYPE(WriteHandler,
    void (egt::asio::error_code, std::size_t))
async_write(AsyncWriteStream& s,
    egt::asio::basic_streambuf<Allocator>& b,
    CompletionCondition completion_condition,
    EGT_ASIO_MOVE_ARG(WriteHandler) handler)
{
  return async_write(s, basic_streambuf_ref<Allocator>(b),
      completion_condition, EGT_ASIO_MOVE_CAST(WriteHandler)(handler));
}

#endif // !defined(EGT_ASIO_NO_IOSTREAM)
#endif // !defined(EGT_ASIO_NO_EXTENSIONS)

} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#endif // EGT_ASIO_IMPL_WRITE_HPP
