/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/egtlog.h"

namespace egt
{
inline namespace v1
{
namespace detail
{
static int l = EGTLOG_LEVEL_WARN;

void loglevel(int level)
{
    l = level;
}

int loglevel()
{
    return l;
}

}
}
}
