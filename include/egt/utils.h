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

#include <string>

namespace egt
{
inline namespace v1
{

namespace experimental
{

/**
 * Toy lua evaluation.
 */
double lua_evaluate(const std::string& expr);

}

}
}

#endif
