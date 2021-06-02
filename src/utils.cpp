/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/utils.h"

namespace egt
{
inline namespace v1
{

std::string egt_version()
{
#ifdef PACKAGE_VERSION
    return PACKAGE_VERSION; // NOLINT
#else
    return {};
#endif
}

std::string egt_git_version()
{
#ifdef GIT_VERSION
    return GIT_VERSION; // NOLINT
#else
    return {};
#endif
}

}
}
