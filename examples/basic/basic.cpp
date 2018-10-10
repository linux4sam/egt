/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include <mui/ui>

using namespace mui;

int main()
{
    Application app;

    Window win;
    win.add(new Button("Hello World"))->align(Label::ALIGN_CENTER);
    win.show();

    return app.run();
}
