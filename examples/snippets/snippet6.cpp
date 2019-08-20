/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <iostream>
#include <egt/ui>

using namespace egt;
using namespace std;

template<class T>
void register_callback(T)
{}

/// @[snippet6]
int main(int argc, const char** argv)
{
    // do something

    int value = 0;
    register_callback(
        [&value](Event & event)
    {
        value++;
        cout << value << endl;
    }
    );

    // do something

    return 0;
}
/// @[snippet6]
