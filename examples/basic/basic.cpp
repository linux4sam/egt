/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include <mui/ui>
#include <mui/uiloader.h>

#define USE_LOADER
#define SHARED_PATH "../share/mui/examples/basic/"

int main()
{
    mui::Application app;

#ifdef USE_LOADER
    mui::experimental::UiLoader loader;
    auto window = loader.load(SHARED_PATH "ui.xml");
    window->show();
#else
    mui::Window window;
    mui::Button button(window, "Press Me");
    button.align(mui::alignmask::CENTER);
    window.show();
#endif

    return app.run();
}
