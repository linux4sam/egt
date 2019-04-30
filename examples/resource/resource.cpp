/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <egt/ui>
#include <egt/painter.h>
#include <sstream>
#include <string>

using namespace std;
using namespace egt;

class MainWindow;

class MyButton : public ImageButton
{
public:
    MyButton(const string& filename,
             const string& label,
             int x = 0,
             int y = 0)
        : ImageButton(Image(filename), label, Rect(Point(x, y), Size()))
    {
        set_position_image_first(true);
        set_image_align(alignmask::center);
        set_text_align(alignmask::center | alignmask::bottom);
    }
};

class MainWindow : public TopWindow
{
public:
    explicit MainWindow(const Size& size)
        : TopWindow(size),
          grid(Tuple(2, 2), 10)
    {
        grid.instance_palette().set(Palette::ColorId::border, Palette::transparent);

        add(expand(grid));

        grid.add(expand(make_shared<MyButton>("image1.png", _("PNG-filename"))));
        grid.add(expand(make_shared<MyButton>(":image2_png", _("PNG-stream"))));
        grid.add(expand(make_shared<MyButton>("image3.bmp", _("BMP-filename"))));
        grid.add(expand(make_shared<MyButton>(":image4_bmp", _("BMP-stream"))));
    }

    StaticGrid grid;
};

int main(int argc, const char** argv)
{
    Application app(argc, argv, "resource");

    MainWindow win1(Size(800, 480));

    win1.show();

    return app.run();
}
