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

/// @[snippet5]
static int value = 0;

static void callback(Event& event)
{
    value++;
    cout << value << endl;
}

int main(int argc, const char** argv)
{
    // do something

    register_callback(callback);

    // do something

    return 0;
}
/// @[snippet5]
