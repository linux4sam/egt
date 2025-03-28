/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_SCREEN_MEMORYSCREEN_H
#define EGT_DETAIL_SCREEN_MEMORYSCREEN_H

/**
 * @file
 * @brief Working with an in-memory screen.
 */

#include <egt/screen.h>
#include <string>

namespace egt
{
inline namespace v1
{
namespace detail
{

/**
 * Screen in an in-memory buffer.
 */
class MemoryScreen : public Screen
{
public:

    explicit MemoryScreen(const Size& size = Size(800, 480));

    void schedule_flip() override {}

    virtual void save_to_file(const std::string& filename) const;
};

}
}
}

#endif
