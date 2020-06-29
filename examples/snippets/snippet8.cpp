/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
static void do_something(const char*) {}

/// @[snippet8]
static void func()
{
    char* ptr = new char[32];
    do_something(ptr);
    delete [] ptr;
}

int main()
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
