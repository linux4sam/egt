/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_COW_H
#define EGT_DETAIL_COW_H

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
 * Copy-On-Write wrapper for an object.
 *
 * This creates copy on write semantics for an object. This also preforms lazy
 * initialization of the object and will automatically default construct it when
 * needed.
 */
template <class T>
class CopyOnWriteAllocate
{
public:

    CopyOnWriteAllocate() = default;

    explicit CopyOnWriteAllocate(T* t)
        : m_ptr(t)
    {}

    explicit CopyOnWriteAllocate(const std::shared_ptr<T>& refptr)
        : m_ptr(refptr)
    {}

    operator bool() const
    {
        return !!m_ptr;
    }

    const T& operator*() const
    {
        allocate();
        return *m_ptr;
    }

    T& operator*()
    {
        detach();
        return *m_ptr;
    }

    const T* operator->() const
    {
        allocate();
        return m_ptr.operator->();
    }

    T* operator->()
    {
        detach();
        return m_ptr.operator->();
    }

private:

    inline void allocate()
    {
        if (!m_ptr)
        {
            m_ptr = std::make_shared<T>();
        }
    }

    inline void detach()
    {
        if (!m_ptr)
        {
            allocate();
        }
        else if (m_ptr && !m_ptr.unique())
        {
            m_ptr = std::make_shared<T>(*m_ptr);
        }
    }

    std::shared_ptr<T> m_ptr;
};

}
}
}

#endif
