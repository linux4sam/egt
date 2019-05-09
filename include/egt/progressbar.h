/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_PROGRESSBAR_H
#define EGT_PROGRESSBAR_H

#include <egt/valuewidget.h>
#include <memory>

namespace egt
{
inline namespace v1
{

/**
 * Displays a progress bar based on a value.
 *
 * @ingroup controls
 */
class ProgressBar : public ValueRangeWidget<int>
{
public:

    /**
     * @param[in] rect Rectangle for the widget.
     * @param[in] min Minimum value for the range.
     * @param[in] max Maximum value in the range.
     * @param[in] value Current value in the range.
     */
    explicit ProgressBar(const Rect& rect = Rect(),
                         int min = 0, int max = 100, int value = 0) noexcept;

    /// @todo Constructors

    virtual void draw(Painter& painter, const Rect& rect) override;

    /**
     * Default draw method for the ProgressBar.
     */
    static void default_draw(ProgressBar& widget, Painter& painter, const Rect& rect);

    virtual Size min_size_hint() const override;

    virtual ~ProgressBar() = default;
};

/**
 * Displays a spinning progress meter.
 *
 * @ingroup controls
 */
class SpinProgress : public ValueRangeWidget<int>
{
public:

    /**
     * @param[in] rect Rectangle for the widget.
     * @param[in] min Minimum value for the range.
     * @param[in] max Maximum value in the range.
     * @param[in] value Current value in the range.
     */
    explicit SpinProgress(const Rect& rect = Rect(),
                          int min = 0, int max = 100, int value = 0) noexcept;

    /// @todo Constructors

    virtual void draw(Painter& painter, const Rect& rect) override;

    /**
     * Default draw method for the SpinProgress.
     */
    static void default_draw(SpinProgress& widget, Painter& painter, const Rect& rect);

    virtual Size min_size_hint() const override;

    virtual ~SpinProgress() = default;
};

/**
 * Displays a level meter based on a value.
 *
 * @image html widget_levelmeter.png
 * @image latex widget_levelmeter.png "widget_levelmeter" width=5cm
 *
 * @ingroup controls
 */
class LevelMeter : public ValueRangeWidget<int>
{
public:

    /**
     * @param[in] rect Rectangle for the widget.
     * @param[in] min Minimum value for the range.
     * @param[in] max Maximum value in the range.
     * @param[in] value Current value in the range.
     */
    explicit LevelMeter(const Rect& rect = Rect(),
                        int min = 0, int max = 100, int value = 0) noexcept;

    /// @todo Constructors

    virtual void draw(Painter& painter, const Rect& rect) override;

    /**
     * Default draw method for the LevelMeter.
     */
    static void default_draw(LevelMeter& widget, Painter& painter, const Rect& rect);

    virtual void set_num_bars(size_t bars)
    {
        m_num_bars = bars;
    }

    virtual size_t num_bars() const
    {
        return m_num_bars;
    }

    virtual Size min_size_hint() const override;

    virtual ~LevelMeter() = default;

protected:

    /**
     * The number of bars to display.
     */
    size_t m_num_bars{10};
};

/**
 * Displays an analog meter based on a percentage value.
 *
 * @image html widget_analogmeter.png
 * @image latex widget_analogmeter.png "widget_analogmeter" width=5cm
 *
 * @ingroup controls
 */
class AnalogMeter : public ValueRangeWidget<float>
{
public:

    /**
     * @param[in] rect Rectangle for the widget.
     */
    explicit AnalogMeter(const Rect& rect = Rect());

    /// @todo Constructors

    virtual void draw(Painter& painter, const Rect& rect) override;

    /**
     * Default draw method for the AnalogMeter.
     */
    static void default_draw(AnalogMeter& widget, Painter& painter, const Rect& rect);

    virtual Size min_size_hint() const override;

    virtual ~AnalogMeter() = default;
};


}
}

#endif
