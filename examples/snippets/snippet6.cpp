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

/// @[snippet6]
int main()
{
    // do something

    int value = 0;
    register_callback(
        [&value](egt::Event&)
    {
        value++;
        std::cout << value << std::endl;
    }
    );

    // do something

    return 0;
}
/// @[snippet6]
