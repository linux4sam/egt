/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_FLAGS_H
#define EGT_DETAIL_FLAGS_H

#include <egt/detail/enum.h>
#include <egt/detail/flagsbase.h>
#include <egt/detail/object.h>
#include <egt/detail/string.h>
#include <iostream>
#include <sstream>
#include <vector>

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

    explicit Flags(const std::string& str)
    {
        this->from_string(str);
    }

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

    inline Flags<T> operator&(const T& flag) const noexcept
    {
        return {this->m_flags& (1 << static_cast<typename detail::FlagsBase<T>::underlying>(flag))};
    }

    inline std::string to_string() const
    {
        std::ostringstream ss;
        if (!this->empty())
        {
            bool first = true;
            auto container = this->get();
            for (auto& item : container)
            {
                if (first)
                    first = false;
                else
                    ss << "|";
                ss << detail::enum_to_string(item);
            }
        }

        return ss.str();
    }

    inline void from_string(const std::string& str)
    {
        this->clear();
        std::vector<std::string> tokens;
        detail::tokenize(str, '|', tokens);
        for (auto& flag : tokens)
            this->set(detail::enum_from_string<T>(flag));
    }
};

template<class T>
inline bool operator==(const Flags<T>& lhs, const Flags<T>& rhs)
{
    return lhs.raw() == rhs.raw();
}

template<class T>
inline bool operator!=(const Flags<T>& lhs, const Flags<T>& rhs)
{
    return !(lhs == rhs);
}

/*
template<class T>
inline bool operator==(const Flags<T>& lhs, const T& rhs)
{
    return lhs.raw() == static_cast<typename Flags<T>::underlying>(rhs);
}

template<class T>
inline bool operator!=(const Flags<T>& lhs, const T& rhs)
{
    return !(lhs == rhs);
}
*/

template<class T>
std::ostream& operator<<(std::ostream& os, const Flags<T>& flags)
{
    return os << flags.tostring();
}

}
}

#endif
