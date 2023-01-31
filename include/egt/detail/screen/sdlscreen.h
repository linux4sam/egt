/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_SCREEN_SDLSCREEN_H
#define EGT_DETAIL_SCREEN_SDLSCREEN_H

/**
 * @file
 * @brief SDL2 screen support.
 */

#include <atomic>
#include <egt/asio.hpp>
#include <egt/detail/meta.h>
#include <egt/input.h>
#include <egt/screen.h>
#include <memory>
#include <string>
#include <thread>

namespace egt
{
inline namespace v1
{
namespace detail
{
struct SDLData;

/**
 * Screen in an SDL window.
 */
class SDLScreen : public Screen
{
public:

    /**
     * @param app Application instance this screen is associated with.
     * @param size Size of the screen.
     */
    explicit SDLScreen(Application& app, const Size& size = Size(800, 480), const std::string& name = {});

    SDLScreen(const SDLScreen&) = delete;
    SDLScreen& operator=(const SDLScreen&) = delete;
    SDLScreen(SDLScreen&&) = delete;
    SDLScreen& operator=(SDLScreen&&) = delete;

    void schedule_flip() override
    {}

    void flip(const DamageArray& damage) override;

    void quit()
    {
        m_quit = true;
    }

    ~SDLScreen() override;

protected:

    void pointer_event(EventId e, const Pointer& pointer);
    void key_event(EventId e, const Key& key);
    void sdl_draw(const DamageArray& damage);
    static KeyboardCode sdl_to_egtkeys(int key);

    /// @private
    std::unique_ptr<detail::SDLData> m_priv;

    /// Internal thread for event processing.
    std::thread m_thread;

    /// Flag to quit thread.
    std::atomic<bool> m_quit{false};

    /// Internal io context.
    asio::io_context m_io;

    /**
     * SDL input dispatcher.
     *
     * @private
     */
    struct SDLInput : public Input
    {
        using Input::Input;
        friend class SDLScreen;
    };

    /// Custom input for dispatching events.
    SDLInput m_in;

    /// Application reference.
    Application& m_app;
};

}
}
}

#endif
