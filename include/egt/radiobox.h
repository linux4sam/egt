/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_RADIOBOX_H
#define EGT_RADIOBOX_H

/**
 * @file
 * @brief Working with checkboxes.
 */
#include <egt/button.h>

namespace egt
{
inline namespace v1
{
class Painter;

/**
 * @brief Boolean RadioBox.
 */
class RadioBox : public Button
{
public:
    RadioBox(const std::string& text = std::string(),
             const Rect& rect = Rect());

    virtual int handle(eventid event) override;

    virtual void draw(Painter& painter, const Rect& rect) override;

    virtual ~RadioBox() = default;
};

}
}

#endif
