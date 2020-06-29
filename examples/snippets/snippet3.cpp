/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <iostream>

/// @[snippet3]
static int add(int first, int second)
{
    return first + second;
}

int main()
{
    float x = add(.4f, .6f);
    std::cout << x << std::endl;
    return 0;
}
/// @[snippet3]
