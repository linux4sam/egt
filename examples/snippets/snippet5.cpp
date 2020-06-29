/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <iostream>

template<class T>
void register_callback(T)
{}

/// @[snippet5]
static int value = 0;

static void callback(egt::Event&)
{
    value++;
    std::cout << value << std::endl;
}

int main()
{
    // do something

    register_callback(callback);

    // do something

    return 0;
}
/// @[snippet5]
