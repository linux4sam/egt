/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <iostream>
using namespace std;

/// @[snippet4]
template<class T>
T add(T first, T second)
{
    return first + second;
}

int main()
{
    int x = add(5, 5);
    float y = add(.4f, .6f);
    cout << x << endl;
    cout << y << endl;
    return 0;
}
/// @[snippet4]
