/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_FIXEDVECTOR_H
#define EGT_FIXEDVECTOR_H

/**
 * @file
 * @brief FixedVector implementation.
 */

#include <array>
#include <cassert>
#include <cstddef>
#include <egt/detail/meta.h>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <utility>

namespace egt
{
inline namespace v1
{
namespace detail
{

template <class BinaryPredicate, class InputIterator1, class InputIterator2>
constexpr bool cmp(InputIterator1 first1, InputIterator1 last1,
                   InputIterator2 first2, InputIterator2 last2,
                   BinaryPredicate pred)
{
    for (; first1 != last1 && first2 != last2; ++first1, ++first2)
    {
        if (!pred(*first1, *first2))
        {
            return false;
        }
    }
    return first1 == last1 && first2 == last2;
}

template <typename InputIt, typename OutputIt>
constexpr OutputIt move(InputIt b, InputIt e, OutputIt to)
{
    for (; b != e; ++b, (void)++to)
    {
        *to = std::move(*b);
    }
    return to;
}

}

/**
 * constexpr vector that tries to behave like, or have an API like, std::vector.
 *
 * The capacity is fixed at compile time with the @b capacity template parameter.
 * Exceeding this capacity will throw an exception.  Otherwise, the vector can
 * be resized as necessary.
 */
template <typename T, std::size_t Capacity = 5>
class FixedVector
{
public:
    using value_type       = T;
    using difference_type  = std::ptrdiff_t;
    using reference        = value_type&;
    using const_reference  = value_type const&;
    using pointer          = T*;
    using const_pointer    = T const*;
    using iterator         = T*;
    using const_iterator   = T const*;
    using size_type        = size_t;
    using reverse_iterator = ::std::reverse_iterator<iterator>;
    using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;

    template<typename I>
    constexpr FixedVector(I begin, const I& end) noexcept
    {
        while (begin != end)
        {
            push_back(*begin);
            ++begin;
        }
    }

    constexpr FixedVector(size_type count, const T& value) noexcept
    {
        while (count)
        {
            push_back(value);
            --count;
        }
    }

    constexpr FixedVector(std::initializer_list<T> init) noexcept
        : FixedVector(init.begin(), init.end())
    {}

    constexpr FixedVector() = default;

    constexpr iterator begin() noexcept
    {
        return m_data.begin();
    }

    constexpr const_iterator begin() const noexcept
    {
        return m_data.begin();
    }

    constexpr iterator end() noexcept
    {
        return m_data.begin() + size();
    }

    constexpr const_iterator end() const noexcept
    {
        return m_data.begin() + size();
    }

    reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() noexcept
    {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    constexpr const_iterator cbegin() noexcept
    {
        return begin();
    }

    constexpr const_iterator cbegin() const noexcept
    {
        return begin();
    }

    constexpr const_iterator cend() noexcept
    {
        return end();
    }

    constexpr const_iterator cend() const noexcept
    {
        return end();
    }

    constexpr auto capacity() const noexcept { return Capacity; }
    constexpr auto size() const noexcept { return m_size; }
    constexpr auto empty() const noexcept { return m_size == 0; }
    constexpr void clear() noexcept { m_size = 0; }

    constexpr const T* data() const
    {
        return m_data.data();
    }

    constexpr reference operator[](size_type pos) noexcept
    {
        return m_data[pos];
    }

    constexpr const_reference operator[](size_type pos) const noexcept
    {
        return m_data[pos];
    }

    constexpr reference at(size_type pos)
    {
        if (pos >= size())
            throw std::out_of_range("FixedVector::at");

        return m_data[pos];
    }

    constexpr const_reference at(size_type pos) const
    {
        if (pos >= size())
            throw std::out_of_range("FixedVector::at");

        return m_data[pos];
    }

    constexpr reference front() noexcept
    {
        assert(size() > 0);
        return m_data[0];
    }
    constexpr const_reference front() const noexcept
    {
        assert(size() > 0);
        return m_data[0];
    }

    constexpr reference back() noexcept
    {
        assert(size() > 0);
        return m_data[size() - 1];
    }
    constexpr const_reference back() const noexcept
    {
        assert(size() > 0);
        return m_data[size() - 1];
    }

    template <typename U>
    constexpr void push_back(U&& value) noexcept
    {
        assert(!full() && "FixedVector is full!");
        emplace_back(std::forward<U>(value));
    }

    /// Appends a default constructed `T` at the end of the FixedVector.
    void push_back() noexcept
    {
        assert(!full() && "FixedVector is full!");
        emplace_back(T{});
    }

    template <typename... Args>
    void emplace_back(Args&& ... args) noexcept
    {
        assert(!full() && "tried to emplace_back on full storage");
        new (end()) T(std::forward<Args>(args)...);
        ++m_size;
    }

    constexpr void swap(FixedVector& other) noexcept
    {
        FixedVector tmp = std::move(other);
        other = std::move(*this);
        (*this) = std::move(tmp);
    }

    constexpr iterator erase(const_iterator position) noexcept
    {
        return erase(position, position + 1);
    }

    constexpr iterator erase(const_iterator first,
                             const_iterator last) noexcept
    {
        iterator p = begin() + (first - begin());
        if (first != last)
        {
            destroy(detail::move(p + (last - first), end(), p), end());
            m_size = size() - static_cast<size_type>(last - first);
        }

        return p;
    }

private:

    constexpr bool full() const
    {
        return size() >= capacity();
    }

    template <typename InputIt>
    void destroy(InputIt first, InputIt last) noexcept
    {
        assert(first >= data() && first <= end()
               && "first is out-of-bounds");
        assert(last >= data() && last <= end()
               && "last is out-of-bounds");
        for (; first != last; ++first)
        {
            first->~T();
        }
    }

    using Storage = std::array<T, Capacity>;

    Storage m_data{};
    std::size_t m_size{0};
};

template <typename T, size_t Capacity>
constexpr bool operator==(FixedVector<T, Capacity> const& a,
                          FixedVector<T, Capacity> const& b) noexcept
{
    return a.size() == b.size()
           and detail::cmp(a.begin(), a.end(), b.begin(), b.end(),
                           std::equal_to<> {});
}

template <typename T, size_t Capacity>
constexpr bool operator<(FixedVector<T, Capacity> const& a,
                         FixedVector<T, Capacity> const& b) noexcept
{
    return detail::cmp(a.begin(), a.end(), b.begin(), b.end(),
                       std::less<> {});
}

template <typename T, size_t Capacity>
constexpr bool operator!=(FixedVector<T, Capacity> const& a,
                          FixedVector<T, Capacity> const& b) noexcept
{
    return !(a == b);
}

template <typename T, size_t Capacity>
constexpr bool operator<=(FixedVector<T, Capacity> const& a,
                          FixedVector<T, Capacity> const& b) noexcept
{
    return detail::cmp(a.begin(), a.end(), b.begin(), b.end(),
                       std::less_equal<> {});
}

template <typename T, size_t Capacity>
constexpr bool operator>(FixedVector<T, Capacity> const& a,
                         FixedVector<T, Capacity> const& b) noexcept
{
    return detail::cmp(a.begin(), a.end(), b.begin(), b.end(),
                       std::greater<> {});
}

template <typename T, size_t Capacity>
constexpr bool operator>=(FixedVector<T, Capacity> const& a,
                          FixedVector<T, Capacity> const& b) noexcept
{
    return detail::cmp(a.begin(), a.end(), b.begin(), b.end(),
                       std::greater_equal<> {});
}

}
}

#endif
