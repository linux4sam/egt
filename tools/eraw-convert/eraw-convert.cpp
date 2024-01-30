/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <cxxopts.hpp>
#include <erawimage.h>
#include <iostream>

int main(int argc, char** argv)
{
    cxxopts::Options options("eraw-convert", "eraw image format converter");
    options.add_options()
    ("h,help", "help")
    ("i,input-format", "input format (png)",
     cxxopts::value<std::string>()->default_value("png"))
    ("o,output-format", "output format (eraw, png, raw)",
     cxxopts::value<std::string>()->default_value("eraw"))
    ("positional", "SOURCE DEST", cxxopts::value<std::vector<std::string>>())
    ;
    options.positional_help("SOURCE DEST");

    options.parse_positional({"positional"});
    auto result = options.parse(argc, argv);

    if (result.count("help"))
    {
        std::cout << options.help() << std::endl;
        return 0;
    }

    if (result.count("positional") != 2)
    {
        std::cerr << options.help() << std::endl;
        return 1;
    }

    auto& positional = result["positional"].as<std::vector<std::string>>();

    std::string in = positional[0];
    std::string out = positional[1];

    egt::shared_cairo_surface_t surface;

    if (result["input-format"].as<std::string>() == "png")
    {
        surface =
            egt::shared_cairo_surface_t(cairo_image_surface_create_from_png(in.c_str()),
                                        cairo_surface_destroy);
    }
    else if (result["input-format"].as<std::string>() == "eraw")
    {
        egt::detail::ErawImage e;
        surface = e.load(in);
    }
    else
    {
        std::cerr << "error: unknown input-format " <<
                  result["input-format"].as<std::string>() << std::endl;
        return 1;
    }

    if (!surface ||
        cairo_surface_status(surface.get()) != CAIRO_STATUS_SUCCESS)
    {
        std::cerr << "error: unable to open input " << in << std::endl;
        return 1;
    }

    const auto data = cairo_image_surface_get_data(surface.get());
    const auto width = cairo_image_surface_get_width(surface.get());
    const auto height = cairo_image_surface_get_height(surface.get());

    if (result["output-format"].as<std::string>() == "eraw")
    {
        egt::detail::ErawImage e;
        e.save(out, data, width, height);
    }
    else if (result["output-format"].as<std::string>() == "raw")
    {
        auto size = width * height  * sizeof(uint32_t);
        std::ofstream o(out, std::ios_base::binary);
        if (!o.is_open())
        {
            std::cerr << "error: unable to write to file file " << out << std::endl;
            return 1;
        }

        o.write(reinterpret_cast<const char*>(data), size);
        o.close();
    }
    else if (result["output-format"].as<std::string>() == "png")
    {
        if (cairo_surface_write_to_png(surface.get(), out.c_str()) != CAIRO_STATUS_SUCCESS)
        {
            std::cerr << "error: unable to write to file file " << out << std::endl;
            return 1;
        }
    }
    else
    {
        std::cerr << "error: unknown output-format " <<
                  result["output-format"].as<std::string>() << std::endl;
        return 1;
    }

    return 0;
}
