/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_UTILS_H
#define EGT_UTILS_H

/**
 * @file
 * @brief Working with basic utilities.
 */

#include <egt/detail/meta.h>
#include <string>

namespace egt
{
inline namespace v1
{

/**
 * Get the version of the egt library.
 */
EGT_API std::string egt_version();

namespace experimental
{

/**
 * Toy lua evaluation.
 */
EGT_API double lua_evaluate(const std::string& expr);

}

}
}

#endif
