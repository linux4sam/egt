/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/themes/shamrock.h"
#include "egt/painter.h"
#include "egt/widget.h"
#include "egt/checkbox.h"

namespace egt
{
inline namespace v1
{

void ShamrockTheme::init_draw()
{
    Drawer<CheckBox>::draw(CheckBox::default_draw);
}

}
}
