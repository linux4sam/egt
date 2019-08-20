/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <iostream>
using namespace std;

/// @[snippet2]
int add(int first, int second)
{
    return first + second;
}

int main()
{
    int x = add(5, 5);
    cout << x << endl;
    return 0;
}
/// @[snippet2]
