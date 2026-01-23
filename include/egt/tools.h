/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_TOOLS_H
#define EGT_TOOLS_H

#include <atomic>
#include <chrono>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <egt/detail/meta.h>

#ifdef HAVE_LIBIIO
// Forward declaration for iiopp types
namespace iiopp
{
class Device;
}
#endif

/**
 * @file
 * @brief Collection of various tools.
 */

namespace egt
{
inline namespace v1
{
namespace experimental
{
/**
 * Monitor CPU usage of the system.
 */
class EGT_API CPUMonitorUsage
{
public:

    /**
     * Get the total CPU usage as a percentage.
     */
    EGT_NODISCARD inline double usage() const { return m_cpu_usage; }

    /**
     * Update the CPU usage.
     *
     * This must be called periodically to perform the calculation.
     */
    void update();

private:

    size_t m_last_total_time{0};
    size_t m_last_idle_time{0};
    double m_cpu_usage{0};
};

/**
 * Calculate "frame-per-second" of something.
 *
 * @code{.cpp}
 * FramesPerSecond fps;
 * fps.start();
 * while (true)
 * {
 *     std::cout << std::round(fps.fps()) << std::endl;
 *     fps.end_frame();
 * }
 * @endcode
 */
class EGT_API FramesPerSecond
{
public:

    FramesPerSecond() noexcept
    {
        start();
    }

    /**
     * Start/reset the counter.
     */
    void start()
    {
        m_start = std::chrono::steady_clock::now();
        m_frames = 0;
    }

    /**
     * Call at the end of every frame.
     */
    void end_frame()
    {
        m_frames++;

        const auto now = std::chrono::steady_clock::now();
        const auto diff = std::chrono::duration<double>(now - m_start).count();
        if (diff > 1.0)
        {
            m_fps = m_frames / diff;
            m_ready = true;
            start();
        }
    }

    /**
     * Is any calculation ready?
     */
    EGT_NODISCARD bool ready() const { return m_ready && m_frames > 0; }

    /**
     * Retrieve the current FPS value.
     */
    float fps()
    {
        m_ready = false;
        return m_fps;
    }

protected:

    /// Start time
    std::chrono::time_point<std::chrono::steady_clock> m_start{};

    /// Number of frames recorded since start time.
    uint64_t m_frames{0};

    /// Calculated FPS
    float m_fps{0.};

    /// Is m_fps valid and ready?
    bool m_ready{false};
};

/**
 * Performance monitor that collects and displays various performance metrics.
 *
 * This class runs in a separate thread to avoid interfering with the EventLoop.
 * It periodically collects metrics (FPS, CPU usage, etc.) and logs them.
 *
 * Metric tracking is controlled via the API:
 * - enable_fps_tracking(): Enable FPS collection
 * - enable_cpu_tracking(): Enable CPU usage collection
 *
 * Log display is controlled via environment variables:
 * - EGT_SHOW_FPS: Display FPS in logs
 * - EGT_SHOW_CPU: Display CPU usage in logs
 */
class EGT_API PerfMonitor
{
public:

    /**
     * Default constructor.
     */
    PerfMonitor();

    /**
     * Destructor.
     *
     * Automatically stops monitoring if running.
     */
    ~PerfMonitor();

    /**
     * Set the update interval for collecting metrics.
     *
     * @param interval Update interval (default: 1 second)
     */
    void set_update_interval(std::chrono::milliseconds interval);

    /**
     * Start monitoring.
     *
     * Starts a background thread that collects and logs statistics.
     */
    void start();

    /**
     * Stop monitoring.
     *
     * Stops the background thread and waits for it to finish.
     */
    void stop();

    /**
     * Check if monitoring is currently active.
     */
    EGT_NODISCARD bool running() const;

    /**
     * Enable or disable FPS tracking.
     *
     * When enabled, notify_frame() calls will be recorded for FPS calculation.
     * @param enable true to enable, false to disable
     */
    void enable_fps_tracking(bool enable = true);

    /**
     * Enable or disable CPU usage tracking.
     *
     * @param enable true to enable, false to disable
     */
    void enable_cpu_tracking(bool enable = true);

    /**
     * Enable or disable power tracking.
     *
     * @param enable true to enable, false to disable
     */
    void enable_power_tracking(bool enable = true);

    /**
     * Enable or disable temperature tracking.
     *
     * @param enable true to enable, false to disable
     */
    void enable_temperature_tracking(bool enable = true);

    /**
     * Check if FPS tracking is enabled.
     */
    EGT_NODISCARD bool fps_tracking_enabled() const;

    /**
     * Check if CPU tracking is enabled.
     */
    EGT_NODISCARD bool cpu_tracking_enabled() const;

    /**
     * Check if power tracking is enabled.
     */
    EGT_NODISCARD bool power_tracking_enabled() const;

    /**
     * Check if temperature tracking is enabled.
     */
    EGT_NODISCARD bool temperature_tracking_enabled() const;

    /**
     * Notify that a frame has been rendered.
     *
     * Must be called from the main thread after each rendered frame
     * to track FPS.
     */
    void notify_frame();

    /**
     * Add a power channel to monitor (direct power reading).
     *
     * @param device IIO device name (e.g., "pac1934")
     * @param channel IIO channel name that provides direct power reading (e.g., "power1")
     * @param description Human-readable description for logging
     * @param raw_attr_name Name of the raw attribute to read (default: "raw")
     * @return true if channel was successfully added, false otherwise
     */
    bool add_power_channel(const std::string& device,
                           const std::string& channel,
                           const std::string& description,
                           const std::string& raw_attr_name = "raw");

    /**
     * Add a power channel to monitor (calculated from voltage and current).
     *
     * @param device IIO device name (e.g., "pac1934")
     * @param voltage_channel IIO channel name for voltage reading
     * @param current_channel IIO channel name for current reading
     * @param description Human-readable description for logging
     * @param raw_attr_name Name of the raw attribute to read for both channels (default: "raw")
     * @return true if channel was successfully added, false otherwise
     */
    bool add_power_channel(const std::string& device,
                           const std::string& voltage_channel,
                           const std::string& current_channel,
                           const std::string& description,
                           const std::string& raw_attr_name = "raw");

    /**
     * Register a temperature sensor to monitor.
     *
     * The sysfs file is expected to contain a temperature value in
     * millidegrees Celsius (e.g., /sys/class/thermal/thermal_zone0/temp).
     *
     * @param sysfs_path Path to the sysfs file containing the temperature
     * @param description Human-readable description for logging
     * @return true if sensor was successfully added, false otherwise
     */
    bool add_temperature_sensor(const std::string& sysfs_path,
                                const std::string& description);

private:

    /**
     * Main monitoring loop running in the background thread.
     */
    void monitor_loop();

    /**
     * Check if FPS display is enabled via environment variable.
     */
    static bool show_fps_enabled();

    /**
     * Check if CPU usage display is enabled via environment variable.
     */
    static bool show_cpu_enabled();

    /**
     * Check if power consumption display is enabled via environment variable.
     */
    static bool show_power_enabled();

    /**
     * Check if temperature display is enabled via environment variable.
     */
    static bool show_temp_enabled();

#ifdef HAVE_LIBIIO
    /**
     * Ensure IIO context is initialized and find device.
     * @return Device if found, empty optional otherwise
     */
    std::optional<iiopp::Device> ensure_iio_device(const std::string& device_name);
#endif

    /// FPS monitor
    FramesPerSecond m_fps_monitor;

    /// CPU usage monitor
    CPUMonitorUsage m_cpu_monitor;

    /// Background thread
    std::unique_ptr<std::thread> m_thread;

    /// Flag to control the monitoring loop
    std::atomic<bool> m_running{false};

    /// Flag to signal thread to stop
    std::atomic<bool> m_stop_requested{false};

    /// Update interval
    std::chrono::milliseconds m_update_interval{1000};

    /// Tracking enable flags
    std::atomic<bool> m_track_fps{false};
    std::atomic<bool> m_track_cpu{false};
    std::atomic<bool> m_track_power{false};
    std::atomic<bool> m_track_temp{false};

    /// Forward declaration for PIMPL
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

}
}
}

#endif
