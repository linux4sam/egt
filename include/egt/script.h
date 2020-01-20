/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SCRIPT_H
#define EGT_SCRIPT_H

/**
 * @file
 * @brief Working with scripts.
 */

#include <egt/detail/meta.h>
#include <string>

namespace egt
{
inline namespace v1
{
namespace experimental
{

/**
 * Toy lua evaluation.
 *
 * This takes a LUA expression, evaluates it, and returns the result.
 *
 * @param expr LUA expression.
 */
EGT_API double lua_evaluate(const std::string& expr);

}
}
}

#endif
