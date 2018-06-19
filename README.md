# Minimal Toolkit and Graphics Framework

## Features

- Antialiased 2D vector graphics.
- Optimized for fast redrawing without hardware acceleration.
- X11, DRM/KMS, and fbdev backends.
- Support for libplanes and hardware LCD planes.
- Animations with a variety of default easing functions.
- A rich widget set designed for embedded touchscreens with everything from buttons, to sliders, to charts.
- Customizable widgets and color schemes.
- C++11 implementation and API.
- evdev, tslib, mouse, touchscreen, and keyboard input.
- Entire library and simple application in under 50K bytes.
- Modern C++11 design and simplicity.

## Todo

Priority 1
- Architecture document for drawing system and widget relationships.
- Change all callbacks to std::function after performance analysis.
- Wrap 2D graphics primitives to abstract widget drawing.
- Investigate performance of cairo vs. libgd vs imlib2.
- Identify a hard path to support 2D GPU under graphics primitive used.
- Backbuffer all widgets and seperate damage() from redraw().
- Setup global theme + individual widget theme for fonts, colors, etc.

Priority 2
- Scroll wheel widget.
- Chart widgets (kplot is nice, but lacks bar charts): pie, bar, line, point.
- XML loader and tool to create screens (find existing tool).
- Do true double buffering on vsync for fbdev/drm backends.
