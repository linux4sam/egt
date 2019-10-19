/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_PROGRESSBAR_H
#define EGT_PROGRESSBAR_H

#include <egt/detail/meta.h>
#include <egt/valuewidget.h>
#include <memory>

namespace egt
{
inline namespace v1
{

class Frame;

/**
 * Displays a progress bar based on a value.
 *
 * @ingroup controls
 */
class EGT_API ProgressBar : public ValueRangeWidget<int>
{
public:

    /**
     * @param[in] rect Rectangle for the widget.
     * @param[in] min Minimum value for the range.
     * @param[in] max Maximum value in the range.
     * @param[in] value Current value in the range.
     */
    explicit ProgressBar(const Rect& rect = {},
                         int min = 0, int max = 100, int value = 0) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Rectangle for the widget.
     * @param[in] min Minimum value for the range.
     * @param[in] max Maximum value in the range.
     * @param[in] value Current value in the range.
     */
    explicit ProgressBar(Frame& parent, const Rect& rect = {},
                         int min = 0, int max = 100, int value = 0) noexcept;

    virtual void draw(Painter& painter, const Rect& rect) override;

    /**
     * Default draw method for the ProgressBar.
     */
    static void default_draw(ProgressBar& widget, Painter& painter, const Rect& rect);

    virtual Size min_size_hint() const override;

    /**
     * Enable/disable showing the label text.
     *
     * @param[in] value When true, the label text is shown.
     */
    virtual void set_show_label(bool value);

    /**
     * Get the show label state.
     */
    inline bool show_label() const { return m_show_label; }

    virtual ~ProgressBar() = default;

protected:
    /**
     * When true, the label text is shown.
     */
    bool m_show_label{true};
};

/**
 * Displays a spinning progress meter.
 *
 * @ingroup controls
 */
class EGT_API SpinProgress : public ValueRangeWidget<int>
{
public:

    /**
     * @param[in] rect Rectangle for the widget.
     * @param[in] min Minimum value for the range.
     * @param[in] max Maximum value in the range.
     * @param[in] value Current value in the range.
     */
    explicit SpinProgress(const Rect& rect = {},
                          int min = 0, int max = 100, int value = 0) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Rectangle for the widget.
     * @param[in] min Minimum value for the range.
     * @param[in] max Maximum value in the range.
     * @param[in] value Current value in the range.
     */
    explicit SpinProgress(Frame& parent, const Rect& rect = {},
                          int min = 0, int max = 100, int value = 0) noexcept;


    virtual void draw(Painter& painter, const Rect& rect) override;

    /**
     * Default draw method for the SpinProgress.
     */
    static void default_draw(SpinProgress& widget, Painter& painter, const Rect& rect);

    virtual Size min_size_hint() const override;

    /**
    * Enable/disable showing the label text.
    *
    * @param[in] value When true, the label text is shown.
    */
    virtual void set_show_label(bool value);

    /**
     * Get the show label state.
     */
    inline bool show_label() const { return m_show_label; }

    virtual ~SpinProgress() = default;
protected:
    /**
     * When true, the label text is shown.
     */
    bool m_show_label{true};
};

/**
 * Displays a level meter based on a value.
 *
 * @image html widget_levelmeter.png
 * @image latex widget_levelmeter.png "widget_levelmeter" width=5cm
 *
 * @ingroup controls
 */
class EGT_API LevelMeter : public ValueRangeWidget<int>
{
public:

    /**
     * @param[in] rect Rectangle for the widget.
     * @param[in] min Minimum value for the range.
     * @param[in] max Maximum value in the range.
     * @param[in] value Current value in the range.
     */
    explicit LevelMeter(const Rect& rect = {},
                        int min = 0, int max = 100, int value = 0) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Rectangle for the widget.
     * @param[in] min Minimum value for the range.
     * @param[in] max Maximum value in the range.
     * @param[in] value Current value in the range.
     */
    explicit LevelMeter(Frame& parent, const Rect& rect = {},
                        int min = 0, int max = 100, int value = 0) noexcept;

    virtual void draw(Painter& painter, const Rect& rect) override;

    /**
     * Default draw method for the LevelMeter.
     */
    static void default_draw(LevelMeter& widget, Painter& painter, const Rect& rect);

    /**
     * Set the number of bars to show.
     */
    virtual void set_num_bars(size_t bars)
    {
        m_num_bars = bars;
    }

    /**
     * Get the number of bars to show.
     */
    inline size_t num_bars() const { return m_num_bars; }

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
class EGT_API AnalogMeter : public ValueRangeWidget<float>
{
public:

    /**
     * @param[in] rect Rectangle for the widget.
     */
    explicit AnalogMeter(const Rect& rect = {}) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Rectangle for the widget.
     */
    explicit AnalogMeter(Frame& parent, const Rect& rect = {}) noexcept;

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
