![Microchip](docs/src/microchip_logo.png)

# Ensemble Graphics Toolkit

The Ensemble Graphics Toolkit (EGT) is a C++ GUI toolkit for Linux on Microchip
AT91/SAMA5 parts.  EGT provides modern GUI functionality, look-and-feel, and
performance.

Want to help out or make some changes?  See [Contributing](CONTRIBUTING.md).

## Features

- Antialiased 2D vector graphics.
- Modern C++11 design and simplicity.
- Optimized for fast redrawing without hardware acceleration.
- X11, DRM/KMS, and fbdev backends.
- Support for libplanes and hardware LCD planes.
- Animations with a variety of default easing functions.
- A rich widget set designed for embedded touchscreens with everything from
  buttons, to video players, to charts.
- Customizable widgets, colors, and themes.
- evdev, tslib, mouse, touchscreen, and keyboard input.

## Dependencies

*Recommended:*
- libplanes >= 1.0.0
- libdrm >= 2.4.0
- cJSON >= 1.6.0
- lua >= 5.3.1
- cairo >= 1.14.6
- tslib >= 1.15
- gstreamer >= 1.12
- libjpeg
- rapidxml
- libinput >= 1.8.2
- libmagic

*Optional:*
- x11
- lua
- libjpeg
- libcurl
- librsvg

## License

EGT is released under the terms of the `Apache 2` license. See the [COPYING](COPYING)
file for more information.
