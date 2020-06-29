/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <iostream>

/// @[snippet7]
struct MyClass
{
    MyClass()
    {
        std::cout << "constructed" << std::endl;
    }

    ~MyClass()
    {
        std::cout << "destructed" << std::endl;
    }
};

static void func()
{
    MyClass instance;
}

int main()
{
    func();
    return 0;
}
/// @[snippet7]
