/*
 * Copyright (C) 2022 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_BITFIELDS_H
#define EGT_BITFIELDS_H

/**
 * @file
 * @brief bit-fields.
 */

#include <egt/detail/enum.h>
#include <egt/detail/meta.h>
#include <egt/detail/string.h>
#include <egt/signal.h>
#include <list>
#include <iosfwd>
#include <sstream>
#include <string>

namespace egt
{
inline namespace v1
{

template<class T> class BitFields;

class BitField
{
public:
    constexpr BitField operator|(const BitField& rhs) const noexcept
    {
        return BitField(m_mask | rhs.m_mask, m_value | rhs.m_value);
    }

    constexpr bool operator==(const BitField& rhs) const noexcept
    {
        return (m_mask == rhs.m_mask) && (m_value == rhs.m_value);
    }

protected:
    constexpr BitField(uint32_t mask, uint32_t value) noexcept
        : m_mask(mask)
        , m_value(value)
    {}

    constexpr BitField(uint32_t bit) noexcept
        : m_mask(bit)
        , m_value(bit)
    {}

    uint32_t m_mask;
    uint32_t m_value;

    template<class T> friend class BitFields;
};

template<class T>
std::list<T> get(const BitFields<T>& fields);

template<class T>
class BitFields
{
public:
    constexpr BitFields() noexcept = default;

    /**
     * Event signal.
     * @{
     */
    /// Invoked when the bit-fields are changed.
    Signal<> on_change;
    /** @} */

    /**
     * @param[in] str String representation of the bit-fields.
     */
    explicit BitFields(const std::string& str)
    {
        from_string(str);
    }

    /**
     * @param[in] str String representation of the bit-fields.
     */
    explicit BitFields(const char* str)
    {
        from_string(str);
    }

    constexpr BitFields(const T& field)
        : m_word(static_cast<const BitField&>(field).m_value)
    {
    }

    /// Copy constructor.
    constexpr BitFields(const BitFields& rhs) noexcept
        : m_word(rhs.m_word)
    {
        // signals are not copied!
    }

    /// Assignment operator.
    // NOLINTNEXTLINE(cert-oop54-cpp)
    BitFields& operator=(const BitFields& rhs)
    {
        if (&rhs != this)
        {
            // signals are not copied!
            if (detail::change_if_diff(m_word, rhs.m_word))
                on_change.invoke();
        }
        return *this;
    }

    BitFields(BitFields&&) noexcept = default;
    BitFields& operator=(BitFields&&) noexcept = default;
    ~BitFields() noexcept = default;

    bool operator==(const BitFields& rhs) const
    {
        return m_word == rhs.m_word;
    }

    bool operator!=(const BitFields& rhs) const
    {
        return !(*this == rhs);
    }

    bool empty() const
    {
        return m_word == 0;
    }

    EGT_NODISCARD constexpr bool is_set(const T& field) const noexcept
    {
        const auto& f = static_cast<const BitField&>(field);
        return (m_word & f.m_mask) == f.m_value;
    }

    /// Set a single bit-field.
    bool set(const T& field)
    {
        const auto& f = static_cast<const BitField&>(field);
        return update((m_word & ~f.m_mask) | f.m_value);
    }

    /// Clear a single bit-field.
    bool clear(const T& field)
    {
        const auto& f = static_cast<const BitField&>(field);
        return update(m_word & ~f.m_mask);
    }

    /// Clear all bit-fields.
    bool clear()
    {
        return update(0);
    }

    /// Or operator.
    constexpr BitFields<T> operator|(const T& field) const noexcept
    {
        BitFields<T> ret(*this);
        ret.set(field);
        return ret;
    }

    /// Delimiter used to separate bit-fields in string representation.
    constexpr static const char BITFIELDS_DELIMITER = '|';

    /**
     * Convert the bit-fields to strings.
     */
    EGT_NODISCARD std::string to_string() const
    {
        std::ostringstream ss;
        if (!empty())
        {
            bool first = true;
            const auto container = get(*this);
            for (const auto& item : container)
            {
                if (first)
                    first = false;
                else
                    ss << BITFIELDS_DELIMITER;
                ss << detail::enum_to_string(item);
            }
        }
        return ss.str();
    }

    /**
     * Convert from string.
     *
     * @note This will clear any existing bit-field first.
     */
    void from_string(const std::string& str)
    {
        clear();
        std::vector<std::string> tokens;
        detail::tokenize(str, BITFIELDS_DELIMITER, tokens);
        for (const auto& field : tokens)
            set(detail::enum_from_string<T>(detail::trim(field)));
    }

protected:

    bool update(uint32_t new_word)
    {
        if (detail::change_if_diff(m_word, new_word))
        {
            on_change.invoke();
            return true;
        }
        return false;
    }

    uint32_t m_word{0};
};

}
}

#endif
