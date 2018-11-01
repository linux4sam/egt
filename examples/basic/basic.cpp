/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include <mui/ui>

int main()
{
    mui::Application app;

    mui::Window window;
    mui::Button button(window, "Hello World");
    button.align(mui::alignmask::CENTER);
    window.show();

    return app.run();
}
