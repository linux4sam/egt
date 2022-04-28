/*
 * Copyright (C) 2021 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_SCREEN_COMPOSERSCREEN_H
#define EGT_DETAIL_SCREEN_COMPOSERSCREEN_H

/**
 * @file
 * @brief Working with an in-memory screen to be used with the MGC.
 */

#include <egt/screen.h>
#include <egt/signal.h>

namespace egt
{
inline namespace v1
{
namespace detail
{

/**
 * Screen in an in-memory buffer for the Microchip Graphic Composer.
 */
class EGT_API ComposerScreen : public Screen
{
public:

    explicit ComposerScreen(const Size& size = Size(800, 480));

    void schedule_flip() override {}

    EGT_NODISCARD bool is_composer() const override { return true; }

    EGT_NODISCARD unsigned char* get_pixmap();

    void resize(const Size& size);

    /**
     * Register a handler to manage screen size changes.
     *
     * @param[in]: handler a callback to execute when the 'on_screen_resized'
     *             signal is invoked.
     *
     * @return a handle that can be unregister later.
     *
     * @note This signal-base mechanism is suited to manage 'static' watchers
     * like default widget sizes, especially within templates, because the
     * ComposerScreen itself can't guess exhaustive lists of all templates
     * instances (SliderType<int>, SliderType<float>, ...) to reset the default
     * sizes for all of them.
     *
     * However, it is not suited to manage 'instance' watchers like fonts and
     * widgets for which we should have to deal with registrations from their
     * copy/move constructors and assignment operators and with unregistrations
     * from the destructors.
     *
     * Besides, this method is 'static' so it can be called before the
     * ComposerScreen instance is created (from MGC plugin) or even if it is
     * not constructed at all, which is what happens anywhere else, like on
     * targets.
     */
    static Signal<>::RegisterHandle register_screen_resize_hook(const Signal<>::EventCallback& handler);

    /**
     * Unregister a handler that managed screen size changes.
     *
     * @param[in]: handle the returned value of register_screen_resize_hook()
     *
     * @note This method is 'static' like unregister_screen_resize_hook() for
     * the very same reason.
     *
     */
    static void unregister_screen_resize_hook(Signal<>::RegisterHandle handle);

private:
    /// Invoked by resize().
    static Signal<> on_screen_resized;
};

}
}
}

#endif
