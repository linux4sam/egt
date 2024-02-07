/*
 * Copyright (C) 2023 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_RANGE_H
#define EGT_DETAIL_RANGE_H

/**
 * @file
 * @brief Range class.
 */

namespace egt
{
inline namespace v1
{
namespace detail
{

/**
 * Utility class to allow range loops from a subset of a container.
 */
template<typename T>
class Range
{
public:
    Range() {}

    void begin(typename T::iterator begin) noexcept
    {
        m_begin = begin;
    }

    typename T::iterator begin() noexcept
    {
        return m_begin;
    }

    typename T::const_iterator begin() const noexcept
    {
        return m_begin;
    }

    typename T::const_iterator cbegin() const noexcept
    {
        return m_begin;
    }

    void end(typename T::iterator end) noexcept
    {
        m_end = end;
    }

    typename T::iterator end() noexcept
    {
        return m_end;
    }

    typename T::const_iterator end() const noexcept
    {
        return m_end;
    }

    typename T::const_iterator cend() const noexcept
    {
        return m_end;
    }

    typename T::reverse_iterator rbegin() noexcept
    {
        return std::reverse_iterator<typename T::iterator>(m_end);
    }

    typename T::reverse_iterator rend() noexcept
    {
        return std::reverse_iterator<typename T::iterator>(m_begin);
    }

    typename T::const_reverse_iterator rbegin() const noexcept
    {
        return std::reverse_iterator<typename T::const_iterator>(m_end);
    }

    typename T::const_reverse_iterator rend() const noexcept
    {
        return std::reverse_iterator<typename T::const_iterator>(m_begin);
    }

    typename T::const_reverse_iterator crbegin() const noexcept
    {
        return std::reverse_iterator<typename T::const_iterator>(m_end);
    }

    typename T::const_reverse_iterator crend() const noexcept
    {
        return std::reverse_iterator<typename T::const_iterator>(m_begin);
    }

    size_t size() const noexcept
    {
        return std::distance(m_begin, m_end);
    }

    bool empty() const noexcept
    {
        return (m_begin == m_end);
    }
private:
    typename T::iterator m_begin;
    typename T::iterator m_end;
};

}
}
}

#endif
