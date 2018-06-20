# Minimal Toolkit and Graphics Framework

## Features

- Antialiased 2D vector graphics.
- Optimized for fast redrawing without hardware acceleration.
- X11, DRM/KMS, and fbdev backends.
- Support for libplanes and hardware LCD planes.
- Animations with a variety of default easing functions.
- A rich widget set designed for embedded touchscreens with everything from buttons, to sliders, to charts.
- Customizable widgets and color schemes.
- evdev, tslib, mouse, touchscreen, and keyboard input.
- Entire library and simple application in under 50K bytes.
- Modern C++11 design and simplicity.

## Todo

Priority 1
- Seperate logical and physical coordinates.
- Architecture document for drawing system, widget relationships, and event handling.
- Change all callbacks to std::function after performance analysis.
- Wrap 2D graphics primitives to abstract widget drawing (prepare for different drawing libraries).
- Investigate performance of cairo vs. libgd vs imlib2.
- Identify a hard path to support 2D GPU under graphics primitives used.
- Backbuffer all widgets and seperate damage() from redraw().
- Setup global theme + individual widget theme for fonts, colors, etc.  Define pallete.

Priority 2
- API documentation.
- Chart widgets (kplot is nice, but lacks bar charts): pie, bar, line, point (having working prototype of kplot).
- XML loader and tool to create screens (find existing tool).
- Do true double buffering on vsync for fbdev/drm backends.
- JPEG/SVG support.
- Message box widget.
- Popup widget.
- Radio widget.
- More grid options.
- Scroll wheel widget.

http://www.peteronion.org.uk/GtkExamples/GladeTutorials.html
