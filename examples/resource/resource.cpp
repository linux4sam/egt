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

MainWindow* win1;

class MyButton : public ImageButton
{
public:
    MyButton(const string& filename,
             const string& label,
             int x = 0,
             int y = 0,
             Widget::flags_type flags = Widget::flags_type())
        : ImageButton(Image(filename), label, Rect(Point(x, y), Size()), flags)
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
          grid(Rect(Point(0, 0), Size(800, 480)), 2, 2, 10)
    {
        grid.palette().set(Palette::ColorId::border, Palette::GroupId::normal, Palette::transparent);

        add(&grid);

        MyButton* b1 = new MyButton("image1.png", _("PNG-filename"));
        grid.add(b1);

        MyButton* b2 = new MyButton(":image2_png", _("PNG-stream"));
        grid.add(b2);

        MyButton* b3 = new MyButton("image3.bmp", _("BMP-filename"));
        grid.add(b3);

        MyButton* b4 = new MyButton(":image4_bmp", _("BMP-stream"));
        grid.add(b4);

    }

    StaticGrid grid;
};

int main(int argc, const char** argv)
{
    Application app(argc, argv, "resource");

    win1 = new MainWindow(Size(800, 480));

    win1->show();

    return app.run();
}
