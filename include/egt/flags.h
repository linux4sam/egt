/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_FLAGS_H
#define EGT_FLAGS_H

#include <egt/detail/flagsbase.h>
#include <egt/detail/object.h>

/**
 * @file Flags base.
 */

namespace egt
{
inline namespace v1
{

template<class T>
class Flags : public detail::Object, public detail::FlagsBase<T>
{
public:
    using detail::FlagsBase<T>::FlagsBase;

    inline bool set(const T flag) noexcept
    {
        const auto res = detail::FlagsBase<T>::set(flag);
        if (res)
            invoke_handlers({eventid::property_changed});
        return res;
    }

    inline bool set(std::initializer_list<T> flags) noexcept
    {
        const auto res = detail::FlagsBase<T>::set(flags);
        if (res)
            invoke_handlers({eventid::property_changed});
        return res;
    }

    inline bool clear(const T flag) noexcept
    {
        const auto res = detail::FlagsBase<T>::clear(flag);
        if (res)
            invoke_handlers({eventid::property_changed});
	return res;
    }

    inline bool clear() noexcept
    {
        const auto res = detail::FlagsBase<T>::clear();
        if (res)
            invoke_handlers({eventid::property_changed});
	return res;
    }

    inline Flags<T> operator|(const T& flag) const noexcept
    {
        return {this->m_flags | (1 << static_cast<typename detail::FlagsBase<T>::underlying>(flag))};
    }
};

}
}

#endif
