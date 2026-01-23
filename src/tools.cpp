/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/tools.h"
#include "detail/egtlog.h"
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <optional>
#include <vector>
#ifdef HAVE_LIBIIO
#include <iiopp.h>
#endif

namespace egt
{
inline namespace v1
{
namespace experimental
{

static void get_cpu_times(std::vector<size_t>& times)
{
    std::ifstream in("/proc/stat");
    if (in.is_open())
    {
        in.ignore(5, ' ');
        size_t time{};
        while (in >> time)
            times.push_back(time);
    }
}

static bool get_cpu_times(size_t& idle_time, size_t& total_time)
{
    static std::vector<size_t> times;
    times.clear();
    get_cpu_times(times);
    if (times.size() < 4)
        return false;
    idle_time = times[3];
    total_time = std::accumulate(times.begin(), times.end(), static_cast<size_t>(0));
    return true;
}

void CPUMonitorUsage::update()
{
    size_t idle_time{};
    size_t total_time{};
    if (get_cpu_times(idle_time, total_time))
    {
        const double idle_time_diff = idle_time - m_last_idle_time;
        const double total_time_diff = total_time - m_last_total_time;
        m_cpu_usage = 100.0 * (1.0 - idle_time_diff / total_time_diff);
        m_last_idle_time = idle_time;
        m_last_total_time = total_time;
    }
}

struct PerfMonitor::Impl
{
#ifdef HAVE_LIBIIO
    struct PowerChannel
    {
        enum class Type
        {
            Direct,      // Direct power reading from a single channel
            Calculated   // Calculated from voltage and current channels
        };

        Type type;
        std::string description;
        iiopp::Device iio_device;                          // IIO device (stored directly, move-only)
        iiopp::Channel iio_channel;                        // For Direct: power channel. For Calculated: voltage channel
        std::optional<iiopp::Channel> iio_channel2;        // For Calculated: current channel. Empty for Direct
        std::string raw_attr_name;                         // Raw attribute name to read (same for both channels in Calculated mode)
    };

    std::vector<PowerChannel> power_channels;
    std::shared_ptr<iiopp::Context> iio_context;  // Shared IIO context for all channels
#endif
};

namespace
{

#ifdef HAVE_LIBIIO
/**
 * Read and compute value from IIO channel using formula: (raw + offset) * scale / 1000
 * @param channel IIO channel to read from
 * @param raw_attr_name Name of the raw attribute to read (e.g., "raw", "mean_raw")
 * @return Computed value in units, or std::nullopt if reading fails
 */
std::optional<double> read_channel_value(iiopp::Channel& channel,
                                         const std::string& raw_attr_name)
{
    try
    {
        const auto raw_attr = channel.attr(raw_attr_name);
        if (!raw_attr)
            return std::nullopt;

        const auto scale_attr = channel.attr("scale");
        const auto offset_attr = channel.attr("offset");

        const double raw_value = raw_attr->read_double();
        const double scale = scale_attr ? scale_attr->read_double() : 1.0;
        const double offset = offset_attr ? offset_attr->read_double() : 0.0;

        // Convert value from milli-units to units
        return (raw_value + offset) * scale / 1000.0;
    }
    catch (const iiopp::error&)
    {
        return std::nullopt;
    }
}
#endif

} // anonymous namespace

bool PerfMonitor::show_fps_enabled()
{
    static const bool value = (getenv("EGT_SHOW_FPS") != nullptr);
    return value;
}

bool PerfMonitor::show_cpu_enabled()
{
    static const bool value = (getenv("EGT_SHOW_CPU") != nullptr);
    return value;
}

bool PerfMonitor::show_power_enabled()
{
    static const bool value = (getenv("EGT_SHOW_POWER") != nullptr);
    return value;
}

PerfMonitor::PerfMonitor()
    : m_impl(std::make_unique<Impl>())
{
}

PerfMonitor::~PerfMonitor()
{
    stop();
}

void PerfMonitor::set_update_interval(std::chrono::milliseconds interval)
{
    m_update_interval = interval;
}

void PerfMonitor::start()
{
    if (running())
        return;

    m_stop_requested = false;
    m_running = true;
    m_thread = std::make_unique<std::thread>(&PerfMonitor::monitor_loop, this);
}

void PerfMonitor::stop()
{
    if (!running())
        return;

    m_stop_requested = true;

    if (m_thread && m_thread->joinable())
    {
        m_thread->join();
        m_thread.reset();
    }
    m_running = false;
}

bool PerfMonitor::running() const
{
    return m_running;
}

void PerfMonitor::enable_fps_tracking(bool enable)
{
    m_track_fps = enable;
}

void PerfMonitor::enable_cpu_tracking(bool enable)
{
    m_track_cpu = enable;
}

void PerfMonitor::enable_power_tracking(bool enable)
{
    m_track_power = enable;
}

bool PerfMonitor::fps_tracking_enabled() const
{
    return m_track_fps;
}

bool PerfMonitor::cpu_tracking_enabled() const
{
    return m_track_cpu;
}

bool PerfMonitor::power_tracking_enabled() const
{
    return m_track_power;
}

void PerfMonitor::notify_frame()
{
    if (fps_tracking_enabled())
        m_fps_monitor.end_frame();
}

#ifdef HAVE_LIBIIO
std::optional<iiopp::Device> PerfMonitor::ensure_iio_device(const std::string& device_name)
{
    if (!m_impl)
        return std::nullopt;

    try
    {
        if (!m_impl->iio_context)
            m_impl->iio_context = iiopp::create_local_context();

        iiopp::Device dev = m_impl->iio_context->find_device(device_name);
        if (!dev)
        {
            detail::warn("IIO device '{}' not found", device_name);
            return std::nullopt;
        }

        return dev;
    }
    catch (const iiopp::error& e)
    {
        detail::warn("Failed to access IIO device '{}': {}", device_name, e.what());
        return std::nullopt;
    }
}
#endif

bool PerfMonitor::add_power_channel([[maybe_unused]] const std::string& device,
                                    [[maybe_unused]] const std::string& channel,
                                    [[maybe_unused]] const std::string& description,
                                    [[maybe_unused]] const std::string& raw_attr_name)
{
#ifdef HAVE_LIBIIO
    auto dev_opt = ensure_iio_device(device);
    if (!dev_opt)
        return false;

    try
    {
        iiopp::Device dev = std::move(*dev_opt);
        iiopp::Channel ch = dev.find_channel(channel, false);
        if (!ch)
        {
            detail::warn("IIO channel '{}' not found in device '{}'", channel, device);
            return false;
        }

        m_impl->power_channels.push_back({
            Impl::PowerChannel::Type::Direct,
            description,
            std::move(dev),
            std::move(ch),
            std::nullopt,
            raw_attr_name
        });
        return true;
    }
    catch (const iiopp::error& e)
    {
        detail::warn("Failed to add power channel '{}' from device '{}': {}", channel, device, e.what());
        return false;
    }
#else
    return false;
#endif
}

bool PerfMonitor::add_power_channel([[maybe_unused]] const std::string& device,
                                    [[maybe_unused]] const std::string& voltage_channel,
                                    [[maybe_unused]] const std::string& current_channel,
                                    [[maybe_unused]] const std::string& description,
                                    [[maybe_unused]] const std::string& raw_attr_name)
{
#ifdef HAVE_LIBIIO
    auto dev_opt = ensure_iio_device(device);
    if (!dev_opt)
        return false;

    try
    {
        iiopp::Device dev = std::move(*dev_opt);
        iiopp::Channel voltage_ch = dev.find_channel(voltage_channel, false);
        if (!voltage_ch)
        {
            detail::warn("IIO voltage channel '{}' not found in device '{}'", voltage_channel, device);
            return false;
        }

        iiopp::Channel current_ch = dev.find_channel(current_channel, false);
        if (!current_ch)
        {
            detail::warn("IIO current channel '{}' not found in device '{}'", current_channel, device);
            return false;
        }

        m_impl->power_channels.push_back({
            Impl::PowerChannel::Type::Calculated,
            description,
            std::move(dev),
            std::move(voltage_ch),
            std::move(current_ch),
            raw_attr_name
        });
        return true;
    }
    catch (const iiopp::error& e)
    {
        detail::warn("Failed to add power channel '{}'/'{}' from device '{}': {}", voltage_channel, current_channel, device, e.what());
        return false;
    }
#else
    return false;
#endif
}

void PerfMonitor::monitor_loop()
{
    while (!m_stop_requested)
    {
        // Schedule next update based on the desired interval
        const auto next_update = std::chrono::steady_clock::now() + m_update_interval;

        const bool show_fps = show_fps_enabled();
        const bool show_cpu = show_cpu_enabled();
        const bool show_power = show_power_enabled();

        const bool show_any = show_fps || show_cpu || show_power;
        if (show_any)
            std::cout << "PerfMonitor:";

        if (fps_tracking_enabled())
        {
            const float fps = m_fps_monitor.fps();
            if (show_fps)
                std::cout << " FPS: " << std::fixed << std::setprecision(0) << std::round(fps);
        }

        if (cpu_tracking_enabled())
        {
            m_cpu_monitor.update();
            const double cpu = m_cpu_monitor.usage();
            if (show_cpu)
                std::cout << " CPU: " << std::fixed << std::setprecision(0) << cpu << "%";
        }

#ifdef HAVE_LIBIIO
        if (power_tracking_enabled() && m_impl && !m_impl->power_channels.empty())
        {
            for (auto& power_ch : m_impl->power_channels)
            {
                try
                {
                    std::optional<double> watts_opt;

                    if (power_ch.type == Impl::PowerChannel::Type::Direct)
                    {
                        watts_opt = read_channel_value(power_ch.iio_channel, power_ch.raw_attr_name);
                        if (watts_opt)
                        {
                            if (show_power)
                                std::cout << " " << power_ch.description << ": "
                                          << std::fixed << std::setprecision(4) << *watts_opt << "W";
                        }
                        else
                        {
                            detail::warn("Failed to read power from channel '{}'", power_ch.description);
                        }
                    }
                    else if (power_ch.type == Impl::PowerChannel::Type::Calculated && power_ch.iio_channel2)
                    {
                        const auto voltage_v_opt = read_channel_value(power_ch.iio_channel, power_ch.raw_attr_name);
                        const auto current_a_opt = read_channel_value(*power_ch.iio_channel2, power_ch.raw_attr_name);

                        if (voltage_v_opt && current_a_opt)
                        {
                            watts_opt = (*voltage_v_opt) * (*current_a_opt);
                            if (show_power)
                                std::cout << " " << power_ch.description << ": "
                                          << std::fixed << std::setprecision(4) << *watts_opt << "W";
                        }
                        else
                        {
                            detail::warn("Failed to read voltage/current from channel '{}'", power_ch.description);
                        }
                    }
                }
                catch (const iiopp::error& e)
                {
                    detail::warn("Error reading power channel '{}': {}", power_ch.description, e.what());
                }
            }
        }
#endif

        if (show_any)
            std::cout << std::endl;

        // Compute remaining time until the next update and sleep at the end of the loop
        const auto after_work = std::chrono::steady_clock::now();
        const auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(next_update - after_work);
        if (remaining.count() > 0)
            std::this_thread::sleep_for(remaining);
    }
}

}
}
}
