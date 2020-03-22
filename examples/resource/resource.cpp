/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <libintl.h>
#include <memory>
#include <string>

class MainWindow;

/**
 * This is a wrapper around gettext().
 */
#define _(String) gettext(String)

class ResourceFrame : public egt::Frame
{
public:
    ResourceFrame(const egt::Image& image,
                  const std::string& label)
    {
        auto sizer = std::make_shared<egt::VerticalBoxSizer>();
        add(expand(sizer));

        auto l = std::make_shared<egt::Label>(label);
        sizer->add(expand_horizontal(l));

        auto i = std::make_shared<egt::ImageLabel>(image);
        i->image_align(egt::AlignFlag::expand);
        sizer->add(egt::expand(i));
    }
};

class MainWindow : public egt::TopWindow
{
public:
    explicit MainWindow()
        : m_grid(std::make_tuple(2, 3))
    {
        auto sizer = std::make_shared<egt::VerticalBoxSizer>();
        add(expand(sizer));

        auto egt_logo = std::make_shared<egt::ImageLabel>(egt::Image("icon:egt_logo_black.png;128"));
        egt_logo->align(egt::AlignFlag::center | egt::AlignFlag::top);
        egt_logo->margin(5);
        sizer->add(egt_logo);

        sizer->add(egt::expand(m_grid));

        m_grid.add(expand(std::make_shared<ResourceFrame>(egt::Image("file:image1.png"), _("PNG-filename"))));
        m_grid.add(expand(std::make_shared<ResourceFrame>(egt::Image("res:image2_png"), _("PNG-stream"))));
        m_grid.add(expand(std::make_shared<ResourceFrame>(egt::Image("file:image3.bmp"), _("BMP-filename"))));
        m_grid.add(expand(std::make_shared<ResourceFrame>(egt::Image("res:image4_bmp"), _("BMP-stream"))));

        try
        {
            m_grid.add(egt::expand(std::make_shared<ResourceFrame>(egt::Image("file:image5.jpg"), _("JPEG-file"))));
        }
        catch (...)
        {}

        try
        {
            m_grid.add(egt::expand(std::make_shared<ResourceFrame>(egt::SvgImage("file:image6.svg", egt::SizeF(0, 205)), _("SVG-file"))));
        }
        catch (...)
        {}
    }

    egt::StaticGrid m_grid;
};

int main(int argc, char** argv)
{
    egt::Application app(argc, argv, "resource");
#ifdef EXAMPLEDATA
    egt::add_search_path(EXAMPLEDATA);
#endif

    MainWindow window;
    window.show();

    return app.run();
}
