/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <iostream>
using namespace std;

void do_something(const char*) {}

/// @[snippet8]
void func()
{
    char* ptr = new char[32];
    do_something(ptr);
    delete [] ptr;
}

int main(int argc, const char** argv)
{
    try
    {
        func();
    }
    catch (...)
    {}
    return 0;
}
/// @[snippet8]
