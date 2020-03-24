/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_COW_H
#define EGT_DETAIL_COW_H

#include <egt/detail/meta.h>
#include <memory>

/**
 * @file
 * @brief Copy-On-Write wrappers.
 */

namespace egt
{
inline namespace v1
{
namespace detail
{

/**
 * Copy-On-Write wrapper for an object or container.
 *
 * This creates copy on write semantics for an object. This also performs lazy
 * initialization of the object and will automatically default construct it when
 * needed.
 */
template <class T>
class CopyOnWriteAllocate
{
public:

    CopyOnWriteAllocate() noexcept = default;
    EGT_OPS_COPY_MOVE(CopyOnWriteAllocate);
    ~CopyOnWriteAllocate() noexcept = default;

    /// Construct with a pointer to the object. This now owns the pointer.
    explicit CopyOnWriteAllocate(T* t)
        : m_ptr(t)
    {}

    /// Construct with a shared_ptr to the object.
    explicit CopyOnWriteAllocate(const std::shared_ptr<T>& refptr)
        : m_ptr(refptr)
    {}

    /// Bool operator.
    explicit operator bool() const
    {
        return !!m_ptr;
    }

    /// Dereference operator.  This will allocate.
    const T& operator*() const
    {
        allocate();
        return *m_ptr;
    }

    /// Dereference operator.
    T& operator*()
    {
        detach();
        return *m_ptr;
    }

    /// Arrow operator.
    const T* operator->() const
    {
        allocate();
        return m_ptr.operator->();
    }

    /// Arrow operator.
    T* operator->()
    {
        detach();
        return m_ptr.operator->();
    }

private:

    void allocate()
    {
        if (!m_ptr)
            m_ptr = std::make_shared<T>();
    }

    void detach()
    {
        if (!m_ptr)
            allocate();
        else if (m_ptr && !m_ptr.unique())
            m_ptr = std::make_shared<T>(*m_ptr);
    }

    std::shared_ptr<T> m_ptr;
};

}
}
}

#endif
