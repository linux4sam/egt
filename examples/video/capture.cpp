/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <iostream>

int main(int argc, const char** argv)
{
    egt::Application app(argc, argv);
    egt::experimental::CameraCapture capture("output.avi");

    capture.on_error([&capture]()
    {
        std::cout << "error: " << capture.error_message() << std::endl;
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

    capture.on_error([&capture, &app]()
    {
        std::cout << " Error:  " << capture.error_message() << std::endl;
        capture.stop();
        app.quit();
    });

    return app.run();
}
