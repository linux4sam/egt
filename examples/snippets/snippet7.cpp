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

void func()
{
    MyClass instance;
}

int main(int argc, const char** argv)
{
    func();
    return 0;
}
/// @[snippet7]
