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
};

}
}
}

#endif
