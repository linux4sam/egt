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

class ResourceFrame : public Frame
{
public:
    ResourceFrame(const Image& image,
                  const string& label)
    {
        auto sizer = std::make_shared<VerticalBoxSizer>();
        add(expand(sizer));

        auto l = std::make_shared<Label>(label);
        sizer->add(expand_horizontal(l));

        auto i = std::make_shared<ImageLabel>(image);
        i->set_image_align(alignmask::expand);
        sizer->add(expand(i));
    }
};

class MainWindow : public TopWindow
{
public:
    explicit MainWindow()
        : m_grid(std::make_tuple(2, 3))
    {
        auto sizer = std::make_shared<VerticalBoxSizer>();
        add(expand(sizer));

        auto egt_logo = std::make_shared<ImageLabel>(Image("@128px/egt_logo_black.png"));
        egt_logo->set_align(alignmask::center | alignmask::top);
        egt_logo->set_margin(5);
        sizer->add(egt_logo);

        sizer->add(expand(m_grid));

        m_grid.add(expand(make_shared<ResourceFrame>(Image("image1.png"), _("PNG-filename"))));
        m_grid.add(expand(make_shared<ResourceFrame>(Image(":image2_png"), _("PNG-stream"))));
        m_grid.add(expand(make_shared<ResourceFrame>(Image("image3.bmp"), _("BMP-filename"))));
        m_grid.add(expand(make_shared<ResourceFrame>(Image(":image4_bmp"), _("BMP-stream"))));

        try
        {
            m_grid.add(expand(make_shared<ResourceFrame>(Image("image5.jpg"), _("JPEG-file"))));
        }
        catch (...)
        {}

        try
        {
            m_grid.add(expand(make_shared<ResourceFrame>(SvgImage("image6.svg", SizeF(0, 205)), _("SVG-file"))));
        }
        catch (...)
        {}
    }

    StaticGrid m_grid;
};

int main(int argc, const char** argv)
{
    Application app(argc, argv, "resource");

    MainWindow window;
    window.show();

    return app.run();
}
