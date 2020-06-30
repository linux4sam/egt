/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <iostream>

/// @[snippet2]
static int add(int first, int second)
{
    return first + second;
}

int main()
{
    int x = add(5, 5);
    std::cout << x << std::endl;
    return 0;
}
/// @[snippet2]
