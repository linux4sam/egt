/*
 * Copyright (C) 2025 Microchip Technology Inc. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SOUND_EFFECT_H
#define EGT_SOUND_EFFECT_H

/**
 * @file
 * @brief Working with sound effects.
 */

#include <string>

#include <egt/detail/meta.h>

namespace egt
{
inline namespace v1
{

namespace detail
{
class SoundEffectImpl;
}

/**
 * Notification Sound Effect.
 *
 * The SoundEffect class is ideal for playing notification sounds, such as
 * providing feedback when a button is clicked. This method is fast,
 * asynchronous, and lightweight.
 *
 * The stream control for Sound'effect is limited to starting and stopping,
 * with an optional repeat feature.
 *
 * @note For more details about the SoundEffect class, please refer to the
 * @ref audio page.
 */
class EGT_API SoundEffect final
{
public:

    /**
     * The SoundDevice object is used to describe an ALSA PCM device. It
     * includes information such as card and device indexes, which can be
     * retrieved using the `aplay -l` command.
     */
    class SoundDevice
    {
    public:

        /**
         * @param[in] card_index The Alsa card index.
         * @param[in] device_index The Alsa device index.
         */
        SoundDevice(int card_index = 0, int device_index = 0) noexcept
            : m_card_index(card_index), m_device_index(device_index)
        {
        }

        /**
         * Set the sound card index.
         *
         * @param[in] card_index The Alsa card index.
         */
        void card_index(int card_index)
        {
            m_card_index = card_index;
        }

        /**
         * Get the sound card index.
         */
        EGT_NODISCARD int card_index() const
        {
            return m_card_index;
        }

        /**
         * Set the sound card name.
         *
         * @param[in] card_name The Alsa card name.
         */
        void card_name(const std::string& card_name)
        {
            m_card_name = card_name;
        }

        /**
         * Get the sound card name.
         */
        EGT_NODISCARD std::string card_name() const
        {
            return m_card_name;
        }

        /**
         * Set the sound device index.
         *
         * @param[in] device_index The Alsa device index.
         */
        void device_index(int device_index)
        {
            m_device_index = device_index;
        }

        /**
         * Get the sound device index.
         */
        EGT_NODISCARD int device_index() const
        {
            return m_device_index;
        }

        /**
         * Set the sound device name.
         *
         * @param[in] card_name The Alsa card name.
         */
        void device_name(std::string device_name)
        {
            m_device_name = device_name;
        }

        /**
         * Get the sound device name.
         */
        EGT_NODISCARD std::string device_name() const
        {
            return m_device_name;
        }

    private:

        int m_card_index{0};
        std::string m_card_name{};
        int m_device_index{0};
        std::string m_device_name{};
    };

    /**
     * SoundEffect constructor.
     *
     * @param[in] uri The uri of the sound file. Only file:// is accepted.
     * @param[in] sound_device The sound device to use. Only the card index
     * and device index are considered.
     */
    explicit SoundEffect(const std::string& uri, const SoundDevice& sound_device = {});

    /**
     * SoundEffect default constructor.
     *
     * Can be useful to set the uri or sound device later.
     */
    SoundEffect() noexcept;

    SoundEffect(const SoundEffect&) = delete;

    SoundEffect& operator=(const SoundEffect&) = delete;

    ~SoundEffect() noexcept;

    /**
     * Set the sound device to use.
     *
     * @param[in] sound_device The sound device to use. Only the card index
     * and device index are considered.
     *
     * @return true on success, false otherwise.
     */
    bool device(const SoundDevice& sound_device) noexcept;

    /**
     * Return the sound device used.
     */
    EGT_NODISCARD const SoundDevice& device() const;

    /**
     * Set the uri of the sound file.
     *
     * @param[in] uri The uri of the sound file. Only file:// is accepted.
     *
     * @return true on success, false otherwise.
     */
    bool media(const std::string& uri) noexcept;

    /**
     * Return the uri of the sound file.
     */
    std::string media() const;

    /**
     * Set the repeat status.
     */
    void repeat(bool value);

    /**
     * Get the repeat status.
     */
    EGT_NODISCARD bool repeat() const;

    /**
     * Start playing.
     */
    void play();

    /**
     * Stop playing.
     */
    void stop();

    /**
     * Get the list of the Alsa PCM devices with their card/device indexes and
     * names.
     */
    static std::vector<SoundDevice> sound_device_list();

private:

    std::unique_ptr<detail::SoundEffectImpl> m_impl;
};

}
}

#endif
