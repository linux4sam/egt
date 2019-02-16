/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/app.h"
#include "egt/eventloop.h"
#include "egt/libinput.h"
#include <cassert>
#include <fstream>
#include <unistd.h>

#ifdef HAVE_LIBINPUT
#include <libinput.h>
#include <libudev.h>
#endif

using namespace std;

#ifdef HAVE_LIBINPUT

namespace egt
{

#if 0
// try to convert pointercal (generated with ts_calibrate) to libinput matrix
// this is broken - doesn't work
static void tslib_pointercal_to_libinput(struct libinput_device* dev)
{
    //ENV{WL_CALIBRATION}="a/g b/g c/g d/g e/g f/g"

    float matrix[6];
    //libinput_device_config_calibration_get_matrix(dev, matrix);
    float a, b, c, d, e, f, g, h, i;

    std::fstream file("/etc/pointercal", std::ios_base::in);
    file >> a >> b >> c >> d >> e >> f >> g >> h >> i;

    cout << "values: " << a << " " <<  b << " " << c << " " << d << " " <<
         e << " " << f << " " << g << " " << h << " " << i << endl;

    matrix[0] = a / g;
    matrix[1] = b / g;
    matrix[2] = c / g;
    matrix[3] = d / g;
    matrix[4] = e / g;
    matrix[5] = f / g;

    cout << matrix[0] << " " <<  matrix[1] << " " << matrix[2] << endl;
    cout << matrix[3] << " " <<  matrix[4] << " " << matrix[5] << endl;

    //matrix[2] /= 800.;
    //matrix[5] /= 480.;

    auto status = libinput_device_config_calibration_set_matrix(dev, matrix);
    if (status != LIBINPUT_CONFIG_STATUS_SUCCESS)
        cout << "Failed to apply calibration.\n";
}
#endif

static int
open_restricted(const char* path, int flags, void* user_data)
{
    ignoreparam(user_data);

    int fd = open(path, flags);
    return fd < 0 ? -errno : fd;
}

static void
close_restricted(int fd, void* user_data)
{
    ignoreparam(user_data);
    close(fd);
}

static const struct libinput_interface interface =
{
    open_restricted,
    close_restricted,
};

static struct libinput* tools_open_udev(const char* seat, bool verbose, bool grab)
{
    ignoreparam(verbose);
    struct libinput* li;
    struct udev* udev = udev_new();

    if (!udev)
    {
        fprintf(stderr, "Failed to initialize udev\n");
        return NULL;
    }

    li = libinput_udev_create_context(&interface, &grab, udev);
    if (!li)
    {
        fprintf(stderr, "Failed to initialize context from udev\n");
        goto out;
    }

    //if (verbose) {
    //  libinput_log_set_handler(li, log_handler);
    //  libinput_log_set_priority(li, LIBINPUT_LOG_PRIORITY_DEBUG);
    //}

    if (libinput_udev_assign_seat(li, seat))
    {
        fprintf(stderr, "Failed to set seat\n");
        libinput_unref(li);
        li = NULL;
        goto out;
    }

out:
    udev_unref(udev);
    return li;
}


LibInput::LibInput()
    : m_input(main_app().event().io())
{
    const char* seat_or_device = "seat0";
    bool verbose = false;
    li = tools_open_udev(seat_or_device, verbose, NULL);
    assert(li);

    m_input.assign(libinput_get_fd(li));

    // go ahead and enumerate devices and start the first async_read
    handle_read(asio::error_code());
}

void LibInput::handle_event_device_notify(struct libinput_event* ev)
{
    struct libinput_device* dev = libinput_event_get_device(ev);
    const char* type;

    if (libinput_event_get_type(ev) == LIBINPUT_EVENT_DEVICE_ADDED)
        type = "added";
    else
        type = "removed";

    INFO(type << " " << libinput_device_get_sysname(dev) << " " <<
         libinput_device_get_name(dev));

    li = libinput_event_get_context(ev);

    //tools_device_apply_config(libinput_event_get_device(ev),
    //			  &w->options);

    if (libinput_event_get_type(ev) == LIBINPUT_EVENT_DEVICE_ADDED)
    {
        // ignore calibratable devices (touchscreens)
        int ret = libinput_device_config_calibration_has_matrix(dev);
        if (ret)
        {
            // disable device, can't calibrate
            libinput_device_config_send_events_set_mode(dev, LIBINPUT_CONFIG_SEND_EVENTS_DISABLED);

            //tslib_pointercal_to_libinput(dev);
        }
    }
}

void LibInput::handle_event_motion(struct libinput_event* ev)
{
    struct libinput_event_pointer* p = libinput_event_get_pointer_event(ev);
    //double dx = libinput_event_pointer_get_dx(p),
    //    dy = libinput_event_pointer_get_dy(p);
    //struct point point;
    //const int mask = ARRAY_LENGTH(w->deltas);
    //size_t idx;

    //w->x += dx;
    //w->y += dy;
    //w->x = clip(w->x, 0.0, w->width);
    //w->y = clip(w->y, 0.0, w->height);

    //idx = w->ndeltas % mask;
    //point = w->deltas[idx];
    //idx = (w->ndeltas + 1) % mask;
    //point.x += libinput_event_pointer_get_dx_unaccelerated(p);
    //point.y += libinput_event_pointer_get_dy_unaccelerated(p);
    //w->deltas[idx] = point;
    //w->ndeltas++;
    //#endif

    //cout << x << "," << y << endl;
    //event_mouse() = Point(x, y);

    event_mouse().x += libinput_event_pointer_get_dx_unaccelerated(p);
    event_mouse().y += libinput_event_pointer_get_dy_unaccelerated(p);

    dispatch(eventid::RAW_POINTER_MOVE);
}

void LibInput::handle_event_absmotion(struct libinput_event* ev)
{
    ignoreparam(ev);

#if 0
    struct libinput_event_pointer* p = libinput_event_get_pointer_event(ev);
    double x = libinput_event_pointer_get_absolute_x_transformed(p, w->width),
           y = libinput_event_pointer_get_absolute_y_transformed(p, w->height);

    w->absx = x;
    w->absy = y;
#endif
}

bool LibInput::handle_event_touch(struct libinput_event* ev)
{
    bool res = false;

    struct libinput_event_touch* t = libinput_event_get_touch_event(ev);
    int slot = libinput_event_touch_get_seat_slot(t);
    //struct libinput_device* dev = libinput_event_get_device(ev);

    if (slot == -1)
        return res;

    switch (libinput_event_get_type(ev))
    {
    case LIBINPUT_EVENT_TOUCH_UP:
        dispatch(eventid::RAW_POINTER_UP);
        break;
    case LIBINPUT_EVENT_TOUCH_DOWN:
    {
        double x = libinput_event_touch_get_x_transformed(t, 800);
        double y = libinput_event_touch_get_y_transformed(t, 480);

        event_mouse() = Point(x, y);

        dispatch(eventid::RAW_POINTER_DOWN);
        break;
    }
    case LIBINPUT_EVENT_TOUCH_MOTION:
    {
        double x = libinput_event_touch_get_x_transformed(t, 800);
        double y = libinput_event_touch_get_y_transformed(t, 480);

        //double x2 = libinput_event_touch_get_x(t);
        //double y2 = libinput_event_touch_get_y(t);

        event_mouse() = Point(x, y);
        res = true;
        break;
    }
    default:
        break;
    }

    return res;
}

void LibInput::handle_event_axis(struct libinput_event* ev)
{
    ignoreparam(ev);

#if 0
    struct libinput_event_pointer* p = libinput_event_get_pointer_event(ev);
    double value;

    if (libinput_event_pointer_has_axis(p,
                                        LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL))
    {
        value = libinput_event_pointer_get_axis_value(p,
                LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL);
        w->vy += value;
        w->vy = clip(w->vy, 0, w->height);
    }

    if (libinput_event_pointer_has_axis(p,
                                        LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL))
    {
        value = libinput_event_pointer_get_axis_value(p,
                LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL);
        w->hx += value;
        w->hx = clip(w->hx, 0, w->width);
    }
#endif
}

void LibInput::handle_event_keyboard(struct libinput_event* ev)
{
    struct libinput_event_keyboard* k = libinput_event_get_keyboard_event(ev);
    unsigned int key = libinput_event_keyboard_get_key(k);

    eventid v = eventid::NONE;
    if (libinput_event_keyboard_get_key_state(k) == LIBINPUT_KEY_STATE_RELEASED)
        v = eventid::KEYBOARD_UP;
    else if (libinput_event_keyboard_get_key_state(k) == LIBINPUT_KEY_STATE_PRESSED)
        v = eventid::KEYBOARD_DOWN;
    //else if (libinput_event_keyboard_get_key_state(k) == LIBINPUT_KEY_STATE_REPEAT)
    //  v = eventid::KEYBOARD_REPEAT;

    if (v != eventid::NONE)
    {
        event_key() = key;
        dispatch(v);
    }
}

void LibInput::handle_event_button(struct libinput_event* ev)
{
    struct libinput_event_pointer* p = libinput_event_get_pointer_event(ev);
    unsigned int button = libinput_event_pointer_get_button(p);
    int is_press;

    is_press = libinput_event_pointer_get_button_state(p) == LIBINPUT_BUTTON_STATE_PRESSED;

    dispatch(is_press ? eventid::RAW_POINTER_DOWN : eventid::RAW_POINTER_UP);

    event_button() = button;
}

void LibInput::handle_event_swipe(struct libinput_event* ev)
{
    ignoreparam(ev);

#if 0
    struct libinput_event_gesture* g = libinput_event_get_gesture_event(ev);
    int nfingers;
    double dx, dy;

    nfingers = libinput_event_gesture_get_finger_count(g);

    switch (libinput_event_get_type(ev))
    {
    case LIBINPUT_EVENT_GESTURE_SWIPE_BEGIN:
        w->swipe.nfingers = nfingers;
        w->swipe.x = w->width / 2;
        w->swipe.y = w->height / 2;
        break;
    case LIBINPUT_EVENT_GESTURE_SWIPE_UPDATE:
        dx = libinput_event_gesture_get_dx(g);
        dy = libinput_event_gesture_get_dy(g);
        w->swipe.x += dx;
        w->swipe.y += dy;
        break;
    case LIBINPUT_EVENT_GESTURE_SWIPE_END:
        w->swipe.nfingers = 0;
        w->swipe.x = w->width / 2;
        w->swipe.y = w->height / 2;
        break;
    default:
        abort();
    }
#endif
}

void LibInput::handle_event_pinch(struct libinput_event* ev)
{
    ignoreparam(ev);

#if 0
    struct libinput_event_gesture* g = libinput_event_get_gesture_event(ev);
    int nfingers;
    double dx, dy;

    nfingers = libinput_event_gesture_get_finger_count(g);

    switch (libinput_event_get_type(ev))
    {
    case LIBINPUT_EVENT_GESTURE_PINCH_BEGIN:
        w->pinch.nfingers = nfingers;
        w->pinch.x = w->width / 2;
        w->pinch.y = w->height / 2;
        break;
    case LIBINPUT_EVENT_GESTURE_PINCH_UPDATE:
        dx = libinput_event_gesture_get_dx(g);
        dy = libinput_event_gesture_get_dy(g);
        w->pinch.x += dx;
        w->pinch.y += dy;
        w->pinch.scale = libinput_event_gesture_get_scale(g);
        w->pinch.angle += libinput_event_gesture_get_angle_delta(g);
        break;
    case LIBINPUT_EVENT_GESTURE_PINCH_END:
        w->pinch.nfingers = 0;
        w->pinch.x = w->width / 2;
        w->pinch.y = w->height / 2;
        w->pinch.angle = 0.0;
        w->pinch.scale = 1.0;
        break;
    default:
        abort();
    }
#endif
}

void LibInput::handle_event_tablet(struct libinput_event* ev)
{
    ignoreparam(ev);

#if 0
    struct libinput_event_tablet_tool* t = libinput_event_get_tablet_tool_event(ev);
    double x, y;
    struct point point;
    int idx;
    const int mask = ARRAY_LENGTH(w->tool.deltas);

    x = libinput_event_tablet_tool_get_x_transformed(t, w->width);
    y = libinput_event_tablet_tool_get_y_transformed(t, w->height);

    switch (libinput_event_get_type(ev))
    {
    case LIBINPUT_EVENT_TABLET_TOOL_PROXIMITY:
        if (libinput_event_tablet_tool_get_proximity_state(t) ==
            LIBINPUT_TABLET_TOOL_PROXIMITY_STATE_OUT)
        {
            w->tool.x_in = 0;
            w->tool.y_in = 0;
            w->tool.x_down = 0;
            w->tool.y_down = 0;
            w->tool.x_up = 0;
            w->tool.y_up = 0;
        }
        else
        {
            w->tool.x_in = x;
            w->tool.y_in = y;
            w->tool.ndeltas = 0;
            w->tool.deltas[0].x = w->width / 2;
            w->tool.deltas[0].y = w->height / 2;
        }
        break;
    case LIBINPUT_EVENT_TABLET_TOOL_TIP:
        w->tool.pressure = libinput_event_tablet_tool_get_pressure(t);
        w->tool.distance = libinput_event_tablet_tool_get_distance(t);
        w->tool.tilt_x = libinput_event_tablet_tool_get_tilt_x(t);
        w->tool.tilt_y = libinput_event_tablet_tool_get_tilt_y(t);
        if (libinput_event_tablet_tool_get_tip_state(t) ==
            LIBINPUT_TABLET_TOOL_TIP_DOWN)
        {
            w->tool.x_down = x;
            w->tool.y_down = y;
        }
        else
        {
            w->tool.x_up = x;
            w->tool.y_up = y;
        }
    /* fallthrough */
    case LIBINPUT_EVENT_TABLET_TOOL_AXIS:
        w->tool.x = x;
        w->tool.y = y;
        w->tool.pressure = libinput_event_tablet_tool_get_pressure(t);
        w->tool.distance = libinput_event_tablet_tool_get_distance(t);
        w->tool.tilt_x = libinput_event_tablet_tool_get_tilt_x(t);
        w->tool.tilt_y = libinput_event_tablet_tool_get_tilt_y(t);

        /* Add the delta to the last position and store them as abs
         * coordinates */
        idx = w->tool.ndeltas % mask;
        point = w->tool.deltas[idx];

        idx = (w->tool.ndeltas + 1) % mask;
        point.x += libinput_event_tablet_tool_get_dx(t);
        point.y += libinput_event_tablet_tool_get_dy(t);
        w->tool.deltas[idx] = point;
        w->tool.ndeltas++;
        break;
    case LIBINPUT_EVENT_TABLET_TOOL_BUTTON:
        break;
    default:
        abort();
    }

#endif
}

void LibInput::handle_read(const asio::error_code& error)
{
    bool res = false;
    if (error)
    {
        ERR(error);
        return;
    }

    struct libinput_event* ev;

    libinput_dispatch(li);

    while ((ev = libinput_get_event(li)))
    {
        switch (libinput_event_get_type(ev))
        {
        case LIBINPUT_EVENT_NONE:
            break;
        case LIBINPUT_EVENT_DEVICE_ADDED:
        case LIBINPUT_EVENT_DEVICE_REMOVED:
            handle_event_device_notify(ev);
            break;
        case LIBINPUT_EVENT_POINTER_MOTION:
            handle_event_motion(ev);
            break;
        case LIBINPUT_EVENT_POINTER_MOTION_ABSOLUTE:
            handle_event_absmotion(ev);
            break;
        case LIBINPUT_EVENT_TOUCH_DOWN:
        case LIBINPUT_EVENT_TOUCH_MOTION:
        case LIBINPUT_EVENT_TOUCH_UP:
            res = handle_event_touch(ev);
            break;
        case LIBINPUT_EVENT_POINTER_AXIS:
            handle_event_axis(ev);
            break;
        case LIBINPUT_EVENT_TOUCH_CANCEL:
        case LIBINPUT_EVENT_TOUCH_FRAME:
            break;
        case LIBINPUT_EVENT_POINTER_BUTTON:
            handle_event_button(ev);
            break;
        case LIBINPUT_EVENT_KEYBOARD_KEY:
            handle_event_keyboard(ev);
            break;
        case LIBINPUT_EVENT_GESTURE_SWIPE_BEGIN:
        case LIBINPUT_EVENT_GESTURE_SWIPE_UPDATE:
        case LIBINPUT_EVENT_GESTURE_SWIPE_END:
            handle_event_swipe(ev);
            break;
        case LIBINPUT_EVENT_GESTURE_PINCH_BEGIN:
        case LIBINPUT_EVENT_GESTURE_PINCH_UPDATE:
        case LIBINPUT_EVENT_GESTURE_PINCH_END:
            handle_event_pinch(ev);
            break;
        case LIBINPUT_EVENT_TABLET_TOOL_AXIS:
        case LIBINPUT_EVENT_TABLET_TOOL_PROXIMITY:
        case LIBINPUT_EVENT_TABLET_TOOL_TIP:
        case LIBINPUT_EVENT_TABLET_TOOL_BUTTON:
            handle_event_tablet(ev);
            break;
        case LIBINPUT_EVENT_TABLET_PAD_BUTTON:
        case LIBINPUT_EVENT_TABLET_PAD_RING:
        case LIBINPUT_EVENT_TABLET_PAD_STRIP:
            break;
        default:
            break;
        }

        libinput_event_destroy(ev);
        libinput_dispatch(li);
    }

    if (res)
        dispatch(eventid::RAW_POINTER_MOVE);

    asio::async_read(m_input, asio::null_buffers(),
                     main_app().event().queue().wrap(detail::priorities::moderate, std::bind(&LibInput::handle_read, this, std::placeholders::_1)));
}

LibInput::~LibInput()
{
    libinput_unref(li);
}
}

#endif
