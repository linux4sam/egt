/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/egtlog.h"
#include "egt/app.h"
#include "egt/detail/screen/sdlscreen.h"
#include "egt/eventloop.h"
#include "egt/utils.h"
#include <SDL2/SDL.h>
#include <cairo.h>

namespace egt
{
inline namespace v1
{
namespace detail
{

struct sdl_window_t_deleter
{
    void operator()(SDL_Window* window) { SDL_DestroyWindow(window); }
};

using unique_sdl_window_t =
    std::unique_ptr<SDL_Window, sdl_window_t_deleter>;

struct sdl_texture_t_deleter
{
    void operator()(SDL_Texture* texture) { SDL_DestroyTexture(texture); }
};

using unique_sdl_texture_t =
    std::unique_ptr<SDL_Texture, sdl_texture_t_deleter>;

struct sdl_renderer_t_deleter
{
    void operator()(SDL_Renderer* renderer) { SDL_DestroyRenderer(renderer); }
};

using unique_sdl_renderer_t =
    std::unique_ptr<SDL_Renderer, sdl_renderer_t_deleter>;

struct SDLData
{
    unique_sdl_window_t window;
    unique_sdl_renderer_t renderer;
    unique_sdl_texture_t texture;
};


SDLScreen::SDLScreen(Application& app, const Size& size)
    : m_priv(std::make_unique<detail::SDLData>()),
      m_app(app)
{
    detail::info("SDL Screen");

    init(size);

    m_buffers.emplace_back(nullptr);

    SDL_Init(SDL_INIT_VIDEO);

    m_priv->window = unique_sdl_window_t(
                         SDL_CreateWindow("EGT",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          size.width(),
                                          size.height(),
                                          0));

    if (!m_priv->window)
        throw std::runtime_error(std::string("unable to create window: ") + SDL_GetError());

    m_priv->renderer = unique_sdl_renderer_t(
                           SDL_CreateRenderer(m_priv->window.get(), -1, SDL_RENDERER_SOFTWARE));
    if (!m_priv->renderer)
        throw std::runtime_error(std::string("unable to create renderer: ") + SDL_GetError());

    m_priv->texture = unique_sdl_texture_t(
                          SDL_CreateTexture(m_priv->renderer.get(), SDL_PIXELFORMAT_ARGB8888,
                                            SDL_TEXTUREACCESS_STREAMING,
                                            size.width(), size.height()));
    if (!m_priv->texture)
        throw std::runtime_error(std::string("unable to create texture: ") + SDL_GetError());

    m_thread = std::thread([size, this]()
    {
        //Work guard prevents io_context from finishing work before it gets more.
        asio::executor_work_guard<asio::io_context::executor_type> tn = asio::make_work_guard(m_io);

        Pointer pointer;
        Key key;
        SDL_Event event;
        while (!m_quit)
        {
            auto time = SDL_GetTicks();
            // Rendering and event handling
            while ((SDL_PollEvent(&event) != 0))
            {
                switch (event.type)
                {
                case SDL_QUIT:
                    asio::post(m_app.event().io(), []()
                    {
                        Application::instance().event().quit(0);
                    });
                    m_quit = true;
                    tn.reset();
                    break;
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                {
                    EGTLOG_DEBUG("physical {} key acting as {} key",
                                 SDL_GetScancodeName(event.key.keysym.scancode),
                                 SDL_GetKeyName(event.key.keysym.sym));

                    key.keycode = sdl_to_egtkeys(event.key.keysym.sym & ~SDLK_SCANCODE_MASK);
                    key.unicode = 0;
                    if ((event.key.keysym.sym & SDLK_SCANCODE_MASK) != SDLK_SCANCODE_MASK)
                    {
                        key.unicode = event.key.keysym.sym;
                    }

                    asio::post(m_app.event().io(), std::bind(&SDLScreen::key_event,
                               this, event.type == SDL_KEYDOWN ?
                               EventId::keyboard_down : EventId::keyboard_up, key));
                    break;
                }
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                {
                    switch (event.button.button)
                    {
                    case SDL_BUTTON_LEFT:
                        pointer.btn = Pointer::Button::left;
                        break;
                    case SDL_BUTTON_MIDDLE:
                        pointer.btn = Pointer::Button::middle;
                        break;
                    case SDL_BUTTON_RIGHT:
                        pointer.btn = Pointer::Button::right;
                        break;
                    }

                    asio::post(m_app.event().io(), std::bind(&SDLScreen::pointer_event,
                               this, event.type == SDL_MOUSEBUTTONDOWN ?
                               EventId::raw_pointer_down : EventId::raw_pointer_up, pointer));
                    break;
                }
                case SDL_MOUSEMOTION:
                    pointer.point = DisplayPoint(event.motion.x, event.motion.y);
                    asio::post(m_app.event().io(), std::bind(&SDLScreen::pointer_event,
                               this, EventId::raw_pointer_move, pointer));
                    break;
                }
            }
            m_io.poll();
            //This is an a sort of wait imposed for systems that have extremely
            //fast thread spinning, thus eating up uncessary cpu, happened on Ubuntu VM.
            if ((SDL_GetTicks() - time) < 10)
            {
                SDL_Delay(10);
            }
        }
    });

}

void SDLScreen::pointer_event(EventId e, const Pointer& pointer)
{
    Event event(e, pointer);
    m_in.dispatch(event);
}

void SDLScreen::key_event(EventId e, const Key& key)
{
    Event event(e);
    event.key() = key;
    m_in.dispatch(event);
}

void SDLScreen::flip(const DamageArray& damage)
{
    void* pixels{};
    int pitch{};

    if (SDL_LockTexture(m_priv->texture.get(), nullptr, &pixels, &pitch) != 0)
        throw std::runtime_error(std::string("failed to lock texture: ") + SDL_GetError());

    ScreenBuffer& buffer = m_buffers.front();
    buffer.surface = unique_cairo_surface_t(
                         cairo_image_surface_create_for_data(
                             static_cast<unsigned char*>(pixels),
                             CAIRO_FORMAT_ARGB32,
                             size().width(), size().height(), pitch));

    Screen::flip(damage);

    SDL_UnlockTexture(m_priv->texture.get());
    SDL_RenderCopy(m_priv->renderer.get(), m_priv->texture.get(), nullptr, nullptr);
    SDL_RenderPresent(m_priv->renderer.get());
}

KeyboardCode SDLScreen::sdl_to_egtkeys(int key)
{
    return static_cast<KeyboardCode>(key);
}

SDLScreen::~SDLScreen()
{
    // @todo this should be setup with atexit() once ... but where
    //SDL_Quit();
    m_quit = true;
    m_io.stop();
    m_thread.join();
}

}
}
}
