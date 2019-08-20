/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <iostream>
using namespace std;

/// @[snippet7]
struct MyClass
{
    MyClass()
    {
        cout << "constructed" << endl;
    }

    ~MyClass()
    {
        cout << "destructed" << endl;
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
