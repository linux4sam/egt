 @page input Input

This chapter discusses various input device support.

@section input_backends Input Support

EGT supports various different backends for gathering input device events.

Library          | Type
---------------- | -----------------------------------
libinput         | Keyboard, Mouse, Touchscreen
tslib            | Touchscreen
raw evdev        | Keyboard, Mouse, Touchscreen, Other
X11              | Keyboard, Mouse, Touchscreen


See @subpage environ for some environment variables that are useful for configuring
input devices.

@section input_mapping Keyboard Mapping

By default, EGT will use a built-in static keyboard mapping.  In most cases, this
should be sufficient.  However, in the event you wish to have a system level and
more flexible way of managing simple or complex keyboard mappings, EGT also
supports the use of [libxkbcommon](https://xkbcommon.org/) irrelevant of using
the X11 backend.

> xkbcommon is a library for handling of keyboard descriptions, including
> loading them from disk, parsing them and handling their state. It's mainly
> meant for client toolkits, window systems, and other system applications

What this means is libxkbcommon is not all that is needed.  You must also have
data files available that describe various components needed for doing proper
keyboard mappings, which are not included with xkbcommon.  In standard
distributions this is usually distributed in a package named xkeyboard-config or
xdata.  See the [xkbcommon documentation](https://xkbcommon.org/doc/current/)
for more information.

@section input_libinput libinput

[libinput](https://www.freedesktop.org/wiki/Software/libinput/) is the prime
interface to receive keyboard and touchscreen or pointer
events.  libinput does [not support gestures on these interfaces](https://wayland.freedesktop.org/libinput/doc/1.14.1/gestures.html#gestures-touchscreens).

While EGT does include ways to configure how it uses libinput, this should not
be used as an end-all means of configuring input devices with libinput. libinput
has a well documented and solid way of configuring input devices and even has
[tools for debugging](https://wayland.freedesktop.org/libinput/doc/1.14.1/tools.html#libinput-list-devices)
configuration of input devices.

Also, take special note of [udev configuration of devices](https://wayland.freedesktop.org/libinput/doc/latest/device-configuration-via-udev.html#)
that controls how they are introduced to libinput.
