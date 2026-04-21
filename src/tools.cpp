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
#include <mutex>
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

    /// Temperature sensor entry
    struct TemperatureSensor
    {
        std::string sysfs_path;   // Path to sysfs temperature file
        std::string description;  // Human-readable description
        std::ifstream file;       // Persistent file handle

        // Move-only due to ifstream
        TemperatureSensor(const std::string& path, const std::string& desc)
            : sysfs_path(path), description(desc), file(path) {}
        TemperatureSensor(TemperatureSensor&&) = default;
        TemperatureSensor& operator=(TemperatureSensor&&) = default;
    };

    std::vector<TemperatureSensor> temperature_sensors;

    /// CPU frequency sysfs file (single-core assumption)
    std::ifstream cpu_freq_file;

    /// Accumulated metrics for averaging
    struct AccumulatedMetrics
    {
        // Pre-allocate with reserve to reduce reallocations
        static constexpr size_t INITIAL_CAPACITY = 600; // 10 minutes at 1 sample/sec

        std::vector<float> fps_samples;
        std::vector<double> cpu_samples;
        std::vector<std::vector<double>> power_samples;
        std::vector<std::vector<double>> temp_samples;
        std::vector<double> cpu_freq_samples;

        void clear()
        {
            fps_samples.clear();
            cpu_samples.clear();
            for (auto& v : power_samples)
                v.clear();
            for (auto& v : temp_samples)
                v.clear();
            cpu_freq_samples.clear();
        }

        void reserve_capacity()
        {
            fps_samples.reserve(INITIAL_CAPACITY);
            cpu_samples.reserve(INITIAL_CAPACITY);
            for (auto& v : power_samples)
                v.reserve(INITIAL_CAPACITY);
            for (auto& v : temp_samples)
                v.reserve(INITIAL_CAPACITY);
            cpu_freq_samples.reserve(INITIAL_CAPACITY);
        }
    };

    AccumulatedMetrics accumulated;
    std::atomic<bool> accumulating{false};
    std::mutex accumulated_mutex;
};

namespace
{

/// Result of computing an average over a set of samples.
struct AverageResult
{
    double value;   ///< Computed average value
    size_t count;   ///< Number of samples used in computation
};

/**
 * Compute average of samples with optional edge trimming.
 *
 * @tparam T Numeric type of samples
 * @param samples Vector of samples to average
 * @param policy Edge handling policy
 * @return AverageResult if successful, nullopt if insufficient samples
 */
template<typename T>
std::optional<AverageResult> compute_average(const std::vector<T>& samples,
                                             PerfMonitor::EdgePolicy policy)
{
    if (samples.empty())
        return std::nullopt;

    auto begin = samples.begin();
    auto end = samples.end();

    if (policy == PerfMonitor::EdgePolicy::DiscardEdges)
    {
        if (samples.size() < 3)
            return std::nullopt;
        ++begin;
        --end;
    }

    const size_t count = static_cast<size_t>(std::distance(begin, end));
    const double sum = std::accumulate(begin, end, 0.0);

    return AverageResult{sum / static_cast<double>(count), count};
}

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

bool PerfMonitor::show_temp_enabled()
{
    static const bool value = (getenv("EGT_SHOW_TEMP") != nullptr);
    return value;
}

bool PerfMonitor::show_cpu_freq_enabled()
{
    static const bool value = (getenv("EGT_SHOW_CPU_FREQ") != nullptr);
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

void PerfMonitor::enable_temperature_tracking(bool enable)
{
    m_track_temp = enable;
}

void PerfMonitor::enable_cpu_freq_tracking(bool enable)
{
    if (enable && m_impl && !m_impl->cpu_freq_file.is_open())
    {
        static constexpr const char* path =
            "/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq";
        m_impl->cpu_freq_file.open(path);
        if (!m_impl->cpu_freq_file.is_open())
        {
            detail::warn("CPU frequency file '{}' cannot be opened", path);
            return;
        }
    }
    m_track_cpu_freq = enable;
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

bool PerfMonitor::temperature_tracking_enabled() const
{
    return m_track_temp;
}

bool PerfMonitor::cpu_freq_tracking_enabled() const
{
    return m_track_cpu_freq;
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

bool PerfMonitor::add_temperature_sensor(const std::string& sysfs_path,
                                         const std::string& description)
{
    if (!m_impl)
        return false;

    m_impl->temperature_sensors.emplace_back(sysfs_path, description);

    if (!m_impl->temperature_sensors.back().file.is_open())
    {
        detail::warn("Temperature sensor file '{}' cannot be opened", sysfs_path);
        m_impl->temperature_sensors.pop_back();
        return false;
    }

    return true;
}

void PerfMonitor::start_accumulate()
{
    if (!m_impl)
        return;

    std::lock_guard<std::mutex> lock(m_impl->accumulated_mutex);
    m_impl->accumulated.clear();

#ifdef HAVE_LIBIIO
    m_impl->accumulated.power_samples.resize(m_impl->power_channels.size());
#endif

    m_impl->accumulated.temp_samples.resize(m_impl->temperature_sensors.size());

    m_impl->accumulated.reserve_capacity();

    m_impl->accumulating = true;
}

void PerfMonitor::stop_accumulate()
{
    if (m_impl)
        m_impl->accumulating = false;
}

bool PerfMonitor::accumulating() const
{
    return m_impl && m_impl->accumulating;
}

void PerfMonitor::log_averages(EdgePolicy policy)
{
    if (!m_impl)
        return;

    std::lock_guard<std::mutex> lock(m_impl->accumulated_mutex);

    std::cout << "=== Performance Averages ===" << std::endl;

    if (auto avg = compute_average(m_impl->accumulated.fps_samples, policy))
    {
        std::cout << "  FPS: " << std::fixed << std::setprecision(0) << avg->value
                  << " (avg over " << avg->count << " samples)" << std::endl;
    }

    if (auto avg = compute_average(m_impl->accumulated.cpu_samples, policy))
    {
        std::cout << "  CPU: " << std::fixed << std::setprecision(0) << avg->value << "%"
                  << " (avg over " << avg->count << " samples)" << std::endl;
    }

#ifdef HAVE_LIBIIO
    for (size_t i = 0; i < m_impl->power_channels.size() && i < m_impl->accumulated.power_samples.size(); ++i)
    {
        if (auto avg = compute_average(m_impl->accumulated.power_samples[i], policy))
        {
            std::cout << "  " << m_impl->power_channels[i].description << ": "
                      << std::fixed << std::setprecision(4) << avg->value << " W"
                      << " (avg over " << avg->count << " samples)" << std::endl;
        }
    }
#endif

    for (size_t i = 0; i < m_impl->temperature_sensors.size() && i < m_impl->accumulated.temp_samples.size(); ++i)
    {
        if (auto avg = compute_average(m_impl->accumulated.temp_samples[i], policy))
        {
            std::cout << "  " << m_impl->temperature_sensors[i].description << ": "
                      << std::fixed << std::setprecision(1) << avg->value << "°C"
                      << " (avg over " << avg->count << " samples)" << std::endl;
        }
    }

    if (auto avg = compute_average(m_impl->accumulated.cpu_freq_samples, policy))
    {
        std::cout << "  CPU Freq: " << std::fixed << std::setprecision(0) << avg->value << " MHz"
                  << " (avg over " << avg->count << " samples)" << std::endl;
    }

    m_impl->accumulated.clear();
}

void PerfMonitor::monitor_loop()
{
    struct IterationMetrics
    {
        std::optional<float> fps;
        std::optional<double> cpu;
        std::vector<std::optional<double>> power;
        std::vector<std::optional<double>> temp;
        std::optional<double> cpu_freq;
    };

    while (!m_stop_requested)
    {
        // Schedule next update based on the desired interval
        const auto next_update = std::chrono::steady_clock::now() + m_update_interval;

        const bool show_fps = show_fps_enabled();
        const bool show_cpu = show_cpu_enabled();
        const bool show_power = show_power_enabled();
        const bool show_temp = show_temp_enabled();
        const bool show_cpu_freq = show_cpu_freq_enabled();

        const bool show_any = show_fps || show_cpu || show_power || show_temp || show_cpu_freq;
        if (show_any)
            std::cout << "PerfMonitor:";

        // Collect metrics first to lock only once at the end.
        IterationMetrics metrics;

        if (fps_tracking_enabled())
        {
            const float fps = m_fps_monitor.fps();
            metrics.fps = fps;
            if (show_fps)
                std::cout << " FPS: " << std::fixed << std::setprecision(0) << std::round(fps);
        }

        if (cpu_tracking_enabled())
        {
            m_cpu_monitor.update();
            const double cpu = m_cpu_monitor.usage();
            metrics.cpu = cpu;
            if (show_cpu)
                std::cout << " CPU: " << std::fixed << std::setprecision(0) << cpu << "%";
        }

#ifdef HAVE_LIBIIO
        if (power_tracking_enabled() && m_impl && !m_impl->power_channels.empty())
        {
            metrics.power.resize(m_impl->power_channels.size());
            for (size_t i = 0; i < m_impl->power_channels.size(); ++i)
            {
                auto& power_ch = m_impl->power_channels[i];
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

                    metrics.power[i] = watts_opt;
                }
                catch (const iiopp::error& e)
                {
                    detail::warn("Error reading power channel '{}': {}", power_ch.description, e.what());
                }
            }
        }
#endif

        if (temperature_tracking_enabled() && m_impl && !m_impl->temperature_sensors.empty())
        {
            metrics.temp.resize(m_impl->temperature_sensors.size());
            for (size_t i = 0; i < m_impl->temperature_sensors.size(); ++i)
            {
                auto& sensor = m_impl->temperature_sensors[i];
                if (sensor.file.is_open())
                {
                    sensor.file.seekg(0);
                    long millidegrees = 0;
                    if (sensor.file >> millidegrees)
                    {
                        const double degrees = millidegrees / 1000.0;
                        metrics.temp[i] = degrees;
                        if (show_temp)
                            std::cout << " " << sensor.description << ": "
                                      << std::fixed << std::setprecision(1) << degrees << "°C";
                    }
                    else
                    {
                        detail::warn("Failed to read temperature from '{}'", sensor.sysfs_path);
                    }
                    sensor.file.clear(); // Clear EOF flag for next read
                }
                else
                {
                    detail::warn("Cannot open temperature sensor file '{}'", sensor.sysfs_path);
                }
            }
        }

        if (cpu_freq_tracking_enabled() && m_impl && m_impl->cpu_freq_file.is_open())
        {
            m_impl->cpu_freq_file.seekg(0);
            long khz = 0;
            if (m_impl->cpu_freq_file >> khz)
            {
                const double mhz = khz / 1000.0;
                metrics.cpu_freq = mhz;
                if (show_cpu_freq)
                    std::cout << " CPU Freq: " << std::fixed << std::setprecision(0)
                              << mhz << " MHz";
            }
            else
            {
                detail::warn("Failed to read CPU frequency");
            }
            m_impl->cpu_freq_file.clear(); // Clear EOF flag for next read
        }

        if (show_any)
            std::cout << std::endl;

        if (m_impl && m_impl->accumulating)
        {
            std::lock_guard<std::mutex> lock(m_impl->accumulated_mutex);

            if (metrics.fps)
                m_impl->accumulated.fps_samples.push_back(*metrics.fps);

            if (metrics.cpu)
                m_impl->accumulated.cpu_samples.push_back(*metrics.cpu);

            for (size_t i = 0; i < metrics.power.size() && i < m_impl->accumulated.power_samples.size(); ++i)
            {
                if (metrics.power[i])
                    m_impl->accumulated.power_samples[i].push_back(*metrics.power[i]);
            }

            for (size_t i = 0; i < metrics.temp.size() && i < m_impl->accumulated.temp_samples.size(); ++i)
            {
                if (metrics.temp[i])
                    m_impl->accumulated.temp_samples[i].push_back(*metrics.temp[i]);
            }

            if (metrics.cpu_freq)
                m_impl->accumulated.cpu_freq_samples.push_back(*metrics.cpu_freq);
        }

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
