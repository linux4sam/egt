/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/themes/midnight.h"
#include "egt/painter.h"
#include "egt/widget.h"
#include "egt/checkbox.h"

namespace egt
{
inline namespace v1
{

void MidnightTheme::init_draw()
{
    Drawer<CheckBox>::set_draw(CheckBox::default_draw);
}

}
}
