/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SRC_DETAIL_ASIOALLOCATOR_H
#define EGT_SRC_DETAIL_ASIOALLOCATOR_H

#include <array>
#include <type_traits>

namespace egt
{
inline namespace v1
{
namespace detail
{

// Class to manage the memory to be used for handler-based custom allocation.
// It contains a single block of memory which may be returned for allocation
// requests. If the memory is in use when an allocation request is made, the
// allocator delegates allocation to the global heap.
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init, hicpp-member-init)
class HandlerAllocator
{
public:
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init, hicpp-member-init)
    HandlerAllocator() noexcept = default;

    void* allocate(std::size_t size)
    {
        if (!m_in_use && size < m_storage.size())
        {
            m_in_use = true;
            return m_storage.data();
        }
        else
        {
            return ::operator new (size);
        }
    }

    void deallocate(void* pointer)
    {
        if (pointer == m_storage.data())
        {
            m_in_use = false;
        }
        else
        {
            ::operator delete (pointer);
        }
    }

private:
    // Storage space used for handler-based custom memory allocation.
    std::array<char, 1024> m_storage;

    // Whether the handler-based custom allocation storage has been used.
    bool m_in_use{false};
};

// Wrapper class template for handler objects to allow handler memory
// allocation to be customized. Calls to operator() are forwarded to the
// encapsulated handler.
template <typename Handler>
class CustomAllocHandler
{
public:
    CustomAllocHandler(HandlerAllocator& a, Handler h)
        : m_allocator(a),
          m_handler(std::move(h))
    {}

    template <typename Arg1>
    void operator()(Arg1 arg1)
    {
        m_handler(arg1);
    }

    template <typename Arg1, typename Arg2>
    void operator()(Arg1 arg1, Arg2 arg2)
    {
        m_handler(arg1, arg2);
    }

    void* asio_handler_allocate(std::size_t size,
                                CustomAllocHandler<Handler>* this_handler)
    {
        return this_handler->m_allocator.allocate(size);
    }

    void asio_handler_deallocate(void* pointer, std::size_t /*size*/,
                                 CustomAllocHandler<Handler>* this_handler)
    {
        this_handler->m_allocator.deallocate(pointer);
    }

private:
    HandlerAllocator& m_allocator;
    Handler m_handler;
};

// Helper function to wrap a handler object to add custom allocation.
template <typename Handler>
inline CustomAllocHandler<Handler> make_custom_alloc_handler(HandlerAllocator& a, Handler h)
{
    return CustomAllocHandler<Handler>(a, h);
}

}
}
}

#endif
