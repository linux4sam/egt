/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <cstdint>

static void do_something(const char*) {}

/// @[snippet9]
class MyClass
{
public:
    MyClass(std::size_t size)
    {
        m_ptr = new char[size];
    }

    ~MyClass()
    {
        delete [] m_ptr;
    }

    char* get() { return m_ptr; }
private:
    char* m_ptr{nullptr};
};

static void func()
{
    MyClass instance(32);
    do_something(instance.get());
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
/// @[snippet9]
