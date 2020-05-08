/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <cxxopts.hpp>
#include <egt/ui>
#include <iostream>

int main(int argc, char** argv)
{
    cxxopts::Options options(argv[0], "capture camera video stream to file");
    options.add_options()
    ("h,help", "Show help")
    ("f,file", "output file", cxxopts::value<std::string>()->default_value("output.avi"))
    ("d,device", "V4L2 device", cxxopts::value<std::string>()->default_value("/dev/video"));
    auto args = options.parse(argc, argv);

    if (args.count("help"))
    {
        std::cout << options.help() << std::endl;
        return 0;
    }

    egt::Application app(argc, argv);

    auto dev(args["device"].as<std::string>());
    egt::experimental::CameraCapture capture(args["file"].as<std::string>(),
            egt::experimental::CameraCapture::ContainerType::avi,
            egt::PixelFormat::yuyv,
            dev);

    capture.on_error([&app](const std::string & err)
    {
        std::cout << "error: " << err << std::endl;
        app.quit();
    });

    egt::Timer stop_timer(std::chrono::seconds(10));
    stop_timer.on_timeout([&capture, &app]()
    {
        std::cout << "stopping capture." << std::endl;
        capture.stop();
        app.quit();
    });

    if (capture.start())
    {
        std::cout << "capturing for 10 seconds..." << std::flush;
        stop_timer.start();
    }

    capture.on_disconnect([&capture, dev, &stop_timer, &app](const std::string & devnode)
    {
        std::cout << "Device removed: " << devnode << std::endl;
        stop_timer.stop();
        capture.stop();
        app.quit();
    });

    return app.run();
}
