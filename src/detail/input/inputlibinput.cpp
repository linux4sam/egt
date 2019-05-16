/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/app.h"
#include "egt/eventloop.h"
#include "egt/screen.h"
#include "egt/detail/input/inputlibinput.h"
#include "egt/detail/string.h"
#include <cassert>
#include <fstream>
#include <unistd.h>
#include <libinput.h>
#include <libudev.h>

using namespace std;

namespace egt
{
inline namespace v1
{
namespace detail
{

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


InputLibInput::InputLibInput()
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

void InputLibInput::handle_event_device_notify(struct libinput_event* ev)
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

    // if the device is handled by another backend, disable libinput events
    for (auto& device : main_app().get_input_devices())
    {
        vector<string> tokens;
        detail::tokenize(device.second, '/', tokens);

        if (tokens.back() == libinput_device_get_sysname(dev))
        {
            if (device.first != "libinput")
            {
                libinput_device_config_send_events_set_mode(dev,
                        LIBINPUT_CONFIG_SEND_EVENTS_DISABLED);
                return;
            }
        }
    }
}

void InputLibInput::handle_event_touch(struct libinput_event* ev)
{
    struct libinput_event_touch* t = libinput_event_get_touch_event(ev);
    int slot = libinput_event_touch_get_seat_slot(t);

    if (slot == -1)
        return;

    switch (libinput_event_get_type(ev))
    {
    case LIBINPUT_EVENT_TOUCH_UP:
    {
        Event event(eventid::raw_pointer_up, m_last_point);
        dispatch(event);
        break;
    }
    case LIBINPUT_EVENT_TOUCH_DOWN:
    {
        const auto& screen_size = main_screen()->size();
        double x = libinput_event_touch_get_x_transformed(t, screen_size.w);
        double y = libinput_event_touch_get_y_transformed(t, screen_size.h);

        Event event(eventid::raw_pointer_down);
        event.pointer().point = m_last_point = DisplayPoint(x, y);
        dispatch(event);
        break;
    }
    case LIBINPUT_EVENT_TOUCH_MOTION:
    {
        const auto& screen_size = main_screen()->size();
        double x = libinput_event_touch_get_x_transformed(t, screen_size.w);
        double y = libinput_event_touch_get_y_transformed(t, screen_size.h);

        Event event(eventid::raw_pointer_move);
        event.pointer().point = m_last_point = DisplayPoint(x, y);
        dispatch(event);
        break;
    }
    default:
        break;
    }
}

void InputLibInput::handle_event_keyboard(struct libinput_event* ev)
{
    struct libinput_event_keyboard* k = libinput_event_get_keyboard_event(ev);
    unsigned int key = libinput_event_keyboard_get_key(k);

    eventid v = eventid::none;
    if (libinput_event_keyboard_get_key_state(k) == LIBINPUT_KEY_STATE_RELEASED)
        v = eventid::keyboard_up;
    else if (libinput_event_keyboard_get_key_state(k) == LIBINPUT_KEY_STATE_PRESSED)
        v = eventid::keyboard_down;
    //else if (libinput_event_keyboard_get_key_state(k) == LIBINPUT_KEY_STATE_REPEAT)
    //  v = eventid::keyboard_repeat;

    if (v != eventid::none)
    {
        Event event(v);
        event.key().key = key;
        dispatch(event);
    }
}

void InputLibInput::handle_event_button(struct libinput_event* ev)
{
    struct libinput_event_pointer* p = libinput_event_get_pointer_event(ev);
    unsigned int button = libinput_event_pointer_get_button(p);

    Pointer::button b = Pointer::button::none;
    switch (button)
    {
    case BTN_LEFT:
        b = Pointer::button::left;
        break;
    case BTN_MIDDLE:
        b = Pointer::button::middle;
        break;
    case BTN_RIGHT:
        b = Pointer::button::right;
        break;
    default:
        /* Other buttons not handled. */
        /* For compatibility reasons, all additional buttons go after the old 4-7 scroll ones */
        return;
    }

    if (b != Pointer::button::none)
    {
        bool is_press = libinput_event_pointer_get_button_state(p) == LIBINPUT_BUTTON_STATE_PRESSED;
        Event event(is_press ? eventid::raw_pointer_down : eventid::raw_pointer_up,
                    m_last_point);
        event.pointer().btn = b;
        dispatch(event);
    }
}

void InputLibInput::handle_read(const asio::error_code& error)
{
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
        case LIBINPUT_EVENT_KEYBOARD_KEY:
            handle_event_keyboard(ev);
            break;
        case LIBINPUT_EVENT_POINTER_BUTTON:
            handle_event_button(ev);
            break;
        case LIBINPUT_EVENT_POINTER_AXIS:
        case LIBINPUT_EVENT_TOUCH_DOWN:
        case LIBINPUT_EVENT_TOUCH_MOTION:
        case LIBINPUT_EVENT_TOUCH_UP:
            handle_event_touch(ev);
            break;
        /* These events are not handled. */
        case LIBINPUT_EVENT_TOUCH_CANCEL:
        case LIBINPUT_EVENT_TOUCH_FRAME:
        case LIBINPUT_EVENT_POINTER_MOTION:
        case LIBINPUT_EVENT_POINTER_MOTION_ABSOLUTE:
        case LIBINPUT_EVENT_GESTURE_SWIPE_BEGIN:
        case LIBINPUT_EVENT_GESTURE_SWIPE_UPDATE:
        case LIBINPUT_EVENT_GESTURE_SWIPE_END:
        case LIBINPUT_EVENT_GESTURE_PINCH_BEGIN:
        case LIBINPUT_EVENT_GESTURE_PINCH_UPDATE:
        case LIBINPUT_EVENT_GESTURE_PINCH_END:
        case LIBINPUT_EVENT_TABLET_TOOL_AXIS:
        case LIBINPUT_EVENT_TABLET_TOOL_PROXIMITY:
        case LIBINPUT_EVENT_TABLET_TOOL_TIP:
        case LIBINPUT_EVENT_TABLET_TOOL_BUTTON:
        case LIBINPUT_EVENT_TABLET_PAD_BUTTON:
        case LIBINPUT_EVENT_TABLET_PAD_RING:
        case LIBINPUT_EVENT_TABLET_PAD_STRIP:
        default:
            break;
        }

        libinput_event_destroy(ev);
        libinput_dispatch(li);
    }

#ifdef USE_PRIORITY_QUEUE
    asio::async_read(m_input, asio::null_buffers(),
                     main_app().event().queue().wrap(detail::priorities::moderate, std::bind(&InputLibInput::handle_read, this, std::placeholders::_1)));
#else
    asio::async_read(m_input, asio::null_buffers(),
                     std::bind(&InputLibInput::handle_read, this, std::placeholders::_1));
#endif
}

InputLibInput::~InputLibInput()
{
    libinput_unref(li);
}

}
}
}
