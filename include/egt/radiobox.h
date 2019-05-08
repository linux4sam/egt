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
#include <string>

namespace egt
{
inline namespace v1
{
class Painter;

/**
 * Boolean RadioBox.
 *
 * @ingroup controls
 */
class RadioBox : public Button
{
public:
    RadioBox(const std::string& text = std::string(),
             const Rect& rect = Rect());

    virtual int handle(eventid event) override;

    virtual void draw(Painter& painter, const Rect& rect) override;

    /**
     * Default draw method for the RadioBox.
     */
    static void default_draw(RadioBox& widget, Painter& painter, const Rect& rect);

    virtual Size min_size_hint() const override;

    virtual ~RadioBox() = default;
};

}
}

#endif
