/*
 * Copyright (C) 2026 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * @file
 * @brief Minimal video player with performance monitoring.
 *
 * This example demonstrates video playback with PerfMonitor integration
 * for FPS, CPU, power consumption and temperature monitoring.
 *
 * Board selection:
 *   --board <name>  - Select board configuration (default: sama7d65_curiosity)
 *
 * Tracking options (all enabled by default):
 *   --no-fps    - Disable FPS tracking
 *   --no-cpu    - Disable CPU tracking
 *   --no-power  - Disable power tracking
 *   --no-temp   - Disable temperature tracking
 *
 * Use environment variables to control log output:
 *   EGT_SHOW_FPS=1    - Display frames per second
 *   EGT_SHOW_CPU=1    - Display CPU usage
 *   EGT_SHOW_POWER=1  - Display power consumption (requires IIO)
 *   EGT_SHOW_TEMP=1   - Display temperature
 */
#include <cxxopts.hpp>
#include <egt/ui>
#include <iostream>

/**
 * Configure PerfMonitor for a specific board.
 *
 * @param perf_monitor Reference to the PerfMonitor instance
 * @param board_name Name of the board to configure
 * @return true if board is recognized and all sensors configured, false otherwise
 */
static bool configure_board(egt::experimental::PerfMonitor& perf_monitor,
                            const std::string& board_name)
{
    if (board_name == "sama7d65_curiosity")
    {
        // Power monitoring channels (PAC1934)
        if (!perf_monitor.add_power_channel("pac1934", "voltage7", "current7", "VDDCORE", "mean_raw"))
            return false;
        if (!perf_monitor.add_power_channel("pac1934", "voltage6", "current6", "VDDIODDR", "mean_raw"))
            return false;
        if (!perf_monitor.add_power_channel("pac1934", "voltage5", "current5", "VDD3V3", "mean_raw"))
            return false;
        if (!perf_monitor.add_power_channel("pac1934", "voltage8", "current8", "VDDCPU", "mean_raw"))
            return false;

        // Temperature sensor
        if (!perf_monitor.add_temperature_sensor("/sys/class/thermal/thermal_zone0/temp", "CPU"))
            return false;

        return true;
    }

    // Add new boards here

    return false;
}

int main(int argc, char** argv)
{
    cxxopts::Options options(argv[0], "Minimal video player with performance monitoring");
    options.add_options()
    ("h,help", "Show help")
    ("i,input", "URI to video file", cxxopts::value<std::string>())
    ("width", "Width of the stream", cxxopts::value<int>()->default_value("320"))
    ("height", "Height of the stream", cxxopts::value<int>()->default_value("192"))
    ("f,format", "Pixel format", cxxopts::value<std::string>()->default_value("yuv420"), "[egt::PixelFormat]")
    ("window-hint", "Window hint: overlay or software", cxxopts::value<std::string>()->default_value("overlay"))
    ("pipeline", "Custom GStreamer pipeline", cxxopts::value<std::string>())
    ("no-fps", "Disable FPS tracking")
    ("no-cpu", "Disable CPU tracking")
    ("no-power", "Disable power tracking")
    ("no-temp", "Disable temperature tracking")
    ("board", "Board name for power/temp configuration", cxxopts::value<std::string>()->default_value("sama7d65_curiosity"), "[sama7d65_curiosity]");
    auto args = options.parse(argc, argv);

    if (args.count("help") ||
        (!args.count("input") && !args.count("pipeline")))
    {
        std::cout << options.help() << std::endl;
        return 0;
    }

    egt::Size size(args["width"].as<int>(), args["height"].as<int>());
    auto format = egt::detail::enum_from_string<egt::PixelFormat>(args["format"].as<std::string>());
    const auto input = args.count("pipeline") ? args["pipeline"].as<std::string>() : args["input"].as<std::string>();
    const bool is_pipeline = args.count("pipeline");
    const auto hint_str = args["window-hint"].as<std::string>();
    const auto hint = (hint_str == "software") ? egt::WindowHint::software : egt::WindowHint::overlay;

    egt::Application app(argc, argv);

    // Enable tracking based on command-line options (enabled by default)
    app.perf_monitor().enable_fps_tracking(!args.count("no-fps"));
    app.perf_monitor().enable_cpu_tracking(!args.count("no-cpu"));

    // Configure board-specific power and temperature monitoring
    const auto board = args["board"].as<std::string>();
    if (!configure_board(app.perf_monitor(), board))
    {
        std::cerr << "Failed to configure board: " << board << std::endl;
        std::cerr << "Only FPS and CPU are tracked" << std::endl;
    }
    else
    {
        app.perf_monitor().enable_power_tracking(!args.count("no-power"));
        app.perf_monitor().enable_temperature_tracking(!args.count("no-temp"));
    }

    egt::TopWindow win;
    win.color(egt::Palette::ColorId::bg, egt::Palette::black);

    egt::VideoWindow player(size, format, hint);
    player.move_to_center(win.center());
    win.add(player);

    // Start playing when window is shown
    win.on_show([&player, input, is_pipeline, &app]()
    {
        if (is_pipeline)
            player.gst_custom_pipeline(input);
        else
            player.media(input);

        app.perf_monitor().start();
        app.perf_monitor().start_accumulate();
        player.play();
    });

    player.on_error([](const std::string& err)
    {
        std::cerr << "Error: " << err << std::endl;
    });

    player.on_eos([&app]()
    {
        app.perf_monitor().stop_accumulate();
        app.perf_monitor().stop();
        app.perf_monitor().log_averages(egt::experimental::PerfMonitor::EdgePolicy::DiscardEdges);
        app.quit();
    });

    win.show();
    player.show();

    return app.run();
}
