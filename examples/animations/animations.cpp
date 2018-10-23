/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include <mui/ui>
#include <mui/painter.h>
#include <string>
#include <vector>
#include <sstream>

using namespace mui;
using namespace std;

int main()
{
    Application app;
    Window win;

    StaticGrid grid(Rect(win.size()), 3, 2, 5);
    grid.flag_clear(widgetmask::NO_BORDER);
    win.add(&grid);

    experimental::AnimationSequence seq;

    experimental::PropertyAnimator in(0, win.w(), std::chrono::seconds(1));
    in.set_prop_callback(std::bind(&StaticGrid::resizew, std::ref(grid), std::placeholders::_1));
    seq.add(&in);

    Button button("hello world");
    grid.add(&button, 0, 0);

    experimental::PropertyAnimator shrink_in(100, 10, std::chrono::seconds(1));
    shrink_in.set_prop_callback(std::bind(&Button::resizew, std::ref(button), std::placeholders::_1));
    experimental::PropertyAnimator shrink_out(10, 100, std::chrono::seconds(1));
    shrink_out.set_prop_callback(std::bind(&Button::resizew, std::ref(button), std::placeholders::_1));
    seq.add(&shrink_in);
    seq.add(&shrink_out);

    Label label("hello world");
    grid.add(&label, 0, 1);

    experimental::PropertyAnimator shrinkh_in(100, 10, std::chrono::seconds(1));
    shrinkh_in.set_prop_callback(std::bind(&Label::resizew, std::ref(label), std::placeholders::_1));
    experimental::PropertyAnimator shrinkh_out(10, 100, std::chrono::seconds(1));
    shrinkh_out.set_prop_callback(std::bind(&Label::resizew, std::ref(label), std::placeholders::_1));
    seq.add(&shrinkh_in);
    seq.add(&shrinkh_out);

    //grid.reposition();

    seq.start();

    win.show();

    return app.run();
}
