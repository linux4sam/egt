/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <iostream>
using namespace std;

/// @[snippet3]
int add(int first, int second)
{
    return first + second;
}

int main()
{
    float x = add(.4f, .6f);
    cout << x << endl;
    return 0;
}
/// @[snippet3]
